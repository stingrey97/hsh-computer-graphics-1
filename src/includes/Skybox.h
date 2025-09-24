#ifndef SKYBOX_H
#define SKYBOX_H

#include "AppContext.h"

// Initializes the skybox (loads textures, sets up buffers)
extern void initSkybox(AppContext *context);

// Draws the skybox using the given view and projection matrices
extern void drawSkybox(AppContext *ctx, GLfloat *V, GLfloat *P);

#endif
