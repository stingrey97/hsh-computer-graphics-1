#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "AudioLoader.h"

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
    GLint  reflect;

    // Camera
    GLfloat INITIAL_EYE[3], INITIAL_LOOK[3], INITIAL_UP[3];
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
    Mesh cube, teapot, column, gras, cottage, baumstamm1, baumstamm2, baumstamm3, slenderman, laterne;

    // Environment mapping
    GLint uEnvMap, uUseEnvMap, uIOR, uEnvStrength, uViewRotLoc;

    // Texture uniform
    GLint albedoLoc, normalLoc, roughnessLoc, uvScale;

    // Textures
    GLuint albedoCottage, normalCottage;
    GLuint albedoColumn, normalColumn, roughColumn;
    GLuint albedoGras, normalGras, roughnessGras;
    GLuint albedoBaum1und2, albedoBaum3, normalBaum, roughBaum;
    GLuint albedoGlas, normalGlas, roughGlas;
    GLuint albedoTeapot;
    GLuint albedoSlenderman, normalSlenderman, roughSlenderman;
    GLuint albedoLaterne, normalLaterne, roughLaterne;

    // Licht/Vollbild Status
    Status status;

    // Audio
    ma_engine soundEngine;
    ma_sound backgroundMusic;
    ma_sound stepSound;
    ma_sound slendermanSpawnSound;
    ma_sound switchSound;

} AppContext;

#endif
