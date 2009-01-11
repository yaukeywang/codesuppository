#ifndef RTINOBJ_H

#define RTINOBJ_H


#include "common/snippets/HeSimpleTypes.h"
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

  void Add(const HeF32 *n)
  {
    mCount++;
    mNormal[0]+=n[0];
    mNormal[1]+=n[1];
    mNormal[2]+=n[2];
  }

  const HeF32 * GetNormal(void) const { return mNormal; };

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
          HeF32 recip = 1.0f / (HeF32) mCount;
          mNormal[0]*=recip;
          mNormal[1]*=recip;
          mNormal[2]*=recip;
        }
        break;
    }
    // ok now need to re-normalize it.
    HeF32 d = sqrtf( mNormal[0] * mNormal[0] + mNormal[1]*mNormal[1] + mNormal[2]*mNormal[2] );
    HeF32 r = 1.0f / d; // compute reciprocol
    mNormal[0]*=r;
    mNormal[1]*=r;
    mNormal[2]*=r;
  }

  void GetNormal(HeF32 *n)
  {
    n[0] = mNormal[0];
    n[1] = mNormal[1];
    n[2] = mNormal[2];
  }


  HeI32   mCount;
  HeF32 mNormal[3]; // the accumulated mean vector normal.
};

typedef std::map< HeI32 , MeanNormal > MeanNormalMap; // lookup table for mean vector normals.

class TerrainVertex
{
public:
  HeF32  mPos[3];
  HeF32  mNormal[3];
  HeF32  mTexel[2];
};

// converts a heightfield into a right-triangular irregular network.
class RtinObj : public HeightData
{
public:
  RtinObj(HeI32 width,HeI32 height,const HeF32 *data);
  ~RtinObj(void);


  void CreateRtin(const char *name,
                  HeI32  err_thresh,
                  bool zup,
                  HeF32 scale,
                  GeometryInterface *iface,
                  const char *texture1,
                  const char *texture2,
                  HeF32 water,
                  bool absolute,
                  bool origin);


  virtual HeI32 GetWidth(void) const { return mWidth; };
  virtual HeI32 GetHeight(void) const { return mHeight; };

	virtual HeF32 Get(HeI32 x,HeI32 y) const
  {
	  if ( x < 0 ) x = 0;
  	if ( x >= mWidth ) x = mWidth-1;
	  if ( y < 0 ) y = 0;
  	if ( y >= mHeight ) y = mHeight-1;
    assert( mData );
    return mData[y*mWidth+x];
  };

  void SaveBlock(const char *fname,GeometryInterface *iface,HeI32 x,HeI32 y);

  void GetVertex(HeI32 index,HeF32 *vtx);

private:
  void GetPos(HeF32 *pos,HeI32 idx,HeU16 *vbuffer);

  HeI32 Translate(HeF32 *p); // translate into co-ordinate space and return table index.
  void AddNormal(HeI32 index,const HeF32 *normal);

  HeI32             mBaseX;
  HeI32             mBaseY;
  HeI32             mWidth;
  HeI32             mHeight;
  const HeF32    *mData; // heightfield data.
  TopoRtin       *mRtin;
  HeI32             mTotalVerts;
  HeI32             mTotalFaces;
  HeF32           mTexRecip;
  ErrorHeuristic *mError;
  HeI32             mErrThresh;
  bool            mZup;
  HeF32           mScale;
  bool            mVrml2;
  char            mTexture1[512];
  char            mTexture2[512];
  HeF32           mWater;
  MeanNormalMap   mNormals;
  bool            mAbsolute;
  bool            mOrigin;
  HeF32           mOffset;
};

#endif
