#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../functions/matrixUtils.h"
#include "../functions/loadShader.h"
#include "../functions/mesh.h"
#include "../functions/drawUtils.h"
#include "../functions/lightUtils.h"
#include "../functions/camera.h"
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../functions/stb_image.h"

// windowData
WindowData windowData;
// windows
GLFWwindow *window;

//Material (struct Material material)
GLint uMat_emission, uMat_ambient, uMat_diffuse, uMat_specular, uMat_shininess;

//Richtungslicht (struct lightSourceR richtungslicht)
GLint uSun_enabled, uSun_direction, uSun_ambient, uSun_diffuse, uSun_specular;

//Punktlicht (struct lightSourceP punktlicht)
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
GLfloat eye[3] = {0.0f, 0.0f, 5.0f};    // Kamera 3 Einheiten vor dem Ursprung
GLfloat center[3] = {0.0f, 0.0f, 0.0f}; // Blick auf den Ursprung
GLfloat up[3] = {0.0f, 1.0f, 0.0f};     // „Oben“ ist +Y

int width = 1280, height = 720;

// Mesh
Mesh cube;
Mesh teapot;
Mesh column;

// Kamera werte
GLfloat spinValue = 0;
GLfloat spinDirection;
GLfloat angle = 0.0f;

GLfloat orbitRadius = 30.0f;

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
    // camera Stuff.
    initCamera(windowData);
    windowData.cameraData.startDirection[0] = center[0];
    windowData.cameraData.startDirection[1] = center[1];
    windowData.cameraData.startDirection[2] = center[2];
    windowData.cameraData.startPosition[0] = eye[0];
    windowData.cameraData.startPosition[1] = eye[1];
    windowData.cameraData.startPosition[2] = eye[2];
    windowData.cameraData.startUp[0] = up[0];
    windowData.cameraData.startUp[1] = up[1];
    windowData.cameraData.startUp[2] = up[2];
    windowData.cameraData.FoV = 45.f;
    windowData.cameraData.mouseSpeed = 0.05f;
    windowData.cameraData.speed = 3.f;
    windowData.cameraData.startHorizontal = 4.712f;
    windowData.cameraData.startVertical = 0.f;

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

    // --- Skybox-Shader kompilieren ---
    glDepthMask(GL_FALSE);
    {
        const char *vsSrc = loadShader("shader/vertex/skybox.glsl");
        const char *fsSrc = loadShader("shader/fragment/skybox.glsl");
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vsSrc, NULL);
        glCompileShader(vs);
        GLint ok;
        glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
        if (!ok)
        {
            GLchar log[1024];
            glGetShaderInfoLog(vs, 1024, NULL, log);
            printf("Skybox VS: %s\n", log);
        }
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fsSrc, NULL);
        glCompileShader(fs);
        glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
        if (!ok)
        {
            GLchar log[1024];
            glGetShaderInfoLog(fs, 1024, NULL, log);
            printf("Skybox FS: %s\n", log);
        }
        freeShader(vsSrc);
        freeShader(fsSrc);

        skyboxProgram = glCreateProgram();
        glAttachShader(skyboxProgram, vs);
        glAttachShader(skyboxProgram, fs);
        glLinkProgram(skyboxProgram);
        glGetProgramiv(skyboxProgram, GL_LINK_STATUS, &ok);
        if (!ok)
        {
            GLchar log[1024];
            glGetProgramInfoLog(skyboxProgram, 1024, NULL, log);
            printf("Skybox Link: %s\n", log);
        }
        glDeleteShader(vs);
        glDeleteShader(fs);

        sbViewLoc = glGetUniformLocation(skyboxProgram, "view");
        sbProjLoc = glGetUniformLocation(skyboxProgram, "projection");
        glUseProgram(skyboxProgram);
        glUniform1i(glGetUniformLocation(skyboxProgram, "skybox"), 0);
        glUseProgram(0);
    }

    // --- Skybox-Geometrie (Würfel, 36 Vertices) ---
    float skyboxVertices[] = {
        -1, 1, -1, -1, -1, -1, 1, -1, -1, 1, -1, -1, 1, 1, -1, -1, 1, -1,
        -1, -1, 1, -1, -1, -1, -1, 1, -1, -1, 1, -1, -1, 1, 1, -1, -1, 1,
        1, -1, -1, 1, -1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, 1, -1, -1,
        -1, -1, 1, -1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, 1, -1, -1, 1,
        -1, 1, -1, 1, 1, -1, 1, 1, 1, 1, 1, 1, -1, 1, 1, -1, 1, -1,
        -1, -1, -1, -1, -1, 1, 1, -1, -1, 1, -1, -1, -1, -1, 1, 1, -1, 1};
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glBindVertexArray(0);

    // --- Cubemap laden ---
    const char *faces[6] = {
        "textures/skybox/px.png", // +X
        "textures/skybox/nx.png", // -X
        "textures/skybox/py.png", // +Y
        "textures/skybox/ny.png", // -Y
        "textures/skybox/pz.png", // +Z
        "textures/skybox/nz.png"  // -Z
    };
    cubemapTex = loadCubemap(faces);

    // Initial die Projection auch dem Skybox-Shader geben:
    perspective(projMatrix, 45.0f, (float)width / height, 0.1f, 100.0f);
    glUseProgram(skyboxProgram);
    glUniformMatrix4fv(sbProjLoc, 1, GL_FALSE, projMatrix);
    glUseProgram(0);

    glDepthMask(GL_TRUE);
}

void draw()
{
    glUseProgram(program);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Perspective- and ViewMatrix
    GLfloat PV[16];
    camera(PV, windowData);
    glUniformMatrix4fv(PVLoc, 1, GL_FALSE, PV);
    glUseProgram(program);
    glUniform3f(viewPosLoc, eye[0], eye[1], eye[2]);

    // Kamera kreisen lassen
    eye[0] = 3 * cosf(angle);
    eye[2] = 3 * sinf(angle);
    angle += 0.01f;

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

    // 4) Punktlicht-Position JEDES Frame in View-Space updaten
    GLfloat lightW[3] = {2.0f, 2.0f, 5.0f}; // deine Welt-Pos
    GLfloat lightV[3];
    transform_point_view(lightV, viewMatrix, lightW);
    glUniform4f(uLamp_position, lightV[0], lightV[1], lightV[2], 1.0f);

    // Draw Call
    glBindVertexArray(vao); // bind vao
    glDrawArrays(GL_TRIANGLES, 0, werte[3]);
}

int main(void)
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_DEPTH_BITS, 24); // 24-Bit Tiefenpuffer
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // set Windowsize
    windowData.xWindowSize = 1200.f;
    windowData.yWindowSize = 800.f;

    window = glfwCreateWindow(width, height, "OpenGL – Team Green", NULL, NULL);

    windowData.window = glfwCreateWindow((int)windowData.xWindowSize, (int)windowData.yWindowSize, "CG-1 Projekt", NULL, NULL);

    if (!windowData.window)
    {
        printf("FEHLER BEIM FENSTER ERSTELLEN");
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(windowData.window, framebuffer_size_callback);
    glfwMakeContextCurrent(windowData.window);
    glewInit();

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    
    init();

    while (!glfwWindowShouldClose(windowData.window))
    {
        draw();
        glfwSwapBuffers(windowData.window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}