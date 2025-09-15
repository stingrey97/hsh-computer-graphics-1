// Self
#include "UniformLoader.h"

// Standard libs
#include <assert.h>

// OpenGL
#include <GL/glew.h>

// Onw libs
#include "AppContext.h"

void loadUniforms(AppContext *ctx)
{
    assert(ctx != NULL);

    // einmal die Location holen Matrizen
    ctx->MVLoc = glGetUniformLocation(ctx->programID, "MV");
    ctx->MVPLoc = glGetUniformLocation(ctx->programID, "MVP");
    ctx->NormalMLoc = glGetUniformLocation(ctx->programID, "NormalM");

    // Get uniforms vor materials
    ctx->uMat_emission = glGetUniformLocation(ctx->programID, "material.emission");
    ctx->uMat_ambient = glGetUniformLocation(ctx->programID, "material.ambient");
    ctx->uMat_diffuse = glGetUniformLocation(ctx->programID, "material.diffuse");
    ctx->uMat_specular = glGetUniformLocation(ctx->programID, "material.specular");
    ctx->uMat_shininess = glGetUniformLocation(ctx->programID, "material.shininess");

    // Richtungslicht
    ctx->uSun_enabled = glGetUniformLocation(ctx->programID, "richtungslicht.enabled");
    ctx->uSun_direction = glGetUniformLocation(ctx->programID, "richtungslicht.direction");
    ctx->uSun_ambient = glGetUniformLocation(ctx->programID, "richtungslicht.ambient");
    ctx->uSun_diffuse = glGetUniformLocation(ctx->programID, "richtungslicht.diffuse");
    ctx->uSun_specular = glGetUniformLocation(ctx->programID, "richtungslicht.specular");

    // Punktlicht
    ctx->uLamp_enabled = glGetUniformLocation(ctx->programID, "punktlicht.enabled");
    ctx->uLamp_position = glGetUniformLocation(ctx->programID, "punktlicht.position");
    ctx->uLamp_ambient = glGetUniformLocation(ctx->programID, "punktlicht.ambient");
    ctx->uLamp_diffuse = glGetUniformLocation(ctx->programID, "punktlicht.diffuse");
    ctx->uLamp_specular = glGetUniformLocation(ctx->programID, "punktlicht.specular");
    ctx->uLamp_linear = glGetUniformLocation(ctx->programID, "punktlicht.linear");
    ctx->uLamp_quadratic = glGetUniformLocation(ctx->programID, "punktlicht.quadratic");

    // Spotlicht
    ctx->uSpot_enabled = glGetUniformLocation(ctx->programID, "spotlicht.enabled");
    ctx->uSpot_position = glGetUniformLocation(ctx->programID, "spotlicht.position");
    ctx->uSpot_direction = glGetUniformLocation(ctx->programID, "spotlicht.direction");
    ctx->uSpot_innerCone = glGetUniformLocation(ctx->programID, "spotlicht.innerCone");
    ctx->uSpot_outerCone = glGetUniformLocation(ctx->programID, "spotlicht.outerCone");
    ctx->uSpot_ambient = glGetUniformLocation(ctx->programID, "spotlicht.ambient");
    ctx->uSpot_diffuse = glGetUniformLocation(ctx->programID, "spotlicht.diffuse");
    ctx->uSpot_specular = glGetUniformLocation(ctx->programID, "spotlicht.specular");
    ctx->uSpot_linear = glGetUniformLocation(ctx->programID, "spotlicht.linear");
    ctx->uSpot_quadratic = glGetUniformLocation(ctx->programID, "spotlicht.quadratic");

    // Nebel
    ctx->uFogColor = glGetUniformLocation(ctx->programID, "nebel.color");
    ctx->uFogDensity = glGetUniformLocation(ctx->programID, "nebel.density");
    ctx->uFogEnabled = glGetUniformLocation(ctx->programID, "nebel.enabled");
}
