#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../functions/matrixUtils.h"
#include "../functions/loadShader.h"
#include "../functions/loadObj.h"
#include "../functions/camera.h"
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../functions/stb_image.h"

// windowData
WindowData windowData;
// windows
GLFWwindow *window;

// Licht Variablen
GLint lightPosLoc, viewPosLoc, lightColorLoc, objectColorLoc, ambientLoc, specularLoc, shininessLoc;

// Shader Variablen
GLuint program, vao;

// Uniform Standorte
GLint modelMatrixLoc, viewMatrixLoc, PVLoc;

// Matrizen
GLfloat modelMatrix[16];
GLfloat viewMatrix[16];
GLfloat projMatrix[16];
GLfloat eye[3] = {0.0f, 0.0f, 5.0f};    // Kamera 3 Einheiten vor dem Ursprung
GLfloat center[3] = {0.0f, 0.0f, 0.0f}; // Blick auf den Ursprung
GLfloat up[3] = {0.0f, 1.0f, 0.0f};     // „Oben“ ist +Y

int width = 1280, height = 720;

int werte[4];

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

// Skybox
GLuint skyboxProgram, skyboxVAO, skyboxVBO, cubemapTex;
GLint sbViewLoc, sbProjLoc;

GLuint loadCubemap(const char *faces[6])
{
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // <— hinzufügen

    // Für Cubemaps: kein Flip
    stbi_set_flip_vertically_on_load(0);

    int w, h, n;
    for (int i = 0; i < 6; ++i)
    {
        unsigned char *data = stbi_load(faces[i], &w, &h, &n, 0);
        if (!data)
        {
            char absPath[PATH_MAX];
            if (realpath(faces[i], absPath))
            {
                printf("Cubemap: konnte %s nicht laden\n", absPath);
            }
            else
            {
                printf("Cubemap: konnte %s nicht laden (realpath fehlgeschlagen)\n", faces[i]);
            }
            continue;
        }

        GLenum srcFmt = (n == 1) ? GL_RED : (n == 3) ? GL_RGB
                                                     : GL_RGBA;
        GLint intFmt = (n == 1) ? GL_R8 : (n == 3) ? GL_RGB8
                                                   : GL_RGBA8;

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                     intFmt, w, h, 0, srcFmt, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return tex;
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
    modelMatrixLoc = glGetUniformLocation(program, "modelMatrix");
    PVLoc = glGetUniformLocation(program, "PV");

    identity(modelMatrix);

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
    countLinesF("objects/teapot.obj", werte);
    GLfloat triangleEcken[werte[3] * 8];
    // lese das OBJ aus und fülle die Vertices
    // 7 Werte pro Vertex: x, y, z, u, v, nx, ny, nz
    loadOBJ("objects/teapot.obj", triangleEcken, werte);

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
    // Eventuell auch useProgramm machen bei mehreren Shadern
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Perspective- and ViewMatrix
    GLfloat PV[16];
    camera(PV, windowData);
    glUniformMatrix4fv(PVLoc, 1, GL_FALSE, PV);
    glUseProgram(program);
    glUniform3f(viewPosLoc, eye[0], eye[1], eye[2]);

    // TRANSFORMATIONEN Generell
    // 1) Kamera
    lookAt(viewMatrix, eye, center, up);
    glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, viewMatrix);

    // 2) Perspektive
    perspective(projMatrix, 45.0f, (float)width / height, 0.1f, 100.0f);
    glUniformMatrix4fv(projMatrix, 1, GL_FALSE, projMatrix);

    // 3) Modell
    identity(modelMatrix);
    scale(modelMatrix, modelMatrix, (GLfloat[]){0.25f, 0.25f, 0.25f});
    glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, modelMatrix);

    // Draw Call
    glBindVertexArray(vao); // bind vao
    glDrawArrays(GL_TRIANGLES, 0, werte[3]);

    // --- Skybox ---
    glDepthFunc(GL_LEQUAL); // Skybox darf bei z == 1 schreiben
    glCullFace(GL_FRONT);   // Innenflächen sichtbar (oder Culling kurz aus)

    glUseProgram(skyboxProgram);

    // View ohne Translation (nur Rotation)
    GLfloat viewNoTrans[16];
    identity(viewNoTrans);
    viewNoTrans[0] = viewMatrix[0];
    viewNoTrans[1] = viewMatrix[1];
    viewNoTrans[2] = viewMatrix[2];
    viewNoTrans[4] = viewMatrix[4];
    viewNoTrans[5] = viewMatrix[5];
    viewNoTrans[6] = viewMatrix[6];
    viewNoTrans[8] = viewMatrix[8];
    viewNoTrans[9] = viewMatrix[9];
    viewNoTrans[10] = viewMatrix[10];

    glUniformMatrix4fv(sbViewLoc, 1, GL_FALSE, viewNoTrans);
    glUniformMatrix4fv(sbProjLoc, 1, GL_FALSE, projMatrix); // falls Aspect/FOV sich änderte

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTex);

    glBindVertexArray(skyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glUseProgram(0);

    // States zurücksetzen
    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);
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