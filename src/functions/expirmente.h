#ifndef EXPIRMENTE_H
#define EXPIRMENTE_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

typedef struct
{
    int sun;
    int lamp;
    int spot;
} Status;
extern void lichtSchalter(GLint sun, GLint lamp, GLint spot, GLFWwindow *window, Status *status);

#endif