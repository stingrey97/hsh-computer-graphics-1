#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>

#include "AppContext.h"

extern void initCamera(AppContext *context);
extern void camera(GLfloat *V, GLfloat *P, AppContext *context);

#endif
