#include "TestMarchingCubes.h"
#include <stdio.h>
#include "RenderDebug.h"

using namespace NVSHARE;

void testMarchingCubes(void)
{
	FILE *fph = fopen("../../media/points.txt","r");
	if ( fph )
	{
		gRenderDebug->pushRenderState();
		gRenderDebug->addToCurrentState(DebugRenderState::InfiniteLifeSpan);
		float a,b,c;
		const float PSCALE=100.0f;
		while ( fscanf(fph,"%f %f %f",&a,&b,&c) == 3 )
		{
			float point[3] = { a*PSCALE, b*PSCALE, c*PSCALE };
			gRenderDebug->DebugPoint(point,0.1f);
		}
		gRenderDebug->popRenderState();
		fclose(fph);
	}
}

