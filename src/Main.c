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

    // Load shader program
    const char *vertexPath = "shader/vertex/vertexShader.glsl";
    const char *fragmentPath = "shader/fragment/fragmentShader.glsl";
    ctx->programID = loadShaders(vertexPath, fragmentPath);
    glUseProgram(ctx->programID);

    // Load Uniforms
    loadUniforms(ctx);

    // Toggle lights
    glUniform1i(ctx->uSun_enabled, 1);
    glUniform1i(ctx->uLamp_enabled, 1);
    glUniform1i(ctx->uSpot_enabled, 1);

    // Toggle fog
    initializeFog(ctx->uFogColor, ctx->uFogDensity);
    glUniform1i(ctx->uFogEnabled, 1);

    // Richtungslicht
    initializeDirectionalLight(ctx->uSun_ambient, ctx->uSun_diffuse, ctx->uSun_specular);

    // Punktlicht
    initializePointLight(ctx->uLamp_ambient, ctx->uLamp_diffuse, ctx->uLamp_specular, ctx->uLamp_linear, ctx->uLamp_quadratic);

    // Spotlicht
    initializeSpotLight(ctx->uSpot_ambient, ctx->uSpot_diffuse, ctx->uSpot_specular, ctx->uSpot_innerCone, ctx->uSpot_outerCone, ctx->uSpot_linear, ctx->uSpot_quadratic);

    // Load meshes in scene
    loadMesh("objects/teapot.obj", &ctx->teapot);
    loadMesh("objects/column.obj", &ctx->column);
    loadMesh("objects/cube.obj", &ctx->cube);
    loadMesh("objects/gras.obj", &ctx->gras);
    loadMesh("objects/cottage_blender.obj", &ctx->cottage);
    loadMesh("objects/Tree1.obj", &ctx->tree1);
    loadMesh("objects/Tree2.obj", &ctx->tree2);
    loadMesh("objects/Tree3.obj", &ctx->tree3);
    loadMesh("objects/slenderman.obj", &ctx->slenderman);

    // Init OpenGL
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

    // View and  Projection matrix
    GLfloat V[16];
    GLfloat P[16];
    camera(V, P, ctx);

    lichtSchalter(ctx->uSun_enabled, ctx->uLamp_enabled, ctx->uSpot_enabled, ctx->window, &ctx->lightStatus);
    nebelSchalter(ctx->uFogEnabled, ctx->window, &ctx->lightStatus);

    // Richtungslicht an die Kamera setzten
    setDirectionalLight(ctx->uSun_direction, V, 1.0f, -1.0f, -1.0f);

    // Headlight (Spotlicht) immer auf die Kamera setzen
    setSpotLight(ctx->uSpot_position, ctx->uSpot_direction, V, ctx->eye, ctx->look);

    // Punktlicht-Position JEDES Frame in View-Space updaten
    setPointLight(ctx->uLamp_position, V, 2.0f, 2.0f, 5.0f);

    // 1) Graue Säule (gestreckter Cube)
    GLfloat M[16];
    identity(M);
    translate(M, M, (GLfloat[]){0.0f, -0.01f, 0.0f});
    scale(M, M, (GLfloat[]){0.42f, 0.45f, 0.42f});
    setMaterialGrayPillar(ctx);
    drawMeshWithModel(&ctx->column, V, P, M, ctx->MVLoc, ctx->MVPLoc, ctx->NormalMLoc);

    // 2) Golderner Teapot
    identity(M);
    translate(M, M, (GLfloat[]){0.0f, 1.6f, 0.0f});
    scale(M, M, (GLfloat[]){0.14f, 0.14f, 0.14f});
    setMaterialPolishedGold(ctx);
    drawMeshWithModel(&ctx->teapot, V, P, M, ctx->MVLoc, ctx->MVPLoc, ctx->NormalMLoc);

    // 3) Gras
    identity(M);
    translate(M, M, (GLfloat[]){0.0f, -0.01f, 0.0f});
    scale(M, M, (GLfloat[]){0.42f, 0.45f, 0.42f});
    setMaterialGrass(ctx);
    drawMeshWithModel(&ctx->gras, V, P, M, ctx->MVLoc, ctx->MVPLoc, ctx->NormalMLoc);

    // 4) Cottage
    identity(M);
    translate(M, M, (GLfloat[]){25.0f, 0.0f, -15.0f});
    rotateY(M, M, 20);
    scale(M, M, (GLfloat[]){0.5f, 0.5f, 0.5f});
    setMaterialWood(ctx);
    drawMeshWithModel(&ctx->cottage, V, P, M, ctx->MVLoc, ctx->MVPLoc, ctx->NormalMLoc);

    // 5) Slenderman
    identity(M);
    translate(M, M, (GLfloat[]){1.4f, 0.0f, -3.2f});
    rotateY(M, M, 180);
    scale(M, M, (GLfloat[]){0.9f, 0.9f, 0.9f});
    setMaterialSlenderman(ctx);
    drawMeshWithModel(&ctx->slenderman, V, P, M, ctx->MVLoc, ctx->MVPLoc, ctx->NormalMLoc);

    // 6) Bäume
    // TODO

    // 7) Glaswürfel
    identity(M);
    translate(M, M, (GLfloat[]){0.0f, 1.42f, 0.0f});
    scale(M, M, (GLfloat[]){0.24f, 0.24f, 0.24f});
    setMaterialGlass(ctx, 0.18f);
    drawtransparentMeshWithModel(&ctx->cube, V, P, M, ctx->MVLoc, ctx->MVPLoc, ctx->NormalMLoc);

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
