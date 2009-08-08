#ifndef MESH_CONSOLIDATE_H

#define MESH_CONSLIDATE_H

#include "NxSimpleTypes.h"

class MeshConsolidateOutput
{
public:
  MeshConsolidateOutput(void)
  {
    mVcount = 0;
    mTcount = 0;
    mVertices = 0;
    mIndices = 0;
    mSubMeshes = 0;
    mEpsilon = 0.00000001f;
  }
  NxU32        mVcount;
  NxU32        mTcount;
  const float  *mVertices;
  const NxU32 *mIndices;
  const NxU32 *mIds;
  const NxU32 *mSubMeshes;
  NxF32        mEpsilon;
};

class MeshConsolidate
{
public:

  virtual bool addTriangle(const float *p1,
                           const float *p2,
                           const float *p3,
                           NxU32 id,
                           NxU32 subMesh) = 0;

  virtual bool meshConsolidate(MeshConsolidateOutput &results) = 0;


};

MeshConsolidate * createMeshConsolidate(float epsilon);
void              releaseMeshConsolidate(MeshConsolidate *cm);

#endif
