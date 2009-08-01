#include "TestVectorFont.h"
#include "common/VectorFont/VectorFont.h"
#include "RenderDebug/RenderDebug.h"

class TestVectorFont : public VectorFontInterface
{
public:
	virtual void drawLine(float x1,float y1,float x2,float y2)
	{
		HeVec3 v1(x1,y1,0);
		HeVec3 v2(x2,y2,0);
		gRenderDebug->DebugLine(&v1.x,&v2.x,0xFFFF00,60.0f);
	}
};

void testVectorFont(void)
{
	TestVectorFont tvf;
	VectorFont *vf = createVectorFont(&tvf);
	vf->vprintf(0,1.0f,true,"This is a test of the emergency broadcasting system.");

	releaseVectorFont(vf);
}

