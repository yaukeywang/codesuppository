#ifndef TERRAIN_H

#define TERRAIN_H

#include "common/snippets/UserMemAlloc.h"

class Terrain;

namespace NVSHARE
{
  class Pd3dTexture;
};


Terrain * createTerrain(NxU32 wid,NxU32 hit,NVSHARE::Pd3dTexture *texture);

void      terrainPlot(NxU32 x,NxU32 y,NxU32 iterationCount);
void      terrainReset(Terrain *t);

void      releaseTerrain(Terrain *t);

void      renderTerrain(Terrain *t,bool wireframe,NxF32 dtime);

void      plot(Terrain *t,NxU32 x,NxU32 y,NxU32 iterCount);

void      terrainComplete(Terrain *t,const NxU32 *data); // the terrain is completed, it can now be copied to a static vertex buffer and index buffer and optimized.

void terrainClampRange(Terrain *t,NxF32 clampLow,NxF32 clampHigh,NxF32 clampScale,NxU32 iterationCount);

NxF32 getRotation(Terrain *t);
void  setRotation(Terrain *t,NxF32 rot);

void  filter(Terrain *t);
void  optimize(Terrain *t);

void  setShowNormals(bool state);
void  setRotationSpeed(NxF32 rspeed);


#endif
