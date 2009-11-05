#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TestTjunctions.h"
#include "MeshImport.h"
using namespace NVSHARE;
#include "RemoveTjunctions.h"
#include "shared/MeshSystem/MeshSystemHelper.h"
#include "RenderDebug.h"

void testTjunctions(MeshSystemHelper * ms)
{
  SEND_TEXT_MESSAGE(0,"Demonstrates how to locate and remove T-junctions from a mesh\r\n");

   if ( ms )
   {
        RemoveTjunctionsDesc desc;
        MeshSystemRaw *mr = ms->getMeshSystemRaw();
        if ( mr && mr->mVcount )
        {
            RemoveTjunctions *rt = createRemoveTjunctions();
            desc.mVcount = mr->mVcount;
            desc.mVertices = mr->mVertices;
            desc.mTcount    = mr->mTcount;
            desc.mIndices   = mr->mIndices;
            desc.mIds       = 0;
            size_t tcount = rt->removeTjunctions(desc);
            gRenderDebug->reset(-1);
            if ( tcount )
            {
				SEND_TEXT_MESSAGE(0,"Input triangle count %d output triangle count %d.\r\n", mr->mTcount, tcount );
				gRenderDebug->pushRenderState();
				gRenderDebug->setCurrentColor(0xFFFF00,0xFFFFFF);
				gRenderDebug->setCurrentDisplayTime(6000);
                for (size_t i=0; i<tcount; i++)
                {
                    size_t i1 = desc.mIndicesOut[i*3+0];
                    size_t i2 = desc.mIndicesOut[i*3+1];
                    size_t i3 = desc.mIndicesOut[i*3+2];
                    const NxF32 *p1 = &desc.mVertices[i1*3];
                    const NxF32 *p2 = &desc.mVertices[i2*3];
                    const NxF32 *p3 = &desc.mVertices[i3*3];
                    gRenderDebug->DebugTri(p1,p2,p3);
                }
				gRenderDebug->popRenderState();
            }
            releaseRemoveTjunctions(rt);
        }
   }
}

