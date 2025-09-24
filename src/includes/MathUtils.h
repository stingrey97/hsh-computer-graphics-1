#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <GL/glew.h>

// Multiplies two 4x4 matrices
extern void mat4f_mul_mat4f(GLfloat *out, const GLfloat *left, const GLfloat *right);

// Multiplies a 4x4 matrix with a 4-element vector
extern void mat4f_mul_vec4f(GLfloat *out, const GLfloat *mat, const GLfloat *vec);

// Sets a 3-element vector with the given values
extern void setVec3(GLfloat v[3], GLfloat x, GLfloat y, GLfloat z);

// Copies a 3-element vector from vec to out
extern void copyVec3(GLfloat out[3], const GLfloat vec[3]);

// Multiplies a 3-element vector by a scalar
extern void multiply3f(GLfloat *out, const GLfloat *vec, const GLfloat a);

// Computes the dot product of two 3-element vectors
extern void dot3f(GLfloat *out, const GLfloat *a, const GLfloat *b);

// Adds two 3-element vectors
extern void plus3f(GLfloat *out, const GLfloat *v1, const GLfloat *v2);

// Subtracts two 3-element vectors
extern void minus3f(GLfloat *out, const GLfloat *v1, const GLfloat *v2);

// Normalizes a 3-element vector
extern void norm3f(GLfloat *out, const GLfloat *vec);

// Computes the length of a 3-element vector
extern GLfloat len3f(const GLfloat *vec);

// Computes the cross product of two 3-element vectors
extern void cross3f(GLfloat *out, const GLfloat *v1, const GLfloat *v2);

// Prints a vector of length n to the console
extern void printVecN(const GLfloat *vec, const int n);

// Prints a 3-element vector to the console
extern void printVec3(const GLfloat *vec);

// Prints a 4-element vector to the console
extern void printVec4(const GLfloat *vec);

// Prints a 4x4 matrix to the console
extern void printMat4(const GLfloat *mat);

// Prints a 3x3 matrix to the console
extern void printMat3(const GLfloat *mat);

// Compares two 3-element vectors, returns 1 if identical
extern int compareVec3(const GLfloat *vecA, const GLfloat *vecB);

// Compares two 4-element vectors, returns 1 if identical
extern int compareVec4(const GLfloat *vecA, const GLfloat *vecB);

// Compares two 4x4 matrices, returns 1 if identical
extern int compareMat4(const GLfloat *matA, const GLfloat *matB);

// Compares two 3x3 matrices, returns 1 if identical
extern int compareMat3(const GLfloat *matA, const GLfloat *matB);

// Extracts a 3x3 matrix from the upper-left block of a 4x4 matrix
extern void mat3_from_mat4(GLfloat out[9], const GLfloat M[16]);

// Computes the inverse transpose of a 3x3 matrix
extern void mat3_inverse_transpose(GLfloat out[9], const GLfloat m[9]);

// Transforms a point from world space to view space
extern void transform_point_view(GLfloat out3[3], const GLfloat V[16], const GLfloat pW[3]);

// Sets a 4x4 identity matrix
extern void identity(GLfloat *out);

// Creates a 4x4 translation matrix
extern void translate(GLfloat *out, const GLfloat *in, const GLfloat *v);

// Creates a 4x4 scaling matrix
extern void scale(GLfloat *out, const GLfloat *in, const GLfloat *v);

// Creates a 4x4 rotation matrix around the X axis
extern void rotateX(GLfloat *out, const GLfloat *in, const GLfloat angle);

// Creates a 4x4 rotation matrix around the Y axis
extern void rotateY(GLfloat *out, const GLfloat *in, const GLfloat angle);

// Creates a 4x4 rotation matrix around the Z axis
extern void rotateZ(GLfloat *out, const GLfloat *in, const GLfloat angle);

// Computes a view matrix from eye, look, and up vectors
extern void lookAt(GLfloat *out, const GLfloat *eye, const GLfloat *look, const GLfloat *up);

// Computes a perspective projection matrix (TODO: refine implementation)
extern void perspective(GLfloat *out, GLfloat const fovy, GLfloat const aspect, GLfloat const near, GLfloat const far);

#endif
