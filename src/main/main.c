// Standard libs
#include <stdio.h>
#include <math.h>

// GL libs
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Third party libs
#define STB_IMAGE_IMPLEMENTATION
#include "../functions/stb_image.h"

// Own libs
#include "../functions/AppContext.h"
#include "../functions/matrixUtils.h"
#include "../functions/loadShaders.h"
#include "../functions/mesh.h"
#include "../functions/drawUtils.h"
#include "../functions/lightUtils.h"
#include "../functions/camera.h"
#include "../functions/expirmente.h"
#include "../functions/fogUtils.h"
#include "../functions/Skybox.h"
#include "../functions/Textures.h"

// Constants
#define INIT_WINDOW_TITLE "OpenGL Program"
#define INIT_WINDOW_WIDTH 1024
#define INIT_WINDOW_HEIGHT 768

// Material (struct Material material)
GLint uMat_emission, uMat_ambient, uMat_diffuse, uMat_specular, uMat_shininess;

// Richtungslicht (struct lightSourceR richtungslicht)
GLint uSun_enabled, uSun_direction, uSun_ambient, uSun_diffuse, uSun_specular;

// Punktlicht (struct lightSourceP punktlicht)
GLint uLamp_enabled, uLamp_position, uLamp_ambient, uLamp_diffuse, uLamp_specular, uLamp_linear, uLamp_quadratic;

// Spotlicht (struct lightSourceS spotlicht)
GLint uSpot_enabled, uSpot_position, uSpot_direction, uSpot_innerCone, uSpot_outerCone, uSpot_ambient, uSpot_diffuse, uSpot_specular, uSpot_linear, uSpot_quadratic;

// Texture uniform
GLint albedoLoc, normalLoc, roughnessLoc, uvScale;

// textures
GLuint albedoCottage, normalCottage, roughnessCottage;
GLuint albedoColumn, normalColumn, roughColumn;
GLuint albedoGras, normalGras, roughnessGras;
GLuint albedoBaum1und2, albedoBaum3, normalBaum, roughBaum;
GLuint albedoGlas, normalGlas, roughGlas;
GLuint albedoTeapot;

// Nebel (struct fog nebel)
GLint uFogColor, uFogDensity, uFogEnabled;

// Uniform Standorte
GLint MVLoc, MVPLoc, NormalMLoc;

// Mesh
Mesh cube, teapot, column, gras, cottage, baumstamm1, baumstamm2, baumstamm3, slenderman;

// Licht Status
Status status = {1, 1, 1};

void framebuffer_size_callback(GLFWwindow *window, int cb_width, int cb_height)
{
    glViewport(0, 0, cb_width, cb_height);
}

// TODO: MATERIALS.C
void setMaterialPolishedGold()
{
    glUniform4f(uMat_emission, 0, 0, 0, 1);
    glUniform4f(uMat_ambient, 0.25f, 0.22f, 0.06f, 1);
    glUniform4f(uMat_diffuse, 0.35f, 0.31f, 0.09f, 1); // alpha=1 (opak)
    glUniform4f(uMat_specular, 0.80f, 0.72f, 0.21f, 1);
    glUniform1f(uMat_shininess, 83.2f);
}

void setMaterialGrayPillar()
{
    glUniform4f(uMat_emission, 0, 0, 0, 1);
    glUniform4f(uMat_ambient, 0.20f, 0.20f, 0.20f, 1);
    glUniform4f(uMat_diffuse, 0.50f, 0.50f, 0.50f, 1); // neutral grau, opak
    glUniform4f(uMat_specular, 0.10f, 0.10f, 0.10f, 1);
    glUniform1f(uMat_shininess, 16.0f);
}

void setMaterialGlass(float alpha)
{
    glUniform4f(uMat_emission, 0, 0, 0, 1);
    glUniform4f(uMat_ambient, 0.02f, 0.02f, 0.03f, 1);
    glUniform4f(uMat_diffuse, 1.0, 0.00f, 1.0f, alpha); // Tönung + Alpha
    glUniform4f(uMat_specular, 1, 1, 1, 1);
    glUniform1f(uMat_shininess, 256.0f);
}
void setMaterialGrass()
{
    glUniform4f(uMat_emission, 0.0f, 0.0f, 0.0f, 1.0f);
    glUniform4f(uMat_ambient, 0.05f, 0.20f, 0.05f, 1.0f);
    glUniform4f(uMat_diffuse, 0.10f, 0.50f, 0.10f, 1.0f);
    glUniform4f(uMat_specular, 0.02f, 0.05f, 0.02f, 1.0f);
    glUniform1f(uMat_shininess, 5.0f);
}
void setMaterialWood()
{
    glUniform4f(uMat_emission, 0.00f, 0.00f, 0.00f, 1.0f);
    glUniform4f(uMat_ambient, 0.08f, 0.05f, 0.03f, 1.0f);  // dunkles Grundlicht
    glUniform4f(uMat_diffuse, 0.45f, 0.30f, 0.15f, 1.0f);  // warmes Braun (Holz)
    glUniform4f(uMat_specular, 0.04f, 0.03f, 0.02f, 1.0f); // kaum Glanz
    glUniform1f(uMat_shininess, 12.0f);                    // matt
}

void setMaterialTree()
{
    glUniform4f(uMat_emission, 0.00f, 0.00f, 0.00f, 1.0f);
    glUniform4f(uMat_ambient, 0.06f, 0.08f, 0.06f, 1.0f);
    glUniform4f(uMat_diffuse, 0.22f, 0.35f, 0.18f, 1.0f);  // dunkles Blattgrün
    glUniform4f(uMat_specular, 0.03f, 0.03f, 0.03f, 1.0f); // kaum Glanz
    glUniform1f(uMat_shininess, 10.0f);
}
void setMaterialSlenderman()
{
    glUniform4f(uMat_emission, 0.00f, 0.00f, 0.00f, 1.0f);
    glUniform4f(uMat_ambient, 0.02f, 0.02f, 0.02f, 1.0f);  // sehr dunkles Grundlicht
    glUniform4f(uMat_diffuse, 0.03f, 0.03f, 0.03f, 1.0f);  // fast schwarz
    glUniform4f(uMat_specular, 0.12f, 0.12f, 0.12f, 1.0f); // feiner Glanz für Silhouette
    glUniform1f(uMat_shininess, 64.0f);
}

void init(AppContext *context)
{
    // Initial camera
    setVec3(context->eye, 5, 0, 0);
    setVec3(context->look, 0, 0, 0);
    setVec3(context->up, 0, 1, 0);

    initCamera(context);

    const char *vertexPath = "shader/vertex/vertexShader.glsl";
    const char *fragmentPath = "shader/fragment/fragmentShader.glsl";

    context->programID = loadShaders(vertexPath, fragmentPath);
    glUseProgram(context->programID);

    // einmal die Location holen Matrizen
    MVLoc = glGetUniformLocation(context->programID, "MV");
    MVPLoc = glGetUniformLocation(context->programID, "MVP");
    NormalMLoc = glGetUniformLocation(context->programID, "NormalM");

    // neue Uniforms für Licht/Material
    // Material
    uMat_emission = glGetUniformLocation(context->programID, "material.emission");
    uMat_ambient = glGetUniformLocation(context->programID, "material.ambient");
    uMat_diffuse = glGetUniformLocation(context->programID, "material.diffuse");
    uMat_specular = glGetUniformLocation(context->programID, "material.specular");
    uMat_shininess = glGetUniformLocation(context->programID, "material.shininess");

    // Richtungslicht
    uSun_enabled = glGetUniformLocation(context->programID, "richtungslicht.enabled");
    uSun_direction = glGetUniformLocation(context->programID, "richtungslicht.direction");
    uSun_ambient = glGetUniformLocation(context->programID, "richtungslicht.ambient");
    uSun_diffuse = glGetUniformLocation(context->programID, "richtungslicht.diffuse");
    uSun_specular = glGetUniformLocation(context->programID, "richtungslicht.specular");

    // Punktlicht
    uLamp_enabled = glGetUniformLocation(context->programID, "punktlicht.enabled");
    uLamp_position = glGetUniformLocation(context->programID, "punktlicht.position");
    uLamp_ambient = glGetUniformLocation(context->programID, "punktlicht.ambient");
    uLamp_diffuse = glGetUniformLocation(context->programID, "punktlicht.diffuse");
    uLamp_specular = glGetUniformLocation(context->programID, "punktlicht.specular");
    uLamp_linear = glGetUniformLocation(context->programID, "punktlicht.linear");
    uLamp_quadratic = glGetUniformLocation(context->programID, "punktlicht.quadratic");

    // Spotlicht
    uSpot_enabled = glGetUniformLocation(context->programID, "spotlicht.enabled");
    uSpot_position = glGetUniformLocation(context->programID, "spotlicht.position");
    uSpot_direction = glGetUniformLocation(context->programID, "spotlicht.direction");
    uSpot_innerCone = glGetUniformLocation(context->programID, "spotlicht.innerCone");
    uSpot_outerCone = glGetUniformLocation(context->programID, "spotlicht.outerCone");
    uSpot_ambient = glGetUniformLocation(context->programID, "spotlicht.ambient");
    uSpot_diffuse = glGetUniformLocation(context->programID, "spotlicht.diffuse");
    uSpot_specular = glGetUniformLocation(context->programID, "spotlicht.specular");
    uSpot_linear = glGetUniformLocation(context->programID, "spotlicht.linear");
    uSpot_quadratic = glGetUniformLocation(context->programID, "spotlicht.quadratic");

    // Nebel
    uFogColor = glGetUniformLocation(context->programID, "nebel.color");
    uFogDensity = glGetUniformLocation(context->programID, "nebel.density");
    uFogEnabled = glGetUniformLocation(context->programID, "nebel.enabled");

    // Textur
    albedoLoc = glGetUniformLocation(context->programID, "uAlbedo");
    normalLoc = glGetUniformLocation(context->programID, "uNormalMap");
    roughnessLoc = glGetUniformLocation(context->programID, "uRoughness");
    uvScale = glGetUniformLocation(context->programID, "uvScale");

    // Texturkanäle
    glUniform1i(albedoLoc, 0);
    glUniform1i(normalLoc, 1);
    glUniform1i(roughnessLoc, 2);

    // Texturen laden
    // Cottage
    albedoCottage = loadTexture2D("textures/cottage/cottage_diffuse.png", 1);
    normalCottage = loadTexture2D("textures/cottage/cottage_normal.png", 0);
    // gras
    albedoGras = loadTexture2D("textures/gras/grass1-albedo3.png", 1);
    normalGras = loadTexture2D("textures/gras/grass1-normal1-ogl.png", 0);
    roughnessGras = loadTexture2D("textures/gras/grass1-rough.png", 0);
    // Bäume
    albedoBaum1und2 = loadTexture2D("textures/trees/tree1/BarkDecidious0143_5_S.jpg", 1);
    albedoBaum3 = loadTexture2D("textures/trees/tree3/BarkDecidious0194_7_S.jpg", 1);
    normalBaum = loadTexture2D("textures/trees/tree1/tree_bark_normal_ogl.png", 0);
    roughBaum = loadTexture2D("textures/trees/tree1/tree_bark_roughness.png", 0);
    // Glas
    albedoGlas = loadTexture2D("textures/glas/glass_albedo_red_a18.png", 1);
    normalGlas = loadTexture2D("textures/glas/glass_normal_flat_ogl.png", 0);
    roughGlas = loadTexture2D("textures/glas/glass_roughness_glossy_012.png", 0);
    // Coulmn
    albedoColumn = loadTexture2D("textures/column/Column_Albedo_fixed.png", 1);
    normalColumn = loadTexture2D("textures/column/column_normal.002.png", 0);
    roughColumn = loadTexture2D("textures/column/column_roughness.png", 0);
    // Teapot
    albedoTeapot = loadTexture2D("textures/teapot/1df5a76d-fb2d-45d4-ae28-7265782ed03b.png", 1);

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
    loadMesh("objects/slenderman.obj", &slenderman);
    loadMesh("objects/tree/tree1/Baumstamm1.obj", &baumstamm1);
    loadMesh("objects/tree/tree2/Baumstamm2.obj", &baumstamm2);
    loadMesh("objects/tree/tree3/Baumstamm3.obj", &baumstamm3);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glClearColor(0.17f, 0.19f, 0.21f, 1.0f);

    // Skybox
    initSkybox(context);
    glUseProgram(context->skyboxProgramID);
    glUniform1i(glGetUniformLocation(context->skyboxProgramID, "skybox"), 0);
}

void draw(AppContext *context)
{
    glUseProgram(context->programID);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Camera and View + Projection Matrix
    GLfloat V[16];
    GLfloat P[16];
    camera(V, P, context);

    lichtSchalter(uSun_enabled, uLamp_enabled, uSpot_enabled, context->window, &status);
    nebelSchalter(uFogEnabled, context->window, &status);

    // Richtungslicht an die Kamera setzten
    setDirectionalLight(uSun_direction, V, 1.0f, -1.0f, -1.0f);

    // Headlight (Spotlicht) immer auf die Kamera setzen
    setSpotLight(uSpot_position, uSpot_direction, V, context->eye, context->look);

    // Punktlicht-Position JEDES Frame in View-Space updaten
    setPointLight(uLamp_position, V, 2.0f, 2.0f, 5.0f);

    GLfloat M[16];
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

    //  Slenderman
    identity(M);
    translate(M, M, (GLfloat[]){1.4f, 0.0f, -3.2f});
    rotateY(M, M, 180);
    scale(M, M, (GLfloat[]){0.9f, 0.9f, 0.9f});
    setMaterialSlenderman();
    drawMeshWithModel(&slenderman, V, P, M, MVLoc, MVPLoc, NormalMLoc);

    // Baum 1
    glUniform2f(uvScale, 1500.0f, 1500.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, albedoBaum1und2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalBaum);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, roughBaum);
    identity(M);
    translate(M, M, (GLfloat[]){-4.0f, 0.0f, -2.5f}); // Position
    rotateY(M, M, 30.0f);                             // Drehung
    scale(M, M, (GLfloat[]){0.9f, 0.9f, 0.9f});
    setMaterialWood();
    drawMeshWithModel(&baumstamm3, V, P, M, MVLoc, MVPLoc, NormalMLoc);

    glUniform2f(uvScale, 1500.0f, 1500.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, albedoBaum1und2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalBaum);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, roughBaum);
    identity(M);
    translate(M, M, (GLfloat[]){14.0f, 0.0f, 12.5f}); // Position
    rotateY(M, M, 30.0f);                             // Drehung
    scale(M, M, (GLfloat[]){0.9f, 0.9f, 0.9f});
    setMaterialWood();
    drawMeshWithModel(&baumstamm3, V, P, M, MVLoc, MVPLoc, NormalMLoc);

    glUniform2f(uvScale, 1500.0f, 1500.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, albedoBaum3);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalBaum);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, roughBaum);
    identity(M);
    translate(M, M, (GLfloat[]){-14.0f, 0.0f, -12.5f}); // Position
    rotateY(M, M, 30.0f);                               // Drehung
    scale(M, M, (GLfloat[]){0.9f, 0.9f, 0.9f});
    setMaterialWood();
    drawMeshWithModel(&baumstamm3, V, P, M, MVLoc, MVPLoc, NormalMLoc);

    // Skybox
    glCullFace(GL_FRONT);
    glUseProgram(context->skyboxProgramID);
    glDepthFunc(GL_LEQUAL);
    GLfloat VP[16];
    GLfloat V_noT[16];
    memcpy(V_noT, V, sizeof(V_noT));
    V_noT[12] = V_noT[13] = V_noT[14] = 0.0f;
    mat4f_mul_mat4f(VP, P, V_noT);
    glUniformMatrix4fv(glGetUniformLocation(context->skyboxProgramID, "VP"), 1, GL_FALSE, VP);

    glBindVertexArray(context->skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, context->skyboxTexture);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glCullFace(GL_BACK);

    // Switch back to the normal depth functionx
    glDepthFunc(GL_LESS);

    // Glaswürfel
    glUseProgram(context->programID);
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
}

int main(void)
{

    AppContext context = {0};
    context.width = INIT_WINDOW_WIDTH;
    context.height = INIT_WINDOW_HEIGHT;

    glfwInit();

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 32);

    context.window = glfwCreateWindow(INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT, INIT_WINDOW_TITLE, NULL, NULL);

    if (!context.window)
    {
        printf("Error creating window\n");
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(context.window, framebuffer_size_callback);
    int fbw, fbh;
    glfwGetFramebufferSize(context.window, &fbw, &fbh);
    glViewport(0, 0, fbw, fbh);

    glfwMakeContextCurrent(context.window);
    glewInit();

    init(&context);

    while (!glfwWindowShouldClose(context.window))
    {
        draw(&context);
        glfwSwapBuffers(context.window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
