#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TestBestFitOBB.h"

#include "RenderDebug.h"
#include "FloatMath.h"
#include "rand.h"
#include "shared/MeshSystem/MeshSystemHelper.h"
#include "SendTextMessage.h"

void testBestFitOBB(MeshSystemHelper *ms)
{
  gRenderDebug->reset();
  gRenderDebug->pushRenderState();
  gRenderDebug->setCurrentDisplayTime(600.0f);

  SEND_TEXT_MESSAGE(0,"Demonstrating the best fit OBB algorithm, originally published on June 16, 2006\r\n");

  NxF32 matrix[16];
  NxF32 sides[3];

  if ( ms )
  {
    MeshSystemRaw *mr = ms->getMeshSystemRaw();
    if ( mr && mr->mVcount )
    {
      fm_computeBestFitOBB(mr->mVcount,mr->mVertices,sizeof(NxF32)*3,sides,matrix,true);
      SEND_TEXT_MESSAGE(0,"Generating best fit obb for input mesh with %d vertices.\r\n", mr->mVcount );
      ms->releaseMeshSystemRaw(mr);
    }
    else
    {
      ms = 0;
    }
  }
  if ( !ms )
  {
    NxF32 quat[4];
    fm_eulerToQuat(ranf()*FM_PI,ranf()*FM_PI,ranf()*FM_PI,quat);
    fm_quatToMatrix(quat,matrix);

    #define PCOUNT 60
    NxF32 points[PCOUNT*3] =
    {
    };

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

    SEND_TEXT_MESSAGE(0,"Generating best fit obb for random set of 60 data points.\r\n");
    fm_computeBestFitOBB(PCOUNT,points,sizeof(NxF32)*3,sides,matrix,true);
  }

  NxF32 bmin[3];
  NxF32 bmax[3];

  bmin[0] = -sides[0]*0.5f;
  bmin[1] = -sides[1]*0.5f;
  bmin[2] = -sides[2]*0.5f;
  bmax[0] = sides[0]*0.5f;
  bmax[1] = sides[1]*0.5f;
  bmax[2] = sides[2]*0.5f;
  gRenderDebug->setCurrentColor(0xFFFF00,0xFFFFFF);
  gRenderDebug->DebugOrientedBound(bmin,bmax,matrix);
  gRenderDebug->popRenderState();
}

