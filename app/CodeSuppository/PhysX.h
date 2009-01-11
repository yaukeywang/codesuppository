#ifndef PHYSX_H

#define PHYSX_H

class PhysX
{
public:
  virtual void importHeightMap(const char *fname) = 0;
  virtual void render(float dtime) = 0;
  virtual void simulate(float dtime) = 0;
};


extern PhysX *gPhysX;

#endif
