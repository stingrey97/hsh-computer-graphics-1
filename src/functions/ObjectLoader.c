// Self
#include "ObjectLoader.h"

// Standard libs
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// OepnGL
#include <GL/glew.h>

typedef struct
{
    float x, y, z;
} Vec3;

typedef struct
{
    float u, v;
} Vec2;

typedef struct
{
    int v, vt, vn;
} FaceIndex;

// wie viele v/t/vn/f zeilen gibt es um entsprechende arrays zu allokieren
void countLinesF(const char *filename, int *werte)
{
    assert(filename != NULL && werte != NULL);

    int countV = 0;
    int countVT = 0;
    int countVN = 0;
    int countF = 0;
    char line[256];
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file: %s\n", filename);
        return;
    }
    while (fgets(line, sizeof(line), file))
    {
        if (line[0] == 'v')
        {
            if (line[1] == ' ')
                countV++;
            else if (line[1] == 'n')
                countVN++;
            else if (line[1] == 't')
                countVT++;
        }
        else if (line[0] == 'f')
        {
            countF++;
        }
    }
    werte[0] = countV;
    werte[1] = countVT;
    werte[2] = countVN;
    werte[3] = countF * 3;
    fclose(file);
}

// Datei erneut öffnen und entsprechende Vertices erstellen Werte = v/vt/vn/f  F wird direkt in out geschrieben
void loadOBJ(const char *filename, GLfloat *out, int *werte)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file: %s\n", filename);
        return;
    }
    Vec3 *vArray = (Vec3 *)malloc((size_t)werte[0] * sizeof(Vec3));
    Vec2 *vtArray = (Vec2 *)malloc((size_t)werte[1] * sizeof(Vec2));
    Vec3 *vnArray = (Vec3 *)malloc((size_t)werte[2] * sizeof(Vec3));

    char line[512];
    int iV = 0, iVT = 0, iVN = 0, iF = 0;
    while (fgets(line, sizeof(line), file))
    {
        if (line[0] == 'v')
        {
            if (line[1] == ' ')
            {
                sscanf(line, "v %f %f %f", &vArray[iV].x, &vArray[iV].y, &vArray[iV].z);
                iV++;
            }
            else if (line[1] == 'n')
            {
                sscanf(line, "vn %f %f %f", &vnArray[iVN].x, &vnArray[iVN].y, &vnArray[iVN].z);
                iVN++;
            }

            else if (line[1] == 't')
            {
                sscanf(line, "vt %f %f", &vtArray[iVT].u, &vtArray[iVT].v);
                iVT++;
            }
        }
        else if (line[0] == 'f')
        {
            FaceIndex merke[3];
            sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                   &merke[0].v, &merke[0].vt, &merke[0].vn,
                   &merke[1].v, &merke[1].vt, &merke[1].vn,
                   &merke[2].v, &merke[2].vt, &merke[2].vn);

            for (int i = 0; i < 3; i++)
            {
                // Vertices bzw Postion
                out[iF] = vArray[merke[i].v - 1].x;
                out[iF + 1] = vArray[merke[i].v - 1].y;
                out[iF + 2] = vArray[merke[i].v - 1].z;
                // Texture Koordinaten
                out[iF + 3] = vtArray[merke[i].vt - 1].u;
                out[iF + 4] = vtArray[merke[i].vt - 1].v;
                // Normale
                out[iF + 5] = vnArray[merke[i].vn - 1].x;
                out[iF + 6] = vnArray[merke[i].vn - 1].y;
                out[iF + 7] = vnArray[merke[i].vn - 1].z;
                iF += 8;
            }
        }
    }
    fclose(file);
    free(vArray);
    free(vtArray);
    free(vnArray);
}
