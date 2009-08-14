#ifndef CONSOLIDATE_MESH_H

#define CONSOLIDATE_MESH_H

#include "UserMemAlloc.h"

class ConsolidateMeshOutput
{
public:
  NxU32        mVcount;
  NxU32        mTcount;
  const NxF32  *mVerticesFloat;
  const NxF64 *mVerticesDouble;
  const NxU32 *mIndices;
};

class ConsolidateMesh
{
public:

  virtual bool addTriangle(const NxF32 *p1,const NxF32 *p2,const NxF32 *p3) = 0;
  virtual bool addTriangle(const NxF64 *p1,const NxF64 *p2,const NxF64 *p3) = 0;

  virtual bool consolidateMesh(ConsolidateMeshOutput &results) = 0;


};

ConsolidateMesh * createConsolidateMesh(void);
void              releaseConsolidateMesh(ConsolidateMesh *cm);

#endif
