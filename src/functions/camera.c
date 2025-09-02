#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "camera.h"

// MousePositions
int xMousePos;
int yMousePos;

double lastTime;

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

void Camera(WindowData winData) {
    glfwGetMousePos(&xMousePos, &yMousePos);
    glfwSetMousePos(winData.xWindowSize/2, winData.yWindowSize/2);

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

    // Move forward
    if (glfwGetKey(winData.window, GLFW_KEY_UP ) == GLFW_PRESS){
        position += direction * deltaTime * speed;
    }
    // Move backward
    if (glfwGetKey(winData.window, GLFW_KEY_DOWN ) == GLFW_PRESS){
        position -= direction * deltaTime * speed;
    }
    // Strafe right
    if (glfwGetKey(winData.window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
        position += right * deltaTime * speed;
    }
    // Strafe left
    if (glfwGetKey(winData.window, GLFW_KEY_LEFT ) == GLFW_PRESS){
        position -= right * deltaTime * speed;
    }
}