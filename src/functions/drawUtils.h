#include <GL/glew.h>

#include "mesh.h"

#ifndef DRAWUTILS_H
#define DRAWUTILS_H

extern void drawMeshWithModel(const Mesh* m,
                            const float viewMatrix[16], const float projMatrix[16], const float model[16],
                            GLint MVLoc, GLint MVPLoc, GLint NormalMLoc);
extern void drawtransparentMeshWithModel(const Mesh* m,
                            const float viewMatrix[16], const float projMatrix[16], const float model[16],
                            GLint MVLoc, GLint MVPLoc, GLint NormalMLoc);

#endif
