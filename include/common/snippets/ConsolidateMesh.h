#ifndef CONSOLIDATE_MESH_H

#define CONSOLIDATE_MESH_H

#include "He.h"

class ConsolidateMeshOutput
{
public:
  HeU32        mVcount;
  HeU32        mTcount;
  const float  *mVerticesFloat;
  const double *mVerticesDouble;
  const HeU32 *mIndices;
};

class ConsolidateMesh
{
public:

  virtual bool addTriangle(const float *p1,const float *p2,const float *p3) = 0;
  virtual bool addTriangle(const double *p1,const double *p2,const double *p3) = 0;

  virtual bool consolidateMesh(ConsolidateMeshOutput &results) = 0;


};

ConsolidateMesh * createConsolidateMesh(void);
void              releaseConsolidateMesh(ConsolidateMesh *cm);

#endif
