#include "mesh.h"

void calculateTangente(const GLfloat* vertices, int vertexCount, GLfloat* tangenten) {
    for (int i = 0; i < vertexCount; i += 3) {                  // Wir verarbeiten immer ein Dreieck auf einmal
        const GLfloat* v0 = &vertices[(i+0)*8];                 // Zeiger auf Vertex 0 dieses Dreiecks
        const GLfloat* v1 = &vertices[(i+1)*8];                 // Zeiger auf Vertex 1 dieses Dreiecks
        const GLfloat* v2 = &vertices[(i+2)*8];                 // Zeiger auf Vertex 2 dieses Dreiecks

        // Positionen
        float p0[3] = { v0[0], v0[1], v0[2] };                   // p0 = (x,y,z) von Vertex 0
        float p1[3] = { v1[0], v1[1], v1[2] };                   // p1 = (x,y,z) von Vertex 1
        float p2[3] = { v2[0], v2[1], v2[2] };                   // p2 = (x,y,z) von Vertex 2

        // UVs
        float uv0[2] = { v0[3], v0[4] };                         // uv0 = (u,v) von Vertex 0
        float uv1[2] = { v1[3], v1[4] };                         // uv1 = (u,v) von Vertex 1
        float uv2[2] = { v2[3], v2[4] };                         // uv2 = (u,v) von Vertex 2

        // Kanten im Raum und im UV
        float e1[3] = { p1[0]-p0[0], p1[1]-p0[1], p1[2]-p0[2] }; // e1 = p1 - p0 (erste Raumkante des Dreiecks)
        float e2[3] = { p2[0]-p0[0], p2[1]-p0[1], p2[2]-p0[2] }; // e2 = p2 - p0 (zweite Raumkante des Dreiecks)
        float du1 = uv1[0]-uv0[0], dv1 = uv1[1]-uv0[1];          // deltaUV1 = uv1 - uv0
        float du2 = uv2[0]-uv0[0], dv2 = uv2[1]-uv0[1];          // deltaUV2 = uv2 - uv0

        float denom = du1*dv2 - du2*dv1;                         // Determinante der 2x2 UV Matrix
        if (fabsf(denom) < 1e-20f) denom = 1e-20f;               // Schutz bei degenerierten UVs (verhindert Division durch 0)
        float f = 1.0f / denom;                                  // Invers der Determinante = Skalarfaktor für Lösung

        // Lösung des linearen Gleichungssystems:
        // e1 = T*du1 + B*dv1
        // e2 = T*du2 + B*dv2
        // ergibt die bekannten Formeln für T und B
        float T[3] = {
            f * ( dv2*e1[0] - dv1*e2[0] ),                       // Tangente x
            f * ( dv2*e1[1] - dv1*e2[1] ),                       // Tangente y
            f * ( dv2*e1[2] - dv1*e2[2] )                        // Tangente z
        };
        float B[3] = {
            f * ( -du2*e1[0] + du1*e2[0] ),                      // Bitangente x
            f * ( -du2*e1[1] + du1*e2[1] ),                      // Bitangente y
            f * ( -du2*e1[2] + du1*e2[2] )                       // Bitangente z
        };

        // pro Vertex: Gram-Schmidt + Sign
        for (int k = 0; k < 3; ++k) {                            // Für jedes der 3 Vertices die Dreieckstangente anpassen
            const GLfloat* vk = &vertices[(i+k)*8];              // vk zeigt auf das aktuelle Vertex

            float N[3] = { vk[5], vk[6], vk[7] };                // N aus deinem Array (nx,ny,nz) des Vertices
            // Gram-Schmidt: T orthogonal zu N machen
            float dotNT = N[0]*T[0] + N[1]*T[1] + N[2]*T[2];     // Projektion von T auf N
            float Tg[3] = { T[0] - N[0]*dotNT,                   // Tg = T - proj_N(T)
                           T[1] - N[1]*dotNT,
                           T[2] - N[2]*dotNT };
            float lenT = sqrtf(Tg[0]*Tg[0] + Tg[1]*Tg[1] + Tg[2]*Tg[2]);
            if (lenT > 1e-10f) { Tg[0]/=lenT; Tg[1]/=lenT; Tg[2]/=lenT; } // Normieren der Tangente

            // Handedness Sign berechnen: stimmt unsere B Richtung mit cross(N, T) überein
            float NxT[3] = {                                         // NxT = N × T_g
                N[1]*Tg[2] - N[2]*Tg[1],
                N[2]*Tg[0] - N[0]*Tg[2],
                N[0]*Tg[1] - N[1]*Tg[0]
            };
            float dot = NxT[0]*B[0] + NxT[1]*B[1] + NxT[2]*B[2];     // Vergleich mit geometrischer B
            float sign = (dot < 0.0f) ? -1.0f : 1.0f;                // Negativ bedeutet gespiegelt, daher Vorzeichen

            GLfloat* out = &tangenten[(i+k)*4];                      // Ausgabeschlitz für dieses Vertex
            out[0] = Tg[0];                                          // tx
            out[1] = Tg[1];                                          // ty
            out[2] = Tg[2];                                          // tz
            out[3] = sign;                                           // tw = handedness Sign
        }
    }
}

void loadMesh(const char* filename, Mesh *out){
    int werte[4];
    // read lines of OBJ4
    countLinesF(filename, werte);
    int vertexCount = werte[3];
    GLfloat* vertices = (GLfloat*)malloc(sizeof(GLfloat) * vertexCount * 8);
    // lese das OBJ aus und fülle die Vertices
    // 7 Werte pro Vertex: x, y, z, u, v, nx, ny, nz
    loadOBJ(filename, vertices, werte);

    // Tangenten Speicherplatz
    GLfloat* tangenten = (GLfloat*) malloc(sizeof(GLfloat) * vertexCount * 4);
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

    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(0));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    free(vertices);
    free(tangenten);

    out->vao = VAO;
    out->count = werte[3];
}
