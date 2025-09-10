#include "lightUtils.h"

void initializeDirectionalLight(GLint uSun_ambient, GLint uSun_diffuse, GLint uSun_specular)
{
    glUniform4f(uSun_ambient, 0.14f, 0.16f, 0.22f, 1.0f);
    glUniform4f(uSun_diffuse, 0.28f, 0.32f, 0.44f, 1.0f);
    glUniform4f(uSun_specular, 0.07f, 0.08f, 0.12f, 1.0f);
}

void initializePointLight(GLint uLamp_ambient, GLint uLamp_diffuse, GLint uLamp_specular, GLint uLamp_linear, GLint uLamp_quadratic)
{
    glUniform4f(uLamp_ambient, 0.25f, 0.25f, 0.25f, 1.0f);
    glUniform4f(uLamp_diffuse, 1.0f, 1.0f, 1.0f, 1.0f);
    glUniform4f(uLamp_specular, 1.0f, 1.0f, 1.0f, 1.0f);
    // Abschwächung (ruhig sanft)
    glUniform1f(uLamp_linear, 0.007f);
    glUniform1f(uLamp_quadratic, 0.0017f);
}

void initializeSpotLight(GLint uSpot_ambient, GLint uSpot_diffuse, GLint uSpot_specular, GLint uSpot_innerCone, GLint uSpot_outerCone, GLint uSpot_linear, GLint uSpot_quadratic)
{
    glUniform4f(uSpot_ambient, 0.1, 0.1f, 0.1f, 1.0f);
    glUniform4f(uSpot_diffuse, 2.00f, 2.00f, 2.00f, 1.0f);
    glUniform4f(uSpot_specular, 2.00f, 2.00f, 2.00f, 1.0f);

    glUniform1f(uSpot_linear, 0.002f);
    glUniform1f(uSpot_quadratic, 0.00002f);

    float innerDeg = 11.0f, outerDeg = 18.0f;
    float innerRad = innerDeg * (float)PI / 180.0f;
    float outerRad = outerDeg * (float)PI / 180.0f;

    glUniform1f(uSpot_innerCone, cosf(innerRad));
    glUniform1f(uSpot_outerCone, cosf(outerRad));
}

void setPointLight(GLint uLamp_position, const float viewMatrix[16], float x, float y, float z)
{
    GLfloat lightW[3] = {x, y, z}; // deine Welt-Pos
    GLfloat lightV[3];
    transform_point_view(lightV, viewMatrix, lightW);
    glUniform4f(uLamp_position, lightV[0], lightV[1], lightV[2], 1.0f);
}

void setSpotLight(GLint uSpot_position, GLint uSpot_direction, const float viewMatrix[16], const float eye[3], const float center[3])
{
    // 1) Position: Kamera-Position -> View-Space
    GLfloat posV[3];
    transform_point_view(posV, viewMatrix, eye);
    // 2) Richtung: center - eye (Welt), dann normalisieren
    GLfloat dirW[3];
    minus3f(dirW, center, eye);
    norm3f(dirW, dirW);
    // 3) Richtung in View-Space drehen (nur 3x3 Rotation)
    GLfloat dirV[3];
    dirV[0] = viewMatrix[0] * dirW[0] + viewMatrix[4] * dirW[1] + viewMatrix[8] * dirW[2];
    dirV[1] = viewMatrix[1] * dirW[0] + viewMatrix[5] * dirW[1] + viewMatrix[9] * dirW[2];
    dirV[2] = viewMatrix[2] * dirW[0] + viewMatrix[6] * dirW[1] + viewMatrix[10] * dirW[2];
    norm3f(dirV, dirV);
    // 4) Ab in den Shader
    glUniform3fv(uSpot_position, 1, posV);
    glUniform3fv(uSpot_direction, 1, dirV);
}

void setDirectionalLight(GLint uSun_direction, const float viewMatrix[16], float x, float y, float z)
{
    float dirW[3] = {x, y, z};
    norm3f(dirW, dirW); // normalisieren

    // In den View-Space drehen: nur die 3x3-Rotation der View-Matrix anwenden, translation ignorieren
    float dirV[3];
    dirV[0] = viewMatrix[0] * dirW[0] + viewMatrix[4] * dirW[1] + viewMatrix[8] * dirW[2];
    dirV[1] = viewMatrix[1] * dirW[0] + viewMatrix[5] * dirW[1] + viewMatrix[9] * dirW[2];
    dirV[2] = viewMatrix[2] * dirW[0] + viewMatrix[6] * dirW[1] + viewMatrix[10] * dirW[2];
    norm3f(dirV, dirV); // sicher normalisieren

    // An den Shader senden
    glUniform3f(uSun_direction, dirV[0], dirV[1], dirV[2]);
}