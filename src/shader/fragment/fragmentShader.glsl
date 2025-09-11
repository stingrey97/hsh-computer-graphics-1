#version 330 core

smooth in vec3 Position;   // View-Space: aus dem VS
in vec2 TexCoord;          // View-SPace: aus dem VS
smooth in mat3 TBN;

out vec4 FragColor;

// Textur
uniform sampler2D uAlbedo;
uniform sampler2D uNormalMap;
uniform sampler2D uRoughness;

// Material uniform Struct 
uniform struct Material {
    vec4 emission; 
    vec4 ambient;
    vec4 diffuse; // nur alpha rest von Textur
    vec4 specular;
    float shininess; // ungenutzt gerade nur Textur roughness
} material;

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

void main(void)
{
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
    vec3 col   = material.emission.rgb;
    float alpha = material.diffuse.a;

    vec3 kd = albedo;
    vec3 ka = albedo;

    // Richtungslicht
    if(richtungslicht.enabled==1){
        light = normalize(-richtungslicht.direction);
        ambient = (richtungslicht.ambient.rgb) * ka;

        NdotL = max(dot(normal, light), 0.0);
        float specAmt = 0.0;
        if (NdotL > 0.0) {
          reflection = reflect(-light, normal);
          float RV = max(dot(reflection, view), 1e-6);
          specAmt = pow(RV, shininessEff);
        }

        diffuse  = (richtungslicht.diffuse.rgb) * NdotL * kd;
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
          specAmt = pow(RV, shininessEff);
        }

        ambient = attenuation * (punktlicht.ambient.rgb) * ka;        
        diffuse = attenuation * (punktlicht.diffuse.rgb) * NdotL * kd;
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
            ambient  = spotlicht.ambient.rgb * ka;

            diffuse  = intensity * spotlicht.diffuse.rgb
                                * max(dot(normal, light), 0.0)
                                * kd;

            specular = intensity * spotlicht.specular.rgb
                                * pow(max(dot(reflection, view), 0.0), shininessEff)
                                * material.specular.rgb;
        } else {
            ambient = spotlicht.ambient.rgb * ka;
        }
        col += ambient + diffuse + specular;
    }

    FragColor = vec4(col,alpha);
}