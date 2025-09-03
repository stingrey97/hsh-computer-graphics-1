#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "camera.h"
#include "matrixUtils.h"
#include <math.h>

// MousePositions
double xMousePos;
double yMousePos;

// glowballs
GLfloat PerspektiveMatrix[16];
GLfloat ViewMatrix[16];

// for deltaTime
static double lastTime;

// pitch Limit
const GLfloat PITCH_LIMIT = 1.55334306f; // 89° in Radiant

// position
GLfloat position[3];

// horizontal angle : toward -Z
GLfloat horizontalAngle = 3.14f;

// vertical angle : 0, look at the horizon
GLfloat verticalAngle = 0.0f;

static short start = 0;
static short debug = 1;

void initCamera(WindowData winData) {
    // Maus verstecken
    glfwSetInputMode(winData.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // Maus in mitte setzen
    glfwSetCursorPos(winData.window, winData.xWindowSize/2, winData.yWindowSize/2);
    // clear Events
    glfwPollEvents();
    // get lastTime
    lastTime = glfwGetTime();
}

void clamp(GLfloat *out, GLfloat value){
    if (*out >  value) *out =  value;
    if (*out < -value) *out = -value;
}

void camera(GLfloat *out, WindowData winData) {
    if(start == 0) {
        glfwSetCursorPos(winData.window, winData.xWindowSize/2, winData.yWindowSize/2);
    }

    glfwGetCursorPos(winData.window, &xMousePos, &yMousePos);
    glfwSetCursorPos(winData.window, winData.xWindowSize/2, winData.yWindowSize/2);

    //deltaTime
    double currentTime = glfwGetTime();
    GLfloat deltaTime = (GLfloat)(currentTime - lastTime);
    lastTime = currentTime;

    // Compute new orientation
    horizontalAngle += winData.cameraData.mouseSpeed * deltaTime * (GLfloat)(winData.xWindowSize/2 - xMousePos );
    verticalAngle   += winData.cameraData.mouseSpeed * deltaTime * (GLfloat)( winData.yWindowSize/2 - yMousePos );
    clamp(&verticalAngle, PITCH_LIMIT);

    if(start == 0) {
        horizontalAngle = winData.cameraData.startHorizontal;
        verticalAngle   = winData.cameraData.startVertical; 
    }

    if(debug == 1){
        printf("\n");
        printf("horizontal:   %f\n", horizontalAngle);
        printf("vertical:     %f\n", verticalAngle);
    }
    

    //apply new Direction
    GLfloat direction[3] = {
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    };
    
    if(debug == 1) printf("direction 1:  %f,%f,%f\n", direction[0], direction[1], direction[2]);

    GLfloat right[3] = {
        sin(horizontalAngle - 3.14f/2.0f),
        0,
        cos(horizontalAngle - 3.14f/2.0f)
    };

    // Move forward
    if (glfwGetKey(winData.window, GLFW_KEY_UP ) == GLFW_PRESS){
        GLfloat dir[3];
        multiply3f2(dir, direction, deltaTime, winData.cameraData.speed);
        plus3f(position, position, dir);
    }
    // Move backward
    if (glfwGetKey(winData.window, GLFW_KEY_DOWN ) == GLFW_PRESS){
        GLfloat dir[3];
        multiply3f2(dir, direction, deltaTime, winData.cameraData.speed);
        minus3f(position, position, dir);
    }
    // Strafe right
    if (glfwGetKey(winData.window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
        GLfloat ri[3];
        multiply3f2(ri, right, deltaTime, winData.cameraData.speed);
        plus3f(position, position, ri);
    }
    // Strafe left
    if (glfwGetKey(winData.window, GLFW_KEY_LEFT ) == GLFW_PRESS){
        GLfloat ri[3];
        multiply3f2(ri, right, deltaTime, winData.cameraData.speed);
        minus3f(position, position, ri);
    }

    GLfloat FoV = winData.cameraData.FoV - (GLfloat)(5 * 1);

    if(start == 0) {
        direction[0] = winData.cameraData.startDirection[0];
        direction[1] = winData.cameraData.startDirection[1];
        direction[2] = winData.cameraData.startDirection[2];

        position[0] = winData.cameraData.startPosition[0];
        position[1] = winData.cameraData.startPosition[1];
        position[2] = winData.cameraData.startPosition[2];

        start = 1;
    }

    perspective(PerspektiveMatrix, FoV, winData.xWindowSize / winData.yWindowSize, 0.1f, 100.0f);

    plus3f(direction, direction, position);

    GLfloat up[3] = {winData.cameraData.startUp[0], winData.cameraData.startUp[1], winData.cameraData.startUp[2]};

    lookAt(ViewMatrix, position, direction, up);

    if(debug == 1) {
        printf("position:     %f,%f,%f\n", position[0], position[1], position[2]);
        printf("direction 2:  %f,%f,%f\n", direction[0], direction[1], direction[2]);
        printf("start:        %i", start);
        printf("\n");
    }
    
    mat4f_mul_mat4f(out, PerspektiveMatrix, ViewMatrix);
}