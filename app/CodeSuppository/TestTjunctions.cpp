#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TestTjunctions.h"
#include "MeshImport/MeshImport.h"
#include "SendTextMessage.h"
#include "RemoveTjunctions.h"
#include "shared/MeshSystem/MeshSystemHelper.h"
#include "RenderDebug/RenderDebug.h"

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
            desc.mVerticesF = mr->mVertices;
            desc.mTcount    = mr->mTcount;
            desc.mIndices   = mr->mIndices;
            desc.mIds       = 0;
            size_t tcount = rt->removeTjunctions(desc);
            gRenderDebug->Reset();
            if ( tcount )
            {
				SEND_TEXT_MESSAGE(0,"Input triangle count %d output triangle count %d.\r\n", mr->mTcount, tcount );
                for (size_t i=0; i<tcount; i++)
                {
                    size_t i1 = desc.mIndicesOut[i*3+0];
                    size_t i2 = desc.mIndicesOut[i*3+1];
                    size_t i3 = desc.mIndicesOut[i*3+2];
                    const float *p1 = &desc.mVerticesF[i1*3];
                    const float *p2 = &desc.mVerticesF[i2*3];
                    const float *p3 = &desc.mVerticesF[i3*3];
                    gRenderDebug->DebugTri(p1,p2,p3,0xFFFF00,6000);
                }
            }
            releaseRemoveTjunctions(rt);
        }
   }
}

