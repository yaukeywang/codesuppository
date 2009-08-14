#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TestBestFitPlane.h"
#include "RenderDebug.h"
#include "FloatMath.h"
#include "rand.h"
#include "shared/MeshSystem/MeshSystemHelper.h"

#include "SendTextMessage.h"

void testBestFitPlane(MeshSystemHelper *ms)
{
  gRenderDebug->reset();
  gRenderDebug->pushRenderState();
  gRenderDebug->setCurrentDisplayTime(15);
  SEND_TEXT_MESSAGE(0,"Demonstration of Best Fit Plane to a set of points, originally published on March 23, 2006\r\n");

  NxF32 plane[4];

  if ( ms )
  {
    MeshSystemRaw *mr = ms->getMeshSystemRaw();
    if ( mr && mr->mVcount )
    {
      SEND_TEXT_MESSAGE(0,"Generating best fit plane for input mesh with %d vertices.\r\n", mr->mVcount );
      fm_computeBestFitPlane(mr->mVcount,mr->mVertices,sizeof(NxF32)*3,0,0,plane);
      ms->releaseMeshSystemRaw(mr);
    }
    else
    {
      ms = 0;
    }
  }
  if( !ms )
  {
    NxF32 quat[4];
    fm_eulerToQuat(ranf()*FM_PI,ranf()*FM_PI,ranf()*FM_PI,quat);
    NxF32 matrix[16];
    fm_quatToMatrix(quat,matrix);

    #define PCOUNT 60
    NxF32 points[PCOUNT*3];
    NxF32 *pos = points;

    for (NxI32 i=0; i<PCOUNT; i++)
    {

      NxF32 t[3] = { pos[0], pos[1], pos[2] };
      pos[0] = (NxF32) ((rand()&63)-32)*0.15f;
      pos[1] = (NxF32) ((rand()&63)-32)*0.0015f;
      pos[2] = (NxF32) ((rand()%63)-32)*0.05f;
      fm_transform(matrix,pos,t);
      gRenderDebug->DebugPoint(t,0.1f);

      pos[0] = t[0];
      pos[1] = t[1];
      pos[2] = t[2];

      pos+=3;
    }
    SEND_TEXT_MESSAGE(0,"Generating best fit plane for 60 random points.\r\n");
    fm_computeBestFitPlane(PCOUNT,points,sizeof(NxF32)*3,0,0,plane);
  }
  gRenderDebug->setCurrentColor(0xFFFF00,0xFFFFFF);
  gRenderDebug->DebugPlane(plane,4,4);
  gRenderDebug->popRenderState();
}

