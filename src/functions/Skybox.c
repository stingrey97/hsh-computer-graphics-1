// Self
#include "Skybox.h"

// Standard libs
#include <GL/glew.h>
#include <assert.h>

// Own libs
#include "Constants.h"
#include "AppContext.h"
#include "TextureLoader.h"
#include "ShaderLoader.h"

void initSkybox(AppContext *context)
{
    assert(context != NULL);

    const float skyboxVertices[] =
        {
            //   Coordinates
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f};

    const unsigned int skyboxIndices[] =
        {
            // Right
            1, 2, 6,
            6, 5, 1,
            // Left
            0, 4, 7,
            7, 3, 0,
            // Top
            4, 5, 6,
            6, 7, 4,
            // Bottom
            0, 3, 2,
            2, 1, 0,
            // Back
            0, 1, 5,
            5, 4, 0,
            // Front
            3, 7, 6,
            6, 2, 3};

#ifdef SKYBOX_DAY
    const char *facesCubemap[6] =
        {
            "textures/skybox/daytime/px.png",
            "textures/skybox/daytime/nx.png",
            "textures/skybox/daytime/py.png",
            "textures/skybox/daytime/ny.png",
            "textures/skybox/daytime/pz.png",
            "textures/skybox/daytime/nz.png",
        };
#endif
#ifdef SKYBOX_MYSTIC
    const char *facesCubemap[6] =
        {
            "textures/skybox/mystic/px.png",
            "textures/skybox/mystic/nx.png",
            "textures/skybox/mystic/py.png",
            "textures/skybox/mystic/ny.png",
            "textures/skybox/mystic/pz.png",
            "textures/skybox/mystic/nz.png",
        };
#endif
#ifdef SKYBOX_NIGHT
    const char *facesCubemap[6] =
        {
            "textures/skybox/nighttime/nightRT.png",
            "textures/skybox/nighttime/nightLF.png",
            "textures/skybox/nighttime/nightUP.png",
            "textures/skybox/nighttime/nightDN.png",
            "textures/skybox/nighttime/nightFT.png",
            "textures/skybox/nighttime/nightBK.png",
        };
#endif

    // Load Skybox program with Shaders
    const char *vertexPath = "shader/vertex/skyboxVertexShader.glsl";
    const char *fragmentPath = "shader/fragment/skyboxFragmentShader.glsl";
    context->skyboxProgramID = loadShaders(vertexPath, fragmentPath);

    // Load Cubemap Texture
    context->skyboxTexture = loadCubemap(facesCubemap);

    // Create VAO, VBO, and EBO
    GLuint skyboxVBO, skyboxEBO;
    glGenVertexArrays(1, &context->skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);
    glBindVertexArray(context->skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
