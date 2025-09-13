#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

    // Camera
    GLfloat eye[3], look[3], up[3];

    // Material
    GLint uMat_emission, uMat_ambient, uMat_diffuse, uMat_specular, uMat_shininess;

} AppContext;

#endif
