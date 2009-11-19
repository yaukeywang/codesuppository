#pragma warning(disable:4702)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <vector>

#include "terrain.h"
#include "common/rtin/rtinobj.h"
#include "RenderDebug.h"
#include "Pd3d/pd3d.h"
#include "common/snippets/floatmath.h"
#include "common/snippets/log.h"

using namespace NVSHARE;

const NxF32 YSCALE = 1.0f / 10.0f;
const NxF32 XZSCALE = 400.0f;

static bool gShowNormals=false;
static NxF32 gRotationSpeed=1;

class TerrainMesh
{
public:

  TerrainMesh(NxU32 vcount,const GeometryVertex *vertices,NxU32 icount,const NxU16 *indices,NVSHARE::Pd3dTexture *texture,NxF32 meshScale)
  {
    mMaterial.mHandle = texture;
    mTcount           = icount/3;
    mVcount           = vcount;
    mIndexBuffer = gPd3d->createIndexBuffer(icount,indices);

    mVertices = MEMALLOC_NEW(NVSHARE::Pd3dGraphicsVertex)[vcount];

    NVSHARE::Pd3dGraphicsVertex *dest = mVertices;

    for (NxU32 i=0; i<vcount; i++)
    {
      dest->mPos[0] = vertices->mPos[0]*meshScale;
      dest->mPos[1] = vertices->mPos[1];
      dest->mPos[2] = vertices->mPos[2]*meshScale;

      dest->mNormal[0] = vertices->mNormal[0];
      dest->mNormal[1] = vertices->mNormal[1]*2.0f;
      dest->mNormal[2] = vertices->mNormal[2];

      dest->mTexel[0] = vertices->mTexel2[0];
      dest->mTexel[1] = vertices->mTexel2[1];

      dest++;
      vertices++;

    }

    mVertexBuffer = gPd3d->createVertexBuffer(vcount,mVertices);

  }

  ~TerrainMesh(void)
  {
    delete []mVertices;
    gPd3d->releaseVertexBuffer(mVertexBuffer);
    gPd3d->releaseIndexBuffer(mIndexBuffer);
  }

  void render(bool wireframe,const NxF32 *matrix)
  {

    gPd3d->renderSection(&mMaterial,mVertexBuffer,mIndexBuffer,mVcount,mTcount);

    if ( wireframe )
    {
      gPd3d->setWireFrame(true);
      gPd3d->renderSection(&mMaterial,mVertexBuffer,mIndexBuffer,mVcount,mTcount);
      gPd3d->setWireFrame(false);
    }

    if ( gShowNormals )
    {
      NVSHARE::Pd3dGraphicsVertex *vtx = mVertices;
	  gRenderDebug->setCurrentColor(0x00FF00);
      for (NxU32 i=0; i<mVcount; i++)
      {
        NxF32 pos[3];

        NxF32 epos[3];
        epos[0] = vtx->mPos[0]+vtx->mNormal[0];
        epos[1] = vtx->mPos[1]+vtx->mNormal[1];
        epos[2] = vtx->mPos[2]+vtx->mNormal[2];

        NxF32 npos[3];

        fm_transform( matrix, vtx->mPos, pos );
        fm_transform( matrix, epos,      npos );

        gRenderDebug->DebugLine(pos,npos);
        vtx++;
      }
    }

  }

  TerrainMesh(NxU32 vbase,NxU32 vcount,NxU32 tvcount,NVSHARE::Pd3dGraphicsVertex *vertices,NxU32 *indices,NxU32 tcount,NVSHARE::Pd3dTexture *texture)
  {
    mMaterial.mHandle = texture;


    NVSHARE::Pd3dGraphicsVertex *vtx = MEMALLOC_NEW(NVSHARE::Pd3dGraphicsVertex)[65536];

    memcpy(vtx,&vertices[vbase],sizeof(NVSHARE::Pd3dGraphicsVertex)*vcount);

    NxU32 *extra = MEMALLOC_NEW(unsigned int)[tvcount];
    memset(extra,0xFF,sizeof(NxU32)*tvcount);

    NxU16 *dindices = MEMALLOC_NEW(unsigned short)[tcount*3];
    const NxU32 *source = indices;
    NxU16 *dest = dindices;

    NxU32 vend = vbase+vcount;

    for (NxU32 i=0; i<tcount; i++)
    {
      NxU32 i1 = source[0];
      NxU32 i2 = source[1];
      NxU32 i3 = source[2];

      bool b1 = i1 >= vbase && i1 < vend;
      bool b2 = i2 >= vbase && i2 < vend;
      bool b3 = i3 >= vbase && i3 < vend;

      if ( b1 && b2 && b3 )
      {

        dest[0] = (NxU16)(i1-vbase);
        dest[1] = (NxU16)(i2-vbase);
        dest[2] = (NxU16)(i3-vbase);

        dest+=3;
      }
      else if ( b1 || b2 || b3 )
      {
        NxU16 d1,d2,d3;

        if ( b1 )
        {
          d1 = (NxU16)(i1-vbase);
        }
        else
        {
          if ( extra[i1] == 0xFFFFFFFF )
          {
            vtx[vcount] = vertices[i1];
            extra[i1] = vcount;
            vcount++;
          }
          d1 = (NxU16)extra[i1];
        }

        if ( b2 )
        {
          d2 = (NxU16)(i2-vbase);
        }
        else
        {
          if ( extra[i2] == 0xFFFFFFFF )
          {
            vtx[vcount] = vertices[i2];
            extra[i2] = vcount;
            vcount++;
          }
          d2 = (NxU16)extra[i2];
        }

        if ( b3 )
        {
          d3 = (NxU16)(i3-vbase);
        }
        else
        {
          if ( extra[i3] == 0xFFFFFFFF )
          {
            vtx[vcount] = vertices[i3];
            extra[i3] = vcount;
            vcount++;
          }
          d3 = (NxU16)extra[i3];
        }
        assert( vcount < 65536 );
        dest[0] = d1;
        dest[1] = d2;
        dest[2] = d3;
        dest+=3;
      }

      source+=3;
    }

    mVertexBuffer = gPd3d->createVertexBuffer( vcount, vtx );

    mVertices = MEMALLOC_NEW(NVSHARE::Pd3dGraphicsVertex)[vcount];
    memcpy(mVertices,vtx,sizeof(NVSHARE::Pd3dGraphicsVertex)*vcount);
    mVcount = vcount;

    mTcount = (dest-dindices)/3;
    mIndexBuffer = gPd3d->createIndexBuffer(mTcount*3,dindices);

    delete []vtx;
    delete []extra;
    delete []dindices;

  }


//private:
  NVSHARE::Pd3dMaterial             mMaterial;
  void                          *mVertexBuffer;
  void                          *mIndexBuffer;
  NxU32                   mVcount;
  NxU32                   mTcount;
  NVSHARE::Pd3dGraphicsVertex     *mVertices; // backing store..

};

typedef std::vector< TerrainMesh * > TerrainMeshVector;


class Terrain : public GeometryInterface
{
public:

  Terrain(NxU32 wid,NxU32 hit,NVSHARE::Pd3dTexture *texture)
  {
    mRebuild = false;
		mClampLow = 0;
		mClampHigh = 1;
		mClampScale = 1;
    mHaveMeshes = false;

    mTexture = texture;
    mRotation = 0;
		mRotationSpeed = 0.1f;

    mWidth    = wid;
    mHeight   = hit;
    mVcount   = 0;
    mVertices = 0;
    mTcount   = 0;
    mIndices  = 0;

    mVcount   = mWidth*mHeight;
    mVertices = MEMALLOC_NEW(NVSHARE::Pd3dGraphicsVertex)[mVcount];
    mTcount   = (mWidth-1)*(mHeight-1)*2;
    mIndices  = MEMALLOC_NEW(unsigned int)[mTcount*3];
    mVertexBuffer = 0;
    mIndexBuffer = 0;


    NVSHARE::Pd3dGraphicsVertex *dest = mVertices;

    NxF32 recipx = 1.0f / (NxF32) mWidth;
    NxF32 recipy = 1.0f / (NxF32) mHeight;

    for (NxI32 y=0; y<(NxI32)mHeight; y++)
    {
      NxF32 fy = (NxF32)(y);

      for (NxI32 x=0; x<(NxI32)mWidth; x++)
      {
        NxF32 fx = (NxF32)(x);

        dest->mPos[0]    = (fx*recipx-0.5f)*XZSCALE;
        dest->mPos[1]    = 0;
        dest->mPos[2]    = (fy*recipy-0.5f)*XZSCALE;
        dest->mTexel[0]  = fx*recipx;
        dest->mTexel[1]  = fy*recipy;
        dest->mNormal[0] = 0;
        dest->mNormal[1] = 1;
        dest->mNormal[2] = 0;

        dest++;
      }
		}

    NxU32 *idx = mIndices;

    for (NxU32 y=0; y<(mHeight-1); y++)
    {
      for (NxU32 x=0; x<(mWidth-1); x++)
      {
        idx = addTris(idx,x,y);
      }
    }

  }

  ~Terrain(void)
  {
    delete []mVertices;
    delete []mIndices;

    if ( mVertexBuffer )
      gPd3d->releaseVertexBuffer(mVertexBuffer);

    if ( mIndexBuffer )
      gPd3d->releaseIndexBuffer(mIndexBuffer);

    TerrainMeshVector::iterator i;
    for (i=mMeshes.begin(); i!=mMeshes.end(); ++i)
    {
      TerrainMesh *tm = (*i);
      delete tm;
    }
    {
      TerrainMeshVector::iterator i;
      for (i=mSplitMeshes.begin(); i!=mSplitMeshes.end(); ++i)
      {
        TerrainMesh *tm = *(i);
        delete tm;
      }
      mSplitMeshes.clear();
    }
  }

  inline void meshRelease(void)
  {
    if ( mHaveMeshes )
    {
      TerrainMeshVector::iterator i;
      for (i=mMeshes.begin(); i!=mMeshes.end(); ++i)
      {
        TerrainMesh *tm = (*i);
        delete tm;
      }
      mMeshes.clear();
      mHaveMeshes = false;
    }
    if ( mVertexBuffer )
    {
      gPd3d->releaseVertexBuffer(mVertexBuffer);
      mVertexBuffer = 0;
    }
    if ( mIndexBuffer )
    {
      gPd3d->releaseIndexBuffer(mIndexBuffer);
      mIndexBuffer = 0;
    }
    if ( !mSplitMeshes.empty() )
    {
      TerrainMeshVector::iterator i;
      for (i=mSplitMeshes.begin(); i!=mSplitMeshes.end(); ++i)
      {
        TerrainMesh *tm = *(i);
        delete tm;
      }
      mSplitMeshes.clear();
    }
  }

  NxU32 getPower(NxU32 side)
  {
    if ( side <= 256 )
      side = 256;
    else if ( side <= 512 )
      side = 512;
    else if ( side <= 1024 )
      side = 1024;
    else if ( side <= 2048 )
      side = 2048;
    else if ( side <= 4096 )
      side = 4096;

    return side;
  }


  void NodeTriangleList(NxI32 vcount,const GeometryVertex *vertex,NxI32 icount,const NxU16 *indices)
  {
    TerrainMesh *tm = MEMALLOC_NEW(TerrainMesh)((NxU32)vcount,vertex,(NxU32)icount,indices,mTexture,mMeshScale);
    mMeshes.push_back(tm);
    mHaveMeshes = true;
  }

  void render(bool wireframe,NxF32 dtime)
  {

    if ( mRebuild )
    {
      mRebuild = false;

      if ( mVertexBuffer )
      {
        gPd3d->releaseVertexBuffer(mVertexBuffer);
        mVertexBuffer = 0;
      }

      if ( mIndexBuffer )
      {
        gPd3d->releaseIndexBuffer(mIndexBuffer);
        mIndexBuffer = 0;
      }

      {
        TerrainMeshVector::iterator i;
        for (i=mSplitMeshes.begin(); i!=mSplitMeshes.end(); ++i)
        {
          TerrainMesh *tm = *(i);
          delete tm;
        }
        mSplitMeshes.clear();
      }

      computeNormals();

      if ( mVcount > 60000 )
      {
        NxU32 v;
        for (v=0; v<mVcount; v+=60000)
        {
          NxU32 remainder = mVcount-v;
          if ( remainder > 60000 ) remainder = 60000;
          TerrainMesh *tm = MEMALLOC_NEW(TerrainMesh)(v,remainder,mVcount,mVertices,mIndices,mTcount,mTexture);
          mSplitMeshes.push_back(tm);
        }
      }
      else
      {
        mVertexBuffer = gPd3d->createVertexBuffer(mVcount,mVertices);
        mIndexBuffer  = gPd3d->createIndexBuffer(mTcount*3,mIndices);
      }

    }

    mRotation+=(dtime*mRotationSpeed*gRotationSpeed);

    NxF32 matrix[16];
    fm_eulerToMatrix(0,mRotation,0,matrix);
    gPd3d->setWorldMatrix(matrix);


		if ( !mMeshes.empty() )
		{
			TerrainMeshVector::iterator i;
			for (i=mMeshes.begin(); i!=mMeshes.end(); ++i)
			{
				TerrainMesh *tm = (*i);
				tm->render(wireframe,matrix);
			}
		}
		else if ( mVertexBuffer && mIndexBuffer )
    {
  		gPd3d->setFractalMode(true);
	  	gPd3d->setClampConstants(mClampLow,mClampHigh,mClampScale*YSCALE);
      mMaterial.mHandle = mTexture;
      gPd3d->renderSection(&mMaterial,mVertexBuffer,mIndexBuffer,mVcount,mTcount);
    }
    else if ( mVertices )
    {
	  	gPd3d->setClampConstants(mClampLow,mClampHigh,mClampScale*YSCALE);
			gPd3d->setFractalMode(true);
      mMaterial.mHandle = mTexture;
      if ( mSplitMeshes.empty() )
      {
        gPd3d->renderSection(&mMaterial,mVertices,mIndices,mVcount,mTcount);
      }
      else
      {
  			TerrainMeshVector::iterator i;
  			for (i=mSplitMeshes.begin(); i!=mSplitMeshes.end(); ++i)
  			{
  				TerrainMesh *tm = (*i);
          gPd3d->renderSection(&mMaterial,tm->mVertexBuffer,tm->mIndexBuffer,tm->mVcount,tm->mTcount);
	  		}
      }

    }

		gPd3d->setFractalMode(false);
  }


  NxU32 * addTris(NxU32 *idx,NxU32 x,NxU32 y) const
  {
    NxU32 i1 = (y*mWidth)+x;
    NxU32 i2 = (y*mWidth)+(x+1);
    NxU32 i3 = ((y+1)*mWidth)+(x+1);
    NxU32 i4 = ((y+1)*mWidth)+x;

    idx[0] = i3;
    idx[1] = i2;
    idx[2] = i1;

    idx[3] = i4;
    idx[4] = i3;
    idx[5] = i1;

    idx+=6;

    return idx;
  }

  NxF32 GetHeight(NxU32 x,NxU32 y)
  {
    if ( x >= mWidth ) x = mWidth-1;
    if ( y >= mHeight ) y = mHeight-1;
    NxU32 index = y*mWidth+x;
    return mVertices[index].mPos[1];
  }

  void plot(NxU32 x,NxU32 y,NxU32 iterCount)
  {
    meshRelease();
    NxF32 fv = (NxF32)(iterCount);

    NxU32 index = y*mWidth+x;
    mVertices[index].mPos[1] = fv;

  }

  void complete(const NxU32 * /*data*/)
  {
    meshRelease();
    mRebuild = true;
  }

  void optimize(void)
  {
    meshRelease();

    NxU32 side = mWidth;
    if ( mHeight > side ) side = mHeight;
    side = getPower(side);

    NxF32 *hf = MEMALLOC_NEW(float)[side*side];
    memset(hf,0,sizeof(NxF32)*side);

    mMeshScale = (NxF32) XZSCALE / side;

		NxF32 vscale = 0.25f;

    for (NxU32 y=0; y<mHeight; y++)
    {
      for (NxU32 x=0; x<mWidth; x++)
      {
        NxU32 index = y*mWidth+x;
        NxF32 v = mVertices[index].mPos[1];
        if ( v < mClampLow )
          v = mClampLow;
        else if ( v > mClampHigh )
          v = mClampHigh;

        v-=mClampLow;
        v*=mClampScale;

        NxU32 dindex = y*side+x;

        hf[dindex] = v*mMeshScale*vscale;
      }
    }
    RtinObj rtin(side,side,hf);
    rtin.CreateRtin("fractal", 16, false, 1.0f, this, "texture1", "texture2", 0, true, true );
    delete []hf;
  }

	void  clampRange(NxF32 clampLow,NxF32 clampHigh,NxF32 clampScale,NxU32 iterationCount)
	{
    NxF32 i = (NxF32)iterationCount;
    mClampLow  = i*clampLow;
    mClampHigh = i*clampHigh;
    if ( mClampLow > mClampHigh )
    {
      mClampLow = mClampHigh;
    }
    mClampScale = clampScale;
//		gLog->Display("ClampLow: %0.2f ClampHigh: %0.2f ClampScale: %0.2f\r\n", mClampLow, mClampHigh, mClampScale );
	}

  void getScaledPoint(NxF32 *dest,const NxF32 *source)
  {
    dest[0] = source[0];
    dest[2] = source[2];

    NxF32 y = source[1];
    if ( y < mClampLow )
      y = mClampLow;
    else if ( y > mClampHigh )
      y = mClampHigh;

    y = y*mClampScale;

    dest[1] = y+1;
  }

  void computeNormals(void)
  {
    for (NxU32 i=0; i<mVcount; i++)
    {
      mVertices[i].mNormal[0] = 0;
      mVertices[i].mNormal[1] = 0;
      mVertices[i].mNormal[2] = 0;
    }

    const NxU32 *indices = mIndices;
    for (NxU32 i=0; i<mTcount; i++)
    {
      NxU32 i1 = indices[0];
      NxU32 i2 = indices[1];
      NxU32 i3 = indices[2];

      NVSHARE::Pd3dGraphicsVertex &v1 = mVertices[i1];
      NVSHARE::Pd3dGraphicsVertex &v2 = mVertices[i2];
      NVSHARE::Pd3dGraphicsVertex &v3 = mVertices[i3];

      NxF32 p1[3];
      NxF32 p2[3];
      NxF32 p3[3];

      getScaledPoint(p1,v1.mPos);
      getScaledPoint(p2,v2.mPos);
      getScaledPoint(p3,v3.mPos);

      NxF32 normal[3];
      fm_computePlane(p3,p2,p1,normal);

      v1.mNormal[0]+=normal[0];
      v1.mNormal[1]+=normal[1];
      v1.mNormal[2]+=normal[2];

      v2.mNormal[0]+=normal[0];
      v2.mNormal[1]+=normal[1];
      v2.mNormal[2]+=normal[2];

      v3.mNormal[0]+=normal[0];
      v3.mNormal[1]+=normal[1];
      v3.mNormal[2]+=normal[2];

      indices+=3;
    }

  }

  NxF32 getRotation(void) const { return mRotation; };
  void setRotation(NxF32 r) { mRotation = r; };

  void reset(void)
  {
  }

  NxF32 tap(NxI32 x,NxI32 y,const NxF32 *data,NxF32 s,NxF32 &t)
  {
    t+=s;
    if ( x < 0 ) x = 0;
    if ( y < 0 ) y = 0;
    if ( x >= (NxI32)(mWidth) ) x = mWidth-1;
    if ( y >= (NxI32)(mHeight) ) y = mHeight-1;
    NxU32 index = y*mWidth+x;
    return data[index]*s;
  }

  NxF32 tapFilter(NxU32 x,NxU32 y,const NxF32 *data)
  {
    NxI32 ix = (NxI32) x;
    NxI32 iy = (NxI32) y;

    #define TAP1 5
    #define TAP2 3
    #define TAP3 1

    NxF32 count=0;

    NxF32 total = tap(ix,iy,data,TAP1,count);
    total+=tap(ix-1,iy,data,TAP2,count);
    total+=tap(ix+1,iy,data,TAP2,count);
    total+=tap(ix,iy-1,data,TAP2,count);
    total+=tap(ix,iy+1,data,TAP2,count);
    total+=tap(ix+1,iy+1,data,TAP3,count);
    total+=tap(ix+1,iy-1,data,TAP3,count);
    total+=tap(ix-1,iy+1,data,TAP3,count);
    total+=tap(ix-1,iy-1,data,TAP3,count);

    return total / count;
  }


  void filter(void)
  {

    meshRelease();

    NxF32 *data = MEMALLOC_NEW(float)[mWidth*mHeight];
    for (NxU32 y=0; y<mHeight; y++)
    {
      for (NxU32 x=0; x<mWidth; x++)
      {
        NxU32 index = y*mWidth+x;
        data[index] = mVertices[index].mPos[1];
      }
    }

    for (NxU32 y=0; y<mHeight; y++)
    {
      for (NxU32 x=0; x<mWidth; x++)
      {
        NxU32 index = y*mWidth+x;
        mVertices[index].mPos[1] = tapFilter(x,y,data);
      }
    }
    delete []data;
    mRebuild = true;
  }

private:
  NVSHARE::Pd3dMaterial             mMaterial;
  NVSHARE::Pd3dTexture             *mTexture;

  TerrainMeshVector              mMeshes;
  TerrainMeshVector              mSplitMeshes;

  NxU32                   mWidth;
  NxU32                   mHeight;
  NxU32                   mVcount;
  NxU32                   mTcount;
  NVSHARE::Pd3dGraphicsVertex      *mVertices;
  NxU32                  *mIndices;

  void                          *mVertexBuffer;
  void                          *mIndexBuffer;

	NxF32                          mClampLow;
	NxF32                          mClampHigh;
	NxF32                          mClampScale;
  NxF32                          mRotation;
	NxF32                          mRotationSpeed;
  bool                           mRebuild;
  bool                           mHaveMeshes;

  NxF32                          mMeshScale; //

};









Terrain * createTerrain(NxU32 wid,NxU32 hit,NVSHARE::Pd3dTexture *texture)
{
  Terrain *t=0;
  t = MEMALLOC_NEW(Terrain)(wid,hit,texture);
  return t;
}

void      releaseTerrain(Terrain *t)
{
  delete t;
}


void      renderTerrain(Terrain *t,bool wireframe,NxF32 dtime)
{
  if ( t )
    t->render(wireframe,dtime);
}

void      terrainReset(Terrain *t)
{
  t->reset();
}

void      plot(Terrain *t,NxU32 x,NxU32 y,NxU32 iterCount)
{
  t->plot(x,y,iterCount);
}

void      terrainComplete(Terrain *t,const NxU32 *data) // the terrain is completed, it can now be copied to a static vertex buffer and index buffer and optimized.
{
  t->complete(data);
}

void  terrainClampRange(Terrain *t,NxF32 clampLow,NxF32 clampHigh,NxF32 clampScale,NxU32 iterationCount)
{
	t->clampRange(clampLow,clampHigh,clampScale,iterationCount);
}

NxF32 getRotation(Terrain *t)
{
  return t->getRotation();
}

void  setRotation(Terrain *t,NxF32 rot)
{
  t->setRotation(rot);
}


void  filter(Terrain *t)
{
  t->filter();
}


void  optimize(Terrain *t)
{
  t->optimize();
}


void  setShowNormals(bool state)
{
  gShowNormals = state;
}

void  setRotationSpeed(NxF32 rspeed)
{
  gRotationSpeed = rspeed;
}
