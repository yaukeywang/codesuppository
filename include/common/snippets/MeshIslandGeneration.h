#ifndef MESH_ISLAND_GENERATION_H

#define MESH_ISLAND_GENERATION_H

#include "UserMemAlloc.h"

namespace NVSHARE
{

class MeshIslandGeneration
{
public:

  virtual NxU32 islandGenerate(NxU32 tcount,const NxU32 *indices,const NxF32 *vertices) = 0;
  virtual NxU32 islandGenerate(NxU32 tcount,const NxU32 *indices,const NxF64 *vertices) = 0;

  // sometimes island generation can produce co-planar islands.  Slivers if you will.  If you are passing these islands into a geometric system
  // that wants to turn them into physical objects, they may not be acceptable.  In this case it may be preferable to merge the co-planar islands with
  // other islands that it 'touches'.
  virtual NxU32 mergeCoplanarIslands(const NxF32 *vertices) = 0;
  virtual NxU32 mergeCoplanarIslands(const NxF64 *vertices) = 0;

  virtual NxU32 mergeTouchingIslands(const NxF32 *vertices) = 0;
  virtual NxU32 mergeTouchingIslands(const NxF64 *vertices) = 0;

  virtual NxU32 *   getIsland(NxU32 index,NxU32 &tcount) = 0;


};

MeshIslandGeneration * createMeshIslandGeneration(void);
void                   releaseMeshIslandGeneration(MeshIslandGeneration *cm);

}; // end of namespace

#endif
