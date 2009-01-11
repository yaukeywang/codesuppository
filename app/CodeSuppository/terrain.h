#ifndef TERRAIN_H

#define TERRAIN_H

#include "common/snippets/UserMemAlloc.h"

class TerrainInterface
{
public:
  virtual void notifyTerrain(HeU32 vcount,const HeF32 *vertices,HeU32 tcount,const HeU32 *indices) = 0;
};

class Terrain
{
public:
  virtual void render(void) = 0;

  virtual HeU32  getWidth(void) = 0;
  virtual HeU32  getHeight(void) = 0;
  virtual HeF32 * getData(void) = 0;
  virtual void   rebuildMesh(void) = 0;
};

Terrain * createTerrain(const char *fname,TerrainInterface *iface);
void      releaseTerrain(Terrain *t);

#endif
