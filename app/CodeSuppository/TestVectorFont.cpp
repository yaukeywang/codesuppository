#include "TestVectorFont.h"
#include "common/VectorFont/VectorFont.h"
#include "RenderDebug/RenderDebug.h"

void testVectorFont(void)
{
	VectorFont *vf = createVectorFont();
	gRenderDebug->setRenderScale(20.0f);
	vf->vprintf("This is a test of the emergency broadcasting system 0123456789");
	gRenderDebug->setRenderScale(1.0f);

#if 0
	HeVec3 p1(0,0.018f,0);
	HeVec3 p2(0,-0.0135f,0);

	HeVec3 p3(-0.0105f,0.0180f,0);
	HeVec3 p4(0.0105f,0.0180f,0);

	gRenderDebug->DebugRay(&p1.x,&p2.x,0.001f,0xFFFFFF,0xFF0000,60.0f);
	gRenderDebug->DebugRay(&p3.x,&p4.x,0.001f,0xFFFF00,0xFF0000,60.0f);
#endif
	releaseVectorFont(vf);
}

