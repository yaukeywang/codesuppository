/*----------------------------------------------------------------------
    Copyright (c) 2004 Open Dynamics Framework Group
          www.physicstools.org
    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, are permitted provided
    that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this list of conditions
    and the following disclaimer.

    Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

    Neither the name of the Open Dynamics Framework Group nor the names of its contributors may
    be used to endorse or promote products derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES,
    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE INTEL OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
    IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "rtinobj.h"
#include "errorh.h"
#include "common/snippets/matrix.h"

#define MULTI_TEXTURE 1
#define TILE_RATE 32

//==================================================================================
RtinObj::RtinObj(HeI32 width,HeI32 height,const HeF32 *data)
{
  mError	= 0;
  mWidth    = width;
  mHeight   = height;
  mTexRecip = 1.0f/HeF32(mHeight+1);
  mData     = data;
  mRtin     = 0;
  mOffset   = 0;
}

//==================================================================================
RtinObj::~RtinObj(void)
{
  delete mRtin;
  delete mError;
}

//==================================================================================
void RtinObj::CreateRtin(const char * /*_name*/,
                  HeI32  err_thresh,
                  bool zup,
                  HeF32 scale,
                  GeometryInterface *iface,
                  const char *texture1,
                  const char *texture2,
                  HeF32 water,
                  bool absolute,
                  bool origin)
{

  const char *fname = "test";

  mTotalVerts = 0;
  mTotalFaces = 0;
  mErrThresh  = err_thresh;
  mZup        = zup;
  mScale      = scale;
  strcpy(mTexture1,texture1);
  strcpy(mTexture2,texture2);
  mWater      = water;
  mAbsolute   = absolute;
  mOrigin     = origin;
  mOffset     = 0;
  if ( mOrigin )
  {
    mOffset = HeF32(mWidth/2)*mScale*-1;
  }


  if ( !mRtin )
  {
    mRtin  = MEMALLOC_NEW(TopoRtin)(this);
    mError = MEMALLOC_NEW(ErrorHeuristic)(*this,*mRtin);
  }

  mRtin->SetErrorHeuristic(mError);


  mRtin->SetBlockSize(BLOCKSIZE);
  mRtin->SetLod(false);
  mRtin->SetErrorThreshold(err_thresh);
  if ( water == -1 )
    mRtin->SetIgnoreSealevel(false);
  else
  {
    mRtin->SetIgnoreSealevel(true);
    mRtin->SetSeaLevel(water);
  }

  mRtin->SetEye(0,0); // cause it to rebuild

  // ok, we need to gather all of the triangles to compute the mean unit normal vector at each vertice for lighting purpoes.
  {


    HeI32 isize = BLOCKLEN*BLOCKLEN*2*3;
    HeU16 *indices = MEMALLOC_NEW_ARRAY(unsigned short,isize)[isize];

    for (HeI32 y=0; y<mHeight; y+=BLOCKSIZE)
    {
      for (HeI32 x=0; x<mWidth; x+=BLOCKSIZE)
      {

        mBaseX = x;
        mBaseY = y;

        HeU16 *result = mRtin->BuildIndices(indices,mBaseX,mBaseY);

        HeI32 icount=0;
        if ( result ) icount = (HeI32)(result-indices);

        if ( icount )
        {
          HeI32 tcount = icount/3;

          HeU16 *index = indices;

          for (HeI32 i=0; i<tcount; i++,index+=3)
          {
        	  HeI32 i1 = index[0];
        	  HeI32 i2 = index[1];
        	  HeI32 i3 = index[2];

            HeF32 p1[3];
            HeF32 p2[3];
            HeF32 p3[3];

            GetVertex(i1,p1);
            GetVertex(i2,p2);
            GetVertex(i3,p3);

            HeI32 index1 = Translate(p1);
            HeI32 index2 = Translate(p2);
            HeI32 index3 = Translate(p3);

            HeF32 normal[3]; // compute the normal vector

            {
              HeF32 vx,vy,vz,wx,wy,wz,vw_x,vw_y,vw_z,mag;

              if ( mZup )
              {

                vx = (p2[0] - p3[0]);
                vy = (p2[1] - p3[1]);
                vz = (p2[2] - p3[2]);

                wx = (p1[0] - p2[0]);
                wy = (p1[1] - p2[1]);
                wz = (p1[2] - p2[2]);

              }
              else
              {
                vx = (p2[0] - p1[0]);
                vy = (p2[1] - p1[1]);
                vz = (p2[2] - p1[2]);

                wx = (p3[0] - p2[0]);
                wy = (p3[1] - p2[1]);
                wz = (p3[2] - p2[2]);

              }

              vw_x = vy * wz - vz * wy;
              vw_y = vz * wx - vx * wz;
              vw_z = vx * wy - vy * wx;

              mag = sqrtf((vw_x * vw_x) + (vw_y * vw_y) + (vw_z * vw_z));

              if ( mag < 0.000001f )
              {
                mag = 0;
              }
              else
              {
                mag = 1.0f/mag;
              }

              normal[0] = vw_x * mag;
              normal[1] = vw_y * mag;
              normal[2] = vw_z * mag;
            }

            AddNormal(index1,normal);
            AddNormal(index2,normal);
            AddNormal(index3,normal);


          }
        }
      }
    }
    {
      MeanNormalMap::iterator i;
      for (i=mNormals.begin(); i!=mNormals.end(); ++i)
      {
        (*i).second.ComputeMean();
      }
    }
    delete []indices;
  }


  for (HeI32 y=0; y<mHeight; y+=BLOCKSIZE)
  {
    for (HeI32 x=0; x<mWidth; x+=BLOCKSIZE)
    {
      SaveBlock(fname,iface,x,y);
    }
  }

  mNormals.clear();

}

//==================================================================================
void RtinObj::GetVertex(HeI32 index,HeF32 *vtx)
{
  HeI32 y = index/BLOCKLEN;
  HeI32 x = index-(y*BLOCKLEN);

  x+=mBaseX;
  y+=mBaseY;

  HeF32 z = Get(x,y);

  vtx[0] = HeF32(x);
  vtx[1] = HeF32(y);
  vtx[2] = z;
}

//==================================================================================
void RtinObj::GetPos(HeF32 *pos,HeI32 idx,HeU16 *vbuffer)
{
  // get world space representation from this lookup buffer.
  HeI32 zheight = vbuffer[idx*2];
  HeI32 index   = vbuffer[idx*2+1];
  HeI32 x = index%BLOCKLEN;
  HeI32 y = index/BLOCKLEN;
  pos[0] = HeF32(x);
  pos[1] = HeF32(y);
  #define ZRECIP 1.0f/16.0f
  pos[2] = HeF32(zheight)*ZRECIP;
}

//==================================================================================
void RtinObj::SaveBlock(const char *fname,GeometryInterface *iface,HeI32 basex,HeI32 basey)
{

  mBaseX = basex;
  mBaseY = basey;

  HeI32 gridx = basex/BLOCKSIZE;
  HeI32 gridy = basey/BLOCKSIZE;

  char meshname[512];
  sprintf(meshname,"%s%02d%02d", fname, gridx,gridy);
  iface->NodeMesh(meshname,0);


  HeI32 isize = BLOCKLEN*BLOCKLEN*2*3;
  HeI32 vsize = BLOCKLEN*BLOCKLEN;

  HeF32 *verts = MEMALLOC_NEW_ARRAY(float,vsize*3)[vsize*3]; // location of vertices.
  HeU16 *workspace = MEMALLOC_NEW_ARRAY(unsigned short,vsize)[vsize];
  memset(workspace,0xff,vsize*sizeof(HeI16));

  HeU16 *indices = MEMALLOC_NEW_ARRAY(unsigned short,isize)[isize];

  HeU16 *result = mRtin->BuildIndices(indices,mBaseX,mBaseY);

  HeI32 icount=0;
  if ( result ) icount = (HeI32)(result-indices);

  if ( icount )
  {
    HeI32 tcount = icount/3;
    HeI32 vcount = 0;

    HeU16 *index = indices;

    for (HeI32 i=0; i<tcount; i++)
    {
  	  HeI32 i1 = index[0];
  	  HeI32 i2 = index[1];
  	  HeI32 i3 = index[2];

      HeF32 p1[3];
      HeF32 p2[3];
      HeF32 p3[3];

      GetVertex(i1,p1);
      GetVertex(i2,p2);
      GetVertex(i3,p3);

      HeU16 idx1 = workspace[i1];
      HeU16 idx2 = workspace[i2];
      HeU16 idx3 = workspace[i3];

      if ( idx1 == 0xFFFF )
      {
        verts[vcount*3+0] = p1[0];
        verts[vcount*3+1] = p1[1];
        verts[vcount*3+2] = p1[2];

        idx1 = (HeU16)vcount;
        workspace[i1] = idx1;
        vcount++;
      }

      if ( idx2 == 0xFFFF )
      {
        verts[vcount*3+0] = p2[0];
        verts[vcount*3+1] = p2[1];
        verts[vcount*3+2] = p2[2];
        idx2 = (HeU16)vcount;
        workspace[i2] = idx2;
        vcount++;
      }

      if ( idx3 == 0xFFFF )
      {
        verts[vcount*3+0] = p3[0];
        verts[vcount*3+1] = p3[1];
        verts[vcount*3+2] = p3[2];
        idx3 = (HeU16)vcount;
        workspace[i3] = idx3;
        vcount++;
      }

      index[0] = idx1;
      index[1] = idx2;
      index[2] = idx3;

      index+=3;

    }


    mTotalVerts+=vcount;
    mTotalFaces+=tcount;

    {
	    HeF32 basex = (HeF32)mBaseX*mScale;
	    HeF32 basey = (HeF32)mBaseY*mScale;

      HeF32 translatex = 0;
      HeF32 translatey = 0;

	    if ( mAbsolute )
	    {
   	  	basex = basey = 0;
	    }
      else
      {
        if ( mOrigin )
        {
          HeF32 h = (HeF32) (mWidth/2)*mScale;
          translatex-=h;
          translatey-=h;
        }
      }

      {
        //char nodename[256];
        //sprintf(meshname,"%s%02d%02d", fname, gridx,gridy);
        iface->NodeMesh(meshname,0);

        MyMatrix m;
        if ( mZup )
        {
          m.SetTranslation(translatex+basex,translatey+basey,0);
        }
        else
        {
          m.SetTranslation(translatex+basex,0,translatey+basey);
        }

        {
        	char scratch[512];
#if MULTI_TEXTURE
          sprintf(scratch,"%s+%s", mTexture1, mTexture2 );
#else
          sprintf(scratch,"%s", mTexture2 );
#endif
          iface->NodeMaterial(scratch,0);
          //iface->NodeTransform(m.GetFloat(), nodename, 0 );
        }
      }

      GeometryVertex *gverts = MEMALLOC_NEW_ARRAY(GeometryVertex,vcount)[vcount];

      for (HeI32 i=0; i<vcount; i++)
      {
        HeF32 x = verts[i*3+0]*mScale-basex;
        HeF32 y = verts[i*3+1]*mScale-basey;
        HeF32 z = verts[i*3+2];

        if ( mOrigin )
        {
          if ( mAbsolute )
          {
            x+=mOffset;
            y+=mOffset;
          }
        }

        if (!mZup)
        {
          HeF32 t = y;
          y = z;
          z = t;
        }

        gverts[i].mPos[0] = x;
        gverts[i].mPos[1] = y;
        gverts[i].mPos[2] = z;

      }

      {
        for (HeI32 i=0; i<vcount; i++)
        {
          HeF32 x = verts[i*3+0];
          HeF32 y = verts[i*3+1];

          HeF32 normal[3];
          normal[0] = 0;
          normal[1] = 0;
          normal[2] = 1;

          HeI32 index = (HeI32)y*mWidth + (HeI32)x;
          MeanNormalMap::iterator found;
          found = mNormals.find(index);

          if ( found != mNormals.end() )
          {
            (*found).second.GetNormal(normal);
          }
          else
          {
//            assert(0); // this should never happen?
          }

          gverts[i].mNormal[0] = normal[0];
          gverts[i].mNormal[1] = normal[1];
          gverts[i].mNormal[2] = normal[2];

        }

      }

      for (HeI32 i=0; i<vcount; i++)
      {
#if MULTI_TEXTURE
        gverts[i].mTexel1[0] = verts[i*3+0]*mTexRecip*TILE_RATE;
        gverts[i].mTexel1[1] = verts[i*3+1]*mTexRecip*TILE_RATE;
#else
        gverts[i].mTexel2[0] = verts[i*3+0]*mTexRecip;
        gverts[i].mTexel2[1] = verts[i*3+1]*mTexRecip;
#endif
        gverts[i].mTexel2[0] = verts[i*3+0]*mTexRecip;
        gverts[i].mTexel2[1] = verts[i*3+1]*mTexRecip;

      }


      HeU16 *outindices = MEMALLOC_NEW_ARRAY(unsigned short,tcount*3)[tcount*3];


      HeU16 *dst = indices;
      for (HeI32 i=0; i<tcount; i++)
      {
        if ( mZup )
        {
          outindices[i*3+0] = dst[2];
          outindices[i*3+1] = dst[1];
          outindices[i*3+2] = dst[0];
        }
        else
        {
          outindices[i*3+0] = dst[0];
          outindices[i*3+1] = dst[1];
          outindices[i*3+2] = dst[2];
        }
        dst+=3;
      }

      iface->NodeTriangleList(vcount,gverts,tcount*3,outindices);

      delete []outindices;
      delete []gverts;
    }
  }

  delete indices;
  delete workspace;
  delete verts;
}

//==================================================================================
HeI32 RtinObj::Translate(HeF32 *p)
{
  HeI32   x = (HeI32)p[0];
  HeI32   y = (HeI32)p[1];
  HeI32 ret = (y*mWidth)+x;

  if ( mZup )
  {
    p[0]*=mScale;
    p[1]*=mScale;
  }
  else
  {
    HeF32 y = p[1];
    p[0]*=mScale;
    p[1] = p[2];
    p[2] = y*mScale;
  }
  return ret;
}

//==================================================================================
void RtinObj::AddNormal(HeI32 index,const HeF32 *normal)
{
  MeanNormalMap::iterator found;

  found = mNormals.find(index); // see if we already have a shared vertex here.
  if ( found == mNormals.end() )
  {
    MeanNormal mn;
    mn.Add(normal);
    mNormals[index] = mn;
  }
  else
  {
    (*found).second.Add(normal);
  }
}
