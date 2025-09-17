// Self
#include "Skybox.h"

// Standard libs
#include <GL/glew.h>
#include <assert.h>
#include <string.h>

// Own libs
#include "Constants.h"
#include "AppContext.h"
#include "TextureLoader.h"
#include "ShaderLoader.h"
#include "MathUtils.h"

static const char *daytimeCubemap[6] =
        {
            "textures/skybox/daytime/px.png",
            "textures/skybox/daytime/nx.png",
            "textures/skybox/daytime/py.png",
            "textures/skybox/daytime/ny.png",
            "textures/skybox/daytime/pz.png",
            "textures/skybox/daytime/nz.png",
        };

    static const char *mysticCubemap[6] =
        {
            "textures/skybox/mystic/px.png",
            "textures/skybox/mystic/nx.png",
            "textures/skybox/mystic/py.png",
            "textures/skybox/mystic/ny.png",
            "textures/skybox/mystic/pz.png",
            "textures/skybox/mystic/nz.png",
        };

    static const char *nighttimeCubemap[6] =
        {
            "textures/skybox/nighttime/nightRT.png",
            "textures/skybox/nighttime/nightLF.png",
            "textures/skybox/nighttime/nightUP.png",
            "textures/skybox/nighttime/nightDN.png",
            "textures/skybox/nighttime/nightFT.png",
            "textures/skybox/nighttime/nightBK.png",
        };


void initSkybox(AppContext *ctx)
{
    assert(ctx != NULL);

    const float skyboxVertices[] =
        {
            //   Coordinates
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
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

    // Load Skybox program with Shaders
    const char *vertexPath = "shader/vertex/skyboxVertexShader.glsl";
    const char *fragmentPath = "shader/fragment/skyboxFragmentShader.glsl";
    ctx->skyboxProgramID = loadShaders(vertexPath, fragmentPath);

    // Load Cubemap Texture
    ctx->skyboxTexture = loadCubemap(nighttimeCubemap);
    ctx->reflect = 0;

    // Create VAO, VBO, and EBO
    GLuint skyboxVBO, skyboxEBO;
    glGenVertexArrays(1, &ctx->skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);
    glBindVertexArray(ctx->skyboxVAO);
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

void drawSkybox(AppContext *ctx, GLfloat *V, GLfloat *P)
{
    glUseProgram(ctx->skyboxProgramID);

    glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);
    GLfloat VP[16];
    GLfloat V_noT[16];
    memcpy(V_noT, V, sizeof(V_noT));
    V_noT[12] = V_noT[13] = V_noT[14] = 0.0f;
    mat4f_mul_mat4f(VP, P, V_noT);

    glUniformMatrix4fv(glGetUniformLocation(ctx->skyboxProgramID, "VP"), 1, GL_FALSE, VP);

    glBindVertexArray(ctx->skyboxVAO);
    glActiveTexture(GL_TEXTURE0);

    if(glfwGetKey(ctx->window, GLFW_KEY_8) == GLFW_PRESS) {
        ctx->skyboxTexture = loadCubemap(mysticCubemap);
    } else if(glfwGetKey(ctx->window, GLFW_KEY_9) == GLFW_PRESS) {
        ctx->skyboxTexture = loadCubemap(daytimeCubemap);
    } else if(glfwGetKey(ctx->window, GLFW_KEY_0) == GLFW_PRESS) {
        ctx->skyboxTexture = loadCubemap(nighttimeCubemap);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, ctx->skyboxTexture);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Reset state (keep currently bound program unchanged)
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
    glCullFace(GL_BACK);
}
