#ifndef DRAWUTILS_H
#define DRAWUTILS_H

#include <GL/glew.h>

#include "Mesh.h"

// Draws a opaque mesh with the given model, view, and projection matrices
extern void drawMeshWithModel(const Mesh *m,
                              const float viewMatrix[16],
                              const float projMatrix[16],
                              const float model[16],
                              GLint MVLoc,
                              GLint MVPLoc,
                              GLint NormalMLoc);

// Draws a transparent mesh with the given model, view, and projection matrices
extern void drawtransparentMeshWithModel(const Mesh *m,
                                         const float viewMatrix[16],
                                         const float projMatrix[16],
                                         const float model[16],
                                         GLint MVLoc,
                                         GLint MVPLoc,
                                         GLint NormalMLoc);

// Draws multiple trees to form a forest using different meshes and textures
extern void drawForrest(int count,
                        GLfloat *M, const GLfloat *V, const GLfloat *P,
                        GLint MVLoc, GLint MVPLoc, GLint NormalMLoc,
                        GLuint albedoA, GLuint albedoB,
                        GLuint normalTex, GLuint roughTex,
                        Mesh *meshA, Mesh *meshB, Mesh *meshC,
                        GLint uvScale);

// Draws the Slenderman model. Returns 1 if Slenderman just spawned (used for triggering audio playback)
extern int drawSlenderman(GLfloat *M, const GLfloat *V, const GLfloat *P,
                          GLint MVLoc, GLint MVPLoc, GLint NormalMLoc,
                          GLuint albedo, GLuint normalTex, GLuint roughTex,
                          Mesh *mesh,
                          GLint uvScale,
                          const GLfloat *eye);

#endif
