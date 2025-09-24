// Self
#include "Camera.h"

// Standard libs
#include <stdio.h>
#include <math.h>
#include <assert.h>

// OpenGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Own libs
#include "Constants.h"
#include "AppContext.h"
#include "MathUtils.h"

void initCamera(AppContext *context)
{
    assert(context->window != NULL);

    glfwSetInputMode(context->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(context->window, context->width / 2, context->height / 2);
    glfwPollEvents();

    copyVec3(context->INITIAL_EYE, context->eye);
    copyVec3(context->INITIAL_LOOK, context->look);
    copyVec3(context->INITIAL_UP, context->up);
}

void clamp(GLfloat *out, GLfloat value)
{
    assert(out != NULL);

    if (*out > value)
        *out = value;
    if (*out < -value)
        *out = -value;
}

void camera(GLfloat *V, GLfloat *P, AppContext *context)
{
    assert(V && P && context && context->window);

    static short isInitialized = 0;
    static double xMousePos, yMousePos;
    static GLfloat horizontalAngle = 0.f;
    static GLfloat verticalAngle = 0.f;
    static double lastTime = 0;

    if (isInitialized == 0)
    {
        lastTime = glfwGetTime();
        glfwSetCursorPos(context->window, context->width / 2, context->height / 2);
        xMousePos = context->width * 0.5;
        yMousePos = context->height * 0.5;
        horizontalAngle = START_HORIZONTAL_ANGLE;
        verticalAngle = START_VERTICAL_ANGLE;
    }

    glfwGetCursorPos(context->window, &xMousePos, &yMousePos);
    glfwSetCursorPos(context->window, context->width / 2, context->height / 2);
    
    const double currentTime = glfwGetTime();
    const GLfloat deltaTime = (GLfloat)(currentTime - lastTime);
    lastTime = currentTime;

    horizontalAngle += MOUSE_SPEED * deltaTime * (GLfloat)(context->width / 2 - xMousePos);
    verticalAngle += MOUSE_SPEED * deltaTime * (GLfloat)(context->height / 2 - yMousePos);
    clamp(&verticalAngle, MAX_PITCH);

    if (isInitialized == 0)
    {
        copyVec3(context->eye, context->INITIAL_EYE);
        copyVec3(context->up, context->INITIAL_UP);

        GLfloat initDir[3];
        minus3f(initDir, context->INITIAL_LOOK, context->INITIAL_EYE);
        if (len3f(initDir) > EPS)
        {
            norm3f(initDir, initDir);
            verticalAngle = asinf(initDir[1]);
            horizontalAngle = atan2f(initDir[0], initDir[2]);
        }
        isInitialized = 1;
    }

    GLfloat dir[3];
    setVec3(dir,
            cosf(verticalAngle) * sinf(horizontalAngle),
            sinf(verticalAngle),
            cosf(verticalAngle) * cosf(horizontalAngle));
    if (len3f(dir) > EPS)
        norm3f(dir, dir);

    GLfloat dir2[3];
    setVec3(dir2,
            cosf(verticalAngle) * sinf(horizontalAngle),
            0.f,
            cosf(verticalAngle) * cosf(horizontalAngle));
    if (len3f(dir2) > EPS)
        norm3f(dir2, dir2);

    GLfloat right[3] = {
        sinf(horizontalAngle - 3.14159265f * 0.5f),
        0.f,
        cosf(horizontalAngle - 3.14159265f * 0.5f)};
    if (len3f(right) > EPS)
        norm3f(right, right);

    float speedModyfier = KEY_SPEED * ((glfwGetKey(context->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? SPRINT_SPEED_MODYFIER : 1.f);
    
    if (glfwGetKey(context->window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(context->window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        GLfloat step[3];
        multiply3f(step, dir2, deltaTime * speedModyfier);
        step[1] = 0.f;
        plus3f(context->eye, context->eye, step);
    }
    if (glfwGetKey(context->window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(context->window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        GLfloat step[3];
        multiply3f(step, dir2, deltaTime * speedModyfier);
        step[1] = 0.f;
        minus3f(context->eye, context->eye, step);
    }
    if (glfwGetKey(context->window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(context->window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        GLfloat step[3];
        multiply3f(step, right, deltaTime * speedModyfier);
        step[1] = 0.f;
        plus3f(context->eye, context->eye, step);
    }
    if (glfwGetKey(context->window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(context->window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        GLfloat step[3];
        multiply3f(step, right, deltaTime * speedModyfier);
        step[1] = 0.f;
        minus3f(context->eye, context->eye, step);
    }

#ifdef FIXED_CAMERA
    static float verticalSpeed;
    float baseHeight = CAMERA_HEIGHT;
    if (glfwGetKey(context->window, GLFW_KEY_C) == GLFW_PRESS ||
        glfwGetKey(context->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        baseHeight = CAMERA_HEIGHT / 2.f;
    }

    if (glfwGetKey(context->window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        if (fabsf(context->eye[1] - baseHeight) < 0.001f)
        {
            verticalSpeed = JUMP_FORCE;
        }
    }

    verticalSpeed -= 9.81f * deltaTime;
    context->eye[1] += verticalSpeed * deltaTime;

    if (context->eye[1] < baseHeight)
    {
        context->eye[1] = baseHeight;
        verticalSpeed = 0.f;
    }

#ifdef DEBUG_MODE
    printf("Höhe: %.2f\n", context->eye[1]);
#endif
#endif

    GLfloat center[3];
    plus3f(center, context->eye, dir);

    lookAt(V, context->eye, center, context->up);
    perspective(P, FoV, (GLfloat)context->width / (GLfloat)context->height, 0.1f, 100.0f);

    copyVec3(context->look, center);

#ifdef DEBUG_MODE
    printf("horizontal:   %f\n", horizontalAngle);
    printf("vertical:     %f\n", verticalAngle);
    printf("dir:          %f,%f,%f\n", dir[0], dir[1], dir[2]);
    printf("eye:          %f,%f,%f\n", context->eye[0], context->eye[1], context->eye[2]);
    printf("center:       %f,%f,%f\n", center[0], center[1], center[2]);
#endif
}
