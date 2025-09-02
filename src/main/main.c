#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../functions/matrixUtils.h"
#include "../functions/loadShader.h"
#include "../functions/loadObj.h"
#include <math.h>

// windows
GLFWwindow * window;

// Licht Variablen
GLint lightPosLoc, viewPosLoc, lightColorLoc, objectColorLoc, ambientLoc, specularLoc, shininessLoc;

// Shader Variablen
GLuint program, vao;

// Uniform Standorte
GLint modelMatrixLoc, viewMatrixLoc, projectMatrixLoc;

// Matrizen
GLfloat modelMatrix[16];
GLfloat viewMatrix[16];
GLfloat projMatrix[16];

GLfloat eye[3] = {0.0f, 0.0f, 100.0f};    // Kamera 3 Einheiten vor dem Ursprung
GLfloat center[3] = {0.0f, 0.0f, 0.0f}; // Blick auf den Ursprung
GLfloat up[3] = {0.0f, 1.0f, 0.0f};     // „Oben“ ist +Y

int werte[4];

// Kamera werte
GLfloat spinValue = 0;
GLfloat spinDirection;
GLfloat angle = 0.0f;

GLfloat orbitRadius = 30.0f;

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
    modelMatrixLoc = glGetUniformLocation(program, "modelMatrix");
    viewMatrixLoc = glGetUniformLocation(program, "viewMatrix");
    projectMatrixLoc = glGetUniformLocation(program, "projectMatrix");

    identity(modelMatrix);
    identity(viewMatrix);
    identity(projMatrix);

    // neue Uniforms für Licht/Material
    lightPosLoc = glGetUniformLocation(program, "lightPos");
    viewPosLoc = glGetUniformLocation(program, "viewPos");
    lightColorLoc = glGetUniformLocation(program, "lightColor");
    objectColorLoc = glGetUniformLocation(program, "objectColor");
    ambientLoc = glGetUniformLocation(program, "ambientStrength");
    specularLoc = glGetUniformLocation(program, "specularStrength");
    shininessLoc = glGetUniformLocation(program, "shininess");

    // sinnvolle Startwerte
    glUniform3f(lightPosLoc, 2.0f, 3.0f, 2.0f);      // Position des Lichts
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);    // weißes Licht
    glUniform3f(objectColorLoc, 1.0f, 0.85f, 0.75f); // Materialfarbe (Beispiel)
    glUniform1f(ambientLoc, 0.20f);                  // Grundhelligkeit
    glUniform1f(specularLoc, 0.50f);                 // Glanzstärke
    glUniform1f(shininessLoc, 32.0f);                // Glanzgröße

    // read lines of OBJ4
    countLinesF("objects/Column_Triangle.obj", werte);
    GLfloat triangleEcken[werte[3] * 8];
    // lese das OBJ aus und fülle die Vertices
    // 7 Werte pro Vertex: x, y, z, u, v, nx, ny, nz
    loadOBJ("objects/Column_Triangle.obj", triangleEcken, werte);

    GLuint triangleVertexBufferObject; // VBO
    glGenBuffers(1, &triangleVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, triangleVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleEcken), triangleEcken, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // create vertex array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, triangleVertexBufferObject);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glViewport(0, 0, 1200, 800);
}

void draw()
{
    // Eventuell auch useProgramm machen bei mehreren Shadern
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TRANSFORMATIONEN Generell
    // 1) Kamera
    lookAt(viewMatrix, eye, center, up);
    glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, viewMatrix);

    // 2) Perspektive
    perspective(projMatrix, 45.0f * (3.14159265358979323846 / 180.0f), 1200.0f / 800.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(projectMatrixLoc, 1, GL_FALSE, projMatrix);

    // 3) Modell
    identity(modelMatrix);
    scale(modelMatrix, modelMatrix, (GLfloat[]){0.25f, 0.25f, 0.25f});
    glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, modelMatrix);

    // Licht viewPostion anpassen durch die Rotation
    glUniform3f(viewPosLoc, eye[0], eye[1], eye[2]);

    // Draw Call
    glBindVertexArray(vao); // bind vao
    glDrawArrays(GL_TRIANGLES, 0, werte[3]);
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