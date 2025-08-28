#version 330 core
layout (location =0) in vec3 aPosition;
layout (location =1) in vec3 aColor;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectMatrix;
out vec3 vertexColor;
void main(void){
    vertexColor = aColor;
    gl_Position = projectMatrix * viewMatrix * modelMatrix * vec4(aPosition, 1.0);
}