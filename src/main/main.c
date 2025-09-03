#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../functions/matrixUtils.h"
#include "../functions/loadShader.h"
#include "../functions/loadObj.h"
#include <math.h>

// windows
GLFWwindow *window;

// --- Material (struct Material material) ---
GLint uMat_emission;  // "material.emission"
GLint uMat_ambient;   // "material.ambient"
GLint uMat_diffuse;   // "material.diffuse"
GLint uMat_specular;  // "material.specular"
GLint uMat_shininess; // "material.shininess"

// --- Richtungslicht (struct lightSourceR richtungslicht) ---
GLint uSun_enabled;   // "richtungslicht.enabled"
GLint uSun_direction; // "richtungslicht.direction"
GLint uSun_ambient;   // "richtungslicht.ambient"
GLint uSun_diffuse;   // "richtungslicht.diffuse"
GLint uSun_specular;  // "richtungslicht.specular"

// --- Punktlicht (struct lightSourceP punktlicht) ---
GLint uLamp_enabled;   // "punktlicht.enabled"
GLint uLamp_position;  // "punktlicht.position"
GLint uLamp_ambient;   // "punktlicht.ambient"
GLint uLamp_diffuse;   // "punktlicht.diffuse"
GLint uLamp_specular;  // "punktlicht.specular"
GLint uLamp_linear;    // "punktlicht.linear"
GLint uLamp_quadratic; // "punktlicht.quadratic"

// Spotlicht
GLint uSpot_enabled;
GLint uSpot_position;  // vec3
GLint uSpot_direction; // vec3
GLint uSpot_innerCone; // float (radians)
GLint uSpot_outerCone; // float (radians)
GLint uSpot_ambient;   // vec4
GLint uSpot_diffuse;   // vec4
GLint uSpot_specular;  // vec4

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

    // Lichter: Directional aus, Punktlicht AN
    glUniform1i(uSun_enabled, 0);
    glUniform1i(uLamp_enabled, 0);
    glUniform1i(uSpot_enabled, 1);

    // Richtungslicht
    glUniform3f(uSun_direction, 5.0f, 5.0f, 0.0f);
    glUniform4f(uSun_ambient, 0.35f, 0.35f, 0.35f, 1.0f);
    glUniform4f(uSun_diffuse, 1.0f, 1.0f, 1.0f, 1.0f);
    glUniform4f(uSun_specular, 1.0f, 1.0f, 1.0f, 1.0f); // Glanz

    // Punktlicht
    GLfloat lightW[3] = {2.0f, 2.0f, 5.0f};
    GLfloat lightV[3];
    transform_point_view(lightV, viewMatrix, lightW);
    glUniform4f(uLamp_position, lightV[0], lightV[1], lightV[2], 1.0f);
    // Farben
    glUniform4f(uLamp_ambient, 0.2f, 0.2f, 0.2f, 1.0f);
    glUniform4f(uLamp_diffuse, 1.0f, 1.0f, 1.0f, 1.0f);
    glUniform4f(uLamp_specular, 1.0f, 1.0f, 1.0f, 1.0f);
    // Abschwächung (ruhig sanft)
    glUniform1f(uLamp_linear, 0.007f);
    glUniform1f(uLamp_quadratic, 0.0017f);

    // Spotlicht
    // Farben (weißes Spotlicht)
    glUniform4f(uSpot_ambient, 0.10f, 0.10f, 0.10f, 1.0f);
    glUniform4f(uSpot_diffuse, 1.00f, 1.00f, 1.00f, 1.0f);
    glUniform4f(uSpot_specular, 1.00f, 1.00f, 1.00f, 1.0f);

    // Winkel in RADIANT (z. B. 12° / 20°)
    float innerDeg = 12.0f, outerDeg = 20.0f;
    float innerRad = innerDeg * (float)M_PI / 180.0f;
    float outerRad = outerDeg * (float)M_PI / 180.0f;
    glUniform1f(uSpot_innerCone, innerRad);
    glUniform1f(uSpot_outerCone, outerRad);

    // Material: GLAS
    glUniform4f(uMat_emission, 0.0f, 0.0f, 0.0f, 1.0f);
    glUniform4f(uMat_ambient, 0.02f, 0.02f, 0.03f, 1.0f);
    float alphaGlass = 0.14f;
    glUniform4f(uMat_diffuse, 0.22f, 0.30f, 0.34f, alphaGlass);
    glUniform4f(uMat_specular, 1.0f, 1.0f, 1.0f, 1.0f);
    glUniform1f(uMat_shininess, 256.0f);

    // read lines of OBJ4
    countLinesF("objects/cube.obj", werte);
    GLfloat triangleEcken[werte[3] * 8];
    // lese das OBJ aus und fülle die Vertices
    // 7 Werte pro Vertex: x, y, z, u, v, nx, ny, nz
    loadOBJ("objects/cube.obj", triangleEcken, werte);

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
    // Kamera kreisen lassen
    eye[0] = 3 * cosf(angle);
    eye[2] = 3 * sinf(angle);
    angle += 0.01f;

    glUseProgram(program);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 1) View/Proj
    lookAt(viewMatrix, eye, center, up);
    perspective(projMatrix, 45.0f * (3.14159265358979323846f / 180.0f),
                1200.0f / 800.0f, 0.1f, 100.0f);

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
    transform_point_view(posV, viewMatrix, posW);
    float dirV[3] = {
        viewMatrix[0] * dirW[0] + viewMatrix[4] * dirW[1] + viewMatrix[8] * dirW[2],
        viewMatrix[1] * dirW[0] + viewMatrix[5] * dirW[1] + viewMatrix[9] * dirW[2],
        viewMatrix[2] * dirW[0] + viewMatrix[6] * dirW[1] + viewMatrix[10] * dirW[2]};
    float L = sqrtf(dirV[0] * dirV[0] + dirV[1] * dirV[1] + dirV[2] * dirV[2]);
    if (L > 1e-8f)
    {
        dirV[0] /= L;
        dirV[1] /= L;
        dirV[2] /= L;
    }
    glUniform3f(uSpot_position, posV[0], posV[1], posV[2]);
    glUniform3f(uSpot_direction, dirV[0], dirV[1], dirV[2]);

    // 2) Model
    identity(modelMatrix);
    translate(modelMatrix, modelMatrix, (GLfloat[]){0.0f, -0.01f, 0.0f});
    scale(modelMatrix, modelMatrix, (GLfloat[]){0.005f, 0.005f, 0.005f});

    // 3) MV, MVP, NormalM
    GLfloat MV[16], MVP[16];
    mat4f_mul_mat4f(MV, viewMatrix, modelMatrix);
    mat4f_mul_mat4f(MVP, projMatrix, MV);

    GLfloat NormalM[9];
    mat3_from_mat4(NormalM, MV);
    mat3_inverse_transpose(NormalM, NormalM);

    // 4) Punktlicht-Position JEDES Frame in View-Space updaten
    {
        GLfloat lightW[3] = {2.0f, 2.0f, 5.0f}; // deine Welt-Pos
        GLfloat lightV[3];
        transform_point_view(lightV, viewMatrix, lightW);
        glUniform4f(uLamp_position, lightV[0], lightV[1], lightV[2], 1.0f);
    }

    // 5) Matrizen hochladen
    glUniformMatrix4fv(MVLoc, 1, GL_FALSE, MV);
    glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, MVP);
    glUniformMatrix3fv(NormalMLoc, 1, GL_FALSE, NormalM);

    glBindVertexArray(vao);
    glDepthMask(GL_FALSE); // Z-Buffer nicht beschreiben (wichtig für Blending)

    // Pass 1: Rückseiten
    glCullFace(GL_FRONT);
    glDrawArrays(GL_TRIANGLES, 0, werte[3]);

    // Pass 2: Vorderseiten
    glCullFace(GL_BACK);
    glDrawArrays(GL_TRIANGLES, 0, werte[3]);
    glDepthMask(GL_TRUE); // Zustand zurücksetzen
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