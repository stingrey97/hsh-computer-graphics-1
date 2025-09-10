#include "camera.h"

#include <stdio.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "AppContext.h"
#include "matrixUtils.h"

#define MAX_PITCH 1.55334306f
#define FOV 45.0f
#define KEY_SPEED 3.0f
#define MOUSE_SPEED 0.05f
#define START_HORIZONTAL_ANGLE 4.72f
#define START_VERTICAL_ANGLE 0.0f
#define NO_DEBUG_MODE

static double lastTime;

static GLfloat INITIAL_EYE[3];
static GLfloat INITIAL_LOOK[3];
static GLfloat INITIAL_UP[3];

void initCamera(AppContext *context)
{
    // Maus verstecken
    glfwSetInputMode(context->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // Maus in mitte setzen
    glfwSetCursorPos(context->window, context->width / 2, context->height / 2);
    // clear Events
    glfwPollEvents();
    // get lastTime
    lastTime = glfwGetTime();

    copyVec3(INITIAL_EYE, context->eye);
    copyVec3(INITIAL_LOOK, context->look);
    copyVec3(INITIAL_UP, context->up);
}

void clamp(GLfloat *out, GLfloat value)
{
    if (*out > value)
        *out = value;
    if (*out < -value)
        *out = -value;
}

void camera(GLfloat *V, GLfloat *P, AppContext *context)
{
    static short isInitialized = 0;

    if (isInitialized == 0)
    {
        glfwSetCursorPos(context->window, context->width / 2, context->height / 2);
    }

    static double xMousePos;
    static double yMousePos;

    static GLfloat horizontalAngle;
    static GLfloat verticalAngle;

    glfwGetCursorPos(context->window, &xMousePos, &yMousePos);
    glfwSetCursorPos(context->window, context->width / 2, context->height / 2);

    // Calculate deltaTime
    double currentTime = glfwGetTime();
    GLfloat deltaTime = (GLfloat)(currentTime - lastTime);
    lastTime = currentTime;

    // Compute new orientation
    horizontalAngle += MOUSE_SPEED * deltaTime * (GLfloat)(context->width / 2 - xMousePos);
    verticalAngle += MOUSE_SPEED * deltaTime * (GLfloat)(context->height / 2 - yMousePos);
    clamp(&verticalAngle, MAX_PITCH);

    if (isInitialized == 0)
    {
        horizontalAngle = START_HORIZONTAL_ANGLE;
        verticalAngle = START_VERTICAL_ANGLE;
    }

    // apply new Direction
    setVec3(context->look,
            cos(verticalAngle) * sin(horizontalAngle),
            sin(verticalAngle),
            cos(verticalAngle) * cos(horizontalAngle));

    // Vector pointing right (u)
    GLfloat right[3] = {
        sin(horizontalAngle - 3.14f / 2.0f),
        0,
        cos(horizontalAngle - 3.14f / 2.0f)};

    // Move forward
    if (glfwGetKey(context->window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(context->window, GLFW_KEY_W) == GLFW_PRESS)
    {
        GLfloat dir[3];
        multiply3f(dir, context->look, deltaTime * KEY_SPEED);
        plus3f(context->eye, context->eye, dir);
    }
    // Move backward
    if (glfwGetKey(context->window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(context->window, GLFW_KEY_S) == GLFW_PRESS)
    {
        GLfloat dir[3];
        multiply3f(dir, context->look, deltaTime * KEY_SPEED);
        minus3f(context->eye, context->eye, dir);
    }
    // Strafe right
    if (glfwGetKey(context->window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(context->window, GLFW_KEY_D) == GLFW_PRESS)
    {
        GLfloat ri[3];
        multiply3f(ri, right, deltaTime * KEY_SPEED);
        plus3f(context->eye, context->eye, ri);
    }
    // Strafe left
    if (glfwGetKey(context->window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(context->window, GLFW_KEY_A) == GLFW_PRESS)
    {
        GLfloat ri[3];
        multiply3f(ri, right, deltaTime * KEY_SPEED);
        minus3f(context->eye, context->eye, ri);
    }

    // In case we want to add zoom by mousewheel later (change "0" to mousewheel)
    static GLfloat FoV = FOV - (GLfloat)(5 * 0);

    if (isInitialized == 0)
    {
        copyVec3(context->eye, INITIAL_EYE);
        copyVec3(context->look, INITIAL_LOOK);
        copyVec3(context->up, INITIAL_UP);
        
        isInitialized = 1;
    }

    plus3f(context->look, context->look, context->eye);

    lookAt(V, context->eye, context->look, context->up);
    
    perspective(P, FoV, (GLfloat)context->width / (GLfloat)context->height, 0.1f, 100.0f);

#ifdef DEBUG_MODE
    printf("horizontal:   %f\n", horizontalAngle);
    printf("vertical:     %f\n", verticalAngle);
    printf("direction:    %f,%f,%f\n", context->look[0], context->look[1], context->look[2]);
    printf("position:     %f,%f,%f\n", context->eye[0], context->eye[1], context->eye[2]);
    printf("start:        %i", isInitialized);
    printf("\n");
#endif
}
