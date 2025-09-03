#ifndef MESH_H
#define MESH_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include "loadObj.h"
typedef struct
{
    GLuint vao;
    GLsizei count; // glDrawArrays count
} Mesh;
extern void loadMesh(const char* filename, Mesh *out);
#endif