#include "matrixTester.h"
#include "matrixUtils.h"

#include <GL/glew.h>

#include "stdio.h"

void testCompare(const float *a, const float *b, int (*compare)(const float* , const float*)) {
    if(compare(a, b) == 1) {
        printf("\033[1;32mTest erfolgreich\033[0m\n");
    } else {
        printf("\033[1;31mTEST FEHLGESCHLAGEN\033[0m\n");
    }
}

void FunktionTester() {

}


void testFunktion(void){

    printf("-----\n");
    printf("\n");
    printf("Tests:\n");
    printf("\n");
    printf("-----\n");

    float eye[3] = {0.f, 0.f, 1.f};
    float look[3] = {0.f, 0.f, -1.f};
    float up[3] = {0.f, 1.f, 0.f};
    float view[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0,-1, 1
    };
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

    printf("-----\n");

    float fov = 45.f;
    float near = 0.1f;
    float far = 100.f;
    float aspect = 16.f/9.f;
    float perspec[16];
    float pMat[16] = {
        13.579950f, 0.000000f, 0.000000f, 0.000000f,
        0.000000f, 24.142136f, 0.000000f, 0.000000f,
        0.000000f, 0.000000f,-10.020020f,-10.000000f,
        0.000000f, 0.000000f,-2.002002f,  0.000000f
    };
    
    printf("FoV: %f | Aspect: %f | Near: %f | Far: %f \n", fov, aspect, near, far);
    printf("erwartetes Ergebnis: \n");
    printMat4(pMat);

    perspective(perspec, fov, aspect, near, far);
    printf("Perspective:\n");
    printMat4(perspec);

    testCompare(perspec, pMat, compareMat4);

    printf("-----\n");

    printf("\n");
    printf(
    "———————————————No bitches?—————————————\n"
    "    ⠀⣞⢽⢪⢣⢣⢣⢫⡺⡵⣝⡮⣗⢷⢽⢽⢽⣮⡷⡽⣜⣜⢮⢺⣜⢷⢽⢝⡽⣝\n"
    "    ⠸⡸⠜⠕⠕⠁⢁⢇⢏⢽⢺⣪⡳⡝⣎⣏⢯⢞⡿⣟⣷⣳⢯⡷⣽⢽⢯⣳⣫⠇\n"
    "    ⠀⠀⢀⢀⢄⢬⢪⡪⡎⣆⡈⠚⠜⠕⠇⠗⠝⢕⢯⢫⣞⣯⣿⣻⡽⣏⢗⣗⠏⠀\n"
    "    ⠀⠪⡪⡪⣪⢪⢺⢸⢢⢓⢆⢤⢀⠀⠀⠀⠀⠈⢊⢞⡾⣿⡯⣏⢮⠷⠁⠀⠀\n"
    "    ⠀⠀⠀⠈⠊⠆⡃⠕⢕⢇⢇⢇⢇⢇⢏⢎⢎⢆⢄⠀⢑⣽⣿⢝⠲⠉⠀⠀⠀⠀\n"
    "    ⠀⠀⠀⠀⠀⡿⠂⠠⠀⡇⢇⠕⢈⣀⠀⠁⠡⠣⡣⡫⣂⣿⠯⢪⠰⠂⠀⠀⠀⠀\n"
    "    ⠀⠀⠀⠀⡦⡙⡂⢀⢤⢣⠣⡈⣾⡃⠠⠄⠀⡄⢱⣌⣶⢏⢊⠂⠀⠀⠀⠀⠀⠀\n"
    "    ⠀⠀⠀⠀⢝⡲⣜⡮⡏⢎⢌⢂⠙⠢⠐⢀⢘⢵⣽⣿⡿⠁⠁⠀⠀⠀⠀⠀⠀⠀\n"
    "    ⠀⠀⠀⠀⠨⣺⡺⡕⡕⡱⡑⡆⡕⡅⡕⡜⡼⢽⡻⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
    "    ⠀⠀⠀⠀⣼⣳⣫⣾⣵⣗⡵⡱⡡⢣⢑⢕⢜⢕⡝⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
    "    ⠀⠀⠀⣴⣿⣾⣿⣿⣿⡿⡽⡑⢌⠪⡢⡣⣣⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
    "    ⠀⠀⠀⡟⡾⣿⢿⢿⢵⣽⣾⣼⣘⢸⢸⣞⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
    "    ⠀⠀⠀⠀⠁⠇⠡⠩⡫⢿⣝⡻⡮⣒⢽⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n"
    "——————————————————————————————————————\n"
    );
    printf("\n");
    printf("-----\n");
    printf("\n");
}
