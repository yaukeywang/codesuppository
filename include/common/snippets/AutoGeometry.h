#ifndef AUTO_GEOMETRY_H

#define AUTO_GEOMETRY_H

#include "UserMemAlloc.h"

namespace JOB_SWARM
{
  class JobSwarmContext;
};

namespace AUTO_GEOMETRY
{

class SimpleHull
{
public:
  SimpleHull(void)
  {
    mBoneIndex = 0;
    mVertexCount = 0;
    mVertices  = 0;
    mTriCount = 0;
    mIndices = 0;
    mMeshVolume = 0;
    mParentIndex = -1;
    mBoneName = 0;
  }
  NxI32           mBoneIndex;
  NxI32           mParentIndex;
  const char   *mBoneName;
  NxF32         mConvexTransform[16];
  NxU32  mVertexCount;
  NxF32        *mVertices;
  NxU32  mTriCount;
  NxU32 *mIndices;
  NxF32         mMeshVolume;

  // OBB
  NxF32         mOBBVolume;
  NxF32         mOBBTransform[16];
  NxF32         mOBBSides[3];

  // sphere
  NxF32         mSphereVolume;
  NxF32         mSphereRadius;
  NxF32         mSphereCenter[3];

  // Capsule
  NxF32         mCapsuleVolume;
  NxF32         mCapsuleHeight;
  NxF32         mCapsuleRadius;
  NxF32         mCapsuleTransform[16];
};

enum BoneOption
{
  BO_INCLUDE,
  BO_EXCLUDE,
  BO_COLLAPSE
};

class SimpleBone
{
public:
  SimpleBone(void)
  {
    mOption = BO_INCLUDE;
    mParentIndex = -1;
    mBoneName = 0;
  }
  BoneOption   mOption;
  const char  *mBoneName;
  NxI32          mParentIndex;
  NxF32        mTransform[16];
  NxF32        mInverseTransform[16];
};

class SimpleSkinnedVertex
{
public:
  NxF32          mPos[3];
  unsigned short mBone[4];
  NxF32          mWeight[4];
};

class SimpleSkinnedMesh
{
public:
  NxU32         mVertexCount;
  SimpleSkinnedVertex *mVertices;

};

class AutoGeometry
{
public:


  virtual bool createCollisionVolumes(NxF32 collapse_percentage,JOB_SWARM::JobSwarmContext *context) = 0;
  virtual SimpleHull ** getResults(NxU32 &geom_count,bool &ready) = 0;

  virtual SimpleHull ** createCollisionVolumes(NxF32 collapse_percentage,NxU32 &geom_count) = 0;

  virtual void addSimpleSkinnedTriangle(const SimpleSkinnedVertex &v1,
                                        const SimpleSkinnedVertex &v2,
                                        const SimpleSkinnedVertex &v3) = 0;

  virtual void addSimpleBone(const SimpleBone &b) = 0;

  virtual const char * stristr(const char *str,const char *match) = 0; // case insensitive ststr

  virtual bool cancel(void) = 0;
  virtual bool isFinish(void) const = 0;

};

AutoGeometry * createAutoGeometry(void);
void           releaseAutoGeometry(AutoGeometry *g);

}; // end of namespace


#endif
