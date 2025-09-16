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

static AppContext ctx;

void framebuffer_size_callback(GLFWwindow *window, int cb_width, int cb_height)
{
    context.height = cb_height;
    context.width = cb_width;
    glViewport(0, 0, cb_width, cb_height);
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
    context.width = fbw;
    context.height = fbh;
    glViewport(0, 0, fbw, fbh);

    glfwMakeContextCurrent(ctx.window);
    glewInit();

    // Initial camera
    setVec3(ctx.eye, 1, 0, 0);
    setVec3(ctx.look, 0, 0, 0);
    setVec3(ctx.up, 0, 1, 0);
    initCamera(&ctx);

    // Load shader program
    const char *vertexPath = "shader/vertex/vertexShader.glsl";
    const char *fragmentPath = "shader/fragment/fragmentShader.glsl";
    ctx.programID = loadShaders(vertexPath, fragmentPath);
    glUseProgram(ctx.programID);

    // Load Uniforms
    loadUniforms(&ctx);

    // Toggle lights
    glUniform1i(ctx.uSun_enabled, 1);
    glUniform1i(ctx.uLamp_enabled, 1);
    glUniform1i(ctx.uSpot_enabled, 1);

    // Toggle fog
    initializeFog(ctx.uFogColor, ctx.uFogDensity);
    glUniform1i(ctx.uFogEnabled, 1);

    // Richtungslicht
    initializeDirectionalLight(ctx.uSun_ambient, ctx.uSun_diffuse, ctx.uSun_specular);

    // Punktlicht
    initializePointLight(ctx.uLamp_ambient, ctx.uLamp_diffuse, ctx.uLamp_specular, ctx.uLamp_linear, ctx.uLamp_quadratic);

    // Spotlicht
    initializeSpotLight(ctx.uSpot_ambient, ctx.uSpot_diffuse, ctx.uSpot_specular, ctx.uSpot_innerCone, ctx.uSpot_outerCone, ctx.uSpot_linear, ctx.uSpot_quadratic);

    // Load meshes in scene
    loadMesh("objects/teapot.obj", &teapot);
    loadMesh("objects/column.obj", &column);
    loadMesh("objects/cube.obj", &cube);
    loadMesh("objects/gras.obj", &gras);
    loadMesh("objects/cottage_blender.obj", &cottage);
    loadMesh("objects/slenderman.obj", &slenderman);
    loadMesh("objects/tree/tree1/Baumstamm1.obj", &baumstamm1);
    loadMesh("objects/tree/tree2/Baumstamm2.obj", &baumstamm2);
    loadMesh("objects/tree/tree3/Baumstamm3.obj", &baumstamm3);
    loadMesh("objects/StreetLamp.obj", &laterne);

    // Init OpenGL
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glClearColor(0.17f, 0.19f, 0.21f, 1.0f);

    // Skybox
    initSkybox(&ctx);
    glUseProgram(ctx.skyboxProgramID);
    glUniform1i(glGetUniformLocation(ctx.skyboxProgramID, "skybox"), 0);

    // Env map binding for main program
    glUseProgram(ctx.programID);
    ctx.uEnvMap = glGetUniformLocation(ctx.programID, "uEnvMap");
    ctx.uUseEnvMap = glGetUniformLocation(ctx.programID, "uUseEnvMap");
    ctx.uIOR = glGetUniformLocation(ctx.programID, "uIOR");
    ctx.uEnvStrength = glGetUniformLocation(ctx.programID, "uEnvStrength");
    ctx.uViewRotLoc = glGetUniformLocation(ctx.programID, "uViewRot");

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ctx.skyboxTexture);
    glUniform1i(ctx.uEnvMap, 1);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // defaults
    glUniform1f(ctx.uIOR, 1.5f);
    glUniform1f(ctx.uEnvStrength, 0.85f);
    glUniform1i(ctx.uUseEnvMap, 0);

    // Texturkanäle
    glUniform1i(albedoLoc, 0);
    glUniform1i(normalLoc, 1);
    glUniform1i(roughnessLoc, 2);

    // Texturen laden
    // Cottage
    ctx.albedoCottage = loadTexture2D("textures/cottage/cottage_diffuse.png", 1);
    ctx.normalCottage = loadTexture2D("textures/cottage/cottage_normal.png", 0);
    // gras
    ctx.albedoGras = loadTexture2D("textures/gras/grass1-albedo3.png", 1);
    ctx.normalGras = loadTexture2D("textures/gras/grass1-normal1-ogl.png", 0);
    ctx.roughnessGras = loadTexture2D("textures/gras/grass1-rough.png", 0);
    // Bäume
    ctx.albedoBaum1und2 = loadTexture2D("textures/trees/tree1/BarkDecidious0143_5_S.jpg", 1);
    ctx.albedoBaum3 = loadTexture2D("textures/trees/tree3/BarkDecidious0194_7_S.jpg", 1);
    ctx.normalBaum = loadTexture2D("textures/trees/tree1/tree_bark_normal_ogl.png", 0);
    ctx.roughBaum = loadTexture2D("textures/trees/tree1/tree_bark_roughness.png", 0);
    // Glas
    ctx.albedoGlas = loadTexture2D("textures/glas/glass_albedo_red_a18.png", 1);
    ctx.normalGlas = loadTexture2D("textures/glas/glass_normal_flat_ogl.png", 0);
    ctx.roughGlas = loadTexture2D("textures/glas/glass_roughness_glossy_012.png", 0);
    // Coulmn
    ctx.albedoColumn = loadTexture2D("textures/column/Column_Albedo_fixed.png", 1);
    ctx.normalColumn = loadTexture2D("textures/column/column_normal.002.png", 0);
    ctx.roughColumn = loadTexture2D("textures/column/column_roughness.png", 0);
    // Teapot
    ctx.albedoTeapot = loadTexture2D("textures/teapot/1df5a76d-fb2d-45d4-ae28-7265782ed03b.png", 1);
    // Slenderman
    ctx.albedoSlenderman = loadTexture2D("", 1);
    ctx.normalSlenderman = loadTexture2D("", 0);
    ctx.roughSlenderman = loadTexture2D("", 0);
    // Laterne
    ctx.albedoLaterne = loadTexture2D("", 1);
    ctx.normalLaterne = loadTexture2D("", 0);
    ctx.roughLaterne = loadTexture2D("", 0);

    return 0;
}

void draw()
{
    glUseProgram(ctx.programID);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // View and  Projection matrix
    GLfloat V[16];
    GLfloat P[16];
    camera(V, P, &ctx);

    // viewMatrix ist deine 4x4-View-Matrix (column-major, OpenGL-konform)
    GLfloat viewRot[9] = {
        V[0], V[1], V[2],
        V[4], V[5], V[6],
        V[8], V[9], V[10]};

    // Achtung: Das sind die oberen linken 3x3-Elemente ohne Translation.
    glUniformMatrix3fv(ctx.uViewRotLoc, 1, GL_FALSE, viewRot);

    lichtSchalter(ctx.uSun_enabled, ctx.uLamp_enabled, ctx.uSpot_enabled, ctx.window, &ctx.lightStatus);
    nebelSchalter(ctx.uFogEnabled, ctx.window, &ctx.lightStatus);
    vollbildschalter(context.window, &status);

    // Richtungslicht an die Kamera setzten
    setDirectionalLight(ctx.uSun_direction, V, 1.0f, -1.0f, -1.0f);

    // Headlight (Spotlicht) immer auf die Kamera setzen
    setSpotLight(ctx.uSpot_position, ctx.uSpot_direction, V, ctx.eye, ctx.look);

    // Punktlicht-Position JEDES Frame in View-Space updaten
    setPointLight(ctx.uLamp_position, V, 0.0f, 5.0f, 0.0f);

    GLfloat M[16];

    // 1) Graue Säule (gestreckter Cube)
    // Cottage
    glUniform2f(uvScale, 1.0f, 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, albedoCottage);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalCottage);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, roughnessCottage);
    identity(M);
    translate(M, M, (GLfloat[]){25.0f, 0.0f, -15.0f});
    rotateY(M, M, 20);
    scale(M, M, (GLfloat[]){0.5f, 0.5f, 0.5f});
    setMaterialWood();
    drawMeshWithModel(&cottage, V, P, M, MVLoc, MVPLoc, NormalMLoc);

    // Gras
    glUniform2f(uvScale, 100.0f, 100.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, albedoGras);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalGras);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, roughnessGras);
    identity(M);
    translate(M, M, (GLfloat[]){0.0f, -0.01f, 0.0f});
    scale(M, M, (GLfloat[]){0.42f, 0.45f, 0.42f});
    setMaterialGrass();
    drawMeshWithModel(&gras, V, P, M, MVLoc, MVPLoc, NormalMLoc);

    // Graue Säule
    glUniform2f(uvScale, 1.0f, 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, albedoColumn);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalColumn);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, roughColumn);
    identity(M);
    translate(M, M, (GLfloat[]){0.0f, -0.01f, 0.0f});
    scale(M, M, (GLfloat[]){0.42f, 0.45f, 0.42f});
    setMaterialGrayPillar();
    drawMeshWithModel(&column, V, P, M, MVLoc, MVPLoc, NormalMLoc);

    // Laterne
    glUniform2f(uvScale, 1.0f, 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, albedoLaterne);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalLaterne);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, roughLaterne);
    identity(M);
    translate(M, M, (GLfloat[]){-3.5f, 0.0f, 0.0f});
    scale(M, M, (GLfloat[]){0.5f, 0.5f, 0.5f});
    setMaterialGrayPillar();
    drawMeshWithModel(&laterne, V, P, M, MVLoc, MVPLoc, NormalMLoc);

    // Golderner Teapot
    glUniform2f(uvScale, 1.0f, 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, albedoTeapot);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalGlas);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, roughGlas);
    identity(M);
    translate(M, M, (GLfloat[]){0.0f, 1.6f, 0.0f});
    scale(M, M, (GLfloat[]){0.14f, 0.14f, 0.14f});
    setMaterialPolishedGold();
    drawMeshWithModel(&teapot, V, P, M, MVLoc, MVPLoc, NormalMLoc);

    // Slenderman
    setMaterialGrayPillar();
    drawSlenderman(M, V, P, MVLoc, MVPLoc, NormalMLoc, albedoSlenderman, normalSlenderman, roughSlenderman, &slenderman, uvScale, context.eye);

    // Forrest
    setMaterialWood();
    drawForrest(80, M, V, P, MVLoc, MVPLoc, NormalMLoc, albedoBaum1und2, albedoBaum3, normalBaum, roughBaum,
                &baumstamm1, &baumstamm2, &baumstamm3, uvScale);
    // Skybox
    drawSkybox(&ctx, V, P);

    // Glaswürfel
    glUniform1i(ctx.uEnvMap, 1);
    glUseProgram(context.programID);
    glUniform2f(uvScale, 1.0f, 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, albedoGlas);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalGlas);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, roughGlas);
    identity(M);
    translate(M, M, (GLfloat[]){0.0f, 1.42f, 0.0f});
    scale(M, M, (GLfloat[]){0.24f, 0.24f, 0.24f});
    setMaterialGlass(0.18f);
    drawtransparentMeshWithModel(&cube, V, P, M, MVLoc, MVPLoc, NormalMLoc);
    glUniform1i(ctx.uEnvMap, 0);
}

int main(void)
{
    // For different entry-points
    chdir("src");
    errno = 0;

    assert(init() == 0);

    while (!glfwWindowShouldClose(ctx.window))
    {
        draw();
        glfwSwapBuffers(ctx.window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
