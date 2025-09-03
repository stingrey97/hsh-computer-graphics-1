#include "lightUtils.h"

void initializeDirectionalLight (GLint uSun_ambient, GLint uSun_diffuse, GLint uSun_specular)
{
    glUniform4f(uSun_ambient, 0.35f, 0.35f, 0.35f, 1.0f);
    glUniform4f(uSun_diffuse, 1.0f, 1.0f, 1.0f, 1.0f);
    glUniform4f(uSun_specular, 1.0f, 1.0f, 1.0f, 1.0f);
}

void initializePointLight(GLint uLamp_ambient, GLint uLamp_diffuse, GLint uLamp_specular, GLint uLamp_linear, GLint uLamp_quadratic)
{
    glUniform4f(uLamp_ambient, 0.2f, 0.2f, 0.2f, 1.0f);
    glUniform4f(uLamp_diffuse, 1.0f, 1.0f, 1.0f, 1.0f);
    glUniform4f(uLamp_specular, 1.0f, 1.0f, 1.0f, 1.0f);
    // Abschwächung (ruhig sanft)
    glUniform1f(uLamp_linear, 0.007f);
    glUniform1f(uLamp_quadratic, 0.0017f);
}

void initializeSpotLight(GLint uSpot_ambient, GLint uSpot_diffuse, GLint uSpot_specular, GLint uSpot_innerCone, GLint uSpot_outerCone)
{
    glUniform4f(uSpot_ambient, 0.10f, 0.10f, 0.10f, 1.0f);
    glUniform4f(uSpot_diffuse, 1.00f, 1.00f, 1.00f, 1.0f);
    glUniform4f(uSpot_specular, 1.00f, 1.00f, 1.00f, 1.0f);

    float innerDeg = 12.0f, outerDeg = 20.0f;
    float innerRad = innerDeg * (float)M_PI / 180.0f;
    float outerRad = outerDeg * (float)M_PI / 180.0f;

    glUniform1f(uSpot_innerCone, cosf(innerRad));
    glUniform1f(uSpot_outerCone, cosf(innerRad));
}