#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TestConvexDecomposition.h"

#include "SendTextMessage.h"


#include "RenderDebug.h"
#include "wavefront.h"
#include "ConvexDecomposition.h"
#include "SendTextMessage.h"
#include "FloatMath.h"
#include "shared/MeshSystem/MeshSystemHelper.h"

#pragma warning(disable:4100)

#define SHOW_TIME 1000

class ConvexResults : public ConvexDecomposition::ConvexDecompInterface
{
public:
  ConvexResults(bool fitobb)
  {
    mFitObb = fitobb;
    mCount = 0;
  }

  virtual void ConvexDecompResult(ConvexDecomposition::ConvexResult &result)
  {
    unsigned int color = gRenderDebug->getDebugColor();

	gRenderDebug->pushRenderState();
	gRenderDebug->setCurrentDisplayTime(15.0f);
    mCount++;

    if ( mFitObb )
    {
      float *vertices = (float *)MEMALLOC_MALLOC(sizeof(float)*result.mHullVcount*3);
      for (NxU32 i=0; i<result.mHullVcount*3; i++)
      {
        vertices[i] = (float)result.mHullVertices[i];
      }

      float matrix[16];
      float sides[3];
      fm_computeBestFitOBB(result.mHullVcount,vertices,sizeof(float)*3,sides,matrix,true);

      float bmin[3];
      float bmax[3];

      bmin[0] = -sides[0]*0.5f;
      bmin[1] = -sides[1]*0.5f;
      bmin[2] = -sides[2]*0.5f;

      bmax[0] = sides[0]*0.5f;
      bmax[1] = sides[1]*0.5f;
      bmax[2] = sides[2]*0.5f;
	  gRenderDebug->setCurrentState(DebugRenderState::SolidShaded);
	  gRenderDebug->setCurrentColor(color,0xFFFFFF);
      gRenderDebug->DebugOrientedBound(bmin,bmax,matrix);


      MEMALLOC_FREE(vertices);
    }
    else
    {
	  gRenderDebug->setCurrentState(DebugRenderState::SolidShaded);
	  gRenderDebug->setCurrentColor(color,0xFFFFFF);
      for (unsigned int i=0; i<result.mHullTcount; i++)
      {
        unsigned int i1 = result.mHullIndices[i*3+0];
        unsigned int i2 = result.mHullIndices[i*3+1];
        unsigned int i3 = result.mHullIndices[i*3+2];
        const double *p1 = &result.mHullVertices[i1*3];
        const double *p2 = &result.mHullVertices[i2*3];
        const double *p3 = &result.mHullVertices[i3*3];
        gRenderDebug->DebugTri(p1,p2,p3);
      }
    }
	gRenderDebug->popRenderState();
  }
  bool mFitObb;
  int mCount;
};

void testConvexDecomposition(MeshSystemHelper *ms,
                             unsigned int depth,
                             float mergePercentage,
                             float concavityPercentage,
                             float volumePercentage,
                             unsigned int maxVertices,
                             float skinWidth,
                             bool fitObb,
                             bool removeTjunctions,
                             bool initialIslandGeneration,
                             bool islandGeneration)
{
  SEND_TEXT_MESSAGE(0,"Demonstrating how to perform Convex Decomposition\r\n");

  if ( ms )
  {

    gRenderDebug->reset();
	gRenderDebug->pushRenderState();

    MeshSystemRaw *mr = ms->getMeshSystemRaw();

    if ( mr && mr->mVcount )
    {
      ConvexDecomposition::DecompDesc desc;
      desc.mVcount = mr->mVcount;
      double *dest = (double *)MEMALLOC_MALLOC( sizeof(double)*mr->mVcount*3);
      for (NxU32 i=0; i<(NxU32)(mr->mVcount*3); i++)
      {
        float f = mr->mVertices[i];
        dest[i] = (double) f;
      }
      desc.mVertices = dest;
      desc.mTcount = mr->mTcount;
      desc.mIndices = (unsigned int *)mr->mIndices;
      desc.mDepth = depth;
      desc.mCpercent = concavityPercentage;
      desc.mPpercent = mergePercentage;
      desc.mVpercent = volumePercentage/100.0f;
      desc.mMaxVertices = maxVertices;
      desc.mSkinWidth = skinWidth;
      desc.mRemoveTjunctions = removeTjunctions;
      desc.mInitialIslandGeneration = initialIslandGeneration;
      desc.mUseIslandGeneration = islandGeneration;

      ConvexResults r(fitObb);
      desc.mCallback = static_cast< ConvexDecomposition::ConvexDecompInterface *>(&r);

      SEND_TEXT_MESSAGE(0,"Generating convex decomposition.\r\n");

      ConvexDecomposition::performConvexDecomposition(desc);

      SEND_TEXT_MESSAGE(0,"Generated %d convex hull results.\r\n", r.mCount );

      MEMALLOC_FREE(dest);
    }
    else
    {
      SEND_TEXT_MESSAGE(0,"Empty input mesh.\r\n");
    }

    ms->releaseMeshSystemRaw(mr);
	gRenderDebug->popRenderState();
  }
  else
  {
    SEND_TEXT_MESSAGE(0,"No mesh loaded.\r\n");
  }
}

