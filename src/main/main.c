#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../functions/matrixUtils.h"
#include "../functions/loadShader.h"
#include "../functions/mesh.h"
#include "../functions/drawUtils.h"
#include "../functions/lightUtils.h"
#include <math.h>

// windows
GLFWwindow *window;

// Material (struct Material material)
GLint uMat_emission, uMat_ambient, uMat_diffuse, uMat_specular, uMat_shininess;

// Richtungslicht (struct lightSourceR richtungslicht)
GLint uSun_enabled, uSun_direction, uSun_ambient, uSun_diffuse, uSun_specular;

// Punktlicht (struct lightSourceP punktlicht)
GLint uLamp_enabled, uLamp_position, uLamp_ambient, uLamp_diffuse, uLamp_specular, uLamp_linear, uLamp_quadratic;

// Spotlicht (struct lightSourceS spotlicht)
GLint uSpot_enabled, uSpot_position, uSpot_direction, uSpot_innerCone, uSpot_outerCone, uSpot_ambient, uSpot_diffuse, uSpot_specular;

// Shader Variablen
GLuint program, vao;

// Uniform Standorte
GLint MVLoc, MVPLoc, NormalMLoc;

// Matrizen
GLfloat modelMatrix[16];
GLfloat viewMatrix[16];
GLfloat projMatrix[16];

GLfloat eye[3] = {0.0f, 0.0f, 100.0f};  // Kamera 3 Einheiten vor dem Ursprung
GLfloat center[3] = {0.0f, 0.0f, 0.0f}; // Blick auf den Ursprung
GLfloat up[3] = {0.0f, 1.0f, 0.0f};     // „Oben“ ist +Y

// Mesh
Mesh cube;
Mesh teapot;
Mesh column;

// Kamera werte
GLfloat spinValue = 0;
GLfloat spinDirection;
GLfloat angle = 0.0f;

GLfloat orbitRadius = 30.0f;

// Materialien setzen
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

void init()
{
    // compile vertex Shader
    const char *vertexText = loadShader("shader/vertex/vertexShader.glsl");
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexText, NULL);
    glCompileShader(vertexShader);
    GLint status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (!status)
    { // Shader checken anosnten bekommt man keine fehlermeldung
        printf("Error Comipiling vertex shader:");
        GLchar infoLog[1024];
        glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
        printf("%s\n", infoLog);
    }
    freeShader(vertexText);

    // cerate and compile fragment shader
    const char *fragmentText = loadShader("shader/fragment/fragmentShader.glsl");
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentText, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (!status)
    { // Shader checken anosnten bekommt man keine fehlermeldung
        printf("Error Comipiling fragment shader:");
        GLchar infoLog[1024];
        glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
        printf("%s\n", infoLog);
    }
    freeShader(fragmentText);

    // create and link shader programm
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status)
    {
        printf("ERROR BEIM LINKEN HILFEEE");
        GLchar infoLog[1024];
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        printf("%s\n", infoLog);
    }
    glValidateProgram(program);
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
    if (!status)
    {
        printf("ERROR BEIM VALIDIEREN HILFEEE");
        GLchar infoLog[1024];
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        printf("%s\n", infoLog);
    }
    // Einmal den Shader programm aktivieren
    glUseProgram(program);

    // einmal die Location holen Matrizen
    MVLoc = glGetUniformLocation(program, "MV");
    MVPLoc = glGetUniformLocation(program, "MVP");
    NormalMLoc = glGetUniformLocation(program, "NormalM");

    identity(modelMatrix);
    identity(viewMatrix);
    identity(projMatrix);

    // neue Uniforms für Licht/Material
    // Material
    uMat_emission = glGetUniformLocation(program, "material.emission");
    uMat_ambient = glGetUniformLocation(program, "material.ambient");
    uMat_diffuse = glGetUniformLocation(program, "material.diffuse");
    uMat_specular = glGetUniformLocation(program, "material.specular");
    uMat_shininess = glGetUniformLocation(program, "material.shininess");

    // Richtungslicht
    uSun_enabled = glGetUniformLocation(program, "richtungslicht.enabled");
    uSun_direction = glGetUniformLocation(program, "richtungslicht.direction");
    uSun_ambient = glGetUniformLocation(program, "richtungslicht.ambient");
    uSun_diffuse = glGetUniformLocation(program, "richtungslicht.diffuse");
    uSun_specular = glGetUniformLocation(program, "richtungslicht.specular");

    // Punktlicht
    uLamp_enabled = glGetUniformLocation(program, "punktlicht.enabled");
    uLamp_position = glGetUniformLocation(program, "punktlicht.position");
    uLamp_ambient = glGetUniformLocation(program, "punktlicht.ambient");
    uLamp_diffuse = glGetUniformLocation(program, "punktlicht.diffuse");
    uLamp_specular = glGetUniformLocation(program, "punktlicht.specular");
    uLamp_linear = glGetUniformLocation(program, "punktlicht.linear");
    uLamp_quadratic = glGetUniformLocation(program, "punktlicht.quadratic");

    // Spotlicht
    uSpot_enabled = glGetUniformLocation(program, "spotlicht.enabled");
    uSpot_position = glGetUniformLocation(program, "spotlicht.position");
    uSpot_direction = glGetUniformLocation(program, "spotlicht.direction");
    uSpot_innerCone = glGetUniformLocation(program, "spotlicht.innerCone");
    uSpot_outerCone = glGetUniformLocation(program, "spotlicht.outerCone");
    uSpot_ambient = glGetUniformLocation(program, "spotlicht.ambient");
    uSpot_diffuse = glGetUniformLocation(program, "spotlicht.diffuse");
    uSpot_specular = glGetUniformLocation(program, "spotlicht.specular");

    // Lichter an aus
    glUniform1i(uSun_enabled, 1);
    glUniform1i(uLamp_enabled, 1);
    glUniform1i(uSpot_enabled, 1);

    // Richtungslicht
    initializeDirectionalLight(uSun_ambient, uSun_diffuse, uSun_specular);

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
    glViewport(0, 0, 1200, 800);
}

void draw()
{
    glUseProgram(program);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Kamera kreisen lassen
    eye[0] = 3 * cosf(angle);
    eye[2] = 3 * sinf(angle);
    angle += 0.01f;

    // 1) View/Proj
    lookAt(viewMatrix, eye, center, up);
    perspective(projMatrix, 45.0f * (3.14159265358979323846f / 180.0f),
                1200.0f / 800.0f, 0.1f, 100.0f);

    // Richtungslicht an die Kamera setzten
    setDirectionalLight(uSun_direction, viewMatrix, 1.0f, 1.0f, 1.0f);

    // Headlight (Spotlicht) immer auf die Kamera setzen
    setSpotLight(uSpot_position, uSpot_direction, viewMatrix, eye, center);

    //Punktlicht-Position JEDES Frame in View-Space updaten
    setPointLight(uLamp_position, viewMatrix, 2.0f, 2.0f, 5.0f);

    // 1) Graue Säule (gestreckter Cube)
    GLfloat M[16];
    identity(M);
    translate(M, M, (GLfloat[]){0.0f, -0.01f, 0.0f});
    scale(M, M, (GLfloat[]){0.0042f, 0.0045f, 0.0042f});
    setMaterialGrayPillar();
    drawMeshWithModel(&column, viewMatrix, projMatrix, M, MVLoc, MVPLoc, NormalMLoc);

    // 2) Golderner Teapot
    identity(M);
    translate(M, M, (GLfloat[]){0.0f, 0.0060f, 0.0f});
    scale(M, M, (GLfloat[]){0.0014f, 0.0014f, 0.0014f});
    setMaterialPolishedGold();
    drawMeshWithModel(&teapot, viewMatrix, projMatrix, M, MVLoc, MVPLoc, NormalMLoc);

    // Glaswürfel
    identity(M);
    translate(M, M, (GLfloat[]){0.0f, 0.0035f, 0.0f});
    scale(M, M, (GLfloat[]){0.0024f, 0.0024f, 0.0024f});
    setMaterialGlass(0.18f);
    drawtransparentMeshWithModel(&cube, viewMatrix, projMatrix, M, MVLoc, MVPLoc, NormalMLoc);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_DEPTH_BITS, 24); // 24-Bit Tiefenpuffer
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1200, 800, "Test Fenster", NULL, NULL);

    if (!window)
    {
        printf("FEHLER BEIM FENSTER ERSTELLEN");
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);
    glewInit();

    init();

    while (!glfwWindowShouldClose(window))
    {
        draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}