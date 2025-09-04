#include <GLFW/glfw3.h>

#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

typedef struct
{
    // Window
    GLFWwindow *window;
    GLint width, height;

    // Shader
    GLuint programID;
    GLuint locationMVP;

    // View and Perspectiv Matrix
    GLfloat V[16];
    GLfloat P[16];

    // Cube VAO/VBO
    GLuint cubeVAO;
    GLuint cubeVBO;

} AppContext;

#endif
