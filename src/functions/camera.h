#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "AppContext.h"

# ifndef CAMERA_H
# define CAMERA_H

extern void initCamera(AppContext *context);
extern void camera(GLfloat *V, GLfloat *P, AppContext *context);

# endif
