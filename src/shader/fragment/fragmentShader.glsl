#version 330 core

smooth in vec3 Position;   // View-Space: aus dem VS
smooth in vec3 Normal;     // View-Space: aus dem VS
out vec4 FragColor;

// Matreial uniform Struct 
uniform struct Material {
    vec4 emission; 
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
} material;

// Environment Mapping additions
uniform samplerCube uEnvMap;
uniform int uUseEnvMap;        // 1 = use env mapping (glass), 0 = off
uniform float uIOR;            // index of refraction (1.5 for glass)
uniform float uEnvStrength;    // 0..1 mix factor with lighting
uniform mat3 uViewRot;         // obere linke 3x3 der View-Matrix; reine Rotation


vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


//Richtungslicht Struct
uniform struct lightSourceR{
    int enabled; // 0 = aus, 1 = an
    vec3 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
} richtungslicht;

// Punktlicht Struct
uniform struct lightSourceP{
    int enabled; // 0 = aus, 1 = an
    vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float linear;
    float quadratic;
} punktlicht;

// Spotlicht Struct
uniform struct lightSourceS{
    int enabled; // 0 = aus, 1 = an
    vec3 position;
    vec3 direction;

    float innerCone; // als Radians
    float outerCone;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float linear; 
    float quadratic;  
}spotlicht;

//Nebel
uniform struct fog{
    vec3  color;
    float density;
    int   enabled;
}nebel;

float computeFogFactorExp2(float d) {
    float x = nebel.density * d;
    float f = exp(-x * x);
    return clamp(f, 0.0, 1.0);
}

void main(void)
{
    vec3 light;
    vec3 reflection;
    float NdotL;
    float specAmt;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;


    vec3 normal = normalize(Normal);
    vec3 view = normalize(-Position);
    vec3 col   = material.emission.rgb;
    float alpha = material.diffuse.a;

    // Richtungslicht
    if(richtungslicht.enabled==1){
        light = normalize(-richtungslicht.direction);
        ambient = (richtungslicht.ambient.rgb) * (material.ambient.rgb);

        NdotL = max(dot(normal, light), 0.0);
        float specAmt = 0.0;
        if (NdotL > 0.0) {
          reflection = reflect(-light, normal);
          float RV = max(dot(reflection, view), 1e-6);
          specAmt = pow(RV, material.shininess);
        }

        diffuse  = (richtungslicht.diffuse.rgb) * NdotL * (material.diffuse.rgb);
        specular = (material.specular.rgb * specAmt * richtungslicht.specular.rgb);

        col += ambient + diffuse + specular;
    }
    // Punktlicht
    if(punktlicht.enabled==1){
        vec3 Lvec = punktlicht.position.xyz - Position;
        float dist = length(Lvec);
        light = (Lvec / max(dist, 1e-6));
        float attenuation = 1.0 / ((1.0)+ punktlicht.linear * dist + punktlicht.quadratic * (dist * dist));

        NdotL = max(dot(normal, light), 0.0);
        specAmt = 0.0;
        if (NdotL > 0.0) {
          reflection = reflect(-light, normal);
          float RV = max(dot(reflection, view), 1e-6);
          specAmt = pow(RV, material.shininess);
        }

        ambient = attenuation * (punktlicht.ambient.rgb) * (material.ambient.rgb);        
        diffuse = attenuation * (punktlicht.diffuse.rgb) * NdotL * (material.diffuse.rgb);
        specular = attenuation * (material.specular.rgb * specAmt * punktlicht.specular.rgb);

        col += ambient + diffuse + specular;
    }
    // Spotlicht
    if(spotlicht.enabled==1){
        light       = normalize(spotlicht.position - Position);
        reflection  = reflect(-light, normal);

        float theta    = dot(-light, normalize(spotlicht.direction));
        float epsilon  = max(spotlicht.innerCone - spotlicht.outerCone, 1e-6);
        float intensity = clamp((theta - spotlicht.outerCone) / epsilon, 0.0, 1.0);

        // Attenuation (NEU)
        float dist        = length(spotlicht.position - Position);
        float attenuation = 1.0 / (1.0 + spotlicht.linear * dist + spotlicht.quadratic * dist * dist);
        intensity *= attenuation;

        ambient  = vec3(0.0);
        diffuse  = vec3(0.0);
        specular = vec3(0.0);
        if (theta > spotlicht.outerCone) {
            ambient  = spotlicht.ambient.rgb * material.ambient.rgb;

            diffuse  = intensity * spotlicht.diffuse.rgb
                                * max(dot(normal, light), 0.0)
                                * material.diffuse.rgb;

            specular = intensity * spotlicht.specular.rgb
                                * pow(max(dot(reflection, view), 0.0), material.shininess)
                                * material.specular.rgb;
        } else {
            ambient = spotlicht.ambient.rgb * material.ambient.rgb;
        }
        col += ambient + diffuse + specular;
    }

    // === Environment mapping (reflection + refraction) ===
    if (uUseEnvMap == 1) {
        // View-Space Eingaben
        vec3 N = normalize(Normal);
        vec3 V = normalize(-Position);     // von Fragment zur Kamera (0,0,0)

        // Richtungen im VIEW-Space berechnen
        float eta   = 1.0 / max(uIOR, 1e-3);
        vec3 R_view = reflect(-V, N);
        vec3 T_view = refract(-V, N, eta);

        // In WELT-Space drehen
        vec3 R_world = normalize(transpose(uViewRot) * R_view);
        vec3 T_world = normalize(transpose(uViewRot) * T_view);

        // >>> Achsen-/Handedness-Fix, damit Cubemap-Orientation zur Skybox passt <<<
        // Häufigster Fall: Z-Achse invertieren
        R_world.z *= -1.0;
        T_world.z *= -1.0;

        // Cubemap in Weltkoordinaten samplen
        vec3 reflCol = texture(uEnvMap, R_world).rgb;
        vec3 refrCol = texture(uEnvMap, T_world).rgb;

        // Fresnel-Mix
        vec3 F0 = vec3(0.04);
        vec3 F  = fresnelSchlick(max(dot(N, V), 0.0), F0);

        vec3 tint = material.diffuse.rgb;
        vec3 glassCol = F * reflCol + (1.0 - F) * (refrCol * tint);

        col = mix(col, glassCol, clamp(uEnvStrength, 0.0, 1.0));
    }

if (nebel.enabled == 1) {
        float dView   = length(Position);
        float fogFact = computeFogFactorExp2(dView);
        col = mix(nebel.color, col, fogFact);
    }


    FragColor = vec4(col,alpha);
}   