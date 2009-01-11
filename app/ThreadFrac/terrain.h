#ifndef TERRAIN_H

#define TERRAIN_H

#include "common/snippets/HeSimpleTypes.h"

class Terrain;

namespace PD3D
{
  class Pd3dTexture;
};

Terrain * createTerrain(HeU32 wid,HeU32 hit,PD3D::Pd3dTexture *texture);

void      terrainPlot(HeU32 x,HeU32 y,HeU32 iterationCount);
void      terrainReset(Terrain *t);

void      releaseTerrain(Terrain *t);

void      renderTerrain(Terrain *t,bool wireframe,HeF32 dtime);

void      plot(Terrain *t,HeU32 x,HeU32 y,HeU32 iterCount);

void      terrainComplete(Terrain *t,const HeU32 *data); // the terrain is completed, it can now be copied to a static vertex buffer and index buffer and optimized.

void terrainClampRange(Terrain *t,HeF32 clampLow,HeF32 clampHigh,HeF32 clampScale,HeU32 iterationCount);

HeF32 getRotation(Terrain *t);
void  setRotation(Terrain *t,HeF32 rot);

void  filter(Terrain *t);
void  optimize(Terrain *t);

void  setShowNormals(bool state);
void  setRotationSpeed(HeF32 rspeed);

#endif
