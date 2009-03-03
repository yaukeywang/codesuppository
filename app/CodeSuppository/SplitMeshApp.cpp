#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#pragma warning(disable:4702)

#include "SplitMeshApp.h"
#include "common/snippets/SplitMesh.h"
#include "common/snippets/wavefront.h"
#include "RenderDebug/RenderDebug.h"
#include "common/snippets/log.h"
#include "common/snippets/FloatMath.h"
#include "common/snippets/ArrowHead.h"
#include "common/snippets/ConsolidateMesh.h"
#include "common/snippets/ConvexDecomposition.h"
#include "common/snippets/MeshIslandGeneration.h"
#include "common/snippets/RemoveTjunctions.h"
#include "common/snippets/rand.h"
#include "common/snippets/perlin4.h"
#include "common/snippets/ImportHeightMap.h"
#include "common/snippets/erode.h"
#include "common/HeMath/HeFoundation.h"
#include "common/compression/compression.h"
#include "shared/MeshSystem/MeshSystemHelper.h"

static const HeF64 EPSILON=0.0001;

class SplitMeshApp : public SPLIT_MESH::RobustMeshInterface
{
public:
  SplitMeshApp(void)
  {
    mRemoveTjunctions = false;
    mTesselate = false;
    mSaveResults = false;
    mSplitMesh = 0;
    mRobustMesh = 0;
    mWireframe = true;
    mSolid     = true;
    mShowLeft = true;
    mShowRight = true;
    mShowRings = true;
    mShowClosure = true;
    mShowTriangles = true;
    mShowSplitPlane = true;
    mExplodeDistance = 0.2;
    mCollapseColinear = false;
    mEdgeIntersect = true;
    mDebugView = false;
    mPerlinNoise = false;

    mRotate[0] = 0;
    mRotate[1] = 0;
    mRotate[2] = 0;

    mPlane[0] = 0;
    mPlane[1] = 1;
    mPlane[2] = 0;
    mPlane[3] = 0;
    mProject[0] = 0;
    mProject[1] = 0;
    mProject[2] = 0;

    mImportHeightMap = 0;
    mErode = 0;

  }

  ~SplitMeshApp(void)
  {
    release();
  }

  void release(void)
  {
    if ( mRobustMesh ) SPLIT_MESH::releaseRobustMesh(mRobustMesh);
    if ( mSplitMesh )  SPLIT_MESH::releaseSplitMesh(mSplitMesh);
    mRobustMesh = 0;
    mSplitMesh = 0;
    if ( mImportHeightMap )
    {
      releaseImportHeightMap(mImportHeightMap);
      mImportHeightMap = 0;
    }
    if ( mErode )
    {
      ERODE::releaseErode(mErode);
      mErode = 0;
    }
  }

  void getDF(const double *p,float *d)
  {
    d[0] = (float)(p[0]+mProject[0]);
    d[1] = (float)(p[1]+mProject[1]);
    d[2] = (float)(p[2]+mProject[2]);
  }

  void getF(const float *p,float *d)
  {
    d[0] = (float)(p[0]+(float)mProject[0]);
    d[1] = (float)(p[1]+(float)mProject[1]);
    d[2] = (float)(p[2]+(float)mProject[2]);
  }

  bool onPlane(const double *p) const
  {
    bool ret = false;

    HeF64 d = fabs(fm_distToPlane(mPlane,p));

    if ( d < EPSILON )
    {
      ret = true;
    }
    return ret;
  }


  void iterateTriangle(const double *p1,const double *p2,const double *p3,size_t /*id*/)
  {
    float fp1[3];
    float fp2[3];
    float fp3[3];

    getDF(p1,fp1);
    getDF(p2,fp2);
    getDF(p3,fp3);

    if ( mSolid )
    {
      gRenderDebug->DebugSolidTri(fp1,fp2,fp3,0x00FFFF);
//      gRenderDebug->DebugSolidTri(fp3,fp2,fp1,0x0000FF);
    }

    if ( mWireframe )
    {
      if ( onPlane(p1) || onPlane(p2) || onPlane(p3) )
      {
        if ( onPlane(p1) && onPlane(p2) )
        {
          gRenderDebug->DebugRay(fp1,fp2,0.002f,0xFFFF00,0xFF0000);
        }
        else
        {
          gRenderDebug->DebugLine(fp1,fp2,0xFFFFD0);
        }

        if ( onPlane(p2) && onPlane(p3) )
        {
          gRenderDebug->DebugRay(fp2,fp3,0.002f,0xFFFF00,0xFF0000);
        }
        else
        {
          gRenderDebug->DebugLine(fp2,fp3,0xFFFFD0);
        }

        if ( onPlane(p3) && onPlane(p1) )
        {
          gRenderDebug->DebugRay(fp3,fp1,0.002f,0xFFFF00,0xFF0000);
        }
        else
        {
          gRenderDebug->DebugLine(fp3,fp1,0xFFFFD0);
        }

      }
      else
      {
        if ( mSolid )
          gRenderDebug->DebugTri(fp1,fp2,fp3,0xA0A0FF);
        else
          gRenderDebug->DebugTri(fp1,fp2,fp3,0xFFFFFF);
      }
    }

  }

  void setMeshSystemHelper(MeshSystemHelper *ms)
  {
    release();
    if ( ms )
    {
      MeshSystemRaw *mr = ms->getMeshSystemRaw();
      if ( mr && mr->mVcount )
      {
        mRobustMesh = SPLIT_MESH::createRobustMesh(0);
        for (unsigned int i=0; i<mr->mTcount; i++)
        {
          int i1 = mr->mIndices[i*3+0];
          int i2 = mr->mIndices[i*3+1];
          int i3 = mr->mIndices[i*3+2];
          const float *p1 = &mr->mVertices[i1*3];
          const float *p2 = &mr->mVertices[i2*3];
          const float *p3 = &mr->mVertices[i3*3];
          mRobustMesh->addTriangle(p1,p2,p3,0);
        }
        rebuild();
      }
      ms->releaseMeshSystemRaw(mr);
    }
  }

  void debugRingSegment(const HeF64 *from,const HeF64 *to,const HeF64 *project,HeU32 color)
  {
    HeF32 p1[3];
    HeF32 p2[3];

    p1[0] = (HeF32)(from[0]+project[0]);
    p1[1] = (HeF32)(from[1]+project[1]);
    p1[2] = (HeF32)(from[2]+project[2]);

    p2[0] = (HeF32)(to[0]+project[0]);
    p2[1] = (HeF32)(to[0]+project[1]);
    p2[2] = (HeF32)(to[0]+project[2]);


    gRenderDebug->DebugSphere(p1,0.002f,0xFFFF00);
    gRenderDebug->DebugRay(p1,p2,0.002f,color,0xFF0000);

  }

  void debugRing(SPLIT_MESH::RingSystem *ring,HeF64 pscale)
  {
    mProject[0] = (HeF32)(mPlane[0]*pscale);
    mProject[1] = (HeF32)(mPlane[1]*pscale);
    mProject[2] = (HeF32)(mPlane[2]*pscale);

    HeU32 edgeCount;
    const HeF64 *edges = ring->getEdges(edgeCount);

    for (HeU32 i=0; i<edgeCount; i++)
    {
      HeF32 p1[3];
      HeF32 p2[3];

      getDF(edges,p1);
      getDF(edges+3,p2);

      gRenderDebug->DebugSphere(p1,0.002f,0xFFFF00);
      gRenderDebug->DebugRay(p1,p2,0.003f,0xFFFF00,0xFF0000);

      edges+=6;

    }

		HeU32 pcount = ring->getPolygonCount();
		for (HeU32 i=0; i<pcount; i++)
		{
			double center[3];
			size_t count;
			bool concave,closed;
			const double *points = ring->getPolygon(i,count,closed,concave,center);
			assert(closed);
			const double *previous = &points[(count-1)*3];
			for (HeU32 i=0; i<count; i++)
			{
        HeF32 p1[3];
        HeF32 p2[3];

        getDF(previous,p1);
        getDF(points,p2);

	      gRenderDebug->DebugSphere(p1,0.002f,0x00FF00);
        gRenderDebug->DebugRay(p1,p2,0.003f,0x00FF00,0xFF0000);

				previous = points;
				points+=3;

			}
		}
  }

  void debugTriangles(SPLIT_MESH::RingSystem *ring,HeF64 pscale)
  {

    mProject[0] = (HeF32)(mPlane[0]*pscale);
    mProject[1] = (HeF32)(mPlane[1]*pscale);
    mProject[2] = (HeF32)(mPlane[2]*pscale);

    HeU32 tcount;
    const HeF64 *triangles = ring->getTriangulation(tcount);
    if ( triangles )
    {
      for (HeU32 i=0; i<tcount; i++)
      {
        HeF32 p1[3];
        HeF32 p2[3];
        HeF32 p3[3];

        getDF(triangles,p1);
        getDF(triangles+3,p2);
        getDF(triangles+6,p3);

        HeU32 color = 0xFFFFFF;

        if ( mWireframe )
        {
          gRenderDebug->DebugRay(p1,p2,0.003f,color,0xFF0000);
          gRenderDebug->DebugRay(p2,p3,0.003f,color,0xFF0000);
          gRenderDebug->DebugRay(p3,p1,0.003f,color,0xFF0000);
        }

        if ( mSolid )
        {
          gRenderDebug->DebugSolidTri(p1,p2,p3,0xFF00FF);
//          gRenderDebug->DebugSolidTri(p3,p2,p1,0xFF0000);
        }

        triangles+=9;
      }
    }

  }


  void debugPolys(SPLIT_MESH::RingSystem *ring,HeF64 pscale,bool flip)
  {
    mProject[0] = (HeF32)(mPlane[0]*pscale);
    mProject[1] = (HeF32)(mPlane[1]*pscale);
    mProject[2] = (HeF32)(mPlane[2]*pscale);

    HeU32 pcount = ring->getPolygonCount();
    for (HeU32 i=0; i<pcount; i++)
    {
      HeU32 pcount;
      bool closed;
      bool concave;
      HeF64 center[3];
      HeF64 *points = ring->getPolygon(i,pcount,closed,concave,center);
      if ( points )
      {
        HeU32 color = 0x00FF00;
        if ( !closed )
        {
          color = 0xFF0000;
        }
        HeF64 *previous = &points[(pcount-1)*3];
        HeF64 *scan     = points;
        for (HeU32 i=0; i<pcount; i++)
        {
          HeF32 p1[3];
          HeF32 p2[3];

          getDF(previous,p1);
          getDF(scan,p2);

          if ( flip )
          {
            gRenderDebug->DebugRay(p2,p1,0.003f,color,0xFF0000);
          }
          else
          {
            gRenderDebug->DebugRay(p1,p2,0.003f,color,0xFF0000);
          }

          previous = scan;
          scan+=3;

        }
        if ( closed )
        {
          fm_Triangulate *t = fm_createTriangulate();
          HeU32 tcount;
          const HeF64 *vertices = t->triangulate3d(pcount,points,sizeof(HeF64)*3,tcount);

          if ( vertices )
          {
            for (HeU32 i=0; i<tcount; i++,vertices+=9)
            {
              const HeF64 *dp1 = vertices;
              const HeF64 *dp2 = vertices+3;
              const HeF64 *dp3 = vertices+6;

              HeF32 p1[3];
              HeF32 p2[3];
              HeF32 p3[3];

              getDF(dp1,p1);
              getDF(dp2,p2);
              getDF(dp3,p3);

              if ( mSolid )
              {
                if ( flip )
                  gRenderDebug->DebugSolidTri(p3,p2,p1,0xFF00FF);
                else
                  gRenderDebug->DebugSolidTri(p1,p2,p3,0xFF00FF);
              }

              if ( mWireframe )
              {
                gRenderDebug->DebugTri(p1,p2,p3,0xFFFFFF);
              }
            }
          }
          fm_releaseTriangulate(t);
        }
      }
    }
  }

  void renderCone(const HeF32 *p1,const HeF32 *p2)
  {
    gRenderDebug->DebugLine(p1,p2);

    HeU32 tcount;
    const HeF32 *triangles = createArrowHead(p1,p2,1,tcount);
    for (HeU32 i=0; i<tcount; i++)
    {
      gRenderDebug->DebugSolidTri(triangles,triangles+3,triangles+6,0xFFFF00);
      gRenderDebug->DebugTri(triangles,triangles+3,triangles+6,0xFFFFFF);
      triangles+=9;
    }
  }

  void render(void)
  {


    if ( mShowSplitPlane )
    {
      HeF32 plane[4];
      plane[0] = (HeF32)mPlane[0];
      plane[1] = (HeF32)mPlane[1];
      plane[2] = (HeF32)mPlane[2];
      plane[3] = (HeF32)mPlane[3];
      gRenderDebug->DebugPlane(plane,4,4,0xFFFF00);
    }

    processTer();

    if ( mDebugView )
    {

      if ( mSplitMesh )
      {
        if ( mShowLeft )
        {
          SPLIT_MESH::RobustMesh *left  = mSplitMesh->getLeftMesh();
          if ( left )
          {
            mProject[0] = mPlane[0]*mExplodeDistance;
            mProject[1] = mPlane[1]*mExplodeDistance;
            mProject[2] = mPlane[2]*mExplodeDistance;

            left->iterateTriangles(this);
          }
          SPLIT_MESH::RingSystem *rl = mSplitMesh->getLeftRingSystem();
          if ( rl )
          {
            if ( mShowRings )
  						debugRing(rl,mExplodeDistance*0.4);
            if ( mShowTriangles ) debugTriangles(rl,mExplodeDistance);
            if ( mShowClosure )   debugPolys(rl,mExplodeDistance*0.8,false);
          }
        }

        if ( mShowRight )
        {
          SPLIT_MESH::RobustMesh *right = mSplitMesh->getRightMesh();
          if ( right )
          {
            mProject[0] = -mPlane[0]*mExplodeDistance;
            mProject[1] = -mPlane[1]*mExplodeDistance;
            mProject[2] = -mPlane[2]*mExplodeDistance;

            right->iterateTriangles(this);
          }
          SPLIT_MESH::RingSystem *rl = mSplitMesh->getLeftRingSystem();
          if ( rl )
          {
            if ( mShowClosure ) debugPolys(rl,-mExplodeDistance*0.8,true);
          }
        }

      }
      else if ( mRobustMesh )
      {
        mRobustMesh->iterateTriangles(this);
      }
    }
    else
    {

      mProject[0] = mPlane[0]*mExplodeDistance;
      mProject[1] = mPlane[1]*mExplodeDistance;
      mProject[2] = mPlane[2]*mExplodeDistance;

      render(mLeftMesh);

      mProject[0] = -mPlane[0]*mExplodeDistance;
      mProject[1] = -mPlane[1]*mExplodeDistance;
      mProject[2] = -mPlane[2]*mExplodeDistance;

      render(mRightMesh);


    }
  }

  void render(const SPLIT_MESH::SimpleMesh &s)
  {
    const size_t *ids = s.mIds;

    for (size_t i=0; i<s.mTcount; i++)
    {
      size_t i1 = s.mIndices[i*3+0];
      size_t i2 = s.mIndices[i*3+1];
      size_t i3 = s.mIndices[i*3+2];
      const float *p1 = &s.mVertices[i1*3];
      const float *p2 = &s.mVertices[i2*3];
      const float *p3 = &s.mVertices[i3*3];
      float _p1[3];
      float _p2[3];
      float _p3[3];
      getF(p1,_p1);
      getF(p2,_p2);
      getF(p3,_p3);

      unsigned int color = 0x00FFFF;

      if ( ids )
      {
        if ( ids[i] == CLOSURE_ID )
        {
          color = 0xFF00FF;
        }
      }
      else if ( i1 >= s.mInitialVcount &&
           i2 >= s.mInitialVcount &&
           i3 >= s.mInitialVcount )
      {
        color = 0xFF00FF;
      }

      gRenderDebug->DebugSolidTri(_p1,_p2,_p3,color);
      gRenderDebug->DebugTri(_p1,_p2,_p3,0xFFFFFF);
    }
  }

  void command(SplitMeshCommand _command,bool state,const float *data)
  {
    switch ( _command )
    {
      case SMC_FILL_BASIN:
        mDesc.mFillBasin = (int) data[0];
        break;
      case SMC_FILL_BASIN_PER:
        mDesc.mFillBasinPer = (int) data[0];
        break;
      case SMC_ERODE_ITERATIONS:
        mDesc.mErodeIterations = (int)data[0];
        break;
      case SMC_ERODE_RATE:
        mDesc.mErodeRate = data[0];
        break;
      case SMC_ERODE_POWER:
        mDesc.mErodePower = data[0];
        break;
      case SMC_SMOOTH_RATE:
        mDesc.mSmoothRate = data[0];
        break;
      case SMC_ERODE_THRESHOLD:
        mDesc.mErodeThreshold = data[0];
        break;
      case SMC_ERODE_SEDIMENTATION:
        mDesc.mErodeSedimentation = data[0];
        break;
      case SMC_REMOVE_TJUNCTIONS:
        if ( state != mRemoveTjunctions )
        {
          mRemoveTjunctions = state;
          rebuild();
        }
        break;
      case SMC_TESSELATE:
        if ( state != mTesselate )
        {
          mTesselate = state;
          rebuild();
        }
        break;
      case SMC_NOISE:
        if ( state != mPerlinNoise )
        {
          mPerlinNoise = state;
          rebuild();
        }
        break;
      case SMC_DEBUG_VIEW:
        if ( state != mDebugView )
        {
          mDebugView = state;
          rebuild();
        }
        break;
      case SMC_TEST_INTERSECTION:
        testIntersection();
        break;
      case SMC_SHOW_LEFT: mShowLeft = state; break;
      case SMC_SHOW_RIGHT: mShowRight = state; break;
      case SMC_SHOW_RINGS: mShowRings = state; break;
      case SMC_SHOW_CLOSURE: mShowClosure = state; break;
      case SMC_SHOW_TRIANGLES: mShowTriangles = state; break;
      case SMC_SHOW_SPLIT_PLANE: mShowSplitPlane = state; break;
      case SMC_EXPLODE:
        if ( data )
          mExplodeDistance = data[0];
        break;
      case SMC_WIREFRAME: mWireframe = state; break;
      case SMC_SOLID: mSolid = state; break;
      case SMC_PLANE_YUP:
        mPlane[0] = 0;
        mPlane[1] = 1;
        mPlane[2] = 0;
        rebuild();
        break;
      case SMC_PLANE_XUP:
        mPlane[0] = 1;
        mPlane[1] = 0;
        mPlane[2] = 0;
        rebuild();
        break;
      case SMC_PLANE_ZUP:
        mPlane[0] = 0;
        mPlane[1] = 0;
        mPlane[2] = 1;
        rebuild();
        break;
      case SMC_PLANE_D:
        if ( data )
        {
          mPlane[3] = data[0];
          rebuild();
        }
        break;
      case SMC_ROTATE_X:
        if ( data )
        {
          mRotate[0] = data[0];
          computePlane();
          rebuild();
        }
        break;
      case SMC_SAVE_RESULTS:
        mSaveResults = true;
        rebuild();
        mSaveResults = false;
        break;
      case SMC_ROTATE_Y:
        if ( data )
        {
          mRotate[1] = data[0];
          computePlane();
          rebuild();
        }
        break;
      case SMC_ROTATE_Z:
        if ( data )
        {
          mRotate[2] = data[0];
          computePlane();
          rebuild();
        }
        break;
      case SMC_COLLAPSE_COLINEAR:
        mCollapseColinear = state;
        rebuild();
        break;
      case SMC_EDGE_INTERSECT:
        mEdgeIntersect = state;
        rebuild();
        break;
      case SMC_SAVE_LEFT_EDGES:
        saveLeftEdges();
        break;
      case SMC_TEST_OBB:
        testOBB();
        break;
      case SMC_TEST_EDGE_WALK:
        testEdgeWalk();
        break;
      case SMC_SAVE_OBJ:
        saveObj();
        break;
      case SMC_CONVEX_DECOMPOSITION:
        convexDecomposition();
        break;
      case SMC_CONSOLIDATE_MESH:
//        consolidateMesh();
//        testTriangulate();
//        testMeshIsland();
//        testRemoveTjunctions();
//        testSphereIntersect();
//        testMeshFracture();
//        testPerlin4();
//        testAABB();
        testCompression();
        break;

    }
  }

  void testCompression(void)
  {
    const char *test = "this is a test string to test for compression testing stuff";
    size_t len = strlen(test);

    HeI32 outlen;
    void *mem = COMPRESSION::compressData(test,len+1,outlen,COMPRESSION::CT_GZIP,COMPRESSION::CL_BEST_COMPRESSION);

    HeI32 olen;
    const char *c = (const char *)COMPRESSION::decompressData(mem,outlen,olen);
    if ( c )
    {
      printf("%s\r\n", c );
    }

  }

  void testPerlin4(void)
  {
    Perlin4 *p = createPerlin4();

    for (float z=0; z<256; z++)
    {
      for (float x=0; x<256; x++)
      {
        float y = p->get(x*0.5f,z*0.5f);
        float pos[3] = { x, y, z };
        gRenderDebug->DebugPoint(pos,0.1f,0xFFFF00,60.0f);
      }
    }
    releasePerlin4(p);
  }

  void testMeshIsland(void)
  {
#if 0
    if ( mRobustMesh )
    {
      SPLIT_MESH::SimpleMesh s;
      mRobustMesh->getSimpleMesh(s);
      MeshIslandGeneration *cm = createMeshIslandGeneration();

      RemoveTjunctions *rt = createRemoveTjunctions();
      size_t tcount_out;
      size_t *indices = rt->removeTjunctions(s.mVcount, s.mVertices, s.mTcount, s.mIndices, tcount_out );

      size_t icount = cm->islandGenerate(tcount_out, indices, s.mVertices );
      if ( icount )
      {
        icount = cm->mergeCoplanarIslands(s.mVertices);
      }

      releaseRemoveTjunctions(rt);
      releaseMeshIslandGeneration(cm);
    }
#endif
  }

  void testMeshFracture(void)
  {
  }

  void testRemoveTjunctions(void)
  {
#if 0
    if ( mRobustMesh )
    {
      SPLIT_MESH::SimpleMesh s;
      mRobustMesh->getSimpleMesh(s);

      RemoveTjunctions *rt = createRemoveTjunctions();

      size_t tcount_out;
      rt->removeTjunctions(s.mVcount, s.mVertices, s.mTcount, s.mIndices, tcount_out);

       releaseRemoveTjunctions(rt);

    }
#endif
  }

  void consolidateMesh(void)
  {
    if ( mRobustMesh )
    {
      SPLIT_MESH::SimpleMesh s;
      mRobustMesh->getSimpleMesh(s);
      ConsolidateMesh *cm = createConsolidateMesh();
      for (size_t i=0; i<s.mTcount; i++)
      {
        size_t i1 = s.mIndices[i*3+0];
        size_t i2 = s.mIndices[i*3+1];
        size_t i3 = s.mIndices[i*3+2];
        const float *p1 = &s.mVertices[i1*3];
        const float *p2 = &s.mVertices[i2*3];
        const float *p3 = &s.mVertices[i3*3];
        cm->addTriangle(p1,p2,p3);
      }
      ConsolidateMeshOutput results;
      cm->consolidateMesh(results);
      releaseConsolidateMesh(cm);
    }
  }

  void convexDecomposition(void)
  {
    if ( mRobustMesh )
    {
      SPLIT_MESH::SimpleMeshDouble d;
      mRobustMesh->getSimpleMeshDouble(d);

      ConvexDecomposition::DecompDesc desc;

      desc.mVcount   = d.mVcount;
      desc.mVertices = d.mVertices;
      desc.mTcount   = d.mTcount;
      desc.mIndices  = d.mIndices;
      desc.mDepth    = 30;


      ConvexDecomposition::performConvexDecomposition(desc);

    }

  }

  void saveObj(void)
  {
    mSplitMesh->buildClosedMesh(false);
    SPLIT_MESH::RobustMesh *left = mSplitMesh->getLeftMesh();
    left->saveObj("left_mesh.obj");
    SPLIT_MESH::RobustMesh *right = mSplitMesh->getRightMesh();
    right->saveObj("right_mesh.obj");
    rebuild();
  }

  void saveLeftEdges(void)
  {
    if ( mSplitMesh )
    {
      SPLIT_MESH::RingSystem *ring = mSplitMesh->getLeftRingSystem();
      HeU32 edgeCount;
      const HeF64 *edges = ring->getEdges(edgeCount);

      gLog->Display("EdgeCount=%d  PlaneEquation: %0.9f,%0.9f,%9f,%0.9f\r\n", edgeCount, (float)mPlane[0], (float)mPlane[1], (float)mPlane[2],(float) mPlane[3]);

      for (HeU32 i=0; i<edgeCount; i++)
      {
        gLog->Display("Edge%d: %0.9f,%0.9f,%0.9f,            %0.9f, %0.9f,%0.9f,\r\n",
          i+1,
          (float)edges[0],
          (float)edges[1],
          (float)edges[2],
          (float)edges[3],
          (float)edges[4],
          (float)edges[5] );

        edges+=6;
      }
    }
  }

  void rebuild(void)
  {
    if ( mRobustMesh )
    {

      if ( mDebugView )
      {
        if ( mSplitMesh == 0 )
        {
          mSplitMesh = SPLIT_MESH::createSplitMesh(0);
        }
        mSplitMesh->splitMesh(mRobustMesh,mPlane,mCollapseColinear,mEdgeIntersect,mSaveResults);
      }
      else
      {
        SPLIT_MESH::SimpleMesh s;
        mRobustMesh->getSimpleMesh(s);

        mLeftMesh.release();
        mRightMesh.release();

        HeF32 plane[4];
        plane[0] = (float)mPlane[0];
        plane[1] = (float)mPlane[1];
        plane[2] = (float)mPlane[2];
        plane[3] = (float)mPlane[3];

        SPLIT_MESH::splitMesh(s,mLeftMesh,mRightMesh,plane,mTesselate,mRemoveTjunctions,true,mPerlinNoise);

      }
    }
  }

  HeF64 normalize(HeF64 *n) // normalize this vector
  {

    HeF64 dist = sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
    HeF64 mag = 0;

    if ( dist > 0.0000001 )
      mag = 1.0 / dist;

    n[0]*=mag;
    n[1]*=mag;
    n[2]*=mag;

    return dist;
  }


  void computePlane(void)
  {
    HeF64 a = mRotate[0]*FM_DEG_TO_RAD;
    HeF64 b = mRotate[1]*FM_DEG_TO_RAD;
    mPlane[0] = sin(a)*cos(b);
    mPlane[1] = sin(a)*sin(b);
    mPlane[2] = cos(a);
    normalize(mPlane);

#if 0
    mPlane[0] = 0;
    mPlane[1] = 1;
    mPlane[2] = 0;
    mPlane[3] = -0.519999981;
#endif
  }

  void testIntersection(void)
  {
#if 0
    double plane[4] = { 1.000000000,0.000000000, 0.000000,-0.244000003 };

//    double plane[4] = { 000000000,1.000000000, 0.000000,-0.244000003 };

//    #define EDGE_COUNT 2
    #define EDGE_COUNT 30

    double edges[EDGE_COUNT*6] =
    {
      -0.244000003,0.436521113,0.101231068,            -0.244000003, 0.436521113,0.256136000,
      -0.244000003,0.436521113,-0.224148005,            -0.244000003, 0.436521113,0.101231068,
      -0.244000003,0.386971653,0.256136000,            -0.244000003, 0.386971653,-0.126063496,
      -0.244000003,0.386971653,-0.126063496,            -0.244000003, 0.386971653,-0.224148005,
      -0.244000003,0.989664912,-0.322515339,            -0.244000003, 1.344658017,-0.351466000,
      -0.244000003,0.639972985,-0.293996990,            -0.244000003, 0.989664912,-0.322515339,
      -0.244000003,1.347721815,-0.323787302,            -0.244000003, 1.350831985,-0.295688987,
      -0.244000003,1.344658017,-0.351466000,            -0.244000003, 1.347721815,-0.323787302,
      -0.244000003,0.997724473,-0.267170668,            -0.244000003, 0.639263988,-0.238220006,
      -0.244000003,1.350831985,-0.295688987,            -0.244000003, 0.997724473,-0.267170668,
      -0.244000003,0.202310160,-0.285802990,            -0.244000003, 0.578548014,-0.285802990,
      -0.244000003,-0.000847000,-0.285802990,            -0.244000003, 0.202310160,-0.285802990,
      -0.244000003,0.375390857,-0.222660005,            -0.244000003, -0.000847000,-0.222660005,
      -0.244000003,0.578548014,-0.222660005,            -0.244000003, 0.375390857,-0.222660005,
      -0.244000003,-0.000847000,-0.244800255,            -0.244000003, -0.000847000,-0.285802990,
      -0.244000003,-0.000847000,-0.222660005,            -0.244000003, -0.000847000,-0.244800255,
      -0.244000003,0.202619269,0.256552011,            -0.244000003, 0.578548014,0.256552011,
      -0.244000003,-0.000847000,0.256552011,            -0.244000003, 0.202619269,0.256552011,
      -0.244000003,0.375081748,0.319694996,            -0.244000003, -0.000847000,0.319694996,
      -0.244000003,0.578548014,0.319694996,            -0.244000003, 0.375081748,0.319694996,
      -0.244000003,-0.000847000,0.297521055,            -0.244000003, -0.000847000,0.256552011,
      -0.244000003,-0.000847000,0.319694996,            -0.244000003, -0.000847000,0.297521055,
      -0.244000003,0.642284155,-0.323042005,            -0.244000003, 0.647705019,-0.323042005,
      -0.244000003,0.577012002,-0.323042005,            -0.244000003, 0.642284155,-0.323042005,
      -0.244000003,0.647705019,0.351466000,            -0.244000003, 0.582432866,0.351466000,
      -0.244000003,0.582432866,0.351466000,            -0.244000003, 0.577012002,0.351466000,
      -0.244000003,0.647705019,-0.323042005,            -0.244000003, 0.647705019,0.299743772,
      -0.244000003,0.647705019,0.299743772,            -0.244000003, 0.647705019,0.351466000,
      -0.244000003,0.577012002,0.351466000,            -0.244000003, 0.577012002,0.299743772,
      -0.244000003,0.577012002,0.299743772,            -0.244000003, 0.577012002,-0.323042005,
    };

    mProject[0] = 0;
    mProject[1] = 0;
    mProject[2] = 0;


    typedef USER_STL::vector< fm_LineSegment > LineSegmentVector;

    SPLIT_MESH::VertexPool *vpool = SPLIT_MESH::Vl_createVertexPool(DEFAULT_GRANULARITY,true);

    LineSegmentVector segments;

    double *scan = edges;

    for (int i=0; i<EDGE_COUNT; i++)
    {
      bool newPos;
      SPLIT_MESH::LineSegment seg;

      HeF64 p1[3];
      HeF64 p2[3];

      p1[0] = scan[0];
      p1[1] = scan[1];
      p1[2] = scan[2];

      p2[0] = scan[3];
      p2[1] = scan[4];
      p2[2] = scan[5];

      seg.mE1 = SPLIT_MESH::Vl_getIndex(vpool,p1,newPos);
      seg.mE2 = SPLIT_MESH::Vl_getIndex(vpool,p2,newPos);

      const HeF64 *spos1 = SPLIT_MESH::Vl_getVertex(vpool,seg.mE1);
      const HeF64 *spos2 = SPLIT_MESH::Vl_getVertex(vpool,seg.mE2);

      segments.push_back(seg);

      HeF32 tp1[3];
      HeF32 tp2[3];
      getDF(spos1,tp1);
      getDF(spos2,tp2);

      tp1[0]+=1;
      tp2[0]+=1;

      gRenderDebug->DebugRay(tp1,tp2,0.03f,0xFFFFFF,0xFF0000,15.0f);

//      gRenderDebug->DebugLine(tp1,tp2,0xFFFFFF,15.0f);
//      gRenderDebug->DebugPoint(tp1,0.01f,0x00FF00,15.0f);
//      gRenderDebug->DebugPoint(tp2,0.01f,0x00FF00,15.0f);

      scan+=6;
    }
#if 1
    SPLIT_MESH::LineSweep *sweep = SPLIT_MESH::createLineSweep();

    HeU32 scount;
    SPLIT_MESH::LineSegment *results = sweep->performLineSweep(&segments[0],segments.size(),plane,vpool,scount);

    if ( results )
    {
      for (HeU32 i=0; i<scount; i++)
      {
        const HeF64 *p1 = Vl_getVertex(vpool, results->mE1 );
        const HeF64 *p2 = Vl_getVertex(vpool, results->mE2 );
        HeF32 tp1[3];
        HeF32 tp2[3];
        getDF(p1,tp1);
        getDF(p2,tp2);
        float radius = 0.01f;
        HeU32 color = 0x00FF00;
        gRenderDebug->DebugRay(tp1,tp2,radius,0xFFFF00,color,15.0f);
        results++;
      }
    }

    SPLIT_MESH::releaseLineSweep(sweep);
#endif

#endif
  }

  void testOBB(void)
  {

    float plane[4];
    plane[0] = (float)mPlane[0];
    plane[1] = (float)mPlane[1];
    plane[2] = (float)mPlane[2];
    plane[3] = (float)mPlane[3];

    float matrix[16];

    fm_planeToMatrix(plane,matrix);

    #define PCOUNT 60

    float points[PCOUNT*3];
    float *pos = points;

    for (int i=0; i<PCOUNT; i++)
    {
      pos[0] = (float) ((rand()&63)-32)*0.15f;
      pos[1] = (float) ((rand()&63)-32)*0.0015f;
      pos[2] = (float) ((rand()%63)-32)*0.05f;

      float t[3];

      fm_transform(matrix,pos,t);

      gRenderDebug->DebugPoint(t,0.1f,0xFFFFFF,15.0f);

      pos[0] = t[0];
      pos[1] = t[1];
      pos[2] = t[2];
      pos+=3;
    }

    {
      float sides[3];
      fm_computeBestFitOBB(PCOUNT,points,sizeof(float)*3,sides,matrix);

      float bmin[3];
      float bmax[3];

      bmin[0] = -sides[0]*0.5f;
      bmin[1] = -sides[1]*0.5f;
      bmin[2] = -sides[2]*0.5f;

      bmax[0] = sides[0]*0.5f;
      bmax[1] = sides[1]*0.5f;
      bmax[2] = sides[2]*0.5f;

      fm_planeToMatrix(plane,matrix);

      gRenderDebug->DebugOrientedBound(bmin,bmax,matrix,0xFFFF00,15.0f);
    }

#if 0
    {
      float sides[3];

      float position[3];
      float quat[4];

      ConvexDecomposition::computeBestFitOBB(PCOUNT,points,sizeof(float)*3,sides,position,quat);

      float bmin[3];
      float bmax[3];

      bmin[0] = -sides[0]*0.5f;
      bmin[1] = -sides[1]*0.5f;
      bmin[2] = -sides[2]*0.5f;

      bmax[0] = sides[0]*0.5f;
      bmax[1] = sides[1]*0.5f;
      bmax[2] = sides[2]*0.5f;

      fm_planeToMatrix(plane,matrix);

      gRenderDebug->DebugOrientedBound(bmin,bmax,position,quat,0x00FF00,15.0f);
    }
#endif

  }

  void testEdgeWalk(void)
  {


   const size_t vcount = 1396;
   float vertices[3*vcount] = {
   2.872505903f,-0.444000006f,-0.532456040f,
   2.832604647f,-0.444000006f,-0.567921460f,
   2.736197948f,-0.346224993f,-0.659344971f,
   2.835437059f,-0.231101006f,-0.577889979f,
   2.986407995f,-0.413502991f,-0.428963989f,
   2.946069241f,-0.444000006f,-0.464577228f,
   2.976153135f,-0.444000006f,-0.429265916f,
   2.987097263f,-0.444000006f,-0.420033216f,
   2.996782541f,-0.444000006f,-0.413102359f,
   3.233974934f,2.059192896f,-0.072430998f,
   3.009100914f,1.971603990f,-0.034219999f,
   2.985371113f,1.982663035f,0.000000000f,
   3.247406006f,2.073333025f,0.000000000f,
   3.252223969f,2.026346922f,-0.148214996f,
   3.036360979f,1.913988948f,-0.169837996f,
   3.276333094f,1.863772988f,-0.294265985f,
   3.058366060f,1.773345947f,-0.297336996f,
   3.337544918f,1.748926997f,-0.340209991f,
   3.086666107f,1.632990956f,-0.338862985f,
   3.377233028f,1.587800026f,-0.400781006f,
   3.115382910f,1.492390037f,-0.380228013f,
   3.473409891f,1.366148949f,-0.454658002f,
   3.163181067f,1.209972024f,-0.462437987f,
   3.528445005f,1.060127020f,-0.489443004f,
   3.189663887f,0.907503009f,-0.500122011f,
   3.559393883f,0.706516027f,-0.452535003f,
   3.280680895f,0.510801971f,-0.499433994f,
   3.619649887f,0.472045988f,-0.391741991f,
   3.321825027f,0.266689986f,-0.465750009f,
   3.641195059f,0.407454014f,-0.362197995f,
   3.360039949f,0.165487006f,-0.412685007f,
   3.674710035f,0.338838995f,-0.307839990f,
   3.378798962f,0.073735997f,-0.355361998f,
   3.700362921f,0.265794009f,-0.240761995f,
   3.438950062f,-0.082580000f,-0.257014990f,
   3.737014055f,0.181837007f,-0.176818997f,
   3.514981031f,-0.121886000f,-0.187589005f,
   3.731688976f,0.186390996f,0.000000000f,
   3.486027956f,-0.141682997f,0.000000000f,
   3.716360092f,2.343386889f,0.000000000f,
   3.727092981f,2.308406115f,-0.150049001f,
   3.458300114f,2.151655912f,-0.102661997f,
   3.454971075f,2.169877052f,0.000000000f,
   4.120841980f,0.627140999f,-0.185461000f,
   4.126564980f,0.642027020f,0.000000000f,
   3.929250956f,0.411689013f,0.000000000f,
   3.955635071f,0.432417005f,-0.197586000f,
   3.889626026f,0.475277007f,-0.247364998f,
   4.105197906f,0.672581017f,-0.300000995f,
   3.861840963f,0.560638011f,-0.311960995f,
   4.068249226f,0.798925996f,-0.400813013f,
   3.849351883f,0.664451003f,-0.387198001f,
   4.041656971f,1.039755940f,-0.496921986f,
   3.823137045f,0.853053987f,-0.461519003f,
   3.988509893f,1.297806025f,-0.539537013f,
   3.747724056f,1.184932947f,-0.518661976f,
   3.886058092f,1.550140977f,-0.504830003f,
   3.650974035f,1.463739991f,-0.478617013f,
   3.881998062f,1.764428020f,-0.476336986f,
   3.590526104f,1.701089978f,-0.411994010f,
   3.814876080f,1.968415976f,-0.442999989f,
   3.541188002f,1.849182963f,-0.386366010f,
   3.758214951f,2.206682920f,-0.323051006f,
   3.479418039f,1.978250027f,-0.317478001f,
   3.745668888f,2.266000986f,-0.241929993f,
   3.460196018f,2.103493929f,-0.180368006f,
   3.300526857f,-0.444000006f,-0.163932592f,
   3.296721935f,-0.444000006f,-0.213598415f,
   3.342159033f,-0.384674013f,-0.201349005f,
   3.290065765f,-0.444000006f,0.000000000f,
   3.335210085f,-0.375755012f,0.000000000f,
   2.361088991f,1.766659975f,-0.202594995f,
   2.339596987f,1.807701945f,0.000000000f,
   2.723503113f,1.898838997f,0.000000000f,
   2.715712070f,1.828219056f,-0.167708993f,
   2.406969070f,1.645328045f,-0.341479003f,
   2.738132000f,1.687332988f,-0.295046002f,
   2.519026995f,1.485931039f,-0.422962010f,
   2.775680065f,1.538709998f,-0.352829009f,
   2.553870916f,1.284278035f,-0.512335002f,
   2.815366030f,1.377583027f,-0.413399011f,
   2.637460947f,1.017259955f,-0.581610978f,
   2.854290962f,1.093950987f,-0.494650006f,
   2.747936964f,0.712252975f,-0.615674019f,
   2.948386908f,0.783357024f,-0.515057981f,
   2.811503887f,0.469098002f,-0.657442987f,
   3.061089993f,0.397226006f,-0.574193001f,
   2.898732901f,0.271916002f,-0.651297987f,
   2.919869900f,0.084875003f,-0.642459989f,
   3.136321068f,0.171237007f,-0.539896011f,
   2.953028917f,-0.127690002f,-0.520498991f,
   3.166160107f,0.021310000f,-0.457260996f,
   3.199570894f,-0.153739005f,-0.380149990f,
   3.106352806f,-0.444000006f,-0.346269816f,
   3.178251743f,-0.444000006f,-0.299794525f,
   3.286426067f,-0.300743997f,-0.276331007f,
   3.220760822f,-0.444000006f,-0.272025704f,
   2.753155947f,0.400828987f,-0.713141024f,
   2.679445028f,0.687426984f,-0.720573008f,
   2.790680885f,0.219187006f,-0.718262970f,
   2.790657043f,0.044323999f,-0.717487991f,
   2.743994951f,-0.129851997f,-0.718276978f,
   2.514162064f,0.988713026f,-0.674526989f,
   2.482917070f,0.997636974f,-0.702425003f,
   2.396354914f,1.242238998f,-0.597721994f,
   2.628460884f,-0.444000006f,-0.869067132f,
   2.690576077f,-0.368057013f,-0.819342971f,
   2.660259724f,-0.444000006f,-0.813532174f,
   2.585894108f,-0.444000006f,-0.973843992f,
   2.594499111f,-0.422948003f,-0.981262982f,
   2.702577114f,-0.197405994f,-0.795611024f,
   2.607429981f,-0.208248004f,-0.974218011f,
   2.734183073f,0.038768999f,-0.865809977f,
   2.649413109f,0.031535000f,-1.024796009f,
   2.731512070f,0.219003007f,-0.891584992f,
   2.668936014f,0.206182003f,-1.014371037f,
   2.706959963f,0.393597007f,-0.849582016f,
   2.616000891f,0.447717011f,-0.960748017f,
   2.604217052f,0.728827000f,-0.769671023f,
   2.570400000f,0.738682985f,-0.830404997f,
   2.679378510f,-0.444000006f,-0.729782403f,
   2.708759785f,-0.444000006f,-0.667125463f,
   2.542438984f,0.999927998f,-0.648591995f,
   2.132390022f,1.755820990f,-0.180271000f,
   2.128139019f,1.788396955f,0.000000000f,
   2.183002949f,1.654070020f,-0.341767013f,
   2.301358938f,1.397721052f,-0.546015024f,
   2.096551895f,1.678482056f,-0.326810002f,
   2.069379091f,1.727818012f,-0.212745994f,
   2.189847946f,1.468991995f,-0.517500997f,
   2.266002893f,1.273252964f,-0.663331985f,
   2.347316980f,1.066113949f,-0.758763015f,
   2.399389029f,0.782091022f,-0.915278971f,
   2.448968887f,0.529857993f,-1.037335038f,
   2.495353937f,0.250521988f,-1.132081032f,
   2.500489950f,-0.035029002f,-1.170179009f,
   2.452588081f,-0.244072005f,-1.138797045f,
   2.439826965f,-0.395384997f,-1.131324053f,
   2.568352699f,-0.444000006f,-0.988961995f,
   2.426175356f,-0.444000006f,-1.109490395f,
   2.210929871f,-0.444000006f,-1.179537416f,
   2.081761360f,-0.444000006f,-1.240276933f,
   2.108911991f,-0.357127994f,-1.259691000f,
   2.042206049f,1.777153969f,0.000000000f,
   1.807582974f,1.756546021f,-0.177626997f,
   1.794464946f,1.787719011f,0.000000000f,
   2.136269093f,-0.187665999f,-1.278056979f,
   2.141789913f,-0.055305999f,-1.295565009f,
   2.081700087f,0.283248991f,-1.259461045f,
   2.023441076f,0.529338002f,-1.144399047f,
   1.975940943f,0.827956975f,-1.018422961f,
   1.958253980f,1.130270958f,-0.861813009f,
   1.859442949f,1.450392962f,-0.632221997f,
   1.821946025f,1.618209004f,-0.403750986f,
   1.798719049f,1.714447021f,-0.259351999f,
   0.737904012f,1.604212999f,-0.281713009f,
   0.223642007f,1.684074998f,-0.160742998f,
   0.234527007f,1.730116963f,0.000000000f,
   0.733817995f,1.740872979f,0.000000000f,
   0.725959003f,1.478055000f,-0.461804003f,
   0.237330005f,1.595746040f,-0.288672000f,
   0.760721982f,1.205036044f,-0.700545013f,
   0.289038986f,1.496446013f,-0.430054992f,
   0.857322991f,0.985001028f,-0.874028981f,
   0.305671006f,1.298416972f,-0.629719019f,
   0.921311975f,0.755658984f,-0.999329984f,
   0.243472993f,1.134081960f,-0.819993019f,
   0.939270973f,0.379985988f,-1.185871959f,
   0.316341013f,0.785310984f,-1.086259961f,
   0.927775025f,0.195225000f,-1.233042955f,
   0.539251983f,0.353983998f,-1.254492998f,
   0.950214982f,0.042750999f,-1.256188035f,
   0.539150000f,0.148261994f,-1.321014047f,
   0.943854988f,-0.101126999f,-1.274770975f,
   0.516871989f,-0.020713000f,-1.356626987f,
   0.960321009f,-0.308052003f,-1.243132949f,
   0.542385995f,-0.188226998f,-1.370375037f,
   0.870325685f,-0.444000006f,-1.195818901f,
   0.715793788f,-0.444000006f,-1.275204659f,
   0.546875000f,-0.390940011f,-1.363244057f,
   0.969213724f,-0.444000006f,-1.178807974f,
   0.633000731f,-0.444000006f,-1.307471752f,
   0.547704160f,-0.444000006f,-1.356244683f,
   0.000000000f,0.167709962f,-1.470412731f,
   0.005537000f,0.163518995f,-1.472265005f,
   0.000000000f,0.165525094f,-1.472229123f,
   0.000000000f,0.177044600f,-1.466482997f,
   0.000000000f,0.013082026f,-1.521888018f,
   0.018085999f,0.002865000f,-1.523198962f,
   0.000000000f,0.007672692f,-1.524864674f,
   0.000000000f,-0.204778835f,-1.545023918f,
   0.034086999f,-0.225059003f,-1.542948961f,
   0.000000000f,-0.220245898f,-1.551265836f,
   0.008318064f,-0.444000006f,-1.571121931f,
   0.000000000f,-0.444000006f,-1.573266983f,
   0.015450112f,-0.444000006f,-1.569282770f,
   1.462507010f,1.745481014f,-0.222394004f,
   1.456655979f,1.785879016f,0.000000000f,
   1.483832002f,1.697594047f,-0.288780987f,
   1.497138023f,1.618934989f,-0.401107997f,
   1.504951000f,1.483492970f,-0.585976005f,
   1.517125964f,1.334367037f,-0.741075993f,
   1.570685029f,0.938327014f,-0.989108026f,
   1.582203031f,0.771082997f,-1.068179965f,
   1.630079985f,0.420235991f,-1.172369003f,
   1.649026990f,0.224457994f,-1.211969018f,
   1.660354972f,0.019397000f,-1.246001959f,
   1.664159060f,-0.189658999f,-1.266728997f,
   1.678622007f,-0.410061002f,-1.291177034f,
   1.980608702f,-0.444000006f,-1.256164193f,
   1.673678398f,-0.444000006f,-1.285483241f,
   1.611559153f,-0.444000006f,-1.250568509f,
   1.471211553f,-0.444000006f,-1.246747494f,
   0.000000000f,1.708879232f,-0.081306018f,
   0.000000000f,1.685217500f,-0.156050697f,
   -0.000000000f,1.732217193f,0.000000000f,
   0.000000000f,1.642608881f,-0.217672020f,
   0.000000000f,1.610479951f,-0.278275162f,
   0.000000000f,1.564336181f,-0.345081568f,
   0.000000000f,1.533421040f,-0.408035219f,
   0.000000000f,1.445673108f,-0.496900886f,
   0.000000000f,1.407327890f,-0.576042175f,
   0.000000000f,1.328171015f,-0.661438346f,
   0.000000000f,1.229821324f,-0.753033578f,
   0.000000000f,1.108699441f,-0.846197724f,
   0.000000000f,0.881539166f,-1.035832524f,
   0.000000000f,0.725368083f,-1.092841268f,
   0.000000000f,0.440043718f,-1.330055952f,
   0.000000000f,0.403825581f,-1.341763854f,
   0.291339308f,-0.444000006f,-1.464742541f,
   4.897808075f,1.946794033f,-0.628057003f,
   4.925394058f,1.955229044f,-0.618836999f,
   4.953739166f,1.952579975f,-0.592256010f,
   4.971871853f,1.924078941f,-0.586026013f,
   4.953402996f,1.907938957f,-0.605609000f,
   4.925498962f,1.917814016f,-0.613412976f,
   4.973133087f,1.992779016f,-0.533465981f,
   5.001903057f,1.914397955f,-0.486723989f,
   4.982364178f,1.873299956f,-0.536189973f,
   4.926194191f,1.859812975f,-0.582136989f,
   4.852578163f,1.889721036f,-0.605031013f,
   4.824747086f,1.946835041f,-0.626357973f,
   4.861610889f,2.001638889f,-0.621913016f,
   4.935561180f,2.016016006f,-0.585699975f,
   4.760921001f,1.881356955f,-0.613120019f,
   4.861618042f,1.854066968f,-0.618094981f,
   4.842883110f,2.021332979f,-0.638024986f,
   4.935880184f,2.051665068f,-0.572853029f,
   4.991745949f,2.009819031f,-0.523205996f,
   4.944705963f,1.840662956f,-0.575374007f,
   4.991709232f,1.844295025f,-0.520761013f,
   4.908019066f,1.796041012f,-0.587881982f,
   4.852581978f,1.816115022f,-0.603105009f,
   4.963545799f,1.783161044f,-0.553515017f,
   4.983034134f,1.806342959f,-0.505779028f,
   5.011429787f,1.875568986f,-0.463205010f,
   4.918466091f,2.084724903f,-0.532712996f,
   4.964944839f,2.053575039f,-0.499902993f,
   5.020077229f,1.978170037f,-0.480978012f,
   4.825995922f,2.089175940f,-0.576083004f,
   4.881574154f,2.094571114f,-0.539949000f,
   4.788475990f,2.028047085f,-0.608618975f,
   4.751519203f,2.039658070f,-0.634133995f,
   4.798296928f,2.118191004f,-0.591085017f,
   4.872539043f,2.159259081f,-0.542891026f,
   4.927885056f,2.156559944f,-0.516578972f,
   5.002439976f,2.105746031f,-0.468232989f,
   5.057495117f,2.002751112f,-0.451974005f,
   5.075994015f,1.900998950f,-0.443792999f,
   5.030289173f,1.827419043f,-0.440802991f,
   4.991903782f,1.760828972f,-0.510385990f,
   4.936066151f,1.737668037f,-0.557276011f,
   4.871154785f,1.741237998f,-0.592325985f,
   4.816181183f,1.797860026f,-0.604026020f,
   5.095375061f,1.961238027f,-0.401515990f,
   5.085813046f,1.888502955f,-0.408327013f,
   5.058178902f,1.788185954f,-0.417387009f,
   5.019835949f,1.739907026f,-0.485561997f,
   4.917973042f,1.681480050f,-0.544201016f,
   4.862636089f,1.684144020f,-0.570155025f,
   4.752336979f,1.723387957f,-0.591296017f,
   4.697607040f,1.768061042f,-0.576663017f,
   5.030641079f,2.102196932f,-0.432328999f,
   5.067202091f,2.027704000f,-0.422291011f,
   4.937950134f,2.181154966f,-0.486939013f,
   4.881855011f,2.194544077f,-0.530287981f,
   4.807794094f,2.143652916f,-0.570393026f,
   4.728962898f,2.036145926f,-0.614306986f,
   4.742358208f,1.882231951f,-0.621649981f,
   4.744050026f,2.179337025f,-0.556056976f,
   4.688114166f,2.046377897f,-0.604517996f,
   4.837330818f,2.235677958f,-0.479332000f,
   4.893849850f,2.220523119f,-0.417712986f,
   4.995977879f,2.148785114f,-0.344534010f,
   5.060840130f,2.053334951f,-0.309323996f,
   5.097062111f,1.954205990f,-0.328794986f,
   5.105588913f,1.864086032f,-0.347113013f,
   4.697023869f,1.862575054f,-0.604048014f,
   4.739440918f,2.299169064f,-0.369987011f,
   4.832890034f,2.262727022f,-0.283787012f,
   4.971394062f,2.146686077f,-0.224781007f,
   5.072165012f,2.043629885f,-0.209227994f,
   5.127157211f,1.942759037f,-0.211214006f,
   5.152698040f,1.830661058f,-0.287041992f,
   5.133171082f,1.715563059f,-0.334257990f,
   5.103426933f,1.624403954f,-0.434275001f,
   4.982821941f,1.503070951f,-0.507606030f,
   4.927281857f,1.486953020f,-0.526327014f,
   4.798409939f,1.473148942f,-0.570617974f,
   4.715645790f,1.502195001f,-0.584339023f,
   4.615407944f,1.639641047f,-0.577768028f,
   4.560750961f,1.785153985f,-0.562427998f,
   4.551484108f,1.841748953f,-0.575191021f,
   4.533113003f,1.989009023f,-0.578975022f,
   4.636581898f,2.263382912f,-0.471996993f,
   4.561298847f,2.133065939f,-0.547248006f,
   4.912302971f,2.162645102f,-0.000090000f,
   5.049689770f,2.032731056f,0.000071000f,
   5.156644821f,1.942901969f,-0.119993001f,
   5.218821049f,1.803552032f,-0.199305996f,
   5.216855049f,1.636644959f,-0.279796988f,
   5.233176231f,1.442458987f,-0.362987995f,
   4.679115772f,1.438434005f,-0.589658022f,
   4.596477985f,1.512987018f,-0.598981977f,
   4.560023785f,1.604022980f,-0.589332998f,
   4.477780819f,1.687857985f,-0.597406983f,
   4.459681988f,1.778884053f,-0.588185012f,
   4.432610035f,1.878872991f,-0.577888012f,
   4.433020115f,2.024713993f,-0.563794017f,
   4.634995937f,2.369115114f,-0.140117005f,
   4.747988224f,2.300163031f,-0.000276000f,
   4.577572823f,2.389534950f,-0.256195992f,
   4.472349167f,2.325133085f,-0.443764001f,
   4.389483929f,2.190421104f,-0.473320007f,
   5.223190784f,1.371490955f,-0.388069004f,
   5.158321857f,1.236770034f,-0.418042988f,
   4.955359936f,1.236724019f,-0.505230010f,
   4.761600971f,1.363013029f,-0.571370006f,
   5.177545071f,1.932322979f,0.000214000f,
   5.321135998f,1.795562029f,-0.119606003f,
   5.378289223f,1.768447995f,0.000441000f,
   5.424281120f,1.721151948f,0.000496000f,
   5.421809196f,1.685348988f,-0.123554997f,
   5.383207798f,1.619660974f,-0.202452004f,
   5.373133183f,1.541141987f,-0.246711999f,
   5.334328175f,1.402712941f,-0.332639009f,
   5.296963215f,1.322443008f,-0.357984006f,
   5.260653019f,1.184172988f,-0.352061987f,
   5.250484943f,1.123028994f,-0.385233998f,
   4.991713047f,1.163095951f,-0.504149020f,
   4.733656883f,1.227370977f,-0.592882991f,
   4.641547203f,1.285421968f,-0.622034013f,
   4.522246838f,1.398277998f,-0.644891977f,
   4.430998802f,1.535753012f,-0.638888001f,
   4.404221058f,1.662433028f,-0.616609991f,
   4.386380196f,1.770867944f,-0.596665978f,
   4.377095222f,1.871713042f,-0.595743001f,
   5.588763237f,1.573845983f,0.000686000f,
   5.604887962f,1.546090961f,-0.113773003f,
   5.566339016f,1.498677015f,-0.190905005f,
   5.528185844f,1.440179944f,-0.250665992f,
   5.471928120f,1.345178962f,-0.313899010f,
   5.425783157f,1.264371991f,-0.329686999f,
   5.397966862f,1.174237967f,-0.346801996f,
   5.388454914f,1.119418979f,-0.351882994f,
   5.687035084f,1.499636054f,-0.110765003f,
   5.741781235f,1.463955998f,-0.124890998f,
   5.730426788f,1.435667992f,-0.209848002f,
   5.685194016f,1.452201962f,-0.188749000f,
   5.710154057f,1.360653043f,-0.290071994f,
   5.608973980f,1.317868948f,-0.319718987f,
   5.647185802f,1.394605994f,-0.257833004f,
   5.681725979f,1.282503009f,-0.333871007f,
   5.563252926f,1.235295057f,-0.317238003f,
   5.672186852f,1.164767027f,-0.354079992f,
   5.580797195f,1.174134970f,-0.351054013f,
   5.680006981f,1.510009050f,0.000786000f,
   5.479629993f,1.111310005f,-0.364187986f,
   5.842092037f,1.427708030f,-0.130723998f,
   5.915025234f,1.382158995f,-0.136820003f,
   5.886832237f,1.385674000f,-0.172364995f,
   5.813217163f,1.415580988f,-0.195261002f,
   5.908842087f,1.388967991f,0.001026000f,
   5.963117123f,1.307775021f,0.001098000f,
   5.979356766f,1.325531960f,-0.125337005f,
   5.941383839f,1.330818057f,-0.178936005f,
   5.743997097f,1.482738972f,0.000851000f,
   5.835497856f,1.436280012f,0.000946000f,
   5.783730984f,1.341470957f,-0.284590006f,
   5.847989082f,1.311568975f,-0.261476010f,
   5.931474209f,1.233482957f,-0.215612993f,
   5.998087883f,1.231873989f,-0.107334003f,
   5.963527203f,1.232406974f,0.001121000f,
   5.927110195f,1.131194949f,0.001118000f,
   5.961686134f,1.110978007f,-0.090324000f,
   5.908782005f,1.066915989f,0.001121000f,
   5.934596062f,1.054795980f,-0.077039003f,
   5.871946812f,1.021427989f,0.001102000f,
   5.897758007f,1.009309053f,-0.080582999f,
   5.878630161f,0.986485004f,-0.128291994f,
   5.923808098f,1.025282979f,-0.153083995f,
   5.950684071f,1.082725048f,-0.175648004f,
   5.800777912f,1.078595996f,-0.319449991f,
   5.884856224f,1.062201977f,-0.249544993f,
   5.857749939f,0.997026026f,-0.236768007f,
   5.755281925f,1.004148960f,-0.307505012f,
   5.763951778f,1.062108994f,-0.338640988f,
   5.912147045f,1.117910981f,-0.254197001f,
   5.959462166f,1.157227039f,-0.187096998f,
   5.757505894f,0.957899988f,-0.210731998f,
   5.674999237f,0.950753987f,-0.227960005f,
   5.837874889f,1.141492963f,-0.305184007f,
   5.773409843f,1.225860953f,-0.323024988f,
   5.764773846f,1.279824018f,-0.308562011f,
   5.895898819f,1.182729959f,-0.164704993f,
   5.849792004f,1.193480015f,-0.181047007f,
   5.803424835f,1.260787010f,-0.210002005f,
   5.840664864f,1.221941948f,-0.187128007f,
   5.672385216f,1.007694006f,-0.341778010f,
   5.573802948f,0.952578008f,-0.243885994f,
   5.580727100f,0.999329984f,-0.349866986f,
   5.535183907f,0.980180025f,-0.341257006f,
   5.690122128f,1.092916965f,-0.370849997f,
   5.681086063f,1.054963946f,-0.355859995f,
   5.598281860f,1.094735026f,-0.386994004f,
   5.470775127f,1.009574056f,-0.355370998f,
   5.580223083f,1.047469974f,-0.372696012f,
   5.425352097f,1.035966992f,-0.342720002f,
   5.434594154f,0.970412016f,-0.330823004f,
   5.480648994f,0.941386998f,-0.316248000f,
   5.379460812f,1.045817018f,-0.349747986f,
   5.370708942f,0.980271995f,-0.337431997f,
   5.373645782f,0.885375023f,-0.208839998f,
   5.638369083f,0.904402018f,-0.222547993f,
   5.546347141f,0.916043997f,-0.246782005f,
   5.748612881f,0.920490026f,-0.205099002f,
   5.511556149f,0.863524973f,-0.177613005f,
   5.603370190f,0.852747977f,-0.162334993f,
   5.704629898f,0.849157989f,-0.128130004f,
   5.241466045f,1.040469050f,-0.383964986f,
   5.223570824f,0.974029005f,-0.362116992f,
   5.299005032f,0.908990979f,-0.260176986f,
   5.197562218f,0.922406018f,-0.302473009f,
   5.851404190f,0.929008007f,-0.105361998f,
   5.836005211f,0.945721984f,0.001092000f,
   5.780789852f,0.891286016f,0.001059000f,
   5.797410965f,0.880032003f,-0.072306000f,
   5.679621220f,0.839500010f,0.000985000f,
   5.568745136f,0.834653020f,-0.071390003f,
   5.579253197f,0.837795973f,0.000896000f,
   5.449163914f,0.847535014f,-0.103909001f,
   5.247762203f,0.848549008f,-0.108546004f,
   5.213434219f,0.856783986f,0.000565000f,
   5.112563133f,0.840686023f,0.000480000f,
   5.137074947f,0.824518025f,-0.144841999f,
   5.356900215f,0.851119995f,-0.138324007f,
   5.264430046f,0.855570972f,-0.181694001f,
   5.323232174f,0.866038978f,0.000660000f,
   5.469641209f,0.838721991f,0.000798000f,
   5.153735161f,0.831574976f,-0.218347996f,
   4.920855045f,0.812339008f,0.000318000f,
   4.963525772f,0.797381997f,-0.143439993f,
   4.996709824f,0.810559988f,-0.280770987f,
   4.712067127f,0.770511985f,-0.333393008f,
   4.669690132f,0.738162994f,-0.188296005f,
   4.655453205f,0.758022010f,0.000098000f,
   5.004627228f,0.908636987f,-0.345272988f,
   4.728808880f,0.878731012f,-0.405800998f,
   5.011084080f,1.066769958f,-0.458561987f,
   5.021254063f,0.951308012f,-0.405566990f,
   4.743249893f,1.059209943f,-0.563421011f,
   4.744871140f,0.950945020f,-0.491082013f,
   4.567895889f,1.149804950f,-0.642484009f,
   4.496191025f,1.024566054f,-0.579491019f,
   4.416474819f,0.921263993f,-0.458838999f,
   4.333502769f,0.951175988f,-0.481516987f,
   4.399629116f,0.850458980f,-0.391855001f,
   4.297891140f,0.911505997f,-0.447986990f,
   4.380854130f,0.704573989f,-0.000130000f,
   4.359047890f,0.700414002f,-0.166650996f,
   4.319798946f,0.731006980f,-0.291211993f,
   4.216373920f,0.852328002f,-0.405873001f,
   4.449010849f,1.260928988f,-0.647431016f,
   4.366867065f,1.131135941f,-0.630581021f,
   4.141189098f,1.008774996f,-0.499341995f,
   4.277226925f,1.077746034f,-0.550818980f,
   4.085564137f,1.288884997f,-0.544405997f,
   4.175444126f,1.350332022f,-0.613987982f,
   4.247733116f,1.381057978f,-0.649595976f,
   4.329607010f,1.464800954f,-0.661485016f,
   4.312224865f,1.683392048f,-0.639944017f,
   4.303615093f,1.799914002f,-0.610387981f,
   4.304562092f,1.906537056f,-0.572242022f,
   4.240116119f,1.770048976f,-0.583945990f,
   4.221814156f,1.660768986f,-0.594089985f,
   4.121120930f,1.614451051f,-0.587188005f,
   4.067575932f,1.572667956f,-0.534996986f,
   4.148882866f,1.759917974f,-0.573764980f,
   4.076851845f,1.746600986f,-0.527436018f,
   4.342350960f,2.581176043f,-0.140316993f,
   4.317602158f,2.588926077f,-0.000745000f,
   4.454961777f,2.523303032f,-0.000603000f,
   4.452383041f,2.524558067f,-0.130254999f,
   4.332083225f,2.586481094f,-0.194916993f,
   4.440143108f,2.510529041f,-0.269163996f,
   4.339601040f,2.592642069f,-0.258890986f,
   4.429894924f,2.495758057f,-0.306894004f,
   4.187257767f,2.590220928f,-0.135844007f,
   4.189883232f,2.606915951f,-0.000863000f,
   4.203324795f,2.589185953f,-0.219163999f,
   4.211651802f,2.602538109f,-0.264010996f,
   4.209940910f,2.600611925f,-0.337597013f,
   4.328606129f,2.563996077f,-0.343892008f,
   4.410843849f,2.500166893f,-0.351972014f,
   4.224730015f,2.624525070f,-0.491916001f,
   4.344027996f,2.585634947f,-0.470948994f,
   4.407729149f,2.531640053f,-0.486692011f,
   4.277169228f,2.652435064f,-0.591368973f,
   4.359482765f,2.615839005f,-0.596817017f,
   4.423033237f,2.561336994f,-0.603561997f,
   4.356679916f,2.682996035f,-0.719049990f,
   4.411540985f,2.663824081f,-0.713132024f,
   4.476089954f,2.606653929f,-0.692300975f,
   4.418953896f,2.707401037f,-0.782531023f,
   4.465268135f,2.695425034f,-0.757266998f,
   4.520603180f,2.639162064f,-0.745541990f,
   4.509554863f,2.719806910f,-0.819971979f,
   4.546655178f,2.709095001f,-0.803780019f,
   4.574544907f,2.669862986f,-0.780363023f,
   4.528081894f,2.467536926f,-0.000521000f,
   4.474318027f,2.418431997f,-0.361346990f,
   4.443850040f,2.449920893f,-0.495431989f,
   4.440760136f,2.490353107f,-0.629287004f,
   4.484666824f,2.534451008f,-0.708887994f,
   4.520164967f,2.557971001f,-0.762426972f,
   4.592619896f,2.569879055f,-0.790452003f,
   4.656565189f,2.680541992f,-0.799683988f,
   4.674691200f,2.598833084f,-0.808005989f,
   4.646702766f,2.728688002f,-0.822295010f,
   4.636909008f,2.750500917f,-0.856859028f,
   4.748031139f,2.683804989f,-0.800044000f,
   4.729446888f,2.606204033f,-0.807512999f,
   4.728930950f,2.721892118f,-0.832624972f,
   4.719171047f,2.751091003f,-0.865965009f,
   4.821324825f,2.691793919f,-0.791886985f,
   4.821335793f,2.621498108f,-0.789636016f,
   4.811798096f,2.728679895f,-0.815406024f,
   4.801435947f,2.750224113f,-0.858918011f,
   5.096148014f,2.759720087f,-0.774729013f,
   4.442912102f,2.343262911f,-0.533219993f,
   4.440712929f,2.398472071f,-0.629127979f,
   4.474625111f,2.465214014f,-0.751890004f,
   4.314039230f,2.329459906f,-0.577509999f,
   4.320630074f,2.385528088f,-0.682582974f,
   4.296769142f,2.260421991f,-0.528796017f,
   4.382052898f,2.433474064f,-0.798757017f,
   4.538177013f,2.513353109f,-0.776566029f,
   4.444738865f,2.475795031f,-0.860514998f,
   4.601514816f,2.533324003f,-0.794193029f,
   4.554459095f,2.511416912f,-0.850570977f,
   4.683343887f,2.554220915f,-0.821927011f,
   4.681847095f,2.551035881f,-0.886234999f,
   4.737893105f,2.564647913f,-0.830389023f,
   4.755191803f,2.568818092f,-0.876311004f,
   4.829566002f,2.580796957f,-0.821826994f,
   4.856248856f,2.593811989f,-0.867066979f,
   4.662220955f,2.556345940f,-0.940617979f,
   4.497718811f,2.547480106f,-0.937659025f,
   4.763337135f,2.590833902f,-0.929247975f,
   4.883021832f,2.613657951f,-0.893163025f,
   4.352602959f,2.524888039f,-0.890532017f,
   4.262797832f,2.490674019f,-0.818315983f,
   4.192789078f,2.411967039f,-0.668016016f,
   4.193959236f,2.370476007f,-0.616705000f,
   4.113934994f,2.334131002f,-0.526471019f,
   4.735138893f,2.645463943f,-0.950890005f,
   4.854957104f,2.660227060f,-0.924310029f,
   4.615662098f,2.633869886f,-0.977985978f,
   4.460581779f,2.622874975f,-0.957001984f,
   4.343216896f,2.589541912f,-0.893105984f,
   4.244622231f,2.554466963f,-0.811725974f,
   4.138687134f,2.474013090f,-0.642310977f,
   4.121994019f,2.458034039f,-0.570029020f,
   4.078509808f,2.411809921f,-0.472193003f,
   4.235494137f,2.636171103f,-0.803613007f,
   4.342998028f,2.664048910f,-0.904347003f,
   4.460785866f,2.695616007f,-0.949971974f,
   4.624800205f,2.708372116f,-0.989444971f,
   4.138494015f,2.557837963f,-0.652651012f,
   4.104291916f,2.537976027f,-0.543437004f,
   4.088356018f,2.510912895f,-0.453792989f,
   4.063204765f,2.509679079f,-0.342987001f,
   4.043796062f,2.386523962f,-0.400391996f,
   4.055262089f,2.505284071f,-0.297282010f,
   4.035752773f,2.399538040f,-0.343957007f,
   4.753015041f,2.699695110f,-0.973278999f,
   4.863708973f,2.722151041f,-0.936659992f,
   4.084506035f,2.570977926f,-0.218168005f,
   4.079812050f,2.571649075f,-0.000951000f,
   4.101754189f,2.557091951f,-0.265855998f,
   4.100906849f,2.560626030f,-0.302395999f,
   4.117366791f,2.568514109f,-0.384499997f,
   4.151183128f,2.579058886f,-0.494605988f,
   4.616625786f,2.749485970f,-0.939333022f,
   4.462368011f,2.725999117f,-0.882674992f,
   4.354196072f,2.711836100f,-0.826545000f,
   4.273414135f,2.686574936f,-0.753673971f,
   4.176064968f,2.636883020f,-0.618384004f,
   4.000110149f,2.424158096f,-0.313253999f,
   4.029016018f,2.519175053f,-0.249384999f,
   4.024213791f,2.527932882f,-0.000987000f,
   3.988172054f,2.489641905f,-0.001008000f,
   4.735445023f,2.749284983f,-0.940329015f,
   4.827733040f,2.754390001f,-0.905049026f,
   3.787342072f,2.396112919f,-0.001159000f,
   3.828001022f,2.352540016f,-0.233340994f,
   3.961908102f,2.347744942f,-0.374747008f,
   3.870548010f,2.292105913f,-0.368963987f,
   4.025363922f,2.359299898f,-0.402597010f,
   4.077562809f,2.305186987f,-0.510338008f,
   3.967084885f,2.280647993f,-0.537643015f,
   3.903911114f,2.241501093f,-0.512467980f,
   3.857922077f,2.141554117f,-0.521067023f,
   3.823348999f,2.141732931f,-0.428355992f,
   4.112301826f,2.285831928f,-0.595499992f,
   4.010847092f,2.270246029f,-0.622148991f,
   3.928869009f,2.223918915f,-0.615683019f,
   3.892378092f,2.149431944f,-0.603588998f,
   4.146067142f,2.278101921f,-0.707372010f,
   4.044730186f,2.254458904f,-0.744208992f,
   3.990744114f,2.205447912f,-0.710794985f,
   3.945463896f,2.130100012f,-0.689535975f,
   4.212974072f,2.303184032f,-0.587113023f,
   4.212058067f,2.279449940f,-0.625926971f,
   4.229331970f,2.274522066f,-0.672748983f,
   4.172142982f,2.303394079f,-0.778972030f,
   4.079122066f,2.264103889f,-0.845009983f,
   4.024700165f,2.187859058f,-0.814218998f,
   4.273005009f,2.310528994f,-0.762171984f,
   4.215418816f,2.350483894f,-0.885764003f,
   4.104864120f,2.298352957f,-0.915735006f,
   4.059713840f,2.194905996f,-0.888152003f,
   4.061988831f,2.231987953f,-1.178382039f,
   4.102303982f,2.299959898f,-1.025861979f,
   4.155550957f,2.388662100f,-1.091959953f,
   4.097362041f,2.366600037f,-1.239985943f,
   4.194232941f,2.408334017f,-1.008471012f,
   4.307092190f,2.367481947f,-0.877201974f,
   4.266263008f,2.421650887f,-1.054798961f,
   4.228522778f,2.435029030f,-1.098574996f,
   4.170470238f,2.458441019f,-1.241845965f,
   4.094116211f,2.426172018f,-1.381031990f,
   4.058310986f,2.293363094f,-1.338053942f,
   4.046672821f,2.341348886f,-1.452530026f,
   4.047406197f,2.207209110f,-1.015107989f,
   3.989434004f,2.118832111f,-0.765085995f,
   4.022592068f,2.123051882f,-0.903284013f,
   4.020298958f,2.141674995f,-1.002364993f,
   4.036946774f,1.992166996f,-1.072561026f,
   4.024857998f,2.106568098f,-1.208099008f,
   4.082987785f,2.175805092f,-1.450929999f,
   4.103302956f,2.038564920f,-1.363736987f,
   4.152165890f,1.868775010f,-1.225002050f,
   4.031515121f,1.968248010f,-0.918460011f,
   4.127902985f,1.828359008f,-1.090512037f,
   3.922436953f,1.972460985f,-0.483563006f,
   3.966639042f,1.969285011f,-0.549291015f,
   3.982909918f,1.987390995f,-0.639809012f,
   4.094111919f,1.889281034f,-0.578435004f,
   4.059885025f,1.860460043f,-0.470086992f,
   4.017508984f,1.996168971f,-0.731653988f,
   4.150297165f,1.820574999f,-0.917046010f,
   4.314791203f,2.142163038f,-0.540650010f,
   4.342700005f,2.112281084f,-0.516691983f,
   4.305410862f,2.059245110f,-0.539407015f,
   4.148927212f,2.388725996f,-1.376878977f,
   4.101726055f,2.312318087f,-1.438163042f,
   4.149437904f,2.340549946f,-1.353693008f,
   4.169404030f,2.306276083f,-1.284031034f,
   4.140097141f,2.295948982f,-1.367195964f,
   4.129926205f,2.308769941f,-1.402258992f,
   4.141758919f,2.279958010f,-1.295339942f,
   4.197644234f,2.394643068f,-1.248646021f,
   4.197515965f,2.349134922f,-1.253043056f,
   4.199316978f,2.304651976f,-1.174540043f,
   4.274886131f,2.220030069f,-0.679075003f,
   4.267320156f,2.121987104f,-0.614939988f,
   4.380990028f,2.207700968f,-0.839016020f,
   4.354411125f,2.102984905f,-0.802579999f,
   4.369750977f,2.298203945f,-0.921894014f,
   4.303193092f,2.347122908f,-1.044412971f,
   4.245861053f,2.375092983f,-1.141312003f,
   4.218457222f,2.356833935f,-1.142444015f,
   4.275404930f,2.319905043f,-1.046399951f,
   4.296537876f,2.243778944f,-0.925459027f,
   4.289048195f,2.172969103f,-0.858693004f,
   4.211814880f,2.282490015f,-1.039137006f,
   4.278045177f,1.976660013f,-0.537351012f,
   4.290375233f,2.112337112f,-0.800189972f,
   4.222795010f,2.228178024f,-0.952751994f,
   4.197608948f,2.164062977f,-0.857433021f,
   4.153174877f,2.084822893f,-0.799668014f,
   4.151090145f,2.102938890f,-0.889759004f,
   4.148865223f,2.148828983f,-0.986567020f,
   4.154565811f,2.190479994f,-1.129552007f,
   4.152556896f,2.235471010f,-1.217046022f,
   4.131948948f,2.050756931f,-0.921855986f,
   4.119993210f,2.063095093f,-1.049178004f,
   4.126160145f,2.120929956f,-1.172520041f,
   4.133174896f,2.175230980f,-1.259323001f,
   4.137657166f,1.944836974f,-1.061023951f,
   4.159228802f,1.949902058f,-0.923196971f,
   4.143794060f,2.013720036f,-1.201385975f,
   4.123290062f,2.086853981f,-1.295135021f,
   4.112679958f,2.195089102f,-1.366907954f,
   4.111934185f,2.234813929f,-1.399948955f,
   4.232340813f,1.903079033f,-0.534362018f,
   4.281287193f,2.055751085f,-0.787000000f,
   4.354940891f,2.064161062f,-0.778850973f,
   4.245224953f,2.008532047f,-0.772641003f,
   2.872505903f,-0.444000006f,0.532456040f,
   2.835437059f,-0.231101006f,0.577889979f,
   2.736197948f,-0.346224993f,0.659344971f,
   2.832604647f,-0.444000006f,0.567921460f,
   2.946069241f,-0.444000006f,0.464577228f,
   2.986407995f,-0.413502991f,0.428963989f,
   2.976153135f,-0.444000006f,0.429265916f,
   2.996782541f,-0.444000006f,0.413102359f,
   3.009100914f,1.971603990f,0.034219999f,
   3.233974934f,2.059192896f,0.072430998f,
   3.036360979f,1.913988948f,0.169837996f,
   3.252223969f,2.026346922f,0.148214996f,
   3.058366060f,1.773345947f,0.297336996f,
   3.276333094f,1.863772988f,0.294265985f,
   3.086666107f,1.632990956f,0.338862985f,
   3.337544918f,1.748926997f,0.340209991f,
   3.115382910f,1.492390037f,0.380228013f,
   3.377233028f,1.587800026f,0.400781006f,
   3.163181067f,1.209972024f,0.462437987f,
   3.473409891f,1.366148949f,0.454658002f,
   3.189663887f,0.907503009f,0.500122011f,
   3.528445005f,1.060127020f,0.489443004f,
   3.280680895f,0.510801971f,0.499433994f,
   3.559393883f,0.706516027f,0.452535003f,
   3.321825027f,0.266689986f,0.465750009f,
   3.619649887f,0.472045988f,0.391741991f,
   3.360039949f,0.165487006f,0.412685007f,
   3.641195059f,0.407454014f,0.362197995f,
   3.378798962f,0.073735997f,0.355361998f,
   3.674710035f,0.338838995f,0.307839990f,
   3.438950062f,-0.082580000f,0.257014990f,
   3.700362921f,0.265794009f,0.240761995f,
   3.514981031f,-0.121886000f,0.187589005f,
   3.737014055f,0.181837007f,0.176818997f,
   3.458300114f,2.151655912f,0.102661997f,
   3.727092981f,2.308406115f,0.150049001f,
   3.955635071f,0.432417005f,0.197586000f,
   4.120841980f,0.627140999f,0.185461000f,
   3.889626026f,0.475277007f,0.247364998f,
   3.861840963f,0.560638011f,0.311960995f,
   4.105197906f,0.672581017f,0.300000995f,
   3.849351883f,0.664451003f,0.387198001f,
   4.068249226f,0.798925996f,0.400813013f,
   3.823137045f,0.853053987f,0.461519003f,
   4.041656971f,1.039755940f,0.496921986f,
   3.747724056f,1.184932947f,0.518661976f,
   3.988509893f,1.297806025f,0.539537013f,
   3.650974035f,1.463739991f,0.478617013f,
   3.886058092f,1.550140977f,0.504830003f,
   3.590526104f,1.701089978f,0.411994010f,
   3.881998062f,1.764428020f,0.476336986f,
   3.541188002f,1.849182963f,0.386366010f,
   3.814876080f,1.968415976f,0.442999989f,
   3.479418039f,1.978250027f,0.317478001f,
   3.758214951f,2.206682920f,0.323051006f,
   3.460196018f,2.103493929f,0.180368006f,
   3.745668888f,2.266000986f,0.241929993f,
   3.285924911f,-0.444000006f,0.057278227f,
   3.342159033f,-0.384674013f,0.201349005f,
   3.296721935f,-0.444000006f,0.213598415f,
   2.715712070f,1.828219056f,0.167708993f,
   2.361088991f,1.766659975f,0.202594995f,
   2.738132000f,1.687332988f,0.295046002f,
   2.406969070f,1.645328045f,0.341479003f,
   2.775680065f,1.538709998f,0.352829009f,
   2.519026995f,1.485931039f,0.422962010f,
   2.815366030f,1.377583027f,0.413399011f,
   2.553870916f,1.284278035f,0.512335002f,
   2.854290962f,1.093950987f,0.494650006f,
   2.637460947f,1.017259955f,0.581610978f,
   2.948386908f,0.783357024f,0.515057981f,
   2.747936964f,0.712252975f,0.615674019f,
   3.061089993f,0.397226006f,0.574193001f,
   2.811503887f,0.469098002f,0.657442987f,
   2.898732901f,0.271916002f,0.651297987f,
   3.136321068f,0.171237007f,0.539896011f,
   2.919869900f,0.084875003f,0.642459989f,
   3.166160107f,0.021310000f,0.457260996f,
   2.953028917f,-0.127690002f,0.520498991f,
   3.199570894f,-0.153739005f,0.380149990f,
   3.286426067f,-0.300743997f,0.276331007f,
   3.178251743f,-0.444000006f,0.299794525f,
   3.265956402f,-0.444000006f,0.234460309f,
   2.679445028f,0.687426984f,0.720573008f,
   2.753155947f,0.400828987f,0.713141024f,
   2.790680885f,0.219187006f,0.718262970f,
   2.790657043f,0.044323999f,0.717487991f,
   2.743994951f,-0.129851997f,0.718276978f,
   2.396354914f,1.242238998f,0.597721994f,
   2.482917070f,0.997636974f,0.702425003f,
   2.514162064f,0.988713026f,0.674526989f,
   2.598472357f,-0.444000006f,0.951928616f,
   2.594499111f,-0.422948003f,0.981262982f,
   2.585894108f,-0.444000006f,0.973843992f,
   2.660259724f,-0.444000006f,0.813532174f,
   2.690576077f,-0.368057013f,0.819342971f,
   2.607429981f,-0.208248004f,0.974218011f,
   2.702577114f,-0.197405994f,0.795611024f,
   2.649413109f,0.031535000f,1.024796009f,
   2.734183073f,0.038768999f,0.865809977f,
   2.668936014f,0.206182003f,1.014371037f,
   2.731512070f,0.219003007f,0.891584992f,
   2.616000891f,0.447717011f,0.960748017f,
   2.706959963f,0.393597007f,0.849582016f,
   2.570400000f,0.738682985f,0.830404997f,
   2.604217052f,0.728827000f,0.769671023f,
   2.689333677f,-0.444000006f,0.744967103f,
   2.708759785f,-0.444000006f,0.667125463f,
   2.542438984f,0.999927998f,0.648591995f,
   2.132390022f,1.755820990f,0.180271000f,
   2.183002949f,1.654070020f,0.341767013f,
   2.301358938f,1.397721052f,0.546015024f,
   2.069379091f,1.727818012f,0.212745994f,
   2.096551895f,1.678482056f,0.326810002f,
   2.189847946f,1.468991995f,0.517500997f,
   2.266002893f,1.273252964f,0.663331985f,
   2.347316980f,1.066113949f,0.758763015f,
   2.399389029f,0.782091022f,0.915278971f,
   2.448968887f,0.529857993f,1.037335038f,
   2.495353937f,0.250521988f,1.132081032f,
   2.500489950f,-0.035029002f,1.170179009f,
   2.452588081f,-0.244072005f,1.138797045f,
   2.439826965f,-0.395384997f,1.131324053f,
   2.472804070f,-0.444000006f,1.069493294f,
   2.426175356f,-0.444000006f,1.109490395f,
   2.161983013f,-0.444000006f,1.204555511f,
   2.108911991f,-0.357127994f,1.259691000f,
   2.081761360f,-0.444000006f,1.240276933f,
   2.210929871f,-0.444000006f,1.179537416f,
   1.807582974f,1.756546021f,0.177626997f,
   2.293758154f,-0.444000006f,1.129045844f,
   2.136269093f,-0.187665999f,1.278056979f,
   2.141789913f,-0.055305999f,1.295565009f,
   2.081700087f,0.283248991f,1.259461045f,
   2.023441076f,0.529338002f,1.144399047f,
   1.975940943f,0.827956975f,1.018422961f,
   1.958253980f,1.130270958f,0.861813009f,
   1.859442949f,1.450392962f,0.632221997f,
   1.821946025f,1.618209004f,0.403750986f,
   1.798719049f,1.714447021f,0.259351999f,
   0.223642007f,1.684074998f,0.160742998f,
   0.737904012f,1.604212999f,0.281713009f,
   0.237330005f,1.595746040f,0.288672000f,
   0.725959003f,1.478055000f,0.461804003f,
   0.289038986f,1.496446013f,0.430054992f,
   0.760721982f,1.205036044f,0.700545013f,
   0.305671006f,1.298416972f,0.629719019f,
   0.857322991f,0.985001028f,0.874028981f,
   0.243472993f,1.134081960f,0.819993019f,
   0.921311975f,0.755658984f,0.999329984f,
   0.316341013f,0.785310984f,1.086259961f,
   0.939270973f,0.379985988f,1.185871959f,
   0.539251983f,0.353983998f,1.254492998f,
   0.927775025f,0.195225000f,1.233042955f,
   0.539150000f,0.148261994f,1.321014047f,
   0.950214982f,0.042750999f,1.256188035f,
   0.516871989f,-0.020713000f,1.356626987f,
   0.943854988f,-0.101126999f,1.274770975f,
   0.542385995f,-0.188226998f,1.370375037f,
   0.960321009f,-0.308052003f,1.243132949f,
   0.546875000f,-0.390940011f,1.363244057f,
   0.969213724f,-0.444000006f,1.178807974f,
   0.715793788f,-0.444000006f,1.275204659f,
   0.547704160f,-0.444000006f,1.356244683f,
   0.000000000f,0.177044600f,1.466482997f,
   0.000000000f,0.165525094f,1.472229123f,
   0.005537000f,0.163518995f,1.472265005f,
   -0.000000000f,0.163652882f,1.473217964f,
   0.000000000f,0.007672692f,1.524864674f,
   0.018085999f,0.002865000f,1.523198962f,
   0.000000000f,-0.001937013f,1.528382778f,
   0.000000000f,-0.220245898f,1.551265836f,
   0.034086999f,-0.225059003f,1.542948961f,
   0.000000000f,-0.246511206f,1.552464724f,
   0.007318492f,-0.444000006f,1.571030974f,
   0.000000000f,-0.444000006f,1.572604299f,
   0.015450112f,-0.444000006f,1.569282770f,
   1.462507010f,1.745481014f,0.222394004f,
   1.483832002f,1.697594047f,0.288780987f,
   1.497138023f,1.618934989f,0.401107997f,
   1.504951000f,1.483492970f,0.585976005f,
   1.517125964f,1.334367037f,0.741075993f,
   1.570685029f,0.938327014f,0.989108026f,
   1.582203031f,0.771082997f,1.068179965f,
   1.630079985f,0.420235991f,1.172369003f,
   1.649026990f,0.224457994f,1.211969018f,
   1.660354972f,0.019397000f,1.246001959f,
   1.664159060f,-0.189658999f,1.266728997f,
   1.678622007f,-0.410061002f,1.291177034f,
   1.723684192f,-0.444000006f,1.277764320f,
   1.673678398f,-0.444000006f,1.285483241f,
   1.611559153f,-0.444000006f,1.250568509f,
   1.179102182f,-0.444000006f,1.170599937f,
   0.000000000f,1.707608700f,0.077861279f,
   0.000000000f,1.685217500f,0.156050697f,
   0.000000000f,1.658087611f,0.208980292f,
   0.000000000f,1.610479951f,0.278275162f,
   0.000000000f,1.580782294f,0.337802678f,
   0.000000000f,1.533421040f,0.408035219f,
   0.000000000f,1.480935812f,0.500656247f,
   0.000000000f,1.407327890f,0.576042175f,
   0.000000000f,1.302784085f,0.672887564f,
   0.000000000f,1.229821324f,0.753033578f,
   0.000000000f,1.085736513f,0.876621664f,
   0.000000000f,0.881539166f,1.035832524f,
   0.000000000f,0.545683205f,1.276811123f,
   0.000000000f,0.440043718f,1.330055952f,
   0.109243728f,-0.444000006f,1.538063526f,
   0.291339308f,-0.444000006f,1.464742541f,
   0.483607918f,-0.444000006f,1.381088018f,
   4.925498962f,1.917814016f,0.613412976f,
   4.953402996f,1.907938957f,0.605609000f,
   4.971871853f,1.924078941f,0.586026013f,
   4.953739166f,1.952579975f,0.592256010f,
   4.925394058f,1.955229044f,0.618836999f,
   4.897808075f,1.946794033f,0.628057003f,
   5.001903057f,1.914397955f,0.486723989f,
   4.973133087f,1.992779016f,0.533465981f,
   4.982364178f,1.873299956f,0.536189973f,
   4.926194191f,1.859812975f,0.582136989f,
   4.852578163f,1.889721036f,0.605031013f,
   4.824747086f,1.946835041f,0.626357973f,
   4.861610889f,2.001638889f,0.621913016f,
   4.935561180f,2.016016006f,0.585699975f,
   4.861618042f,1.854066968f,0.618094981f,
   4.760921001f,1.881356955f,0.613120019f,
   4.935880184f,2.051665068f,0.572853029f,
   4.842883110f,2.021332979f,0.638024986f,
   4.991745949f,2.009819031f,0.523205996f,
   4.944705963f,1.840662956f,0.575374007f,
   4.991709232f,1.844295025f,0.520761013f,
   4.852581978f,1.816115022f,0.603105009f,
   4.908019066f,1.796041012f,0.587881982f,
   4.963545799f,1.783161044f,0.553515017f,
   4.983034134f,1.806342959f,0.505779028f,
   5.011429787f,1.875568986f,0.463205010f,
   4.964944839f,2.053575039f,0.499902993f,
   4.918466091f,2.084724903f,0.532712996f,
   5.020077229f,1.978170037f,0.480978012f,
   4.881574154f,2.094571114f,0.539949000f,
   4.825995922f,2.089175940f,0.576083004f,
   4.788475990f,2.028047085f,0.608618975f,
   4.798296928f,2.118191004f,0.591085017f,
   4.751519203f,2.039658070f,0.634133995f,
   4.872539043f,2.159259081f,0.542891026f,
   4.927885056f,2.156559944f,0.516578972f,
   5.002439976f,2.105746031f,0.468232989f,
   5.057495117f,2.002751112f,0.451974005f,
   5.075994015f,1.900998950f,0.443792999f,
   5.030289173f,1.827419043f,0.440802991f,
   4.991903782f,1.760828972f,0.510385990f,
   4.936066151f,1.737668037f,0.557276011f,
   4.871154785f,1.741237998f,0.592325985f,
   4.816181183f,1.797860026f,0.604026020f,
   5.085813046f,1.888502955f,0.408327013f,
   5.095375061f,1.961238027f,0.401515990f,
   5.058178902f,1.788185954f,0.417387009f,
   5.019835949f,1.739907026f,0.485561997f,
   4.917973042f,1.681480050f,0.544201016f,
   4.862636089f,1.684144020f,0.570155025f,
   4.752336979f,1.723387957f,0.591296017f,
   4.697607040f,1.768061042f,0.576663017f,
   5.067202091f,2.027704000f,0.422291011f,
   5.030641079f,2.102196932f,0.432328999f,
   4.937950134f,2.181154966f,0.486939013f,
   4.881855011f,2.194544077f,0.530287981f,
   4.807794094f,2.143652916f,0.570393026f,
   4.728962898f,2.036145926f,0.614306986f,
   4.742358208f,1.882231951f,0.621649981f,
   4.688114166f,2.046377897f,0.604517996f,
   4.744050026f,2.179337025f,0.556056976f,
   4.837330818f,2.235677958f,0.479332000f,
   4.893849850f,2.220523119f,0.417712986f,
   4.995977879f,2.148785114f,0.344534010f,
   5.060840130f,2.053334951f,0.309323996f,
   5.097062111f,1.954205990f,0.328794986f,
   5.105588913f,1.864086032f,0.347113013f,
   4.697023869f,1.862575054f,0.604048014f,
   4.832890034f,2.262727022f,0.283787012f,
   4.739440918f,2.299169064f,0.369987011f,
   4.971394062f,2.146686077f,0.224781007f,
   5.072165012f,2.043629885f,0.209227994f,
   5.127157211f,1.942759037f,0.211214006f,
   5.152698040f,1.830661058f,0.287041992f,
   5.133171082f,1.715563059f,0.334257990f,
   5.103426933f,1.624403954f,0.434275001f,
   4.982821941f,1.503070951f,0.507606030f,
   4.927281857f,1.486953020f,0.526327014f,
   4.798409939f,1.473148942f,0.570617974f,
   4.715645790f,1.502195001f,0.584339023f,
   4.615407944f,1.639641047f,0.577768028f,
   4.560750961f,1.785153985f,0.562427998f,
   4.551484108f,1.841748953f,0.575191021f,
   4.533113003f,1.989009023f,0.578975022f,
   4.561298847f,2.133065939f,0.547248006f,
   4.636581898f,2.263382912f,0.471996993f,
   5.156644821f,1.942901969f,0.119993001f,
   5.218821049f,1.803552032f,0.199305996f,
   5.216855049f,1.636644959f,0.279796988f,
   5.233176231f,1.442458987f,0.362987995f,
   4.596477985f,1.512987018f,0.598981977f,
   4.679115772f,1.438434005f,0.589658022f,
   4.560023785f,1.604022980f,0.589332998f,
   4.477780819f,1.687857985f,0.597406983f,
   4.459681988f,1.778884053f,0.588185012f,
   4.432610035f,1.878872991f,0.577888012f,
   4.433020115f,2.024713993f,0.563794017f,
   4.634995937f,2.369115114f,0.140117005f,
   4.577572823f,2.389534950f,0.256195992f,
   4.472349167f,2.325133085f,0.443764001f,
   4.389483929f,2.190421104f,0.473320007f,
   5.223190784f,1.371490955f,0.388069004f,
   5.158321857f,1.236770034f,0.418042988f,
   4.955359936f,1.236724019f,0.505230010f,
   4.761600971f,1.363013029f,0.571370006f,
   5.321135998f,1.795562029f,0.119606003f,
   5.421809196f,1.685348988f,0.123554997f,
   5.383207798f,1.619660974f,0.202452004f,
   5.373133183f,1.541141987f,0.246711999f,
   5.334328175f,1.402712941f,0.332639009f,
   5.296963215f,1.322443008f,0.357984006f,
   5.260653019f,1.184172988f,0.352061987f,
   5.250484943f,1.123028994f,0.385233998f,
   4.991713047f,1.163095951f,0.504149020f,
   4.733656883f,1.227370977f,0.592882991f,
   4.641547203f,1.285421968f,0.622034013f,
   4.522246838f,1.398277998f,0.644891977f,
   4.430998802f,1.535753012f,0.638888001f,
   4.404221058f,1.662433028f,0.616609991f,
   4.386380196f,1.770867944f,0.596665978f,
   4.377095222f,1.871713042f,0.595743001f,
   5.604887962f,1.546090961f,0.113773003f,
   5.566339016f,1.498677015f,0.190905005f,
   5.528185844f,1.440179944f,0.250665992f,
   5.471928120f,1.345178962f,0.313899010f,
   5.425783157f,1.264371991f,0.329686999f,
   5.397966862f,1.174237967f,0.346801996f,
   5.388454914f,1.119418979f,0.351882994f,
   5.685194016f,1.452201962f,0.188749000f,
   5.730426788f,1.435667992f,0.209848002f,
   5.741781235f,1.463955998f,0.124890998f,
   5.687035084f,1.499636054f,0.110765003f,
   5.647185802f,1.394605994f,0.257833004f,
   5.608973980f,1.317868948f,0.319718987f,
   5.710154057f,1.360653043f,0.290071994f,
   5.563252926f,1.235295057f,0.317238003f,
   5.681725979f,1.282503009f,0.333871007f,
   5.580797195f,1.174134970f,0.351054013f,
   5.672186852f,1.164767027f,0.354079992f,
   5.479629993f,1.111310005f,0.364187986f,
   5.813217163f,1.415580988f,0.195261002f,
   5.886832237f,1.385674000f,0.172364995f,
   5.915025234f,1.382158995f,0.136820003f,
   5.842092037f,1.427708030f,0.130723998f,
   5.979356766f,1.325531960f,0.125337005f,
   5.941383839f,1.330818057f,0.178936005f,
   5.783730984f,1.341470957f,0.284590006f,
   5.847989082f,1.311568975f,0.261476010f,
   5.931474209f,1.233482957f,0.215612993f,
   5.998087883f,1.231873989f,0.107334003f,
   5.961686134f,1.110978007f,0.090324000f,
   5.934596062f,1.054795980f,0.077039003f,
   5.897758007f,1.009309053f,0.080582999f,
   5.923808098f,1.025282979f,0.153083995f,
   5.878630161f,0.986485004f,0.128291994f,
   5.950684071f,1.082725048f,0.175648004f,
   5.763951778f,1.062108994f,0.338640988f,
   5.755281925f,1.004148960f,0.307505012f,
   5.857749939f,0.997026026f,0.236768007f,
   5.884856224f,1.062201977f,0.249544993f,
   5.800777912f,1.078595996f,0.319449991f,
   5.959462166f,1.157227039f,0.187096998f,
   5.912147045f,1.117910981f,0.254197001f,
   5.757505894f,0.957899988f,0.210731998f,
   5.674999237f,0.950753987f,0.227960005f,
   5.837874889f,1.141492963f,0.305184007f,
   5.773409843f,1.225860953f,0.323024988f,
   5.764773846f,1.279824018f,0.308562011f,
   5.895898819f,1.182729959f,0.164704993f,
   5.849792004f,1.193480015f,0.181047007f,
   5.840664864f,1.221941948f,0.187128007f,
   5.803424835f,1.260787010f,0.210002005f,
   5.573802948f,0.952578008f,0.243885994f,
   5.672385216f,1.007694006f,0.341778010f,
   5.580727100f,0.999329984f,0.349866986f,
   5.535183907f,0.980180025f,0.341257006f,
   5.681086063f,1.054963946f,0.355859995f,
   5.690122128f,1.092916965f,0.370849997f,
   5.598281860f,1.094735026f,0.386994004f,
   5.470775127f,1.009574056f,0.355370998f,
   5.580223083f,1.047469974f,0.372696012f,
   5.425352097f,1.035966992f,0.342720002f,
   5.434594154f,0.970412016f,0.330823004f,
   5.480648994f,0.941386998f,0.316248000f,
   5.379460812f,1.045817018f,0.349747986f,
   5.370708942f,0.980271995f,0.337431997f,
   5.373645782f,0.885375023f,0.208839998f,
   5.546347141f,0.916043997f,0.246782005f,
   5.638369083f,0.904402018f,0.222547993f,
   5.748612881f,0.920490026f,0.205099002f,
   5.511556149f,0.863524973f,0.177613005f,
   5.603370190f,0.852747977f,0.162334993f,
   5.704629898f,0.849157989f,0.128130004f,
   5.241466045f,1.040469050f,0.383964986f,
   5.223570824f,0.974029005f,0.362116992f,
   5.299005032f,0.908990979f,0.260176986f,
   5.197562218f,0.922406018f,0.302473009f,
   5.851404190f,0.929008007f,0.105361998f,
   5.797410965f,0.880032003f,0.072306000f,
   5.568745136f,0.834653020f,0.071390003f,
   5.449163914f,0.847535014f,0.103909001f,
   5.137074947f,0.824518025f,0.144841999f,
   5.247762203f,0.848549008f,0.108546004f,
   5.356900215f,0.851119995f,0.138324007f,
   5.264430046f,0.855570972f,0.181694001f,
   5.153735161f,0.831574976f,0.218347996f,
   4.963525772f,0.797381997f,0.143439993f,
   4.996709824f,0.810559988f,0.280770987f,
   4.669690132f,0.738162994f,0.188296005f,
   4.712067127f,0.770511985f,0.333393008f,
   5.004627228f,0.908636987f,0.345272988f,
   4.728808880f,0.878731012f,0.405800998f,
   5.021254063f,0.951308012f,0.405566990f,
   5.011084080f,1.066769958f,0.458561987f,
   4.743249893f,1.059209943f,0.563421011f,
   4.744871140f,0.950945020f,0.491082013f,
   4.567895889f,1.149804950f,0.642484009f,
   4.496191025f,1.024566054f,0.579491019f,
   4.416474819f,0.921263993f,0.458838999f,
   4.297891140f,0.911505997f,0.447986990f,
   4.399629116f,0.850458980f,0.391855001f,
   4.333502769f,0.951175988f,0.481516987f,
   4.359047890f,0.700414002f,0.166650996f,
   4.319798946f,0.731006980f,0.291211993f,
   4.216373920f,0.852328002f,0.405873001f,
   4.366867065f,1.131135941f,0.630581021f,
   4.449010849f,1.260928988f,0.647431016f,
   4.141189098f,1.008774996f,0.499341995f,
   4.277226925f,1.077746034f,0.550818980f,
   4.175444126f,1.350332022f,0.613987982f,
   4.085564137f,1.288884997f,0.544405997f,
   4.247733116f,1.381057978f,0.649595976f,
   4.329607010f,1.464800954f,0.661485016f,
   4.312224865f,1.683392048f,0.639944017f,
   4.303615093f,1.799914002f,0.610387981f,
   4.304562092f,1.906537056f,0.572242022f,
   4.221814156f,1.660768986f,0.594089985f,
   4.240116119f,1.770048976f,0.583945990f,
   4.121120930f,1.614451051f,0.587188005f,
   4.067575932f,1.572667956f,0.534996986f,
   4.148882866f,1.759917974f,0.573764980f,
   4.076851845f,1.746600986f,0.527436018f,
   4.452383041f,2.524558067f,0.130254999f,
   4.342350960f,2.581176043f,0.140316993f,
   4.440143108f,2.510529041f,0.269163996f,
   4.332083225f,2.586481094f,0.194916993f,
   4.429894924f,2.495758057f,0.306894004f,
   4.339601040f,2.592642069f,0.258890986f,
   4.187257767f,2.590220928f,0.135844007f,
   4.203324795f,2.589185953f,0.219163999f,
   4.211651802f,2.602538109f,0.264010996f,
   4.328606129f,2.563996077f,0.343892008f,
   4.209940910f,2.600611925f,0.337597013f,
   4.410843849f,2.500166893f,0.351972014f,
   4.344027996f,2.585634947f,0.470948994f,
   4.224730015f,2.624525070f,0.491916001f,
   4.407729149f,2.531640053f,0.486692011f,
   4.359482765f,2.615839005f,0.596817017f,
   4.277169228f,2.652435064f,0.591368973f,
   4.423033237f,2.561336994f,0.603561997f,
   4.411540985f,2.663824081f,0.713132024f,
   4.356679916f,2.682996035f,0.719049990f,
   4.476089954f,2.606653929f,0.692300975f,
   4.465268135f,2.695425034f,0.757266998f,
   4.418953896f,2.707401037f,0.782531023f,
   4.520603180f,2.639162064f,0.745541990f,
   4.546655178f,2.709095001f,0.803780019f,
   4.509554863f,2.719806910f,0.819971979f,
   4.574544907f,2.669862986f,0.780363023f,
   4.474318027f,2.418431997f,0.361346990f,
   4.443850040f,2.449920893f,0.495431989f,
   4.440760136f,2.490353107f,0.629287004f,
   4.484666824f,2.534451008f,0.708887994f,
   4.520164967f,2.557971001f,0.762426972f,
   4.592619896f,2.569879055f,0.790452003f,
   4.674691200f,2.598833084f,0.808005989f,
   4.656565189f,2.680541992f,0.799683988f,
   4.646702766f,2.728688002f,0.822295010f,
   4.636909008f,2.750500917f,0.856859028f,
   4.729446888f,2.606204033f,0.807512999f,
   4.748031139f,2.683804989f,0.800044000f,
   4.728930950f,2.721892118f,0.832624972f,
   4.719171047f,2.751091003f,0.865965009f,
   4.821335793f,2.621498108f,0.789636016f,
   4.821324825f,2.691793919f,0.791886985f,
   4.811798096f,2.728679895f,0.815406024f,
   4.801435947f,2.750224113f,0.858918011f,
   5.096148014f,2.759720087f,0.774729013f,
   4.442912102f,2.343262911f,0.533219993f,
   4.440712929f,2.398472071f,0.629127979f,
   4.474625111f,2.465214014f,0.751890004f,
   4.320630074f,2.385528088f,0.682582974f,
   4.314039230f,2.329459906f,0.577509999f,
   4.296769142f,2.260421991f,0.528796017f,
   4.382052898f,2.433474064f,0.798757017f,
   4.538177013f,2.513353109f,0.776566029f,
   4.444738865f,2.475795031f,0.860514998f,
   4.601514816f,2.533324003f,0.794193029f,
   4.554459095f,2.511416912f,0.850570977f,
   4.683343887f,2.554220915f,0.821927011f,
   4.681847095f,2.551035881f,0.886234999f,
   4.755191803f,2.568818092f,0.876311004f,
   4.737893105f,2.564647913f,0.830389023f,
   4.856248856f,2.593811989f,0.867066979f,
   4.829566002f,2.580796957f,0.821826994f,
   4.497718811f,2.547480106f,0.937659025f,
   4.662220955f,2.556345940f,0.940617979f,
   4.763337135f,2.590833902f,0.929247975f,
   4.883021832f,2.613657951f,0.893163025f,
   4.352602959f,2.524888039f,0.890532017f,
   4.262797832f,2.490674019f,0.818315983f,
   4.192789078f,2.411967039f,0.668016016f,
   4.193959236f,2.370476007f,0.616705000f,
   4.113934994f,2.334131002f,0.526471019f,
   4.854957104f,2.660227060f,0.924310029f,
   4.735138893f,2.645463943f,0.950890005f,
   4.615662098f,2.633869886f,0.977985978f,
   4.460581779f,2.622874975f,0.957001984f,
   4.343216896f,2.589541912f,0.893105984f,
   4.244622231f,2.554466963f,0.811725974f,
   4.138687134f,2.474013090f,0.642310977f,
   4.121994019f,2.458034039f,0.570029020f,
   4.078509808f,2.411809921f,0.472193003f,
   4.342998028f,2.664048910f,0.904347003f,
   4.235494137f,2.636171103f,0.803613007f,
   4.460785866f,2.695616007f,0.949971974f,
   4.624800205f,2.708372116f,0.989444971f,
   4.138494015f,2.557837963f,0.652651012f,
   4.104291916f,2.537976027f,0.543437004f,
   4.088356018f,2.510912895f,0.453792989f,
   4.043796062f,2.386523962f,0.400391996f,
   4.063204765f,2.509679079f,0.342987001f,
   4.035752773f,2.399538040f,0.343957007f,
   4.055262089f,2.505284071f,0.297282010f,
   4.753015041f,2.699695110f,0.973278999f,
   4.863708973f,2.722151041f,0.936659992f,
   4.084506035f,2.570977926f,0.218168005f,
   4.101754189f,2.557091951f,0.265855998f,
   4.100906849f,2.560626030f,0.302395999f,
   4.117366791f,2.568514109f,0.384499997f,
   4.151183128f,2.579058886f,0.494605988f,
   4.462368011f,2.725999117f,0.882674992f,
   4.616625786f,2.749485970f,0.939333022f,
   4.354196072f,2.711836100f,0.826545000f,
   4.273414135f,2.686574936f,0.753673971f,
   4.176064968f,2.636883020f,0.618384004f,
   4.029016018f,2.519175053f,0.249384999f,
   4.000110149f,2.424158096f,0.313253999f,
   4.735445023f,2.749284983f,0.940329015f,
   4.827733040f,2.754390001f,0.905049026f,
   3.828001022f,2.352540016f,0.233340994f,
   3.870548010f,2.292105913f,0.368963987f,
   3.961908102f,2.347744942f,0.374747008f,
   4.025363922f,2.359299898f,0.402597010f,
   3.967084885f,2.280647993f,0.537643015f,
   4.077562809f,2.305186987f,0.510338008f,
   3.903911114f,2.241501093f,0.512467980f,
   3.823348999f,2.141732931f,0.428355992f,
   3.857922077f,2.141554117f,0.521067023f,
   4.010847092f,2.270246029f,0.622148991f,
   4.112301826f,2.285831928f,0.595499992f,
   3.928869009f,2.223918915f,0.615683019f,
   3.892378092f,2.149431944f,0.603588998f,
   4.044730186f,2.254458904f,0.744208992f,
   4.146067142f,2.278101921f,0.707372010f,
   3.990744114f,2.205447912f,0.710794985f,
   3.945463896f,2.130100012f,0.689535975f,
   4.212974072f,2.303184032f,0.587113023f,
   4.212058067f,2.279449940f,0.625926971f,
   4.172142982f,2.303394079f,0.778972030f,
   4.229331970f,2.274522066f,0.672748983f,
   4.079122066f,2.264103889f,0.845009983f,
   4.024700165f,2.187859058f,0.814218998f,
   4.215418816f,2.350483894f,0.885764003f,
   4.273005009f,2.310528994f,0.762171984f,
   4.104864120f,2.298352957f,0.915735006f,
   4.059713840f,2.194905996f,0.888152003f,
   4.097362041f,2.366600037f,1.239985943f,
   4.155550957f,2.388662100f,1.091959953f,
   4.102303982f,2.299959898f,1.025861979f,
   4.061988831f,2.231987953f,1.178382039f,
   4.194232941f,2.408334017f,1.008471012f,
   4.307092190f,2.367481947f,0.877201974f,
   4.266263008f,2.421650887f,1.054798961f,
   4.228522778f,2.435029030f,1.098574996f,
   4.170470238f,2.458441019f,1.241845965f,
   4.058310986f,2.293363094f,1.338053942f,
   4.094116211f,2.426172018f,1.381031990f,
   4.046672821f,2.341348886f,1.452530026f,
   4.047406197f,2.207209110f,1.015107989f,
   3.989434004f,2.118832111f,0.765085995f,
   4.022592068f,2.123051882f,0.903284013f,
   4.020298958f,2.141674995f,1.002364993f,
   4.024857998f,2.106568098f,1.208099008f,
   4.036946774f,1.992166996f,1.072561026f,
   4.082987785f,2.175805092f,1.450929999f,
   4.103302956f,2.038564920f,1.363736987f,
   4.152165890f,1.868775010f,1.225002050f,
   4.127902985f,1.828359008f,1.090512037f,
   4.031515121f,1.968248010f,0.918460011f,
   3.966639042f,1.969285011f,0.549291015f,
   3.922436953f,1.972460985f,0.483563006f,
   3.982909918f,1.987390995f,0.639809012f,
   4.059885025f,1.860460043f,0.470086992f,
   4.094111919f,1.889281034f,0.578435004f,
   4.017508984f,1.996168971f,0.731653988f,
   4.150297165f,1.820574999f,0.917046010f,
   4.342700005f,2.112281084f,0.516691983f,
   4.314791203f,2.142163038f,0.540650010f,
   4.305410862f,2.059245110f,0.539407015f,
   4.101726055f,2.312318087f,1.438163042f,
   4.148927212f,2.388725996f,1.376878977f,
   4.129926205f,2.308769941f,1.402258992f,
   4.140097141f,2.295948982f,1.367195964f,
   4.169404030f,2.306276083f,1.284031034f,
   4.149437904f,2.340549946f,1.353693008f,
   4.141758919f,2.279958010f,1.295339942f,
   4.197644234f,2.394643068f,1.248646021f,
   4.197515965f,2.349134922f,1.253043056f,
   4.199316978f,2.304651976f,1.174540043f,
   4.267320156f,2.121987104f,0.614939988f,
   4.274886131f,2.220030069f,0.679075003f,
   4.354411125f,2.102984905f,0.802579999f,
   4.380990028f,2.207700968f,0.839016020f,
   4.369750977f,2.298203945f,0.921894014f,
   4.303193092f,2.347122908f,1.044412971f,
   4.245861053f,2.375092983f,1.141312003f,
   4.218457222f,2.356833935f,1.142444015f,
   4.275404930f,2.319905043f,1.046399951f,
   4.296537876f,2.243778944f,0.925459027f,
   4.289048195f,2.172969103f,0.858693004f,
   4.211814880f,2.282490015f,1.039137006f,
   4.278045177f,1.976660013f,0.537351012f,
   4.290375233f,2.112337112f,0.800189972f,
   4.222795010f,2.228178024f,0.952751994f,
   4.197608948f,2.164062977f,0.857433021f,
   4.153174877f,2.084822893f,0.799668014f,
   4.151090145f,2.102938890f,0.889759004f,
   4.148865223f,2.148828983f,0.986567020f,
   4.154565811f,2.190479994f,1.129552007f,
   4.152556896f,2.235471010f,1.217046022f,
   4.131948948f,2.050756931f,0.921855986f,
   4.119993210f,2.063095093f,1.049178004f,
   4.126160145f,2.120929956f,1.172520041f,
   4.133174896f,2.175230980f,1.259323001f,
   4.159228802f,1.949902058f,0.923196971f,
   4.137657166f,1.944836974f,1.061023951f,
   4.143794060f,2.013720036f,1.201385975f,
   4.123290062f,2.086853981f,1.295135021f,
   4.112679958f,2.195089102f,1.366907954f,
   4.111934185f,2.234813929f,1.399948955f,
   4.232340813f,1.903079033f,0.534362018f,
   4.281287193f,2.055751085f,0.787000000f,
   4.354940891f,2.064161062f,0.778850973f,
   4.245224953f,2.008532047f,0.772641003f,
   0.000000000f,-0.444000006f,-1.571966887f,
   0.000000000f,-0.444000006f,-1.572916508f,
   0.000000000f,-0.444000006f,1.540538073f,
   0.000000000f,-0.444000006f,-1.521299124f,
   0.000000000f,-0.444000006f,1.538903475f,
   0.000000000f,-0.444000006f,-1.424398422f,
   0.000000000f,-0.444000006f,1.493143320f,
   0.000000000f,-0.444000006f,-1.359891295f,
   0.000000000f,-0.444000006f,1.256602645f,
   0.000000000f,-0.444000006f,-1.183533072f,
   0.000000000f,-0.444000006f,0.911852479f,
  };
  const size_t edgeCount = 65;
  size_t edges[edgeCount*2] = {
  0,1,
  5,0,
  6,5,
  8,7,
  7,6,
  66,67,
  69,66,
  93,8,
  94,93,
  96,94,
  67,96,
  107,105,
  105,108,
  120,107,
  121,120,
  138,139,
  108,138,
  140,141,
  139,140,
  177,178,
  180,177,
  181,182,
  178,181,
  195,194,
  209,210,
  141,209,
  210,211,
  211,212,
  212,180,
  229,195,
  182,229,
  1,121,
  723,720,
  720,724,
  724,726,
  726,727,
  779,777,
  777,69,
  727,801,
  801,802,
  802,779,
  813,811,
  811,814,
  826,827,
  814,826,
  843,813,
  844,843,
  847,845,
  845,848,
  848,850,
  850,844,
  882,881,
  883,882,
  895,896,
  909,847,
  910,909,
  911,910,
  881,912,
  912,911,
  896,927,
  927,928,
  928,929,
  929,883,
  827,723,
  1386,895,
 };

    fm_VertexIndex *vindex = fm_createVertexIndex((double)0.0001,false);

    for (size_t i=0; i<vcount; i++)
    {
      bool np;
      const float *v = &vertices[i*3];
      double p[3];
      p[0] = v[0];
      p[1] = v[1];
      p[2] = v[2];
      vindex->getIndex(p, np );
    }

    EdgeWalker ew(vcount,vindex,false);
    for (size_t i=0; i<edgeCount; i++)
    {
      size_t i1 = edges[i*2+0];
      size_t i2 = edges[i*2+1];

      const float *_p1 = &vertices[i1*3];
      const float *_p2 = &vertices[i2*3];

      double p1[3];
      double p2[3];

      p1[0] = _p1[0];
      p1[1] = _p1[1];
      p1[2] = _p1[2];

      p2[0] = _p2[0];
      p2[1] = _p2[1];
      p2[2] = _p2[2];

      bool np;
      i1 = vindex->getIndex(p1,np);
      i2 = vindex->getIndex(p2,np);

      ew.addEdge( i1, i2 );

    }

    ew.process();

    fm_releaseVertexIndex(vindex);


  }

  void testTriangulate(void)
  {
    double points[10*3] =
    {
      -1.884502053, 5.153564930, -1.050009012,
      -1.585170984, 5.153564930, -0.276419014,
      -1.364210963, 5.153564930, 0.024723001,
      -0.739763975, 5.153564930, 0.208785996,
      0.014838000, 5.153564930, -0.005838000,
      0.552298009, 5.153564930, 0.047825001,
      1.097702980, 5.153564930, 0.007746000,
      1.359846950, 5.153564930, 0.476752996,
      1.481034040, 5.153564930, 0.504414022,
      2.030071020, 5.153564930, 0.792048991
    };


    double results[10*3];

    size_t pcount = fm_consolidatePolygon(10,points,sizeof(double)*3,results);

    const double *prev = &results[(pcount-1)*3];
    const double *scan = results;
    for (size_t i=0; i<pcount; i++)
    {
      float p1[3];
      float p2[3];
      fm_doubleToFloat3(prev,p1);
      fm_doubleToFloat3(scan,p2);
      gRenderDebug->DebugRay(p1,p2,0.01f,0xFFFFFF,0xFF0000,30.0f);
      prev = scan;
      scan+=3;
    }

    fm_Triangulate *t = fm_createTriangulate();
    size_t tcount;
    const double *tris = t->triangulate3d(pcount,results,sizeof(double)*3,tcount);
    if ( tris )
    {
      for (size_t i=0; i<tcount; i++)
      {
        gRenderDebug->DebugTri(tris,tris+3,tris+6,0xFFFF00,30.0f);
        tris+=9;
      }
    }
    fm_releaseTriangulate(t);

  }

  void testSphereIntersect(void)
  {
    gRenderDebug->Reset();
    float radius = ranf(1,5);
    float center[3];
    center[0] = ranf(-1,1);
    center[1] = ranf(-1,1);
    center[2] = ranf(-1,1);

    gRenderDebug->DebugDetailedSphere(center,radius,128,0xFFFFFF,30.0f,true,true);

    for (size_t i=0; i<100; i++)
    {
      float p1[3];
      float p2[3];

      p1[0] = ranf(-5,5);
      p1[1] = ranf(-5,5);
      p1[2] = ranf(-5,5);

      p2[0] = ranf(-5,5);
      p2[1] = ranf(-5,5);
      p2[2] = ranf(-5,5);

      float intersect[3];
      bool hit = fm_lineSphereIntersect(center,radius,p1,p2,intersect);
      if ( hit )
      {
        gRenderDebug->DebugRay(p1,p2,0.1f,0xFFFF00,0xFF0000,30.0f);
        gRenderDebug->DebugDetailedSphere(intersect,0.1f,16,0xFF0000,30.0f);
      }
      else
      {
        gRenderDebug->DebugRay(p1,p2,0.1f,0x404040,0xFF0000,30.0f);
      }


    }

  }

  void testAABB(void)
  {
    float bmin[3] = { -1, -2, -3 };
    float bmax[3] = {  2,  4,  6 };

    gRenderDebug->DebugBound(bmin,bmax,0xFF00FF,60.0f,true,true);
    gRenderDebug->DebugBound(bmin,bmax,0xFFFFFF,60.0f,true,false);

    for (HeU32 i=0; i<10; i++)
    {
      float p1[3];
      float p2[3];

      p1[0] = ranf(-5,5);
      p1[1] = ranf(-5,5);
      p1[2] = ranf(-5,5);

      p2[0] = ranf(-5,5);
      p2[1] = ranf(-5,5);
      p2[2] = ranf(-5,5);

      float intersect[3];

      bool hit = fm_intersectLineSegmentAABB(bmin,bmax,p1,p2,intersect);
      if ( hit )
      {
        gRenderDebug->DebugRay(p1,p2,0.1f,0xFFFF00,0xFF0000,60.0f);
        gRenderDebug->DebugDetailedSphere(intersect,0.1f,16,0xFFFF00,60.0f);
      }
      else
      {
        gRenderDebug->DebugLine(p1,p2,0x808080,60.0f);
      }


    }

  }


  void importTer(const char *fname)
  {
    release();
    mImportHeightMap = createImportHeightMap();
    bool ok = mImportHeightMap->importHeightMap(fname);
    if ( ok )
    {
      mDesc.mWidth = mImportHeightMap->getWidth();
      mDesc.mDepth = mImportHeightMap->getDepth();
      mDesc.mData  = mImportHeightMap->getData();
      mDesc.mErodeCount = 0;
      mErode = ERODE::createErode(mDesc);
    }
  }

  void getPoint(ImportHeightMap *h,HeVec3 &v,HeU32 x,HeU32 z)
  {
    v.y = h->getPoint(x,z)*10;
    v.x = (HeF32)x;
    v.z = (HeF32)z;
  }


  void processTer(void)
  {
    if ( mErode )
    {
      bool ok = mErode->erode();
      mErode->getResults();
      gRenderDebug->Reset();
      HeU32 wid = mImportHeightMap->getWidth();
      HeU32 depth = mImportHeightMap->getDepth();
      for (HeU32 z=0; z<(depth-1); z++)
      {
        for (HeU32 x=0; x<(wid-1); x++)
        {
          HeVec3 p1;
          HeVec3 p2;
          HeVec3 p3;
          HeVec3 p4;

          getPoint(mImportHeightMap,p1,x,z);
          getPoint(mImportHeightMap,p2,x+1,z);
          getPoint(mImportHeightMap,p3,x+1,z+1);
          getPoint(mImportHeightMap,p4,x,z+1);

          gRenderDebug->DebugSolidTri(&p3.x,&p2.x,&p1.x,0x808080,60000.0f);
//          gRenderDebug->DebugTri(&p1.x,&p2.x,&p3.x,0xFFFFFF,60000.0f);

          gRenderDebug->DebugSolidTri(&p4.x,&p3.x,&p1.x,0x808080,60000.0f);
//          gRenderDebug->DebugTri(&p1.x,&p3.x,&p4.x,0xFFFFFF,60000.0f);

        }
      }
      if ( !ok )
      {
        gLog->Display("Erosion complete.\r\n");
        release();
      }
    }
  }

private:
  bool         mRemoveTjunctions;
  bool         mPerlinNoise;
  bool         mDebugView;
  bool         mTesselate;
  bool         mSaveResults;
  bool         mWireframe;
  bool         mSolid;
  bool         mShowLeft;
  bool         mShowRight;
  bool         mShowRings;
  bool         mShowClosure;
  bool         mShowTriangles;
  bool         mShowSplitPlane;
  bool         mCollapseColinear;
  bool         mEdgeIntersect;
  HeF64        mExplodeDistance;
  SPLIT_MESH::SplitMesh   *mSplitMesh;
  SPLIT_MESH::RobustMesh  *mRobustMesh;
  HeF64        mPlane[4];
  HeF64        mProject[3];
  HeF64        mRotate[3];

  SPLIT_MESH::SimpleMesh   mLeftMesh;
  SPLIT_MESH::SimpleMesh   mRightMesh;


  ImportHeightMap  *mImportHeightMap;
  ERODE::Erode     *mErode;
  ERODE::ErodeDesc  mDesc;
};


static SplitMeshApp gApp;


void appSetMeshSystemHelper(MeshSystemHelper *ms)
{
  gApp.setMeshSystemHelper(ms);
}

void appRender(void)
{
  gApp.render();
}


void appCommand(SplitMeshCommand _command,bool state,const float *data)
{
  gApp.command(_command,state,data);
}

void appImportTer(const char *fname)
{
  gApp.importTer(fname);
}
