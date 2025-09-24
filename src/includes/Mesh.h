#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>

// Represents a mesh with a vertex array object (VAO) and vertex count
typedef struct
{
    GLuint vao;    // OpenGL vertex array object handle
    GLsizei count; // Number of vertices
} Mesh;

// Loads a mesh from file into the given Mesh struct
extern void loadMesh(const char *filename, Mesh *out);

#endif
