#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TestBestFitCapsule.h"

using namespace NVSHARE;
#include "RenderDebug.h"
#include "FloatMath.h"
#include "shared/MeshSystem/MeshSystemHelper.h"
#include "rand.h"

void testBestFitCapsule(MeshSystemHelper *ms)
{
  gRenderDebug->reset();
  gRenderDebug->pushRenderState();
  gRenderDebug->setCurrentColor(0xFFFFFF);
  gRenderDebug->setCurrentDisplayTime(15);


  SEND_TEXT_MESSAGE(0,"Demonstrating the best fit capsule algorithm, originally published on June 16, 2006\r\n");

  NxF32 matrix[16];
  NxF32 radius=1;
  NxF32 height=1;

  if ( ms )
  {
    MeshSystemRaw *mr = ms->getMeshSystemRaw();
    if ( mr && mr->mVcount )
    {
      fm_computeBestFitCapsule(mr->mVcount,mr->mVertices,sizeof(NxF32)*3,radius,height,matrix,true);
      SEND_TEXT_MESSAGE(0,"Generating best fit capsule for input mesh with %d vertices.\r\n", mr->mVcount );
      ms->releaseMeshSystemRaw(mr);
    }
    else
    {
      ms = 0;
    }
  }
  if ( !ms )
  {
    Rand r;
    r.setSeed(rand());

    NxF32 quat[4];
    fm_eulerToQuat(r.ranf()*FM_PI,r.ranf()*FM_PI,r.ranf()*FM_PI,quat);
    fm_quatToMatrix(quat,matrix);

    #define PCOUNT 60
    NxF32 points[PCOUNT*3] =
    {
    };

    NxF32 *pos = points;


    for (NxI32 i=0; i<PCOUNT; i++)
    {
      NxF32 t[3] = { pos[0], pos[1], pos[2] };

      pos[0] = (NxF32) ((r.get()&63)-32)*0.35f;
      pos[1] = (NxF32) ((r.get()&63)-32)*0.025f;
      pos[2] = (NxF32) ((r.get()%63)-32)*0.015f;

      fm_transform(matrix,pos,t);

      gRenderDebug->DebugPoint(t,0.1f);

      pos[0] = t[0];
      pos[1] = t[1];
      pos[2] = t[2];

      pos+=3;
    }

    SEND_TEXT_MESSAGE(0,"Generating best fit capsule for random set of 60 data points.\r\n");
    fm_computeBestFitCapsule(PCOUNT,points,sizeof(NxF32)*3,radius,height,matrix,true);
  }

  gRenderDebug->DebugOrientedCapsule(radius,height,matrix);
  gRenderDebug->popRenderState();
}

