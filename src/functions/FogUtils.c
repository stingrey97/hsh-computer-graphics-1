// Self
#include "FogUtils.h"

// OpenGL
#include <GL/glew.h>

void initializeFog(GLint uFogColor, GLint uFogDensity)
{
    glUniform3f(uFogColor, 0.17f, 0.19f, 0.21f);
    glUniform1f(uFogDensity, 0.1f);
}
