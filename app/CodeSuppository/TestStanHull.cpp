#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TestStanHull.h"

#include "RenderDebug/RenderDebug.h"
#include "common/snippets/wavefront.h"
#include "common/snippets/stanhull.h"
#include "common/snippets/SendTextMessage.h"
#include "shared/MeshSystem/MeshSystemHelper.h"

void testStanHull(MeshSystemHelper *ms)
{
  SEND_TEXT_MESSAGE(0,"Demonstrating StanHull code snippet, originally published on March 4, 2006\r\n");

  gRenderDebug->Reset();

  if ( ms )
  {
    MeshSystemRaw *mr = ms->getMeshSystemRaw();
    if ( mr && mr->mVcount )
    {
      SEND_TEXT_MESSAGE(0,"Generating convex hull for input mesh with %d vertices.\r\n", mr->mVcount );
      // now generate the convex hull.
      STAN_HULL::HullDesc desc((STAN_HULL::HullFlag)(STAN_HULL::QF_TRIANGLES | STAN_HULL::QF_SKIN_WIDTH),mr->mVcount,mr->mVertices,sizeof(HeF32)*3);
      desc.mMaxVertices = 64;
      desc.mSkinWidth = 0.1f;

      STAN_HULL::HullLibrary h;
      STAN_HULL::HullResult result;
      STAN_HULL::HullError e = h.CreateConvexHull(desc,result);
      if ( e == STAN_HULL::QE_OK )
      {
        SEND_TEXT_MESSAGE(0,"Output Hull contains %d triangle faces.\r\n", result.mNumFaces );
        SEND_TEXT_MESSAGE(0,"Fly the camera inside the hull to see the mesh details.\r\n");
        for (HeU32 i=0; i<result.mNumFaces; i++)
        {
          HeU32 i1 = result.mIndices[i*3+0];
          HeU32 i2 = result.mIndices[i*3+1];
          HeU32 i3 = result.mIndices[i*3+2];
          const HeF32 *p1 = &result.mOutputVertices[i1*3];
          const HeF32 *p2 = &result.mOutputVertices[i2*3];
          const HeF32 *p3 = &result.mOutputVertices[i3*3];
          gRenderDebug->DebugSolidTri(p1,p2,p3,0xFFFF00,600.0f);
          gRenderDebug->DebugSolidTri(p3,p2,p1,0xFFFF00,600.0f);
          gRenderDebug->DebugTri(p1,p2,p3,0xFFFFFF,600.0f);
        }
        h.ReleaseResult(result);
      }
      else
      {
        SEND_TEXT_MESSAGE(0,"Failed to create ConvexHull\r\n");
      }
    }
    else
    {
      SEND_TEXT_MESSAGE(0,"Empty input mesh.\r\n");
    }
    ms->releaseMeshSystemRaw(mr);
  }
  else
  {
    SEND_TEXT_MESSAGE(0,"No input mesh.\r\n");
  }

}

