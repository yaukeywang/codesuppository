#ifndef MESH_ISLAND_GENERATION_H

#define MESH_ISLAND_GENERATION_H


class MeshIslandGeneration
{
public:

  virtual size_t islandGenerate(size_t tcount,const size_t *indices,const float *vertices) = 0;
  virtual size_t islandGenerate(size_t tcount,const size_t *indices,const double *vertices) = 0;

  // sometimes island generation can produce co-planar islands.  Slivers if you will.  If you are passing these islands into a geometric system
  // that wants to turn them into physical objects, they may not be acceptable.  In this case it may be preferable to merge the co-planar islands with
  // other islands that it 'touches'.
  virtual size_t mergeCoplanarIslands(const float *vertices) = 0;
  virtual size_t mergeCoplanarIslands(const double *vertices) = 0;

  virtual size_t mergeTouchingIslands(const float *vertices) = 0;
  virtual size_t mergeTouchingIslands(const double *vertices) = 0;

  virtual size_t *   getIsland(size_t index,size_t &tcount) = 0;


};

MeshIslandGeneration * createMeshIslandGeneration(void);
void                   releaseMeshIslandGeneration(MeshIslandGeneration *cm);

#endif
