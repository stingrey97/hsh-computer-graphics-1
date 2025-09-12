// Self
#include "Mesh.h"

// Standard libs
#include <stdio.h>
#include <stdlib.h>

// OpenGL
#include <GL/glew.h>

// Own libs
#include "ObjectLoader.h"

void loadMesh(const char* filename, Mesh *out){
    int werte[4];
    // read lines of OBJ4
    countLinesF(filename, werte);
    GLfloat* vertices = (GLfloat*)malloc(sizeof(GLfloat) * werte[3] * 8);
    // lese das OBJ aus und fülle die Vertices
    // 7 Werte pro Vertex: x, y, z, u, v, nx, ny, nz
    loadOBJ(filename, vertices, werte);

    GLuint VBO, VAO; // VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * werte[3] * 8, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // create vertex array object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    free(vertices);

    out->vao = VAO;
    out->count = werte[3];
}