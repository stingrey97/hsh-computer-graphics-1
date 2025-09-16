#ifndef DRAWUTILS_H
#define DRAWUTILS_H

#include <GL/glew.h>

#include "Mesh.h"

extern void drawMeshWithModel(const Mesh *m,
                              const float viewMatrix[16],
                              const float projMatrix[16],
                              const float model[16],
                              GLint MVLoc,
                              GLint MVPLoc,
                              GLint NormalMLoc);

extern void drawtransparentMeshWithModel(const Mesh *m,
                                         const float viewMatrix[16],
                                         const float projMatrix[16],
                                         const float model[16],
                                         GLint MVLoc,
                                         GLint MVPLoc,
                                         GLint NormalMLoc);

extern void drawForrest(int count,
                        GLfloat *M, const GLfloat *V, const GLfloat *P,
                        GLint MVLoc, GLint MVPLoc, GLint NormalMLoc,
                        GLuint albedoA, GLuint albedoB,
                        GLuint normalTex, GLuint roughTex,
                        Mesh *meshA, Mesh *meshB, Mesh *meshC,
                        GLint uvScale);

extern void drawSlenderman(GLfloat *M, const GLfloat *V, const GLfloat *P,
                    GLint MVLoc, GLint MVPLoc, GLint NormalMLoc,
                    GLuint albedo, GLuint normalTex, GLuint roughTex,
                    Mesh *mesh,
                    GLint uvScale,
                    const GLfloat *eye);

#endif
