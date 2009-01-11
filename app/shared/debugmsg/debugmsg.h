#ifndef DEBUG_MSG_H

#define DEBUG_MSG_H

#include "../../include/common/snippets/UserMemAlloc.h"

#ifdef DEBUG_SERVER

bool openDebug(void);
bool closeDebug(void);
bool processDebug(void);

#endif

void debugTri(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true);
void debugSolidTri(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f);
void debugPoint(const HeF32 *pos,HeF32 radius,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true);
void debugLine(const HeF32 *p1,const HeF32 *p2,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true);
void debugSolidTri(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,const HeF32 *n1,const HeF32 *n2,const HeF32 *n3,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f);
void debugSphere(const HeF32 *pos,HeF32 radius,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true);

void flushDebug(void);
void resetDebug(void);

#endif
