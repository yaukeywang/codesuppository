#include <d3d9.h>
#include <d3dx9.h>
#include <d3d9types.h>
#include <ddraw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ApexRenderInterface.h"
#include "common/snippets/UserMemAlloc.h"
#include "common/snippets/FloatMath.h"
#include "ClientPhysics/ClientPhysics.h"
#include "RenderDebug/RenderDebug.h"
#include "Pd3d/Pd3d.h"
#include "MeshImport/MeshImport.h"

#pragma warning(disable:4100)

extern LPDIRECT3DDEVICE9 GlobalD3DDevice;
CLIENT_PHYSICS::ApexRenderInterface *gApexRenderInterface=0;

#define SAFE_RELEASE(x) if ( x ) { x->Release(); x = 0; }

namespace CLIENT_PHYSICS
{

class MyApexMaterial : public ApexMaterial
{
public:
  MyApexMaterial(const char *matName)
  {
  }

  virtual void release(void)
  {
    MEMALLOC_DELETE(MyApexMaterial,this);
  }

};


class MyApexVertexBuffer : public ApexVertexBuffer
{
public:
  MyApexVertexBuffer(HeU32 vertexFormat,HeU32 vertexSize,HeU32 vcount,const unsigned char *vertices)
  {
    mVertexFormat = vertexFormat;
    mStride = vertexSize;
    mVcount = vcount;
    GlobalD3DDevice->CreateVertexBuffer(vcount*mStride,D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED,&mVertices, NULL );
    
    if ( vertices )
    {
      unsigned char *av = (unsigned char *)lock(0,vcount);
      if ( av )
      {
        memcpy(av,vertices,mStride*vcount);
        unlock();
      }
      
    }
  }

  ~MyApexVertexBuffer(void)
  {
    SAFE_RELEASE(mVertices);
  }


  virtual void *lock(HeU32 firstElement,HeU32 numElements)
  {
    void * ret = 0;
    if ( mVertices )
    {
      mVertices->Lock(firstElement*mStride,numElements*mStride,&ret,0);
    }
    return ret;
  }

  virtual void  unlock(void)
  {
    if ( mVertices )
    {
      mVertices->Unlock();
    }
  }

  virtual void  release(void)
  {
    MEMALLOC_DELETE(MyApexVertexBuffer,this);
  }

  virtual HeU32 getVertexFormat(void) const { return mVertexFormat; };
  virtual HeU32 getVertexCount(void) const { return mVcount; };
  virtual HeU32 getVertexStride(void) const { return mStride; };

  HeU32                    mVertexFormat;
  HeU32                    mStride;
  HeU32                    mVcount;
  LPDIRECT3DVERTEXBUFFER9  mVertices;
  
};

class MyApexIndexBuffer : public ApexIndexBuffer
{
public:
  MyApexIndexBuffer(HeU32 icount,const HeU32 *indices)
  {
    mStride = sizeof(HeU32);
    mIcount = icount;
    mIndices = 0;
    GlobalD3DDevice->CreateIndexBuffer(icount*sizeof(HeU32), D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED, &mIndices, 0 );
    HeU32 *dest = (HeU32 *)lock(0,mIcount);
    if ( dest )
    {
      if ( indices )
      {
        memcpy(dest,indices,mStride*mIcount);
      }
      else
      {
        memset(dest,0,mStride*mIcount);
      }
      unlock();
    }
  }

  MyApexIndexBuffer(HeU32 icount,const HeU16 *indices)
  {
    mStride = sizeof(HeU16);
    mIcount = icount;
    mIndices = 0;
    GlobalD3DDevice->CreateIndexBuffer(icount*sizeof(HeU16), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &mIndices, 0 );
    HeU16 *dest = (HeU16 *)lock(0,mIcount);
    if ( dest )
    {
      if ( indices )
      {
        memcpy(dest,indices,mStride*mIcount);
      }
      else
      {
        memset(dest,0,mStride*mIcount);
      }
      unlock();
    }
  }

  ~MyApexIndexBuffer(void)
  {
    SAFE_RELEASE(mIndices);
  }

  virtual void release(void)
  {
    MEMALLOC_DELETE(MyApexIndexBuffer,this);
  }

  virtual void *lock(HeU32 firstElement,HeU32 numElements)
  {
    void *ret = 0;
    if ( mIndices )
    {
      mIndices->Lock(firstElement*mStride,numElements*mStride,&ret,0);
    }
    return ret;
  }

  virtual void  unlock(void)
  {
    if ( mIndices )
    {
      mIndices->Unlock();
    }
  }

  HeU32 mStride;
  HeU32 mIcount;
  LPDIRECT3DINDEXBUFFER9 mIndices;
};

class MyApexRenderInterface : public CLIENT_PHYSICS::ApexRenderInterface
{
public:
  MyApexRenderInterface(void)
  {
    strcpy(mMaterial.mTexture,"triumvirate_guard.dds");
    mVertexDeclaration = 0;
    D3DVERTEXELEMENT9 decl[] = {
      { 0,  0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
      { 0,  8, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1 }, // Bone weights (color index 1)
      { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 2 }, // Bone indices (color index 2)
      { 1,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,  0 },
      { 1, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,    0 },
      D3DDECL_END()
    };
    if ( GlobalD3DDevice )
      GlobalD3DDevice->CreateVertexDeclaration(decl,&mVertexDeclaration);
  }

  ~MyApexRenderInterface(void)
  {
    SAFE_RELEASE(mVertexDeclaration);
  }

  virtual void apexProcess(NxScene *scene,float dtime)
  {

  }

  virtual void syncClothPhysics(NxScene *scene,CLIENT_PHYSICS::ApexCloth *c)
  {

  }

  virtual ApexMaterial     * createApexMaterial(const char *matName,void *userData)
  {
    MyApexMaterial *m = MEMALLOC_NEW(MyApexMaterial)(matName);
    return static_cast< ApexMaterial *>(m);
  }

  virtual ApexVertexBuffer * createApexVertexBuffer(HeU32 vertexFormat,HeU32 vertexSize,HeU32 vcount,const unsigned char *vertices)
  {
    MyApexVertexBuffer *m = MEMALLOC_NEW(MyApexVertexBuffer)(vertexFormat,vertexSize,vcount,vertices);
    return static_cast< ApexVertexBuffer *>(m);
  }

  virtual ApexIndexBuffer  * createApexIndexBuffer(HeU32 icount,const HeU32 *indices)
  {
    MyApexIndexBuffer *m = MEMALLOC_NEW(MyApexIndexBuffer)(icount,indices);
    return static_cast< ApexIndexBuffer *>(m);
  }

  virtual ApexIndexBuffer  * createApexIndexBuffer(HeU32 icount,const HeU16 *indices)
  {
    MyApexIndexBuffer *m = MEMALLOC_NEW(MyApexIndexBuffer)(icount,indices);
    return static_cast< ApexIndexBuffer *>(m);
  }

  virtual void               apexRender(const ApexRenderDesc &desc)
  {

    PD3D::Pd3dGraphicsVertex *vertices = MEMALLOC_NEW_ARRAY(PD3D::Pd3dGraphicsVertex, desc.mNumVerts)[desc.mNumVerts];

    for (HeU32 i=0; i<desc.mVertexBufferCount; i++)
    {
      MyApexVertexBuffer *vb = static_cast< MyApexVertexBuffer *>(desc.mVertexBuffers[i]);
      const unsigned char *src = (const unsigned char *)vb->lock(desc.mFirstVertex,desc.mNumVerts);

      PD3D::Pd3dGraphicsVertex *dest = vertices;
      HeU32 stride = vb->getVertexStride();
      HeU32 format = vb->getVertexFormat();
      suppress_unused_variable_warning(stride);
      suppress_unused_variable_warning(format);

      for (unsigned int i=0; i<desc.mNumVerts; i++)
      {
        const unsigned char *start = src;
        if ( format & VFF_POSITION )
        {
          const float *s = (const float *)src;
          dest->mPos[0] = s[0];
          dest->mPos[1] = s[1];
          dest->mPos[2] = s[2];
          src+=sizeof(float)*3;
        }
        if ( format & VFF_NORMAL )
        {
          const float *s = (const float *)src;
          dest->mNormal[0] = s[0];
          dest->mNormal[1] = s[1];
          dest->mNormal[2] = s[2];
          src+=sizeof(float)*3;
        }
        if ( format & VFF_TANGENT )
        {
          src+=sizeof(float)*3;
        }
        if ( format & VFF_BINORMAL )
        {
          src+=sizeof(float)*3;
        }
        if ( format & VFF_TEXCOORD0 )
        {
          const float *s = (const float *)src;
          dest->mTexel[0] = s[0];
          dest->mTexel[1] = s[1];
          src+=sizeof(float)*2;
        }
        if ( format & VFF_COLOR )
        {
          src+=4;
        }
        if ( format & VFF_TEXCOORD1 )
        {
          src+=sizeof(float)*2;
        }
        if ( format & VFF_TEXCOORD2 )
        {
          src+=sizeof(float)*2;
        }
        if ( format & VFF_TEXCOORD3 )
        {
          src+=sizeof(float)*2;
        }
        if ( format & VFF_BONE_INDEX )
        {
          src+=sizeof(HeU8)*4;
        }
        if ( format & VFF_BONE_WEIGHT )
        {
          src+=sizeof(HeU8)*4;
        }

        HeU32 diff = src-start;
        assert( diff == stride );
        suppress_unused_variable_warning(diff);

        dest++;
      }
      vb->unlock();
    }

    MyApexIndexBuffer  *ib = static_cast< MyApexIndexBuffer *>(desc.mIndexBuffer);
    const HeU32 *isrc = (const HeU32 *)ib->lock(desc.mFirstIndex,desc.mNumIndices);
    HeU32 *indices = MEMALLOC_NEW_ARRAY(HeU32,desc.mNumIndices)[desc.mNumIndices];
    HeU32 tcount = desc.mNumIndices/3;
    memcpy(indices,isrc,sizeof(HeU32)*desc.mNumIndices);

#if 0
    for (HeU32 i=0; i<tcount; i++)
    {
      unsigned int i1 = indices[i*3+0];
      unsigned int i2 = indices[i*3+1];
      unsigned int i3 = indices[i*3+2];
      PD3D::Pd3dGraphicsVertex &v1 = vertices[i1];
      PD3D::Pd3dGraphicsVertex &v2 = vertices[i2];
      PD3D::Pd3dGraphicsVertex &v3 = vertices[i3];
      gRenderDebug->DebugTri(v1.mPos,v2.mPos,v3.mPos);
    }
#endif
    gPd3d->renderSection(&mMaterial,vertices,indices,desc.mNumVerts,tcount);


    ib->unlock();

    MEMALLOC_DELETE_ARRAY(HeU32,indices);
    MEMALLOC_DELETE_ARRAY(PD3D::Pd3dGraphicsVertex,vertices);


  }

  PD3D::Pd3dMaterial mMaterial;
  LPDIRECT3DVERTEXDECLARATION9 mVertexDeclaration;
};


}; // end of namespace

CLIENT_PHYSICS::ApexRenderInterface * createApexRenderInterface(void)
{
  CLIENT_PHYSICS::MyApexRenderInterface *m = MEMALLOC_NEW(CLIENT_PHYSICS::MyApexRenderInterface);
  return static_cast< CLIENT_PHYSICS::ApexRenderInterface *>(m);
}

