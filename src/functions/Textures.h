#include <GL/glew.h>

#ifndef TEXTURES_H
#define TEXTURES_H

GLuint loadTexture2D(const char* path, int useRGB);
GLuint loadCubemap(const char *faces[6]);

#endif