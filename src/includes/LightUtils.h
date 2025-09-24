#ifndef LIGHTUTILS_H
#define LIGHTUTILS_H

#include <GL/glew.h>

// Initializes a directional light (ambient, diffuse, specular components)
extern void initializeDirectionalLight(GLint uSun_ambient, GLint uSun_diffuse, GLint uSun_specular);

// Initializes a point light (ambient, diffuse, specular, attenuation parameters)
extern void initializePointLight(GLint uLamp_ambient, GLint uLamp_diffuse, GLint uLamp_specular,
                                 GLint uLamp_linear, GLint uLamp_quadratic);

// Initializes a spotlight (ambient, diffuse, specular, cone angles, attenuation parameters)
extern void initializeSpotLight(GLint uSpot_ambient, GLint uSpot_diffuse, GLint uSpot_specular,
                                GLint uSpot_innerCone, GLint uSpot_outerCone,
                                GLint uSpot_linear, GLint uSpot_quadratic);

// Sets the point light position in view space
extern void setPointLight(GLint uLamp_position, const float viewMatrix[16], float x, float y, float z);

// Sets the directional light direction in view space
extern void setDirectionalLight(GLint uSun_direction, const float viewMatrix[16], float x, float y, float z);

// Sets the spotlight position and direction in view space
extern void setSpotLight(GLint uSpot_position, GLint uSpot_direction,
                         const float viewMatrix[16],
                         const float eye[3], const float center[3]);

#endif
