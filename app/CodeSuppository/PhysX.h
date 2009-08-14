#ifndef PHYSX_H

#define PHYSX_H

#include "UserMemAlloc.h"

class PhysX
{
public:
  virtual void importHeightMap(const char *fname) = 0;
  virtual void render(NxF32 dtime) = 0;
  virtual void simulate(NxF32 dtime) = 0;
};


extern PhysX *gPhysX;

#endif
