#pragma warning(disable:4702)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <vector>

#include "terrain.h"
#include "common/rtin/rtinobj.h"
#include "RenderDebug/RenderDebug.h"
#include "Pd3d/pd3d.h"
#include "common/snippets/floatmath.h"
#include "common/snippets/log.h"

const HeF32 YSCALE = 1.0f / 10.0f;
const HeF32 XZSCALE = 400.0f;

static bool gShowNormals=false;
static HeF32 gRotationSpeed=1;

class TerrainMesh
{
public:

  TerrainMesh(HeU32 vcount,const GeometryVertex *vertices,HeU32 icount,const HeU16 *indices,PD3D::Pd3dTexture *texture,HeF32 meshScale)
  {
    mMaterial.mHandle = texture;
    mTcount           = icount/3;
    mVcount           = vcount;
    mIndexBuffer = gPd3d->createIndexBuffer(icount,indices);

    mVertices = MEMALLOC_NEW_ARRAY(PD3D::Pd3dGraphicsVertex,vcount)[vcount];

    PD3D::Pd3dGraphicsVertex *dest = mVertices;

    for (HeU32 i=0; i<vcount; i++)
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

  void render(bool wireframe,const HeF32 *matrix)
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
      PD3D::Pd3dGraphicsVertex *vtx = mVertices;
      for (HeU32 i=0; i<mVcount; i++)
      {
        HeF32 pos[3];

        HeF32 epos[3];
        epos[0] = vtx->mPos[0]+vtx->mNormal[0];
        epos[1] = vtx->mPos[1]+vtx->mNormal[1];
        epos[2] = vtx->mPos[2]+vtx->mNormal[2];

        HeF32 npos[3];

        fm_transform( matrix, vtx->mPos, pos );
        fm_transform( matrix, epos,      npos );

        gRenderDebug->DebugLine(pos,npos,0x00FF00);
        vtx++;
      }
    }

  }

  TerrainMesh(HeU32 vbase,HeU32 vcount,HeU32 tvcount,PD3D::Pd3dGraphicsVertex *vertices,HeU32 *indices,HeU32 tcount,PD3D::Pd3dTexture *texture)
  {
    mMaterial.mHandle = texture;


    PD3D::Pd3dGraphicsVertex *vtx = MEMALLOC_NEW_ARRAY(PD3D::Pd3dGraphicsVertex,65536)[65536];

    memcpy(vtx,&vertices[vbase],sizeof(PD3D::Pd3dGraphicsVertex)*vcount);

    HeU32 *extra = MEMALLOC_NEW_ARRAY(unsigned int,tvcount)[tvcount];
    memset(extra,0xFF,sizeof(HeU32)*tvcount);

    HeU16 *dindices = MEMALLOC_NEW_ARRAY(unsigned short,tcount*3)[tcount*3];
    const HeU32 *source = indices;
    HeU16 *dest = dindices;

    HeU32 vend = vbase+vcount;

    for (HeU32 i=0; i<tcount; i++)
    {
      HeU32 i1 = source[0];
      HeU32 i2 = source[1];
      HeU32 i3 = source[2];

      bool b1 = i1 >= vbase && i1 < vend;
      bool b2 = i2 >= vbase && i2 < vend;
      bool b3 = i3 >= vbase && i3 < vend;

      if ( b1 && b2 && b3 )
      {

        dest[0] = (HeU16)(i1-vbase);
        dest[1] = (HeU16)(i2-vbase);
        dest[2] = (HeU16)(i3-vbase);

        dest+=3;
      }
      else if ( b1 || b2 || b3 )
      {
        HeU16 d1,d2,d3;

        if ( b1 )
        {
          d1 = (HeU16)(i1-vbase);
        }
        else
        {
          if ( extra[i1] == 0xFFFFFFFF )
          {
            vtx[vcount] = vertices[i1];
            extra[i1] = vcount;
            vcount++;
          }
          d1 = (HeU16)extra[i1];
        }

        if ( b2 )
        {
          d2 = (HeU16)(i2-vbase);
        }
        else
        {
          if ( extra[i2] == 0xFFFFFFFF )
          {
            vtx[vcount] = vertices[i2];
            extra[i2] = vcount;
            vcount++;
          }
          d2 = (HeU16)extra[i2];
        }

        if ( b3 )
        {
          d3 = (HeU16)(i3-vbase);
        }
        else
        {
          if ( extra[i3] == 0xFFFFFFFF )
          {
            vtx[vcount] = vertices[i3];
            extra[i3] = vcount;
            vcount++;
          }
          d3 = (HeU16)extra[i3];
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

    mVertices = MEMALLOC_NEW_ARRAY(PD3D::Pd3dGraphicsVertex,vcount)[vcount];
    memcpy(mVertices,vtx,sizeof(PD3D::Pd3dGraphicsVertex)*vcount);
    mVcount = vcount;

    mTcount = (dest-dindices)/3;
    mIndexBuffer = gPd3d->createIndexBuffer(mTcount*3,dindices);

    delete []vtx;
    delete []extra;
    delete []dindices;

  }


//private:
  PD3D::Pd3dMaterial             mMaterial;
  void                          *mVertexBuffer;
  void                          *mIndexBuffer;
  HeU32                   mVcount;
  HeU32                   mTcount;
  PD3D::Pd3dGraphicsVertex     *mVertices; // backing store..

};

typedef std::vector< TerrainMesh * > TerrainMeshVector;


class Terrain : public GeometryInterface
{
public:

  Terrain(HeU32 wid,HeU32 hit,PD3D::Pd3dTexture *texture)
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
    mVertices = MEMALLOC_NEW_ARRAY(PD3D::Pd3dGraphicsVertex,mVcount)[mVcount];
    mTcount   = (mWidth-1)*(mHeight-1)*2;
    mIndices  = MEMALLOC_NEW_ARRAY(unsigned int,mTcount*3)[mTcount*3];
    mVertexBuffer = 0;
    mIndexBuffer = 0;


    PD3D::Pd3dGraphicsVertex *dest = mVertices;

    HeF32 recipx = 1.0f / (HeF32) mWidth;
    HeF32 recipy = 1.0f / (HeF32) mHeight;

    for (HeI32 y=0; y<(HeI32)mHeight; y++)
    {
      HeF32 fy = (HeF32)(y);

      for (HeI32 x=0; x<(HeI32)mWidth; x++)
      {
        HeF32 fx = (HeF32)(x);

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

    HeU32 *idx = mIndices;

    for (HeU32 y=0; y<(mHeight-1); y++)
    {
      for (HeU32 x=0; x<(mWidth-1); x++)
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

  HeU32 getPower(HeU32 side)
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


  void NodeTriangleList(HeI32 vcount,const GeometryVertex *vertex,HeI32 icount,const HeU16 *indices)
  {
    TerrainMesh *tm = MEMALLOC_NEW(TerrainMesh)((HeU32)vcount,vertex,(HeU32)icount,indices,mTexture,mMeshScale);
    mMeshes.push_back(tm);
    mHaveMeshes = true;
  }

  void render(bool wireframe,HeF32 dtime)
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
        HeU32 v;
        for (v=0; v<mVcount; v+=60000)
        {
          HeU32 remainder = mVcount-v;
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

    HeF32 matrix[16];
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


  HeU32 * addTris(HeU32 *idx,HeU32 x,HeU32 y) const
  {
    HeU32 i1 = (y*mWidth)+x;
    HeU32 i2 = (y*mWidth)+(x+1);
    HeU32 i3 = ((y+1)*mWidth)+(x+1);
    HeU32 i4 = ((y+1)*mWidth)+x;

    idx[0] = i3;
    idx[1] = i2;
    idx[2] = i1;

    idx[3] = i4;
    idx[4] = i3;
    idx[5] = i1;

    idx+=6;

    return idx;
  }

  HeF32 GetHeight(HeU32 x,HeU32 y)
  {
    if ( x >= mWidth ) x = mWidth-1;
    if ( y >= mHeight ) y = mHeight-1;
    HeU32 index = y*mWidth+x;
    return mVertices[index].mPos[1];
  }

  void plot(HeU32 x,HeU32 y,HeU32 iterCount)
  {
    meshRelease();
    HeF32 fv = (HeF32)(iterCount);

    HeU32 index = y*mWidth+x;
    mVertices[index].mPos[1] = fv;

  }

  void complete(const HeU32 * /*data*/)
  {
    meshRelease();
    mRebuild = true;
  }

  void optimize(void)
  {
    meshRelease();

    HeU32 side = mWidth;
    if ( mHeight > side ) side = mHeight;
    side = getPower(side);

    HeF32 *hf = MEMALLOC_NEW_ARRAY(float,side*side)[side*side];
    memset(hf,0,sizeof(HeF32)*side);

    mMeshScale = (HeF32) XZSCALE / side;

		HeF32 vscale = 0.25f;

    for (HeU32 y=0; y<mHeight; y++)
    {
      for (HeU32 x=0; x<mWidth; x++)
      {
        HeU32 index = y*mWidth+x;
        HeF32 v = mVertices[index].mPos[1];
        if ( v < mClampLow )
          v = mClampLow;
        else if ( v > mClampHigh )
          v = mClampHigh;

        v-=mClampLow;
        v*=mClampScale;

        HeU32 dindex = y*side+x;

        hf[dindex] = v*mMeshScale*vscale;
      }
    }
    RtinObj rtin(side,side,hf);
    rtin.CreateRtin("fractal", 16, false, 1.0f, this, "texture1", "texture2", 0, true, true );
    delete []hf;
  }

	void  clampRange(HeF32 clampLow,HeF32 clampHigh,HeF32 clampScale,HeU32 iterationCount)
	{
    HeF32 i = (HeF32)iterationCount;
    mClampLow  = i*clampLow;
    mClampHigh = i*clampHigh;
    if ( mClampLow > mClampHigh )
    {
      mClampLow = mClampHigh;
    }
    mClampScale = clampScale;
//		gLog->Display("ClampLow: %0.2f ClampHigh: %0.2f ClampScale: %0.2f\r\n", mClampLow, mClampHigh, mClampScale );
	}

  void getScaledPoint(HeF32 *dest,const HeF32 *source)
  {
    dest[0] = source[0];
    dest[2] = source[2];

    HeF32 y = source[1];
    if ( y < mClampLow )
      y = mClampLow;
    else if ( y > mClampHigh )
      y = mClampHigh;

    y = y*mClampScale;

    dest[1] = y+1;
  }

  void computeNormals(void)
  {
    for (HeU32 i=0; i<mVcount; i++)
    {
      mVertices[i].mNormal[0] = 0;
      mVertices[i].mNormal[1] = 0;
      mVertices[i].mNormal[2] = 0;
    }

    const HeU32 *indices = mIndices;
    for (HeU32 i=0; i<mTcount; i++)
    {
      HeU32 i1 = indices[0];
      HeU32 i2 = indices[1];
      HeU32 i3 = indices[2];

      PD3D::Pd3dGraphicsVertex &v1 = mVertices[i1];
      PD3D::Pd3dGraphicsVertex &v2 = mVertices[i2];
      PD3D::Pd3dGraphicsVertex &v3 = mVertices[i3];

      HeF32 p1[3];
      HeF32 p2[3];
      HeF32 p3[3];

      getScaledPoint(p1,v1.mPos);
      getScaledPoint(p2,v2.mPos);
      getScaledPoint(p3,v3.mPos);

      HeF32 normal[3];
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

  HeF32 getRotation(void) const { return mRotation; };
  void setRotation(HeF32 r) { mRotation = r; };

  void reset(void)
  {
  }

  HeF32 tap(HeI32 x,HeI32 y,const HeF32 *data,HeF32 s,HeF32 &t)
  {
    t+=s;
    if ( x < 0 ) x = 0;
    if ( y < 0 ) y = 0;
    if ( x >= (HeI32)(mWidth) ) x = mWidth-1;
    if ( y >= (HeI32)(mHeight) ) y = mHeight-1;
    HeU32 index = y*mWidth+x;
    return data[index]*s;
  }

  HeF32 tapFilter(HeU32 x,HeU32 y,const HeF32 *data)
  {
    HeI32 ix = (HeI32) x;
    HeI32 iy = (HeI32) y;

    #define TAP1 5
    #define TAP2 3
    #define TAP3 1

    HeF32 count=0;

    HeF32 total = tap(ix,iy,data,TAP1,count);
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

    HeF32 *data = MEMALLOC_NEW_ARRAY(float,mWidth*mHeight)[mWidth*mHeight];
    for (HeU32 y=0; y<mHeight; y++)
    {
      for (HeU32 x=0; x<mWidth; x++)
      {
        HeU32 index = y*mWidth+x;
        data[index] = mVertices[index].mPos[1];
      }
    }

    for (HeU32 y=0; y<mHeight; y++)
    {
      for (HeU32 x=0; x<mWidth; x++)
      {
        HeU32 index = y*mWidth+x;
        mVertices[index].mPos[1] = tapFilter(x,y,data);
      }
    }
    delete []data;
    mRebuild = true;
  }

private:
  PD3D::Pd3dMaterial             mMaterial;
  PD3D::Pd3dTexture             *mTexture;

  TerrainMeshVector              mMeshes;
  TerrainMeshVector              mSplitMeshes;

  HeU32                   mWidth;
  HeU32                   mHeight;
  HeU32                   mVcount;
  HeU32                   mTcount;
  PD3D::Pd3dGraphicsVertex      *mVertices;
  HeU32                  *mIndices;

  void                          *mVertexBuffer;
  void                          *mIndexBuffer;

	HeF32                          mClampLow;
	HeF32                          mClampHigh;
	HeF32                          mClampScale;
  HeF32                          mRotation;
	HeF32                          mRotationSpeed;
  bool                           mRebuild;
  bool                           mHaveMeshes;

  HeF32                          mMeshScale; //

};









Terrain * createTerrain(HeU32 wid,HeU32 hit,PD3D::Pd3dTexture *texture)
{
  Terrain *t=0;
  t = MEMALLOC_NEW(Terrain)(wid,hit,texture);
  return t;
}

void      releaseTerrain(Terrain *t)
{
  delete t;
}


void      renderTerrain(Terrain *t,bool wireframe,HeF32 dtime)
{
  if ( t )
    t->render(wireframe,dtime);
}

void      terrainReset(Terrain *t)
{
  t->reset();
}

void      plot(Terrain *t,HeU32 x,HeU32 y,HeU32 iterCount)
{
  t->plot(x,y,iterCount);
}

void      terrainComplete(Terrain *t,const HeU32 *data) // the terrain is completed, it can now be copied to a static vertex buffer and index buffer and optimized.
{
  t->complete(data);
}

void  terrainClampRange(Terrain *t,HeF32 clampLow,HeF32 clampHigh,HeF32 clampScale,HeU32 iterationCount)
{
	t->clampRange(clampLow,clampHigh,clampScale,iterationCount);
}

HeF32 getRotation(Terrain *t)
{
  return t->getRotation();
}

void  setRotation(Terrain *t,HeF32 rot)
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

void  setRotationSpeed(HeF32 rspeed)
{
  gRotationSpeed = rspeed;
}
