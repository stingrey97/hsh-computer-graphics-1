#version 330 core

smooth in vec3 Position;   // View-Space: aus dem VS
in vec2 TexCoord;          // View-SPace: aus dem VS
smooth in mat3 TBN;

out vec4 FragColor;

// Textures
uniform sampler2D uAlbedo;
uniform sampler2D uNormalMap;
uniform sampler2D uRoughness;

// Material uniform Struct 
uniform struct Material {
    vec4 emission;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
} material;

// Environment Mapping (Reflections)
uniform samplerCube uEnvMap;
uniform int uUseEnvMap;
uniform float uIOR; // IOR = index of refraction (1.5 for glass)
uniform float uEnvStrength;
uniform mat3 uViewRot;

// Berechnet die Fresnel-Schlick-Approximation für die Stärke der Reflektion abhängig vom Einfallswinkel
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Directional light struct
uniform struct lightSourceR {
    int enabled;
    vec3 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
} richtungslicht;

// Pointlight struct
uniform struct lightSourceP {
    int enabled;
    vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float linear;
    float quadratic;
} punktlicht;

// Spotlight struct
uniform struct lightSourceS {
    int enabled;
    vec3 position;
    vec3 direction;

    float innerCone; // radiant
    float outerCone;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float linear;
    float quadratic;
} spotlicht;

// Fog struct
uniform struct fog {
    vec3 color;
    float density;
    int enabled;
} nebel;

// Fog factor using the exponential squared model, clamped to 0-1.
float computeFogFactorExp2(float d) {
    float x = nebel.density * d;
    float f = exp(-x * x);
    return clamp(f, 0.0, 1.0);
}

void main(void) {
    vec3 light;
    vec3 reflection;
    float NdotL;
    float specAmt;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 albedo = texture(uAlbedo, TexCoord).rgb;

    vec3 n_ts = texture(uNormalMap, TexCoord).rgb * 2.0 - 1.0;
    vec3 normalMapVS = normalize(TBN * n_ts);
    vec3 normal = normalMapVS;

    float rough = texture(uRoughness, TexCoord).r;
    rough = clamp(rough, 0.0, 1.0);
    float shininessEff = mix(128.0, 2.0, rough);

    vec3 view = normalize(-Position);
    vec3 col = material.emission.rgb;
    float alpha = texture(uAlbedo, TexCoord).a; // Durchsichtig durch Textur sonst material.diffuse.a

    vec3 kd = albedo;
    vec3 ka = albedo;

    // Directional light
    if(richtungslicht.enabled == 1) {
        light = normalize(-richtungslicht.direction);
        ambient = (richtungslicht.ambient.rgb) * ka;

        NdotL = max(dot(normal, light), 0.0);
        float specAmt = 0.0;
        if(NdotL > 0.0) {
            reflection = reflect(-light, normal);
            float RV = max(dot(reflection, view), 1e-6);
            specAmt = pow(RV, shininessEff);
        }

        diffuse = (richtungslicht.diffuse.rgb) * NdotL * kd;
        specular = (material.specular.rgb * specAmt * richtungslicht.specular.rgb);

        col += ambient + diffuse + specular;
    }
    // Pointlight
    if(punktlicht.enabled == 1) {
        vec3 Lvec = punktlicht.position.xyz - Position;
        float dist = length(Lvec);
        light = (Lvec / max(dist, 1e-6));
        float attenuation = 1.0 / ((1.0) + punktlicht.linear * dist + punktlicht.quadratic * (dist * dist));

        NdotL = max(dot(normal, light), 0.0);
        specAmt = 0.0;
        if(NdotL > 0.0) {
            reflection = reflect(-light, normal);
            float RV = max(dot(reflection, view), 1e-6);
            specAmt = pow(RV, shininessEff);
        }

        ambient = attenuation * (punktlicht.ambient.rgb) * ka;
        diffuse = attenuation * (punktlicht.diffuse.rgb) * NdotL * kd;
        specular = attenuation * (material.specular.rgb * specAmt * punktlicht.specular.rgb);

        col += ambient + diffuse + specular;
    }
    // Spotlight
    if(spotlicht.enabled == 1) {
        light = normalize(spotlicht.position - Position);
        reflection = reflect(-light, normal);

        float theta = dot(-light, normalize(spotlicht.direction));
        float epsilon = max(spotlicht.innerCone - spotlicht.outerCone, 1e-6);
        float intensity = clamp((theta - spotlicht.outerCone) / epsilon, 0.0, 1.0);

        // Attenuation
        float dist = length(spotlicht.position - Position);
        float attenuation = 1.0 / (1.0 + spotlicht.linear * dist + spotlicht.quadratic * dist * dist);
        intensity *= attenuation;

        ambient = vec3(0.0);
        diffuse = vec3(0.0);
        specular = vec3(0.0);
        if(theta > spotlicht.outerCone) {
            ambient = spotlicht.ambient.rgb * ka;

            diffuse = intensity * spotlicht.diffuse.rgb * max(dot(normal, light), 0.0) * kd;

            specular = intensity * spotlicht.specular.rgb * pow(max(dot(reflection, view), 0.0), shininessEff) * material.specular.rgb;
        } else {
            ambient = spotlicht.ambient.rgb * ka;
        }
        col += ambient + diffuse + specular;
    }

    // Environment mapping (reflection + refraction) 
    if(uUseEnvMap == 1) {                                            // Wenn Environment-Mapping aktiviert ist
        vec3 N = normalize(normal);                                  // Normalenvektor normalisieren (Oberflächennormale im View-Space)
        vec3 V = normalize(-Position);                               // Blickrichtungsvektor normalisieren (vom Fragment zum Auge, im View-Space)

        float eta = 1.0 / max(uIOR, 1e-3);                           // Brechungsindex-Verhältnis (Verhältnis Luft/Material, Vermeidung von Division durch 0)
        vec3 R_view = reflect(-V, N);                                // Reflexionsvektor im View-Space berechnen
        vec3 T_view = refract(-V, N, eta);                           // Refraktionsvektor (gebrochener Strahl) im View-Space berechnen

        // Transform form View-Space in World-Space
        vec3 R_world = normalize(transpose(uViewRot) * R_view);      // Reflexionsvektor in den Weltkoordinaten
        vec3 T_world = normalize(transpose(uViewRot) * T_view);      // Refraktionsvektor in den Weltkoordinaten

        // Matching cubemap orientation (invert axis)
        R_world.z *= -1.0;                                           // Z-Achse des Reflexionsvektors invertieren
        T_world.z *= -1.0;                                           // Z-Achse des Refraktionsvektors invertieren

        vec3 reflCol = texture(uEnvMap, R_world).rgb;                // Reflexionsfarbe aus der Environment-Map samplen
        vec3 refrCol = texture(uEnvMap, T_world).rgb;                // Refraktionsfarbe (gebrochener Strahl) aus der Environment-Map samplen

        vec3 F0 = vec3(0.04);                                        // Basis-Reflexionsfaktor für die Fresnel-Gleichung (ungefärbtes Glas ca. 4%)
        vec3 F = fresnelSchlick(max(dot(N, V), 0.0), F0);            // Fresnel-Faktor berechnen (abhängig vom Blickwinkel)

        vec3 tint = material.diffuse.rgb;                            // Materialfarbe als Tönung für die Transmission
        vec3 glassCol = F * reflCol + (1.0 - F) * (refrCol * tint);  // Mischung: Fresnel-bedingte Reflexion + Transmission mit Tönung

        col = mix(col, glassCol, clamp(uEnvStrength, 0.0, 1.0));     // Ergebnisfarbe mit Glasfarbe mischen, gesteuert durch Stärke
    }

    // Fog
    if(nebel.enabled == 1) {
        float dView = length(Position);
        float fogFact = computeFogFactorExp2(dView);
        col = mix(nebel.color, col, fogFact);
    }

    FragColor = vec4(col, alpha);
}
