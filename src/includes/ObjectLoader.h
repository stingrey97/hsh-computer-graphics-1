#ifndef LOADOBJ_H
#define LOADOBJ_H

#include <GL/glew.h>

// Counts the number of relevant lines (e.g., vertices, normals, faces) in an OBJ file
extern void countLinesF(const char *filename, int *werte);

// Loads an OBJ file into the output buffer (vertices, normals, etc.)
extern void loadOBJ(const char *filename, GLfloat *out, int *werte);

#endif
