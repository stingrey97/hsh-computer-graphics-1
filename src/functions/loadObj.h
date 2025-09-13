#include <GL/glew.h>

# ifndef LOADOBJ_H
# define LOADOBJ_H



extern void countLinesF(const char *filename, int *werte);
extern void loadOBJ(const char *filename, GLfloat *out, int *werte);

#endif
