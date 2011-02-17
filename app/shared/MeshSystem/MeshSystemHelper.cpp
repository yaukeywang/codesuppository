#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "MeshImport.h"
#include "MeshSystemHelper.h"
#include "UserMemAlloc.h"
#include "fmem.h"
#include "sutil.h"
#include "RenderDebug.h"
#include "stringdict.h"
#include "FloatMath.h"
#include "NxFoundation.h"
#include "Pd3d/pd3d.h"
#include "NxMat44.h"
#include <vector>

using namespace NVSHARE;

#pragma warning(disable:4100 4189)

typedef NVSHARE::Array< NVSHARE::Pd3dMaterial >	MaterialVector;

class MyMeshSystemHelper : public MeshSystemHelper, public NVSHARE::Memalloc
{
public:

  typedef std::vector< NxMat44 > HeMat44Vector;

  MyMeshSystemHelper(void)
  {
    mSelectCollision = -1;
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

	mMaterials.clear();
  }

  virtual NVSHARE::MeshSystem * getMeshSystem(void) const
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
        for (NxU32 i=0; i<mMeshSystem->mMeshCount; i++)
        {
          debugRender( mMeshSystem->mMeshes[i], mSkeleton );
        }
      }
      if ( mSkeleton && showSkeleton )
      {
        for (NxI32 i=0; i<mSkeleton->mBoneCount; i++)
        {
          NVSHARE::MeshBoneInstance &b = mSkeleton->mBones[i];
          if ( b.mParentIndex != -1 )
          {
//            NVSHARE::MeshBoneInstance &p = mSkeleton->mBones[b.mParentIndex];
//            gRenderDebug->DebugThickRay(&p.mAnimTransform[12],&b.mAnimTransform[12],0.02f,0.05f, 0xFFFF00, 0xFF0000, 0.001f, true );
          }
          else
          {
//            gRenderDebug->DebugDetailedSphere(&b.mAnimTransform[12], 0.1f, 16, 0xFFFFFF, 0.001f, true, true );
          }
        }
      }
      if ( showCollision )
      {
        if ( mMeshSystem->mMeshCollisionCount )
        {
          for (NxU32 i=0; i<mMeshSystem->mMeshCollisionCount; i++)
          {
            debugRender( mMeshSystem->mMeshCollisionRepresentations[i], mSkeleton, mSelectCollision );
          }
        }
      }
    }
    else
    {
      if ( mMeshSystem && showMesh )
      {
//        if ( mShowBounds )
//          gRenderDebug->DebugBound(mMeshSystem->mAABB.mMin, mMeshSystem->mAABB.mMax, 0xFFFFFF );
        for (NxU32 i=0; i<mMeshSystem->mMeshCount; i++)
        {
          debugRender( mMeshSystem->mMeshes[i] );
        }
      }
      if ( mSkeleton && showSkeleton )
      {
        for (NxI32 i=0; i<mSkeleton->mBoneCount; i++)
        {
          NVSHARE::MeshBoneInstance &b = mSkeleton->mBones[i];
          if ( b.mParentIndex != -1 )
          {
//            NVSHARE::MeshBoneInstance &p = mSkeleton->mBones[b.mParentIndex];
//            gRenderDebug->DebugThickRay(&p.mTransform[12],&b.mTransform[12],0.02f,0.05f, 0xFFFF00, 0xFF0000, 0.001f, true );
          }
          else
          {
//            gRenderDebug->DebugDetailedSphere(&b.mTransform[12], 0.1f, 16, 0xFFFFFF, 0.001f, true, true );
          }
        }
      }
      if ( showCollision && mMeshSystem )
      {
        if ( mMeshSystem->mMeshCollisionCount )
        {
          for (NxU32 i=0; i<mMeshSystem->mMeshCollisionCount; i++)
          {
            debugRender( mMeshSystem->mMeshCollisionRepresentations[i],0, mSelectCollision );
          }
        }
      }
    }
  }

  virtual void setSelectCollision(NxI32 sc)
  {
    if ( sc != mSelectCollision )
    {
      mSelectCollision = sc;
      if ( mMeshSystem->mMeshCollisionCount )
      {
        if ( mSelectCollision >= 0 )
        {
          NVSHARE::MeshCollisionRepresentation *cr = mMeshSystem->mMeshCollisionRepresentations[0];
          if ( mSelectCollision < (NxI32)cr->mCollisionCount )
          {
            NVSHARE::MeshCollision *c = cr->mCollisionGeometry[mSelectCollision];
            SEND_TEXT_MESSAGE(0,"Selected collision %s\r\n", c->mName );
          }
        }
      }
    }
  }

  void debugRender(NVSHARE::MeshCollisionRepresentation *m,NVSHARE::MeshSkeletonInstance *skeleton,NxI32 selectCollision)
  {
    if ( selectCollision >= 0 )
    {
      if ( selectCollision < (NxI32)m->mCollisionCount )
      {
        NVSHARE::MeshCollision *c = m->mCollisionGeometry[selectCollision];
        debugRender(c,skeleton);
      }
    }
    else
    {
      for (NxU32 i=0; i<m->mCollisionCount; i++)
      {
        NVSHARE::MeshCollision *c = m->mCollisionGeometry[i];
        debugRender(c,skeleton);
      }
    }
  }

  void debugRender(NVSHARE::MeshCollision *m,NVSHARE::MeshSkeletonInstance *skeleton)
  {
    NxU32 color = gRenderDebug->getDebugColor();

    NxF32 combined[16];

    const NxF32 *transform = m->mTransform;
    if ( skeleton )
    {
      for (NxI32 i=0; i<skeleton->mBoneCount; i++)
      {
        NVSHARE::MeshBoneInstance &b = mSkeleton->mBones[i];
        if ( strcmp(b.mBoneName,m->mName) == 0 )
        {
          fm_multiplyTransform(m->mTransform,b.mAnimTransform,combined);
          transform = combined;
          break;
        }
      }
    }

    switch ( m->getType() )
    {
      case NVSHARE::MCT_CAPSULE:
        {
//          NVSHARE::MeshCollisionCapsule *c = static_cast< NVSHARE::MeshCollisionCapsule *>(m);
//          gRenderDebug->DebugOrientedCapsule(c->mRadius, c->mHeight, transform, color );
        }
        break;
      case NVSHARE::MCT_SPHERE:
        {
//          NVSHARE::MeshCollisionSphere *c = static_cast< NVSHARE::MeshCollisionSphere *>(m);
//          gRenderDebug->DebugSphere(&transform[12],c->mRadius, color );
        }
        break;
      case NVSHARE::MCT_BOX:
        {
//          NVSHARE::MeshCollisionBox *c = static_cast< NVSHARE::MeshCollisionBox *>(m);
//          gRenderDebug->DebugOrientedBound(c->mSides,transform,color);
        }
        break;

      case NVSHARE::MCT_CONVEX:
        {
          NVSHARE::MeshCollisionConvex *c = static_cast< NVSHARE::MeshCollisionConvex *>(m);
          for (NxU32 i=0; i<c->mTriCount; i++)
          {
            NxU32 i1 = c->mIndices[i*3+0];
            NxU32 i2 = c->mIndices[i*3+1];
            NxU32 i3 = c->mIndices[i*3+2];

            const NxF32 *p1 = &c->mVertices[i1*3];
            const NxF32 *p2 = &c->mVertices[i2*3];
            const NxF32 *p3 = &c->mVertices[i3*3];

            NxF32 t1[3],t2[3],t3[3];
            fm_transform( transform, p1, t1 );
            fm_transform( transform, p2, t2 );
            fm_transform( transform, p3, t3 );
//            gRenderDebug->DebugSolidTri(t1,t2,t3,color);
//            gRenderDebug->DebugTri(t1,t2,t3,0xFFFFFF);
          }
        }
        break;

    }
  }

  void debugRender(NVSHARE::Mesh *m)
  {
    NxU32 color = gRenderDebug->getDebugColor();
//    if ( mShowBounds )
//      gRenderDebug->DebugBound(m->mAABB.mMin, m->mAABB.mMax, 0xFFFF00 );
    for (NxU32 i=0; i<m->mSubMeshCount; i++)
    {
      debugRender(m->mSubMeshes[i],color,m);
      color = gRenderDebug->getDebugColor();
    }
  }

  void debugRender(NVSHARE::Mesh *m,NVSHARE::MeshSkeletonInstance *skeleton)
  {
    NxI32 vcount = m->mVertexCount;
    NVSHARE::MeshVertex *vertices = MEMALLOC_NEW(NVSHARE::MeshVertex)[vcount];
    gMeshImport->transformVertices(vcount,m->mVertices,vertices,skeleton);

    NxU32 color = gRenderDebug->getDebugColor(true);
    for (NxU32 i=0; i<m->mSubMeshCount; i++)
    {
      debugRender(m->mSubMeshes[i],color,vertices);
      color = gRenderDebug->getDebugColor();
    }

    delete []vertices;
  }

  void debugRender(NVSHARE::SubMesh *m,NxU32 color,NVSHARE::Mesh *pm)
  {
//    if ( mShowBounds )
//      gRenderDebug->DebugBound(m->mAABB.mMin, m->mAABB.mMax, color);

    NVSHARE::Pd3dGraphicsVertex *vertices = MEMALLOC_NEW(NVSHARE::Pd3dGraphicsVertex)[pm->mVertexCount];
    NVSHARE::Pd3dGraphicsVertex *dest = vertices;
    const NVSHARE::MeshVertex *src = pm->mVertices;
    for (NxU32 i=0; i<pm->mVertexCount; i++)
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
      for (NxU32 i=0; i<m->mTriCount; i++)
      {
        NxU32 i1 = m->mIndices[i*3+0];
        NxU32 i2 = m->mIndices[i*3+1];
        NxU32 i3 = m->mIndices[i*3+2];
        assert( i1 >= 0 && i1 < pm->mVertexCount );
        assert( i2 >= 0 && i2 < pm->mVertexCount );
        assert( i3 >= 0 && i3 < pm->mVertexCount );
        const NVSHARE::MeshVertex &v1 = pm->mVertices[i1];
        const NVSHARE::MeshVertex &v2 = pm->mVertices[i2];
        const NVSHARE::MeshVertex &v3 = pm->mVertices[i3];
        gRenderDebug->DebugTri(v1.mPos,v2.mPos,v3.mPos);
      }
    }
    else
	{
		NVSHARE::Pd3dMaterial* mat = &mMaterial;
		NxI32 idx = getMatIdx( m->mMaterialName );
		if ( idx >= 0 )
			mat = &mMaterials[ idx ];
		gPd3d->renderSection(mat,vertices,m->mIndices,pm->mVertexCount,m->mTriCount);
	}
	vertices;

    delete []vertices;

  }

  void debugSkeleton(const NVSHARE::MeshVertex &vtx)
  {
    if ( mSkeleton )
    {
      NVSHARE::MeshBoneInstance &b = mSkeleton->mBones[vtx.mBone[0]];
      gRenderDebug->DebugLine(vtx.mPos,&b.mTransform[12]);
    }

  }

  void debugRender(NVSHARE::SubMesh *m,NxU32 color,const NVSHARE::MeshVertex *vertices)
  {
	  gRenderDebug->setCurrentColor(color,0xFFFFFF);
	  if ( mShowWireframe )
	  {
		  gRenderDebug->removeFromCurrentState(NVSHARE::DebugRenderState::SolidShaded);
		  gRenderDebug->removeFromCurrentState(NVSHARE::DebugRenderState::SolidWireShaded);
	  }
	  else
	  {
		  gRenderDebug->addToCurrentState(NVSHARE::DebugRenderState::SolidWireShaded);
	  }

    for (NxU32 i=0; i<m->mTriCount; i++)
    {
      NxU32 i1 = m->mIndices[i*3+0];
      NxU32 i2 = m->mIndices[i*3+1];
      NxU32 i3 = m->mIndices[i*3+2];

      const NVSHARE::MeshVertex &v1 = vertices[i1];
      const NVSHARE::MeshVertex &v2 = vertices[i2];
      const NVSHARE::MeshVertex &v3 = vertices[i3];

      gRenderDebug->DebugTri(v1.mPos,v2.mPos,v3.mPos);
    }
  }

  virtual bool importMesh(const char *fname)
  {
    bool ret = false;

    release();

    NxU32 len;
    NxU8 *data = getLocalFile(fname,len);
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

		mMaterials.clear();
		for ( NxU32 i = 0; i < mMeshSystem->mMaterialCount; ++i )
		{
			const MeshMaterial& meshMaterial = mMeshSystem->mMaterials[i];
			if ( strlen( meshMaterial.mName ) > 0 )
			{
				NVSHARE::Pd3dMaterial d3dMat( mMaterial );
				strcpy( d3dMat.mTexture, meshMaterial.mName );
				mMaterials.pushBack( d3dMat );
			}
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

  virtual void advanceAnimation(NxF32 dtime,NxF32 rate)
  {
    mTime+=(dtime*rate);
    NxI32 track = (NxI32)mTime;
    if ( mSkeleton )
    {
      gMeshImport->sampleAnimationTrack(track,mMeshSystem,mSkeleton);
    }
  }

  virtual const NxF32 * getCompositeTransforms(NxU32 &bone_count) 
  {
    const NxF32 *ret = 0;
    bone_count = 0;
    if ( mSkeleton )
    {
      mTransforms.clear();
      for (NxI32 i=0; i<mSkeleton->mBoneCount; i++)
      {
        NVSHARE::MeshBoneInstance &bi = mSkeleton->mBones[i];
        NxMat44 m;
        m.set(bi.mCompositeAnimTransform);
        mTransforms.push_back(m);
      }
      ret = (const NxF32 *)&mTransforms[0];
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
          NVSHARE::MeshSerialize ms(NVSHARE::MSF_EZMESH);
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
          NVSHARE::MeshSerialize ms(NVSHARE::MSF_OGRE3D);
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

    if ( mMeshSystem )
    {
      char scratch[512];
      char export_name[512];
      char material_name[512];

      strncpy(scratch, mMeshSystem->mAssetName, 512 );

      char *dot = (char *)lastDot(scratch);

      if ( dot )
      {
        *dot = 0;
        sprintf(export_name,"%s_export.obj", scratch );
        sprintf(material_name,"%s_export.mtl", scratch );

        StringRef ref = mStrings.Get(export_name);
        SEND_TEXT_MESSAGE(0,"Serializing mesh '%s' to '%s'\r\n", mMeshSystem->mAssetName, export_name );
        mMeshSystem->mAssetName = ref.Get(); // the new asset name...
        NVSHARE::MeshSerialize ms(NVSHARE::MSF_WAVEFRONT);
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
            FILE *fph = fopen(material_name, "wb");
            if ( fph )
            {
              fwrite(ms.mExtendedData,ms.mExtendedLen,1,fph);
              fclose(fph);
              SEND_TEXT_MESSAGE(0,"Saving material data to '%s'\r\n", material_name );
            }
            else
            {
              SEND_TEXT_MESSAGE(0,"Failed to open file '%s' for write access.\r\n", material_name );
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

    return ret;
  }

  NVSHARE::MeshSystemContainer * getMeshSystemContainer(void) { return mMeshSystemContainer; };

  virtual MeshSystemRaw * getMeshSystemRaw(void)
  {
    MeshSystemRaw *ret = 0;
    if ( mMeshSystem )
    {
      ret = MEMALLOC_NEW(MeshSystemRaw);
      typedef std::vector< NxU32 > HeU32Vector;
      HeU32Vector indices;
      fm_VertexIndex *vi = fm_createVertexIndex(0.0001f,false);
      for (NxU32 i=0; i<mMeshSystem->mMeshCount; i++)
      {
        NVSHARE::Mesh *m = mMeshSystem->mMeshes[i];
        for (NxU32 j=0; j<m->mSubMeshCount; j++)
        {
          NVSHARE::SubMesh *sm = m->mSubMeshes[j];
          for (NxU32 k=0; k<sm->mTriCount; k++)
          {
            NxU32 i1 = sm->mIndices[k*3+0];
            NxU32 i2 = sm->mIndices[k*3+1];
            NxU32 i3 = sm->mIndices[k*3+2];
            assert( i1 >= 0 && i1 < m->mVertexCount );
            assert( i2 >= 0 && i2 < m->mVertexCount );
            assert( i3 >= 0 && i3 < m->mVertexCount );
            const NVSHARE::MeshVertex &v1 = m->mVertices[i1];
            const NVSHARE::MeshVertex &v2 = m->mVertices[i2];
            const NVSHARE::MeshVertex &v3 = m->mVertices[i3];
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
        ret->mIndices = (NxU32 *)MEMALLOC_MALLOC(sizeof(NxU32)*ret->mTcount*3);
        memcpy(ret->mIndices,&indices[0],sizeof(NxU32)*ret->mTcount*3);
        ret->mVertices = (NxF32 *)MEMALLOC_MALLOC(sizeof(NxF32)*ret->mVcount*3);
        memcpy(ret->mVertices, vi->getVerticesFloat(), sizeof(NxF32)*ret->mVcount*3);
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
      delete mr;
    }
  }

  NxI32 getMatIdx(const char* matName)
  {
	  for ( NxU32 i = 0; i < mMeshSystem->mMaterialCount; ++i )
	  {
		  if ( matName == mMeshSystem->mMaterials[i].mName )
			  return i;
	  }

	  return -1;
  }

  NxU32 getMaterialCount()
  {
	  return mMaterials.size();
  }

  NVSHARE::Pd3dMaterial* getMaterial(NxU32 idx)
  {
	  if ( idx < 0 || idx >= mMaterials.size() )
		  return &mMaterial;
	  return &mMaterials[idx];
  }

private:
  NxF32 mTime;
  bool mShowWireframe;
  bool mFlipWinding;
  bool mShowBounds;
  NVSHARE::MeshSkeletonInstance *mSkeleton;
  NVSHARE::MeshSystemContainer *mMeshSystemContainer;
  NVSHARE::MeshSystem  *mMeshSystem;
  StringDict mStrings;
  HeMat44Vector mTransforms;
  NVSHARE::Pd3dMaterial mMaterial;
  MaterialVector mMaterials;
  NxI32 mSelectCollision;
};

MeshSystemHelper * createMeshSystemHelper(void)
{
  MyMeshSystemHelper *h = MEMALLOC_NEW(MyMeshSystemHelper);
  return static_cast< MeshSystemHelper *>(h);
}


void   releaseMeshSystemHelper(MeshSystemHelper *m)
{
  MyMeshSystemHelper *s = static_cast< MyMeshSystemHelper *>(m);
  delete s;
}


