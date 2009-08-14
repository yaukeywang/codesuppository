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
    NxU32 color = gRenderDebug->getDebugColor();

	gRenderDebug->pushRenderState();
	gRenderDebug->setCurrentDisplayTime(15.0f);
    mCount++;

    if ( mFitObb )
    {
      NxF32 *vertices = (NxF32 *)MEMALLOC_MALLOC(sizeof(NxF32)*result.mHullVcount*3);
      for (NxU32 i=0; i<result.mHullVcount*3; i++)
      {
        vertices[i] = (NxF32)result.mHullVertices[i];
      }

      NxF32 matrix[16];
      NxF32 sides[3];
      fm_computeBestFitOBB(result.mHullVcount,vertices,sizeof(NxF32)*3,sides,matrix,true);

      NxF32 bmin[3];
      NxF32 bmax[3];

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
      for (NxU32 i=0; i<result.mHullTcount; i++)
      {
        NxU32 i1 = result.mHullIndices[i*3+0];
        NxU32 i2 = result.mHullIndices[i*3+1];
        NxU32 i3 = result.mHullIndices[i*3+2];
        const NxF64 *p1 = &result.mHullVertices[i1*3];
        const NxF64 *p2 = &result.mHullVertices[i2*3];
        const NxF64 *p3 = &result.mHullVertices[i3*3];
        gRenderDebug->DebugTri(p1,p2,p3);
      }
    }
	gRenderDebug->popRenderState();
  }
  bool mFitObb;
  NxI32 mCount;
};

void testConvexDecomposition(MeshSystemHelper *ms,
                             NxU32 depth,
                             NxF32 mergePercentage,
                             NxF32 concavityPercentage,
                             NxF32 volumePercentage,
                             NxU32 maxVertices,
                             NxF32 skinWidth,
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
      NxF64 *dest = (NxF64 *)MEMALLOC_MALLOC( sizeof(NxF64)*mr->mVcount*3);
      for (NxU32 i=0; i<(NxU32)(mr->mVcount*3); i++)
      {
        NxF32 f = mr->mVertices[i];
        dest[i] = (NxF64) f;
      }
      desc.mVertices = dest;
      desc.mTcount = mr->mTcount;
      desc.mIndices = (NxU32 *)mr->mIndices;
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

