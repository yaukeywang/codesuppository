#ifndef SMOOTH_PATH_H

#define SMOOTH_PATH_H

#include "UserMemAlloc.h"

class SmoothPath;

SmoothPath  * sp_createSmoothPath(const HeF32 *points,HeU32 pstride,HeU32 count,HeF32 &length);
HeF32         sp_getPoint(SmoothPath *sp,HeF32 *dest,HeF32 ftime,HeU32 &index);
HeF32         sp_getPointLinear(SmoothPath *sp,HeF32 *dest,HeF32 ftime,HeU32 &index);
void          sp_releaseSmoothPath(SmoothPath *sp);


#endif
