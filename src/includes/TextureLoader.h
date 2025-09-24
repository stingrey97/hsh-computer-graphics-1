#ifndef TEXTURES_H
#define TEXTURES_H

#include <GL/glew.h>

// Loads a 2D texture from file, optionally in RGB format
GLuint loadTexture2D(const char* path, int useRGB);

// Loads a cubemap texture from 6 image files (order: +X, -X, +Y, -Y, +Z, -Z)
GLuint loadCubemap(const char *faces[6]);

#endif
