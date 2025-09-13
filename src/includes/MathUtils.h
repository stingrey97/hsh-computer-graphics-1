#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H

#include <GL/glew.h>

/**
 * Multipliziert zwei 4x4 Matrizen
 */
extern void mat4f_mul_mat4f(GLfloat *out, const GLfloat *left, const GLfloat *right);

/**
 * Multipliziert eine 4x4 Matrix mit einem 4-elementigen Vektor
 */
extern void mat4f_mul_vec4f(GLfloat *out, const GLfloat *mat, const GLfloat *vec);

/**
 * Setzt einen 3-elementigen Vektor auf die entsprechenden Werte
 */
extern void setVec3(GLfloat v[3], GLfloat x, GLfloat y, GLfloat z);

/**
 * Kopiert einen Vector von vec nach out, weil das in C sonst nicht geht :-)
 */
extern void copyVec3(GLfloat out[3], const GLfloat vec[3]);

/**
 * Skalare Multiplikation eines 3-elemetnigen Vektors
 */
extern void multiply3f(GLfloat *out, const GLfloat *vec, const GLfloat a);

/**
 * Multipliziert zwei 3-elementige Vektoren miteinander
 */
extern void dot3f(GLfloat *out, const GLfloat *a, const GLfloat *b);

/**
 * Addiert zwei 3-elementige Vektoren
 */
extern void plus3f(GLfloat *out, const GLfloat *v1, const GLfloat *v2);

/**
 * Subtrahiert zwei 3-elementige Vektoren
 */
extern void minus3f(GLfloat *out, const GLfloat *v1, const GLfloat *v2);

/**
 * Normalisiert einen 3-elemetigen Vektor
 */
extern void norm3f(GLfloat *out, const GLfloat *vec);

/**
 * Berechnet die Länge eines 3-elemetigen Vektors
 */
extern GLfloat len3f(const GLfloat *vec);

/**
 * Berechnet das Kreuzprodukt von zwei 3-elementigen Vektoren
 */
extern void cross3f(GLfloat *out, const GLfloat *v1, const GLfloat *v2);

/**
 * Gibt einen Vektor der Länge n auf der Konsole aus
 */
extern void printVecN(const GLfloat *vec, const int n);

/**
 * Gibt einen Vektor der Länge 3 auf der Konsole aus
 */
extern void printVec3(const GLfloat *vec);

/**
 * Gibt einen Vektor der Länge 4 auf der Konsole aus
 */
extern void printVec4(const GLfloat *vec);

/**
 * Gibt eine 4x4 Matrix auf der Konsole aus
 */
extern void printMat4(const GLfloat *mat);

/**
 * Erstellt eine 3x3 Matrix aus einer 4x4 Matrix (oberer linker Block)
 */
extern void mat3_from_mat4(GLfloat out[9], const GLfloat M[16]);

/**
 * Berechnet die Inverse Transponierte einer 3x3 Matrix
 */
extern void mat3_inverse_transpose(GLfloat out[9], const GLfloat m[9]);

/**
 * Transformiert einen Punkt pW (World Space) in den View Space
 */
extern void transform_point_view(GLfloat out3[3], const GLfloat V[16], const GLfloat pW[3]);

/**
 * 4x4 Einheitsmatrix
 */
extern void identity(GLfloat *out);

/**
 * 4x4 Translationsmatrix
 */
extern void translate(GLfloat *out, const GLfloat *in, const GLfloat *v);

/**
 * 4x4 Skalierungsmatrix
 */
extern void scale(GLfloat *out, const GLfloat *in, const GLfloat *v);

/**
 * 4x4 Rotationsmatrix um X achse
 */
extern void rotateX(GLfloat *out, const GLfloat *in, const GLfloat angle);

/**
 * 4x4 Rotationsmatrix um Y achse
 */
extern void rotateY(GLfloat *out, const GLfloat *in, const GLfloat angle);

/**
 * 4x4 Rotationsmatrix um Z achse
 */
extern void rotateZ(GLfloat *out, const GLfloat *in, const GLfloat angle);

/**
 * Berechnet die Viewmatrix für die vorgegebenen Kamera-Koordinaten.
 */
extern void lookAt(GLfloat *out, const GLfloat *eye, const GLfloat *look, const GLfloat *up);

/**
 * TODO
 */
extern void perspective(GLfloat *out, GLfloat const fovy, GLfloat const aspect, GLfloat const near, GLfloat const far);

#endif
