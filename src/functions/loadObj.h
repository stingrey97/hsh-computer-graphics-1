# ifndef LOADOBJ_H
# define LOADOBJ_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
extern void countLinesF(const char *filename, int *werte);
extern void loadOBJ(const char *filename, GLfloat *out, int *werte);
# endif