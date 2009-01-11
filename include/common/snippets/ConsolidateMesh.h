#ifndef CONSOLIDATE_MESH_H

#define CONSOLIDATE_MESH_H


class ConsolidateMeshOutput
{
public:
  size_t        mVcount;
  size_t        mTcount;
  const float  *mVerticesFloat;
  const double *mVerticesDouble;
  const size_t *mIndices;
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
