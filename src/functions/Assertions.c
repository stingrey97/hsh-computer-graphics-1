// Self
#include "Assertions.h"

// Standard libs
#include <assert.h>
#include <math.h>
#include <stdbool.h>

// OpenGL
#include <GL/glew.h>

// Own libs
#include "Constants.h"
#include "AppContext.h"

// Macro for uniforms, must not be -1
#define REQ_LOC(loc_) assert((loc_) >= 0)

int isValidVector3f(const GLfloat vec[3])
{
    assert(vec != NULL);
    return isfinite(vec[0]) && isfinite(vec[1]) && isfinite(vec[2]);
}

int isValidVector4f(const GLfloat vec[4])
{
    assert(vec != NULL);
    return isfinite(vec[0]) && isfinite(vec[1]) && isfinite(vec[2]) && isfinite(vec[3]);
}

int isNotZeroVector(const GLfloat vec[3])
{
    assert(vec != NULL);
    return fabsf(vec[0]) + fabsf(vec[1]) + fabsf(vec[2]) > 0.0f;
}

int isValidMatrix16f(const GLfloat *mat)
{
    assert(mat != NULL);
    for (int i = 0; i < 16; i++)
        if (!isfinite(mat[i]))
            return false;
    return true;
}

int isValidMatrix9f(const GLfloat *mat)
{
    assert(mat != NULL);
    for (int i = 0; i < 9; i++)
        if (!isfinite(mat[i]))
            return false;
    return true;
}

int vectorsAreDifferent(const GLfloat a[3], const GLfloat b[3])
{
    assert(a != NULL && b != NULL);
    const GLfloat dx = a[0] - b[0];
    const GLfloat dy = a[1] - b[1];
    const GLfloat dz = a[2] - b[2];
    const GLfloat dist2 = dx * dx + dy * dy + dz * dz;
    return dist2 > EPS;
}

static inline int vectorsNotParallel(const GLfloat a[3], const GLfloat b[3])
{
    const GLfloat cx = a[1] * b[2] - a[2] * b[1];
    const GLfloat cy = a[2] * b[0] - a[0] * b[2];
    const GLfloat cz = a[0] * b[1] - a[1] * b[0];
    const GLfloat mag2 = cx * cx + cy * cy + cz * cz;
    return mag2 > EPS;
}

int contextIsValidAndComplete(const AppContext *ctx)
{
    assert(ctx != NULL);

    // Window
    assert(ctx->window != NULL);
    assert(ctx->width > 0);
    assert(ctx->height > 0);

    // Shader Programme/Objekte
    assert(ctx->programID != 0);
    assert(ctx->skyboxProgramID != 0);
    assert(ctx->skyboxVAO != 0);
    assert(ctx->skyboxTexture != 0);

    // Kamera-Vektoren
    assert(isValidVector3f(ctx->eye));
    assert(isValidVector3f(ctx->look));
    assert(isValidVector3f(ctx->up));
    assert(vectorsAreDifferent(ctx->eye, ctx->look));
    assert(isNotZeroVector(ctx->up));

    // Forward = look - eye darf nicht parallel zu up sein
    const GLfloat forward[3] = {
        ctx->look[0] - ctx->eye[0],
        ctx->look[1] - ctx->eye[1],
        ctx->look[2] - ctx->eye[2]};
    assert(isNotZeroVector(forward));
    assert(vectorsNotParallel(forward, ctx->up));

    // Uniform-Locations (dürfen 0 sein, aber nicht -1)
    REQ_LOC(ctx->MVLoc);
    REQ_LOC(ctx->MVPLoc);
    REQ_LOC(ctx->NormalMLoc);

    REQ_LOC(ctx->uMat_emission);
    REQ_LOC(ctx->uMat_ambient);
    REQ_LOC(ctx->uMat_diffuse);
    REQ_LOC(ctx->uMat_specular);
    REQ_LOC(ctx->uMat_shininess);

    REQ_LOC(ctx->uSun_enabled);
    REQ_LOC(ctx->uSun_direction);
    REQ_LOC(ctx->uSun_ambient);
    REQ_LOC(ctx->uSun_diffuse);
    REQ_LOC(ctx->uSun_specular);

    REQ_LOC(ctx->uLamp_enabled);
    REQ_LOC(ctx->uLamp_position);
    REQ_LOC(ctx->uLamp_ambient);
    REQ_LOC(ctx->uLamp_diffuse);
    REQ_LOC(ctx->uLamp_specular);
    REQ_LOC(ctx->uLamp_linear);
    REQ_LOC(ctx->uLamp_quadratic);

    REQ_LOC(ctx->uSpot_enabled);
    REQ_LOC(ctx->uSpot_position);
    REQ_LOC(ctx->uSpot_direction);
    REQ_LOC(ctx->uSpot_innerCone);
    REQ_LOC(ctx->uSpot_outerCone);
    REQ_LOC(ctx->uSpot_ambient);
    REQ_LOC(ctx->uSpot_diffuse);
    REQ_LOC(ctx->uSpot_specular);
    REQ_LOC(ctx->uSpot_linear);
    REQ_LOC(ctx->uSpot_quadratic);

    REQ_LOC(ctx->uFogColor);
    REQ_LOC(ctx->uFogDensity);
    REQ_LOC(ctx->uFogEnabled);

    return 1;
}
