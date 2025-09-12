// Self
#include "Assertions.h"

// Standard libs
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>

// OpenGL
#include <GL/glew.h>

// Own libs
#include "AppContext.h"

int isValidVector3f(const GLfloat vec[3])
{
    return isfinite(vec[0]) && isfinite(vec[1]) && isfinite(vec[2]);
}

int isNotZeroVector(const GLfloat vec[3])
{
    return fabsf(vec[0]) + fabsf(vec[1]) + fabsf(vec[2]) > 0.0f;
}

int isValidMatrix16f(const GLfloat *mat)
{
    for (int i = 0; i < 16; i++)
    {
        if (!isfinite(mat[i]))
        {
            return false;
        }
    }
    return true;
}

int vectorsAreDifferent(const GLfloat a[3], const GLfloat b[3])
{
    assert(a != NULL);
    assert(b != NULL);

    const GLfloat dx = a[0] - b[0];
    const GLfloat dy = a[1] - b[1];
    const GLfloat dz = a[2] - b[2];

    const GLfloat dist2 = dx * dx + dy * dy + dz * dz;

    return dist2 > EPS;
}
