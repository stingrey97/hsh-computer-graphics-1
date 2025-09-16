#ifndef TEXTURES_H
#define TEXTURES_H

#include <GL/glew.h>

GLuint loadTexture2D(const char* path, int useRGB);
GLuint loadCubemap(const char *faces[6]);

#endif
