// Self
#include "ShaderLoader.h"

// Standard libs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// OpenGL
#include <GL/glew.h>

char *readShaderCode(const char *file_path)
{
    assert(file_path != NULL);

    FILE *file = fopen(file_path, "rb");
    if (!file)
    {
        printf("Impossible to open %s. Are you in the right directory?\n", file_path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    rewind(file);

    char *code = (char *)malloc(length + 1);
    if (!code)
    {
        printf("Failed to allocate memory for shader code\n");
        fclose(file);
        return NULL;
    }

    fread(code, 1, length, file);
    code[length] = '\0';
    fclose(file);
    return code;
}

GLuint loadShaders(const char *vertex_file_path, const char *fragment_file_path)
{
    assert(vertex_file_path != NULL && fragment_file_path != NULL);

    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    char *VertexShaderCode = readShaderCode(vertex_file_path);
    if (VertexShaderCode == NULL)
        return 0;

    char *FragmentShaderCode = readShaderCode(fragment_file_path);
    if (FragmentShaderCode == NULL)
    {
        free(VertexShaderCode);
        return 0;
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    printf("Compiling shader: %s\n", vertex_file_path);
    const char *VertexSourcePointer = VertexShaderCode;
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        char *VertexShaderErrorMessage = (char *)malloc(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, VertexShaderErrorMessage);
        VertexShaderErrorMessage[InfoLogLength] = '\0';
        printf("%s\n", VertexShaderErrorMessage);
        free(VertexShaderErrorMessage);
    }

    printf("Compiling shader: %s\n", fragment_file_path);
    const char *FragmentSourcePointer = FragmentShaderCode;
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        char *FragmentShaderErrorMessage = (char *)malloc(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, FragmentShaderErrorMessage);
        FragmentShaderErrorMessage[InfoLogLength] = '\0';
        printf("%s\n", FragmentShaderErrorMessage);
        free(FragmentShaderErrorMessage);
    }

    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0)
    {
        char *ProgramErrorMessage = (char *)malloc(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, ProgramErrorMessage);
        ProgramErrorMessage[InfoLogLength] = '\0';
        printf("%s\n", ProgramErrorMessage);
        free(ProgramErrorMessage);
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    free(VertexShaderCode);
    free(FragmentShaderCode);

    return ProgramID;
}
