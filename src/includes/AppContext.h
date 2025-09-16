#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Mesh.h"
#include "Experiments.h"

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

    // Richtungslicht (struct lightSourceR richtungslicht)
    GLint uSun_enabled, uSun_direction, uSun_ambient, uSun_diffuse, uSun_specular;

    // Punktlicht (struct lightSourceP punktlicht)
    GLint uLamp_enabled, uLamp_position, uLamp_ambient, uLamp_diffuse, uLamp_specular, uLamp_linear, uLamp_quadratic;

    // Spotlicht (struct lightSourceS spotlicht)
    GLint uSpot_enabled, uSpot_position, uSpot_direction, uSpot_innerCone, uSpot_outerCone, uSpot_ambient, uSpot_diffuse, uSpot_specular, uSpot_linear, uSpot_quadratic;

    // Nebel (struct fog nebel)
    GLint uFogColor, uFogDensity, uFogEnabled;

    // Uniform Standorte
    GLint MVLoc, MVPLoc, NormalMLoc;

    // Mesh
    Mesh cube, teapot, column, gras, cottage, tree1, tree2, tree3, slenderman;

    // Environment mapping
    GLint uEnvMap, uUseEnvMap, uIOR, uEnvStrength, uViewRotLoc;

    // Licht Status
    Status lightStatus;

} AppContext;

#endif
