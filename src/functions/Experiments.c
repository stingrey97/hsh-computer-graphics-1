// Self
#include "Experiments.h"

// Standard libs
#include <assert.h>

// OpenGL
#include <GLFW/glfw3.h>

void lichtSchalter(GLint sun, GLint lamp, GLint spot, GLFWwindow *window, Status *status)
{
    assert(sun >= 0 && lamp >= 0);
    assert(window != NULL && status != NULL);

    // Edge-Detection: nur bei Tastendruck-Kante toggeln
    static int prevf = GLFW_RELEASE;
    static int prev1 = GLFW_RELEASE;
    static int prev2 = GLFW_RELEASE;

    int k1 = glfwGetKey(window, GLFW_KEY_F);
    if (k1 == GLFW_PRESS && prevf == GLFW_RELEASE)
    {
        status->spot = !status->spot;
        glUniform1i(spot, status->spot);
    }
    prevf = k1;

    int k2 = glfwGetKey(window, GLFW_KEY_1);
    if (k2 == GLFW_PRESS && prev1 == GLFW_RELEASE)
    {
        status->sun = !status->sun;
        glUniform1i(sun, status->sun);
    }
    prev1 = k2;

    int k3 = glfwGetKey(window, GLFW_KEY_2);
    if (k3 == GLFW_PRESS && prev2 == GLFW_RELEASE)
    {
        status->lamp = !status->lamp;
        glUniform1i(lamp, status->lamp);
    }
    prev2 = k3;
}

void nebelSchalter(GLint fogEnabled, GLFWwindow *window, Status *status)
{
    assert(fogEnabled >= 0);
    assert(window != NULL && status != NULL);

    static int prevN = GLFW_RELEASE;
    int kn = glfwGetKey(window, GLFW_KEY_N);
    if (kn == GLFW_PRESS && prevN == GLFW_RELEASE)
    {
        status->nebel = !status->nebel;
        glUniform1i(fogEnabled, status->nebel);
    }
    prevN = kn;
}

void vollbildschalter(GLFWwindow *window, Status *status)
{
    static int prevV = GLFW_RELEASE;
    int kn = glfwGetKey(window, GLFW_KEY_V);
    if (kn == GLFW_PRESS && prevV == GLFW_RELEASE)
    {
        static int winX, winY, winW, winH;
        if (!status->vollbild)
        {
            glfwGetWindowPos(window, &winX, &winY);
            glfwGetWindowSize(window, &winW, &winH);

            GLFWmonitor *monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(window, monitor, 0, 0,
                                 mode->width, mode->height, mode->refreshRate);

            status->vollbild = !status->vollbild;
        }
        else
        {
            glfwSetWindowMonitor(window, NULL,
                                 winX, winY, winW, winH, 0);
            status->vollbild = !status->vollbild;
        }
    }
    prevV = kn;
}

void reflektionSchalter(GLFWwindow* window, int* enable) {
    static int prevKey = GLFW_RELEASE;
    int k7 = glfwGetKey(window, GLFW_KEY_7);
    if (k7 == GLFW_PRESS && prevKey == GLFW_RELEASE)
    {
        *enable = (*enable == 0) ? 1 : 0;
    }
    prevKey = k7;
}
