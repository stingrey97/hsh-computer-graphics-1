#include <stdio.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../functions/stb_image.h"

#include "../functions/AppContext.h"
#include "../functions/matrixUtils.h"
#include "../functions/loadShaders.h"
#include "../functions/mesh.h"
#include "../functions/drawUtils.h"
#include "../functions/lightUtils.h"
#include "../functions/camera.h"

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
GLint uSpot_enabled, uSpot_position, uSpot_direction, uSpot_innerCone, uSpot_outerCone, uSpot_ambient, uSpot_diffuse, uSpot_specular;

// Uniform Standorte
GLint MVLoc, MVPLoc, NormalMLoc;

// Mesh
Mesh cube;
Mesh teapot;
Mesh column;

/* TODO: FIX FRAMEBUFFER CALLBACK WHEN SKYBOX IS DONE

void framebuffer_size_callback(GLFWwindow *window, int cb_width, int cb_height)
{
    glViewport(0, 0, cb_width, cb_height);

    perspective(projMatrix, 45.0f, (float)cb_width / (float)cb_height, 0.1f, 100.0f);

    glUseProgram(program);
    glUniformMatrix4fv(projMatrix, 1, GL_FALSE, projMatrix);

    glUseProgram(skyboxProgram);
    glUniformMatrix4fv(sbProjLoc, 1, GL_FALSE, projMatrix);
    glUseProgram(0);
}
*/

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

void init(AppContext *context)
{
    // Initial camera
    setVec3(context->eye, 0, 0, 5);
    setVec3(context->look, 0, 0, 0);
    setVec3(context->up, 0, 1, 5);

    initCamera(context);

    const char *vertexPath = "shader/vertex/vertexShader.glsl";
    const char *fragmentPath = "shader/fragment/fragmentShader.glsl";

    context->programID = loadShaders(vertexPath, fragmentPath);

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

    // Lichter an aus
    glUniform1i(uSun_enabled, 1);
    glUniform1i(uLamp_enabled, 1);
    glUniform1i(uSpot_enabled, 1);

    // Richtungslicht
    initializeDirectionalLight(uSun_ambient, uSun_diffuse, uSun_specular);
    glUniform3f(uSun_direction, 5.0f, 5.0f, 0.0f); // in draw call

    // Punktlicht
    initializePointLight(uLamp_ambient, uLamp_diffuse, uLamp_specular, uLamp_linear, uLamp_quadratic);

    // Spotlicht
    initializeSpotLight(uSpot_ambient, uSpot_diffuse, uSpot_specular, uSpot_innerCone, uSpot_outerCone);

    loadMesh("objects/teapot.obj", &teapot);
    loadMesh("objects/column.obj", &column);
    loadMesh("objects/cube.obj", &cube);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

void draw(AppContext *context)
{
    glUseProgram(context->programID);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Camera and View + Projection Matrix
    GLfloat V[16];
    GLfloat P[16];
    camera(V, P, context);

    // Headlight (Spotlicht) immer auf die Kamera setzen
    

    float posW[3] = {eye[0], eye[1], eye[2]};
    float dirW[3] = {center[0] - eye[0], center[1] - eye[1], center[2] - eye[2]};
    float d = sqrtf(dirW[0] * dirW[0] + dirW[1] * dirW[1] + dirW[2] * dirW[2]);
    if (d > 1e-8f)
    {
        dirW[0] /= d;
        dirW[1] /= d;
        dirW[2] /= d;
    }
    float posV[3];
    transform_point_view(posV, V, posW);
    float dirV[3] = {
        V[0] * dirW[0] + V[4] * dirW[1] + V[8] * dirW[2],
        V[1] * dirW[0] + V[5] * dirW[1] + V[9] * dirW[2],
        V[2] * dirW[0] + V[6] * dirW[1] + V[10] * dirW[2]};
    float L = sqrtf(dirV[0] * dirV[0] + dirV[1] * dirV[1] + dirV[2] * dirV[2]);
    if (L > 1e-8f)
    {
        dirV[0] /= L;
        dirV[1] /= L;
        dirV[2] /= L;
    }
    glUniform3f(uSpot_position, posV[0], posV[1], posV[2]);
    glUniform3f(uSpot_direction, dirV[0], dirV[1], dirV[2]);

    // 4) Punktlicht-Position JEDES Frame in View-Space updaten
    GLfloat lightW[3] = {2.0f, 2.0f, 5.0f}; // deine Welt-Pos
    GLfloat lightV[3];
    transform_point_view(lightV, V, lightW);
    glUniform4f(uLamp_position, lightV[0], lightV[1], lightV[2], 1.0f);
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
        printf("FEHLER BEIM FENSTER ERSTELLEN\n");
        glfwTerminate();
        return -1;
    }

    // TODO: glfwSetFramebufferSizeCallback(context.window, framebuffer_size_callback);

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
