#ifndef DRAWUTILS_H
#define DRAWUTILS_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "mesh.h"
#include "matrixUtils.h"
extern void drawMeshWithModel(const Mesh* m,
                            const float viewMatrix[16], const float projMatrix[16], const float model[16],
                            GLint MVLoc, GLint MVPLoc, GLint NormalMLoc);
extern void drawtransparentMeshWithModel(const Mesh* m,
                            const float viewMatrix[16], const float projMatrix[16], const float model[16],
                            GLint MVLoc, GLint MVPLoc, GLint NormalMLoc);
#endif