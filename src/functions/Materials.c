// Self
#include "Materials.h"

// Standard libs
#include <assert.h>

// OpenGL
#include <GL/glew.h>

void setMaterialPolishedGold(AppContext *ctx)
{
    assert(ctx != NULL);

    glUniform4f(ctx->uMat_emission, 0, 0, 0, 1);
    glUniform4f(ctx->uMat_ambient, 0.25f, 0.22f, 0.06f, 1);
    glUniform4f(ctx->uMat_diffuse, 0.35f, 0.31f, 0.09f, 1);
    glUniform4f(ctx->uMat_specular, 0.80f, 0.72f, 0.21f, 1);
    glUniform1f(ctx->uMat_shininess, 83.2f);
}

void setMaterialGrayPillar(AppContext *ctx)
{
    assert(ctx != NULL);

    glUniform4f(ctx->uMat_emission, 0, 0, 0, 1);
    glUniform4f(ctx->uMat_ambient, 0.20f, 0.20f, 0.20f, 1);
    glUniform4f(ctx->uMat_diffuse, 0.50f, 0.50f, 0.50f, 1);
    glUniform4f(ctx->uMat_specular, 0.10f, 0.10f, 0.10f, 1);
    glUniform1f(ctx->uMat_shininess, 16.0f);
}

void setMaterialGlass(AppContext *ctx, GLfloat alpha)
{
    assert(ctx != NULL);
    assert(alpha >= 0.f && alpha <= 1.f);

    glUniform4f(ctx->uMat_emission, 0, 0, 0, 1);
    glUniform4f(ctx->uMat_ambient, 0.02f, 0.02f, 0.03f, 1);
    glUniform4f(ctx->uMat_diffuse, 1.0, 0.00f, 1.0f, alpha);
    glUniform4f(ctx->uMat_specular, 1, 1, 1, 1);
    glUniform1f(ctx->uMat_shininess, 256.0f);
}
void setMaterialGrass(AppContext *ctx)
{
    assert(ctx != NULL);

    glUniform4f(ctx->uMat_emission, 0.0f, 0.0f, 0.0f, 1.0f);
    glUniform4f(ctx->uMat_ambient, 0.05f, 0.20f, 0.05f, 1.0f);
    glUniform4f(ctx->uMat_diffuse, 0.10f, 0.50f, 0.10f, 1.0f);
    glUniform4f(ctx->uMat_specular, 0.02f, 0.05f, 0.02f, 1.0f);
    glUniform1f(ctx->uMat_shininess, 5.0f);
}
void setMaterialWood(AppContext *ctx)
{
    assert(ctx != NULL);

    glUniform4f(ctx->uMat_emission, 0.00f, 0.00f, 0.00f, 1.0f);
    glUniform4f(ctx->uMat_ambient, 0.08f, 0.05f, 0.03f, 1.0f);
    glUniform4f(ctx->uMat_diffuse, 0.45f, 0.30f, 0.15f, 1.0f);
    glUniform4f(ctx->uMat_specular, 0.04f, 0.03f, 0.02f, 1.0f);
    glUniform1f(ctx->uMat_shininess, 12.0f);
}

void setMaterialTree(AppContext *ctx)
{
    assert(ctx != NULL);

    glUniform4f(ctx->uMat_emission, 0.00f, 0.00f, 0.00f, 1.0f);
    glUniform4f(ctx->uMat_ambient, 0.06f, 0.08f, 0.06f, 1.0f);
    glUniform4f(ctx->uMat_diffuse, 0.22f, 0.35f, 0.18f, 1.0f);
    glUniform4f(ctx->uMat_specular, 0.03f, 0.03f, 0.03f, 1.0f);
    glUniform1f(ctx->uMat_shininess, 10.0f);
}
void setMaterialSlenderman(AppContext *ctx)
{
    assert(ctx != NULL);

    glUniform4f(ctx->uMat_emission, 0.00f, 0.00f, 0.00f, 1.0f);
    glUniform4f(ctx->uMat_ambient, 0.02f, 0.02f, 0.02f, 1.0f);
    glUniform4f(ctx->uMat_diffuse, 0.03f, 0.03f, 0.03f, 1.0f);
    glUniform4f(ctx->uMat_specular, 0.12f, 0.12f, 0.12f, 1.0f);
    glUniform1f(ctx->uMat_shininess, 64.0f);
}
