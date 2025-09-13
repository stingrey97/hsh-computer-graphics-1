#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>

typedef struct
{
    GLuint vao;
    GLsizei count;
} Mesh;

extern void loadMesh(const char *filename, Mesh *out);

#endif
