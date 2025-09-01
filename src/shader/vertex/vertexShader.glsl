#version 330 core
layout (location =0) in vec3 aPosition;     // Positionen aus dem VBO
layout (location =1) in vec3 aNormal;       // Normalen aus dem VBO

uniform mat4 modelMatrix;                   // Modellmatrix bringt Objekt in den Weltraum
uniform mat4 viewMatrix;                    // Viewmatrix bringt Welt in den Kameraraum
uniform mat4 projectMatrix;                 // Projektionsmatrix bringt Kamera in Clipraum


out vec3 FragPos;                           // An den Fragment Shader: Weltposition jedes Fragments
out vec3 Normal;                            // An den Fragment Shader: Normale im Weltraum

void main(void){
    vec4 worldPos = modelMatrix * vec4(aPosition, 1.0); // Objektposition in Weltposition umrechnen
    FragPos = worldPos.xyz;                              // Weltposition für spätere Lichtberechnung merken

    // Normalen in den Weltraum transformieren
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix))); // Inverse Transpose für korrekte Normale bei Skalierung
    Normal = normalize(normalMatrix * aNormal);                 // transformierte Normale normalisieren
    
    gl_Position = projectMatrix * viewMatrix * worldPos; // Endposition des Vertex im Clipraum
}   