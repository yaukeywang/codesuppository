#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "MeshImport/MeshImport.h"
#include "MeshSystemHelper.h"
#include "common/snippets/UserMemAlloc.h"
#include "common/snippets/fmem.h"
#include "common/snippets/sutil.h"
#include "common/snippets/SendTextMessage.h"
#include "RenderDebug/RenderDebug.h"
#include "common/snippets/stringdict.h"
#include "common/snippets/FloatMath.h"
#include "common/HeMath/HeFoundation.h"
#include "Pd3d/pd3d.h"

#pragma warning(disable:4100)

class MyMeshSystemHelper : public MeshSystemHelper
{
public:

  typedef USER_STL::vector< HeMat44 > HeMat44Vector;

  MyMeshSystemHelper(void)
  {
    mShowBounds = false;
    mFlipWinding = false;
    mMeshSystem = 0;
    mMeshSystemContainer = 0;
    mTime = 0;
    mSkeleton = 0;
    mShowWireframe = false;
    strcpy(mMaterial.mTexture,"wood.dds");
  }

  ~MyMeshSystemHelper(void)
  {
    release();
  }

  void release(void)
  {
    if ( mSkeleton )
    {
      gMeshImport->releaseMeshSkeletonInstance(mSkeleton);
      mSkeleton = 0;
    }
    if ( mMeshSystemContainer )
    {
      gMeshImport->releaseMeshSystemContainer(mMeshSystemContainer);
      mMeshSystemContainer = 0;
      mMeshSystem = 0;
    }
  }

  virtual MESHIMPORT::MeshSystem * getMeshSystem(void) const
  {
    return mMeshSystem;
  }

  virtual void debugRender(bool showMesh,bool showSkeleton,bool showWireframe,bool showAnimation,bool showCollision,bool flipWinding)
  {
    gRenderDebug->getDebugColor(true);
    mShowWireframe = showWireframe;
    mFlipWinding = flipWinding;

    if ( showAnimation && mSkeleton )
    {
      if ( mMeshSystem && showMesh )
      {
        for (HeU32 i=0; i<mMeshSystem->mMeshCount; i++)
        {
          debugRender( mMeshSystem->mMeshes[i], mSkeleton );
        }
      }
      if ( mSkeleton && showSkeleton )
      {
        for (int i=0; i<mSkeleton->mBoneCount; i++)
        {
          MESHIMPORT::MeshBoneInstance &b = mSkeleton->mBones[i];
          if ( b.mParentIndex != -1 )
          {
            MESHIMPORT::MeshBoneInstance &p = mSkeleton->mBones[b.mParentIndex];
            gRenderDebug->DebugThickRay(&p.mAnimTransform[12],&b.mAnimTransform[12],0.02f,0.05f, 0xFFFF00, 0xFF0000, 0.001f, true );
          }
          else
          {
            gRenderDebug->DebugDetailedSphere(&b.mAnimTransform[12], 0.1f, 16, 0xFFFFFF, 0.001f, true, true );
          }
        }
      }
      if ( showCollision )
      {
        if ( mMeshSystem->mMeshCollisionCount )
        {
          for (HeU32 i=0; i<mMeshSystem->mMeshCollisionCount; i++)
          {
            debugRender( mMeshSystem->mMeshCollisionRepresentations[i], mSkeleton );
          }
        }
      }
    }
    else
    {
      if ( mMeshSystem && showMesh )
      {
        if ( mShowBounds )
          gRenderDebug->DebugBound(mMeshSystem->mAABB.mMin, mMeshSystem->mAABB.mMax, 0xFFFFFF );
        for (HeU32 i=0; i<mMeshSystem->mMeshCount; i++)
        {
          debugRender( mMeshSystem->mMeshes[i] );
        }
      }
      if ( mSkeleton && showSkeleton )
      {
        for (int i=0; i<mSkeleton->mBoneCount; i++)
        {
          MESHIMPORT::MeshBoneInstance &b = mSkeleton->mBones[i];
          if ( b.mParentIndex != -1 )
          {
            MESHIMPORT::MeshBoneInstance &p = mSkeleton->mBones[b.mParentIndex];
            gRenderDebug->DebugThickRay(&p.mTransform[12],&b.mTransform[12],0.02f,0.05f, 0xFFFF00, 0xFF0000, 0.001f, true );
          }
          else
          {
            gRenderDebug->DebugDetailedSphere(&b.mTransform[12], 0.1f, 16, 0xFFFFFF, 0.001f, true, true );
          }
        }
      }
      if ( showCollision && mMeshSystem )
      {
        if ( mMeshSystem->mMeshCollisionCount )
        {
          for (HeU32 i=0; i<mMeshSystem->mMeshCollisionCount; i++)
          {
            debugRender( mMeshSystem->mMeshCollisionRepresentations[i],0 );
          }
        }
      }
    }
  }

  void debugRender(MESHIMPORT::MeshCollisionRepresentation *m,MESHIMPORT::MeshSkeletonInstance *skeleton)
  {
    for (HeU32 i=0; i<m->mCollisionCount; i++)
    {
      MESHIMPORT::MeshCollision *c = m->mCollisionGeometry[i];
      debugRender(c,skeleton);
    }
  }

  void debugRender(MESHIMPORT::MeshCollision *m,MESHIMPORT::MeshSkeletonInstance *skeleton)
  {
    HeU32 color = gRenderDebug->getDebugColor();

    const float *transform = m->mTransform;
    if ( skeleton )
    {
      for (int i=0; i<skeleton->mBoneCount; i++)
      {
        MESHIMPORT::MeshBoneInstance &b = mSkeleton->mBones[i];
        if ( strcmp(b.mBoneName,m->mName) == 0 )
        {
          transform = b.mAnimTransform;
          break;
        }
      }
    }

    switch ( m->getType() )
    {
      case MESHIMPORT::MCT_CONVEX:
        {
          MESHIMPORT::MeshCollisionConvex *c = static_cast< MESHIMPORT::MeshCollisionConvex *>(m);
          for (HeU32 i=0; i<c->mTriCount; i++)
          {
            HeU32 i1 = c->mIndices[i*3+0];
            HeU32 i2 = c->mIndices[i*3+1];
            HeU32 i3 = c->mIndices[i*3+2];

            const float *p1 = &c->mVertices[i1*3];
            const float *p2 = &c->mVertices[i2*3];
            const float *p3 = &c->mVertices[i3*3];

            float t1[3],t2[3],t3[3];
            fm_transform( transform, p1, t1 );
            fm_transform( transform, p2, t2 );
            fm_transform( transform, p3, t3 );
            gRenderDebug->DebugSolidTri(t1,t2,t3,color);
            gRenderDebug->DebugTri(t1,t2,t3,0xFFFFFF);
          }
        }
        break;

    }
  }

  void debugRender(MESHIMPORT::Mesh *m)
  {
    HeU32 color = gRenderDebug->getDebugColor();
    if ( mShowBounds )
      gRenderDebug->DebugBound(m->mAABB.mMin, m->mAABB.mMax, 0xFFFF00 );
    for (HeU32 i=0; i<m->mSubMeshCount; i++)
    {
      debugRender(m->mSubMeshes[i],color,m);
      color = gRenderDebug->getDebugColor();
    }
  }

  void debugRender(MESHIMPORT::Mesh *m,MESHIMPORT::MeshSkeletonInstance *skeleton)
  {
    int vcount = m->mVertexCount;
    MESHIMPORT::MeshVertex *vertices = MEMALLOC_NEW_ARRAY(MESHIMPORT::MeshVertex,vcount);
    gMeshImport->transformVertices(vcount,m->mVertices,vertices,skeleton);

    HeU32 color = gRenderDebug->getDebugColor(true);
    for (HeU32 i=0; i<m->mSubMeshCount; i++)
    {
      debugRender(m->mSubMeshes[i],color,vertices);
      color = gRenderDebug->getDebugColor();
    }

    MEMALLOC_DELETE_ARRAY(MESHIMPORT::MeshVertex,vertices);
  }

  void debugRender(MESHIMPORT::SubMesh *m,HeU32 color,MESHIMPORT::Mesh *pm)
  {
    if ( mShowBounds )
      gRenderDebug->DebugBound(m->mAABB.mMin, m->mAABB.mMax, color);

    PD3D::Pd3dGraphicsVertex *vertices = MEMALLOC_NEW_ARRAY(PD3D::Pd3dGraphicsVertex,pm->mVertexCount)[pm->mVertexCount];
    PD3D::Pd3dGraphicsVertex *dest = vertices;
    const MESHIMPORT::MeshVertex *src = pm->mVertices;
    for (HeU32 i=0; i<pm->mVertexCount; i++)
    {
      dest->mPos[0] = src->mPos[0];
      dest->mPos[1] = src->mPos[1];
      dest->mPos[2] = src->mPos[2];
      dest->mTexel[0] = src->mTexel1[0];
      dest->mTexel[1] = src->mTexel1[1];
      dest->mNormal[0] = src->mNormal[0];
      dest->mNormal[1] = src->mNormal[1];
      dest->mNormal[2] = src->mNormal[2];
      dest++;
      src++;
    }
    if ( mShowWireframe )
    {
      for (HeU32 i=0; i<m->mTriCount; i++)
      {
        HeU32 i1 = m->mIndices[i*3+0];
        HeU32 i2 = m->mIndices[i*3+1];
        HeU32 i3 = m->mIndices[i*3+2];
        assert( i1 >= 0 && i1 < pm->mVertexCount );
        assert( i2 >= 0 && i2 < pm->mVertexCount );
        assert( i3 >= 0 && i3 < pm->mVertexCount );
        const MESHIMPORT::MeshVertex &v1 = pm->mVertices[i1];
        const MESHIMPORT::MeshVertex &v2 = pm->mVertices[i2];
        const MESHIMPORT::MeshVertex &v3 = pm->mVertices[i3];
        gRenderDebug->DebugTri(v1.mPos,v2.mPos,v3.mPos);
      }
    }
    else
    {
      gPd3d->renderSection(&mMaterial,vertices,m->mIndices,pm->mVertexCount,m->mTriCount);
    }
    suppress_unused_variable_warning(vertices);

    MEMALLOC_DELETE_ARRAY(PD3D::Pd3dGraphicsVertex,vertices);

  }

  void debugSkeleton(const MESHIMPORT::MeshVertex &vtx)
  {
    if ( mSkeleton )
    {
      MESHIMPORT::MeshBoneInstance &b = mSkeleton->mBones[vtx.mBone[0]];
      gRenderDebug->DebugLine(vtx.mPos,&b.mTransform[12]);
    }

  }

  void debugRender(MESHIMPORT::SubMesh *m,HeU32 color,const MESHIMPORT::MeshVertex *vertices)
  {
    for (HeU32 i=0; i<m->mTriCount; i++)
    {
      HeU32 i1 = m->mIndices[i*3+0];
      HeU32 i2 = m->mIndices[i*3+1];
      HeU32 i3 = m->mIndices[i*3+2];

      const MESHIMPORT::MeshVertex &v1 = vertices[i1];
      const MESHIMPORT::MeshVertex &v2 = vertices[i2];
      const MESHIMPORT::MeshVertex &v3 = vertices[i3];

      if ( mShowWireframe )
        gRenderDebug->DebugTri(v1.mPos,v2.mPos,v3.mPos, color );
      else
      {
        if ( mFlipWinding )
          gRenderDebug->DebugSolidTri(v3.mPos,v2.mPos,v1.mPos, color );
        else
          gRenderDebug->DebugSolidTri(v1.mPos,v2.mPos,v3.mPos, color );
      }

    }
  }

  virtual bool importMesh(const char *fname)
  {
    bool ret = false;

    release();

    HeU32 len;
    unsigned char *data = getLocalFile(fname,len);
    if ( data )
    {
      mMeshSystemContainer = gMeshImport->createMeshSystemContainer(fname,data,len,0);
      if ( mMeshSystemContainer )
      {
        if ( stristr(fname,".gr2") )
          gMeshImport->scale(mMeshSystemContainer,10);
        mMeshSystem = gMeshImport->getMeshSystem(mMeshSystemContainer);
        SEND_TEXT_MESSAGE(0,"Successfully imported mesh '%s'\r\n", fname );

        if ( mMeshSystem->mSkeletonCount )
        {
          mSkeleton = gMeshImport->createMeshSkeletonInstance( *mMeshSystem->mSkeletons[0] );
        }

        ret = true;
      }
    }
    else
    {
      SEND_TEXT_MESSAGE(0,"Failed to load file '%s'\r\n", fname );
    }
    return ret;
  }

  virtual void advanceAnimation(float dtime,float rate)
  {
    mTime+=(dtime*rate);
    int track = (int)mTime;
    if ( mSkeleton )
    {
      gMeshImport->sampleAnimationTrack(track,mMeshSystem,mSkeleton);
    }
  }

  virtual const float * getCompositeTransforms(HeU32 &bone_count) 
  {
    const float *ret = 0;
    bone_count = 0;
    if ( mSkeleton )
    {
      mTransforms.clear();
      for (int i=0; i<mSkeleton->mBoneCount; i++)
      {
        MESHIMPORT::MeshBoneInstance &bi = mSkeleton->mBones[i];
        HeMat44 m;
        m.set(bi.mCompositeAnimTransform);
        mTransforms.push_back(m);
      }
      ret = (const float *)&mTransforms[0];
      bone_count = mSkeleton->mBoneCount;
    }
    return ret;
  }

  virtual bool exportEZM(void)
  {
    bool ret = false;
    if ( mMeshSystem )
    {
      char scratch[512];
      char export_name[512];

      strncpy(scratch, mMeshSystem->mAssetName, 512 );

      char *dot = (char *)lastDot(scratch);

      if ( dot )
      {
        if ( stricmp(dot,".xml") == 0 ) // if it is already an ezmesh...
        {
          dot = stristr(scratch,".mesh.xml");
          if ( dot == 0 )
          {
            dot = stristr(scratch,".skeleton.xml");
          }
        }
        if ( dot )
        {
          *dot = 0;
          sprintf(export_name,"%s_export.ezm", scratch );
          StringRef ref = mStrings.Get(export_name);
          SEND_TEXT_MESSAGE(0,"Serializing mesh '%s' to '%s'\r\n", mMeshSystem->mAssetName, export_name );
          mMeshSystem->mAssetName = ref.Get(); // the new asset name...
          MESHIMPORT::MeshSerialize ms(MESHIMPORT::MSF_EZMESH);
          bool ok = gMeshImport->serializeMeshSystem(mMeshSystem,ms);
          if ( ok )
          {
            FILE *fph = fopen(export_name, "wb");
            if ( fph )
            {
              fwrite(ms.mBaseData,ms.mBaseLen,1,fph);
              fclose(fph);
              ret = true;
            }
            else
            {
              SEND_TEXT_MESSAGE(0,"Failed to open file '%s' for write access.\r\n", export_name );
            }
            gMeshImport->releaseSerializeMemory(ms);
          }
          else
          {
            SEND_TEXT_MESSAGE(0,"Serialization failed.\r\n");
          }
        }
      }
    }

    return ret;
  }

  virtual bool exportOgre(void)
  {
    bool ret = false;

    if ( mMeshSystem )
    {
      char scratch[512];
      char export_name[512];
      char skeleton_name[512];
      char link_name[512];

      strncpy(scratch, mMeshSystem->mAssetName, 512 );

      char *dot = (char *)lastDot(scratch);

      if ( dot )
      {
        if ( stricmp(dot,".xml") == 0 ) // if it is already an ezmesh...
        {
          dot = stristr(scratch,".mesh.xml");
          if ( dot == 0 )
          {
            dot = stristr(scratch,".skeleton.xml");
          }
        }
        if ( dot )
        {
          *dot = 0;
          sprintf(export_name,"%s_export.mesh.xml", scratch );
          sprintf(skeleton_name,"%s_export.skeleton.xml", scratch );
          sprintf(link_name,"%s_export", scratch );

          StringRef ref = mStrings.Get(export_name);
          SEND_TEXT_MESSAGE(0,"Serializing mesh '%s' to '%s'\r\n", mMeshSystem->mAssetName, export_name );
          mMeshSystem->mAssetName = ref.Get(); // the new asset name...
          MESHIMPORT::MeshSerialize ms(MESHIMPORT::MSF_OGRE3D);
          ms.mSaveFileName = export_name;

          bool ok = gMeshImport->serializeMeshSystem(mMeshSystem,ms);
          if ( ok )
          {
            FILE *fph = fopen(export_name, "wb");
            if ( fph )
            {
              fwrite(ms.mBaseData,ms.mBaseLen,1,fph);
              fclose(fph);
              ret = true;
            }
            else
            {
              SEND_TEXT_MESSAGE(0,"Failed to open file '%s' for write access.\r\n", export_name );
            }

            if ( ms.mExtendedData )
            {
              FILE *fph = fopen(skeleton_name, "wb");
              if ( fph )
              {
                fwrite(ms.mExtendedData,ms.mExtendedLen,1,fph);
                fclose(fph);
                SEND_TEXT_MESSAGE(0,"Saving skeleton data to '%s'\r\n", skeleton_name );
              }
              else
              {
                SEND_TEXT_MESSAGE(0,"Failed to open file '%s' for write access.\r\n", skeleton_name );
              }
            }
            gMeshImport->releaseSerializeMemory(ms);
          }
          else
          {
            SEND_TEXT_MESSAGE(0,"Serialization failed.\r\n");
          }
        }
      }
    }

    return ret;
  }

  virtual bool exportObj(void)
  {
    bool ret = false;
    SEND_TEXT_MESSAGE(0,"Not implemeented yet.\r\n");
    return ret;
  }

  MESHIMPORT::MeshSystemContainer * getMeshSystemContainer(void) { return mMeshSystemContainer; };

  virtual MeshSystemRaw * getMeshSystemRaw(void)
  {
    MeshSystemRaw *ret = 0;
    if ( mMeshSystem )
    {
      ret = MEMALLOC_NEW(MeshSystemRaw);
      typedef USER_STL::vector< HeU32 > HeU32Vector;
      HeU32Vector indices;
      fm_VertexIndex *vi = fm_createVertexIndex(0.0001f,false);
      for (HeU32 i=0; i<mMeshSystem->mMeshCount; i++)
      {
        MESHIMPORT::Mesh *m = mMeshSystem->mMeshes[i];
        for (HeU32 j=0; j<m->mSubMeshCount; j++)
        {
          MESHIMPORT::SubMesh *sm = m->mSubMeshes[j];
          for (HeU32 k=0; k<sm->mTriCount; k++)
          {
            HeU32 i1 = sm->mIndices[k*3+0];
            HeU32 i2 = sm->mIndices[k*3+1];
            HeU32 i3 = sm->mIndices[k*3+2];
            assert( i1 >= 0 && i1 < m->mVertexCount );
            assert( i2 >= 0 && i2 < m->mVertexCount );
            assert( i3 >= 0 && i3 < m->mVertexCount );
            const MESHIMPORT::MeshVertex &v1 = m->mVertices[i1];
            const MESHIMPORT::MeshVertex &v2 = m->mVertices[i2];
            const MESHIMPORT::MeshVertex &v3 = m->mVertices[i3];
            bool newPos;

            i1 = vi->getIndex(v1.mPos,newPos);
            i2 = vi->getIndex(v2.mPos,newPos);
            i3 = vi->getIndex(v3.mPos,newPos);

            indices.push_back(i1);
            indices.push_back(i2);
            indices.push_back(i3);
          }
        }
      }
      ret->mVcount = vi->getVcount();
      if ( ret->mVcount )
      {
        ret->mTcount = indices.size()/3;
        ret->mIndices = (HeU32 *)MEMALLOC_MALLOC(sizeof(HeU32)*ret->mTcount*3);
        memcpy(ret->mIndices,&indices[0],sizeof(HeU32)*ret->mTcount*3);
        ret->mVertices = (HeF32 *)MEMALLOC_MALLOC(sizeof(HeF32)*ret->mVcount*3);
        memcpy(ret->mVertices, vi->getVerticesFloat(), sizeof(HeF32)*ret->mVcount*3);
      }
      fm_releaseVertexIndex(vi);
    }
    return ret;
  }

  virtual void            releaseMeshSystemRaw(MeshSystemRaw *mr)
  {
    if ( mr )
    {
      MEMALLOC_FREE(mr->mVertices);
      MEMALLOC_FREE(mr->mIndices);
      MEMALLOC_DELETE(MeshSystemRaw,mr);
    }
  }

private:
  float mTime;
  bool mShowWireframe;
  bool mFlipWinding;
  bool mShowBounds;
  MESHIMPORT::MeshSkeletonInstance *mSkeleton;
  MESHIMPORT::MeshSystemContainer *mMeshSystemContainer;
  MESHIMPORT::MeshSystem  *mMeshSystem;
  StringDict mStrings;
  HeMat44Vector mTransforms;
  PD3D::Pd3dMaterial mMaterial;
};

MeshSystemHelper * createMeshSystemHelper(void)
{
  MyMeshSystemHelper *h = MEMALLOC_NEW(MyMeshSystemHelper);
  return static_cast< MeshSystemHelper *>(h);
}


void   releaseMeshSystemHelper(MeshSystemHelper *m)
{
  MyMeshSystemHelper *s = static_cast< MyMeshSystemHelper *>(m);
  MEMALLOC_DELETE(MyMeshSystemHelper,s);
}


