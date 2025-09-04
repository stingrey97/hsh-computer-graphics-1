#version 330 core

<<<<<<< HEAD
uniform mat4 modelMatrix;                   // Modellmatrix bringt Objekt in den Weltraum
uniform mat4 viewMatrix;                    // Viewmatrix bringt Welt in den Kameraraum
uniform mat4 projMatrix;                 // Projektionsmatrix bringt Kamera in Clipraum
=======
layout(location = 0) in vec3 vVertex;
layout(location = 1) in vec3 vNormal;
>>>>>>> e617669561d700d79f00312a05917d7f5149de39

uniform mat4 MV;        // ModelView
uniform mat4 MVP;       // ModelViewProjection
uniform mat3 NormalM;   // Normalenmatrix = inverse(transpose(upper-left 3x3 von MV))

smooth out vec3 Position; // im Augenkoordinatensystem
smooth out vec3 Normal;   // im Augenkoordinatensystem

void main(void)
{
    vec4 V = vec4(vVertex, 1.0);  // w = 1 hinzufügen

<<<<<<< HEAD
    // Normalen in den Weltraum transformieren
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix))); // Inverse Transpose für korrekte Normale bei Skalierung
    Normal = normalize(normalMatrix * aNormal);                 // transformierte Normale normalisieren
    
    gl_Position = projMatrix * viewMatrix * worldPos; // Endposition des Vertex im Clipraum
}   
=======
    gl_Position = MVP * V;        // Clip-Koordinaten
    vec4 Pos = MV * V;            // Augenkoordinaten
    Position = Pos.xyz / Pos.w;
    Normal   = normalize(NormalM * vNormal);
}
>>>>>>> e617669561d700d79f00312a05917d7f5149de39
