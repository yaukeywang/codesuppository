#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "terrain.h"
#include "ImportHeightMap.h"
#include "NxFoundation.h"
#include "RenderDebug/RenderDebug.h"
#include "simplevector.h"
#include "pd3d/pd3d.h"
#include "FloatMath.h"

#pragma warning(disable:4100)

#define TSCALE 0.001f

class MyTerrain : public Terrain
{
public:

  MyTerrain(const char *fname,TerrainInterface *iface)
  {
    mData =0;
    mWidth = 0;
    mHeight = 0;
    mInterface = 0;
    mIndexBuffer = 0;
    mVertexBuffer = 0;

    strncpy(mMaterial.mTexture,"terrain.dds",512);
    ImportHeightMap *importHeightMap = createImportHeightMap();
    bool ok = importHeightMap->importHeightMap(fname);
    if ( !ok )
    {
      releaseImportHeightMap(importHeightMap);
    }
    else
    {
      mWidth = importHeightMap->getWidth();
      mHeight = importHeightMap->getDepth();

      mData = new NxF32[mWidth*mHeight];
      memcpy(mData,importHeightMap->getData(),sizeof(NxF32)*mWidth*mHeight);
      NxU32 tcount = mWidth*mHeight;
      for (NxU32 i=0; i<tcount; i++)
        mData[i]*=40;

      releaseImportHeightMap(importHeightMap);
      mInterface = iface;
      rebuildMesh();
    }
  }

  ~MyTerrain(void)
  {
    if ( mVertexBuffer ) gPd3d->releaseVertexBuffer(mVertexBuffer);
    if ( mIndexBuffer ) gPd3d->releaseIndexBuffer(mIndexBuffer);
    delete []mData;
  }

  NxU32 getIndex(NxU32 wid,NxU32 depth,NxU32 x,NxU32 z)
  {
    return (z*wid)+x;
  }

  void render(void)
  {
    if ( mVertexBuffer )
    {
      gPd3d->renderSection(&mMaterial,mVertexBuffer,mIndexBuffer,mVcount,mTcount);
    }
  }

  NxU32  getWidth(void)
  {
    return mWidth;
  }

  NxU32  getHeight(void)
  {
    return mHeight;
  }

  NxF32 * getData(void)
  {
    return mData;
  }

  void   rebuildMesh(void)
  {
    if ( mVertexBuffer ) gPd3d->releaseVertexBuffer(mVertexBuffer);
    if ( mIndexBuffer ) gPd3d->releaseIndexBuffer(mIndexBuffer);
    mVertexBuffer = 0;
    mIndexBuffer = 0;
    if ( mData )
    {

      SimpleVector< NxU32 > indices;
      SimpleVector< NxVec3 > vertices;

      NxU32 wid   = mWidth;
      NxU32 depth = mHeight;

      for (NxU32 z=0; z<depth; z++)
      {
        for (NxU32 x=0; x<wid; x++)
        {
          NxVec3 p1;
          NxU32 index = z*wid+x;
          p1.x = (NxF32)x;
          p1.y = mData[index];
          p1.z = (NxF32)z;
          vertices.push_back(p1);
        }
      }

      for (NxU32 z=0; z<(depth-1); z++)
      {
        for (NxU32 x=0; x<(wid-1); x++)
        {
          NxU32 i1 = getIndex(wid,depth,x,z);
          NxU32 i2 = getIndex(wid,depth,x+1,z);
          NxU32 i3 = getIndex(wid,depth,x+1,z+1);
          NxU32 i4 = getIndex(wid,depth,x,z+1);

          indices.push_back(i3);
          indices.push_back(i2);
          indices.push_back(i1);

          indices.push_back(i4);
          indices.push_back(i3);
          indices.push_back(i1);

        }
      }

      NxU32 vcount = vertices.size();

      PD3D::Pd3dGraphicsVertex *gv = new PD3D::Pd3dGraphicsVertex[vcount];
      for (NxU32 i=0; i<vcount; i++)
      {
        PD3D::Pd3dGraphicsVertex *dest = &gv[i];
        const NxVec3 &source           = vertices[i];
        dest->mPos[0] = source.x;
        dest->mPos[1] = source.y;
        dest->mPos[2] = source.z;
        dest->mTexel[0] = source.x*TSCALE;
        dest->mTexel[1] = source.z*TSCALE;
      }

      fm_computeMeanNormals(vcount,gv->mPos,sizeof(PD3D::Pd3dGraphicsVertex),gv->mNormal,sizeof(PD3D::Pd3dGraphicsVertex),indices.size()/3,&indices[0]);
      mVertexBuffer = gPd3d->createVertexBuffer(vcount,gv);
      delete []gv;

      mIndexBuffer = gPd3d->createIndexBuffer(indices.size(),&indices[0]);

      if ( mInterface )
      {
        mInterface->notifyTerrain(vcount,&vertices[0].x,indices.size()/3,&indices[0]);
      }


    }

  }

private:
  NxU32               mWidth;
  NxU32               mHeight;
  NxF32              *mData;
  NxU32               mVcount;
  NxU32               mTcount;
  PD3D::Pd3dMaterial  mMaterial;
  void               *mVertexBuffer;
  void               *mIndexBuffer;
  TerrainInterface   *mInterface;
};



Terrain * createTerrain(const char *fname,TerrainInterface *iface)
{
  MyTerrain *mt = new MyTerrain(fname,iface);
  return static_cast< Terrain *>(mt);
}

void      releaseTerrain(Terrain *t)
{
  MyTerrain *mt = static_cast< MyTerrain *>(t);
  delete mt;
}

