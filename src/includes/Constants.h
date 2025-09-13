#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <GL/glew.h>

/*
 * Window
 */
#define INIT_WINDOW_TITLE "OpenGL Program"
#define INIT_WINDOW_WIDTH 1024
#define INIT_WINDOW_HEIGHT 768

/*
 * Camera
 */
#define MAX_PITCH 1.55334306f
#define FOV 45.0f
#define KEY_SPEED 3.0f
#define MOUSE_SPEED 0.05f
#define START_HORIZONTAL_ANGLE 4.72f
#define START_VERTICAL_ANGLE 0.0f
#define FIXED_CAMERA
#define CAMERA_HEIGHT 2.7f
#define NO_DEBUG_MODE

/*
 * Skybox
 */
#define SKYBOX_NIGHT // Skybox texture to be load. Possible values: SKYBOX_DAY, SKYBOX_NIGHT, SKYBOX_MYSTIC

/*
 * Math
 */
#define EPS 1e-6f
#define PI 3.14159265358979323846

#endif
