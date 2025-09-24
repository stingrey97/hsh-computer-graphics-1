#ifndef LOAD_SHADERS_H
#define LOAD_SHADERS_H

#include <GL/glew.h>

// Creates a program, compiles the vertex and fragment shaders from the given file paths, and links them
extern GLuint loadShaders(const char *vertex_file_path, const char *fragment_file_path);

#endif
