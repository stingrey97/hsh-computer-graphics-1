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
#include "UniformLoader.h"
#include "TextureLoader.h"
#include "MatrixTester.h"

static AppContext ctx;

void framebuffer_size_callback(GLFWwindow *window, int cb_width, int cb_height)
{
    ctx.height = cb_height;
    ctx.width = cb_width;
    glViewport(0, 0, cb_width, cb_height);
}

void updateFpsCounter(GLFWwindow* w) {
    static double prev = 0.0;
    static int frames = 0;
    double t = glfwGetTime();
    double dt = t - prev;
    if (dt >= 1.0) {
        char title[128];
        snprintf(title, sizeof(title), "%s  |  FPS: %.1f", INIT_WINDOW_TITLE, frames / dt);
        glfwSetWindowTitle(w, title);
        prev = t;
        frames = 0;
    }
    frames++;
}

int init()
{
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
    ctx.width = fbw;
    ctx.height = fbh;
    glViewport(0, 0, fbw, fbh);

    glfwMakeContextCurrent(ctx.window);
    glewInit();
    glfwSwapInterval(1);

    // verarbeitung der Einagen über Sticky Keys
    glfwSetInputMode(ctx.window, GLFW_STICKY_KEYS, GLFW_TRUE);

    // Kamera
    setVec3(ctx.eye, 1, 0, 0);
    setVec3(ctx.look, 0, 0, 0);
    setVec3(ctx.up, 0, 1, 0);
    initCamera(&ctx);

    // Shader laden + aktivieren
    const char *vertexPath = "shader/vertex/vertexShader.glsl";
    const char *fragmentPath = "shader/fragment/fragmentShader.glsl";
    ctx.programID = loadShaders(vertexPath, fragmentPath);
    glUseProgram(ctx.programID);

    // Uniform-Locations (Hauptprogramm)
    loadUniforms(&ctx);

    // Licht/Nebel Defaults
    glUniform1i(ctx.uSun_enabled, 1);
    glUniform1i(ctx.uLamp_enabled, 1);
    glUniform1i(ctx.uSpot_enabled, 1);
    initializeFog(ctx.uFogColor, ctx.uFogDensity);
    glUniform1i(ctx.uFogEnabled, 1);
    initializeDirectionalLight(ctx.uSun_ambient, ctx.uSun_diffuse, ctx.uSun_specular);
    initializePointLight(ctx.uLamp_ambient, ctx.uLamp_diffuse, ctx.uLamp_specular, ctx.uLamp_linear, ctx.uLamp_quadratic);
    initializeSpotLight(ctx.uSpot_ambient, ctx.uSpot_diffuse, ctx.uSpot_specular, ctx.uSpot_innerCone, ctx.uSpot_outerCone, ctx.uSpot_linear, ctx.uSpot_quadratic);

    // Meshes
    loadMesh("objects/teapot.obj", &ctx.teapot);
    loadMesh("objects/column.obj", &ctx.column);
    loadMesh("objects/cube.obj", &ctx.cube);
    loadMesh("objects/gras.obj", &ctx.gras);
    loadMesh("objects/cottage_blender.obj", &ctx.cottage);
    loadMesh("objects/slenderman.obj", &ctx.slenderman);
    loadMesh("objects/tree/tree1/Baumstamm1.obj", &ctx.baumstamm1);
    loadMesh("objects/tree/tree2/Baumstamm2.obj", &ctx.baumstamm2);
    loadMesh("objects/tree/tree3/Baumstamm3.obj", &ctx.baumstamm3);
    loadMesh("objects/StreetLamp.obj", &ctx.laterne);

    // GL-State
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND); // wird für den Glas-Pass genutzt
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.17f, 0.19f, 0.21f, 1.0f);

    // Skybox initialisieren (eigenes Programm)
    initSkybox(&ctx);
    glUseProgram(ctx.skyboxProgramID);
    glUniform1i(glGetUniformLocation(ctx.skyboxProgramID, "skybox"), 0); // Skybox-Sampler auf TU0

    // Zurück zum Hauptprogramm und Sampler/Defaults setzen
    glUseProgram(ctx.programID);
    glUniform1i(ctx.albedoLoc, 0);
    glUniform1i(ctx.normalLoc, 1);
    glUniform1i(ctx.roughnessLoc, 2);
    glUniform1i(ctx.uEnvMap, 3); // samplerCube → Texture Unit 3
    glUniform1i(ctx.uUseEnvMap, 0);
    glUniform1f(ctx.uIOR, 1.5f);
    glUniform1f(ctx.uEnvStrength, 0.85f);

    // Cubemap (Skybox-Textur) auf TU3 für das Hauptprogramm binden
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ctx.skyboxTexture);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // 2D-Texturen
    ctx.albedoCottage = loadTexture2D("textures/cottage/cottage_diffuse.png", 1);
    ctx.normalCottage = loadTexture2D("textures/cottage/cottage_normal.png", 0);
    ctx.albedoGras = loadTexture2D("textures/gras/grass1-albedo3.png", 1);
    ctx.normalGras = loadTexture2D("textures/gras/grass1-normal1-ogl.png", 0);
    ctx.roughnessGras = loadTexture2D("textures/gras/grass1-rough.png", 0);
    ctx.albedoBaum1und2 = loadTexture2D("textures/trees/tree1/BarkDecidious0143_5_S.jpg", 1);
    ctx.albedoBaum3 = loadTexture2D("textures/trees/tree3/BarkDecidious0194_7_S.jpg", 1);
    ctx.normalBaum = loadTexture2D("textures/trees/tree1/tree_bark_normal_ogl.png", 0);
    ctx.roughBaum = loadTexture2D("textures/trees/tree1/tree_bark_roughness.png", 0);
    ctx.albedoGlas = loadTexture2D("textures/glas/glass_albedo_red_a18.png", 1);
    ctx.normalGlas = loadTexture2D("textures/glas/glass_normal_flat_ogl.png", 0);
    ctx.roughGlas = loadTexture2D("textures/glas/glass_roughness_glossy_012.png", 0);
    ctx.albedoColumn = loadTexture2D("textures/column/Column_Albedo_fixed.png", 1);
    ctx.normalColumn = loadTexture2D("textures/column/column_normal.002.png", 0);
    ctx.roughColumn = loadTexture2D("textures/column/column_roughness.png", 0);
    ctx.albedoTeapot = loadTexture2D("textures/teapot/1df5a76d-fb2d-45d4-ae28-7265782ed03b.png", 1);
    ctx.albedoSlenderman = loadTexture2D("textures/slenderman/Slenderman_Albedo.png", 1);
    ctx.normalSlenderman = loadTexture2D("textures/slenderman/Slender_Normal.png", 0);
    ctx.roughSlenderman = loadTexture2D("textures/slenderman/Slender_Roughness.png", 0);
    ctx.albedoLaterne = loadTexture2D("textures/laterne/Lantern_Albedo.png", 1);
    ctx.normalLaterne = loadTexture2D("textures/laterne/Lantern_Normal.png", 0);
    ctx.roughLaterne = loadTexture2D("textures/laterne/Lantern_Roughness.png", 0);

    return 0;
}

void draw()
{
    glUseProgram(ctx.programID);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // View/Projection
    GLfloat V[16], P[16];
    camera(V, P, &ctx);

    // obere linke 3x3 der View-Matrix an den Shader (für Env-Map)
    GLfloat viewRot[9] = {
        V[0], V[1], V[2],
        V[4], V[5], V[6],
        V[8], V[9], V[10]};
    glUniformMatrix3fv(ctx.uViewRotLoc, 1, GL_FALSE, viewRot);

    // Env-Defaults pro Frame
    glUniform1i(ctx.uUseEnvMap, 0);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ctx.skyboxTexture);

    // UI/States
    lichtSchalter(ctx.uSun_enabled, ctx.uLamp_enabled, ctx.uSpot_enabled, ctx.window, &ctx.status);
    nebelSchalter(ctx.uFogEnabled, ctx.window, &ctx.status);
    vollbildschalter(ctx.window, &ctx.status);

    // Lichter setzen
    setDirectionalLight(ctx.uSun_direction, V, 1.0f, -1.0f, -1.0f);
    setSpotLight(ctx.uSpot_position, ctx.uSpot_direction, V, ctx.eye, ctx.look);
    setPointLight(ctx.uLamp_position, V, 0.0f, 5.0f, 0.0f);

    GLfloat M[16];

    if (glfwGetKey(ctx.window, GLFW_KEY_7) == GLFW_PRESS)
    {
        ctx.reflect = ctx.reflect == 0 ? 1 : 0;
    }

    // ---------- Opaque rendering ----------
    glDisable(GL_BLEND);

    // Test: Make everything reflective
    glUniform1i(ctx.uUseEnvMap, ctx.reflect);

    // Cottage
    glUniform2f(ctx.uvScale, 1.0f, 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ctx.albedoCottage);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ctx.normalCottage);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, ctx.roughnessCottage);
    identity(M);
    translate(M, M, (GLfloat[]){25.0f, 0.0f, -15.0f});
    rotateY(M, M, 20);
    scale(M, M, (GLfloat[]){0.5f, 0.5f, 0.5f});
    setMaterialWood(&ctx);
    drawMeshWithModel(&ctx.cottage, V, P, M, ctx.MVLoc, ctx.MVPLoc, ctx.NormalMLoc);

    // Gras
    glUniform2f(ctx.uvScale, 100.0f, 100.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ctx.albedoGras);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ctx.normalGras);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, ctx.roughnessGras);
    identity(M);
    translate(M, M, (GLfloat[]){0.0f, -0.01f, 0.0f});
    scale(M, M, (GLfloat[]){0.42f, 0.45f, 0.42f});
    setMaterialGrass(&ctx);
    drawMeshWithModel(&ctx.gras, V, P, M, ctx.MVLoc, ctx.MVPLoc, ctx.NormalMLoc);

    // Säule
    glUniform2f(ctx.uvScale, 1.0f, 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ctx.albedoColumn);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ctx.normalColumn);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, ctx.roughColumn);
    identity(M);
    translate(M, M, (GLfloat[]){0.0f, -0.01f, 0.0f});
    scale(M, M, (GLfloat[]){0.42f, 0.45f, 0.42f});
    setMaterialGrayPillar(&ctx);
    drawMeshWithModel(&ctx.column, V, P, M, ctx.MVLoc, ctx.MVPLoc, ctx.NormalMLoc);

    // Laterne
    glUniform2f(ctx.uvScale, 1.0f, 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ctx.albedoLaterne);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ctx.normalLaterne);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, ctx.roughLaterne);
    identity(M);
    translate(M, M, (GLfloat[]){-3.5f, 0.0f, 0.0f});
    scale(M, M, (GLfloat[]){0.5f, 0.5f, 0.5f});
    setMaterialGrayPillar(&ctx);
    drawMeshWithModel(&ctx.laterne, V, P, M, ctx.MVLoc, ctx.MVPLoc, ctx.NormalMLoc);

    // Teapot
    glDisable(GL_CULL_FACE);
    glUniform2f(ctx.uvScale, 1.0f, 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ctx.albedoTeapot);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ctx.normalGlas);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, ctx.roughGlas);
    identity(M);
    static float teapotAngle = 0.0f;
    teapotAngle += 0.5f;
    if (teapotAngle >= 360.0f)
        teapotAngle -= 360.0f;
    rotateY(M, M, teapotAngle);
    translate(M, M, (GLfloat[]){0.0f, 1.6f, 0.0f});
    scale(M, M, (GLfloat[]){0.14f, 0.14f, 0.14f});
    setMaterialPolishedGold(&ctx);
    drawMeshWithModel(&ctx.teapot, V, P, M, ctx.MVLoc, ctx.MVPLoc, ctx.NormalMLoc);
    glEnable(GL_CULL_FACE);

    // Slenderman (opak)
    setMaterialGrayPillar(&ctx);
    drawSlenderman(M, V, P, ctx.MVLoc, ctx.MVPLoc, ctx.NormalMLoc,
                   ctx.albedoSlenderman, ctx.normalSlenderman, ctx.roughSlenderman,
                   &ctx.slenderman, ctx.uvScale, ctx.eye);

    // Wald
    setMaterialWood(&ctx);
    drawForrest(MAX_TREE_COUNT, M, V, P, ctx.MVLoc, ctx.MVPLoc, ctx.NormalMLoc,
                ctx.albedoBaum1und2, ctx.albedoBaum3, ctx.normalBaum, ctx.roughBaum,
                &ctx.baumstamm1, &ctx.baumstamm2, &ctx.baumstamm3, ctx.uvScale);

    // ---------- Skybox ----------
    drawSkybox(&ctx, V, P);

    // ---------- Transparent rendering (Glass) ----------
    glUseProgram(ctx.programID);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ctx.skyboxTexture);
    glEnable(GL_BLEND);

    glUniform1i(ctx.uUseEnvMap, 1);
    glUniform1f(ctx.uIOR, 1.5f);
    glUniform1f(ctx.uEnvStrength, 0.85f);

    glUniform2f(ctx.uvScale, 1.0f, 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ctx.albedoGlas);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ctx.normalGlas);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, ctx.roughGlas);
    identity(M);
    translate(M, M, (GLfloat[]){0.0f, 1.42f, 0.0f});
    scale(M, M, (GLfloat[]){0.24f, 0.24f, 0.24f});
    setMaterialGlass(&ctx, 0.18f);
    drawtransparentMeshWithModel(&ctx.cube, V, P, M, ctx.MVLoc, ctx.MVPLoc, ctx.NormalMLoc);

    glUniform1i(ctx.uUseEnvMap, 0);
}

int main(void)
{
    chdir("src");
    assert(init() == 0);

    // Kontext sicher binden und VSync setzen
    glfwMakeContextCurrent(ctx.window);

    // Zwei Zeitmess-Queries (Ping-Pong)
    GLuint q[2];
    glGenQueries(2, q);

    testeFunktionen();

    // FPS-Variablen
    double t0 = glfwGetTime();
    int    frames = 0;

    // Ping-Pong-Index und Flag, ob ein Vorframe existiert
    int cur = 0;
    int have_prev = 0;

    while (!glfwWindowShouldClose(ctx.window))
    {
        // --- GPU-Time dieses Frames messen (GL_TIME_ELAPSED) ---
        glBeginQuery(GL_TIME_ELAPSED, q[cur]);

        draw(); // deine Render-Calls

        glEndQuery(GL_TIME_ELAPSED);

        // Präsentieren + Events
        glfwSwapBuffers(ctx.window);
        glfwPollEvents();

        // --- FPS live berechnen ---
        frames++;
        double now = glfwGetTime();
        double fps = frames / (now - t0);

        // --- GPU-Zeit vom Vorframe blockierend abholen ---
        double gpu_ms = -1.0;
        if (have_prev) {
            // GL_QUERY_RESULT blockiert, bis Resultat vorliegt -> garantiert ein Wert
            GLuint64 elapsed_ns = 0;
            glGetQueryObjectui64v(q[1 - cur], GL_QUERY_RESULT, &elapsed_ns);
            gpu_ms = (double)elapsed_ns / 1e6; // ns -> ms
        } else {
            have_prev = 1; // nach dem ersten Frame steht ab jetzt ein Vorframe-Ergebnis bereit
        }

        // Fenstertitel setzen (immer FPS; GPU-ms sobald vorhanden)
        char title[160];
        if (gpu_ms >= 0.0)
            snprintf(title, sizeof(title), "%s | FPS: %.1f | GPU: %.3f ms",
                     INIT_WINDOW_TITLE, fps, gpu_ms);
        else
            snprintf(title, sizeof(title), "%s | FPS: %.1f | GPU: …",
                     INIT_WINDOW_TITLE, fps);
        // glfwSetWindowTitle(ctx.window, title);

        // Ping-Pong umschalten
        cur = 1 - cur;

        // FPS-Fenster jede Sekunde neu starten (stabilere Anzeige)
        if (now - t0 >= 1.0) {
            t0 = now;
            frames = 0;
        }
    }

    glDeleteQueries(2, q);
    glfwTerminate();
    return 0;
}
