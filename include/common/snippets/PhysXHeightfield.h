#ifndef PHYSX_HEIGHT_FIELD_H

#define PHYSX_HEIGHT_FIELD_H

class PhysXHeightFieldInterface
{
public:
  virtual void receiveHeightFieldMesh(unsigned int vcount,const float *vertices,unsigned int tcount,const unsigned int *indices) = 0;
};

#define HF_STEP_SIZE 32    // the maximum row/column size for a sub-mesh.  Must be a #define because memory is allocated on the stack for this operation.  This routine is entirely thread safe!

void physXHeightFieldToMesh(int wid,                           // the width of the source heightfield
                            int hit,                           // the height of the source heightfield
                            const unsigned int *heightField,            // the raw 32 bit heightfield data.
                            float verticalExtent,                // the vertical extent of the heightfield
                            float columnScale,                   // the column scale
                            float rowScale,                      // the row scale
                            float heightScale,                   // the heightscale
                            float gameScale,                     // optional scale to change from game/graphics units into physics units.  Default should be 1 to 1
                            PhysXHeightFieldInterface *callback);// your interface to receive the mesh data an an indexed triangle mesh.

#endif
