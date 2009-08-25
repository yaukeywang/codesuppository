#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#pragma warning(disable:4702)

#include "ConsolidateMesh.h"
#include "FloatMath.h"

#pragma warning(disable:4100)

#define DEBUG_SHOW 0

#if DEBUG_SHOW
#include "RenderDebug/RenderDebug.h"
#include "log.h"
#endif

#include "UserMemAlloc.h"

namespace CONSOLIDATE_MESH
{

class Polygon;
class Edge;

static void addPolyPoint(Edge *p,Edge **polyPoints,Polygon *parent);
static void removePolyPoint(Edge *p,Edge **polyPoints);
static bool hasPolyPoint(Polygon *p,NxU32 i,Edge **polyPoints);

class Edge
{
public:
  Edge(void)
  {
    mI1 = 0;
    mI2 = 0;
    mNext = 0;
    mPrevious = 0;
    mParent = 0;
  }

  void init(NxU32 i1,NxU32 i2,Polygon *parent)
  {
    assert( i1 < 65536 );
    assert( i2 < 65536 );
    mI1 = i1;
    mI2 = i2;
    mNext = 0;
    mPrevious = 0;
    mParent = parent;
  }

  NxU32 getHash(void) const { return (mI2<<16)|mI1; };
  NxU32 getReverseHash(void) const { return (mI1<<16)|mI2; };


  NxU32  mI1;
  NxU32  mI2;

  Edge   *mNext;
  Edge   *mPrevious;

  Polygon *mParent;
  Edge    *mNextPolyPoint;


};

class Polygon
{
public:

  Polygon(void)
  {
    mMerged = false;
    mRemoved = false;
    mEcount = 0;
    mRoot   = 0;
    mTail   = 0;
    mNextPolygonEdge = 0;
    mIndex = 0;
  }

  void copy(const NxF32 *p,NxF32 *t,NxF32 offsetx,NxF32 offsety,NxF32 offsetz)
  {
    t[0] = (NxF32)(p[0]+offsetx);
    t[1] = (NxF32)(p[1]+offsety);
    t[2] = (NxF32)(p[2]+offsetz);
  }

  void debug(NxF32 offset,fm_VertexIndex *vindex)
  {

    static NxF32 offsetx = 0;
    //offsetx+=10;

    Edge *e = mRoot;
    while ( e )
    {
      const NxF32 *p1 = vindex->getVertexFloat(e->mI1);
      const NxF32 *p2 = vindex->getVertexFloat(e->mI2);
      NxF32 _p1[3];
      NxF32 _p2[3];
      copy(p1,_p1,offsetx,offset,0);
      copy(p2,_p2,offsetx,offset,0);
#if DEBUG_SHOW
      gRenderDebug->DebugRay(_p1,_p2,0.02f,0xFFFFFF,0xFF0000,300.0f);
#endif
      e = e->mNext;
    }
  }

  void addEdge(Edge *e)
  {
    if ( mRoot == 0 )
    {
      mRoot = e;
      mTail = e;
      e->mNext = 0;
      e->mPrevious = 0;
    }
    else // add to the end of the polyg
    {
      mTail->mNext = e; // the old tail's next is now us
      e->mNext = 0;     // our next is null
      e->mPrevious = mTail; // our previous is the old tail
      mTail = e;        // the new tail is us
    }
    mEcount++;
  }

  void removeEdge(Edge *e,Edge **polyPoints)
  {
    removePolyPoint(e,polyPoints);
    Edge *next     = e->mNext;
    Edge *previous = e->mPrevious;
    if ( previous )
    {
      previous->mNext = next;
    }
    else
    {
      mRoot = next;
    }

    if ( next )
    {
      next->mPrevious = previous;
    }
    else
    {
      mTail = previous;
    }
    mEcount--;
  }

  void insertEdge(Edge *edge_at,Edge *new_edge,Edge **polyPoints)
  {

    addPolyPoint(new_edge,polyPoints,this);

    Edge *his_next = edge_at->mNext;
    edge_at->mNext = new_edge;
    new_edge->mPrevious = edge_at;
    new_edge->mNext     = his_next;
    if ( his_next )
    {
      his_next->mPrevious = new_edge;
    }
    else
    {
      mTail = new_edge;
    }
    mEcount++;
  }

  Edge * determineInsertionPoint(Polygon *merge,Edge *e,Edge ** polyPoints)
  {
    (void)polyPoints;
    Edge *ret = 0;

#ifdef _DEBUG
    validatePolygon(polyPoints);
#endif

    assert( merge->mEcount == 3 );

    // This edge 'e' is getting collapsed...
    Edge *tri[3];
    tri[0] = merge->mRoot;
    tri[1]= tri[0]->mNext;
    tri[2]= tri[1]->mNext;

    Edge *scan=0;
    Edge *scan_prev=0;
    Edge *scan_next=0;

    if ( tri[0]->getHash() == e->getReverseHash() )
    {
      scan = tri[0];
      scan_next = tri[1];
      scan_prev = tri[2];
    }
    else if ( tri[1]->getHash() == e->getReverseHash() )
    {
      scan = tri[1];
      scan_next = tri[2];
      scan_prev = tri[0];
    }
    else
    {
      assert( tri[2]->getHash() == e->getReverseHash() );
      scan = tri[2];
      scan_next = tri[0];
      scan_prev = tri[1];
    }

    assert(scan); // we wouldn't be here unless we had a shared edge!
    assert(scan_prev);
    assert(scan_next);

    Edge *e_prev = e->mPrevious ? e->mPrevious : mTail;
    Edge *e_next = e->mNext ? e->mNext : mRoot;

    Edge *insert_one = 0;

    if ( scan_next->getHash() == e_prev->getReverseHash() )
    {
      insert_one = scan_prev;
    }
    else if ( scan_next->getHash() == e_next->getReverseHash() )
    {
      insert_one = scan_prev;
    }
    else if ( scan_prev->getHash() == e_prev->getReverseHash() )
    {
      insert_one = scan_next;
    }
    else if ( scan_prev->getHash() == e_next->getReverseHash() )
    {
      insert_one = scan_next;
    }

    if ( !insert_one )
    {
      ret = scan_next;
    }
    return ret;
  }


  Edge * mergePolygon(Polygon *merge,Edge *e,Edge **polyPoints)
  {
#ifdef _DEBUG
    validatePolygon(polyPoints);
#endif

    assert( merge->mEcount == 3 );
    merge->mRemoved = true;
    mMerged = true;

    Edge *ret = 0;

    // This edge 'e' is getting collapsed...
    Edge *tri[3];
    tri[0] = merge->mRoot;
    tri[1]= tri[0]->mNext;
    tri[2]= tri[1]->mNext;

    Edge *scan=0;
    Edge *scan_prev=0;
    Edge *scan_next=0;

    if ( tri[0]->getHash() == e->getReverseHash() )
    {
      scan = tri[0];
      scan_next = tri[1];
      scan_prev = tri[2];
    }
    else if ( tri[1]->getHash() == e->getReverseHash() )
    {
      scan = tri[1];
      scan_next = tri[2];
      scan_prev = tri[0];
    }
    else
    {
      assert( tri[2]->getHash() == e->getReverseHash() );
      scan = tri[2];
      scan_next = tri[0];
      scan_prev = tri[1];
    }

    assert(scan); // we wouldn't be here unless we had a shared edge!
    assert(scan_prev);
    assert(scan_next);

    Edge *e_prev = e->mPrevious ? e->mPrevious : mTail;
    Edge *e_next = e->mNext ? e->mNext : mRoot;

    Edge *kill1 = 0;
    Edge *kill2 = 0;
    Edge *insert_one = 0;

    if ( scan_next->getHash() == e_prev->getReverseHash() )
    {
      // kill e_prev
      // kill e
      // insert scan_prev
      kill1 = e_prev;
      kill2 = e;
      insert_one = scan_prev;
    }
    else if ( scan_next->getHash() == e_next->getReverseHash() )
    {
      // kill e
      // kill e_next
      // insert scan_prev
      kill1 = e;
      kill2 = e_prev;
      insert_one = scan_prev;
    }
    else if ( scan_prev->getHash() == e_prev->getReverseHash() )
    {
      // kill e_prev
      // kill e
      // insert scan_next
      kill1 = e_prev;
      kill2 = e;
      insert_one = scan_next;
    }
    else if ( scan_prev->getHash() == e_next->getReverseHash() )
    {
      // kill e
      // kill e_next
      // insert scan_next
      kill1 = e;
      kill2 = e_next;
      insert_one = scan_next;
    }

    if ( insert_one )
    {
      Edge *insert_at = kill1->mPrevious ? kill1->mPrevious : mTail;
      assert( insert_at != kill1 );
      assert( insert_at != kill2 );
#ifdef _DEBUG
      validatePolygon(polyPoints);
#endif
      removeEdge(kill1,polyPoints);
      removeEdge(kill2,polyPoints);

      insertEdge(insert_at,insert_one,polyPoints);

      // now, after we have done the insert, we need to make sure this did not cause an additional collapse!
      Edge *collapse_point = insert_one;
      Edge *collapse_next  = collapse_point->mNext ? collapse_point->mNext : mRoot;

      while ( collapse_point->mI1 == collapse_next->mI2 && collapse_point->mI2 == collapse_next->mI1 )
      {
        Edge *collapse_previous = collapse_point->mPrevious ? collapse_point->mPrevious : mTail;

        removeEdge(collapse_point,polyPoints);
        removeEdge(collapse_next,polyPoints);

        collapse_point = collapse_previous;
        collapse_next  = collapse_point->mNext ? collapse_point->mNext : mRoot;
      }
#ifdef _DEBUG
      validatePolygon(polyPoints);
#endif
      ret = insert_one;
    }
    else
    {



      Edge *previous = e->mPrevious;

      Edge *insert = scan_next;
      insert->mPrevious = previous;

      assert(insert);

      ret = insert;

      if ( previous )
        previous->mNext = insert;
      else
      {
        assert( e == mRoot );
        mRoot = insert;
      }

      Edge *patch = insert->mNext ? insert->mNext : merge->mRoot;
      assert( patch != scan );
      insert->mNext = patch;

      patch->mPrevious = insert;
      patch->mNext     = e->mNext;
      mEcount++; // added two edges but removed one, change is one new edge

      if ( patch->mNext == 0 )
      {
        mTail = patch;
      }
      else
      {
        patch->mNext->mPrevious = patch;
      }

      addPolyPoint(ret,polyPoints,this);


    }



    merge->mRoot = 0; // we snarfed all of his edges

#ifdef _DEBUG
    validatePolygon(polyPoints);
#endif

    return ret;
  }

#ifdef _DEBUG
  void validatePolygon(Edge ** polyPoints,bool validate_bidirectional=true)
  {
    Edge *e = mRoot;
    Edge *prev = 0;

    NxU32 count = 0;

    while ( e )
    {
      count++;
      assert( e->mPrevious == prev );
      if ( e->mPrevious )
      {
        assert( e->mPrevious->mI2 == e->mI1 );
      }
      else
      {
        assert( e == mRoot );
      }
      if ( e->mNext )
      {
        assert( e->mNext->mI1 == e->mI2 );
      }

      if ( prev )
      {
        if ( validate_bidirectional && prev->mI1 == e->mI2 && prev->mI2 == e->mI1 )
        {
          assert(0);
        }
      }

      prev = e;
      e = e->mNext;
    }

    assert( prev->mI2 == mRoot->mI1 );
    assert( mTail == prev );
    assert( count == mEcount );

    if ( validate_bidirectional && mRoot->mI1 == mTail->mI2 && mRoot->mI2 == mTail->mI1 )
    {
      assert(0);
    }

    e = mRoot;
    while ( e )
    {
      assert( hasPolyPoint(this,e->mI2,polyPoints) );
      e = e->mNext;
    }

  }
#endif

  bool    mMerged:1;
  bool    mRemoved:1;

  NxU32  mEcount;
  Edge   *mRoot;
  Edge   *mTail;
  NxF32  mNormal[3];
  NxU32 mIndex; // polygon index (used for debugging)

  Polygon *mNextPolygonEdge;
};


static bool hasPolyPoint(Polygon *p,NxU32 i,Edge **polyPoints)
{
  bool ret = false;

  Edge *scan = polyPoints[i];
  while ( scan )
  {
    if ( scan->mParent == p )
    {
      ret = true;
      break;
    }
    scan = scan->mNextPolyPoint;
  }

#if 0
  bool tret = false;

  scan = p->mRoot;
  while ( scan )
  {
    if ( scan->mI2 == i )
    {
      tret = true;
      break;
    }
    scan = scan->mNext;
  }

    assert( ret == tret );
#endif

  return ret;

}

static void addPolyPoint(Edge *p,Edge **polyPoints,Polygon *parent)
{
  NxU32 i = p->mI2;

  Edge *scan = polyPoints[i];
  while ( scan )
  {
    assert( scan != p );
    scan = scan->mNextPolyPoint;
  }

  p->mParent = parent;
  if ( polyPoints[i] )
  {
    p->mNextPolyPoint = polyPoints[i];
  }
  else
  {
    p->mNextPolyPoint = 0;
  }
  polyPoints[i] = p;
}


static void removePolyPoint(Edge *p,Edge **polyPoints)
{
  NxU32 i = p->mI2;

  Edge *scan = polyPoints[i];
  Edge *prev = 0;

  while ( scan && scan != p )
  {
    prev = scan;
    scan = scan->mNextPolyPoint;
  }

//  assert( scan );
//  assert( scan == p );

  if ( scan )
  {
    if ( prev )
    {
      prev->mNextPolyPoint = p->mNextPolyPoint;
    }
    else
    {
      polyPoints[i] = p->mNextPolyPoint;
    }
    p->mNextPolyPoint = 0;
  }

}

#define SPLIT_EPSILON 0.000001f

typedef USER_STL::vector< NxU32 > HeU32Vector;
typedef USER_STL_EXT::hash_map< NxU32, Polygon * > PolygonHashMap;

class MyConsolidateMesh : public ConsolidateMesh
{
public:

  MyConsolidateMesh(void)
  {
    mPolygons = 0;
    mPolyPoints = 0;
    mEdges = 0;
    mVertexIndex = 0;
    mUseDouble   = false;
    mVertexOutput = 0;
  }

  ~MyConsolidateMesh(void)
  {
    releaseOutput();
    releaseInput();
  }

  void releaseInput(void)
  {
    delete []mPolyPoints;
    delete []mPolygons;
    delete []mEdges;
    mPolyPoints = 0;
    mPolygons = 0;
    mEdges    = 0;
    if ( mVertexIndex )
    {
      fm_releaseVertexIndex(mVertexIndex);
      mVertexIndex = 0;
    }
    mIndices.clear();
    mPolygonEdges.clear();
    mPolygonCount = 0;
    mEdgeCount    = 0;
  }

  void releaseOutput(void)
  {
    if ( mVertexOutput )
    {
      fm_releaseVertexIndex(mVertexOutput);
      mVertexOutput = 0;
    }
    mOutputIndices.clear();
  }

  virtual bool addTriangle(const NxF32 *p1,const NxF32 *p2,const NxF32 *p3)
  {
    bool ret = true;

    if ( mVertexIndex == 0 )
    {
      mVertexIndex = fm_createVertexIndex((NxF32)SPLIT_EPSILON,false);
      mUseDouble = false;
    }

    if ( !mUseDouble )
    {

      bool np;

      NxU32 i1 = mVertexIndex->getIndex(p1,np);
      NxU32 i2 = mVertexIndex->getIndex(p2,np);
      NxU32 i3 = mVertexIndex->getIndex(p3,np);

      if ( i1 == i2 || i1 == i3 || i2 == i3 )
      {
        ret = false;
      }
      else
      {
        mIndices.push_back(i1);
        mIndices.push_back(i2);
        mIndices.push_back(i3);
      }
    }
    else
    {
      ret = false;
    }
    return ret;
  }

  virtual bool addTriangle(const NxF64 *p1,const NxF64 *p2,const NxF64 *p3)
  {
    bool ret = true;


    if ( mVertexIndex == 0 )
    {
      mVertexIndex = fm_createVertexIndex((NxF64)SPLIT_EPSILON,false);
      mUseDouble = true;
    }

    if ( mUseDouble )
    {

      bool np;

      NxU32 i1 = mVertexIndex->getIndex(p1,np);
      NxU32 i2 = mVertexIndex->getIndex(p2,np);
      NxU32 i3 = mVertexIndex->getIndex(p3,np);

      if ( i1 == i2 || i1 == i3 || i2 == i3 )
      {
        ret = false;
      }
      else
      {
        mIndices.push_back(i1);
        mIndices.push_back(i2);
        mIndices.push_back(i3);
      }
    }
    else
    {
      ret = false;
    }

    return ret;
  }

  Edge * addEdge(Edge *e,Polygon *p,NxU32 i1,NxU32 i2)
  {
    e->init(i1,i2,p);

    addPolyPoint(e,mPolyPoints,p);

    p->addEdge(e);

    PolygonHashMap::iterator found;
    found = mPolygonEdges.find(e->getHash());
    if ( found == mPolygonEdges.end() )
    {
      mPolygonEdges[ e->getHash() ] = p;
    }
    else
    {
      Polygon *pn = (*found).second;
      p->mNextPolygonEdge = pn;
      (*found).second = p;
    }

    e++;
    return e;
  }


  virtual bool consolidateMesh(ConsolidateMeshOutput &output)
  {
    bool ret = false;

    output.mVcount         = 0;
    output.mTcount         = 0;
    output.mVerticesFloat  = 0;
    output.mVerticesDouble = 0;
    output.mIndices        = 0;

    releaseOutput();

    mPolygonCount = mIndices.size()/3;
    mEdgeCount    = mPolygonCount*3;

    if ( mPolygonCount > 0 )
    {
      if ( mVertexIndex->getVcount() >= 65536 )
      {
        printf("Input mesh has more than 65536 unique vertices.  This mesh is too large for to be consolidated with this algorithm.\r\n");
      }
      else
      {

        if ( mUseDouble )
          mVertexOutput = fm_createVertexIndex((NxF64)SPLIT_EPSILON,false);
        else
          mVertexOutput = fm_createVertexIndex((NxF32)SPLIT_EPSILON,false);

        NxU32 vcount = mVertexIndex->getVcount();
        mPolyPoints = new Edge*[vcount];
        mPolygons = new Polygon[mPolygonCount];
        mEdges    = new Edge[mEdgeCount];
        memset(mPolyPoints,0,sizeof(Edge *)*vcount);


        Polygon *p = mPolygons;
        Edge    *e = mEdges;

        const NxU32 *scan = &mIndices[0];
        for (NxU32 i=0; i<mPolygonCount; i++)
        {

          NxU32 i1 = scan[0];
          NxU32 i2 = scan[1];
          NxU32 i3 = scan[2];

          p->mIndex = i;

          if ( mUseDouble )
          {
            const NxF64 *p1 = mVertexIndex->getVertexDouble(i1);
            const NxF64 *p2 = mVertexIndex->getVertexDouble(i2);
            const NxF64 *p3 = mVertexIndex->getVertexDouble(i3);
            NxF64 normal[3];
            fm_computePlane(p1,p2,p3,normal);
            fm_doubleToFloat3(normal,p->mNormal);
          }
          else
          {
            const NxF32 *p1 = mVertexIndex->getVertexFloat(i1);
            const NxF32 *p2 = mVertexIndex->getVertexFloat(i2);
            const NxF32 *p3 = mVertexIndex->getVertexFloat(i3);
            fm_computePlane(p1,p2,p3,p->mNormal);
          }

          e = addEdge(e,p,i1,i2);
          e = addEdge(e,p,i2,i3);
          e = addEdge(e,p,i3,i1);

          p++;
          scan+=3;
        }

        // Consolidate all triangles into polygons in a single pass.
        {
          Polygon *p = mPolygons;
          for (NxU32 i=0; i<mPolygonCount; i++)
          {
            consolidate(p,i);
            p++;
          }
        }


        {
          Polygon *p = mPolygons;
          fm_Triangulate *t = fm_createTriangulate(); // create the 3d polygon triangulator
          for (NxU32 j=0; j<mPolygonCount; j++)
          {
            if ( !p->mRemoved )
            {

#if DEBUG_SHOW
              p->debug(0,mVertexIndex);
#endif
              #define MAX_VERTS 2048
              assert( p->mEcount < MAX_VERTS );
              if ( p->mEcount < MAX_VERTS )
              {

                NxF64 vertices[MAX_VERTS*3];
                NxF64 _vertices[MAX_VERTS*3];

                Edge *e = p->mRoot;
                NxU32 pcount = 0;
                NxF64 *dest = _vertices;
                while ( e )
                {
                  if ( mUseDouble )
                  {
                    const NxF64 *p = mVertexIndex->getVertexDouble(e->mI2);
                    dest[0] = p[0];
                    dest[1] = p[1];
                    dest[2] = p[2];
                  }
                  else
                  {
                    const NxF32 *p = mVertexIndex->getVertexFloat(e->mI2);
                    dest[0] = (NxF64)p[0];
                    dest[1] = (NxF64)p[1];
                    dest[2] = (NxF64)p[2];
                  }
                  dest+=3;
                  pcount++;
                  e = e->mNext;
                }

                pcount = fm_consolidatePolygon(pcount,_vertices,sizeof(NxF64)*3,vertices);

                switch ( pcount )
                {
                  case 0:
                  case 1:
                  case 2:
//                    assert(0);
                    break;
                  case 3:
                    if ( mUseDouble )
                    {
                      addTriangleOutput(vertices,vertices+3,vertices+6);
                    }
                    else
                    {
                      NxF32 p1[3];
                      NxF32 p2[3];
                      NxF32 p3[3];
                      fm_doubleToFloat3(vertices,p1);
                      fm_doubleToFloat3(vertices+3,p2);
                      fm_doubleToFloat3(vertices+6,p3);
                      addTriangleOutput(p1,p2,p3);
                    }
                    break;
                  case 4:
                    if ( mUseDouble )
                    {
                      const NxF64 *p1 = vertices;
                      const NxF64 *p2 = vertices+3;
                      const NxF64 *p3 = vertices+6;
                      const NxF64 *p4 = vertices+9;
                      addTriangleOutput(p1,p2,p3);
                      addTriangleOutput(p1,p3,p4);
                    }
                    else
                    {
                      NxF32 p1[3];
                      NxF32 p2[3];
                      NxF32 p3[3];
                      NxF32 p4[3];
                      fm_doubleToFloat3(vertices,p1);
                      fm_doubleToFloat3(vertices+3,p2);
                      fm_doubleToFloat3(vertices+6,p3);
                      fm_doubleToFloat3(vertices+9,p4);
                      addTriangleOutput(p1,p2,p3);
                      addTriangleOutput(p1,p3,p4);
                    }
                    break;
                  default:
                    {
                      NxU32 tcount;
                      const NxF64 *triangles = t->triangulate3d(pcount,vertices,sizeof(NxF64)*3,tcount,false,SPLIT_EPSILON);

                      if ( triangles )
                      {
                        for (NxU32 i=0; i<tcount; i++)
                        {
                          const NxF64 *p1 = triangles;
                          const NxF64 *p2 = triangles+3;
                          const NxF64 *p3 = triangles+6;
                          if ( mUseDouble )
                          {
                            addTriangleOutput(p3,p2,p1);
                          }
                          else
                          {
                            NxF32 _p1[3];
                            NxF32 _p2[3];
                            NxF32 _p3[3];
                            fm_doubleToFloat3(p1,_p1);
                            fm_doubleToFloat3(p2,_p2);
                            fm_doubleToFloat3(p3,_p3);
                            addTriangleOutput(_p3,_p2,_p1);
                          }
                          triangles+=9;
                        }
                      }
                      else
                      {

//                        SEND_TEXT_MESSAGE(0,"Triangulation failed on %d points.\r\n", pcount);
//
//                        for (NxU32 i=0; i<pcount; i++)
//                        {
//                          const NxF64 *p = &vertices[i*3];
//                          SEND_TEXT_MESSAGE(0,"Point%d : %0.9f, %0.9f, %0.9f \r\n", i+1, (NxF32)p[0],(NxF32)p[1],(NxF32)p[2]);
//                        }
                      }
                    }
                    break;

                }
              }
            }
            p++;
          }

          fm_releaseTriangulate(t);
          releaseInput();

          if ( !mOutputIndices.empty() )
          {
            ret = true;
            output.mVcount = mVertexOutput->getVcount();
            output.mTcount = mOutputIndices.size()/3;
            output.mIndices = &mOutputIndices[0];
            output.mVerticesFloat = 0;
            output.mVerticesDouble = 0;
            if ( mUseDouble )
            {
              output.mVerticesDouble = mVertexOutput->getVerticesDouble();
            }
            else
            {
              output.mVerticesFloat = mVertexOutput->getVerticesFloat();
            }
          }
        }
      }
    }

//    SEND_TEXT_MESSAGE(0,"Output mesh has %d triangles.\r\n", mOutputIndices.size()/3);

    return ret;
  }


  bool normalMatch(const NxF32 *n1,const NxF32 *n2) const
  {
    NxF32 dot = fm_dot(n1,n2);
    return dot >= 0.9999f && dot <= 1.0001f ? true : false;
  }

  Polygon * sharedEdge(NxU32 hash,Polygon *check)
  {
    Polygon *ret = 0;

    PolygonHashMap::iterator found;
    found = mPolygonEdges.find( hash );
    if ( found != mPolygonEdges.end() )
    {
      Polygon *p = (*found).second;
      while ( p )
      {
        assert( p != check );
        if ( normalMatch(p->mNormal,check->mNormal) )
        {

          ret = p;
          break;
        }
        p = p->mNextPolygonEdge;
      }
    }

    return ret;
  }

  void removeEdge(Polygon *p,Edge *e)
  {
    PolygonHashMap::iterator found;
    found = mPolygonEdges.find( e->getHash() );
    if ( found != mPolygonEdges.end() )
    {
      Polygon *prev = 0;
      Polygon *scan = (*found).second;
      while ( scan && scan != p )
      {
        prev = scan;
        scan = scan->mNextPolygonEdge;
      }
      if ( scan )
      {
        if ( prev == 0 )
        {
          if ( scan->mNextPolygonEdge )
          {
            (*found).second = scan->mNextPolygonEdge;
          }
          else
          {
            mPolygonEdges.erase(found); // no more polygons have an edge here
          }
        }
        else
        {
          prev->mNextPolygonEdge = scan->mNextPolygonEdge;
        }
      }
    }
    else
    {
      assert(0); // impossible!
    }
  }

  void remove(Polygon *p)
  {
    Edge *e = p->mRoot;
    while ( e )
    {
      removeEdge(p,e);
      e = e->mNext;
    }
  }

  void removePolyPoints(Polygon *p)
  {
    assert( p->mEcount == 3 );
    Edge *e1 = p->mRoot;
    Edge *e2 = e1->mNext;
    Edge *e3 = e2->mNext;
    removePolyPoint(e1,mPolyPoints);
    removePolyPoint(e2,mPolyPoints);
    removePolyPoint(e3,mPolyPoints);
  }

  void consolidate(Polygon *p,NxU32 index)
  {

    NxU32 merge_count = 0;

    if ( !p->mRemoved )  // if this is not a polygon which has already been removed (folded up into another)
    {

      remove(p);         // remove it

      // first see if we share any edges...
      Edge *e = p->mRoot;

      while ( e )
      {

        Polygon *merge = sharedEdge(e->getReverseHash(),p);

        if ( merge )
        {
          Edge *insertion_point = p->determineInsertionPoint(merge,e,mPolyPoints);

          if ( insertion_point )
          {
            NxU32 new_point = insertion_point->mI2;
            if ( hasPolyPoint(p,new_point,mPolyPoints) )
            {
              merge = 0;
            }
          }

          if ( merge )
          {
            removePolyPoints(merge);
            remove(merge);
            e = p->mergePolygon(merge,e,mPolyPoints);
            merge_count++;
          }
          else
          {
            e = e->mNext; // we skipped this triangle because it would have inserted a point touching the polygon we are building
          }
        }
        else
        {
          e = e->mNext;
        }
      }

    }
  }


#pragma warning(push)
#pragma warning(disable:4100)
  NxI32 getIndex(NxI32 x,NxI32 y,NxI32 wid,NxI32 hit) const
  {
    assert( x >= 0 && x < wid );
    assert( y >= 0 && y < hit );
    return (y*wid)+x+1;
  }
#pragma warning(pop)

  void addTriangleOutput(const NxF32 *p1,const NxF32 *p2,const NxF32 *p3)
  {
#if DEBUG_SHOW
    gRenderDebug->DebugSolidTri(p1,p2,p3,0x00FFFF,30.0f);
    gRenderDebug->DebugTri(p1,p2,p3,0xFFFF00,30.0f);
#endif
    assert( !mUseDouble );
    bool np;
    NxU32 i1 = mVertexOutput->getIndex(p1,np);
    NxU32 i2 = mVertexOutput->getIndex(p2,np);
    NxU32 i3 = mVertexOutput->getIndex(p3,np);
    mOutputIndices.push_back(i1);
    mOutputIndices.push_back(i2);
    mOutputIndices.push_back(i3);
  }

  void addTriangleOutput(const NxF64 *p1,const NxF64 *p2,const NxF64 *p3)
  {
#if DEBUG_SHOW
    gRenderDebug->DebugSolidTri(p1,p2,p3,0x00FFFF,30.0f);
    gRenderDebug->DebugTri(p1,p2,p3,0xFFFF00,30.0f);
#endif

    assert( mUseDouble );
    bool np;
    NxU32 i1 = mVertexOutput->getIndex(p1,np);
    NxU32 i2 = mVertexOutput->getIndex(p2,np);
    NxU32 i3 = mVertexOutput->getIndex(p3,np);
    mOutputIndices.push_back(i1);
    mOutputIndices.push_back(i2);
    mOutputIndices.push_back(i3);
  }

  bool                   mUseDouble;
  fm_VertexIndex        *mVertexIndex;
  Polygon               *mPolygons;
  Edge                  *mEdges;
  HeU32Vector           mIndices;
  NxU32                 mPolygonCount;
  NxU32                 mEdgeCount;
  PolygonHashMap         mPolygonEdges;

  fm_VertexIndex        *mVertexOutput;
  HeU32Vector           mOutputIndices;

  Edge                 **mPolyPoints;

};

};

using namespace CONSOLIDATE_MESH;

ConsolidateMesh * createConsolidateMesh(void)
{
  MyConsolidateMesh *mcm = new MyConsolidateMesh;
  return static_cast< ConsolidateMesh *>(mcm);
}

void              releaseConsolidateMesh(ConsolidateMesh *cm)
{
  MyConsolidateMesh *mcm = static_cast< MyConsolidateMesh *>(cm);
  delete mcm;
}

