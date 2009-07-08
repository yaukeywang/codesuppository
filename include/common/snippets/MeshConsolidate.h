#ifndef MESH_CONSOLIDATE_H

#define MESH_CONSLIDATE_H

#include "HeSimpleTypes.h"

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
  }
  HeU32        mVcount;
  HeU32        mTcount;
  const float  *mVertices;
  const HeU32 *mIndices;
  const HeU32 *mIds;
  const HeU32 *mSubMeshes;
};

class MeshConsolidate
{
public:

  virtual bool addTriangle(const float *p1,
                           const float *p2,
                           const float *p3,
                           HeU32 id,
                           HeU32 subMesh) = 0;

  virtual bool meshConsolidate(MeshConsolidateOutput &results) = 0;


};

MeshConsolidate * createMeshConsolidate(void);
void              releaseMeshConsolidate(MeshConsolidate *cm);

#endif
