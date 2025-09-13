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
#include "../functions/matrixTester.h"

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

// Nebel (struct fog nebel)
GLint uFogColor, uFogDensity, uFogEnabled;

// Environment mapping
GLint uEnvMap, uUseEnvMap, uIOR, uEnvStrength, uViewRotLoc;

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

// TODO: MATERIALS.C
void setMaterialPolishedGold()
{
    glUniform4f(uMat_emission, 0, 0, 0, 1);
    glUniform4f(uMat_ambient, 0.25f, 0.22f, 0.06f, 1);
    glUniform4f(uMat_diffuse, 0.35f, 0.31f, 0.09f, 1); // alpha=1 (opak)
    glUniform4f(uMat_specular, 0.80f, 0.72f, 0.21f, 1);
    glUniform1f(uMat_shininess, 150.2f);
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
    initSkybox(context);
    glUseProgram(context->skyboxProgramID);
    glUniform1i(glGetUniformLocation(context->skyboxProgramID, "skybox"), 0);

    // Env map binding for main program
    glUseProgram(context->programID);
    uEnvMap = glGetUniformLocation(context->programID, "uEnvMap");
    uUseEnvMap = glGetUniformLocation(context->programID, "uUseEnvMap");
    uIOR = glGetUniformLocation(context->programID, "uIOR");
    uEnvStrength = glGetUniformLocation(context->programID, "uEnvStrength");
    uViewRotLoc = glGetUniformLocation(context->programID, "uViewRot");

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, context->skyboxTexture);
    glUniform1i(uEnvMap, 1);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // defaults
    glUniform1f(uIOR, 1.5f);
    glUniform1f(uEnvStrength, 0.85f);
    glUniform1i(uUseEnvMap, 0);
}

void draw(AppContext *context)
{
    glUseProgram(context->programID);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Camera and View + Projection Matrix
    GLfloat V[16];
    GLfloat P[16];
    camera(V, P, context);

    // viewMatrix ist deine 4x4-View-Matrix (column-major, OpenGL-konform)
    GLfloat viewRot[9] = {
        V[0], V[1], V[2],
        V[4], V[5], V[6],
        V[8], V[9], V[10]};

    // Achtung: Das sind die oberen linken 3x3-Elemente ohne Translation.
    glUniformMatrix3fv(uViewRotLoc, 1, GL_FALSE, viewRot);

    lichtSchalter(uSun_enabled, uLamp_enabled, uSpot_enabled, context->window, &status);
    nebelSchalter(uFogEnabled, context->window, &status);

    // Richtungslicht an die Kamera setzten
    setDirectionalLight(uSun_direction, V, 1.0f, -1.0f, -1.0f);

    // Headlight (Spotlicht) immer auf die Kamera setzen
    setSpotLight(uSpot_position, uSpot_direction, V, context->eye, context->look);

    // Punktlicht-Position JEDES Frame in View-Space updaten
    setPointLight(uLamp_position, V, 2.0f, 2.0f, 5.0f);

    // Make everything reflecting?
    glUniform1i(uUseEnvMap, 1);


    // 1) Graue Säule (gestreckter Cube)
    GLfloat M[16];
    identity(M);
    translate(M, M, (GLfloat[]){0.0f, -0.01f, 0.0f});
    scale(M, M, (GLfloat[]){0.42f, 0.45f, 0.42f});
    setMaterialGrayPillar();
    drawMeshWithModel(&column, V, P, M, MVLoc, MVPLoc, NormalMLoc);

    // 2) Golderner Teapot
    identity(M);
    translate(M, M, (GLfloat[]){0.0f, 1.6f, 0.0f});
    scale(M, M, (GLfloat[]){0.14f, 0.14f, 0.14f});
    setMaterialPolishedGold();
    drawMeshWithModel(&teapot, V, P, M, MVLoc, MVPLoc, NormalMLoc);

    // 3) Gras
    identity(M);
    translate(M, M, (GLfloat[]){0.0f, -0.01f, 0.0f});
    scale(M, M, (GLfloat[]){0.42f, 0.45f, 0.42f});
    setMaterialGrass();
    drawMeshWithModel(&gras, V, P, M, MVLoc, MVPLoc, NormalMLoc);

    // 4) Cottage
    identity(M);
    translate(M, M, (GLfloat[]){25.0f, 0.0f, -15.0f});
    rotateY(M, M, 20);
    scale(M, M, (GLfloat[]){0.5f, 0.5f, 0.5f});
    setMaterialWood();
    drawMeshWithModel(&cottage, V, P, M, MVLoc, MVPLoc, NormalMLoc);

    // 5) Slenderman
    identity(M);
    translate(M, M, (GLfloat[]){1.4f, 0.0f, -3.2f});
    rotateY(M, M, 180);
    scale(M, M, (GLfloat[]){0.9f, 0.9f, 0.9f});
    setMaterialSlenderman();
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

            setMaterialTree();
            drawMeshWithModel(tree, V, P, M, MVLoc, MVPLoc, NormalMLoc);
        }
    }

    // 7) Glaswürfel
    identity(M);
    translate(M, M, (GLfloat[]){0.0f, 1.42f, 0.0f});
    scale(M, M, (GLfloat[]){0.24f, 0.24f, 0.24f});
    setMaterialGlass(0.18f);
    glUniform1i(uUseEnvMap, 1);
    drawtransparentMeshWithModel(&cube, V, P, M, MVLoc, MVPLoc, NormalMLoc);
    glUniform1i(uUseEnvMap, 0);

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

    testeFunktionen();

    while (!glfwWindowShouldClose(context.window))
    {
        draw(&context);
        glfwSwapBuffers(context.window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
