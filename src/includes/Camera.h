#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>

#include "AppContext.h"

// Initializes the camera settings and state within the given application context
extern void initCamera(AppContext *context);

// Updates the camera view (V) and projection (P) matrices based on the current context
extern void camera(GLfloat *V, GLfloat *P, AppContext *context);

#endif
