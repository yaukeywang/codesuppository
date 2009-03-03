#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <vector>

#include "TestAutoGeometry.h"

#include "common/snippets/SendTextMessage.h"
#include "common/AutoGeometry/AutoGeometry.h"
#include "MeshImport/MeshImport.h"
#include "shared/MeshSystem/MeshSystemHelper.h"
#include "common/snippets/UserMemAlloc.h"
#include "common/snippets/sutil.h"
#include "common/snippets/FloatMath.h"
#include "RenderDebug/RenderDebug.h"
#include "common/snippets/JobSwarm.h"

#pragma warning(disable:4100)


class MyTestAutoGeometry : public TestAutoGeometry
{
public:
  MyTestAutoGeometry(MeshSystemHelper *h)
  {
    mAutoGeometry = 0;
    mHelper = h;

    MESHIMPORT::MeshSystem *ms = h->getMeshSystem();
    if ( ms )
    {
      if ( ms->mSkeletonCount && ms->mMeshCount )
      {
        MESHIMPORT::MeshSkeleton *sk = ms->mSkeletons[0];
        MESHIMPORT::MeshSkeletonInstance *msk = gMeshImport->createMeshSkeletonInstance(*sk); // create a world-space instance of the skeleton.
        if ( msk )
        {
          AUTO_GEOMETRY::AutoGeometry *ag = AUTO_GEOMETRY::createAutoGeometry();
          mAutoGeometry = ag;

          MESHIMPORT::MeshBoneInstance *binst = msk->mBones;
          for (int i=0; i<msk->mBoneCount; i++)
          {
            AUTO_GEOMETRY::SimpleBone b;

            if ( stristr(binst->mBoneName,"finger") ) b.mOption = AUTO_GEOMETRY::BO_COLLAPSE;
            if ( stristr(binst->mBoneName,"toe") )    b.mOption = AUTO_GEOMETRY::BO_COLLAPSE;

            b.mParentIndex = binst->mParentIndex;
            b.mBoneName = binst->mBoneName;
            memcpy(b.mTransform,binst->mTransform,sizeof(float)*16 );
            memcpy(b.mInverseTransform, binst->mInverseTransform,sizeof(float)*16 );
            ag->addSimpleBone(b);
            binst++;
          }

          typedef USER_STL::vector< unsigned int > UintVector;
          typedef USER_STL::vector< AUTO_GEOMETRY::SimpleSkinnedVertex > SimpleSkinnedVertexVector;

          UintVector indices;
          SimpleSkinnedVertexVector vertices;

          for (unsigned int k=0; k<ms->mMeshCount; k++)
          {
            MESHIMPORT::Mesh *m = ms->mMeshes[k];
            unsigned int base_index = vertices.size();
            for (unsigned int i=0; i<m->mVertexCount; i++)
            {
              MESHIMPORT::MeshVertex &v = m->mVertices[i];
              AUTO_GEOMETRY::SimpleSkinnedVertex s;
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

            for (unsigned int i=0; i<m->mSubMeshCount; i++)
            {
              MESHIMPORT::SubMesh *sm = m->mSubMeshes[i];

              if ( stristr(sm->mMaterialName,"__cloth") == 0 ) // if not cloth
              {
                for (unsigned int j=0; j<sm->mTriCount; j++)
                {
                  HeU32 i1 = sm->mIndices[j*3+0]+base_index;
                  HeU32 i2 = sm->mIndices[j*3+1]+base_index;
                  HeU32 i3 = sm->mIndices[j*3+2]+base_index;

                  const AUTO_GEOMETRY::SimpleSkinnedVertex &v1 = vertices[i1];
                  const AUTO_GEOMETRY::SimpleSkinnedVertex &v2 = vertices[i2];
                  const AUTO_GEOMETRY::SimpleSkinnedVertex &v3 = vertices[i3];
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
      unsigned int geom_count;
      bool ready;
      AUTO_GEOMETRY::SimpleHull **hulls = mAutoGeometry->getResults(geom_count,ready);
      if ( ready )
      {
        if ( hulls )
        {
          MESHIMPORT::MeshSystemContainer *msc = mHelper->getMeshSystemContainer();
          MESHIMPORT::MeshImportInterface *iface = gMeshImport->getMeshImportInterface(msc);
          iface->importCollisionRepresentation("ClothCollision",0);
          for (unsigned int i=0; i<geom_count; i++)
          {
            AUTO_GEOMETRY::SimpleHull *h = hulls[i];

#if 0
            float combined[16];
            float local[16];
            fm_identity(local);
            local[12] = h->mCenter[0];
            local[13] = h->mCenter[1];
            local[14] = h->mCenter[2];
            fm_multiplyTransform( local, h->mTransform, combined );

            const float *pos = &combined[12];
            gRenderDebug->DebugPoint(pos,1.0f,0xFFFFFF,60.0f);
            gRenderDebug->DebugDetailedSphere(pos, h->mRadius,16, 0x0000FF, 60.0f, true, true );
            gRenderDebug->DebugDetailedSphere(pos, h->mRadius,16, 0xFFFFFF, 60.0f, true, false);
#else
            float combined[16];
            fm_multiplyTransform( h->mLocalTransform, h->mTransform, combined );
//            gRenderDebug->DebugOrientedBound( h->mSides, combined, 0x0000FF, 60.0f, true, true );
            gRenderDebug->DebugOrientedBound( h->mSides, combined, 0xFFFFFF, 60.0f, true, false );
#endif
            iface->importConvexHull("ClothCollision", h->mBoneName, h->mTransform, h->mVertexCount, h->mVertices, h->mTriCount, h->mIndices );
            SEND_TEXT_MESSAGE(0,"Generated hull for bone: %s Volume: %0.6f\r\n", h->mBoneName, h->mMeshVolume );
          }
          gMeshImport->gather(msc);
          AUTO_GEOMETRY::releaseAutoGeometry(mAutoGeometry);
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
  AUTO_GEOMETRY::AutoGeometry *mAutoGeometry;
};



TestAutoGeometry * createTestAutoGeometry(MeshSystemHelper *h)
{
  MyTestAutoGeometry *m = MEMALLOC_NEW(MyTestAutoGeometry)(h);
  return static_cast< TestAutoGeometry *>(m);
}

void               releaseTestAutoGeometry(TestAutoGeometry *t)
{
  MyTestAutoGeometry *m = static_cast< MyTestAutoGeometry *>(t);
  MEMALLOC_DELETE(MyTestAutoGeometry,m);
}
