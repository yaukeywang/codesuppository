#ifndef OPEN_CLOSED_MESH_H

#define OPEN_CLOSED_MESH_H

#include "UserMemAlloc.h"
// Determines whether a triangle mesh is open or closed.
// If it is open, it can return the open edges via a callback interface.

class OpenClosedMesh
{
public:

  virtual const size_t * isClosedMesh(size_t tcount,size_t *indices,size_t &ecount) = 0;

};

OpenClosedMesh * createOpenClosedMesh(void);
void             releaseOpenClosedMesh(OpenClosedMesh *mesh);

#endif
