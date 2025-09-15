// Self
#include "DrawUtils.h"

// Standard libs
#include <stdio.h>
#include <assert.h>

// OpenGL
#include <GL/glew.h>

// Own libs
#include "Mesh.h"
#include "MathUtils.h"
#include "Assertions.h"

void drawMeshWithModel(const Mesh *m,
                       const float viewMatrix[16], const float projMatrix[16], const float model[16],
                       GLint MVLoc, GLint MVPLoc, GLint NormalMLoc)
{
    // Assertions
    assert(m != NULL);
    assert(isValidMatrix16f(viewMatrix));
    assert(isValidMatrix16f(projMatrix));
    assert(isValidMatrix16f(model));
    assert(MVLoc >= 0 && MVPLoc >= 0 && NormalMLoc >= 0);

    GLfloat MV[16], MVP[16], NormalM[9];
    mat4f_mul_mat4f(MV, viewMatrix, model);
    mat4f_mul_mat4f(MVP, projMatrix, MV);
    mat3_from_mat4(NormalM, MV);
    mat3_inverse_transpose(NormalM, NormalM);

    glUniformMatrix4fv(MVLoc, 1, GL_FALSE, MV);
    glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, MVP);
    glUniformMatrix3fv(NormalMLoc, 1, GL_FALSE, NormalM);

    glBindVertexArray(m->vao);
    glDrawArrays(GL_TRIANGLES, 0, m->count);
}

void drawtransparentMeshWithModel(const Mesh *m,
                                  const float viewMatrix[16], const float projMatrix[16], const float model[16],
                                  GLint MVLoc, GLint MVPLoc, GLint NormalMLoc)
{
    // Assertions
    assert(m != NULL);
    assert(isValidMatrix16f(viewMatrix));
    assert(isValidMatrix16f(projMatrix));
    assert(isValidMatrix16f(model));
    assert(MVLoc >= 0 && MVPLoc >= 0 && NormalMLoc >= 0);

    // Matrizen vorbereiten (einmal) – wird in beiden Pässen benutzt
    GLfloat MV[16], MVP[16], NormalM[9];
    mat4f_mul_mat4f(MV, viewMatrix, model);
    mat4f_mul_mat4f(MVP, projMatrix, MV);
    mat3_from_mat4(NormalM, MV);
    mat3_inverse_transpose(NormalM, NormalM);
    glUniformMatrix4fv(MVLoc, 1, GL_FALSE, MV);
    glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, MVP);
    glUniformMatrix3fv(NormalMLoc, 1, GL_FALSE, NormalM);

    glBindVertexArray(m->vao);

    glDepthMask(GL_FALSE); // kein Z-Write für Transparenz

    glCullFace(GL_FRONT); // Pass 1: Rückseiten
    glDrawArrays(GL_TRIANGLES, 0, m->count);

    glCullFace(GL_BACK); // Pass 2: Vorderseiten
    glDrawArrays(GL_TRIANGLES, 0, m->count);

    glDepthMask(GL_TRUE);
}
