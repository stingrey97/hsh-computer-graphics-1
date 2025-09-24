#ifndef FOGUTILS_H
#define FOGUTILS_H

#include <GL/glew.h>

// Initializes fog parameters (color and density) in the shader
extern void initializeFog(GLint uFogColor, GLint uFogDensity);

#endif
