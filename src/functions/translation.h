# ifndef TRANSLATION_H
# define TRANSLATION_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
extern void identity(GLfloat* out);
extern void translate(GLfloat *out,GLfloat* in, GLfloat* v);
extern void scale(GLfloat *out,GLfloat* in, GLfloat* v);    
extern void rotateZ(GLfloat *out,GLfloat* in, float angle);
extern void rotateX(GLfloat *out,GLfloat* in, float angle);
extern void rotateY(GLfloat *out,GLfloat* in, float angle);
extern void lookAt(GLfloat *out, GLfloat *eye, GLfloat *center, GLfloat *up);
extern void perspective(GLfloat *out, float fovy, float aspect, float near, float far);
# endif