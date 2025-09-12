#include <GL/glew.h>

#ifndef MESH_H
#define MESH_H

typedef struct
{
    GLuint vao;
    GLsizei count;
} Mesh;

extern void loadMesh(const char *filename, Mesh *out);

#endif
