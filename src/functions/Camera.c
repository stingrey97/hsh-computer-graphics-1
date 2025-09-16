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

    // Maus verstecken
    glfwSetInputMode(context->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // Maus in mitte setzen
    glfwSetCursorPos(context->window, context->width / 2, context->height / 2);
    // clear Events
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

    // Maus-Reset beim ersten Aufruf (verhindert großen ersten Delta-Sprung)
    if (isInitialized == 0)
    {
        lastTime = glfwGetTime();
        glfwSetCursorPos(context->window, context->width / 2, context->height / 2);
        xMousePos = context->width * 0.5;
        yMousePos = context->height * 0.5;
        // Startwinkel aus Konstanten
        horizontalAngle = START_HORIZONTAL_ANGLE;
        verticalAngle = START_VERTICAL_ANGLE;
    }

    // Maus lesen & zurück in Mitte setzen
    glfwGetCursorPos(context->window, &xMousePos, &yMousePos);
    glfwSetCursorPos(context->window, context->width / 2, context->height / 2);

    // Delta-Zeit
    const double currentTime = glfwGetTime();
    const GLfloat deltaTime = (GLfloat)(currentTime - lastTime);
    lastTime = currentTime;

    // Winkel-Update (Yaw/Pitch)
    horizontalAngle += MOUSE_SPEED * deltaTime * (GLfloat)(context->width / 2 - xMousePos);
    verticalAngle += MOUSE_SPEED * deltaTime * (GLfloat)(context->height / 2 - yMousePos);
    clamp(&verticalAngle, MAX_PITCH);

    // Einmalige Initialisierung: Eye/Up übernehmen; optional Startblick aus INITIAL_LOOK ableiten
    if (isInitialized == 0)
    {
        copyVec3(context->eye, context->INITIAL_EYE);
        copyVec3(context->up, context->INITIAL_UP);

        // Falls INITIAL_LOOK als Punkt (Center) gesetzt ist, Richtungswinkel daraus ableiten
        GLfloat initDir[3];
        minus3f(initDir, context->INITIAL_LOOK, context->INITIAL_EYE); // dir ~ look - eye
        if (len3f(initDir) > EPS)
        {
            norm3f(initDir, initDir);
            // Winkel aus dir rekonstruieren
            verticalAngle = asinf(initDir[1]);
            horizontalAngle = atan2f(initDir[0], initDir[2]); // x,z -> yaw
        }
        isInitialized = 1;
    }

    // Blickrichtung aus Winkeln (dir = Vorwärts-Vektor)
    GLfloat dir[3];
    setVec3(dir,
            cosf(verticalAngle) * sinf(horizontalAngle),
            sinf(verticalAngle),
            cosf(verticalAngle) * cosf(horizontalAngle));
    // numerisch stabil halten
    if (len3f(dir) > EPS)
        norm3f(dir, dir);

    // Strafe-Richtung (rechts) nur aus Yaw (keine Roll-Komponente)
    GLfloat right[3] = {
        sinf(horizontalAngle - 3.14159265f * 0.5f),
        0.f,
        cosf(horizontalAngle - 3.14159265f * 0.5f)};
    if (len3f(right) > EPS)
        norm3f(right, right);

    // Bewegung: W/S vor-zurück entlang dir, A/D seitlich entlang right
    if (glfwGetKey(context->window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(context->window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        GLfloat step[3];
        multiply3f(step, dir, deltaTime * KEY_SPEED);
        plus3f(context->eye, context->eye, step);
    }
    if (glfwGetKey(context->window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(context->window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        GLfloat step[3];
        multiply3f(step, dir, deltaTime * KEY_SPEED);
        minus3f(context->eye, context->eye, step);
    }
    if (glfwGetKey(context->window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(context->window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        GLfloat step[3];
        multiply3f(step, right, deltaTime * KEY_SPEED);
        plus3f(context->eye, context->eye, step);
    }
    if (glfwGetKey(context->window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(context->window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        GLfloat step[3];
        multiply3f(step, right, deltaTime * KEY_SPEED);
        minus3f(context->eye, context->eye, step);
    }

#ifdef FIXED_CAMERA
    context->eye[1] = CAMERA_HEIGHT;
#endif

    // Center = eye + dir  (Punkt!), genau das erwartet lookAt nach Vorlesung
    GLfloat center[3];
    plus3f(center, context->eye, dir);

    // View & Projection berechnen
    lookAt(V, context->eye, center, context->up);
    static GLfloat FoV = FOV; // ggf. später per Mausrad anpassen
    perspective(P, FoV, (GLfloat)context->width / (GLfloat)context->height, 0.1f, 100.0f);

    // Optional: für Debug-Zwecke context->look als Center mitführen (Punkt, nicht Richtung)
    copyVec3(context->look, center);

#ifdef DEBUG_MODE
    printf("horizontal:   %f\n", horizontalAngle);
    printf("vertical:     %f\n", verticalAngle);
    printf("dir:          %f,%f,%f\n", dir[0], dir[1], dir[2]);
    printf("eye:          %f,%f,%f\n", context->eye[0], context->eye[1], context->eye[2]);
    printf("center:       %f,%f,%f\n", center[0], center[1], center[2]);
#endif
}
