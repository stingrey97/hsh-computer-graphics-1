#version 330 core
in vec3 FragPos;                      // Interpolierte Weltposition vom Vertex Shader
in vec3 Normal;                       // Interpolierte Normale im Weltraum

out vec4 FragColor;                   // Finale Farbe des Pixels

uniform vec3 lightPos;                // Position des Punktlichts im Weltraum
uniform vec3 viewPos;                 // Kameraposition im Weltraum
uniform vec3 lightColor;              // Lichtfarbe und Grundintensität
uniform vec3 objectColor;             // Grundfarbe des Materials

uniform float ambientStrength;        // Anteil des Umgebungslichts
uniform float specularStrength;       // Stärke der Glanzlichter
uniform float shininess;              // Glanz Exponent für die Größe des Highlightsr;


void main(void){
    // Ambient Anteil unabhängig von Richtung
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse Anteil abhängig vom Winkel zwischen Licht und Normale
    vec3 N = normalize(Normal);                   // Sicherstellen, dass die Normale Länge 1 hat
    vec3 L = normalize(lightPos - FragPos);      // Richtung vom Fragment zum Licht
    float diff = max(dot(N, L), 0.0);            // Cosinus des Winkels, nicht negativ werden lassen
    vec3 diffuse = diff * lightColor;            // Diffuse Lichtfarbe skalieren

    // Specular Anteil abhängig von Blickrichtung und Spiegelrichtung
    vec3 V = normalize(viewPos - FragPos);       // Richtung vom Fragment zum Auge
    vec3 R = reflect(-L, N);                     // Spiegelrichtung des Lichts an der Normalen
    float spec = pow(max(dot(V, R), 0.0), shininess); // Glanz berechnen mit Exponent
    vec3 specular = specularStrength * spec * lightColor; // Glanzfarbe skalieren

    // Zusammenführen und mit Objektfarbe multiplizieren
    vec3 color = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(color, 1.0);                // Ausgeben mit voller Deckkraft
}