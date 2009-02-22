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

#pragma warning(disable:4100)


void testAutoGeometry(MeshSystemHelper *h)
{
  MESHIMPORT::MeshSystem *ms = h->getMeshSystem();
  if ( ms )
  {
    if ( ms->mSkeletonCount && ms->mMeshCount )
    {
      MESHIMPORT::MeshSkeleton *sk = ms->mSkeletons[0];
      MESHIMPORT::MeshSkeletonInstance *msk = gMeshImport->createMeshSkeletonInstance(*sk); // create a world-space instance of the skeleton.
      if ( msk )
      {
        AUTO_GEOMETRY::SimpleBone *bones = MEMALLOC_NEW_ARRAY(AUTO_GEOMETRY::SimpleBone,msk->mBoneCount)[msk->mBoneCount];

        AUTO_GEOMETRY::SimpleBone *b = bones;
        MESHIMPORT::MeshBoneInstance *binst = msk->mBones;
        for (int i=0; i<msk->mBoneCount; i++)
        {
          if ( stristr(binst->mBoneName,"finger") ) b->mOption = AUTO_GEOMETRY::BO_COLLAPSE;
          if ( stristr(binst->mBoneName,"toe") )    b->mOption = AUTO_GEOMETRY::BO_COLLAPSE;

          b->mParentIndex = binst->mParentIndex;
          b->mBoneName = binst->mBoneName;
          memcpy(b->mTransform,binst->mTransform,sizeof(float)*16 );
          memcpy(b->mInverseTransform, binst->mInverseTransform,sizeof(float)*16 );
          b++;
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
            s.mPos[0] = v.mPos[0];
            s.mPos[1] = v.mPos[1];
            s.mPos[2] = v.mPos[2];
            s.mBone[0] = v.mBone[0];
            s.mBone[1] = v.mBone[1];
            s.mBone[2] = v.mBone[2];
            s.mBone[3] = v.mBone[3];
            s.mWeight[0] = v.mWeight[0];
            s.mWeight[1] = v.mWeight[1];
            s.mWeight[2] = v.mWeight[2];
            s.mWeight[3] = v.mWeight[3];
            vertices.push_back(s);
          }

          for (unsigned int i=0; i<m->mSubMeshCount; i++)
          {
            MESHIMPORT::SubMesh *sm = m->mSubMeshes[i];
            unsigned int icount = sm->mTriCount*3;
            for (unsigned int j=0; j<icount; j++)
            {
              indices.push_back( sm->mIndices[j]+base_index );
            }
          }
        }

        AUTO_GEOMETRY::SimpleSkinnedMesh ssm;
        ssm.mVertexCount   = vertices.size();
        ssm.mVertices = &vertices[0];

        AUTO_GEOMETRY::AutoGeometry *ag = AUTO_GEOMETRY::createAutoGeometry();
        if ( ag )
        {
          unsigned int geom_count;
          AUTO_GEOMETRY::SimpleHull **hulls = ag->createCollisionVolumes(0,sk->mBoneCount,bones,&ssm,geom_count);
          if ( hulls )
          {
            SEND_TEXT_MESSAGE(0,"%d input bones produced %d output hulls.\r\n", sk->mBoneCount, geom_count );
            MESHIMPORT::MeshSystemContainer *msc = h->getMeshSystemContainer();
            MESHIMPORT::MeshImportInterface *iface = gMeshImport->getMeshImportInterface(msc);

            iface->importCollisionRepresentation("ClothCollision",0);

            for (unsigned int i=0; i<geom_count; i++)
            {
              AUTO_GEOMETRY::SimpleHull *h = hulls[i];

              iface->importConvexHull("ClothCollision", h->mBoneName, h->mTransform, h->mVertexCount, h->mVertices, h->mTriCount, h->mIndices );
#if 0
              unsigned int color = gRenderDebug->getDebugColor();
              for (unsigned int j=0; j<h->mTriCount; j++)
              {
                unsigned int i1 = h->mIndices[j*3+0];
                unsigned int i2 = h->mIndices[j*3+1];
                unsigned int i3 = h->mIndices[j*3+2];
                const float *p1 = &h->mVertices[i1*3];
                const float *p2 = &h->mVertices[i2*3];
                const float *p3 = &h->mVertices[i3*3];
                float v1[3];
                float v2[3];
                float v3[3];
                fm_transform(h->mTransform,p1,v1);
                fm_transform(h->mTransform,p2,v2);
                fm_transform(h->mTransform,p3,v3);
                gRenderDebug->DebugSolidTri(v1,v2,v3,color,60.0f);
                gRenderDebug->DebugTri(v1,v2,v3,0xFFFFFF,60.0f);
              }
#endif
            }
            gMeshImport->gather(msc);
          }
          AUTO_GEOMETRY::releaseAutoGeometry(ag);
        }

        MEMALLOC_DELETE_ARRAY(AUTO_GEOMETRY::SimpleBone,bones);

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
