#ifndef DEBUG_MSG_H

#define DEBUG_MSG_H

#include "UserMemAlloc.h"

#ifdef DEBUG_SERVER

bool openDebug(void);
bool closeDebug(void);
bool processDebug(void);

#endif

void debugTri(const NxF32 *p1,const NxF32 *p2,const NxF32 *p3,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true);
void debugSolidTri(const NxF32 *p1,const NxF32 *p2,const NxF32 *p3,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f);
void debugPoint(const NxF32 *pos,NxF32 radius,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true);
void debugLine(const NxF32 *p1,const NxF32 *p2,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true);
void debugSolidTri(const NxF32 *p1,const NxF32 *p2,const NxF32 *p3,const NxF32 *n1,const NxF32 *n2,const NxF32 *n3,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f);
void debugSphere(const NxF32 *pos,NxF32 radius,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true);

void flushDebug(void);
void resetDebug(void);

#endif
