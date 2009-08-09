#include "TestVectorFont.h"
#include "VectorFont.h"
#include "RenderDebug.h"
#include "NxVec3.h"

class TestVectorFont : public VectorFontInterface
{
public:
	virtual void drawLine(float x1,float y1,float x2,float y2)
	{
		NxVec3 v1(x1,y1,0);
		NxVec3 v2(x2,y2,0);
		gRenderDebug->pushRenderState();
		gRenderDebug->setCurrentDisplayTime(60.0f);
		gRenderDebug->setCurrentColor(0xFFFF00,0xFFFFFF);
		gRenderDebug->DebugLine(&v1.x,&v2.x);
		gRenderDebug->popRenderState();
	}
};

void testVectorFont(void)
{
	TestVectorFont tvf;
	VectorFont *vf = createVectorFont(&tvf);
	vf->vprintf(0,1.0f,true,"This is a test of the emergency broadcasting system.");

	releaseVectorFont(vf);
}

