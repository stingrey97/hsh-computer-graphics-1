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

extern void setPointLight();
extern void setDirectionalLight();
extern void setSpotLight();
#endif