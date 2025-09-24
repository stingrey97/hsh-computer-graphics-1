#ifndef MATERIALS_H
#define MATERIALS_H

#include <GL/glew.h>

#include "AppContext.h"

// Applies material properties for polished gold (ambient, diffuse, specular, shininess)
void setMaterialPolishedGold(AppContext *ctx);

// Applies material properties for a gray stone/concrete pillar
void setMaterialGrayPillar(AppContext *ctx);

// Applies material properties for transparent glass with a given alpha value
void setMaterialGlass(AppContext *ctx, GLfloat alpha);

// Applies material properties for grass
void setMaterialGrass(AppContext *ctx);

// Applies material properties for wood
void setMaterialWood(AppContext *ctx);

// Applies material properties for tree
void setMaterialTree(AppContext *ctx);

// Applies material properties for the Slenderman model
void setMaterialSlenderman(AppContext *ctx);

#endif
