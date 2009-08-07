#ifndef SMOOTH_PATH_H

#define SMOOTH_PATH_H

#include "UserMemAlloc.h"

class SmoothPath;

SmoothPath  * sp_createSmoothPath(const NxF32 *points,NxU32 pstride,NxU32 count,NxF32 &length);
NxF32         sp_getPoint(SmoothPath *sp,NxF32 *dest,NxF32 ftime,NxU32 &index);
NxF32         sp_getPointLinear(SmoothPath *sp,NxF32 *dest,NxF32 ftime,NxU32 &index);
void          sp_releaseSmoothPath(SmoothPath *sp);


#endif
