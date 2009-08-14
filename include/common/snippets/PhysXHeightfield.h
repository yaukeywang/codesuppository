#ifndef PHYSX_HEIGHT_FIELD_H

#define PHYSX_HEIGHT_FIELD_H

#include "UserMemAlloc.h"

class PhysXHeightFieldInterface
{
public:
  virtual void receiveHeightFieldMesh(NxU32 vcount,const NxF32 *vertices,NxU32 tcount,const NxU32 *indices) = 0;
};

#define HF_STEP_SIZE 32    // the maximum row/column size for a sub-mesh.  Must be a #define because memory is allocated on the stack for this operation.  This routine is entirely thread safe!

void physXHeightFieldToMesh(NxI32 wid,                           // the width of the source heightfield
                            NxI32 hit,                           // the height of the source heightfield
                            const NxU32 *heightField,            // the raw 32 bit heightfield data.
                            NxF32 verticalExtent,                // the vertical extent of the heightfield
                            NxF32 columnScale,                   // the column scale
                            NxF32 rowScale,                      // the row scale
                            NxF32 heightScale,                   // the heightscale
                            NxF32 gameScale,                     // optional scale to change from game/graphics units into physics units.  Default should be 1 to 1
                            PhysXHeightFieldInterface *callback);// your interface to receive the mesh data an an indexed triangle mesh.

#endif
