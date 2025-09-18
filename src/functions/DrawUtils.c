// Self
#include "DrawUtils.h"

// Standard libs
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

// OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Own libs
#include "Mesh.h"
#include "Constants.h"
#include "MathUtils.h"
#include "Constants.h"
#include "Assertions.h"

static GLfloat position[2];

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

// Zahl zwischen 0 und 1
float frand01(void)
{
    return (float)rand() / (float)RAND_MAX;
}

// Zahl zwischen a und b
float frand(float a, float b)
{
    return a + (b - a) * frand01();
}

void drawForrest(int count,
                 GLfloat *M, const GLfloat *V, const GLfloat *P,
                 GLint MVLoc, GLint MVPLoc, GLint NormalMLoc,
                 GLuint albedoA, GLuint albedoB,
                 GLuint normalTex, GLuint roughTex,
                 Mesh *meshA, Mesh *meshB, Mesh *meshC,
                 GLint uvScale)
{
    // Gleicher Seed bäume sind gleich
    srand(1337);
    const float rMin = 20.0f; // innen frei
    const float rMax = 45.0f; // außen Wald
    GLuint albedo;
    Mesh *mesh;
    int m;
    float baseAngle;
    float angle;
    float radius;
    float x;
    float z;
    float rotYdeg;
    float s;

    // Konstant: Normal und Roughness einmal binden
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, roughTex);
    glUniform2f(uvScale, 1500.0f, 1500.0f);

    for (int i = 0; i < count; ++i)
    {
        // 1) Variante wählen
        if (frand01() < 0.5f)
        {
            albedo = albedoA;
        }
        else
        {
            albedo = albedoB;
        }

        m = rand() % 3;
        if (m == 0)
        {
            mesh = meshA;
        }
        else if (m == 1)
        {
            mesh = meshB;
        }
        else
        {
            mesh = meshC;
        }

        // 2) Position auf einem Ring mit leichtem Jitter
        baseAngle = 2.0f * (float)PI * ((float)i / (float)count);
        angle = baseAngle + frand(-0.25f, 0.25f); // etwas streuen
        radius = frand(rMin, rMax);
        x = cosf(angle) * radius;
        z = sinf(angle) * radius;

        // 3) Rotation und Scale
        rotYdeg = frand(0.0f, 360.0f);
        s = frand(0.6f, 1.0f);

        // 4) Texturen binden
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedo);

        // 5) Modelmatrix
        identity(M);
        translate(M, M, (GLfloat[]){x, 0.0f, z});
        rotateY(M, M, rotYdeg);
        scale(M, M, (GLfloat[]){s, s, s});

        drawMeshWithModel(mesh, V, P, M, MVLoc, MVPLoc, NormalMLoc);
    }
}

int drawSlenderman(GLfloat *M, const GLfloat *V, const GLfloat *P,
                   GLint MVLoc, GLint MVPLoc, GLint NormalMLoc,
                   GLuint albedo, GLuint normalTex, GLuint roughTex,
                   Mesh *mesh,
                   GLint uvScale,
                   const GLfloat *eye)
{
    double timeNow = glfwGetTime();     // Sekunden seit Programmstart
    double cycle = fmod(timeNow, 30.0); // 30-Sekunden-Periode

    // Slenderman soll zu uns gucken
    GLfloat blickRichtung[] = {-V[2], 0, -V[10]};
    norm3f(blickRichtung, blickRichtung);

    GLfloat toEyeX = eye[0] - (eye[0] + blickRichtung[0] * 23);
    GLfloat toEyeZ = eye[2] - (eye[2] + blickRichtung[2] * 23);
    float yawRad = atan2f(toEyeX, toEyeZ);
    float yawDeg = yawRad * (180.0f / (float)PI);

    static bool shouldPlay = true;

    if (cycle > 20.0f)
    {
        // Golderner Teapot
        glUniform2f(uvScale, 1.0f, 1.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedo);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTex);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, roughTex);
        identity(M);
        translate(M, M, (GLfloat[]){position[0], 0, position[1]});
        rotateY(M, M, yawDeg);
        scale(M, M, (GLfloat[]){0.75f, 0.75f, 0.75f});
        drawMeshWithModel(mesh, V, P, M, MVLoc, MVPLoc, NormalMLoc);
        if (cycle > 20.0f && cycle < 21.0f && shouldPlay)
        {
            shouldPlay = false;
            return 1;
        }
    }
    else
    {
        shouldPlay = true;
        position[0] = eye[0] + blickRichtung[0] * 23;
        position[1] = eye[2] + blickRichtung[2] * 23;
    }
    return 0;
}
