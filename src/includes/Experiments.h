#ifndef EXPERIMENTE_H
#define EXPERIMENTE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Holds the current status of various render options (lights, fog, fullscreen)
typedef struct
{
    int sun;       // Directional light enabled/disabled
    int lamp;      // Point light enabled/disabled
    int spot;      // Spotlight enabled/disabled
    int nebel;     // Fog enabled/disabled
    int vollbild;  // Fullscreen enabled/disabled
} Status;

// Toggles directional light, point light, and spotlight states
extern void lichtSchalter(GLint sun, GLint lamp, GLint spot, GLFWwindow *window, Status *status);

// Toggles fog rendering state
extern void nebelSchalter(GLint fogEnabled, GLFWwindow *window, Status *status);

// Toggles fullscreen mode
extern void vollbildschalter(GLFWwindow *window, Status *status);

// Toggles reflection rendering state
extern void reflektionSchalter(GLFWwindow* ctx, int* enable);

#endif
