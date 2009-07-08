#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#pragma warning(disable:4702)
#include <vector>
#include <hash_map>

#define TESSELATE_SIZE 0.2
#define NOISE_SIZE 0.2
#define PSCALE 32

#define USE_PERLIN 1

#if USE_PERLIN
#include "perlin4.h"
#endif

#pragma warning(disable:4996)

#define SHOW_DEBUG 0


#if SHOW_DEBUG
#include "RenderDebug/RenderDebug.h"
#include "log.h"
#endif
#include "UserMemAlloc.h"

#include "SplitMesh.h"
#include "FloatMath.h"
#include "RemoveTjunctions.h"

namespace SPLIT_MESH
{

static bool gFreeze=false;
static double gPlane[4] = { 1.000000000,0.000000000,0.000000000,0.948000014 };

static float ranf(void)
{
  float v = (float)rand();
  v*=(1.0f/32768.0f);
  v-=0.5f;
  return v;
}

//************************************************************************************
//************************************************************************************
//** Code to support 'RobustMeshes'; full double precision, snapped and welded vertices
//************************************************************************************
//************************************************************************************

typedef USER_STL::vector< size_t > size_tVector;

class MyRobustMesh : public RobustMesh
{
public:
  MyRobustMesh(fm_VertexIndex *vindex)
  {
    if ( vindex )
    {
      mVertexIndex = vindex;
      mMyVertexIndex = false;
    }
    else
    {
      mVertexIndex = fm_createVertexIndex( (double)SPLIT_EPSILON, false );
      mMyVertexIndex = true;
    }
  }

  ~MyRobustMesh(void)
  {
    if ( mMyVertexIndex )
    {
      fm_releaseVertexIndex(mVertexIndex);
    }
  }

  void addTriangle(const float *_p1,const float *_p2,const float *_p3,size_t id)
	{
    double p1[3];
    double p2[3];
    double p3[3];

    fm_floatToDouble3(_p1,p1);
    fm_floatToDouble3(_p2,p2);
    fm_floatToDouble3(_p3,p3);

    addTriangle(p1,p2,p3,id);
	}

  void addTriangle(const double *p1,const double *p2,const double *p3,size_t id)
	{
    bool np;

    size_t i1 = mVertexIndex->getIndex(p1,np);
    size_t i2 = mVertexIndex->getIndex(p2,np);
    size_t i3 = mVertexIndex->getIndex(p3,np);

    mIndices.push_back(i1);
    mIndices.push_back(i2);
    mIndices.push_back(i3);
    mId.push_back(id);
	}

  void iterateTriangles(RobustMeshInterface *iface)
	{

    if ( !mIndices.empty() )
    {

      const size_t *indices = &mIndices[0];
      size_t tcount = mIndices.size()/3;
      const size_t *id = &mId[0];

      for (size_t i=0; i<tcount; i++)
      {
        size_t i1 = *indices++;
        size_t i2 = *indices++;
        size_t i3 = *indices++;

        const double *v1 = mVertexIndex->getVertexDouble(i1);
        const double *v2 = mVertexIndex->getVertexDouble(i2);
        const double *v3 = mVertexIndex->getVertexDouble(i3);

        size_t _id = *id++;

        iface->iterateTriangle(v1,v2,v3,_id);
      }
    }
	}


  bool getSimpleMesh(SimpleMesh &simpleMesh)
  {
    bool ret = false;

    if ( !mIndices.empty() )
    {
      size_t *idx = &mIndices[0];
      size_t tcount   = mIndices.size()/3;
      const double *vertices = mVertexIndex->getVerticesDouble();
      size_t vcount = mVertexIndex->getVcount();
      size_t *ids = &mId[0];

      simpleMesh.setSimpleMesh(vcount,tcount,vertices,idx,ids);

      ret = true;
    }

    return ret;

  }



  bool getSimpleMeshDouble(SimpleMeshDouble &simpleMesh)
  {
    bool ret = false;

    if ( !mIndices.empty() )
    {
      size_t *idx = &mIndices[0];
      size_t tcount   = mIndices.size()/3;
      const double *vertices = mVertexIndex->getVerticesDouble();
      size_t vcount = mVertexIndex->getVcount();
      size_t *ids = &mId[0];
      simpleMesh.setSimpleMesh(vcount,tcount,vertices,idx,ids);
      ret = true;
    }

    return ret;
  }

  void getOffset(const double *p,float *t)
  {
    t[0] = (float)(p[0]+40);
    t[1] = (float)p[1];
    t[2] = (float)p[2];
  }

    bool saveObj(const char * fname)
    {
      bool ret = false;
      if ( !mIndices.empty() )
      {
        SimpleMesh s;
        getSimpleMesh(s);
        FILE *fph = fopen(fname,"wb");
        if ( fph )
        {
          const float *v = s.mVertices;
          for (size_t i=0; i<s.mVcount; i++)
          {
            fprintf(fph,"v %0.9f %0.9f %0.9f\r\n", v[0], v[1], v[2] );
            v+=3;
          }
          const size_t *indices = s.mIndices;
          for (size_t i=0; i<s.mTcount; i++)
          {
            size_t i1 = indices[0];
            size_t i2 = indices[1];
            size_t i3 = indices[2];

            fprintf(fph,"f %d %d %d\r\n",i1+1,i2+1,i3+1);

            indices+=3;
          }
          fclose(fph);
          ret = true;
        }

      }
      return ret;
    }


  fm_VertexIndex * getVertexIndex(void) { return mVertexIndex; };

  void addTriangle(size_t i1,size_t i2,size_t i3,size_t id)
  {
    mIndices.push_back(i1);
    mIndices.push_back(i2);
    mIndices.push_back(i3);
    mId.push_back(id);
  }

  void addTriangles(size_t tcount,const size_t *indices,size_t id)
  {
    for (size_t i=0; i<(tcount*3); i++)
    {
      mIndices.push_back(*indices++);
    }
    for (size_t i=0; i<tcount; i++)
    {
      mId.push_back(id);
    }
  }

private:
  bool                  mMyVertexIndex;
  fm_VertexIndex       *mVertexIndex;
  size_tVector          mIndices;
  size_tVector          mId;
};




RobustMesh * createRobustMesh(fm_VertexIndex *vindex)
{
  MyRobustMesh * mesh = new MyRobustMesh(vindex);
  return static_cast< RobustMesh *>(mesh);
}

void         releaseRobustMesh(RobustMesh *mesh)
{
  MyRobustMesh *rm = static_cast< MyRobustMesh *>(mesh);
  delete rm;
}



//************************************************************************************
//*** End of Robust Mesh Code
//************************************************************************************


//************************************************************************************
//************************************************************************************
//** Code to support 'RingSystems' edges that will be converted into polygons
//************************************************************************************
//************************************************************************************


typedef USER_STL::vector< double >  doubleVector;

class Edge
{
public:

  Edge(size_t i1,size_t i2,fm_VertexIndex * /*vpool*/)
  {
    mI1        = i1;
    mI2        = i2;
  }


  size_t     mI1;
  size_t     mI2;

};

typedef USER_STL::vector< Edge > EdgeVector;
typedef USER_STL::vector< Edge *> EdgePtrVector;

typedef USER_STL_EXT::hash_map<size_t, Edge *> EdgeIndexMap;
typedef USER_STL::vector< size_t > SizeTVector;
typedef USER_STL::vector< int > intVector;


class Ring
{
public:
  Ring(Edge *e)
  {

    mPoints.push_back(e->mI1);
    mPoints.push_back(e->mI2);

    mClosed = false;
    mConcave = false;
    mCenter[0] = 0;
    mCenter[1] = 0;
    mCenter[2] = 0;
  }

  Ring(size_t i1,size_t i2)
  {
    mPoints.push_back(i1);
    mPoints.push_back(i2);

    mClosed = false;
    mConcave = false;
    mCenter[0] = 0;
    mCenter[1] = 0;
    mCenter[2] = 0;
  }

  void add(size_t i,fm_VertexIndex * pool,bool collapseColinear)
  {

    for (size_t j=0; j<mPoints.size(); j++)
    {
      size_t e = mPoints[j];
      if ( e == i )
        return;
      HE_ASSERT( e != i );
    }

#if 1
    if ( collapseColinear )
    {
      size_t pindex = (size_t)(mPoints.size()-2);

      size_t i1 = mPoints[pindex];
      size_t i2 = mPoints[pindex+1];
      size_t i3 = i;

      const double *p1 = pool->getVertexDouble(i1);
      const double *p2 = pool->getVertexDouble(i2);
      const double *p3 = pool->getVertexDouble(i3);

      if ( fm_colinear(p1,p2,p3) )
      {
        mPoints[pindex+1] = i;
      }
      else
      {
        mPoints.push_back(i);
      }
    }
    else
#endif
    {
      mPoints.push_back(i);
    }
  }

  void setClosed(bool state,fm_VertexIndex *pool,bool collapseColinear)
  {
    mClosed = state;
    if ( collapseColinear )
    {
      size_t pindex = (size_t)(mPoints.size()-2);

      size_t i1 = mPoints[pindex];
      size_t i2 = mPoints[pindex+1];
      size_t i3 = 0;

      const double *p1 = pool->getVertexDouble(i1);
      const double *p2 = pool->getVertexDouble(i2);
      const double *p3 = pool->getVertexDouble(i3);

      if ( fm_colinear(p1,p2,p3) )
      {
        size_t count = (size_t)(mPoints.size());
        mPoints.resize(count-1);
      }
    }
  }

  bool isClosed(void) const { return mClosed; };
  bool isConcave(void) const { return mConcave; };

  void getCenter(double *center) const
  {
    if ( center )
    {
      center[0] = mCenter[0];
      center[1] = mCenter[1];
      center[2] = mCenter[2];
    }
  }

  void normalize(double *p) const
  {
    double dist = sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2]);
    double recip = 1 / dist;
    p[0]*=recip;
    p[1]*=recip;
    p[2]*=recip;
  }

  bool concave(size_t a,size_t b,size_t c,fm_VertexIndex *p) const
  {
    bool ret = false;

    const double *p1 = p->getVertexDouble(a);
    const double *p2 = p->getVertexDouble(b);
    const double *p3 = p->getVertexDouble(c);

    double e1[3];
    double e2[3];

    e1[0] = p1[0] - p2[0];
    e1[1] = p1[1] - p2[1];
    e1[2] = p1[2] - p2[2];

    e2[0] = p3[0] - p2[0];
    e2[1] = p3[1] - p2[1];
    e2[2] = p3[2] - p2[2];

    normalize(e1);
    normalize(e2);

    double dot = e1[0]*e2[0]+e1[1]*e2[1]+e1[2]*e2[2];

    if ( dot > 0.000001 )
    {
      ret = true;
    }
    return ret;
  }

  void computeConvexity(fm_VertexIndex *pool)
  {
    mConcave = false;
    size_t count = mPoints.size();

    if ( count >= 3 )
    {
      size_t a = mPoints[count-1];
      size_t b = mPoints[0];
      size_t c = mPoints[1];
      if ( concave(a,b,c,pool) )
      {
        mConcave = true;
      }
      else
      {
        for (size_t i=2; i<count; i++)
        {
          a = b;
          b = c;
          c = mPoints[i];
          if ( concave(a,b,c,pool) )
          {
            mConcave = true;
            break;
          }
        }
      }

      double bmin[3];
      double bmax[3];

      for (size_t i=0; i<count; i++)
      {
        size_t a = mPoints[i];
        const double *pos = pool->getVertexDouble(a);
        if ( i==0)
        {
          bmin[0] = pos[0];
          bmin[1] = pos[1];
          bmin[2] = pos[2];
          bmax[0] = pos[0];
          bmax[1] = pos[1];
          bmax[2] = pos[2];
        }
        else
        {
          if ( pos[0] < bmin[0] ) bmin[0] = pos[0];
          if ( pos[1] < bmin[1] ) bmin[1] = pos[1];
          if ( pos[2] < bmin[2] ) bmin[2] = pos[2];

          if ( pos[0] > bmax[0] ) bmax[0] = pos[0];
          if ( pos[1] > bmax[1] ) bmax[1] = pos[1];
          if ( pos[2] > bmax[2] ) bmax[2] = pos[2];
        }
      }

      mCenter[0] = (bmin[0]+bmax[0])*0.5;
      mCenter[1] = (bmin[1]+bmax[1])*0.5;
      mCenter[2] = (bmin[2]+bmax[2])*0.5;
    }

  }

  bool        mClosed:1;
  bool        mConcave:1;
  double       mCenter[3];
  SizeTVector mPoints;
};

typedef USER_STL::vector< Ring > RingVector;

class LineSegment
{
public:

  LineSegment(const double *p1,const double *p2,fm_VertexIndex *vindex)
  {
    copy(mP1,p1);
    copy(mP2,p2);

    bool new1,new2;

    mI1    = vindex->getIndex(mP1,new1);
    mI2    = vindex->getIndex(mP2,new2);

    if ( mI1 == mI2 )
    {
//      gLog->Display("WARNING: segment too small!\r\n");
    }

    mConsumed = false;

  }

  bool collapse(const double *p1,const double *p2) const
  {
    double d = fm_distanceSquared(p1,p2);
    return  ( d < (SPLIT_EPSILON*SPLIT_EPSILON) ) ? true : false;
  }

  void copy(double *dest,const double *src) const
  {
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
  }

  bool collapse(LineSegment &s)
  {
    bool ret = false;

    if ( !s.mConsumed )
    {
      if ( mI2 == s.mI1 )
      {
        if ( fm_colinear(mP1,mP2,s.mP2) )
        {
          copy(mP2,s.mP2);
          mI2 = s.mI2;
          s.mConsumed = true;
          ret = true;
        }
      }
      else if ( mI1 == s.mI2 )
      {
        if ( fm_colinear(s.mP1,s.mP2,mP2) )
        {
          copy(mP1,s.mP1);
          mI1 = s.mI1;
          s.mConsumed = true;
          ret = true;
        }
      }
    }
    return ret;
  }

  bool    mConsumed:1;
  bool    mCollapsed:1;

  size_t   mI1;
  size_t   mI2;

  double  mP1[3];
  double  mP2[3];
};

typedef USER_STL::vector< LineSegment > LineSegmentVector;

class MyRingSystem : public RingSystem
{
public:
  MyRingSystem(fm_VertexIndex *vindex)
  {
    mPlane[0] = 0;
    mPlane[1] = 1;
    mPlane[2] = 0;
    mPlane[3] = 0;
    mAxis      = FM_YAXIS;
    if ( vindex )
    {
      mVertexIndex = vindex;
      mMyVertexIndex = false;
    }
    else
    {
      mVertexIndex = fm_createVertexIndex( (double)SPLIT_EPSILON, false );
      mMyVertexIndex = true;
    }
  }

  ~MyRingSystem(void)
  {
    if ( mMyVertexIndex )
    {
      fm_releaseVertexIndex(mVertexIndex);
    }
  }

  void addLineSegment(const double *p1,const double *p2)
  {
    LineSegment s(p1,p2,mVertexIndex);
    mLineSegments.push_back(s);
  }

  bool collapse(LineSegment &s)
  {
    bool ret = false;

    LineSegmentVector::iterator i;
    for (i=mLineSegments.begin(); i!=mLineSegments.end(); i++)
    {
      if ( (*i).collapse(s) )
      {
        ret = true;
        break;
      }
    }
    return ret;
  }

  void edgeStitch(void)
  {
    if ( !mLineSegments.empty() )
    {

#if 0
      bool consumed = false;

      do
      {
        consumed = false;
        LineSegmentVector::iterator i;
        for (i=mLineSegments.begin(); i!=mLineSegments.end(); i++)
        {
          LineSegment &s = (*i);
          if ( !s.mConsumed )
          {
            if ( collapse(s) )
            {
              consumed = true;
            }
          }
        }
      } while ( consumed );
#endif

      LineSegmentVector::iterator i;
      for (i=mLineSegments.begin(); i!=mLineSegments.end(); i++)
      {
        LineSegment &s = (*i);
        if ( !s.mConsumed )
        {
          Edge e(s.mI1,s.mI2,mVertexIndex);
          mEdges.push_back(e);
        }
      }
    }
  }

  size_t buildPolygons(bool collapseColinear,bool edgeIntersect,bool saveResults)
  {
    size_t count = mEdges.size();
    if ( count )
    {

      //**********************************
      if ( edgeIntersect )
      {
        fm_LineSegment *segments = new fm_LineSegment[count];
        Edge *e = &mEdges[0];
        fm_LineSegment *dest = segments;
        for (size_t i=0; i<count; i++)
        {
          dest->mE1 = e->mI1;
          dest->mE2 = e->mI2;
          dest++;
          e++;
        }

        fm_LineSweep *sweep = fm_createLineSweep();
        size_t scount;
        fm_LineSegment *output = sweep->performLineSweep(segments,count,mPlane,mVertexIndex,scount);

        mEdges.clear();
        mEdges.reserve(scount);
        for (size_t i=0; i<scount; i++)
        {
          Edge e( output->mE1, output->mE2, mVertexIndex );
          mEdges.push_back(e);
          output++;
        }


        fm_releaseLineSweep(sweep);
        delete []segments;
      }

      //**********************************

      EdgeWalker ew( mVertexIndex->getVcount(), mVertexIndex, saveResults );


      size_t ecount = mEdges.size();
      Edge *e = &mEdges[0];
      for (size_t i=0; i<ecount; i++)
      {
        ew.addEdge( e->mI1, e->mI2 );
        e++;
      }


      ew.process();
      size_t pcount = ew.getPolygonCount();

#if 0
      if ( pcount == 0 )
      {
        gPlane[0] = mPlane[0];
        gPlane[1] = mPlane[1];
        gPlane[2] = mPlane[2];
        gPlane[3] = mPlane[3];
        if ( gFreeze == false )
        {
          gLog->Display("Plane: %0.9f,%0.9f,%0.9f,%0.9f\r\n", mPlane[0], mPlane[1], mPlane[2], mPlane[3] );
        }
        gFreeze = true;
      }
#endif

      for (size_t i=0; i<pcount; i++)
      {
        size_t pc;
        const size_t *indices = ew.getPolygon(i,pc);
        if ( indices )
        {
          Ring r(indices[0],indices[1]);
          for (size_t i=2; i<pc; i++)
          {
            r.add(indices[i], mVertexIndex, collapseColinear );
          }
          r.setClosed(true,mVertexIndex,collapseColinear);
				  mRings.push_back(r);
        }
      }
    }
    return mRings.size();
  }

  size_t getPolygonCount(void) // report the number of rings.
  {
    size_t ret = 0;

    ret = mRings.size();

    return ret;
  }

  double *getPolygon(size_t polygonIndex,size_t &vertexCount,bool &closed,bool &concave,double *center)
  {
    double *ret = 0;
    vertexCount = 0;
    closed = false;
    mResults.clear();

    HE_ASSERT( polygonIndex < mRings.size() );

    if ( polygonIndex < mRings.size() )
    {
      const Ring &r = mRings[polygonIndex];
      closed = r.isClosed();
      concave = r.isConcave();
      r.getCenter(center);
      if ( !r.mPoints.empty() )
      {
        SizeTVector::const_iterator i;
        for (i=r.mPoints.begin(); i!=r.mPoints.end(); ++i)
        {
          size_t v = (*i);
          const double *pos = mVertexIndex->getVertexDouble(v);
          mResults.push_back(pos[0]);
          mResults.push_back(pos[1]);
          mResults.push_back(pos[2]);
        }
        ret = &mResults[0];
        vertexCount = r.mPoints.size();
      }

    }


    return ret;
  }


  double *getEdges(size_t &edgeCount)
  {
    double *ret = 0;

    mResults.clear();
    edgeCount = mEdges.size();

    EdgeVector::const_iterator i;
    for (i=mEdges.begin(); i!=mEdges.end(); ++i)
    {
      const Edge &e = (*i);
      const double * p1 = mVertexIndex->getVertexDouble( e.mI1 );
      const double * p2 = mVertexIndex->getVertexDouble( e.mI2 );

      mResults.push_back( p1[0] );
      mResults.push_back( p1[1] );
      mResults.push_back( p1[2] );

      mResults.push_back( p2[0] );
      mResults.push_back( p2[1] );
      mResults.push_back( p2[2] );
    }

    if ( edgeCount )
    {
      ret = &mResults[0];
    }

    return ret;
  }

  const double * getTriangulation(size_t &tcount)  // returns the delauney triangulation
  {
    const double *ret = 0;
    tcount = 0;

    if ( mTriangles.empty() && !mEdges.empty() )
    {
    }

    if ( !mTriangles.empty() )
    {
      ret = &mTriangles[0];
      tcount = mTriangles.size()/(3*3);
    }

    return ret;
  }


  void setPlaneEquation(const double *plane)
  {
    mPlane[0] = plane[0];
    mPlane[1] = plane[1];
    mPlane[2] = plane[2];
    mPlane[3] = plane[3];
    mAxis     = fm_getDominantAxis(mPlane);
  }

private:
  bool        mMyVertexIndex;
  EdgeVector   mEdges;
  fm_VertexIndex  *mVertexIndex;
  doubleVector  mResults;
  RingVector   mRings;
  doubleVector  mTriangles;
  double        mPlane[4];
  FM_Axis      mAxis;
  LineSegmentVector mLineSegments;
};


RingSystem *createRingSystem(fm_VertexIndex *vindex)
{
  MyRingSystem *mrs = new MyRingSystem(vindex);
  return static_cast< RingSystem *>(mrs);
}

void        releaseRingSystem(RingSystem *rs)
{
  MyRingSystem *mrs = static_cast< MyRingSystem *>(rs);
  delete mrs;
}



//************************************************************************************
//*** End of the RingSystem code
//************************************************************************************


//************************************************************************************
//************************************************************************************
//** Code to support 'SplitMesh'
//************************************************************************************
//************************************************************************************


static const double EPSILON=0.0001;

class BuildRingSystem : public RobustMeshInterface
{
public:

  RingSystem * buildRingSystem(RobustMesh *mesh,double *plane,bool collapseColinear,bool edgeIntersect,bool saveResults,fm_VertexIndex *vindex)
  {

    mVertexIndex = vindex;

    mPlane[0] = plane[0];
    mPlane[1] = plane[1];
    mPlane[2] = plane[2];
    mPlane[3] = plane[3];
    mRingSystem = createRingSystem(mVertexIndex);

    mRingSystem->setPlaneEquation(mPlane);
    mesh->iterateTriangles(this);
    mRingSystem->edgeStitch();
    mRingSystem->buildPolygons(collapseColinear,edgeIntersect,saveResults);

    return mRingSystem;
  }

  inline double distTo(const double *plane,const double *p) const
  {
    return plane[3]+(p[0]*plane[0]+p[1]*plane[1]+p[2]*plane[2]);
  }

  bool onPlane(const double *p) const
  {
    bool ret = false;

    double d = fabs(distTo(mPlane,p));

    if ( d < EPSILON )
    {
      ret = true;
    }

    return ret;
  }

  void iterateTriangle(const double *p1,const double *p2,const double *p3,size_t /*id*/)
  {
    bool np;

    mVertexIndex->getIndex(p1,np);
    mVertexIndex->getIndex(p2,np);
    mVertexIndex->getIndex(p3,np);

    if ( onPlane(p1) || onPlane(p2) || onPlane(p3) )
    {

      if ( onPlane(p1) && onPlane(p2) )
      {
        mRingSystem->addLineSegment(p1,p2);
      }

      if ( onPlane(p2) && onPlane(p3) )
      {
        mRingSystem->addLineSegment(p2,p3);
      }

      if ( onPlane(p3) && onPlane(p1) )
      {
        mRingSystem->addLineSegment(p3,p1);
      }
    }
  }

private:
  RingSystem *mRingSystem;
  double      mPlane[4];
  fm_VertexIndex *mVertexIndex;

};

class MySplitMesh : public SplitMesh, public RobustMeshInterface
{
public:
  MySplitMesh(fm_VertexIndex *vindex)
  {
    mLeftMesh = 0;
    mRightMesh = 0;
    mLeftRingSystem = 0;
    mVertexIndex = vindex;
  }

  ~MySplitMesh(void)
  {
    release();
  }

  void release(void)
  {
    if ( mLeftMesh )       releaseRobustMesh(mLeftMesh);
    if ( mRightMesh )      releaseRobustMesh(mRightMesh);
    if ( mLeftRingSystem ) releaseRingSystem(mLeftRingSystem);
    mLeftRingSystem = 0;
    mLeftMesh = 0;
    mRightMesh = 0;

  }

  bool splitMesh(RobustMesh *rm,const double *plane,bool collapseColinear,bool edgeIntersect,bool saveResults)
  {
    bool ret = false;

    release();

    mPlane[0] = (double)plane[0];
    mPlane[1] = (double)plane[1];
    mPlane[2] = (double)plane[2];
    mPlane[3] = (double)plane[3];

    if ( gFreeze )
    {
      mPlane[0] = gPlane[0];
      mPlane[1] = gPlane[1];
      mPlane[2] = gPlane[2];
      mPlane[3] = gPlane[3];
    }

    mLeftMesh  = createRobustMesh(mVertexIndex);
    mRightMesh = createRobustMesh(mVertexIndex);

    rm->iterateTriangles(this);


    BuildRingSystem brs;

    mLeftRingSystem  = brs.buildRingSystem(mLeftMesh,mPlane,collapseColinear,edgeIntersect,saveResults,mLeftMesh->getVertexIndex());

    return ret;
  }

  void buildClosedMesh(bool tesselate)
  {
    buildClosure(mLeftRingSystem,mLeftMesh,false,tesselate);
    buildClosure(mLeftRingSystem,mRightMesh,true,tesselate);
  }

  void buildClosure(RingSystem *ring,RobustMesh *mesh,bool flip,bool tesselate)
  {
    size_t pcount = ring->getPolygonCount();
    for (size_t i=0; i<pcount; i++)
    {
      size_t pcount;
      bool closed;
      bool concave;
      double center[3];
      double *_points = ring->getPolygon(i,pcount,closed,concave,center);
      if ( _points && closed )
      {
        fm_Triangulate *t = fm_createTriangulate();
        size_t tcount;

        const double *vertices = t->triangulate3d(pcount,_points,sizeof(double)*3,tcount,true,SPLIT_EPSILON);
        if ( vertices )
        {
          size_tVector indices;
          for (size_t i=0; i<tcount; i++,vertices+=9)
          {
            const double *dp1 = vertices;
            const double *dp2 = vertices+3;
            const double *dp3 = vertices+6;

            bool newp;
            size_t i1 = mVertexIndex->getIndex(dp1,newp);
            size_t i2 = mVertexIndex->getIndex(dp2,newp);
            size_t i3 = mVertexIndex->getIndex(dp3,newp);

            if ( flip )
            {
              indices.push_back(i3);
              indices.push_back(i2);
              indices.push_back(i1);
            }
            else
            {
              indices.push_back(i1);
              indices.push_back(i2);
              indices.push_back(i3);
            }
          }
          fm_releaseTriangulate(t);

          if ( tesselate )
          {
            fm_Tesselate *tess = fm_createTesselate();

            size_t tcount = indices.size()/3;
            size_t outcount;

            const size_t *idx = tess->tesselate(mVertexIndex,tcount,&indices[0],(float)TESSELATE_SIZE,8,outcount);

            mesh->addTriangles(outcount,idx,CLOSURE_ID);

            fm_releaseTesselate(tess);
          }
          else
          {
            mesh->addTriangles(tcount,&indices[0],CLOSURE_ID);
          }

        }
      }
    }

  }

  RobustMesh * getLeftMesh(void)
  {
    return mLeftMesh;
  }

  RobustMesh * getRightMesh(void)
  {
    return mRightMesh;
  }

  void iterateTriangle(const double *p1,const double *p2,const double *p3,size_t id)
  {
    double triangle[9];

    triangle[0] = p1[0];
    triangle[1] = p1[1];
    triangle[2] = p1[2];

    triangle[3] = p2[0];
    triangle[4] = p2[1];
    triangle[5] = p2[2];

    triangle[6] = p3[0];
    triangle[7] = p3[1];
    triangle[8] = p3[2];

    double front[3*5];
    double back[3*5];

    size_t fcount;
    size_t bcount;

    PlaneTriResult result = fm_planeTriIntersection(mPlane,triangle,sizeof(double)*3, 0.00000001, front, fcount, back, bcount );

    switch ( result )
    {
      case PTR_FRONT:
        mLeftMesh->addTriangle( p1, p2, p3, id );
        break;
      case PTR_BACK:
        mRightMesh->addTriangle(p1,p2,p3, id);
        break;
      case PTR_SPLIT:
        if ( fcount )
        {
          mLeftMesh->addTriangle(&front[0],&front[3],&front[6],id);
          if ( fcount == 4 )
          {
            mLeftMesh->addTriangle(&front[0],&front[6],&front[9],id);
          }
        }
        if ( bcount )
        {
          mRightMesh->addTriangle(&back[0],&back[3],&back[6],id);
          if ( bcount == 4 )
          {
            mRightMesh->addTriangle(&back[0],&back[6],&back[9],id);
          }
        }
        break;

    }

  }

  RingSystem * getLeftRingSystem(void)
  {
    return mLeftRingSystem;
  }

private:
  fm_VertexIndex *mVertexIndex;
  RobustMesh  *mLeftMesh;
  RobustMesh  *mRightMesh;
  double        mPlane[4];
  RingSystem  *mLeftRingSystem;
};


SplitMesh * createSplitMesh(fm_VertexIndex *vindex)
{
  MySplitMesh *msm = new MySplitMesh(vindex);
  return static_cast< SplitMesh *>(msm);
}

void        releaseSplitMesh(SplitMesh *sm)
{
  MySplitMesh *msm = static_cast< MySplitMesh *>(sm);
  delete msm;
}

#if USE_PERLIN
#pragma warning(disable:4305)
static float adjustV(float v,float vmin,float vmax)
{
  v-=vmin;
  v = v / (vmax-vmin);
  v-=0.5f;
  v*=NOISE_SIZE;
  return v;
}


#endif

//************************************************************************************
//**** End of the 'SplitMesh' source code
//************************************************************************************
bool splitMesh(const SimpleMesh &inputMesh,SimpleMesh &leftMesh,SimpleMesh &rightMesh,const float *planeEquation,bool tesselate,bool remove_tjunctions,bool build_closure,bool perlin_noise)
{
  bool ret = true;

  RobustMesh *mesh = createRobustMesh(0);


  fm_VertexIndex *vindex = mesh->getVertexIndex();

  size_t ivcount = inputMesh.mVcount;
  size_t *remap_indices = new size_t[ivcount];
  memset(remap_indices,0xFF,sizeof(size_t)*ivcount);

  const size_t *ids = inputMesh.mIds;

  for (size_t i=0; i<inputMesh.mTcount; i++)
  {
    size_t i1 = inputMesh.mIndices[i*3+0];
    size_t i2 = inputMesh.mIndices[i*3+1];
    size_t i3 = inputMesh.mIndices[i*3+2];

    HE_ASSERT( i1 >= 0 && i1 < ivcount );
    HE_ASSERT( i2 >= 0 && i2 < ivcount );
    HE_ASSERT( i3 >= 0 && i3 < ivcount );

    const float *p1 = &inputMesh.mVertices[i1*3];
    const float *p2 = &inputMesh.mVertices[i2*3];
    const float *p3 = &inputMesh.mVertices[i3*3];

    bool np;
    size_t _i1 = vindex->getIndex(p1,np);
    remap_indices[i1] = _i1;
    size_t _i2 = vindex->getIndex(p2,np);
    remap_indices[i2] = _i2;
    size_t _i3 = vindex->getIndex(p3,np);
    remap_indices[i3] = _i3;

    size_t id = 0;
    if ( ids ) id = *ids++;

    mesh->addTriangle(_i1,_i2,_i3,id);
  }

  size_t inputVcount = vindex->getVcount();


  // ok..we have created the RobustMesh
  SplitMesh *s = createSplitMesh(vindex); // create the SplitMesh system


  double plane[4];

  plane[0] = planeEquation[0];
  plane[1] = planeEquation[1];
  plane[2] = planeEquation[2];
  plane[3] = planeEquation[3];

  s->splitMesh(mesh,plane,true,true,false);

  if ( build_closure )
  {
    s->buildClosedMesh(tesselate);
  }

  RobustMesh *lm = s->getLeftMesh();

  RemoveTjunctions *rt = createRemoveTjunctions();

  if ( lm )
  {
    lm->getSimpleMesh(leftMesh);
    if ( leftMesh.mTcount && remove_tjunctions )
    {
      RemoveTjunctionsDesc desc;
      desc.mVcount = leftMesh.mVcount;
      desc.mVerticesF = leftMesh.mVertices;
      desc.mTcount = leftMesh.mTcount;
      desc.mIndices = leftMesh.mIndices;
      desc.mIds      = leftMesh.mIds;

      rt->removeTjunctions(desc);
      leftMesh.setIndices(desc.mTcountOut,desc.mIndicesOut,desc.mIds);
    }
  }

  RobustMesh *rm = s->getRightMesh();

  if ( rm )
  {
    rm->getSimpleMesh(rightMesh);
    if ( rightMesh.mTcount && remove_tjunctions )
    {
      RemoveTjunctionsDesc desc;
      desc.mVcount = rightMesh.mVcount;
      desc.mVerticesF = rightMesh.mVertices;
      desc.mTcount = rightMesh.mTcount;
      desc.mIndices = rightMesh.mIndices;
      desc.mIds      = rightMesh.mIds;
      rt->removeTjunctions(desc);
      rightMesh.setIndices(desc.mTcountOut,desc.mIndicesOut,desc.mIds);
    }
  }

  releaseRemoveTjunctions(rt);

  size_t endVcount = vindex->getVcount();
  size_t *remap = new size_t[endVcount];
  memset(remap,0xFF,sizeof(size_t)*endVcount);

  for (size_t i=0; i<ivcount; i++)
  {
    size_t r = remap_indices[i]; // the original index mapped to this vertex...
    if ( r != 0xFFFFFFFF )
    {
      remap[r] = i;
    }
  }

  leftMesh.setInitialVcount(inputVcount);
  rightMesh.setInitialVcount(inputVcount);

  leftMesh.setRemapIndex(remap);
  rightMesh.setRemapIndex(remap);

#if USE_PERLIN
  if ( perlin_noise )
  {
    Perlin4 *perlin = createPerlin4();

    float vmin = 1e9;
    float vmax = -1e9;

    for (size_t i=0; i<leftMesh.mVcount; i++)
    {
      float *p = &leftMesh.mVertices[i*3];
      float d = fabsf(fm_distToPlane(planeEquation,p));
      if ( d < 0.001f )
      {

        float v = perlin->get(p[0]*PSCALE,p[2]*PSCALE);

        if ( v < vmin )
          vmin = v;
        if ( v > vmax )
          vmax = v;

      }
    }

    for (size_t i=0; i<rightMesh.mVcount; i++)
    {
      float *p = &rightMesh.mVertices[i*3];
      float d = fabsf(fm_distToPlane(planeEquation,p));
      if ( d < 0.001f )
      {
        float v = perlin->get(p[0]*PSCALE,p[2]*PSCALE);
        if ( v < vmin )
          v = vmin;
        if ( v > vmax )
          vmax =vmin;

      }
    }



    for (size_t i=0; i<leftMesh.mVcount; i++)
    {
      float *p = &leftMesh.mVertices[i*3];
      float d = fabsf(fm_distToPlane(planeEquation,p));
      if ( d < 0.001f )
      {

        float v = perlin->get(p[0]*PSCALE,p[2]*PSCALE);

        v = adjustV(v,vmin,vmax);

        float dir[3];

        dir[0] = (float)plane[0]*v;
        dir[1] = (float)plane[1]*v;
        dir[2] = (float)plane[2]*v;

        p[0]+=dir[0];
        p[1]+=dir[1];
        p[2]+=dir[2];


      }
    }

    for (size_t i=0; i<rightMesh.mVcount; i++)
    {
      float *p = &rightMesh.mVertices[i*3];
      float d = fabsf(fm_distToPlane(planeEquation,p));
      if ( d < 0.001f )
      {
        float v = perlin->get(p[0]*PSCALE,p[2]*PSCALE);
        v = adjustV(v,vmin,vmax);

        float dir[3];
        dir[0] = (float)plane[0]*v;
        dir[1] = (float)plane[1]*v;
        dir[2] = (float)plane[2]*v;

        p[0]+=dir[0];
        p[1]+=dir[1];
        p[2]+=dir[2];
      }
    }

    releasePerlin4(perlin);
  }
#endif

  delete []remap;

  releaseSplitMesh(s);

  delete []remap_indices;


  return ret;
}

bool splitMesh(const SimpleMeshDouble &inputMesh,SimpleMeshDouble &leftMesh,SimpleMeshDouble &rightMesh,const double *planeEquation,bool tesselate,bool remove_tjunctions,bool build_closure,bool /*perlin_noise*/)
{
  bool ret = true;

  RobustMesh *mesh = createRobustMesh(0);


  fm_VertexIndex *vindex = mesh->getVertexIndex();

  size_t ivcount = inputMesh.mVcount;
  size_t *remap_indices = new size_t[ivcount];
  memset(remap_indices,0xFF,sizeof(size_t)*ivcount);


  for (size_t i=0; i<inputMesh.mTcount; i++)
  {
    size_t i1 = inputMesh.mIndices[i*3+0];
    size_t i2 = inputMesh.mIndices[i*3+1];
    size_t i3 = inputMesh.mIndices[i*3+2];

    HE_ASSERT( i1 >= 0 && i1 < ivcount );
    HE_ASSERT( i2 >= 0 && i2 < ivcount );
    HE_ASSERT( i3 >= 0 && i3 < ivcount );

    const double *p1 = &inputMesh.mVertices[i1*3];
    const double *p2 = &inputMesh.mVertices[i2*3];
    const double *p3 = &inputMesh.mVertices[i3*3];

    bool np;
    size_t _i1 = vindex->getIndex(p1,np);
    remap_indices[i1] = _i1;
    size_t _i2 = vindex->getIndex(p2,np);
    remap_indices[i2] = _i2;
    size_t _i3 = vindex->getIndex(p3,np);
    remap_indices[i3] = _i3;
    mesh->addTriangle(_i1,_i2,_i3,0);
  }

  size_t inputVcount = vindex->getVcount();


  // ok..we have created the RobustMesh
  SplitMesh *s = createSplitMesh(vindex); // create the SplitMesh system


  double plane[4];

  plane[0] = planeEquation[0];
  plane[1] = planeEquation[1];
  plane[2] = planeEquation[2];
  plane[3] = planeEquation[3];

  s->splitMesh(mesh,plane,true,true,false);

  if ( build_closure )
  {
    s->buildClosedMesh(tesselate);
  }

  RobustMesh *lm = s->getLeftMesh();

  RemoveTjunctions *rt = createRemoveTjunctions();

  if ( lm )
  {
    lm->getSimpleMeshDouble(leftMesh);
    if ( leftMesh.mTcount && remove_tjunctions )
    {
      RemoveTjunctionsDesc desc;
      desc.mVcount = leftMesh.mVcount;
      desc.mVerticesD = leftMesh.mVertices;
      desc.mTcount = leftMesh.mTcount;
      desc.mIndices = leftMesh.mIndices;
      desc.mIds      = leftMesh.mIds;
      rt->removeTjunctions(desc);
      leftMesh.setIndices(desc.mTcountOut,desc.mIndicesOut,desc.mIds);
    }
  }

  RobustMesh *rm = s->getRightMesh();

  if ( rm )
  {
    rm->getSimpleMeshDouble(rightMesh);
    if ( rightMesh.mTcount && remove_tjunctions )
    {
      RemoveTjunctionsDesc desc;
      desc.mVcount = rightMesh.mVcount;
      desc.mVerticesD = rightMesh.mVertices;
      desc.mTcount = rightMesh.mTcount;
      desc.mIndices = rightMesh.mIndices;
      desc.mIds      = rightMesh.mIds;
      rt->removeTjunctions(desc);
      rightMesh.setIndices(desc.mTcountOut,desc.mIndicesOut,desc.mIds);
    }
  }

  releaseRemoveTjunctions(rt);

  size_t endVcount = vindex->getVcount();
  size_t *remap = new size_t[endVcount];
  memset(remap,0xFF,sizeof(size_t)*endVcount);

  for (size_t i=0; i<ivcount; i++)
  {
    size_t r = remap_indices[i]; // the original index mapped to this vertex...
    HE_ASSERT( r < endVcount );
    if ( r != 0xFFFFFF ) // make sure this vertice was referenced by the input mesh..
    {
      remap[r] = i;
    }
  }

  leftMesh.setInitialVcount(inputVcount);
  rightMesh.setInitialVcount(inputVcount);

  leftMesh.setRemapIndex(remap);
  rightMesh.setRemapIndex(remap);

  if ( tesselate )
  {
    for (size_t i=inputVcount; i<leftMesh.mVcount; i++)
    {
      double *p = &leftMesh.mVertices[i*3];
      float v = ranf()*0.1f;
      float dir[3];
      dir[0] = (float)plane[0]*v;
      dir[1] = (float)plane[1]*v;
      dir[2] = (float)plane[2]*v;
      p[0]+=(double)dir[0];
      p[1]+=(double)dir[1];
      p[2]+=(double)dir[2];
    }
    for (size_t i=inputVcount; i<rightMesh.mVcount; i++)
    {
      double *p = &rightMesh.mVertices[i*3];
      float v = ranf()*0.1f;
      float dir[3];
      dir[0] = (float)plane[0]*v;
      dir[1] = (float)plane[1]*v;
      dir[2] = (float)plane[2]*v;
      p[0]+=(double)dir[0];
      p[1]+=(double)dir[1];
      p[2]+=(double)dir[2];
    }

  }
  delete []remap;

  releaseSplitMesh(s);

  delete []remap_indices;


  return ret;
}


}; // end of namespace
