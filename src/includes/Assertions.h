#ifndef ASSERTIONS_H
#define ASSERTIONS_H

#include <GL/glew.h>

// Returns 1 if the vector contains only finite values (no NaN, +Inf, -Inf).
int isValidVector3f(const GLfloat vec[3]);

// Returns 1 if the vector is not the zero vector (length > 0).
int isNotZeroVector(const GLfloat vec[3]);

// Returns 1 if all 16 entries of the 4x4 matrix are finite values.
int isValidMatrix16f(const GLfloat *mat);

// Returns 1 if two 3D vectors are different.
int vectorsAreDifferent(const GLfloat a[3], const GLfloat b[3]);

#endif
