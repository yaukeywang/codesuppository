#ifndef MARCHING_CUBES_H

#define MARCHING_CUBES_H

#include "UserMemAlloc.h"

class iMarchingCubes
{
public:
protected:
	~iMarchingCubes(void) { };
};


iMarchingCubes *createMarchingCubes(NxU32 xWid,NxU32 yWid,NxU32 zWid);
void		    releaseMarchingCubes(iMarchingCubes *cubes);

#endif
