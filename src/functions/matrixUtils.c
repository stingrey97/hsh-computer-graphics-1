#include "matrixUtils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <GL/glew.h>

float deg2radf(float deg) { return (float)PI * deg / 180.0f; }

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

void setVec3(GLfloat v[3], const GLfloat x, const GLfloat y, const GLfloat z) {
    v[0] = x; v[1] = y; v[2] = z;
}

void copyVec3(GLfloat out[3], const GLfloat vec[3]) {
    memcpy(out, vec, 3 * sizeof(GLfloat));
}

void dot3f(GLfloat *out, const GLfloat *a, const GLfloat *b)
{
    *out = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void multiply3f(GLfloat *out, const GLfloat *vec, const GLfloat a){
    out[0] = vec[0] * a; 
    out[1] = vec[1] * a;
    out[2] = vec[2] * a;
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

int compareVecN(const GLfloat *vecA, const GLfloat *vecB, const int n) {
    for (int i = 0; i < n; i++)
    {
        if(vecA[i] != vecB[i]) return 0;
    }
    return 1;
}

int compareVec3(const GLfloat *vecA, const GLfloat *vecB) { return compareVecN(vecA, vecB, 3); }

int compareVec4(const GLfloat *vecA, const GLfloat *vecB) { return compareVecN(vecA, vecB, 4); }

int compareMat4(const GLfloat *matA, const GLfloat *matB) {
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if(matA[j * 4 + i] != matB[j * 4 + i]) return 0;
        }
        
    }
    return 1;
}

void mat3_from_mat4(GLfloat out[9], const GLfloat M[16]) {
    out[0]=M[0];  out[1]=M[1];  out[2]=M[2];  
    out[3]=M[4];  out[4]=M[5];  out[5]=M[6];  
    out[6]=M[8];  out[7]=M[9];  out[8]=M[10];  
}

void mat3_inverse_transpose(GLfloat out[9], const GLfloat m[9]) {
    
    float a=m[0], d=m[1], g=m[2]; 
    float b=m[3], e=m[4], h=m[5]; 
    float c=m[6], f=m[7], i=m[8]; 

    float A =   e*i - f*h;
    float B = -(d*i - f*g);
    float C =   d*h - e*g;
    float D = -(b*i - c*h);
    float E =   a*i - c*g;
    float F = -(a*h - b*g);
    float G =   b*f - c*e;
    float H = -(a*f - c*d);
    float I =   a*e - b*d;

    float det = a*A + b*B + c*C;

    // Falls die inverse nicht möglich ist
    if (fabsf(det) < 1e-12f) { 
        det = 1.0f;
    }
    float invDet = 1.0f / det;

    
    out[0] = A*invDet; out[1] = D*invDet; out[2] = G*invDet; 
    out[3] = B*invDet; out[4] = E*invDet; out[5] = H*invDet; 
    out[6] = C*invDet; out[7] = F*invDet; out[8] = I*invDet; 
}

void transform_point_view(GLfloat out3[3], const GLfloat V[16], const GLfloat pW[3]) {
    float x = pW[0], y = pW[1], z = pW[2];
    out3[0] = V[0]*x + V[4]*y + V[8]*z  + V[12];
    out3[1] = V[1]*x + V[5]*y + V[9]*z  + V[13];
    out3[2] = V[2]*x + V[6]*y + V[10]*z + V[14];
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
