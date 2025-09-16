// Self
#include "Mesh.h"

// Standard libs
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

// OpenGL
#include <GL/glew.h>

// Own libs
#include "ObjectLoader.h"

void calculateTangente(const GLfloat *vertices, int vertexCount, GLfloat *tangenten_out)
{
    // Layout: [x y z u v nx ny nz] pro Vertex
    const int stride = 8;

    // Akkumulatoren
    GLfloat *tanSum = (GLfloat *)calloc(vertexCount * 3, sizeof(GLfloat));
    GLfloat *bitSum = (GLfloat *)calloc(vertexCount * 3, sizeof(GLfloat));

    for (int i = 0; i < vertexCount; i += 3)
    {
        const GLfloat *v0 = &vertices[(i + 0) * stride];
        const GLfloat *v1 = &vertices[(i + 1) * stride];
        const GLfloat *v2 = &vertices[(i + 2) * stride];

        // Positionen
        float p0x = v0[0], p0y = v0[1], p0z = v0[2];
        float p1x = v1[0], p1y = v1[1], p1z = v1[2];
        float p2x = v2[0], p2y = v2[1], p2z = v2[2];

        // UVs
        float u0 = v0[3], v0u = v0[4];
        float u1 = v1[3], v1u = v1[4];
        float u2 = v2[3], v2u = v2[4];

        // Kanten
        float e1x = p1x - p0x, e1y = p1y - p0y, e1z = p1z - p0z;
        float e2x = p2x - p0x, e2y = p2y - p0y, e2z = p2z - p0z;

        float du1 = u1 - u0, dv1 = v1u - v0u;
        float du2 = u2 - u0, dv2 = v2u - v0u;

        float denom = du1 * dv2 - du2 * dv1;
        if (fabsf(denom) < 1e-12f)
        {
            // degenerierte UVs: dieses Dreieck trägt nicht sinnvoll zu T/B bei
            continue;
        }
        float f = 1.0f / denom;

        // Dreiecks-T/B
        float Tx = f * (dv2 * e1x - dv1 * e2x);
        float Ty = f * (dv2 * e1y - dv1 * e2y);
        float Tz = f * (dv2 * e1z - dv1 * e2z);

        float Bx = f * (-du2 * e1x + du1 * e2x);
        float By = f * (-du2 * e1y + du1 * e2y);
        float Bz = f * (-du2 * e1z + du1 * e2z);

        // Auf die drei Vertex-Akkus addieren
        int i0 = (i + 0) * 3, i1 = (i + 1) * 3, i2 = (i + 2) * 3;
        tanSum[i0 + 0] += Tx;
        tanSum[i0 + 1] += Ty;
        tanSum[i0 + 2] += Tz;
        tanSum[i1 + 0] += Tx;
        tanSum[i1 + 1] += Ty;
        tanSum[i1 + 2] += Tz;
        tanSum[i2 + 0] += Tx;
        tanSum[i2 + 1] += Ty;
        tanSum[i2 + 2] += Tz;

        bitSum[i0 + 0] += Bx;
        bitSum[i0 + 1] += By;
        bitSum[i0 + 2] += Bz;
        bitSum[i1 + 0] += Bx;
        bitSum[i1 + 1] += By;
        bitSum[i1 + 2] += Bz;
        bitSum[i2 + 0] += Bx;
        bitSum[i2 + 1] += By;
        bitSum[i2 + 2] += Bz;
    }

    // Finalisierung je Vertex: Gram-Schmidt, Handedness, Fallback wenn nötig
    for (int v = 0; v < vertexCount; ++v)
    {
        const GLfloat *V = &vertices[v * stride];
        float Nx = V[5], Ny = V[6], Nz = V[7];

        // Tangenten-Summe orthogonalisieren gegen N
        float Tx = tanSum[v * 3 + 0], Ty = tanSum[v * 3 + 1], Tz = tanSum[v * 3 + 2];

        // Fallback, falls zu klein (z. B. wegen Degeneraten): baue T aus N
        float lenT2 = Tx * Tx + Ty * Ty + Tz * Tz;
        if (lenT2 < 1e-20f)
        {
            // wähle eine Hilfsachse, die nicht parallel zu N ist
            float ax = (fabsf(Nx) < 0.9f) ? 1.0f : 0.0f;
            float ay = (fabsf(Nx) < 0.9f) ? 0.0f : 1.0f;
            float az = 0.0f;
            // T = normalize( cross( N, Hilfsachse ) )
            float cx = Ny * az - Nz * ay;
            float cy = Nz * ax - Nx * az;
            float cz = Nx * ay - Ny * ax;
            Tx = cx;
            Ty = cy;
            Tz = cz;
        }

        // Gram-Schmidt
        float dotNT = Nx * Tx + Ny * Ty + Nz * Tz;
        Tx -= Nx * dotNT;
        Ty -= Ny * dotNT;
        Tz -= Nz * dotNT;
        float lenT = sqrtf(Tx * Tx + Ty * Ty + Tz * Tz);
        if (lenT > 1e-20f)
        {
            Tx /= lenT;
            Ty /= lenT;
            Tz /= lenT;
        }
        else
        {
            Tx = 1;
            Ty = 0;
            Tz = 0;
        } // letzter Fallback

        // Handedness aus aufsummierter Bitangente
        float Bx = bitSum[v * 3 + 0], By = bitSum[v * 3 + 1], Bz = bitSum[v * 3 + 2];
        // sign = sign( dot( cross(N,T), Bsum ) )
        float cx = Ny * Tz - Nz * Ty;
        float cy = Nz * Tx - Nx * Tz;
        float cz = Nx * Ty - Ny * Tx;
        float sign = (cx * Bx + cy * By + cz * Bz) < 0.0f ? -1.0f : 1.0f;

        GLfloat *out = &tangenten_out[v * 4];
        out[0] = Tx;
        out[1] = Ty;
        out[2] = Tz;
        out[3] = sign;
    }

    free(tanSum);
    free(bitSum);
}

void loadMesh(const char *filename, Mesh *out)
{
    assert(filename != NULL && out != NULL);

    int werte[4];
    // read lines of OBJ4
    countLinesF(filename, werte);
    int vertexCount = werte[3];
    GLfloat *vertices = (GLfloat *)malloc(sizeof(GLfloat) * vertexCount * 8);
    // lese das OBJ aus und fülle die Vertices
    // 7 Werte pro Vertex: x, y, z, u, v, nx, ny, nz
    loadOBJ(filename, vertices, werte);

    // Tangenten Speicherplatz
    GLfloat *tangenten = (GLfloat *)malloc(sizeof(GLfloat) * vertexCount * 4);
    calculateTangente(vertices, vertexCount, tangenten);

    GLuint VBO, VAO, TBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount * 8, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // create vertex array object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &TBO);
    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount * 4, tangenten, GL_STATIC_DRAW);

    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(0));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    free(vertices);
    free(tangenten);

    out->vao = VAO;
    out->count = werte[3];
}
