#ifndef AG_SCREEN_PIPE_H

#define AG_SCREEN_PIPE_H

#include "UserMemAlloc.h"

#define PIPESIZE 4096 // no more than 4096 screen quad's at a time.

class AgScreenPipeline;

class AgScreenVertex
{
public:

	void Set(NxF32 x,NxF32 y,NxF32 z,NxF32 tx,NxF32 ty,NxU32 color)
	{
		mPos[0]   = x;
		mPos[1]   = y;
		mPos[2]   = z;

		mTexel[0] = tx;
		mTexel[1] = ty;
    mTexel[2] = 0;

		mColor    = color;
	}

	NxF32        mPos[3];
	NxF32        mTexel[3];
	NxU32 mColor;
};

class AgScreenPipe
{
public:
	AgScreenPipe(void)
	{
		mFrameNo = 0xFFFFFFFF;
		mNext        = 0;
		mVcount      = 0;
		mVertices    = 0;
		mDest        = 0;
		mIndices     = 0;
	}

	~AgScreenPipe(void);

	void NewFrame(void);

	void RenderScreenQuad(NxF32 x,NxF32 y,NxF32 z,NxF32 wid,NxF32 hit,NxF32 s0,NxF32 t0,NxF32 s1,NxF32 t1,NxU32 color)
	{
		if ( mFrameNo != gGlobalFrameNo )
			NewFrame(); // not inlined to keep main routine fast

		if ( mVcount == (PIPESIZE*4) ) Flush();

		AgScreenVertex *dest = &mVertices[mVcount];

		dest[0].Set(x,y,z,        s0,t0,color);
		dest[1].Set(x+wid,y,z,    s1,t0,color);
		dest[2].Set(x+wid,y+hit,z,s1,t1,color);
		dest[3].Set(x,y+hit,z,    s0,t1,color);

		mDest[0] = (NxU16)mVcount;
		mDest[1] = (NxU16)(mVcount+1);
		mDest[2] = (NxU16)(mVcount+2);
		mDest[3] = (NxU16)mVcount;
		mDest[4] = (NxU16)(mVcount+2);
		mDest[5] = (NxU16)(mVcount+3);

		mVcount+=4;
		mDest+=6;

	}

	virtual void Flush(void);

	static void FlushCurrent(void)
	{
		AgScreenPipe *p = gHeadCurrent;
		while ( p )
		{
			p->Flush();
			p = p->mNext;
		}
		gHeadCurrent = 0;
		gGlobalFrameNo++;
	}

protected:
			 NxU32    mFrameNo; // the current frame number this pipe stream is active for.
			 AgScreenPipe   *mNext;
			 NxU32    mVcount;
			 AgScreenVertex *mVertices;
			 NxU16 *mDest;
			 NxU16 *mIndices;

static NxU32     gGlobalFrameNo;
static AgScreenPipe    *gHeadCurrent;
static AgScreenPipe    *gTailCurrent;

};


#endif
