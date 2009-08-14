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
    MEMALLOC_DELETE_ARRAY(const NxF32,mVertice);
    MEMALLOC_DELETE_ARRAY(NxU32,mIndices);
  }

  NxU32 mVcount;
  const NxF32 *mVertices;
  NxU32 mTcount;
  NxU32 mIndices;
};

#endif
