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

    // Kamera
    GLfloat eye[3];
    GLfloat look[3];
    GLfloat up[3];

} AppContext;

#endif
