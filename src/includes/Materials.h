#ifndef MATERIALS_H
#define MATERIALS_H

#include <GL/glew.h>

#include "AppContext.h"

void setMaterialPolishedGold(AppContext *ctx);
void setMaterialGrayPillar(AppContext *ctx);
void setMaterialGlass(AppContext *ctx, GLfloat alpha);
void setMaterialGrass(AppContext *ctx);
void setMaterialWood(AppContext *ctx);
void setMaterialTree(AppContext *ctx);
void setMaterialSlenderman(AppContext *ctx);

#endif
