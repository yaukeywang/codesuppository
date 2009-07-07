#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#pragma warning(disable:4702)

#include "MeshConsolidate.h"
#include "FloatMath.h"
#include "He.h"
#include "common/HeMath/HeVec3.h"

#pragma warning(disable:4100)

#include <vector>
#include "UserMemAlloc.h"

namespace MESH_CONSOLIDATE
{

class TempTri
{
public:
  HeU32     mI1;
  HeU32     mI2;
  HeU32     mI3;
  HeU32     mId;
  HeU32     mSubMesh;
};

class Polygon;

class Edge
{
public:

  HeU32 getHash(void) const { return (mI2<<16)|mI1; };
  HeU32 getInverseHash(void) const { return (mI1<<16)|mI2; };


  HeU32 mI1;
  HeU32 mI2;
  Polygon *mParent;

  Edge    *mNext;              // inside the edge hash map
  Edge   *mPrevious;

  Edge    *mNextPolyPoint;     // inside a specific polygon.
};

class Polygon
{
public:
  HeU32   mId;
  HeU32   mSubMesh;
  HeVec3  mNormal;
  Edge   *mHead;
};

typedef USER_STL::vector< TempTri > TempTriVector;
typedef stdext::hash_map< HeU32, Edge * > EdgeHashMap;

class MyMeshConsolidate : public MeshConsolidate
{
public:

  MyMeshConsolidate(void)
  {
    mVertices = fm_createVertexIndex(0.00001f,false);
    mPolygons = 0;
    mEdges = 0;
    mPolyCount = 0;
    mEdgeCount = 0;
  }

  ~MyMeshConsolidate(void)
  {
    release();
  }

  void release(void)
  {
    if ( mVertices )
    {
        fm_releaseVertexIndex(mVertices);
    }
    MEMALLOC_DELETE_ARRAY(Polygon,mPolygons);
    MEMALLOC_DELETE_ARRAY(Edge,mEdges);
    mPolygons = 0;
    mEdges = 0;
    mPolyCount = 0;
    mEdgeCount = 0;
  }

  virtual bool addTriangle(const float *p1,
                           const float *p2,
                           const float *p3,
                           HeU32 id,
                           HeU32 subMesh)
  {
    bool ret = false;

    bool np;
    TempTri t;
    t.mI1 = mVertices->getIndex(p1,np);
    t.mI2 = mVertices->getIndex(p2,np);
    t.mI3 = mVertices->getIndex(p3,np);
    t.mId = id;
    t.mSubMesh = subMesh;

    if ( t.mI1 == t.mI2 || t.mI1 == t.mI3 || t.mI2 == t.mI3 )
    {
        assert(0); // added a degenerate triangle!
    }
    else
    {
        ret = true;
        mInputTriangles.push_back(t);
    }

    return ret;
  }


  virtual bool meshConsolidate(MeshConsolidateOutput &results)
  {
    bool ret = false;

    if ( !mInputTriangles.empty() )
    {
        mPolyCount = mInputTriangles.size();
        mPolygons  = MEMALLOC_NEW_ARRAY(Polygon,mPolyCount)[mPolyCount];


    }

    return ret;
  }

  HeU32           mPolyCount;
  HeU32           mEdgeCount;
  Polygon        *mPolygons;
  Edge           *mEdges;

  fm_VertexIndex *mVertices;
  TempTriVector mInputTriangles;
};

};

using namespace MESH_CONSOLIDATE;

MeshConsolidate * createMeshConsolidate(void)
{
  MyMeshConsolidate *mcm = new MyMeshConsolidate;
  return static_cast< MeshConsolidate *>(mcm);
}

void              releaseMeshConsolidate(MeshConsolidate *cm)
{
  MyMeshConsolidate *mcm = static_cast< MyMeshConsolidate *>(cm);
  delete mcm;
}

