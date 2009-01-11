#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "SmoothPath.h"
#include "../HeMath/HeFoundation.h"
#include <vector>

class BsplineNode
{
public:
  HeF32 x;              // 4 time/distance x-axis component.
  HeF32 y;              // 8 y component.
  HeF32 u;              // 12
  HeF32 p;              // 16
  HeF32 d;              // 20
  HeF32 w;              // 24
  HeF32 inverseU;       // 28
  HeF32 inverseD;       // 32
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

  void Reserve(HeU32 size)
  {
    mCurrent = 0;
    mSize = size;
    mIndex = 0;
    mNodes = MEMALLOC_NEW_ARRAY(BsplineNode,size)[size];
  };

  HeU32 GetSize(void)
  {
    return mIndex;
  };

  HeF32 GetEntry(HeU32 i)
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

  HeF32 f(HeF32 x)
  {
	  return( x*x*x - x);
  }

  // build spline.
  // copied from Sedgwick's Algorithm's in C, page 548.
  void ComputeBspline(void)
  {

    {

    HeU32 n = mIndex;

  	for (HeU32 i=1; i<(n-1); i++)
  	{
  		mNodes[i].d = 2.0f * (mNodes[i+1].x - mNodes[i-1].x);
      mNodes[i].inverseD = 1.0f / mNodes[i].d;
  	}

  	for (HeU32 i=0; i<(n-1); i++)
    {
  	  mNodes[i].u = mNodes[i+1].x-mNodes[i].x;
      mNodes[i].inverseU = 1.0f / mNodes[i].u;
    }

  	for (HeU32 i=1; i<(n-1); i++)
    {
  		mNodes[i].w = 6.0f*((mNodes[i+1].y - mNodes[i].y)   / mNodes[i].u - (mNodes[i].y   - mNodes[i-1].y) / mNodes[i-1].u);
    }

  	mNodes[0].p   = 0.0f;
  	mNodes[n-1].p = 0.0f;

  	for (HeU32 i=1; i<(n-2); i++)
  	{
  		mNodes[i+1].w = mNodes[i+1].w - mNodes[i].w*mNodes[i].u /mNodes[i].d;
  		mNodes[i+1].d = mNodes[i+1].d - mNodes[i].u * mNodes[i].u / mNodes[i].d;
  	}

  	for (HeU32 i=n-2; i>0; i--)
    {
  		mNodes[i].p = (mNodes[i].w - mNodes[i].u * mNodes[i+1].p ) / mNodes[i].d;
    }
    }
  }

  HeF32 setup(HeF32 v,HeU32 &segment)
  {

  	HeF32 t=0;

    HeU32 n = mIndex;

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
    	HeU32 i=0;
  	  while ( v > mNodes[i+1].x && i < (n-1) ) i++;
  	  t = (v - mNodes[i].x ) * mNodes[i].inverseU;
      segment = i;
      mCurrent = i;
    }

    return t;
  }

  HeF32 Evaluate(HeF32 t,HeU32 i)
  {
  	return( t*mNodes[i+1].y + (1-t)*mNodes[i].y +	mNodes[i].u * mNodes[i].u * (f(t)*mNodes[i+1].p +	f(1-t)*mNodes[i].p )* (1.0f/6.0f) );
  }

  HeF32 EvaluateLinear(HeF32 t,HeU32 i)
  {
    float y1 = mNodes[i].y;
    float y2 = mNodes[i+1].y;

    return (y2-y1)*t+y1;

  }


  void AddNode(HeF32 x,HeF32 y)
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
  HeU32        mIndex;
  HeU32        mSize;
  HeU32        mCurrent;
  BsplineNode *mNodes;
};





class SmoothPath
{
public:

  SmoothPath(const HeF32 *input_points,HeU32 pstride,HeU32 count,HeF32 &length)
  {
    mCount = count;

    mXaxis.Reserve(count);
    mYaxis.Reserve(count);
    mZaxis.Reserve(count);

    mTime = 0;

    HeVec3 previous;

    const char *scan = (const char *) input_points;
    for (HeU32 i=0; i<count; i++)
    {
      HeVec3 p( (const HeF32 *) scan );

      if ( i )
      {
        HeF32 dist = previous.distance(p);
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


  HeF32 GetLength(void) { return mTime; }; //total length of spline

  HeF32 Evaluate(HeF32 dist,HeF32 *pos,HeU32 &segment,bool linear)
  {
    HeF32 t;

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
  HeF32  mTime; // time/distance travelled.
  HeU32     mCount;
  Bspline mXaxis;
  Bspline mYaxis;
  Bspline mZaxis;
};


SmoothPath  * sp_createSmoothPath(const HeF32 *points,HeU32 pstride,HeU32 count,HeF32 &length)
{
  assert( count >= 2 );
  SmoothPath *sp = MEMALLOC_NEW(SmoothPath)(points,pstride,count,length);
  return sp;
}

HeF32 sp_getPoint(SmoothPath *sp,HeF32 *dest,HeF32 ftime,HeU32 &index)
{
  return sp->Evaluate(ftime,dest,index,false);
}

HeF32         sp_getPointLinear(SmoothPath *sp,HeF32 *dest,HeF32 ftime,HeU32 &index)
{
  return sp->Evaluate(ftime,dest,index,true);
}

void          sp_releaseSmoothPath(SmoothPath *sp)
{
  delete sp;
}

