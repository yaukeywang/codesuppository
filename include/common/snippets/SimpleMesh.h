#ifndef SIMPLE_MESH_H

#define SIMPLE_MESH_H

// defines a triangle mesh containing ony geometry (i.e) position data.
#include "UserMemAlloc.h"

class SimpleMesh
{
public:
  SimpleMesh(void)
  {
    mVcount = 0;
    mVertices = 0;
    mTcount = 0;
    mIndices = 0;
  }
  ~SimpleMesh(void)
  {
    delete []mVertices;
    delete []mIndices;
  }

  NxU32 mVcount;
  const NxF32 *mVertices;
  NxU32 mTcount;
  NxU32 mIndices;
};

#endif
