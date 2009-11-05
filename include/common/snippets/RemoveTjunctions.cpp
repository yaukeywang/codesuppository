#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#pragma warning(disable:4702)
#pragma warning(disable:4127) //conditional expression is constant (because _HAS_EXCEPTIONS=0)
#include <vector>
#include <hash_map>
#include "UserMemAlloc.h"
#include "NvArray.h"
#include "NvHashMap.h"
#include "RemoveTjunctions.h"
#include "FloatMath.h"

#pragma warning(disable:4189)

using namespace NVSHARE;

namespace REMOVE_TJUNCTIONS
{

class AABB
{
public:
  NxF32 mMin[3];
  NxF32 mMax[3];
};

bool gDebug=false;
NxU32 gCount=0;

typedef NVSHARE::Array< NxU32 > NxU32Vector;

class Triangle
{
public:
  Triangle(void)
  {
    mPending = false;
    mSplit = false;
    mI1 = mI2 = mI3 = 0xFFFFFFFF;
    mId = 0;
  }

  Triangle(NxU32 i1,NxU32 i2,NxU32 i3,const float *vertices,NxU32 id)
  {
    mPending = false;
    init(i1,i2,i3,vertices,id);
    mSplit = false;
  }

  void init(NxU32 i1,NxU32 i2,NxU32 i3,const float *vertices,NxU32 id)
  {
    mSplit = false;
    mI1 = i1;
    mI2 = i2;
    mI3 = i3;
    mId = id;

    const float *p1 = &vertices[mI1*3];
    const float *p2 = &vertices[mI2*3];
    const float *p3 = &vertices[mI3*3];

    initMinMax(p1,p2,p3);
  }

  void initMinMax(const float *p1,const float *p2,const float *p3)
  {
    fm_copy3(p1,mBmin);
    fm_copy3(p1,mBmax);
    fm_minmax(p2,mBmin,mBmax);
    fm_minmax(p3,mBmin,mBmax);
  }

  void init(const NxU32 *idx,const float *vertices,NxU32 id)
  {
    mSplit = false;
    mI1 = idx[0];
    mI2 = idx[1];
    mI3 = idx[2];
    mId = id;

    const float *p1 = &vertices[mI1*3];
    const float *p2 = &vertices[mI2*3];
    const float *p3 = &vertices[mI3*3];

    initMinMax(p1,p2,p3);

  }

  bool intersects(const float *pos,const float *p1,const float *p2,float epsilon) const
  {
    bool ret = false;

    float sect[3];
    LineSegmentType type;

    float dist = fm_distancePointLineSegment(pos,p1,p2,sect,type,epsilon);

    if ( type == LS_MIDDLE && dist < epsilon )
    {
      ret = true;
    }

    return ret;
  }

  bool intersects(NxU32 i,const float *vertices,NxU32 &edge,float epsilon) const
  {
    bool ret = true;

    const float *pos = &vertices[i*3];
    const float *p1  = &vertices[mI1*3];
    const float *p2  = &vertices[mI2*3];
    const float *p3  = &vertices[mI3*3];
    if ( intersects(pos,p1,p2,epsilon) )
    {
      edge = 0;
    }
    else if ( intersects(pos,p2,p3,epsilon) )
    {
      edge = 1;
    }
    else if ( intersects(pos,p3,p1,epsilon) )
    {
      edge = 2;
    }
    else
    {
      ret = false;
    }
    return ret;
  }

  bool intersects(const Triangle *t,const float *vertices,NxU32 &intersection_index,NxU32 &edge,float epsilon)
  {
    bool ret = false;

    if ( fm_intersectAABB(mBmin,mBmax,t->mBmin,t->mBmax) ) // only if the AABB's of the two triangles intersect...
    {

      if ( t->intersects(mI1,vertices,edge,epsilon) )
      {
        intersection_index = mI1;
        ret = true;
      }

      if ( t->intersects(mI2,vertices,edge,epsilon) )
      {
        intersection_index = mI2;
        ret = true;
      }

      if ( t->intersects(mI3,vertices,edge,epsilon) )
      {
        intersection_index = mI3;
        ret = true;
      }

    }

    return ret;
  }

  bool    mSplit:1;
  bool    mPending:1;
  NxU32   mI1;
  NxU32   mI2;
  NxU32   mI3;
  NxU32   mId;
  float   mBmin[3];
  float   mBmax[3];
};

class Edge
{
public:
  Edge(void)
  {
    mNextEdge = 0;
    mTriangle = 0;
    mHash = 0;
  }

  NxU32 init(Triangle *t,NxU32 i1,NxU32 i2)
  {
    mTriangle = t;
    mNextEdge = 0;
    NX_ASSERT( i1 < 65536 );
    NX_ASSERT( i2 < 65536 );
    if ( i1 < i2 )
    {
      mHash = (i1<<16)|i2;
    }
    else
    {
      mHash = (i2<<16)|i1;
    }
    return mHash;
  }
  Edge     *mNextEdge;
  Triangle *mTriangle;
  NxU32    mHash;
};


typedef NVSHARE::Array< Triangle * > TriangleVector;
typedef NVSHARE::HashMap< NxU32, Edge * > EdgeMap;

class MyRemoveTjunctions : public RemoveTjunctions
{
public:
  MyRemoveTjunctions(void)
  {
    mInputTriangles = 0;
    mEdges = 0;
    mVcount = 0;
    mVertices = 0;
    mEdgeCount = 0;
  }
  ~MyRemoveTjunctions(void)
  {
    release();
  }

  virtual NxU32 removeTjunctions(RemoveTjunctionsDesc &desc)
  {
    NxU32 ret = 0;

	mEpsilon = desc.mEpsilon;

	size_t TcountOut;

    desc.mIndicesOut = removeTjunctions(desc.mVcount, desc.mVertices, desc.mTcount, desc.mIndices, TcountOut, desc.mIds);

#ifdef WIN32
#	pragma warning(push)
#	pragma warning(disable:4267)
#endif

	NX_ASSERT( TcountOut < UINT_MAX );
	desc.mTcountOut = TcountOut;

#ifdef WIN32
#	pragma warning(pop)
#endif

    if ( !mIds.empty() )
    {
      desc.mIdsOut = &mIds[0];
    }

    ret = desc.mTcountOut;

    bool check = ret != desc.mTcount;
#if 0
    while ( check )
    {
        NxU32 tcount = ret;
        NxU32 *indices  = new NxU32[tcount*3];
        NxU32 *ids      = new NxU32[tcount];
        memcpy(indices,desc.mIndicesOut,sizeof(NxU32)*ret*3);
        memcpy(ids,desc.mIdsOut,sizeof(NxU32)*ret);
        desc.mIndicesOut = removeTjunctions(desc.mVcount, desc.mVertices, tcount, indices, desc.mTcountOut, ids );
        if ( !mIds.empty() )
        {
          desc.mIdsOut = &mIds[0];
        }
        ret = desc.mTcountOut;
        delete []indices;
        delete []ids;
        check = ret != tcount;
    }
#endif
    return ret;
  }

  Edge * addEdge(Triangle *t,Edge *e,NxU32 i1,NxU32 i2)
  {
    NxU32 hash = e->init(t,i1,i2);
    const EdgeMap::Entry *found = mEdgeMap.find(hash);
    if ( found == NULL )
    {
      mEdgeMap[hash] = e;
    }
    else
    {
      Edge *old_edge = (*found).second;
      e->mNextEdge = old_edge;
      mEdgeMap.erase(hash);
      mEdgeMap[hash] = e;
    }
    e++;
    mEdgeCount++;
    return e;
  }

  Edge * init(Triangle *t,const NxU32 *indices,const float *vertices,Edge *e,NxU32 id)
  {
    t->init(indices,vertices,id);
    e = addEdge(t,e,t->mI1,t->mI2);
    e = addEdge(t,e,t->mI2,t->mI3);
    e = addEdge(t,e,t->mI3,t->mI1);
    return e;
  }

  void release(void)
  {
    mIds.clear();
    mEdgeMap.clear();
    mIndices.clear();
    mSplit.clear();
    delete []mInputTriangles;
    delete []mEdges;
    mInputTriangles = 0;
    mEdges = 0;
    mVcount = 0;
    mVertices = 0;
    mEdgeCount = 0;

  }

  virtual NxU32 * removeTjunctions(NxU32 vcount,
                                    const float *vertices,
                                    size_t tcount,
                                    const NxU32 *indices,
                                    size_t &tcount_out,
                                    const NxU32 * ids)
  {
    NxU32 *ret  = 0;

    release();

    mVcount   = vcount;
    mVertices = vertices;
    mTcount   = (NxU32)tcount;
    tcount_out = 0;

    mTcount         = (NxU32)tcount;
    mMaxTcount      = (NxU32)tcount*2;
    mInputTriangles = new Triangle[mMaxTcount];
    Triangle *t     = mInputTriangles;

    mEdges          = new Edge[mMaxTcount*3];
    mEdgeCount      = 0;

    NxU32 id = 0;

    Edge *e = mEdges;
    for (NxU32 i=0; i<tcount; i++)
    {
      if ( ids ) id = *ids++;
      e =init(t,indices,vertices,e,id);
      indices+=3;
      t++;
    }

    {
      TriangleVector test;

      EdgeMap::Iterator i = mEdgeMap.getIterator();
      for (; !i.done(); ++i)
      {
        Edge *e = (*i).second;
        if ( e->mNextEdge == 0 ) // open edge!
        {
          Triangle *t = e->mTriangle;
          if ( !t->mPending )
          {
            test.pushBack(t);
            t->mPending = true;
          }
        }
      }

      if ( !test.empty() )
      {
        TriangleVector::Iterator i;
        for (i=test.begin(); i!=test.end(); ++i)
        {
          Triangle *t = (*i);
          locateIntersection(t);
        }
      }

    }

    while ( !mSplit.empty() )
    {
      TriangleVector scan = mSplit;
      mSplit.clear();
      TriangleVector::Iterator i;
      for (i=scan.begin(); i!=scan.end(); ++i)
      {
        Triangle *t = (*i);
        locateIntersection(t);
      }
    }


    mIndices.clear();
    mIds.clear();

    t = mInputTriangles;
    for (NxU32 i=0; i<mTcount; i++)
    {
      mIndices.pushBack(t->mI1);
      mIndices.pushBack(t->mI2);
      mIndices.pushBack(t->mI3);
      mIds.pushBack(t->mId);
      t++;
    }


   mEdgeMap.clear();
   delete []mEdges;
   mEdges = 0;
   delete []mInputTriangles;
   mInputTriangles = 0;
   tcount_out = mIndices.size()/3;
   ret = tcount_out ? &mIndices[0] : 0;
#ifdef _DEBUG
   if ( ret )
   {
	   const NxU32 *scan = ret;
	   for (NxU32 i=0; i<tcount_out; i++)
	   {
		   NxU32 i1 = scan[0];
		   NxU32 i2 = scan[1];
		   NxU32 i3 = scan[2];
		   assert( i1 != i2 && i1 != i3 && i2 != i3 );
		   scan+=3;
	   }
   }
#endif
    return ret;
  }

  Triangle * locateIntersection(Triangle *scan,Triangle *t)
  {
    Triangle *ret = 0;

    NxU32 t1 = (NxU32)(scan-mInputTriangles);
    NxU32 t2 = (NxU32)(t-mInputTriangles);

    NX_ASSERT( t1 < mTcount );
    NX_ASSERT( t2 < mTcount );

    NX_ASSERT( scan->mI1 < mVcount );
    NX_ASSERT( scan->mI2 < mVcount );
    NX_ASSERT( scan->mI3 < mVcount );

    NX_ASSERT( t->mI1 < mVcount );
    NX_ASSERT( t->mI2 < mVcount );
    NX_ASSERT( t->mI3 < mVcount );


    NxU32 intersection_index;
    NxU32 edge;

    if ( scan != t && scan->intersects(t,mVertices,intersection_index,edge,mEpsilon) )
    {

	  if ( t->mI1 == intersection_index || t->mI2 == intersection_index || t->mI3 == intersection_index )
	  {
	  }
	  else
	  {
		  // here is where it intersects!
		  NxU32 i1,i2,i3;
		  NxU32 j1,j2,j3;
		  NxU32 id = t->mId;

		  switch ( edge )
		  {
			case 0:
			  i1 = t->mI1;
			  i2 = intersection_index;
			  i3 = t->mI3;
			  j1 = intersection_index;
			  j2 = t->mI2;
			  j3 = t->mI3;
			  break;
			case 1:
			  i1 = t->mI2;
			  i2 = intersection_index;
			  i3 = t->mI1;
			  j1 = intersection_index;
			  j2 = t->mI3;
			  j3 = t->mI1;
			  break;
			case 2:
			  i1 = t->mI3;
			  i2 = intersection_index;
			  i3 = t->mI2;
			  j1 = intersection_index;
			  j2 = t->mI1;
			  j3 = t->mI2;
			  break;
			default:
			  NX_ASSERT(0);
			  i1 = i2 = i3 = 0;
			  j1 = j2 = j3 = 0;
			  break;
		  }

		  if ( mTcount < mMaxTcount )
		  {
			t->init(i1,i2,i3,mVertices,id);
			Triangle *newt = &mInputTriangles[mTcount];
			newt->init(j1,j2,j3,mVertices,id);
			mTcount++;
			t->mSplit = true;
			newt->mSplit = true;

			mSplit.pushBack(t);
			mSplit.pushBack(newt);
			ret = scan;
		  }
	    }
    }
    return ret;
  }

  Triangle * testIntersection(Triangle *scan,Triangle *t)
  {
    Triangle *ret = 0;

    NxU32 t1 = (NxU32)(scan-mInputTriangles);
    NxU32 t2 = (NxU32)(t-mInputTriangles);

    NX_ASSERT( t1 < mTcount );
    NX_ASSERT( t2 < mTcount );

    NX_ASSERT( scan->mI1 < mVcount );
    NX_ASSERT( scan->mI2 < mVcount );
    NX_ASSERT( scan->mI3 < mVcount );

    NX_ASSERT( t->mI1 < mVcount );
    NX_ASSERT( t->mI2 < mVcount );
    NX_ASSERT( t->mI3 < mVcount );


    NxU32 intersection_index;
    NxU32 edge;

    assert( scan != t );

    if ( scan->intersects(t,mVertices,intersection_index,edge,mEpsilon) )
    {
      // here is where it intersects!
      NxU32 i1,i2,i3;
      NxU32 j1,j2,j3;
      NxU32 id = t->mId;

      switch ( edge )
      {
        case 0:
          i1 = t->mI1;
          i2 = intersection_index;
          i3 = t->mI3;
          j1 = intersection_index;
          j2 = t->mI2;
          j3 = t->mI3;
          break;
        case 1:
          i1 = t->mI2;
          i2 = intersection_index;
          i3 = t->mI1;
          j1 = intersection_index;
          j2 = t->mI3;
          j3 = t->mI1;
          break;
        case 2:
          i1 = t->mI3;
          i2 = intersection_index;
          i3 = t->mI2;
          j1 = intersection_index;
          j2 = t->mI1;
          j3 = t->mI2;
          break;
        default:
          NX_ASSERT(0);
          i1 = i2 = i3 = 0;
          j1 = j2 = j3 = 0;
          break;
      }

      if ( mTcount < mMaxTcount )
      {
        t->init(i1,i2,i3,mVertices,id);
        Triangle *newt = &mInputTriangles[mTcount];
        newt->init(j1,j2,j3,mVertices,id);
        mTcount++;
        t->mSplit = true;
        newt->mSplit = true;

        mSplit.pushBack(t);
        mSplit.pushBack(newt);
        ret = scan;
      }
    }
    return ret;
  }

  Triangle * locateIntersection(Triangle *t)
  {
    Triangle *ret = 0;

    Triangle *scan = mInputTriangles;

    for (NxU32 i=0; i<mTcount; i++)
    {
      ret = locateIntersection(scan,t);
      if ( ret )
        break;
      scan++;
    }
    return ret;
  }


  Triangle             *mInputTriangles;
  NxU32                mVcount;
  NxU32                mMaxTcount;
  NxU32                mTcount;
  const float          *mVertices;
  NxU32Vector          mIndices;
  NxU32Vector          mIds;
  TriangleVector        mSplit;
  NxU32                mEdgeCount;
  Edge                 *mEdges;
  EdgeMap               mEdgeMap;
  NxF32                 mEpsilon;
};

}; // end of namespace

using namespace REMOVE_TJUNCTIONS;

RemoveTjunctions * createRemoveTjunctions(void)
{
  MyRemoveTjunctions *m = new MyRemoveTjunctions;
  return static_cast< RemoveTjunctions *>(m);
}

void               releaseRemoveTjunctions(RemoveTjunctions *tj)
{
  MyRemoveTjunctions *m = static_cast< MyRemoveTjunctions *>(tj);
  delete m;
}

