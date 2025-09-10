#include <GL/glew.h>
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

    // Skybox
    GLuint skyboxProgramID;
    GLuint skyboxVAO;
    GLuint skyboxTexture;

    // Kamera
    GLfloat eye[3];
    GLfloat look[3];
    GLfloat up[3];

} AppContext;

#endif
