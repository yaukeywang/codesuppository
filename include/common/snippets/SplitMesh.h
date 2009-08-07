#ifndef SPLIT_MESH_H

#define SPLIT_MESH_H

#include <stdlib.h>
#include "NxSimpleTypes.h"

#if 0
#include "log.h"
#include "RenderDebug/RenderDebug.h"
#endif


#define SPLIT_EPSILON 0.00000001f
#define CLOSURE_ID 0xFFFFFFFF

class fm_VertexIndex;

namespace SPLIT_MESH
{



class SimpleMesh
{
public:
  SimpleMesh(void)
  {
    mVcount = 0;
    mTcount = 0;
    mVertices = 0;
    mIndices  = 0;
    mRemapIndex = 0;
    mInitialVcount = 0;
    mIds = 0;
  }

  SimpleMesh(size_t vcount,size_t tcount,const float *vertices,const size_t *indices,const size_t *ids)
  {
    mInitialVcount = 0;
    mVcount = 0;
    mTcount = 0;
    mVertices = 0;
    mIndices  = 0;
    mRemapIndex = 0;
    mIds = 0;
    setSimpleMesh(vcount,tcount,vertices,indices,ids);
  }

  ~SimpleMesh(void)
  {
    release();
  }

  void release(void)
  {
    delete []mVertices;
    delete []mIndices;
    delete []mRemapIndex;
    delete []mIds;
    mIds = 0;
    mVertices = 0;
    mIndices = 0;
    mRemapIndex = 0;
    mVcount = 0;
    mTcount = 0;
  }

  void setSimpleMesh(size_t vcount,size_t tcount,const float *vertices,const size_t *indices,const size_t *ids)
  {
    release();
    mVertices = new float[vcount*3];
    mIndices  = new size_t[tcount*3];
    mVcount = vcount;
    mTcount = tcount;
    memcpy(mVertices,vertices,sizeof(float)*3*vcount);
    memcpy(mIndices,indices,sizeof(size_t)*3*tcount);
    if ( ids )
    {
      mIds = new size_t[tcount];
      memcpy(mIds,ids,sizeof(size_t)*tcount);
    }
  }

  void setSimpleMesh(size_t vcount,size_t tcount,const double *vertices,const size_t *indices,const size_t *ids)
  {
    release();
    mVertices = new float[vcount*3];
    mIndices  = new size_t[tcount*3];
    mVcount = vcount;
    mTcount = tcount;

    float *dest = mVertices;

    for (size_t i=0; i<vcount; i++)
    {
      dest[0] = (float)vertices[0];
      dest[1] = (float)vertices[1];
      dest[2] = (float)vertices[2];
      dest+=3;
      vertices+=3;
    }

    memcpy(mIndices,indices,sizeof(size_t)*3*tcount);
    if ( ids )
    {
      mIds = new size_t[tcount];
      memcpy(mIds,ids,sizeof(size_t)*tcount);
    }
  }

  void setRemapIndex(size_t *indices)
  {
    delete []mRemapIndex;
    mRemapIndex = new size_t[mVcount];
    memcpy(mRemapIndex,indices,sizeof(size_t)*mVcount);
  }

  void setIndices(size_t tcount,const size_t *indices,const size_t *ids)
  {
    delete []mIndices;
    mIndices = 0;
    delete []mIds;
    mIds = 0;
    mTcount = tcount;
    if ( mTcount )
    {
      mIndices = new size_t[mTcount*3];
      memcpy(mIndices,indices,sizeof(size_t)*mTcount*3);
      if ( ids )
      {
        mIds = new size_t[tcount];
        memcpy(mIds,ids,sizeof(size_t)*tcount);
      }
    }
  }

  size_t getInitialVcount(void) const { return mInitialVcount; };
  void setInitialVcount(size_t v) { mInitialVcount = v; };

  size_t     mVcount;
  size_t     mTcount;
  float     *mVertices;
  size_t    *mIndices;
  size_t    *mRemapIndex; // remaps an index to the original index passed in.  A remap value of 0xFFFFFFFF means it is a new vertex.
  size_t     mInitialVcount;
  size_t    *mIds;

};

bool splitMesh(const SimpleMesh &inputMesh,SimpleMesh &leftMesh,SimpleMesh &rightMesh,const float *planeEquation,bool tesselate,bool remove_tjunctions,bool build_closure,bool perlin_noise);

class SimpleMeshDouble
{
public:
  SimpleMeshDouble(void)
  {
    mVcount = 0;
    mTcount = 0;
    mVertices = 0;
    mIndices  = 0;
    mIds = 0;
    mRemapIndex = 0;
    mInitialVcount = 0;
  }

  SimpleMeshDouble(size_t vcount,size_t tcount,const double *vertices,const size_t *indices,const size_t *ids)
  {
    mRemapIndex = 0;
    mVcount = 0;
    mTcount = 0;
    mVertices = 0;
    mIndices  = 0;
    mInitialVcount = 0;
    mIds = 0;
    setSimpleMesh(vcount,tcount,vertices,indices,ids);
  }

  ~SimpleMeshDouble(void)
  {
    release();
  }

  void release(void)
  {
    delete []mVertices;
    delete []mIndices;
    delete []mRemapIndex;
    delete []mIds;
    mIds = 0;
    mVertices = 0;
    mIndices = 0;
    mVcount = 0;
    mTcount = 0;
    mRemapIndex = 0;
  }

  void setSimpleMesh(size_t vcount,size_t tcount,const double *vertices,const size_t *indices,const size_t *ids)
  {
    release();
    mVertices = new double[vcount*3];
    mIndices  = new size_t[tcount*3];
    mVcount = vcount;
    mTcount = tcount;
    memcpy(mVertices,vertices,sizeof(double)*3*vcount);
    memcpy(mIndices,indices,sizeof(size_t)*3*tcount);
    if ( ids )
    {
      mIds = new size_t[tcount];
      memcpy(mIds,ids,sizeof(size_t)*tcount);
    }
  }

  void setRemapIndex(size_t *indices)
  {
    delete []mRemapIndex;
    mRemapIndex = new size_t[mVcount];
    memcpy(mRemapIndex,indices,sizeof(size_t)*mVcount);
  }

  void setIndices(size_t tcount,const size_t *indices,const size_t *ids)
  {
    delete []mIndices;
    mIndices = 0;
    delete []mIds;
    mIds = 0;
    mTcount = tcount;
    if ( mTcount )
    {
      mIndices = new size_t[mTcount*3];
      memcpy(mIndices,indices,sizeof(size_t)*mTcount*3);
      if ( ids )
      {
        mIds = new size_t[mTcount];
        memcpy(mIds,ids,sizeof(size_t)*mTcount);
      }
    }
  }

  size_t getInitialVcount(void) const { return mInitialVcount; };
  void setInitialVcount(size_t v) { mInitialVcount = v; };


  size_t         mVcount;
  size_t         mTcount;
  double        *mVertices;
  size_t        *mIndices;
  size_t        *mIds;
  size_t        *mRemapIndex; // remaps an index to the original index passed in.  A remap value of 0xFFFFFFFF means it is a new vertex.
  size_t         mInitialVcount;
};

bool splitMesh(const SimpleMeshDouble &inputMesh,SimpleMeshDouble &leftMesh,SimpleMeshDouble &rightMesh,const double *planeEquation,bool tesselate,bool remove_tjunctions,bool build_closure,bool perlin_noise);


//************************************************************************************
//************************************************************************************
//** Prototypes of all of the internal code...used for debugging purposes
//************************************************************************************
//************************************************************************************
class RobustMeshInterface
{
public:
  virtual void iterateTriangle(const double *p1,const double *p2,const double *p3,size_t id) = 0;
};

class RobustMesh
{
public:

  virtual void addTriangle(const float *p1,const float *p2,const float *p3,size_t id) = 0;
  virtual void addTriangle(const double *p1,const double *p2,const double *p3,size_t id) = 0;

  virtual void iterateTriangles(RobustMeshInterface *iface) = 0;

  virtual bool getSimpleMesh(SimpleMesh &simpleMesh) = 0;
  virtual bool getSimpleMeshDouble(SimpleMeshDouble &simpleMesh) = 0;
  virtual bool saveObj(const char *fname) = 0;

  virtual fm_VertexIndex * getVertexIndex(void) = 0;

  virtual void addTriangle(size_t i1,size_t i2,size_t i3,size_t id) = 0;
  virtual void addTriangles(size_t tcount,const size_t *indices,size_t id) = 0;
};

RobustMesh * createRobustMesh(fm_VertexIndex *vindex);
void         releaseRobustMesh(RobustMesh *mesh);


class RingSystem
{
public:

  virtual void addLineSegment(const double *p1,const double *p2) = 0;

  virtual size_t buildPolygons(bool collapseColinear,bool intersectEddges,bool saveResults) = 0;

  virtual double *getEdges(size_t &edgeCount) = 0;

  virtual size_t  getPolygonCount(void) = 0; // report the number of rings.
  virtual double *getPolygon(size_t polygonIndex,size_t &polygonCount,bool &closed,bool &concave,double *center) = 0;

  virtual const double * getTriangulation(size_t &tcount) = 0; // returns the delauney triangulation

  virtual void setPlaneEquation(const double *plane) = 0;

  virtual void edgeStitch(void) = 0;

};


RingSystem *createRingSystem(fm_VertexIndex *vindex);
void        releaseRingSystem(RingSystem *rs);


class SplitMesh
{
public:

  virtual bool splitMesh(RobustMesh *rm,const double *plane,bool collapseColinear,bool edgeIntersect,bool saveResults) = 0;
  virtual RobustMesh * getLeftMesh(void) = 0;
  virtual RobustMesh * getRightMesh(void) = 0;
  virtual RingSystem * getLeftRingSystem(void) = 0;
  virtual void buildClosedMesh(bool tesselate) = 0; // closes up the mesh...


};

SplitMesh * createSplitMesh(fm_VertexIndex *vindex);
void        releaseSplitMesh(SplitMesh *sm);


};

#include <stdio.h>
#include <vector>
#include "UserMemAlloc.h"
#include "FloatMath.h"


typedef USER_STL::vector< size_t > size_tVector;

class EdgeWalk
{
public:

  EdgeWalk(size_t i1,size_t i2)
  {
    mI1        = i1;
    mI2        = i2;
    mNextLeft  = 0;
    mNextRight = 0;
    mNext      = 0;
    mPrevious  = 0;
    mTestFrame = 0;
    mBiDirectional = false;
    mFlipped   = false;
  }

  void setI1(size_t i)
  {
    mI1 = i;
  }

  void setI2(size_t i)
  {
    mI2 = i;
  }

  size_t getI1(void) const
  {
    return mFlipped ? mI2 : mI1;
  }

  size_t getI2(void) const
  {
    return mFlipped ? mI1 : mI2;
  }

  bool       mBiDirectional;
  bool       mFlipped;
  size_t     mTestFrame;
  EdgeWalk  *mNextLeft;
  EdgeWalk  *mNextRight;
  EdgeWalk  *mNext;
  EdgeWalk  *mPrevious;

private:
  size_t     mI1;
  size_t     mI2;

};

typedef USER_STL::vector< EdgeWalk >   EdgeWalkVector;
typedef USER_STL::vector< EdgeWalk * > EdgeWalkPtrVector;

class EdgeWalker
{
public:

  EdgeWalker(size_t vcount,fm_VertexIndex *vindex,bool save)
  {
    mSave = save;
    mVertexIndex = vindex;
    mVcount = vcount;
    mRoot   = 0;
    mJunkPile = 0;
    mEdgeCheck  = new unsigned int[vcount];
    mLeftEdges  = new EdgeWalk*[vcount];
    mRightEdges = new EdgeWalk*[vcount];
    memset(mLeftEdges,0,sizeof(EdgeWalk *)*vcount);
    memset(mRightEdges,0,sizeof(EdgeWalk *)*vcount);
    memset(mEdgeCheck,0,sizeof(unsigned int)*vcount);
  }

  ~EdgeWalker(void)
  {
    delete []mLeftEdges;
    delete []mRightEdges;
    delete []mEdgeCheck;
  }

  void addEdge(size_t i1,size_t i2)
  {

    EdgeWalk e(i1,i2);
    mRawEdges.push_back(e);

    if ( i1 != i2 )
    {
#if 0
      if ( mVertexIndex )
      {
        const double *vertices = mVertexIndex->getVerticesDouble();
        const double *p1 = &vertices[i1*3];
        const double *p2 = &vertices[i2*3];
        float t1[3];
        float t2[3];

        t1[0] = (float)p1[0];
        t1[1] = (float)p1[1];
        t1[2] = (float)p1[2];

        t2[0] = (float)p2[0];
        t2[1] = (float)p2[1];
        t2[2] = (float)p2[2];

        gRenderDebug->DebugRay(t1,t2,0.005f,0xFFFFFF,0xFF0000,1500.0f);
      }
#endif

      bool duplicate = false;

      EdgeWalkVector::iterator i;

      for (i=mEdges.begin(); i!=mEdges.end(); ++i)
      {
        EdgeWalk &e = (*i);

        if ( e.getI1() == i1 && e.getI2() == i2 )
        {
          duplicate = true;
        }
        else if ( e.getI1() == i2 && e.getI2() == i1 )
        {
          e.mBiDirectional = true;
          duplicate = true;
        }
      }
      if ( !duplicate )
      {
        EdgeWalk e(i1,i2);
//        e.mBiDirectional = true;
        mEdges.push_back(e);
      }
    }
  }

  void save(void)
  {
#pragma warning(push)
#pragma warning(disable:4996)
    FILE *fph = fopen("EdgeWalk.txt", "wb");

    if ( fph )
    {
      fprintf(fph,"   const size_t vcount = %d;\r\n", mVcount );
      fprintf(fph,"   float vertices[3*vcount] = {\r\n");

      const double *vertices = mVertexIndex->getVerticesDouble();
      for (size_t i=0; i<mVcount; i++)
      {
        const double *p = &vertices[i*3];
        fprintf(fph,"   %0.9ff,%0.9ff,%0.9ff,\r\n", (float) p[0], (float)p[1],(float)p[2] );
      }
      fprintf(fph,"  };\r\n");

      fprintf(fph,"  const size_t edgeCount = %d;\r\n", mRawEdges.size() );
      fprintf(fph,"  size_t edges[edgeCount*2] = {\r\n");
      {
        EdgeWalkVector::iterator i;
        for (i=mRawEdges.begin(); i!=mRawEdges.end(); ++i)
        {
          EdgeWalk &e = (*i);
          fprintf(fph,"  %d,%d,\r\n", e.getI1(), e.getI2() );
        }
      }
      fprintf(fph," };\r\n");
      fclose(fph);
    }
#pragma warning(pop)
  }

  EdgeWalk * snapToLeft(size_t to,EdgeWalk *exclude)
  {
    EdgeWalk *ret = 0;

    const double SNAP_DIST=0.01;
    double nearest = SNAP_DIST*SNAP_DIST;

    if ( mVertexIndex )
    {
      const double *p1 = mVertexIndex->getVertexDouble(to);
      EdgeWalk *e = mRoot;

      while ( e )
      {
        size_t from = e->getI1();
        const double *p2 = mVertexIndex->getVertexDouble(from);
        double dist = fm_distanceSquared(p1,p2);
        if ( dist < nearest && e != exclude )
        {
          nearest = dist;
          ret = e;
        }
        e = e->mNext;
      }
    }

    if ( ret )
    {
      removeLeftEdge(ret,ret->getI1());
      ret->setI1(to);
      insertLeftEdge(ret,to);
    }

    return ret;
  }




  void debug(EdgeWalk * /*walk*/,double /*yoffset*/)
  {
#if 0
    double xoffset = 0;
    double zoffset = 0;
//    xoffset = yoffset;
//    yoffset = 0;

    if ( mVertexIndex )
    {
      const double *_p1 = mVertexIndex->getVertexDouble(walk->getI1());
      const double *_p2 = mVertexIndex->getVertexDouble(walk->getI2());
      double p1[3];
      double p2[3];
      p1[0] = _p1[0]+xoffset;
      p1[1] = _p1[1]+yoffset;
      p1[2] = _p1[2]+zoffset;
      p2[0] = _p2[0]+xoffset;
      p2[1] = _p2[1]+yoffset;
      p2[2] = _p2[2]+zoffset;
      gRenderDebug->DebugRay(p1,p2,0.003f,0xFFFFFF,0xFF0000,30.0f);
      unsigned int color = 0x00FF00;
      if ( isDuplicateLeft(walk->getI1()) )
      {
        color = 0xFFFF00;
      }
      gRenderDebug->DebugSphere(p1,0.001f,color,30.0f);
    }

#if 0
    if ( yoffset == 0 )
    {
      const double *p1 = mVertexIndex->getVertexDouble(walk->getI1());
      const double *p2 = mVertexIndex->getVertexDouble(walk->getI2());

      gLog->Display("F: %d to %d (%0.9f,%0.9f,%0.9f) to (%0.9f,%0.9f,%0.9f)\r\n", walk->getI1(), walk->getI2(),
        (float)p1[0],(float)p1[1],(float)p1[2],(float)p2[0],(float)p2[1],(float)p2[2]);
    }
#endif

#endif
  }

  void processEdges(void)
  {
#if 0
    static bool first = true;
    if ( first )
    {
      first = false;
      save();
    }
    return;
#endif

    size_t count   = mEdges.size();
    if ( count > 0 )
    {
      EdgeWalk *e    = &mEdges[0];
      EdgeWalk *prev = 0;

      mRoot = e;

      memset(mLeftEdges,0,sizeof(EdgeWalk *)*mVcount);
      memset(mRightEdges,0,sizeof(EdgeWalk *)*mVcount);

      for (size_t i=0; i<count; i++)
      {
        e->mPrevious = prev;
  #if 1
        if ( e->mBiDirectional )
        {
          size_t i1 = e->getI1();
          e->setI1(e->getI2());
          e->setI2(i1);
        }
  #endif
        if ( (i+1) < count )
        {
          e->mNext = e+1;
        }
        else
        {
          e->mNext = 0;
        }

        size_t left        = e->getI1();
        size_t right       = e->getI2();

        e->mNextLeft       = mLeftEdges[left];
        mLeftEdges[left]   = e;

        e->mNextRight      = mRightEdges[right];

        mRightEdges[right] = e;

        prev = e;
        e++;
      }

      unsigned int check_count = 0;

      float yoffset = 0;



      size_t kill_count = 0;

      EdgeWalk *edge_start = mRoot;

      while ( mRoot )
      {

        EdgeWalk *e = mRoot;
        while ( e )
        {
          debug(e,yoffset);
          e = e->mNext;
        }

        check_count++;

        EdgeWalk *walk = edge_start;

        kill_count = 0;

        mEdgeCheck[ walk->getI1() ] = check_count;
        mEdgeCheck[ walk->getI2() ] = check_count;

        EdgeWalkPtrVector ring;

        EdgeWalk *last_duplicate_left = 0;

        ring.push_back(walk);

        debug(walk,yoffset-0.01f);

  //      gLog->Display("From: %d to %d\r\n", walk->getI1(), walk->getI2() );

        float yoff = -0.01f;

        while ( walk )
        {

          if ( isDuplicateLeft(walk->getI1()) )
          {
            last_duplicate_left = walk;
          }

          EdgeWalk *prev = walk;

          size_t to = walk->getI2();

          walk = mLeftEdges[ to ];

          if ( walk == 0 )
          {
            walk = snapToLeft(to,prev);
          }


  #if 0
          if ( walk && vertices)
          {
            const float *p1 = &vertices[ walk->getI1()*3];
            const float *p2 = &vertices[ walk->getI2()*3];

            gLog->Display("F: %d to %d (%0.9f,%0.9f,%0.9f) to (%0.9f,%0.9f,%0.9f)\r\n", walk->getI1(), walk->getI2(),
              p1[0],p1[1],p1[2],p2[0],p2[1],p2[2]);
          }
  #endif

          if ( walk == prev )
          {
            walk = walk->mNextLeft;
          }
#if 0
          if ( !walk )
          {
            walk = mRightEdges[to];
            if ( walk && walk == prev )
            {
              walk = walk->mNextRight;
            }
            if ( walk && walk->mBiDirectional )
            {
              walk->mFlipped = true;
            }
            else
            {
              walk = 0;
            }
          }
          else
          {
            walk->mFlipped = false;
          }
#endif

          if ( walk )
          {
            yoff+=0.001f;
            debug(walk,yoffset-yoff);
            ring.push_back(walk);

            if ( mEdgeCheck[ walk->getI2() ] == check_count )
            {
              // closed!
              if ( isDuplicateLeft(walk->getI1()) )
              {
                last_duplicate_left = walk;
              }
              break;
            }
            mEdgeCheck[walk->getI2()] = check_count;
          }

        }

        if ( walk ) // if it is closed
        {
          EdgeWalkPtrVector polygon;

          size_t poly_start  = 0xFFFFFFFF;
          size_t shared_left = 0xFFFFFFFF;
          size_t out_count   = 0;

          size_t count = ring.size();

          mPolygons.push_back( mIndices.size() );

          for (size_t i =0; i<count; i++)
          {
            EdgeWalk *e = ring[i];
            if ( poly_start != 0xFFFFFFFF || e->getI1() == walk->getI2() )
            {

              out_count++;

              //debug(e,vertices,yoffset+0.25f);

              if ( isDuplicateLeft(e->getI1()) )
              {
                shared_left = i;
              }
              if ( poly_start == 0xFFFFFFFF )
              {
                poly_start = i;
                mIndices.push_back( e->getI1() );
                mIndices.push_back( e->getI2() );
              }
              else
              {
                if ( (i+1) != count )
                {
                  mIndices.push_back( e->getI2() );
                }
              }
            }
          }

          mPolygons.push_back( out_count );

          if ( shared_left != 0xFFFFFFFF )
          {
  #if 1
            for (size_t i=poly_start; i<count; i++)
            {
              EdgeWalk *e = ring[i];
              removeEdge(e);
            }
  #else
            bool remove_ok = false;

            for (size_t i=0; i<out_count; i++)
            {
              EdgeWalk *e = ring[shared_left];

              if ( isDuplicateRight(e->getI2()) )
              {
                if ( remove_ok == false )
                {
                  //          unlinkEdge(e);
                }
                else
                {
                  removeEdge(e);
                  kill_count++;
                }
                remove_ok = true;
              }
              else if ( isDuplicateLeft(e->getI1()) )
              {
                //unlinkEdge(e);
                remove_ok = false;
              }
              else if ( remove_ok )
              {
                removeEdge(e);
                kill_count++;
              }
              shared_left++;
              if ( shared_left == count )
              {
                shared_left = poly_start;
              }
            }
  #endif
          }
          else
          {
            for (size_t i=poly_start; i<count; i++)
            {
              EdgeWalk *e = ring[i];
              removeEdge(e);
              kill_count++;
            }
          }
        }
        else
        {
          if ( last_duplicate_left )
          {
            bool remove_ok = false;
            EdgeWalkPtrVector::iterator i;
            for (i=ring.begin(); i!=ring.end(); ++i)
            {
              EdgeWalk *e = (*i);
              if ( e == last_duplicate_left )
              {
                remove_ok = true;
              }
              if ( remove_ok )
              {
                removeEdge(e);
                kill_count++;
                e->mNext = mJunkPile;
                e->mPrevious = 0;
                mJunkPile = e;
              }
            }
          }
          else
          {
            EdgeWalkPtrVector::iterator i;
            for (i=ring.begin(); i!=ring.end(); ++i)
            {
              EdgeWalk *e = (*i);
              removeEdge(e);
              kill_count++;
              e->mNext = mJunkPile;
              e->mPrevious = 0;
              mJunkPile = e;
            }
          }
        }

        if ( mRoot )
        {

          edge_start = mRoot;

          yoffset+=1.0f;
        }

        if ( check_count >= 100 )
        {
//          gLog->Display("Attempted edge walk more than 60 times, bailing.\r\n");
          break;
        }

//        break;

      }
    }
  }

  void process(void)
  {
    if ( mSave ) save();
    processEdges();
#if 0
    if ( mJunkPile  )
    {
      mEdges.clear();
      EdgeWalk *e = mJunkPile;
      while ( e )
      {
        debug(e,vertices,0);
        mEdges.push_back(e);
        e = e->mNext;
      }
      mJunkPile = 0;
      processEdgesBiDirectional(vertices);
    }
#endif
  }

  void removeLeftEdge(EdgeWalk *e,size_t index)
  {
    EdgeWalk *scan = mLeftEdges[index];
    EdgeWalk *prev = 0;

    while ( scan &&  scan != e )
    {
      prev = scan;
      scan = scan->mNextLeft;
    }

    if ( scan )
    {
      if ( prev )
      {
        prev->mNextLeft = e->mNextLeft;
      }
      else
      {
        mLeftEdges[index] = e->mNextLeft;
      }
    }
  }

  bool isDuplicateLeft(size_t i) const
  {
    bool ret = false;

    EdgeWalk *scan = mLeftEdges[i];
    if ( scan && scan->mNextLeft )
      ret = true;

    return ret;
  }

  bool isDuplicateRight(size_t i) const
  {
    bool ret = false;

    EdgeWalk *scan = mRightEdges[i];
    if ( scan && scan->mNextRight )
      ret = true;

    return ret;
  }

  void insertLeftEdge(EdgeWalk *e,size_t index)
  {
    EdgeWalk *scan = mLeftEdges[index];
    mLeftEdges[index] = e;
    e->mNextLeft = scan;
  }

  void removeRightEdge(EdgeWalk *e,size_t index)
  {
    EdgeWalk *scan = mRightEdges[index];
    EdgeWalk *prev = 0;

    while ( scan &&  scan != e )
    {
      prev = scan;
      scan = scan->mNextRight;
    }

    if ( scan )
    {
      if ( prev )
      {
        prev->mNextRight = e->mNextRight;
      }
      else
      {
        mRightEdges[index] = e->mNextRight;
      }
    }
  }

  void removeEdge(EdgeWalk *e)
  {
    e->mFlipped = false;
    // remove edge from doubly linked list
    if ( e->mPrevious ) // if we have a previous, then the 'next' of our previous is our 'next
    {
      e->mPrevious->mNext = e->mNext;
    }
    else // if we have no previous, then we were the root and should update the root pointer
    {
      mRoot = e->mNext;
    }

    if ( e->mNext ) // if we had a next, then his previous is now our previous
    {
      e->mNext->mPrevious = e->mPrevious;
    }

    // now patch the left and right edge lists..
    if ( mLeftEdges[e->getI1()] )
    {
      removeLeftEdge(e,e->getI1());
    }
    if ( mRightEdges[e->getI2()] )
    {
      removeRightEdge(e,e->getI2());
    }

    e->mNext = 0;
    e->mPrevious = 0;
    e->mNextLeft = 0;
    e->mNextRight = 0;

  }

  void unlinkEdge(EdgeWalk *e)
  {
    // now patch the left and right edge lists..
    assert( mLeftEdges[e->getI1()] );
    assert( mRightEdges[e->getI2()] );

    removeLeftEdge(e,e->getI1());
    removeRightEdge(e,e->getI2());

  }

  size_t getPolygonCount(void) const { return mPolygons.size()/2; };

  const size_t * getPolygon(size_t index,size_t &pcount) const
  {
    const size_t *ret = 0;
    size_t i = mPolygons[index*2];
    pcount   = mPolygons[index*2+1];
    if ( pcount )
    {
      ret = &mIndices[i];
    }
    return ret;
  }


private:
  size_t               mVcount;
  EdgeWalk            *mRoot;
  EdgeWalk           **mLeftEdges;
  EdgeWalk           **mRightEdges;

  EdgeWalkVector       mEdges;
  EdgeWalkVector       mRawEdges;

  unsigned int        *mEdgeCheck;
  EdgeWalk            *mJunkPile; // fragment edges that don't build polygons
  size_tVector         mIndices;
  size_tVector         mPolygons;
  fm_VertexIndex      *mVertexIndex;
  bool                 mSave;
};


#endif
