#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TestMeshConsolidation.h"
#include "MeshImport/MeshSystem.h"
#include "common/snippets/SendTextMessage.h"
#include "common/snippets/ConsolidateMesh.h"
#include "shared/MeshSystem/MeshSystemHelper.h"
#include "RenderDebug/RenderDebug.h"
#include "common/snippets/RemoveTjunctions.h"

void testMeshConsolidation(MeshSystemHelper * ms)
{
  SEND_TEXT_MESSAGE(0,"Demonstrates how to consolidate a mesh\r\n");

   if ( ms )
   {
 	    gRenderDebug->Reset();
        MeshSystemRaw *mr = ms->getMeshSystemRaw();
        if ( mr && mr->mVcount )
        {
          RemoveTjunctionsDesc desc;
          RemoveTjunctions *rt = createRemoveTjunctions();
          desc.mVcount = mr->mVcount;
          desc.mVerticesF = mr->mVertices;
          desc.mTcount    = mr->mTcount;
          desc.mIndices   = mr->mIndices;
          desc.mIds       = 0;
          size_t tcount = rt->removeTjunctions(desc);
		  SEND_TEXT_MESSAGE(0,"Input triangle count %d output triangle count %d.\r\n", mr->mTcount, tcount );
		  ConsolidateMesh *cm = createConsolidateMesh();
  		  for (HeU32 i=0; i<tcount; i++)
  		  {
  			HeU32 i1 = desc.mIndicesOut[i*3+0];
  			HeU32 i2 = desc.mIndicesOut[i*3+1];
  			HeU32 i3 = desc.mIndicesOut[i*3+2];
			if ( i1 != i2 && i1 != i3 && i2 != i3 )
			{
  				const HeF32 *p1 = &desc.mVerticesF[i1*3];
  				const HeF32 *p2 = &desc.mVerticesF[i2*3];
  				const HeF32 *p3 = &desc.mVerticesF[i3*3];
  				cm->addTriangle(p1,p2,p3);
			}
			else
			{
				SEND_TEXT_MESSAGE(0,"Skipped degenerate triangle.\r\n");
			}
  		  }
  		  ConsolidateMeshOutput results;
  		  cm->consolidateMesh(results);
  		  for (HeU32 i=0; i<results.mTcount; i++)
  		  {
  			HeU32 i1 = results.mIndices[i*3+0];
  			HeU32 i2 = results.mIndices[i*3+1];
  			HeU32 i3 = results.mIndices[i*3+2];
  			const HeF32 *p1 = &results.mVerticesFloat[i1*3];
  			const HeF32 *p2 = &results.mVerticesFloat[i2*3];
  			const HeF32 *p3 = &results.mVerticesFloat[i3*3];
  			gRenderDebug->DebugTri(p1,p2,p3,0xFFFF00,6000);
  		  }
  		  SEND_TEXT_MESSAGE(0,"Input TriCount: %d Output TriCount: %d\r\n", tcount, results.mTcount );
  		  releaseConsolidateMesh(cm);
          releaseRemoveTjunctions(rt);
        }
   }
}

