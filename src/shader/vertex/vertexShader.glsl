#version 330 core

layout(location = 0) in vec3 vVertex;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;
layout(location = 3) in vec4 vTangente;


uniform mat4 MV;        // ModelView
uniform mat4 MVP;       // ModelViewProjection
uniform mat3 NormalM;   // Normalenmatrix = inverse(transpose(upper-left 3x3 von MV))
uniform vec2 uvScale;   // Wiederholung des Bildes

smooth out vec3 Position; // im Augenkoordinatensystem
out vec2 TexCoord;        // im Augenkoordinatensystem
smooth out mat3 TBN;      // im AUgenkoordinatensystem

void main(void)
{
    vec4 V = vec4(vVertex, 1.0);  // w = 1 hinzufügen

    gl_Position = MVP * V;        // Clip-Koordinaten
    vec4 Pos = MV * V;            // Augenkoordinaten
    Position = Pos.xyz / Pos.w;
    vec3 Normal   = normalize(NormalM * vNormal);
    TexCoord = vTexCoord * uvScale;

    // T mit der linearen MV in den View Space
    vec3 T = mat3(MV) * vTangente.xyz;
    // Gram-Schmidt: T orthogonal zu N machen
    T = normalize(T - Normal * dot(Normal, T));
    vec3 B = normalize(cross(Normal, T) * vTangente.w);

    TBN = mat3(T, B, Normal);
}
