#ifndef AG_SCREEN_PIPE_H

#define AG_SCREEN_PIPE_H

#define PIPESIZE 4096 // no more than 4096 screen quad's at a time.

class AgScreenPipeline;

class AgScreenVertex
{
public:

	void Set(HeF32 x,HeF32 y,HeF32 z,HeF32 tx,HeF32 ty,HeU32 color)
	{
		mPos[0]   = x;
		mPos[1]   = y;
		mPos[2]   = z;

		mTexel[0] = tx;
		mTexel[1] = ty;
    mTexel[2] = 0;

		mColor    = color;
	}

	HeF32        mPos[3];
	HeF32        mTexel[3];
	HeU32 mColor;
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

	void RenderScreenQuad(HeF32 x,HeF32 y,HeF32 z,HeF32 wid,HeF32 hit,HeF32 s0,HeF32 t0,HeF32 s1,HeF32 t1,HeU32 color)
	{
		if ( mFrameNo != gGlobalFrameNo )
			NewFrame(); // not inlined to keep main routine fast

		if ( mVcount == (PIPESIZE*4) ) Flush();

		AgScreenVertex *dest = &mVertices[mVcount];

		dest[0].Set(x,y,z,        s0,t0,color);
		dest[1].Set(x+wid,y,z,    s1,t0,color);
		dest[2].Set(x+wid,y+hit,z,s1,t1,color);
		dest[3].Set(x,y+hit,z,    s0,t1,color);

		mDest[0] = (HeU16)mVcount;
		mDest[1] = (HeU16)(mVcount+1);
		mDest[2] = (HeU16)(mVcount+2);
		mDest[3] = (HeU16)mVcount;
		mDest[4] = (HeU16)(mVcount+2);
		mDest[5] = (HeU16)(mVcount+3);

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
			 HeU32    mFrameNo; // the current frame number this pipe stream is active for.
			 AgScreenPipe   *mNext;
			 HeU32    mVcount;
			 AgScreenVertex *mVertices;
			 HeU16 *mDest;
			 HeU16 *mIndices;

static HeU32     gGlobalFrameNo;
static AgScreenPipe    *gHeadCurrent;
static AgScreenPipe    *gTailCurrent;

};


#endif
