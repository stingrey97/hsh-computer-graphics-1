# ifndef CAMERA_H
# define CAMERA_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

typedef struct{
    GLFWwindow * window;
    int xWindowSize;
    int yWindowSize;
} WindowData;

extern WindowData windowData;
extern void Camera(GLfloat *out, WindowData winData);
//extern void loadOBJ(const char *filename, GLfloat *out, int *werte);
# endif