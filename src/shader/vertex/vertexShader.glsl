#version 330 core
layout(location=0) in vec3 vVertex;
layout(location=1) in vec3 vNormal;
// MV-Spalten
layout(location=2) in vec4 iMV0;
layout(location=3) in vec4 iMV1;
layout(location=4) in vec4 iMV2;
layout(location=5) in vec4 iMV3;
// NormalM-Spalten
layout(location=10) in vec3 iN0;
layout(location=11) in vec3 iN1;
layout(location=12) in vec3 iN2;

// Uniforms für Nicht-Instanced-Objekte
uniform mat4 MV;
uniform mat4 MVP;
uniform mat3 NormalM;

// NEU für Instancing:
uniform mat4 P;
uniform bool useInstancing;

smooth out vec3 Position;
smooth out vec3 Normal;

void main(){
    mat4 MVx = useInstancing ? mat4(iMV0,iMV1,iMV2,iMV3) : MV;
    mat3 Nx  = useInstancing ? mat3(iN0,iN1,iN2)         : NormalM;

    vec4 V4 = vec4(vVertex,1.0);
    gl_Position = P * (MVx * V4); // MVP nicht als Attribut
    vec4 Pos = MVx * V4;
    Position = Pos.xyz / Pos.w;
    Normal   = normalize(Nx * vNormal);
}