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

    // 6) Bäume / kleiner Wald (einfacher Ring, wechselnde Modelle)
    {
        const int count = 50;     // Anzahl Bäume
        const float cx = 0.8f;    // Mittelpunkt X (z. B. Nähe Cottage)
        const float cz = -1.4f;   // Mittelpunkt Z
        const float rMin = 10.0f; // innerer Radius (freie Fläche)
        const float rMax = 50.0f; // äußerer Radius (Waldgröße)

        for (int i = 0; i < count; ++i)
        {
            // Winkel & Radius (einfach)
            float t = i * 0.7f;                 // Schrittwinkel
            float u = (float)i / (float)count;  // 0..1
            float r = rMin + (rMax - rMin) * u; // wächst nach außen

            // Position + kleine Offsets (gegen perfekte Ordnung)
            float x = cx + r * cosf(t);
            float z = cz + r * sinf(t);
            if (i % 2 == 0)
                x += 0.20f;
            if (i % 3 == 0)
                z -= 0.15f;

            // Größe & Rotation leicht variieren
            float s = 0.40f + 0.15f * ((i % 4) / 3.0f); // ~0.40..0.55
            float rotDeg = 10.0f * (i % 18);            // 0..170° in 10°-Schritten
            // Falls deine rotateY RADIAN erwartet, nimm:
            // float rotRad = rotDeg * (float)M_PI / 180.0f;

            // Variante wählen: 0 -> tree1, 1 -> tree2, 2 -> tree3
            Mesh *tree = &tree1;
            if (i % 3 == 1)
                tree = &tree2;
            else if (i % 3 == 2)
                tree = &tree3;

            // Model-Matrix und Draw
            GLfloat M[16];
            identity(M);
            translate(M, M, (GLfloat[]){x, 0.0f, z});
            rotateY(M, M, rotDeg); // oder rotateY(M,M,rotRad) s.o.
            scale(M, M, (GLfloat[]){s, s, s});

            setMaterialTree(ctx);
            drawMeshWithModel(tree, V, P, M, MVLoc, MVPLoc, NormalMLoc);
        }
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
