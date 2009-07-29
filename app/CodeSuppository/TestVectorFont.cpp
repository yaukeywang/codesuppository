#include "TestVectorFont.h"
#include "common/VectorFont/VectorFont.h"

void testVectorFont(void)
{
	VectorFont *vf = createVectorFont();
	releaseVectorFont(vf);
}

