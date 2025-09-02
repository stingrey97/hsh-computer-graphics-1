#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "camera.h"
#include "matrixUtils.h"
#include <math.h>

// MousePositions
double xMousePos;
double yMousePos;

// for deltaTime
double lastTime;

// glowballs
GLfloat PerspektiveMatrix[16];
GLfloat ViewMatrix[16];

// position
GLfloat position[3] = { 0, 0, 5 };

// horizontal angle : toward -Z
GLfloat horizontalAngle = 3.14f;

// vertical angle : 0, look at the horizon
GLfloat verticalAngle = 0.0f;

// Initial Field of View
GLfloat initialFoV = 45.0f;

GLfloat speed = 3.0f; // 3 units / second
GLfloat mouseSpeed = 0.005f;

void camera(GLfloat *out, WindowData winData) {
    glfwGetCursorPos(winData.window, &xMousePos, &yMousePos);

    glfwSetCursorPos(winData.window, winData.xWindowSize/2, winData.yWindowSize/2);

    //deltaTime
    double currentTime = glfwGetTime();
    GLfloat deltaTime = (GLfloat)(currentTime - lastTime);
    lastTime = currentTime;

    // Compute new orientation
    horizontalAngle += mouseSpeed * deltaTime * (GLfloat)(winData.xWindowSize/2 - xMousePos );
    verticalAngle   += mouseSpeed * deltaTime * (GLfloat)( winData.yWindowSize/2 - yMousePos );

    //apply new Direction
    GLfloat direction[3] = {
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    };

    GLfloat right[3] = {
        sin(horizontalAngle - 3.14f/2.0f),
        0,
        cos(horizontalAngle - 3.14f/2.0f)
    };

    // Move forward
    if (glfwGetKey(winData.window, GLFW_KEY_UP ) == GLFW_PRESS){
        GLfloat dir[3];
        multiply3f2(dir, direction, deltaTime, speed);
        plus3f(position, position, dir);
    }
    // Move backward
    if (glfwGetKey(winData.window, GLFW_KEY_DOWN ) == GLFW_PRESS){
        GLfloat dir[3];
        multiply3f2(dir, direction, deltaTime, speed);
        minus3f(position, position, dir);
    }
    // Strafe right
    if (glfwGetKey(winData.window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
        GLfloat ri[3];
        multiply3f2(ri, right, deltaTime, speed);
        plus3f(position, position, ri);
    }
    // Strafe left
    if (glfwGetKey(winData.window, GLFW_KEY_LEFT ) == GLFW_PRESS){
        GLfloat ri[3];
        multiply3f2(ri, right, deltaTime, speed);
        minus3f(position, position, ri);
    }

    GLfloat FoV = initialFoV - (GLfloat)(5 * 1);

    perspective(PerspektiveMatrix, FoV, winData.xWindowSize / winData.yWindowSize, 0.1f, 100.0f);

    plus3f(direction, direction, position);
    GLfloat up[3];
    cross3f(up, right, direction),

    lookAt(ViewMatrix, position, direction, up);

    mat4f_mul_mat4f(out, PerspektiveMatrix, ViewMatrix);
}