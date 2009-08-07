#ifndef TERRAIN_H

#define TERRAIN_H

#include "UserMemAlloc.h"

class TerrainInterface
{
public:
  virtual void notifyTerrain(NxU32 vcount,const NxF32 *vertices,NxU32 tcount,const NxU32 *indices) = 0;
};

class Terrain
{
public:
  virtual void render(void) = 0;

  virtual NxU32  getWidth(void) = 0;
  virtual NxU32  getHeight(void) = 0;
  virtual NxF32 * getData(void) = 0;
  virtual void   rebuildMesh(void) = 0;
};

Terrain * createTerrain(const char *fname,TerrainInterface *iface);
void      releaseTerrain(Terrain *t);

#endif
