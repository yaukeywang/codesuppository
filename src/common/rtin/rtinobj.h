#ifndef RTINOBJ_H

#define RTINOBJ_H


#include "common/snippets/UserMemAlloc.h"
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



/** @file rtinobj.h
 *  @brief A wrapper class to manage the terrain tessalation process.
 *
 *  @author John W. Ratcliff
*/

/** @file rtinobj.cpp
 *  @brief A wrapper class to manage the terrain tessalation process.
 *
 *  @author John W. Ratcliff
*/


#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "common/snippets/UserMemAlloc.h"
#include "toportin.h"


#include "common/snippets/Geometry.h"

class TopoRtin;
class ErrorHeuristic;

class MeanNormal
{
public:
  MeanNormal(void)
  {
    mCount = 0;
    mNormal[0] = 0;
    mNormal[1] = 0;
    mNormal[2] = 0;
  }

  void Add(const NxF32 *n)
  {
    mCount++;
    mNormal[0]+=n[0];
    mNormal[1]+=n[1];
    mNormal[2]+=n[2];
  }

  const NxF32 * GetNormal(void) const { return mNormal; };

  void ComputeMean(void)
  {
    switch ( mCount )
    {
      case 0:
      case 1:
        break;
      case 2:
        mNormal[0]*=0.5f;
        mNormal[1]*=0.5f;
        mNormal[2]*=0.5f;
        break;
      default:
        {
          NxF32 recip = 1.0f / (NxF32) mCount;
          mNormal[0]*=recip;
          mNormal[1]*=recip;
          mNormal[2]*=recip;
        }
        break;
    }
    // ok now need to re-normalize it.
    NxF32 d = sqrtf( mNormal[0] * mNormal[0] + mNormal[1]*mNormal[1] + mNormal[2]*mNormal[2] );
    NxF32 r = 1.0f / d; // compute reciprocol
    mNormal[0]*=r;
    mNormal[1]*=r;
    mNormal[2]*=r;
  }

  void GetNormal(NxF32 *n)
  {
    n[0] = mNormal[0];
    n[1] = mNormal[1];
    n[2] = mNormal[2];
  }


  NxI32   mCount;
  NxF32 mNormal[3]; // the accumulated mean vector normal.
};

typedef USER_STL::map< NxI32 , MeanNormal > MeanNormalMap; // lookup table for mean vector normals.

class TerrainVertex
{
public:
  NxF32  mPos[3];
  NxF32  mNormal[3];
  NxF32  mTexel[2];
};

// converts a heightfield into a right-triangular irregular network.
class RtinObj : public HeightData
{
public:
  RtinObj(NxI32 width,NxI32 height,const NxF32 *data);
  ~RtinObj(void);


  void CreateRtin(const char *name,
                  NxI32  err_thresh,
                  bool zup,
                  NxF32 scale,
                  GeometryInterface *iface,
                  const char *texture1,
                  const char *texture2,
                  NxF32 water,
                  bool absolute,
                  bool origin);


  virtual NxI32 GetWidth(void) const { return mWidth; };
  virtual NxI32 GetHeight(void) const { return mHeight; };

	virtual NxF32 Get(NxI32 x,NxI32 y) const
  {
	  if ( x < 0 ) x = 0;
  	if ( x >= mWidth ) x = mWidth-1;
	  if ( y < 0 ) y = 0;
  	if ( y >= mHeight ) y = mHeight-1;
    assert( mData );
    return mData[y*mWidth+x];
  };

  void SaveBlock(const char *fname,GeometryInterface *iface,NxI32 x,NxI32 y);

  void GetVertex(NxI32 index,NxF32 *vtx);

private:
  void GetPos(NxF32 *pos,NxI32 idx,NxU16 *vbuffer);

  NxI32 Translate(NxF32 *p); // translate into co-ordinate space and return table index.
  void AddNormal(NxI32 index,const NxF32 *normal);

  NxI32             mBaseX;
  NxI32             mBaseY;
  NxI32             mWidth;
  NxI32             mHeight;
  const NxF32    *mData; // heightfield data.
  TopoRtin       *mRtin;
  NxI32             mTotalVerts;
  NxI32             mTotalFaces;
  NxF32           mTexRecip;
  ErrorHeuristic *mError;
  NxI32             mErrThresh;
  bool            mZup;
  NxF32           mScale;
  bool            mVrml2;
  char            mTexture1[512];
  char            mTexture2[512];
  NxF32           mWater;
  MeanNormalMap   mNormals;
  bool            mAbsolute;
  bool            mOrigin;
  NxF32           mOffset;
};

#endif
