#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <NxVec3.h>

#include "TestIslandGeneration.h"
#include "MeshImport.h"
using namespace NVSHARE;
#include "MeshIslandGeneration.h"
#include "shared/MeshSystem/MeshSystemHelper.h"
#include "RenderDebug.h"
#include "RaycastMesh.h"

#pragma warning(disable:4702)

void testIslandGeneration(MeshSystemHelper * ms)
{
  SEND_TEXT_MESSAGE(0,"Demonstrates how to locate and remove T-junctions from a mesh\r\n");

   if ( ms )
   {
        MeshSystemRaw *mr = ms->getMeshSystemRaw();
        if ( mr && mr->mVcount )
        {
			gRenderDebug->reset();
			gRenderDebug->pushRenderState();
			gRenderDebug->setCurrentDisplayTime(100);
			gRenderDebug->addToCurrentState(DebugRenderState::SolidWireShaded);

#if 1

			RaycastMesh *rm = createRaycastMesh(mr->mVcount,mr->mVertices,mr->mTcount,mr->mIndices);

			const float *bmin = rm->getBoundMin();
			const float *bmax = rm->getBoundMax();

			#define SCAN_SIZE 16
			gRenderDebug->setCurrentArrowSize(0.02f);

			for (unsigned int y=0; y<SCAN_SIZE; y++)
			{
				float fractionY = (float)y/SCAN_SIZE;
				float fy = (bmax[1]-bmin[1])*fractionY+bmin[1];
				for (unsigned int x=0; x<SCAN_SIZE; x++)
				{
					float fractionZ = (float)x/SCAN_SIZE;
					float fz = (bmax[2]-bmin[2])*fractionZ+bmin[2];

					NxVec3 from(-2,fy,fz);
					NxVec3 to(2,fy,fz);
					NxVec3 hitLocation;
					NxVec3 normal;
					NxF32 hitDistance;

					bool hit = rm->raycast(&from.x,&to.x,&hitLocation.x,&normal.x,&hitDistance);
					if ( hit )
					{
						gRenderDebug->debugRay(from,hitLocation);
						NxVec3 toPos = hitLocation+normal*0.1f;
						gRenderDebug->debugRay(hitLocation,toPos);
					}
					else
					{
//						gRenderDebug->debugRay(from,to);
					}
				}
			}

			rm->release();
#else

			MeshIslandGeneration *mig = createMeshIslandGeneration();
			size_t icount = mig->islandGenerate(mr->mTcount,mr->mIndices,mr->mVertices);
			SEND_TEXT_MESSAGE(0,"Found %d unique islands.\r\n", icount);
			for (size_t i=0; i<icount; i++)
			{
				size_t tcount;
				size_t *indices = mig->getIsland(i,tcount);
				SEND_TEXT_MESSAGE(0,"Island%d has %d triangles.\r\n", i+1, tcount );
				NxU32 color = gRenderDebug->getDebugColor();
				gRenderDebug->setCurrentColor(color);
				gRenderDebug->setCurrentDisplayTime(6000);
				for (size_t i=0; i<tcount; i++)
				{
					size_t i1 = indices[i*3+0];
					size_t i2 = indices[i*3+1];
					size_t i3 = indices[i*3+2];

					const NxF32 *p1 = &mr->mVertices[i1*3];
					const NxF32 *p2 = &mr->mVertices[i2*3];
					const NxF32 *p3 = &mr->mVertices[i3*3];
					gRenderDebug->DebugTri(p1,p2,p3);
				}
			}
			releaseMeshIslandGeneration(mig);
#endif

			gRenderDebug->popRenderState();
        }
   }
}

