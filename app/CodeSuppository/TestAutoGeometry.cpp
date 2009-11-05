#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <vector>

#include "TestAutoGeometry.h"

using namespace NVSHARE;
#include "AutoGeometry.h"
#include "MeshImport.h"
#include "shared/MeshSystem/MeshSystemHelper.h"
#include "UserMemAlloc.h"
#include "sutil.h"
#include "FloatMath.h"
#include "RenderDebug.h"
#include "JobSwarm.h"

#pragma warning(disable:4100)

#if 0
class MyTestAutoGeometry : public TestAutoGeometry
{
public:
  MyTestAutoGeometry(MeshSystemHelper *h)
  {
    mAutoGeometry = 0;
    mHelper = h;

    NVSHARE::MeshSystem *ms = h->getMeshSystem();
    if ( ms )
    {
      if ( ms->mSkeletonCount && ms->mMeshCount )
      {
        NVSHARE::MeshSkeleton *sk = ms->mSkeletons[0];
        NVSHARE::MeshSkeletonInstance *msk = gMeshImport->createMeshSkeletonInstance(*sk); // create a world-space instance of the skeleton.
        if ( msk )
        {
          NVSHARE::AutoGeometry *ag = NVSHARE::createAutoGeometry();
          mAutoGeometry = ag;

          NVSHARE::MeshBoneInstance *binst = msk->mBones;
          for (NxI32 i=0; i<msk->mBoneCount; i++)
          {
            NVSHARE::SimpleBone b;

            if ( stristr(binst->mBoneName,"finger") ) b.mOption = NVSHARE::BO_COLLAPSE;
            if ( stristr(binst->mBoneName,"toe") )    b.mOption = NVSHARE::BO_COLLAPSE;

            b.mParentIndex = binst->mParentIndex;
            b.mBoneName = binst->mBoneName;
            memcpy(b.mTransform,binst->mTransform,sizeof(NxF32)*16 );
            memcpy(b.mInverseTransform, binst->mInverseTransform,sizeof(NxF32)*16 );
            ag->addSimpleBone(b);
            binst++;
          }

          typedef std::vector< NxU32 > UintVector;
          typedef std::vector< NVSHARE::SimpleSkinnedVertex > SimpleSkinnedVertexVector;

          UintVector indices;
          SimpleSkinnedVertexVector vertices;

          for (NxU32 k=0; k<ms->mMeshCount; k++)
          {
            NVSHARE::Mesh *m = ms->mMeshes[k];
            NxU32 base_index = vertices.size();
            for (NxU32 i=0; i<m->mVertexCount; i++)
            {
              NVSHARE::MeshVertex &v = m->mVertices[i];
              NVSHARE::SimpleSkinnedVertex s;
              s.mPos[0]    = v.mPos[0];
              s.mPos[1]    = v.mPos[1];
              s.mPos[2]    = v.mPos[2];
              s.mBone[0]   = v.mBone[0];
              s.mBone[1]   = v.mBone[1];
              s.mBone[2]   = v.mBone[2];
              s.mBone[3]   = v.mBone[3];
              s.mWeight[0] = v.mWeight[0];
              s.mWeight[1] = v.mWeight[1];
              s.mWeight[2] = v.mWeight[2];
              s.mWeight[3] = v.mWeight[3];
              vertices.push_back(s);
            }

            for (NxU32 i=0; i<m->mSubMeshCount; i++)
            {
              NVSHARE::SubMesh *sm = m->mSubMeshes[i];

              if ( stristr(sm->mMaterialName,"__cloth") == 0 ) // if not cloth
              {
                for (NxU32 j=0; j<sm->mTriCount; j++)
                {
                  NxU32 i1 = sm->mIndices[j*3+0]+base_index;
                  NxU32 i2 = sm->mIndices[j*3+1]+base_index;
                  NxU32 i3 = sm->mIndices[j*3+2]+base_index;

                  const NVSHARE::SimpleSkinnedVertex &v1 = vertices[i1];
                  const NVSHARE::SimpleSkinnedVertex &v2 = vertices[i2];
                  const NVSHARE::SimpleSkinnedVertex &v3 = vertices[i3];
                  ag->addSimpleSkinnedTriangle(v1,v2,v3);
                }
              }
            }

            bool ok = ag->createCollisionVolumes(0,gJobSwarmContext);
            if ( !ok )
            {
              releaseAutoGeometry(ag);
              mAutoGeometry = 0;
            }

          }

        }
        else
        {
          SEND_TEXT_MESSAGE(0,"Failed to create an instance of the skeleton.\r\n");
        }
      }
      else
      {
        SEND_TEXT_MESSAGE(0,"The input mesh does not have a skeleton.\r\n");
      }
    }
    else
    {
      SEND_TEXT_MESSAGE(0,"No Mesh loaded.\r\n");
    }

  }


  virtual bool pump(void)  // returns true if still processing, false it can be released.
  {
    bool ret  = false;
    if ( mAutoGeometry )
    {
      NxU32 geom_count;
      bool ready;
      NVSHARE::SimpleHull **hulls = mAutoGeometry->getResults(geom_count,ready);
      if ( ready )
      {
        if ( hulls )
        {
          NVSHARE::MeshSystemContainer *msc = mHelper->getMeshSystemContainer();
          NVSHARE::MeshImportInterface *iface = gMeshImport->getMeshImportInterface(msc);
          iface->importCollisionRepresentation("ClothCollision",0);
          for (NxU32 i=0; i<geom_count; i++)
          {
            NVSHARE::SimpleHull *h = hulls[i];

            NVSHARE::MeshCollisionType t = NVSHARE::MCT_CAPSULE;

            if ( (h->mSphereVolume*0.9f) < h->mOBBVolume )
            {
              t = NVSHARE::MCT_SPHERE;
            }
            else if ( (h->mOBBVolume*0.95f) < h->mCapsuleVolume )
            {
              t = NVSHARE::MCT_BOX;
            }

            t = NVSHARE::MCT_CONVEX;

            switch ( t )
            {
              case NVSHARE::MCT_SPHERE:
                {
                  NxF32 matrix[16];
                  fm_identity(matrix);
                  fm_setTranslation(matrix,h->mSphereCenter);
                  iface->importSphere("ClothCollision", h->mBoneName, matrix, h->mSphereRadius ); 
                  SEND_TEXT_MESSAGE(0,"Generated sphere for bone: %s Volume: %0.6f\r\n", h->mBoneName, h->mMeshVolume );
                }
                break;
              case NVSHARE::MCT_BOX:
                iface->importOBB("ClothCollision", h->mBoneName, h->mOBBTransform, h->mOBBSides );
                SEND_TEXT_MESSAGE(0,"Generated OBB for bone: %s Volume: %0.6f\r\n", h->mBoneName, h->mMeshVolume );
                break;
              case NVSHARE::MCT_CAPSULE:
                {
                  const NxF32 capsuleScale = 1.0f;
                  if ( h->mCapsuleHeight <= 0 )
                  {
                    iface->importSphere("ClothCollision", h->mBoneName, h->mCapsuleTransform, h->mCapsuleRadius*capsuleScale );
                    SEND_TEXT_MESSAGE(0,"Generated sphere(c) for bone: %s Volume: %0.6f\r\n", h->mBoneName, h->mMeshVolume );
                  }
                  else
                  {
                    iface->importCapsule("ClothCollision", h->mBoneName, h->mCapsuleTransform, h->mCapsuleRadius*capsuleScale, h->mCapsuleHeight*capsuleScale );
                    SEND_TEXT_MESSAGE(0,"Generated capsule for bone: %s Volume: %0.6f\r\n", h->mBoneName, h->mMeshVolume );
                  }
                }
                break;
              case NVSHARE::MCT_CONVEX:
                iface->importConvexHull("ClothCollision", h->mBoneName, h->mConvexTransform, h->mVertexCount, h->mVertices, h->mTriCount, h->mIndices );
                SEND_TEXT_MESSAGE(0,"Generated hull for bone: %s Volume: %0.6f\r\n", h->mBoneName, h->mMeshVolume );
                break;
            }
          }
          gMeshImport->gather(msc);
          NVSHARE::releaseAutoGeometry(mAutoGeometry);
          mAutoGeometry = 0;
        }
      }
      else
      {
        ret = true; // it's not ready yet...
      }
    }
    return ret;
  }

private:
  MeshSystemHelper            *mHelper;
  NVSHARE::AutoGeometry *mAutoGeometry;
};



TestAutoGeometry * createTestAutoGeometry(MeshSystemHelper *h)
{
  MyTestAutoGeometry *m = MEMALLOC_NEW(MyTestAutoGeometry)(h);
  return static_cast< TestAutoGeometry *>(m);
}

void               releaseTestAutoGeometry(TestAutoGeometry *t)
{
  MyTestAutoGeometry *m = static_cast< MyTestAutoGeometry *>(t);
  delete m;
}

#else

TestAutoGeometry * createTestAutoGeometry(MeshSystemHelper *h)
{
	return 0;
}

void               releaseTestAutoGeometry(TestAutoGeometry *t)
{
}


#endif