#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TestBestFitCapsule.h"

#include "common/snippets/SendTextMessage.h"
#include "RenderDebug/RenderDebug.h"
#include "common/snippets/FloatMath.h"
#include "common/snippets/rand.h"
#include "shared/MeshSystem/MeshSystemHelper.h"
#include "common/snippets/rand.h"

void testBestFitCapsule(MeshSystemHelper *ms)
{
  gRenderDebug->Reset();

  SEND_TEXT_MESSAGE(0,"Demonstrating the best fit capsule algorithm, originally published on June 16, 2006\r\n");

  float matrix[16];
  float radius=1;
  float height=1;

  if ( ms )
  {
    MeshSystemRaw *mr = ms->getMeshSystemRaw();
    if ( mr && mr->mVcount )
    {
      fm_computeBestFitCapsule(mr->mVcount,mr->mVertices,sizeof(float)*3,radius,height,matrix,true);
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

    float quat[4];
    fm_eulerToQuat(r.ranf()*FM_PI,r.ranf()*FM_PI,r.ranf()*FM_PI,quat);
    fm_quatToMatrix(quat,matrix);

    #define PCOUNT 60
    float points[PCOUNT*3] =
    {
    };

    float *pos = points;

    for (int i=0; i<PCOUNT; i++)
    {
      float t[3] = { pos[0], pos[1], pos[2] };

      pos[0] = (float) ((r.get()&63)-32)*0.35f;
      pos[1] = (float) ((r.get()&63)-32)*0.025f;
      pos[2] = (float) ((r.get()%63)-32)*0.015f;

      fm_transform(matrix,pos,t);

      gRenderDebug->DebugPoint(t,0.1f,0xFFFFFF,15.0f);

      pos[0] = t[0];
      pos[1] = t[1];
      pos[2] = t[2];

      pos+=3;
    }

    SEND_TEXT_MESSAGE(0,"Generating best fit capsule for random set of 60 data points.\r\n");
    fm_computeBestFitCapsule(PCOUNT,points,sizeof(float)*3,radius,height,matrix,true);
  }

  gRenderDebug->DebugOrientedCapsule(radius,height,matrix,0xFFFF00,600.0f);
}

