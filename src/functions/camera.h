# ifndef CAMERA_H
# define CAMERA_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

// Camera Settings
typedef struct {
    GLfloat FoV;
    GLfloat speed;
    GLfloat mouseSpeed;
    GLfloat startDirection[3];
    GLfloat startPosition[3];
    GLfloat startUp[3];
    GLfloat startHorizontal;
    GLfloat startVertical;
} CameraData;

//Window Settings
typedef struct{
    GLFWwindow * window;
    GLfloat xWindowSize;
    GLfloat yWindowSize;
    CameraData cameraData;
} WindowData;

extern WindowData windowData;

extern void initCamera(WindowData winData);
extern void camera(GLfloat *out, WindowData winData);
# endif