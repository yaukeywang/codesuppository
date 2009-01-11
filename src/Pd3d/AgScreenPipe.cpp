#include "common/snippets/UserMemAlloc.h"
#include "Ag.h"
#include "AgScreenPipe.h"


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>


HeU32        AgScreenPipe::gGlobalFrameNo  = 0;
AgScreenPipe *      AgScreenPipe::gHeadCurrent    = 0;
AgScreenPipe *      AgScreenPipe::gTailCurrent    = 0;

void AgScreenPipe::NewFrame(void)
{
	mFrameNo = gGlobalFrameNo;

	if ( mVertices == 0 )
	{
		mVertices = (AgScreenVertex *)MEMALLOC_MALLOC(sizeof(AgScreenVertex)*PIPESIZE*4);
		mIndices  = (HeU16 *) MEMALLOC_MALLOC(sizeof(HeU16)*PIPESIZE*6);
	}

	mVcount = 0;
	mDest   = mIndices;

	// patch the render linked list for this frame!
	if ( gHeadCurrent )
	{
		assert( gTailCurrent );
		gTailCurrent->mNext = this; // add it to the tail!
		mNext = 0;
		gTailCurrent = this;
	}
	else
	{
		gHeadCurrent = this;
		gTailCurrent = this;
		mNext = 0;
	}

}

AgScreenPipe::~AgScreenPipe(void)
{
	MEMALLOC_FREE(mVertices);
	MEMALLOC_FREE(mIndices);
}


void AgScreenPipe::Flush(void)
{
	mVcount = 0;
	mDest   = mIndices;
}
