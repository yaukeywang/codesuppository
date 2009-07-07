#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#pragma warning(disable:4702)

#include "MeshConsolidate.h"
#include "FloatMath.h"
#include "He.h"

#pragma warning(disable:4100)

#include <vector>
#include "UserMemAlloc.h"

namespace MESH_CONSOLIDATE
{

class MyMeshConsolidate : public MeshConsolidate
{
public:

  MyMeshConsolidate(void)
  {
  }

  ~MyMeshConsolidate(void)
  {
  }

  virtual bool addTriangle(const float *p1,
                           const float *p2,
                           const float *p3,
                           HeU32 id,
                           HeU32 subMesh)
  {
    bool ret = false;
    return ret;
  }


  virtual bool meshConsolidate(MeshConsolidateOutput &results)
  {
    bool ret = false;
    return ret;
  }

};

};

using namespace MESH_CONSOLIDATE;

MeshConsolidate * createMeshConsolidate(void)
{
  MyMeshConsolidate *mcm = new MyMeshConsolidate;
  return static_cast< MeshConsolidate *>(mcm);
}

void              releaseMeshConsolidate(MeshConsolidate *cm)
{
  MyMeshConsolidate *mcm = static_cast< MyMeshConsolidate *>(cm);
  delete mcm;
}

