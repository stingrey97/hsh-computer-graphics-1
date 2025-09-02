#include "matrixUtils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <GL/glew.h>

static inline float deg2radf(float deg) { return (float)M_PI * deg / 180.0f; }

void mat4f_mul_mat4f(float *restrict out, const float *a, const float *b)
{
    GLfloat tmp[16];

    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            GLfloat sum = 0.0f;
            for (int k = 0; k < 4; ++k)
            {
                sum += a[k * 4 + row] * b[col*4 + k];
            }
            tmp[col*4 + row] = sum;
        }
    }

    for (int i = 0; i < 16; ++i)
        out[i] = tmp[i];
}

void mat4f_mul_vec4f(GLfloat *out, const GLfloat *mat, const GLfloat *vec)
{
    for (int i = 0; i < 4; i++)
    {
        GLfloat sum = 0;
        for (int j = 0; j < 4; j++)
        {
            sum += mat[j * 4 + i] * vec[j];
        }
        out[i] = sum;
    }
}

void dot3f(GLfloat *out, const GLfloat *a, const GLfloat *b)
{
    *out = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void plus3f(GLfloat *out, const GLfloat *v1, const GLfloat *v2)
{
    out[0] = v1[0] + v2[0];
    out[1] = v1[1] + v2[1];
    out[2] = v1[2] + v2[2];
}

void minus3f(GLfloat *out, const GLfloat *v1, const GLfloat *v2)
{
    out[0] = v1[0] - v2[0];
    out[1] = v1[1] - v2[1];
    out[2] = v1[2] - v2[2];
}

void norm3f(GLfloat *out, const GLfloat *vec)
{
    GLfloat len = len3f(vec);

    if (len > EPS)
    {
        out[0] = vec[0] / len;
        out[1] = vec[1] / len;
        out[2] = vec[2] / len;
    }
    else
    {
        out[0] = out[1] = out[2] = 0.0f;
    }
}

GLfloat len3f(const GLfloat *vec)
{
    GLfloat len = 0;
    len += vec[0] * vec[0];
    len += vec[1] * vec[1];
    len += vec[2] * vec[2];

    return sqrtf(len);
}

void cross3f(GLfloat *out, const GLfloat *v1, const GLfloat *v2)
{
    GLfloat x = v1[1] * v2[2] - v1[2] * v2[1];
    GLfloat y = v1[2] * v2[0] - v1[0] * v2[2];
    GLfloat z = v1[0] * v2[1] - v1[1] * v2[0];

    out[0]=x; out[1]=y; out[2]=z;
}

void printVecN(const GLfloat *vec, const int n)
{
    for (int i = 0; i < n; i++)
        printf("%f ", vec[i]);
    printf("\n");
}

void printVec3(const GLfloat *vec) { printVecN(vec, 3); }

void printVec4(const GLfloat *vec) { printVecN(vec, 4); }

void printMat4(const GLfloat *mat)
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%f ", mat[j * 4 + i]);
        }
        printf("\n");
    }
}

void identity(GLfloat *out)
{
    static const GLfloat I[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};
    memcpy(out, I, sizeof(I));
}

void translate(GLfloat *out, const GLfloat *in, const GLfloat *v)
{
    GLfloat T[16];
    identity(T);
    T[12] = v[0];
    T[13] = v[1];
    T[14] = v[2];
    mat4f_mul_mat4f(out, in, T);
}

void scale(GLfloat *out, const GLfloat *in, const GLfloat *v)
{
    GLfloat S[16];
    identity(S);
    S[0] = v[0];
    S[5] = v[1];
    S[10] = v[2];
    mat4f_mul_mat4f(out, in, S);
}

void rotateX(GLfloat *out, const GLfloat *in, float angle_deg)
{
    const GLfloat a = deg2radf(angle_deg);
    const GLfloat c = cosf(a), s = sinf(a);

    GLfloat R[16];
    identity(R);
    R[5] = c;
    R[6] = s;
    R[9] = -s;
    R[10] = c;

    mat4f_mul_mat4f(out, in, R);
}

void rotateY(GLfloat *out, const GLfloat *in, float angle_deg)
{
    const GLfloat a = deg2radf(angle_deg);
    const GLfloat c = cosf(a), s = sinf(a);

    GLfloat R[16];
    identity(R);
    R[0] = c;
    R[2] = -s;
    R[8] = s;
    R[10] = c;

    mat4f_mul_mat4f(out, in, R);
}

void rotateZ(GLfloat *out, const GLfloat *in, float angle_deg)
{
    const GLfloat a = deg2radf(angle_deg);
    const GLfloat c = cosf(a), s = sinf(a);

    GLfloat R[16];
    identity(R);
    R[0] = c;
    R[1] = s;
    R[4] = -s;
    R[5] = c;

    mat4f_mul_mat4f(out, in, R);
}

void lookAt(GLfloat *out, const GLfloat *eye, const GLfloat *look, const GLfloat *up)
{
    GLfloat n[3], u[3], v[3];

    minus3f(n, eye, look);
    if (len3f(n) < EPS)
    {
        printf("ERROR in lookAt(): ||eye-look|| < EPS\n");
        return;
    }
    
    cross3f(u, up, n);
    if (len3f(u) < EPS)
    {
        printf("ERROR in lookAt(): ||up x n|| < EPS\n");
        return;
    }
    

    cross3f(v, n, u);

    norm3f(n, n);
    norm3f(u, u);
    norm3f(v, v);

    GLfloat tx, ty, tz;
    dot3f(&tx, u, eye);
    tx = -tx;
    dot3f(&ty, v, eye);
    ty = -ty;
    dot3f(&tz, n, eye);
    tz = -tz;

    out[0]  = u[0];  out[4]  = u[1];  out[8]  = u[2];  out[12] = tx;
    out[1]  = v[0];  out[5]  = v[1];  out[9]  = v[2];  out[13] = ty;
    out[2]  = n[0];  out[6]  = n[1];  out[10] = n[2];  out[14] = tz;
    out[3]  = 0.0f;  out[7]  = 0.0f;  out[11] = 0.0f;  out[15] = 1.0f;
}

void perspective(GLfloat *out, GLfloat const fovy, GLfloat const aspect, GLfloat const near, GLfloat const far)
{
    const GLfloat fovy_rad = deg2radf(fovy), t = near * tanf(fovy_rad / 2.0f), b = -t, r = t * aspect, l = -r;

    GLfloat M[16] = {0.0f};   

    M[0]  = 2.0f / (r - l);
    M[5]  = 2.0f / (t - b);
    M[8]  = (1.0f / near) * ((r + l) / (r - l)); 
    M[9]  = (1.0f / near) * ((t + b) / (t - b));
    M[10] = -((1.0f / near) * ((far + near) / (far - near)));
    M[11] = -(1.0f / near);
    M[14] = -((2.0f * far) / (far - near));
    M[15] = 0.0f;

    memcpy(out, M, sizeof(M));
}
