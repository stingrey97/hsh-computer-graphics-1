// Self
#include "MatrixTester.h"

// Standard libs
#include <stdio.h>
#include <math.h>

// OpenGL
#include <GL/glew.h>

// Own libs
#include "MathUtils.h"

static int erfolgreichCounter;
static int fehlgeschlagenCounter;

void testCompare(const float *a, const float *b, int (*compare)(const float *, const float *))
{
    if (compare(a, b) == 1)
    {
        printf("\n");
        printf("\033[1;32mTest erfolgreich\033[0m\n");
        erfolgreichCounter++;
    }
    else
    {
        printf("\n");
        printf("\033[1;31mTEST FEHLGESCHLAGEN!\033[0m\n");
        fehlgeschlagenCounter++;
    }
}

void compareFloats(float a, float b)
{
    if (a == b)
    {
        printf("\n");
        printf("\033[1;32mTest erfolgreich\033[0m\n");
        erfolgreichCounter++;
    }
    else
    {
        printf("\n");
        printf("\033[1;31mTEST FEHLGESCHLAGEN!\033[0m\n");
        fehlgeschlagenCounter++;
    }
}

void printLine()
{
    printf("\n");
    printf("——————————————————————————————————————\n");
    printf("\n");
}

void printErgebnis(int erfolgreich, int fehlgeschlagen)
{
    printf("\033[1;31mFehlgeschlagen: %d\033[0m  "
           "\033[1;32mErfolgreich: %d\033[0m\n",
           fehlgeschlagen, erfolgreich);
    printf("\n");
    if (fehlgeschlagenCounter == 0)
    {
        printf("\n");
        printf("\033[1;32mAlle Tests erfolgreich!\033[0m\n");
    }
}

void testeFunktionen(void)
{

    printLine();
    printf("\033[1;34mTests:\033[0m\n");
    printLine();

    float eye[3] = {0.f, 0.f, 1.f};
    float look[3] = {0.f, 0.f, -1.f};
    float up[3] = {0.f, 1.f, 0.f};
    float view[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, -1, 1};
    float lookat[16];

    printf("Eye:  ");
    printVec3(eye);
    printf("Look: ");
    printVec3(look);
    printf("Up:   ");
    printVec3(up);

    printf("erwartetes Ergebnis: \n");
    printMat4(view);

    lookAt(lookat, eye, look, up);
    printf("LookAt Ergebnis: \n");
    printMat4(lookat);

    testCompare(lookat, view, compareMat4);

    printLine();

    float fov = 45.f;
    float near = 0.1f;
    float far = 100.f;
    float aspect = 16.f / 9.f;
    float perspec[16];
    float pMat[16] = {
        1.357995033, 0, 0, 0, 0, 2.4142, 0, 0, 0, 0, -1.0020, -1, 0, 0, -0.2002, 0};

    printf("FoV: %.3f | Aspect: %.3f | Near: %.3f | Far: %.3f \n", fov, aspect, near, far);
    printf("erwartetes Ergebnis: \n");
    printMat4(pMat);

    perspective(perspec, fov, aspect, near, far);
    printf("Perspective:\n");
    printMat4(perspec);

    testCompare(perspec, pMat, compareMat4);

    printLine();

    GLfloat V[16];
    identity(V);

    V[12] = 1.f;
    V[13] = 2.f;
    V[14] = 3.f;

    GLfloat pW[3] = {4.f, 5.f, 6.f};
    GLfloat out3[3];
    GLfloat expected3[3] = {5.f, 7.f, 9.f};

    printf("Matrix V:\n");
    printMat4(V);
    printf("Punkt pW (Welt): ");
    printVec3(pW);

    transform_point_view(out3, V, pW);
    printf("transform_point_view(V, pW):\n");
    printVec3(out3);
    printf("erwarteter transformierter Punkt:\n");
    printVec3(expected3);
    testCompare(out3, expected3, compareVec3);

    printLine();

    float vectorA[3] = {1.f, 2.f, 3.f};
    float vectorB[3] = {0.f, 1.f, 0.f};
    printf("Vektor A:\n");
    printVec3(vectorA);
    printf("vektor B:\n");
    printVec3(vectorB);

    printLine();

    float lenVecB = (float)len3f(vectorB);
    printf("Länge von Vektor B: %.3f\n", lenVecB);
    float laenge = 1.f;
    printf("erwartete Länge: %.3f\n", laenge);
    compareFloats(laenge, lenVecB);

    printLine();

    float number = 3.f;
    float outVecATimesNum[3];
    multiply3f(outVecATimesNum, vectorA, number);
    printf("Vektor A * 3: \n");
    printVec3(outVecATimesNum);
    float vecATimesNum[3] = {3.f, 6.f, 9.f};
    printf("erwartet Vektor A * 3: \n");
    printVec3(vecATimesNum);
    testCompare(outVecATimesNum, vecATimesNum, compareVec3);

    printLine();

    float expectedDot = 2.f;
    float outDot;
    dot3f(&outDot, vectorA, vectorB);
    printf("Skalarproduckt Vektor A x Vektor B: %.3f\n", outDot);
    printf("erwartetes Skalarproduckt Vektor A x Vektor B: %.3f\n", expectedDot);
    compareFloats(outDot, expectedDot);

    printLine();

    float outVecPlus[3];
    float expectedVecPlus[3] = {1.f, 3.f, 3.f};
    plus3f(outVecPlus, vectorA, vectorB);
    printf("Vektor A + Vektor B:\n");
    printVec3(outVecPlus);
    printf("erwartetes Vektor A + Vektor B:\n");
    printVec3(expectedVecPlus);
    testCompare(outVecPlus, expectedVecPlus, compareVec3);

    printLine();

    float outVecMinus[3];
    float expectedVecMinus[3] = {1.f, 1.f, 3.f};
    minus3f(outVecMinus, vectorA, vectorB);
    printf("Vektor A - Vektor B:\n");
    printVec3(outVecMinus);
    printf("erwartetes Vektor A - Vektor B:\n");
    printVec3(expectedVecMinus);
    testCompare(outVecMinus, expectedVecMinus, compareVec3);

    printLine();

    float outVecNorm[3];
    float expectedVecNorm[3] = {1.f / (float)sqrt(14), 2.f / (float)sqrt(14), 3.f / (float)sqrt(14)};
    norm3f(outVecNorm, vectorA);
    printf("Normierter Vektor A: \n");
    printVec3(outVecNorm);
    printf("erwarteter Normierter Vektor A:\n");
    printVec3(expectedVecNorm);
    testCompare(outVecNorm, expectedVecNorm, compareVec3);

    printLine();

    float outVecCross[3];
    float expectedVecCross[3] = {-3.f, 0.f, 1.f};
    cross3f(outVecCross, vectorA, vectorB);
    printf("Vektor A x Vektor B: \n");
    printVec3(outVecCross);
    printf("erwartetes Vektor A x Vektor B: \n");
    printVec3(expectedVecCross);
    testCompare(outVecCross, expectedVecCross, compareVec3);

    printLine();

    float angle = 90.f;
    float testMatA[16];
    identity(testMatA);
    printf("Matrix zum testen: \n");
    printMat4(testMatA);
    printf("\n");

    float expectedRotateX[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
    float outRotateX[16];
    rotateX(outRotateX, testMatA, angle);
    printf("Matrix nach X-Roatation um %.f Grad: \n", angle);
    printMat4(outRotateX);
    printf("Erwartete Matrix nach X-Roatation um %.f Grad: \n", angle);
    printMat4(expectedRotateX);
    testCompare(outRotateX, expectedRotateX, compareMat4);
    printf("\n");

    float expectedRotateY[16] = {
        0.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
    float outRotateY[16];
    rotateY(outRotateY, testMatA, angle);
    printf("Matrix nach Y-Roatation um %.f Grad: \n", angle);
    printMat4(outRotateY);
    printf("Erwartete Matrix nach Y-Roatation um %.f Grad: \n", angle);
    printMat4(expectedRotateY);
    testCompare(outRotateY, expectedRotateY, compareMat4);
    printf("\n");

    float expectedRotateZ[16] = {
        0.0f, 1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};
    float outRotateZ[16];
    rotateZ(outRotateZ, testMatA, angle);
    printf("Matrix nach Z-Roatation um %.f Grad: \n", angle);
    printMat4(outRotateZ);
    printf("Erwartete Matrix nach Z-Roatation um %.f Grad: \n", angle);
    printMat4(expectedRotateZ);
    testCompare(outRotateZ, expectedRotateZ, compareMat4);

    printLine();

    float B[16] = {
        2.f, 0.f, 0.f, 0.f,
        0.f, 3.f, 0.f, 0.f,
        0.f, 0.f, 4.f, 0.f,
        0.f, 0.f, 0.f, 1.f};

    float C[16] = {
        5.f, 0.f, 0.f, 0.f,
        0.f, 6.f, 0.f, 0.f,
        0.f, 0.f, 7.f, 0.f,
        0.f, 0.f, 0.f, 1.f};
    float outMulMat[16];
    float expectedMulMat[16] = {
        10.f, 0.f, 0.f, 0.f,
        0.f, 18.f, 0.f, 0.f,
        0.f, 0.f, 28.f, 0.f,
        0.f, 0.f, 0.f, 1.f};

    printf("Matrix B:\n");
    printMat4(B);
    printf("Matrix C:\n");
    printMat4(C);

    mat4f_mul_mat4f(outMulMat, B, C);
    printf("B * C (Mat4 x Mat4):\n");
    printMat4(outMulMat);
    printf("erwartetes B * C:\n");
    printMat4(expectedMulMat);
    testCompare(outMulMat, expectedMulMat, compareMat4);

    printLine();

    float M_Scale[16] = {
        2.f, 0.f, 0.f, 0.f,
        0.f, 3.f, 0.f, 0.f,
        0.f, 0.f, 4.f, 0.f,
        0.f, 0.f, 0.f, 1.f};
    float v4[4] = {1.f, 2.f, 3.f, 1.f};
    float outVec4[4];
    float expectedVec4[4] = {2.f, 6.f, 12.f, 1.f};

    printf("Matrix M:\n");
    printMat4(M_Scale);
    printf("Vektor v:\n");
    printVec4(v4);

    mat4f_mul_vec4f(outVec4, M_Scale, v4);
    printf("M * v:\n");
    printVec4(outVec4);
    printf("erwartetes M * v:\n");
    printVec4(expectedVec4);
    testCompare(outVec4, expectedVec4, compareVec4);

    printLine();

    float inIdScale[16];
    identity(inIdScale);
    float outScale[16];
    float s[3] = {2.f, 3.f, 4.f};
    float expectedScale[16] = {
        2.f, 0.f, 0.f, 0.f,
        0.f, 3.f, 0.f, 0.f,
        0.f, 0.f, 4.f, 0.f,
        0.f, 0.f, 0.f, 1.f};

    printf("Matrix Input (Identity):\n");
    printMat4(inIdScale);
    printf("Scale-Vektor: ");
    printVec3(s);

    scale(outScale, inIdScale, s);
    printf("Matrix nach Skalierung (%.f, %.f, %.f):\n", s[0], s[1], s[2]);
    printMat4(outScale);
    printf("erwartete Skalierung (%.f, %.f, %.f):\n", s[0], s[1], s[2]);
    printMat4(expectedScale);
    testCompare(outScale, expectedScale, compareMat4);

    printLine();

    float inIdTrans[16];
    identity(inIdTrans);
    float outTranslate[16];
    float t[3] = {1.f, 2.f, 3.f};
    float expectedTranslate[16] = {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        1.f, 2.f, 3.f, 1.f};

    printf("Matrix Input (Identity):\n");
    printMat4(inIdTrans);
    printf("Translation-Vektor: ");
    printVec3(t);

    translate(outTranslate, inIdTrans, t);
    printf("Matrix nach Translation (%.f, %.f, %.f):\n", t[0], t[1], t[2]);
    printMat4(outTranslate);
    printf("erwartete Translation (%.f, %.f, %.f):\n", t[0], t[1], t[2]);
    printMat4(expectedTranslate);
    testCompare(outTranslate, expectedTranslate, compareMat4);

    printLine();

    float M_T_S[16] = {
        2.f, 0.f, 0.f, 0.f,
        0.f, 3.f, 0.f, 0.f,
        0.f, 0.f, 4.f, 0.f,
        1.f, 2.f, 3.f, 1.f};

    float outMat3[9];
    float expectedMat3[9] = {
        2.f, 0.f, 0.f,
        0.f, 3.f, 0.f,
        0.f, 0.f, 4.f};

    printf("Matrix Input (Mat4):\n");
    printMat4(M_T_S);

    mat3_from_mat4(outMat3, M_T_S);
    printf("aus Mat4 extrahierte Mat3:\n");
    printMat3(outMat3);
    printf("erwartete Mat3:\n");
    printMat3(expectedMat3);
    testCompare(outMat3, expectedMat3, compareMat3);

    printLine();

    float outMat3InvT[9];
    float MAT3x3[16] = {
        2.f, 0.f, 0.f,
        0.f, 3.f, 0.f,
        0.f, 0.f, 4.f};

    float expectedMat3InvT[9] = {
        0.5f, 0.f, 0.f,
        0.f, 0.3333333f, 0.f,
        0.f, 0.f, 0.25f};

    printf("Matrix Input (Mat4):\n");
    printMat4(MAT3x3);

    mat3_inverse_transpose(outMat3InvT, MAT3x3);
    printf("Mat3 Inverse-Transpose aus Mat4:\n");
    printMat3(outMat3InvT);
    printf("erwartete Mat3 Inverse-Transpose:\n");
    printMat3(expectedMat3InvT);
    testCompare(outMat3InvT, expectedMat3InvT, compareMat3);

    printLine();
    printErgebnis(erfolgreichCounter, fehlgeschlagenCounter);
    printf("\n");
}
