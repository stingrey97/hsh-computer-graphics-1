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
        light = normalize(spotlicht.position - Position);

        float theta = dot(-light, normalize(spotlicht.direction));
        float epsilon = max((spotlicht.innerCone - spotlicht.outerCone), 1e-6);
        float intensity = clamp((theta - spotlicht.outerCone)/ epsilon, 0.0f , 1.0f);

        float NdotL = max(dot(normal, light), 0.0);
        float specAmt = 0.0;
        if (NdotL > 0.0) {
            vec3 reflection = reflect(-light, normal);
            float RV = max(dot(reflection, view), 1e-6);
            specAmt = pow(RV, material.shininess);
        }
        ambient = (spotlicht.ambient.rgb) * (material.ambient.rgb);
        diffuse = intensity * (spotlicht.diffuse.rgb) * NdotL * (material.diffuse.rgb);
        specular = intensity * (material.specular.rgb) * specAmt * (spotlicht.specular.rgb);

        col += (ambient + diffuse + specular);
    }

    FragColor = vec4(col,alpha);
}