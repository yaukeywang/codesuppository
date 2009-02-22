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
    MEMALLOC_DELETE_ARRAY(const float,mVertice);
    MEMALLOC_DELETE_ARRAY(unsigned int,mIndices);
  }

  unsigned int mVcount;
  const float *mVertices;
  unsigned int mTcount;
  unsigned int mIndices;
};

#endif
