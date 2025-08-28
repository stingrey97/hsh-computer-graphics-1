#version 330 core
in vec3 vertexColor;
void main(void){
    gl_FragColor = vec4(vertexColor, 1.0);
};