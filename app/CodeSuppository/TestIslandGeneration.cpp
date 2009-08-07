#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TestIslandGeneration.h"
#include "MeshImport/MeshImport.h"
#include "SendTextMessage.h"
#include "MeshIslandGeneration.h"
#include "shared/MeshSystem/MeshSystemHelper.h"
#include "RenderDebug/RenderDebug.h"

void testIslandGeneration(MeshSystemHelper * ms)
{
  SEND_TEXT_MESSAGE(0,"Demonstrates how to locate and remove T-junctions from a mesh\r\n");

   if ( ms )
   {
        MeshSystemRaw *mr = ms->getMeshSystemRaw();
        if ( mr && mr->mVcount )
        {
			gRenderDebug->Reset();
			MeshIslandGeneration *mig = createMeshIslandGeneration();
			size_t icount = mig->islandGenerate(mr->mTcount,mr->mIndices,mr->mVertices);
			SEND_TEXT_MESSAGE(0,"Found %d unique islands.\r\n", icount);
			for (size_t i=0; i<icount; i++)
			{
				size_t tcount;
				size_t *indices = mig->getIsland(i,tcount);
				SEND_TEXT_MESSAGE(0,"Island%d has %d triangles.\r\n", i+1, tcount );
				unsigned int color = gRenderDebug->getDebugColor();
				for (size_t i=0; i<tcount; i++)
				{
					size_t i1 = indices[i*3+0];
					size_t i2 = indices[i*3+1];
					size_t i3 = indices[i*3+2];

					const float *p1 = &mr->mVertices[i1*3];
					const float *p2 = &mr->mVertices[i2*3];
					const float *p3 = &mr->mVertices[i3*3];
					gRenderDebug->DebugTri(p1,p2,p3,color,6000);
				}
			}
			releaseMeshIslandGeneration(mig);
        }
   }
}

