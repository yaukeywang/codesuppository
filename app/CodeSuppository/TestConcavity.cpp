#include "TestConcavity.h"
#include "RenderDebug.h"
#include "shared/MeshSystem/MeshSystemHelper.h"
#include "StanHull.h"
#include "convexdecomposition/NvConvexDecomposition.h"

using namespace NVSHARE;

void testConcavity(MeshSystemHelper * ms)
{

	static	CONVEX_DECOMPOSITION::iConvexDecomposition *ic = 0;

	if ( ic )
	{
		if ( ic->isComputeComplete() )
		{
			gRenderDebug->reset();
        	gRenderDebug->pushRenderState();
    		gRenderDebug->setCurrentDisplayTime(150.0f);
    		gRenderDebug->addToCurrentState(NVSHARE::DebugRenderState::SolidWireShaded);
    		NxU32 hullCount = ic->getHullCount();
			SEND_TEXT_MESSAGE(0,"Convex Decomposition produced %d hulls.\r\n", hullCount );
    		for (NxU32 i=0; i<hullCount; i++)
    		{
    			NxU32 color = gRenderDebug->getDebugColor();
    			gRenderDebug->setCurrentColor(color,0xFFFFFF);

    			CONVEX_DECOMPOSITION::ConvexHullResult result;
    			bool ok = ic->getConvexHullResult(i,result);
				ok;
    			assert(ok);
    			for (NxU32 j=0; j<result.mTcount; j++)
    			{
    				NxU32 i1 = result.mIndices[j*3+0];
    				NxU32 i2 = result.mIndices[j*3+1];
    				NxU32 i3 = result.mIndices[j*3+2];
    				const NxF32 *p1 = &result.mVertices[i1*3];
    				const NxF32 *p2 = &result.mVertices[i2*3];
    				const NxF32 *p3 = &result.mVertices[i3*3];
    				gRenderDebug->DebugTri(p1,p2,p3);
    			}
    		}

    		CONVEX_DECOMPOSITION::releaseConvexDecomposition(ic);

        	gRenderDebug->popRenderState();
			ic = 0;
		}
		else
		{
			SEND_TEXT_MESSAGE(0,"Still computing previous convex decomposition\r\n");
		}
	}
	else
	{
    	MeshSystemRaw *mr = ms ? ms->getMeshSystemRaw() : 0;
        if ( mr && mr->mVcount )
        {

    		ic = CONVEX_DECOMPOSITION::createConvexDecomposition();

    		for (NxU32 i=0; i<mr->mTcount; i++)
    		{
    			NxU32 i1 = mr->mIndices[i*3+0];
    			NxU32 i2 = mr->mIndices[i*3+1];
    			NxU32 i3 = mr->mIndices[i*3+2];
    			const NxF32 *p1 = &mr->mVertices[i1*3];
    			const NxF32 *p2 = &mr->mVertices[i2*3];
    			const NxF32 *p3 = &mr->mVertices[i3*3];
    			ic->addTriangle(p1,p2,p3);
    		}
			SEND_TEXT_MESSAGE(0,"Computing Convex Decomposition of mesh with %d triangles.\r\n", mr->mTcount );
    		ic->computeConvexDecomposition();
    	}
    	ms->releaseMeshSystemRaw(mr);
    }

}

