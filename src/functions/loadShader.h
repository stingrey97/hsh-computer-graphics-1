# ifndef LOADSHADER_H
# define LOADSHADER_H
#include <stdio.h>   
#include <stdlib.h>
extern char * loadShader(const char *filename);
extern void freeShader(const char *shader);
# endif