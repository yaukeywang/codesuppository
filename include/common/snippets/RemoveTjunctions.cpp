#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#pragma warning(disable:4702)
#include <vector>
#include "UserMemAlloc.h"

#include "RemoveTjunctions.h"
#include "FloatMath.h"
#include "He.h"

#pragma warning(disable:4127)
#pragma warning(disable:4189)

#define DEBUG_SHOW 0

#if DEBUG_SHOW
#include "RenderDebug/RenderDebug.h"
#endif

namespace REMOVE_TJUNCTIONS
{

class AABB
{
public:
  float mMin[3];
  float mMax[3];
};

bool gDebug=false;
size_t gCount=0;

const double EPSILON = 0.001f;

typedef USER_STL::vector< size_t > size_tVector;

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

  Triangle(size_t i1,size_t i2,size_t i3,const double *vertices,size_t id)
  {
    mPending = false;
    init(i1,i2,i3,vertices,id);
    mSplit = false;
  }

  void init(size_t i1,size_t i2,size_t i3,const double *vertices,size_t id)
  {
    mSplit = false;
    mI1 = i1;
    mI2 = i2;
    mI3 = i3;
    mId = id;

    const double *p1 = &vertices[mI1*3];
    const double *p2 = &vertices[mI2*3];
    const double *p3 = &vertices[mI3*3];

    initMinMax(p1,p2,p3);
  }

  void initMinMax(const double *p1,const double *p2,const double *p3)
  {
    fm_copy3(p1,mBmin);
    fm_copy3(p1,mBmax);
    fm_minmax(p2,mBmin,mBmax);
    fm_minmax(p3,mBmin,mBmax);
  }

  void init(const size_t *idx,const double *vertices,size_t id)
  {
    mSplit = false;
    mI1 = idx[0];
    mI2 = idx[1];
    mI3 = idx[2];
    mId = id;

    const double *p1 = &vertices[mI1*3];
    const double *p2 = &vertices[mI2*3];
    const double *p3 = &vertices[mI3*3];

    initMinMax(p1,p2,p3);

  }

#if DEBUG_SHOW
  void debug(unsigned int color,const double *vertices,double pass=0) const
  {
    const double *_p1 = &vertices[mI1*3];
    const double *_p2 = &vertices[mI2*3];
    const double *_p3 = &vertices[mI3*3];
    double p1[3];
    double p2[3];
    double p3[3];
    fm_copy3(_p1,p1);
    fm_copy3(_p2,p2);
    fm_copy3(_p3,p3);

    p1[1]+=pass;
    p2[1]+=pass;
    p3[1]+=pass;

    gRenderDebug->DebugSolidTri(p1,p2,p3,color,30.0f);
    gRenderDebug->DebugTri(p1,p2,p3,0xFFFFFF,30.0f);
    gRenderDebug->DebugSphere(p1,0.002f,0xFFFF00,30.0f);
    gRenderDebug->DebugSphere(p2,0.002f,0xFFFF00,30.0f);
    gRenderDebug->DebugSphere(p3,0.002f,0xFFFF00,30.0f);
  }
#endif


  bool intersects(const double *pos,const double *p1,const double *p2) const
  {
    bool ret = false;

    double sect[3];
    LineSegmentType type;
    double dist = fm_distancePointLineSegment(pos,p1,p2,sect,type);
    if ( type == LS_MIDDLE && dist < EPSILON )
    {
      ret = true;
    }

    return ret;
  }

  bool intersects(size_t i,const double *vertices,size_t &edge) const
  {
    bool ret = true;

    const double *pos = &vertices[i*3];
    const double *p1  = &vertices[mI1*3];
    const double *p2  = &vertices[mI2*3];
    const double *p3  = &vertices[mI3*3];
    if ( intersects(pos,p1,p2) )
    {
      edge = 0;
    }
    else if ( intersects(pos,p2,p3) )
    {
      edge = 1;
    }
    else if ( intersects(pos,p3,p1) )
    {
      edge = 2;
    }
    else
    {
      ret = false;
    }
    return ret;
  }

  bool intersects(const Triangle *t,const double *vertices,size_t &intersection_index,size_t &edge)
  {
    bool ret = false;

    if ( fm_intersectAABB(mBmin,mBmax,t->mBmin,t->mBmax) ) // only if the AABB's of the two triangles intersect...
    {

      if ( t->intersects(mI1,vertices,edge) )
      {
        intersection_index = mI1;
        ret = true;
      }

      if ( t->intersects(mI2,vertices,edge) )
      {
        intersection_index = mI2;
        ret = true;
      }

      if ( t->intersects(mI3,vertices,edge) )
      {
        intersection_index = mI3;
        ret = true;
      }

    }

    return ret;
  }

  bool    mSplit:1;
  bool    mPending:1;
  size_t  mI1;
  size_t  mI2;
  size_t  mI3;
  size_t  mId;

  double   mBmin[3];
  double   mBmax[3];
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

  size_t init(Triangle *t,size_t i1,size_t i2)
  {
    mTriangle = t;
    mNextEdge = 0;
    HE_ASSERT( i1 < 65536 );
    HE_ASSERT( i2 < 65536 );
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
  size_t    mHash;
};


typedef USER_STL::vector< Triangle * > TriangleVector;
typedef USER_STL::hash_map< size_t, Edge * > EdgeMap;

class MyRemoveTjunctions : public RemoveTjunctions
{
public:

  virtual size_t removeTjunctions(RemoveTjunctionsDesc &desc)
  {
    size_t ret = 0;

    if ( desc.mVerticesF )
    {
      desc.mIndicesOut = removeTjunctions(desc.mVcount, desc.mVerticesF, desc.mTcount, desc.mIndices, desc.mTcountOut, desc.mIds);
    }
    else if ( desc.mVerticesD )
    {
      desc.mIndicesOut = removeTjunctions(desc.mVcount, desc.mVerticesD, desc.mTcount, desc.mIndices, desc.mTcountOut, desc.mIds );
    }

    if ( !mIds.empty() )
    {
      desc.mIds = &mIds[0];
    }

    ret = desc.mTcountOut;

    return ret;
  }

  size_t * removeTjunctions(size_t vcount,
                            const float *vertices,
                            size_t tcount,
                            const size_t *indices,
                            size_t &tcount_out,
                            const size_t *ids)
  {
    double *dvertices = new double[vcount*3];
    const float *source = vertices;
    double *dest        = dvertices;
    for (size_t i=0; i<vcount; i++)
    {
      dest[0] = source[0];
      dest[1] = source[1];
      dest[2] = source[2];
      dest+=3;
      source+=3;
    }

    size_t *ret = removeTjunctions(vcount,dvertices,tcount,indices,tcount_out,ids);

    delete []dvertices;

    return ret;
  }

  Edge * addEdge(Triangle *t,Edge *e,size_t i1,size_t i2)
  {
    size_t hash = e->init(t,i1,i2);
    EdgeMap::iterator found;
    found = mEdgeMap.find(hash);
    if ( found == mEdgeMap.end() )
    {
      mEdgeMap[hash] = e;
    }
    else
    {
      Edge *old_edge = (*found).second;
      e->mNextEdge = old_edge;
      (*found).second = e;
    }
    e++;
    mEdgeCount++;
    return e;
  }

  Edge * init(Triangle *t,const size_t *indices,const double *vertices,Edge *e,size_t id)
  {
    t->init(indices,vertices,id);
    e = addEdge(t,e,t->mI1,t->mI2);
    e = addEdge(t,e,t->mI2,t->mI3);
    e = addEdge(t,e,t->mI3,t->mI1);
    return e;
  }

  virtual size_t * removeTjunctions(size_t vcount,
                                    const double *vertices,
                                    size_t tcount,
                                    const size_t *indices,
                                    size_t &tcount_out,
                                    const size_t * ids)
  {
    size_t *ret  = 0;

    mVcount   = vcount;
    mVertices = vertices;
    mTcount   = tcount;
    tcount_out = 0;

    mTcount         = tcount;
    mMaxTcount      = tcount*2;
    mInputTriangles = new Triangle[mMaxTcount];
    Triangle *t     = mInputTriangles;

    mEdges          = new Edge[mMaxTcount*3];
    mEdgeCount      = 0;

    size_t id = 0;

    Edge *e = mEdges;
    for (size_t i=0; i<tcount; i++)
    {
      if ( ids ) id = *ids++;
      e =init(t,indices,vertices,e,id);
      indices+=3;
      t++;
    }


    {
      TriangleVector test;

      EdgeMap::iterator i;
      for (i=mEdgeMap.begin(); i!=mEdgeMap.end(); ++i)
      {
        Edge *e = (*i).second;
        if ( e->mNextEdge == 0 ) // open edge!
        {
          Triangle *t = e->mTriangle;
          if ( t->mPending == false )
          {
            test.push_back(t);
            t->mPending = true;
          }
        }
      }
      if ( !test.empty() )
      {
        TriangleVector::iterator i;
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
      TriangleVector::iterator i;
      for (i=scan.begin(); i!=scan.end(); ++i)
      {
        Triangle *t = (*i);
        locateIntersection(t);
      }
    }



    mIndices.clear();
    mIds.clear();

    t = mInputTriangles;
    for (size_t i=0; i<mTcount; i++)
    {
      mIndices.push_back(t->mI1);
      mIndices.push_back(t->mI2);
      mIndices.push_back(t->mI3);
      mIds.push_back(t->mId);
#if DEBUG_SHOW
      if ( t->mSplit )
        t->debug(0xFF0000,mVertices);
      else
        t->debug(0xFF00FF,mVertices);
#endif
      t++;
    }


   mEdgeMap.clear();
   delete []mEdges;
   mEdges = 0;
   delete []mInputTriangles;
   mInputTriangles = 0;
   tcount_out = mIndices.size()/3;
   ret = &mIndices[0];

    return ret;
  }

  Triangle * locateIntersection(Triangle *scan,Triangle *t)
  {
    Triangle *ret = 0;

    size_t t1 = (size_t)(scan-mInputTriangles);
    size_t t2 = (size_t)(t-mInputTriangles);

    HE_ASSERT( t1 < mTcount );
    HE_ASSERT( t2 < mTcount );

    HE_ASSERT( scan->mI1 < mVcount );
    HE_ASSERT( scan->mI2 < mVcount );
    HE_ASSERT( scan->mI3 < mVcount );

    HE_ASSERT( t->mI1 < mVcount );
    HE_ASSERT( t->mI2 < mVcount );
    HE_ASSERT( t->mI3 < mVcount );


    size_t intersection_index;
    size_t edge;

    if ( scan != t && scan->intersects(t,mVertices,intersection_index,edge) )
    {
      // here is where it intersects!
      size_t i1,i2,i3;
      size_t j1,j2,j3;
      size_t id = t->mId;

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
          HE_ASSERT(0);
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

#if DEBUG_SHOW
        const double *point = &mVertices[intersection_index*3];
        gRenderDebug->DebugSphere(point,0.01f,0xFFFF00,60.0f);
#endif

        mSplit.push_back(t);
        mSplit.push_back(newt);
        ret = scan;
      }
    }
    return ret;
  }

  Triangle * locateIntersection(Triangle *t)
  {
    Triangle *ret = 0;

    Triangle *scan = mInputTriangles;

    for (size_t i=0; i<mTcount; i++)
    {
      ret = locateIntersection(scan,t);
      if ( ret )
        break;
      scan++;
    }
    return ret;
  }


  TriangleVector        mPairs;
  Triangle             *mInputTriangles;
  size_t                mVcount;
  size_t                mMaxTcount;
  size_t                mTcount;
  const double          *mVertices;
  size_tVector          mIndices;
  size_tVector          mIds;
  TriangleVector        mSplit;
  size_t                mEdgeCount;
  Edge                 *mEdges;
  EdgeMap               mEdgeMap;
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

