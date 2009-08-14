#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <hash_map>

#pragma warning(disable:4100 4288)

#include "UserMemAlloc.h"
#include <vector>
#include <queue>

#include "MeshIslandGeneration.h"
#include "FloatMath.h"

#define SHOW_DEBUG 0
#include "NxSimpleTypes.h"

#if SHOW_DEBUG
#include "RenderDebug/RenderDebug.h"
//#include "log.h"
#endif

namespace MESH_ISLAND_GENERATION
{

typedef USER_STL::vector< size_t > size_tVector;

class Edge;
class Island;

class AABB
{
public:
  NxF32 mMin[3];
  NxF32 mMax[3];
};

class Triangle
{
public:
  Triangle(void)
  {
    mConsumed = false;
    mIsland   = 0;
    mHandle   = 0;
    mId       = 0;
  }

  void minmax(const NxF32 *p,AABB &box)
  {
    if ( p[0] < box.mMin[0] ) box.mMin[0] = p[0];
    if ( p[1] < box.mMin[1] ) box.mMin[1] = p[1];
    if ( p[2] < box.mMin[2] ) box.mMin[2] = p[2];

    if ( p[0] > box.mMax[0] ) box.mMax[0] = p[0];
    if ( p[1] > box.mMax[1] ) box.mMax[1] = p[1];
    if ( p[2] > box.mMax[2] ) box.mMax[2] = p[2];
  }

  void minmax(const NxF64 *p,AABB &box)
  {
    if ( (NxF32)p[0] < box.mMin[0] ) box.mMin[0] = (NxF32)p[0];
    if ( (NxF32)p[1] < box.mMin[1] ) box.mMin[1] = (NxF32)p[1];
    if ( (NxF32)p[2] < box.mMin[2] ) box.mMin[2] = (NxF32)p[2];
    if ( (NxF32)p[0] > box.mMax[0] ) box.mMax[0] = (NxF32)p[0];
    if ( (NxF32)p[1] > box.mMax[1] ) box.mMax[1] = (NxF32)p[1];
    if ( (NxF32)p[2] > box.mMax[2] ) box.mMax[2] = (NxF32)p[2];
  }

  void buildBox(const NxF32 *vertices_f,const NxF64 *vertices_d,size_t id);

  void render(NxU32 color)
  {
//    gRenderDebug->DebugBound(&mBox.mMin[0],&mBox.mMax[0],color,60.0f);
  }

  void getTriangle(NxF32 *tri,const NxF32 *vertices_f,const NxF64 *vertices_d);

  size_t    mHandle;
  bool      mConsumed;
  Edge     *mEdges[3];
  Island   *mIsland;   // identifies which island it is a member of
  unsigned short  mId;
  AABB      mBox;
};


class Edge
{
public:
  Edge(void)
  {
    mI1 = 0;
    mI2 = 0;
    mHash = 0;
    mNext = 0;
    mPrevious = 0;
    mParent = 0;
    mNextTriangleEdge = 0;
  }

  void init(size_t i1,size_t i2,Triangle *parent)
  {
    assert( i1 < 65536 );
    assert( i2 < 65536 );

    mI1 = i1;
    mI2 = i2;
    mHash        = (i2<<16)|i1;
    mReverseHash = (i1<<16)|i2;
    mNext = 0;
    mPrevious = 0;
    mParent = parent;
  }

  size_t  mI1;
  size_t  mI2;
  size_t  mHash;
  size_t  mReverseHash;

  Edge     *mNext;
  Edge     *mPrevious;
  Edge     *mNextTriangleEdge;
  Triangle *mParent;
};

typedef USER_STL_EXT::hash_map< size_t, Edge * > EdgeHashMap;
typedef USER_STL::vector< Triangle * > TriangleVector;

class EdgeCheck
{
public:
  EdgeCheck(Triangle *t,Edge *e)
  {
    mTriangle = t;
    mEdge     = e;
  }

  Triangle  *mTriangle;
  Edge      *mEdge;
};

typedef std::queue< EdgeCheck > EdgeCheckQueue;

class Island 
{
public:
  Island(Triangle *t,Triangle *root)
  {
    mVerticesFloat = 0;
    mVerticesDouble = 0;
    t->mIsland = this;
    mTriangles.push_back(t);
    mCoplanar = false;
    fm_initMinMax(mMin,mMax);
  }

  void add(Triangle *t,Triangle *root)
  {
    t->mIsland = this;
    mTriangles.push_back(t);
  }

  void merge(Island &isl)
  {
    TriangleVector::iterator i;
    for (i=isl.mTriangles.begin(); i!=isl.mTriangles.end(); ++i)
    {
      Triangle *t = (*i);
      mTriangles.push_back(t);
    }
    isl.mTriangles.clear();
  }

  bool isTouching(Island *isl,const NxF32 *vertices_f,const NxF64 *vertices_d)
  {
    bool ret = false;

    mVerticesFloat = vertices_f;
    mVerticesDouble = vertices_d;

    if ( fm_intersectAABB(mMin,mMax,isl->mMin,isl->mMax) ) // if the two islands has an intersecting AABB
    {
      // todo..
    }


    return ret;
  }


#if SHOW_DEBUG
  void debugTri(const NxF32 *t,NxU32 color)
  {
    gRenderDebug->DebugSolidTri(t,t+3,t+6,color,60.0f);
    gRenderDebug->DebugTri(t,t+3,t+6,color,60.0f);
  }
#endif

  void SAP_DeletePair(const void* object0, const void* object1, void* user_data, void* pair_user_data)
  {
  }

  void render(NxU32 color)
  {
//    gRenderDebug->DebugBound(mMin,mMax,color,60.0f);
    TriangleVector::iterator i;
    for (i=mTriangles.begin(); i!=mTriangles.end(); ++i)
    {
      Triangle *t = (*i);
      t->render(color);
    }
  }


  const NxF64   *mVerticesDouble;
  const NxF32    *mVerticesFloat;

  NxF32           mMin[3];
  NxF32           mMax[3];
  bool            mCoplanar; // marked as co-planar..
  TriangleVector  mTriangles;
};


void Triangle::getTriangle(NxF32 *tri,const NxF32 *vertices_f,const NxF64 *vertices_d)
{
  size_t i1 = mEdges[0]->mI1;
  size_t i2 = mEdges[1]->mI1;
  size_t i3 = mEdges[2]->mI1;
  if ( vertices_f )
  {
    const NxF32 *p1 = &vertices_f[i1*3];
    const NxF32 *p2 = &vertices_f[i2*3];
    const NxF32 *p3 = &vertices_f[i3*3];
    fm_copy3(p1,tri);
    fm_copy3(p2,tri+3);
    fm_copy3(p3,tri+6);
  }
  else
  {
    const NxF64 *p1 = &vertices_d[i1*3];
    const NxF64 *p2 = &vertices_d[i2*3];
    const NxF64 *p3 = &vertices_d[i3*3];
    fm_doubleToFloat3(p1,tri);
    fm_doubleToFloat3(p2,tri+3);
    fm_doubleToFloat3(p3,tri+6);
  }
}

void Triangle::buildBox(const NxF32 *vertices_f,const NxF64 *vertices_d,size_t id)
{
  mId = (unsigned short)id;
  size_t i1 = mEdges[0]->mI1;
  size_t i2 = mEdges[1]->mI1;
  size_t i3 = mEdges[2]->mI1;

  if ( vertices_f )
  {
    const NxF32 *p1 = &vertices_f[i1*3];
    const NxF32 *p2 = &vertices_f[i2*3];
    const NxF32 *p3 = &vertices_f[i3*3];
    mBox.mMin[0] = p1[0];
    mBox.mMin[1] = p1[1];
    mBox.mMin[2] = p1[2];
    mBox.mMax[0] = p1[0];
    mBox.mMax[1] = p1[1];
    mBox.mMax[2] = p1[2];
    minmax(p2,mBox);
    minmax(p3,mBox);
  }
  else
  {
    const NxF64 *p1 = &vertices_d[i1*3];
    const NxF64 *p2 = &vertices_d[i2*3];
    const NxF64 *p3 = &vertices_d[i3*3];
    mBox.mMin[0] = (NxF32)p1[0];
    mBox.mMin[1] = (NxF32)p1[1];
    mBox.mMin[2] = (NxF32)p1[2];
    mBox.mMax[0] = (NxF32)p1[0];
    mBox.mMax[1] = (NxF32)p1[1];
    mBox.mMax[2] = (NxF32)p1[2];
    minmax(p2,mBox);
    minmax(p3,mBox);
  }

  assert(mIsland);
  if ( mIsland )
  {
    if ( mBox.mMin[0] < mIsland->mMin[0] ) mIsland->mMin[0] = mBox.mMin[0];
    if ( mBox.mMin[1] < mIsland->mMin[1] ) mIsland->mMin[1] = mBox.mMin[1];
    if ( mBox.mMin[2] < mIsland->mMin[2] ) mIsland->mMin[2] = mBox.mMin[2];

    if ( mBox.mMax[0] > mIsland->mMax[0] ) mIsland->mMax[0] = mBox.mMax[0];
    if ( mBox.mMax[1] > mIsland->mMax[1] ) mIsland->mMax[1] = mBox.mMax[1];
    if ( mBox.mMax[2] > mIsland->mMax[2] ) mIsland->mMax[2] = mBox.mMax[2];
  }

}


typedef USER_STL::vector< Island * > IslandVector;

class MyMeshIslandGeneration : public MeshIslandGeneration
{
public:
  MyMeshIslandGeneration(void)
  {
    mTriangles = 0;
    mEdges     = 0;
    mVerticesDouble = 0;
    mVerticesFloat  = 0;
  }

  ~MyMeshIslandGeneration(void)
  {
    reset();
  }

  void reset(void)
  {
    delete []mTriangles;
    delete []mEdges;
    mTriangles = 0;
    mEdges = 0;
    mTriangleEdges.clear();
    IslandVector::iterator i;
    for (i=mIslands.begin(); i!=mIslands.end(); ++i)
    {
      Island *_i = (*i);
      delete _i;
    }
    mIslands.clear();
  }

  size_t islandGenerate(size_t tcount,const size_t *indices,const NxF64 *vertices)
  {
    mVerticesDouble = vertices;
    mVerticesFloat  = 0;
    return islandGenerate(tcount,indices);
  }

  size_t islandGenerate(size_t tcount,const size_t *indices,const NxF32 *vertices)
  {
    mVerticesDouble = 0;
    mVerticesFloat  = vertices;
    return islandGenerate(tcount,indices);
  }

  size_t islandGenerate(size_t tcount,const size_t *indices)
  {
    size_t ret = 0;

    reset();

    mTcount = tcount;
    mTriangles = new Triangle[tcount];
    mEdges     = new Edge[tcount*3];
    Edge *e = mEdges;

    for (size_t i=0; i<tcount; i++)
    {
      Triangle &t = mTriangles[i];

      size_t i1 = *indices++;
      size_t i2 = *indices++;
      size_t i3 = *indices++;

      t.mEdges[0] = e;
      t.mEdges[1] = e+1;
      t.mEdges[2] = e+2;

      e = addEdge(e,&t,i1,i2);
      e = addEdge(e,&t,i2,i3);
      e = addEdge(e,&t,i3,i1);

    }

    // while there are still edges to process...
#if SHOW_DEBUG
    NxF32 offset = 0;
#endif

    while ( !mTriangleEdges.empty() )
    {

#if SHOW_DEBUG
      NxU32 color = gRenderDebug->getDebugColor();
//      offset+=2;
#endif

      EdgeHashMap::iterator found = mTriangleEdges.begin();

      Triangle *t = (*found).second->mParent;

      Island *i = new Island(t,mTriangles);  // the initial triangle...
      removeTriangle(t); // remove this triangle from the triangle-edges hashmap

#if SHOW_DEBUG
      debugTriangle(t,color,offset);
#endif

      mIslands.push_back(i);

      // now keep adding to this island until we can no longer walk any shared edges..
      addEdgeCheck(t,t->mEdges[0]);
      addEdgeCheck(t,t->mEdges[1]);
      addEdgeCheck(t,t->mEdges[2]);

      while ( !mEdgeCheckQueue.empty() )
      {

        EdgeCheck e = mEdgeCheckQueue.front();
        mEdgeCheckQueue.pop();

        // Process all triangles which share this edge
        Edge *edge = locateSharedEdge(e.mEdge);

        while ( edge )
        {
          Triangle *t = edge->mParent;
          assert(!t->mConsumed);
          i->add(t,mTriangles);
          removeTriangle(t); // remove this triangle from the triangle-edges hashmap

#if SHOW_DEBUG
          debugTriangle(t,color,offset);
#endif

          // now keep adding to this island until we can no longer walk any shared edges..

          if ( edge != t->mEdges[0] )
          {
            addEdgeCheck(t,t->mEdges[0]);
          }

          if ( edge != t->mEdges[1] )
          {
            addEdgeCheck(t,t->mEdges[1]);
          }

          if ( edge != t->mEdges[2] )
          {
            addEdgeCheck(t,t->mEdges[2]);
          }

          edge = locateSharedEdge(e.mEdge); // keep going until all shared edges have been processed!
        }

      }
    }

    ret = mIslands.size();

    return ret;
  }

  size_t *   getIsland(size_t index,size_t &otcount)
  {
    size_t *ret  = 0;

    mIndices.clear();
    if ( index < mIslands.size() )
    {
      Island *i = mIslands[index];
      otcount = i->mTriangles.size();
      TriangleVector::iterator j;
      for (j=i->mTriangles.begin(); j!=i->mTriangles.end(); ++j)
      {
        Triangle *t = (*j);
        mIndices.push_back(t->mEdges[0]->mI1);
        mIndices.push_back(t->mEdges[1]->mI1);
        mIndices.push_back(t->mEdges[2]->mI1);
      }
      ret = &mIndices[0];
    }

    return ret;
  }

private:

#if SHOW_DEBUG
  void debugEdge(Edge *e,const NxF32 *vertices,NxU32 color)
  {
    const NxF32 *p1 = &vertices[e->mI1*3];
    const NxF32 *p2 = &vertices[e->mI2*3];
    gRenderDebug->DebugRay(p1,p2,0.001f,0xFFFFFF,0xFF0000,10.0f);
  }

  void debugEdge(Edge *e,const NxF64 *vertices,NxU32 color)
  {
    const NxF64 *p1 = &vertices[e->mI1*3];
    const NxF64 *p2 = &vertices[e->mI2*3];
    gRenderDebug->DebugRay(p1,p2,0.001f,0xFFFFFF,0xFF0000,10.0f);
  }



  void debugTriangle(Triangle *t,NxU32 color,NxF32 offset)
  {
    size_t i1 = t->mEdges[0]->mI2;
    size_t i2 = t->mEdges[1]->mI2;
    size_t i3 = t->mEdges[2]->mI2;

    if ( mVerticesFloat )
    {
      const NxF32 *_p1 = &mVerticesFloat[i1*3];
      const NxF32 *_p2 = &mVerticesFloat[i2*3];
      const NxF32 *_p3 = &mVerticesFloat[i3*3];
      NxF32 p1[3];
      NxF32 p2[3];
      NxF32 p3[3];

      fm_copy3(_p1,p1);
      fm_copy3(_p2,p2);
      fm_copy3(_p3,p3);

      p1[0]+=offset;
      p2[0]+=offset;
      p3[0]+=offset;

      gRenderDebug->DebugSolidTri(p1,p2,p3,color,10.0f);

      debugEdge(t->mEdges[0],mVerticesFloat,color);
      debugEdge(t->mEdges[1],mVerticesFloat,color);
      debugEdge(t->mEdges[2],mVerticesFloat,color);
    }
    else if ( mVerticesDouble )
    {
      const NxF64 *p1 = &mVerticesDouble[i1*3];
      const NxF64 *p2 = &mVerticesDouble[i2*3];
      const NxF64 *p3 = &mVerticesDouble[i3*3];

      gRenderDebug->DebugSolidTri(p1,p2,p3,color,10.0f);

      debugEdge(t->mEdges[0],mVerticesDouble,color);
      debugEdge(t->mEdges[1],mVerticesDouble,color);
      debugEdge(t->mEdges[2],mVerticesDouble,color);
    }



  }
#endif

  void removeTriangle(Triangle *t)
  {
    t->mConsumed = true;

    removeEdge(t->mEdges[0]);
    removeEdge(t->mEdges[1]);
    removeEdge(t->mEdges[2]);

  }


  Edge * locateSharedEdge(Edge *e)
  {
    Edge *ret = 0;

    EdgeHashMap::iterator found;
    found = mTriangleEdges.find( e->mReverseHash );
    if ( found != mTriangleEdges.end() )
    {
      ret = (*found).second;
      assert( ret->mHash == e->mReverseHash );
    }
    return ret;
  }

  void removeEdge(Edge *e)
  {
    EdgeHashMap::iterator found;
    found = mTriangleEdges.find( e->mHash );

    if ( found != mTriangleEdges.end() )
    {
      Edge *prev = 0;
      Edge *scan = (*found).second;
      while ( scan && scan != e )
      {
        prev = scan;
        scan = scan->mNextTriangleEdge;
      }

      if ( scan )
      {
        if ( prev == 0 )
        {
          if ( scan->mNextTriangleEdge )
          {
            (*found).second = scan->mNextTriangleEdge;
          }
          else
          {
            mTriangleEdges.erase(found); // no more polygons have an edge here
          }
        }
        else
        {
          prev->mNextTriangleEdge = scan->mNextTriangleEdge;
        }
      }
      else
      {
        assert(0);
      }
    }
    else
    {
      assert(0); // impossible!
    }
  }


  Edge * addEdge(Edge *e,Triangle *t,size_t i1,size_t i2)
  {

    e->init(i1,i2,t);

    EdgeHashMap::iterator found;
    found = mTriangleEdges.find(e->mHash);
    if ( found == mTriangleEdges.end() )
    {
      mTriangleEdges[ e->mHash ] = e;
    }
    else
    {
      Edge *pn = (*found).second;
      e->mNextTriangleEdge = pn;
      (*found).second = e;
    }

    e++;

    return e;
  }

  void addEdgeCheck(Triangle *t,Edge *e)
  {
    EdgeCheck ec(t,e);
    mEdgeCheckQueue.push(ec);
  }

  size_t mergeCoplanarIslands(const NxF32 *vertices)
  {
    mVerticesFloat = vertices;
    mVerticesDouble = 0;
    return mergeCoplanarIslands();
  }

  size_t mergeCoplanarIslands(const NxF64 *vertices)
  {
    mVerticesDouble = vertices;
    mVerticesFloat = 0;
    return mergeCoplanarIslands();
  }

  // this island needs to be merged
  void mergeTouching(Island *isl)
  {
    Island *touching = 0;

    IslandVector::iterator i;
    for (i=mIslands.begin(); i!=mIslands.end(); ++i)
    {
      Island *_i = (*i);
      if ( !_i->mCoplanar ) // can't merge with coplanar islands!
      {
        if ( _i->isTouching(isl,mVerticesFloat,mVerticesDouble) )
        {
          touching = _i;
        }
      }
    }
  }

  size_t mergeCoplanarIslands(void)
  {
    size_t  ret = 0;

    if ( !mIslands.empty() )
    {


      size_t  cp_count  = 0;
      size_t  npc_count = 0;

      size_t  count = mIslands.size();

      for (size_t i=0; i<count; i++)
      {

        size_t otcount;
        const size_t *oindices = getIsland(i,otcount);

        if ( otcount )
        {

          bool isCoplanar;

          if ( mVerticesFloat )
            isCoplanar = fm_isMeshCoplanar(otcount, oindices, mVerticesFloat, true);
          else
            isCoplanar = fm_isMeshCoplanar(otcount, oindices, mVerticesDouble, true);

          if ( isCoplanar )
          {
            Island *isl = mIslands[i];
            isl->mCoplanar = true;
            cp_count++;
          }
          else
          {
            npc_count++;
          }
        }
        else
        {
          assert(0);
        }
      }

      if ( cp_count )
      {
        if ( npc_count == 0 ) // all islands are co-planar!
        {
          IslandVector temp = mIslands;
          mIslands.clear();
          Island *isl = mIslands[0];
          mIslands.push_back(isl);
          for (size_t i=1; i<cp_count; i++)
          {
            Island *_i = mIslands[i];
            isl->merge(*_i);
            delete _i;
          }
        }
        else
        {


          Triangle *t = mTriangles;
          for (size_t i=0; i<mTcount; i++)
          {
            t->buildBox(mVerticesFloat,mVerticesDouble,i);
            t++;
          }

          IslandVector::iterator i;
          for (i=mIslands.begin(); i!=mIslands.end(); ++i)
          {
            Island *isl = (*i);

            NxU32 color = 0x00FF00;

            if ( isl->mCoplanar )
            {
              color = 0xFFFF00;
            }

            mergeTouching(isl);

          }

          IslandVector temp = mIslands;
          mIslands.clear();
          for (i=temp.begin(); i!=temp.end(); i++)
          {
            Island *isl = (*i);
            if ( isl->mCoplanar )
            {
              delete isl; // kill it
            }
            else
            {
              mIslands.push_back(isl);
            }
          }
          ret = mIslands.size();
        }
      }
      else
      {
        ret = npc_count;
      }
    }


    return ret;
  }

  size_t mergeTouchingIslands(const NxF32 *vertices)
  {
    size_t ret = 0;

    return ret;
  }

  size_t mergeTouchingIslands(const NxF64 *vertices)
  {
    size_t ret = 0;

    return ret;
  }

  size_t           mTcount;
  Triangle        *mTriangles;
  Edge            *mEdges;
  EdgeHashMap      mTriangleEdges;
  IslandVector     mIslands;
  EdgeCheckQueue   mEdgeCheckQueue;
  const NxF64    *mVerticesDouble;
  const NxF32     *mVerticesFloat;
  size_tVector     mIndices;
};

}; // end of namespace

using namespace MESH_ISLAND_GENERATION;

MeshIslandGeneration * createMeshIslandGeneration(void)
{
  MyMeshIslandGeneration *mig = new MyMeshIslandGeneration;
  return static_cast< MeshIslandGeneration *>(mig);
}

void                   releaseMeshIslandGeneration(MeshIslandGeneration *cm)
{
  MyMeshIslandGeneration *mig = static_cast< MyMeshIslandGeneration *>(cm);
  delete mig;
}

