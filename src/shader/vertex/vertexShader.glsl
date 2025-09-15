#version 330 core

layout(location = 0) in vec3 vVertex;
layout(location = 1) in vec3 vNormal;


uniform mat4 MV;        // ModelView
uniform mat4 MVP;       // ModelViewProjection
uniform mat3 NormalM;   // Normalenmatrix = inverse(transpose(upper-left 3x3 von MV))

smooth out vec3 Position; // im Augenkoordinatensystem
smooth out vec3 Normal;   // im Augenkoordinatensystem

void main(void)
{
    vec4 V = vec4(vVertex, 1.0);  // w = 1 hinzufügen

    gl_Position = MVP * V;        // Clip-Koordinaten
    vec4 Pos = MV * V;            // Augenkoordinaten
    Position = Pos.xyz / Pos.w;
    Normal   = normalize(NormalM * vNormal);
}
