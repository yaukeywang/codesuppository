#include "PlaySpriteBuffer.h"
#include "RenderDebug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <vector>

class SpriteBuffer
{
public:
	SpriteBuffer(FILE *fph)
	{
		mCount = 0;
		mBuffer = NULL;
		fread(&mCount, sizeof(mCount),1,fph);
		if ( mCount > 0 )
		{
			mBuffer = (float *)::malloc(sizeof(float)*3*mCount);
			fread(mBuffer,sizeof(float)*3*mCount,1,fph);
		}
	}
	~SpriteBuffer(void)
	{
		::free(mBuffer);
	}

	void process(void)
	{
		for (int i=0; i<mCount; i++)
		{
			NVSHARE::gRenderDebug->DebugPoint(&mBuffer[i*3],0.05f);
		}
	}

	int		mCount;
	float	*mBuffer;
};

typedef std::vector< SpriteBuffer * > SpriteBufferVector;

class _PlaySpriteBuffer : public PlaySpriteBuffer
{
public:
	_PlaySpriteBuffer(void)
	{
		mFrame = 0;
		FILE *fph = fopen("CopySpriteBuffer.bin","rb");
		if ( fph )
		{
			for (;;)
			{
				SpriteBuffer *sb = new SpriteBuffer(fph);
				if ( sb->mCount == 0 )
				{
					delete sb;
					break;
				}
				else
				{
					mSprites.push_back(sb);
				}
			}
		}
	}

	virtual ~_PlaySpriteBuffer(void)
	{
		for (SpriteBufferVector::iterator i=mSprites.begin(); i!=mSprites.end(); ++i)
		{
			SpriteBuffer *sb = (*i);
			delete sb;
		}
	}


	virtual void process(void)
	{
		if ( mFrame < (int)mSprites.size() )
		{
			mSprites[mFrame]->process();
		}
	}

	virtual void setFrame(int frame)
	{
		if ( frame < (int)mSprites.size() )
		{
			mFrame = frame;
		}
	}

	virtual void release(void)
	{
		delete this;
	}

	int	mFrame;
	SpriteBufferVector	mSprites;
};


PlaySpriteBuffer *createPlaySpriteBuffer(void)
{
	_PlaySpriteBuffer *p = new _PlaySpriteBuffer;
	return static_cast< PlaySpriteBuffer *>(p);
}
