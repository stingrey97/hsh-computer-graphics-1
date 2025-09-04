#ifndef LIGHTUTILS_H
#define LIGHTUTILS_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>
#include "matrixUtils.h"
extern void initializeDirectionalLight(GLint uSun_ambient, GLint uSun_diffuse, GLint uSun_specular);
extern void initializePointLight(GLint uLamp_ambient, GLint uLamp_diffuse, GLint uLamp_specular, GLint uLamp_linear, GLint uLamp_quadratic);
extern void initializeSpotLight(GLint uSpot_ambient, GLint uSpot_diffuse, GLint uSpot_specular, GLint uSpot_innerCone, GLint uSpot_outerCone);

extern void setPointLight(GLint uLamp_position, const float viewMatrix[16], float x, float y, float z);
extern void setDirectionalLight(GLint uSun_direction, const float viewMatrix[16], float x, float y, float z);
extern void setSpotLight(GLint uSpot_position, GLint uSpot_direction, const float viewMatrix[16], const float eye[3], const float center[3]);
#endif