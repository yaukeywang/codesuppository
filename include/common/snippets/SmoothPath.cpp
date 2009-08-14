#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "SmoothPath.h"
#include "NxFoundation.h"
#include <vector>

class BsplineNode
{
public:
  NxF32 x;              // 4 time/distance x-axis component.
  NxF32 y;              // 8 y component.
  NxF32 u;              // 12
  NxF32 p;              // 16
  NxF32 d;              // 20
  NxF32 w;              // 24
  NxF32 inverseU;       // 28
  NxF32 inverseD;       // 32
};

typedef USER_STL::vector< BsplineNode > BsplineNodeVector;

class Bspline
{
public:
  Bspline(void)
  {
    mNodes = 0;
    mIndex = 0;
    mSize  = 0;
    mCurrent = 0;
  }

  ~Bspline(void)
  {
    delete []mNodes;
  }

  void Reserve(NxU32 size)
  {
    mCurrent = 0;
    mSize = size;
    mIndex = 0;
    mNodes = MEMALLOC_NEW_ARRAY(BsplineNode,size)[size];
  };

  NxU32 GetSize(void)
  {
    return mIndex;
  };

  NxF32 GetEntry(NxU32 i)
  {
    return mNodes[i].y;
  }

  void Clear(void)
  {
    mCurrent = 0;
    mIndex = 0;
    mSize  = 0;
    delete []mNodes;
    mNodes = 0;
  };

  NxF32 f(NxF32 x)
  {
	  return( x*x*x - x);
  }

  // build spline.
  // copied from Sedgwick's Algorithm's in C, page 548.
  void ComputeBspline(void)
  {

    {

    NxU32 n = mIndex;

  	for (NxU32 i=1; i<(n-1); i++)
  	{
  		mNodes[i].d = 2.0f * (mNodes[i+1].x - mNodes[i-1].x);
      mNodes[i].inverseD = 1.0f / mNodes[i].d;
  	}

  	for (NxU32 i=0; i<(n-1); i++)
    {
  	  mNodes[i].u = mNodes[i+1].x-mNodes[i].x;
      mNodes[i].inverseU = 1.0f / mNodes[i].u;
    }

  	for (NxU32 i=1; i<(n-1); i++)
    {
  		mNodes[i].w = 6.0f*((mNodes[i+1].y - mNodes[i].y)   / mNodes[i].u - (mNodes[i].y   - mNodes[i-1].y) / mNodes[i-1].u);
    }

  	mNodes[0].p   = 0.0f;
  	mNodes[n-1].p = 0.0f;

  	for (NxU32 i=1; i<(n-2); i++)
  	{
  		mNodes[i+1].w = mNodes[i+1].w - mNodes[i].w*mNodes[i].u /mNodes[i].d;
  		mNodes[i+1].d = mNodes[i+1].d - mNodes[i].u * mNodes[i].u / mNodes[i].d;
  	}

  	for (NxU32 i=n-2; i>0; i--)
    {
  		mNodes[i].p = (mNodes[i].w - mNodes[i].u * mNodes[i+1].p ) / mNodes[i].d;
    }
    }
  }

  NxF32 setup(NxF32 v,NxU32 &segment)
  {

  	NxF32 t=0;

    NxU32 n = mIndex;

    bool found = false;

    if ( v >= mNodes[mCurrent].x && v < mNodes[mCurrent+1].x )
    {
      segment = mCurrent;
  	  t = (v - mNodes[mCurrent].x ) * mNodes[mCurrent].inverseU;
      found = true;
    }
    else
    {
      mCurrent++;
      if ( mCurrent < (n-1) )
      {
        if ( v >= mNodes[mCurrent].x && v < mNodes[mCurrent+1].x )
        {
          segment = mCurrent;
      	  t = (v - mNodes[mCurrent].x ) * mNodes[mCurrent].inverseU;
          found = true;
        }
      }
    }

    if ( !found )
    {
    	NxU32 i=0;
  	  while ( v > mNodes[i+1].x && i < (n-1) ) i++;
  	  t = (v - mNodes[i].x ) * mNodes[i].inverseU;
      segment = i;
      mCurrent = i;
    }

    return t;
  }

  NxF32 Evaluate(NxF32 t,NxU32 i)
  {
  	return( t*mNodes[i+1].y + (1-t)*mNodes[i].y +	mNodes[i].u * mNodes[i].u * (f(t)*mNodes[i+1].p +	f(1-t)*mNodes[i].p )* (1.0f/6.0f) );
  }

  NxF32 EvaluateLinear(NxF32 t,NxU32 i)
  {
    NxF32 y1 = mNodes[i].y;
    NxF32 y2 = mNodes[i+1].y;

    return (y2-y1)*t+y1;

  }


  void AddNode(NxF32 x,NxF32 y)
  {
    assert( mIndex < mSize );
    if ( mIndex < mSize )
    {
      mNodes[mIndex].x = x;
      mNodes[mIndex].y = y;
      mIndex++;
    }
  }

private:
  NxU32        mIndex;
  NxU32        mSize;
  NxU32        mCurrent;
  BsplineNode *mNodes;
};





class SmoothPath
{
public:

  SmoothPath(const NxF32 *input_points,NxU32 pstride,NxU32 count,NxF32 &length)
  {
    mCount = count;

    mXaxis.Reserve(count);
    mYaxis.Reserve(count);
    mZaxis.Reserve(count);

    mTime = 0;

    NxVec3 previous;

    const char *scan = (const char *) input_points;
    for (NxU32 i=0; i<count; i++)
    {
      NxVec3 p( (const NxF32 *) scan );

      if ( i )
      {
        NxF32 dist = previous.distance(p);
        mTime+=dist;
      }

      mXaxis.AddNode(mTime,p.x);
      mYaxis.AddNode(mTime,p.y);
      mZaxis.AddNode(mTime,p.z);

      previous = p;
      scan+=pstride;
    }

    mXaxis.ComputeBspline();
    mYaxis.ComputeBspline();
    mZaxis.ComputeBspline();

    length = mTime;
  }

  ~SmoothPath(void)
  {
  }


  NxF32 GetLength(void) { return mTime; }; //total length of spline

  NxF32 Evaluate(NxF32 dist,NxF32 *pos,NxU32 &segment,bool linear)
  {
    NxF32 t;

    if ( mCount == 2 || linear )
    {
      t = mXaxis.setup(dist,segment);

      pos[0] = mXaxis.EvaluateLinear(t,segment);
      pos[1] = mYaxis.EvaluateLinear(t,segment);
      pos[2] = mZaxis.EvaluateLinear(t,segment);

    }
    else
    {

      t = mXaxis.setup(dist,segment);

    pos[0] = mXaxis.Evaluate(t,segment);
    pos[1] = mYaxis.Evaluate(t,segment);
    pos[2] = mZaxis.Evaluate(t,segment);

    }

    return t;
  }

private:
  NxF32  mTime; // time/distance travelled.
  NxU32     mCount;
  Bspline mXaxis;
  Bspline mYaxis;
  Bspline mZaxis;
};


SmoothPath  * sp_createSmoothPath(const NxF32 *points,NxU32 pstride,NxU32 count,NxF32 &length)
{
  assert( count >= 2 );
  SmoothPath *sp = MEMALLOC_NEW(SmoothPath)(points,pstride,count,length);
  return sp;
}

NxF32 sp_getPoint(SmoothPath *sp,NxF32 *dest,NxF32 ftime,NxU32 &index)
{
  return sp->Evaluate(ftime,dest,index,false);
}

NxF32         sp_getPointLinear(SmoothPath *sp,NxF32 *dest,NxF32 ftime,NxU32 &index)
{
  return sp->Evaluate(ftime,dest,index,true);
}

void          sp_releaseSmoothPath(SmoothPath *sp)
{
  delete sp;
}

