#ifndef ENTITY_H

#define ENTITY_H

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#include "../../include/common/snippets/UserMemAlloc.h"

//======================================================================================
HeI32 DrawCircle(HeI32 locx, HeI32 locy, HeI32 radius, HeI32 color);
HeI32 PrintText(HeI32 x, HeI32 y, HeI32 color, char* output, ...);
HeI32 DrawLine(HeI32 x1, HeI32 y1, HeI32 x2, HeI32 y2, HeI32 color);
void DrawPoint(HeI32 locx,HeI32 locy,HeI32 color);

void createEntityFactory(HeI32 count);

void processEntityFactory(HeF32 dtime);

void renderEntityFactory(void);
void releaseEntityFactory(void);


#endif
