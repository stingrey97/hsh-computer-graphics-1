// Standard libs
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>

// OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Third party libs
#define STB_IMAGE_IMPLEMENTATION
#include "includes/ImageLoader.h"

// Own libs
#include "Constants.h"
#include "AppContext.h"
#include "MathUtils.h"
#include "ShaderLoader.h"
#include "Mesh.h"
#include "DrawUtils.h"
#include "LightUtils.h"
#include "Camera.h"
#include "Experiments.h"
#include "FogUtils.h"
#include "Skybox.h"
#include "Materials.h"

// Richtungslicht (struct lightSourceR richtungslicht)
GLint uSun_enabled, uSun_direction, uSun_ambient, uSun_diffuse, uSun_specular;

// Punktlicht (struct lightSourceP punktlicht)
GLint uLamp_enabled, uLamp_position, uLamp_ambient, uLamp_diffuse, uLamp_specular, uLamp_linear, uLamp_quadratic;

// Spotlicht (struct lightSourceS spotlicht)
GLint uSpot_enabled, uSpot_position, uSpot_direction, uSpot_innerCone, uSpot_outerCone, uSpot_ambient, uSpot_diffuse, uSpot_specular, uSpot_linear, uSpot_quadratic;

// Nebel (struct fog nebel)
GLint uFogColor, uFogDensity, uFogEnabled;

// Uniform Standorte
GLint MVLoc, MVPLoc, NormalMLoc;

// Mesh
Mesh cube, teapot, column, gras, cottage, tree1, tree2, tree3, slenderman;

// Licht Status
Status status = {1, 1, 1};

/*
 * ???
 */
// --- Instancing: CPU -> GPU Strukturen
typedef struct
{
    GLfloat m[16];
} Mat4C;
typedef struct
{
    GLfloat m[9];
} Mat3C;

typedef struct
{
    Mat4C MV;
    Mat3C NormalM;
} InstanceCPU;

typedef struct
{
    GLuint vbo;
    int count;
    Mat4C *Model;
    InstanceCPU *cpu;
    GLsizeiptr capacity; // in Bytes, für VBO und cpu
} InstanceSet;

// Drei Baumvarianten
static InstanceSet gTrees[3] = {0};

// Uniform-Schalter
GLint uUseInstancing;

// --- Attribute für Instancing an ein VAO binden (C, keine Lambdas)
static void bindInstanceAttributes(GLuint vao, GLuint *outVBO, GLsizeiptr bufferSizeBytes)
{
    glBindVertexArray(vao);
    glGenBuffers(1, outVBO);
    glBindBuffer(GL_ARRAY_BUFFER, *outVBO);
    glBufferData(GL_ARRAY_BUFFER, bufferSizeBytes, NULL, GL_DYNAMIC_DRAW); // initial leer

    GLsizei stride = (GLsizei)sizeof(InstanceCPU);
    size_t base = 0;

    // iMV -> locations 2..5 (4 x vec4)
    for (int k = 0; k < 4; ++k)
    {
        glEnableVertexAttribArray(2 + k);
        glVertexAttribPointer(2 + k, 4, GL_FLOAT, GL_FALSE, stride, (void *)(base + k * 4 * sizeof(GLfloat)));
        glVertexAttribDivisor(2 + k, 1);
    }
    base += 16 * sizeof(GLfloat);

    // iNormalM -> locations 10..12 (3 x vec3)
    for (int k = 0; k < 3; ++k)
    {
        glEnableVertexAttribArray(10 + k);
        glVertexAttribPointer(10 + k, 3, GL_FLOAT, GL_FALSE, stride, (void *)(base + k * 3 * sizeof(GLfloat)));
        glVertexAttribDivisor(10 + k, 1);
    }

    glBindVertexArray(0);
}

// --- Pro Frame CPU-Daten befüllen und instanziert zeichnen (C, keine Lambdas)
static void updateAndDrawTrees(const Mesh *mesh, InstanceSet *set,
                               const GLfloat V[16], const GLfloat P[16],
                               AppContext *ctx)
{
    if (!set || set->count <= 0) return;

    const float maxDist2 = TREE_RENDER_DISTANCE * TREE_RENDER_DISTANCE;
    int write = 0;

    for (int i = 0; i < set->count; ++i) {
        // Weltposition aus Model-Matrix (column-major: M[12..14])
        float wx = set->Model[i].m[12];
        float wy = set->Model[i].m[13];
        float wz = set->Model[i].m[14];

        float dx = wx - ctx->eye[0];
        float dy = wy - ctx->eye[1];
        float dz = wz - ctx->eye[2];
        float d2 = dx*dx + dy*dy + dz*dz;
        if (d2 > maxDist2) continue; // zu weit weg -> überspringen

        // MV = V * M
        GLfloat MV[16];
        mat4f_mul_mat4f(MV, V, set->Model[i].m);

        // Instanzdaten schreiben -> kompakt in set->cpu[write]
        memcpy(set->cpu[write].MV.m, MV, sizeof(MV));
        mat3_from_mat4(set->cpu[write].NormalM.m, MV);
        ++write;
    }

    if (write == 0) return; // nichts sichtbar

    // Upload genau der sichtbaren Instanzen
    glBindBuffer(GL_ARRAY_BUFFER, set->vbo);
    void* dst = glMapBufferRange(GL_ARRAY_BUFFER, 0,
                                 (GLsizeiptr)write * (GLsizeiptr)sizeof(InstanceCPU),
                                 GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
    if (dst) {
        memcpy(dst, set->cpu, (size_t)write * sizeof(InstanceCPU));
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }

    // Draw nur mit sichtbarer Anzahl
    glBindVertexArray(mesh->vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, mesh->count, write);
    glBindVertexArray(0);
}

/*
 * ???
 */

void framebuffer_size_callback(GLFWwindow *window, int cb_width, int cb_height)
{
    glViewport(0, 0, cb_width, cb_height);
}

void init(AppContext *ctx)
{
    // Initial camera
    setVec3(ctx->eye, 5, 0, 0);
    setVec3(ctx->look, 0, 0, 0);
    setVec3(ctx->up, 0, 1, 0);

    initCamera(ctx);

    const char *vertexPath = "shader/vertex/vertexShader.glsl";
    const char *fragmentPath = "shader/fragment/fragmentShader.glsl";

    ctx->programID = loadShaders(vertexPath, fragmentPath);
    glUseProgram(ctx->programID);

    // einmal die Location holen Matrizen
    MVLoc = glGetUniformLocation(ctx->programID, "MV");
    MVPLoc = glGetUniformLocation(ctx->programID, "MVP");
    NormalMLoc = glGetUniformLocation(ctx->programID, "NormalM");

    // Get uniforms vor materials
    ctx->uMat_emission = glGetUniformLocation(ctx->programID, "material.emission");
    ctx->uMat_ambient = glGetUniformLocation(ctx->programID, "material.ambient");
    ctx->uMat_diffuse = glGetUniformLocation(ctx->programID, "material.diffuse");
    ctx->uMat_specular = glGetUniformLocation(ctx->programID, "material.specular");
    ctx->uMat_shininess = glGetUniformLocation(ctx->programID, "material.shininess");

    // Richtungslicht
    uSun_enabled = glGetUniformLocation(ctx->programID, "richtungslicht.enabled");
    uSun_direction = glGetUniformLocation(ctx->programID, "richtungslicht.direction");
    uSun_ambient = glGetUniformLocation(ctx->programID, "richtungslicht.ambient");
    uSun_diffuse = glGetUniformLocation(ctx->programID, "richtungslicht.diffuse");
    uSun_specular = glGetUniformLocation(ctx->programID, "richtungslicht.specular");

    // Punktlicht
    uLamp_enabled = glGetUniformLocation(ctx->programID, "punktlicht.enabled");
    uLamp_position = glGetUniformLocation(ctx->programID, "punktlicht.position");
    uLamp_ambient = glGetUniformLocation(ctx->programID, "punktlicht.ambient");
    uLamp_diffuse = glGetUniformLocation(ctx->programID, "punktlicht.diffuse");
    uLamp_specular = glGetUniformLocation(ctx->programID, "punktlicht.specular");
    uLamp_linear = glGetUniformLocation(ctx->programID, "punktlicht.linear");
    uLamp_quadratic = glGetUniformLocation(ctx->programID, "punktlicht.quadratic");

    // Spotlicht
    uSpot_enabled = glGetUniformLocation(ctx->programID, "spotlicht.enabled");
    uSpot_position = glGetUniformLocation(ctx->programID, "spotlicht.position");
    uSpot_direction = glGetUniformLocation(ctx->programID, "spotlicht.direction");
    uSpot_innerCone = glGetUniformLocation(ctx->programID, "spotlicht.innerCone");
    uSpot_outerCone = glGetUniformLocation(ctx->programID, "spotlicht.outerCone");
    uSpot_ambient = glGetUniformLocation(ctx->programID, "spotlicht.ambient");
    uSpot_diffuse = glGetUniformLocation(ctx->programID, "spotlicht.diffuse");
    uSpot_specular = glGetUniformLocation(ctx->programID, "spotlicht.specular");
    uSpot_linear = glGetUniformLocation(ctx->programID, "spotlicht.linear");
    uSpot_quadratic = glGetUniformLocation(ctx->programID, "spotlicht.quadratic");

    // Nebel
    uFogColor = glGetUniformLocation(ctx->programID, "nebel.color");
    uFogDensity = glGetUniformLocation(ctx->programID, "nebel.density");
    uFogEnabled = glGetUniformLocation(ctx->programID, "nebel.enabled");

    // ???
    uUseInstancing = glGetUniformLocation(ctx->programID, "useInstancing");

    // Lichter an aus
    glUniform1i(uSun_enabled, 1);
    glUniform1i(uLamp_enabled, 1);
    glUniform1i(uSpot_enabled, 1);

    // Nebel an
    glUniform1i(uFogEnabled, 1);

    // Nebel
    initializeFog(uFogColor, uFogDensity);

    // Richtungslicht
    initializeDirectionalLight(uSun_ambient, uSun_diffuse, uSun_specular);

    // Punktlicht
    initializePointLight(uLamp_ambient, uLamp_diffuse, uLamp_specular, uLamp_linear, uLamp_quadratic);

    // Spotlicht
    initializeSpotLight(uSpot_ambient, uSpot_diffuse, uSpot_specular, uSpot_innerCone, uSpot_outerCone, uSpot_linear, uSpot_quadratic);

    loadMesh("objects/teapot.obj", &teapot);
    loadMesh("objects/column.obj", &column);
    loadMesh("objects/cube.obj", &cube);
    loadMesh("objects/gras.obj", &gras);
    loadMesh("objects/cottage_blender.obj", &cottage);
    loadMesh("objects/Tree1.obj", &tree1);
    loadMesh("objects/Tree2.obj", &tree2);
    loadMesh("objects/Tree3.obj", &tree3);
    loadMesh("objects/slenderman.obj", &slenderman);

    {
        const int count = MAX_TREE_COUNT;
        const float cx = 0.8f, cz = -1.4f, rMin = 10.0f, rMax = 50.0f;

        // Speicher für Model pro Variante (max count; final count unten)
        gTrees[0].Model = (Mat4C *)malloc(count * sizeof(Mat4C));
        gTrees[1].Model = (Mat4C *)malloc(count * sizeof(Mat4C));
        gTrees[2].Model = (Mat4C *)malloc(count * sizeof(Mat4C));
        int n1 = 0, n2 = 0, n3 = 0;

        for (int i = 0; i < count; ++i)
        {
            float t = i * 0.7f;
            float u = (float)i / (float)count;
            float r = rMin + (rMax - rMin) * u;

            float x = cx + r * cosf(t);
            float z = cz + r * sinf(t);
            if (i % 2 == 0)
                x += 0.20f;
            if (i % 3 == 0)
                z -= 0.15f;

            float s = 0.40f + 0.15f * ((i % 4) / 3.0f);
            float rotDeg = 10.0f * (i % 18);

            GLfloat M[16];
            identity(M);
            translate(M, M, (GLfloat[]){x, 0.0f, z});
            rotateY(M, M, rotDeg);
            scale(M, M, (GLfloat[]){s, s, s});

            int v = i % 3;
            if (v == 0)
            {
                memcpy(gTrees[0].Model[n1++].m, M, sizeof(M));
            }
            else if (v == 1)
            {
                memcpy(gTrees[1].Model[n2++].m, M, sizeof(M));
            }
            else
            {
                memcpy(gTrees[2].Model[n3++].m, M, sizeof(M));
            }
        }
        gTrees[0].count = n1;
        gTrees[1].count = n2;
        gTrees[2].count = n3;

        for (int v = 0; v < 3; ++v)
        {
            gTrees[v].capacity = (GLsizeiptr)gTrees[v].count * sizeof(InstanceCPU);
            gTrees[v].cpu = (InstanceCPU *)malloc((size_t)gTrees[v].capacity);
        }

        // ---------- Instance-VBO an die VAOs hängen ----------
        // Hilfsfunktion lokal (bindet Attribute 2..12 mit Divisor 1)
        bindInstanceAttributes(tree1.vao, &gTrees[0].vbo, gTrees[0].capacity);
        bindInstanceAttributes(tree2.vao, &gTrees[1].vbo, gTrees[1].capacity);
        bindInstanceAttributes(tree3.vao, &gTrees[2].vbo, gTrees[2].capacity);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glClearColor(0.17f, 0.19f, 0.21f, 1.0f);

    // Skybox
    initSkybox(ctx);
    glUseProgram(ctx->skyboxProgramID);
    glUniform1i(glGetUniformLocation(ctx->skyboxProgramID, "skybox"), 0);
}

void draw(AppContext *ctx)
{
    glUseProgram(ctx->programID);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Camera and View + Projection Matrix
    GLfloat V[16];
    GLfloat P[16];
    camera(V, P, ctx);

    lichtSchalter(uSun_enabled, uLamp_enabled, uSpot_enabled, ctx->window, &status);
    nebelSchalter(uFogEnabled, ctx->window, &status);

    // Richtungslicht an die Kamera setzten
    setDirectionalLight(uSun_direction, V, 1.0f, -1.0f, -1.0f);

    // Headlight (Spotlicht) immer auf die Kamera setzen
    setSpotLight(uSpot_position, uSpot_direction, V, ctx->eye, ctx->look);

    // Punktlicht-Position JEDES Frame in View-Space updaten
    setPointLight(uLamp_position, V, 2.0f, 2.0f, 5.0f);

    // 1) Graue Säule (gestreckter Cube)
    GLfloat M[16];
    identity(M);
    translate(M, M, (GLfloat[]){0.0f, -0.01f, 0.0f});
    scale(M, M, (GLfloat[]){0.42f, 0.45f, 0.42f});
    setMaterialGrayPillar(ctx);
    drawMeshWithModel(&column, V, P, M, MVLoc, MVPLoc, NormalMLoc);

    // 2) Golderner Teapot
    identity(M);
    translate(M, M, (GLfloat[]){0.0f, 1.6f, 0.0f});
    scale(M, M, (GLfloat[]){0.14f, 0.14f, 0.14f});
    setMaterialPolishedGold(ctx);
    drawMeshWithModel(&teapot, V, P, M, MVLoc, MVPLoc, NormalMLoc);

    // 3) Gras
    identity(M);
    translate(M, M, (GLfloat[]){0.0f, -0.01f, 0.0f});
    scale(M, M, (GLfloat[]){0.42f, 0.45f, 0.42f});
    setMaterialGrass(ctx);
    drawMeshWithModel(&gras, V, P, M, MVLoc, MVPLoc, NormalMLoc);

    // 4) Cottage
    identity(M);
    translate(M, M, (GLfloat[]){25.0f, 0.0f, -15.0f});
    rotateY(M, M, 20);
    scale(M, M, (GLfloat[]){0.5f, 0.5f, 0.5f});
    setMaterialWood(ctx);
    drawMeshWithModel(&cottage, V, P, M, MVLoc, MVPLoc, NormalMLoc);

    // 5) Slenderman
    identity(M);
    translate(M, M, (GLfloat[]){1.4f, 0.0f, -3.2f});
    rotateY(M, M, 180);
    scale(M, M, (GLfloat[]){0.9f, 0.9f, 0.9f});
    setMaterialSlenderman(ctx);
    drawMeshWithModel(&slenderman, V, P, M, MVLoc, MVPLoc, NormalMLoc);

    // 6) Bäume / kleiner Wald — instanced
    {
        glDisable(GL_BLEND);
        glUniform1i(uUseInstancing, 1);
        setMaterialTree(ctx);

        // P-Uniform setzen (nur einmal pro Frame nötig)
        GLint PLoc = glGetUniformLocation(ctx->programID, "P");
        glUniformMatrix4fv(PLoc, 1, GL_FALSE, P);

        updateAndDrawTrees(&tree1, &gTrees[0], V, P, ctx);
        updateAndDrawTrees(&tree2, &gTrees[1], V, P, ctx);
        updateAndDrawTrees(&tree3, &gTrees[2], V, P, ctx);

        glUniform1i(uUseInstancing, 0);
        glEnable(GL_BLEND);
    }

    // 7) Glaswürfel
    identity(M);
    translate(M, M, (GLfloat[]){0.0f, 1.42f, 0.0f});
    scale(M, M, (GLfloat[]){0.24f, 0.24f, 0.24f});
    setMaterialGlass(ctx, 0.18f);
    drawtransparentMeshWithModel(&cube, V, P, M, MVLoc, MVPLoc, NormalMLoc);

    // Skybox
    glCullFace(GL_FRONT);
    glUseProgram(ctx->skyboxProgramID);
    glDepthFunc(GL_LEQUAL);
    GLfloat VP[16];
    GLfloat V_noT[16];
    memcpy(V_noT, V, sizeof(V_noT));
    V_noT[12] = V_noT[13] = V_noT[14] = 0.0f;
    mat4f_mul_mat4f(VP, P, V_noT);
    glUniformMatrix4fv(glGetUniformLocation(ctx->skyboxProgramID, "VP"), 1, GL_FALSE, VP);

    glBindVertexArray(ctx->skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ctx->skyboxTexture);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glCullFace(GL_BACK);

    // Switch back to the normal depth functionx
    glDepthFunc(GL_LESS);
}

int main(void)
{
    chdir("src"); // For different entry-points
    errno = 0;    // Reset if pwd was already src

    AppContext ctx = {0};
    ctx.width = INIT_WINDOW_WIDTH;
    ctx.height = INIT_WINDOW_HEIGHT;

    glfwInit();

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 32);

    ctx.window = glfwCreateWindow(INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT, INIT_WINDOW_TITLE, NULL, NULL);

    if (!ctx.window)
    {
        printf("Error creating window\n");
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(ctx.window, framebuffer_size_callback);
    int fbw, fbh;
    glfwGetFramebufferSize(ctx.window, &fbw, &fbh);
    glViewport(0, 0, fbw, fbh);

    glfwMakeContextCurrent(ctx.window);
    glewInit();

    init(&ctx);

    while (!glfwWindowShouldClose(ctx.window))
    {
        draw(&ctx);
        glfwSwapBuffers(ctx.window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
