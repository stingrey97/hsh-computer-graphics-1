#ifndef EXPIRMENTE_H
#define EXPIRMENTE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef struct
{
    int sun;
    int lamp;
    int spot;
    int nebel;
    int vollbild;

} Status;

extern void lichtSchalter(GLint sun, GLint lamp, GLint spot, GLFWwindow *window, Status *status);
extern void nebelSchalter(GLint fogEnabled, GLFWwindow *window, Status *status);
extern void vollbildschalter(GLFWwindow *window, Status *status);
extern void reflektionSchalter(GLFWwindow* ctx, int* enable);

#endif
