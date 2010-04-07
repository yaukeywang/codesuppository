#ifndef RENDER_PACKET_H

#define RENDER_PACKET_H

#include <stdlib.h>
#include <assert.h>

enum RenderPacketType
{
	RPT_INDEXED_TRIANGLE_LIST,
	RPT_LAST
};

class RenderPacket
{
public:
	RenderPacket(void)
	{
		mType = RPT_LAST;
		mDataLen = 0;
		mData = 0;
		mIndex = 0;
	}

	RenderPacket(const void *data,unsigned int dlen)
	{
		mType = RPT_LAST;
		mDataLen = dlen;
		mIndex   = 0;
		mData    = (char *)::malloc(dlen);
		memcpy(mData,data,dlen);
	}

	~RenderPacket(void)
	{
		::free(mData);
	}

	unsigned int slen(const char *c) const
	{
		unsigned int ret = 0;
		while ( *c++ ) ret++;
		return ret;
	}

	void store(const char *c)
	{
		unsigned int len = slen(c);
		store(len);
		write(c,len+1);
	}

	void store(const float *v,unsigned int count)
	{
		if ( v )
		{
			store(count);
			write(v,sizeof(float)*count);
		}
		else
		{
			unsigned int c=0;
			store(c);
		}
	}

	void store(const unsigned int *v,unsigned int count)
	{
		if ( v )
		{
			store(count);
			write(v,sizeof(unsigned)*count);
		}
		else
		{
			unsigned int c=0;
			store(c);
		}
	}

	void store(unsigned int v)
	{
		write(&v,sizeof(unsigned int));
	}

	void write(const void *mem,unsigned int len)
	{
		if ( (mIndex+len) > mDataLen )
		{
			if ( mDataLen == 0 )
				mDataLen = 32768;
			else
				mDataLen = mDataLen*2;
			if ( mDataLen < len )
			{
				mDataLen = len*2;
			}
			char *newData = (char *)::malloc(mDataLen);
			if ( mIndex )
			{
				memcpy(newData,mData,mIndex);
			}
			::free(mData);
			mData = newData;
		}
		char *data = &mData[mIndex];
		memcpy(data,mem,len);
		mIndex+=len;
	}

	unsigned int getUINT(void)
	{
		const unsigned int *ret = (const unsigned int *)&mData[mIndex];
		mIndex+=sizeof(unsigned int);
		assert(mIndex <= mDataLen );
		return *ret;
	}

	const float * getFloatPointer(void)
	{
		const float *ret = 0;
		unsigned int count = getUINT();
		if ( count > 0 )
		{
			ret = (const float *)&mData[mIndex];
			mIndex+=sizeof(float)*count;
			assert(mIndex <= mDataLen );
		}
		return ret;
	}

	const unsigned int * getUINTPointer(void)
	{
		const unsigned int *ret = 0;
		unsigned int count = getUINT();
		if ( count > 0 )
		{
			ret = (const unsigned int *)&mData[mIndex];
			mIndex+=sizeof(unsigned int)*count;
			assert(mIndex <= mDataLen );
		}
		return ret;
	}

	const char * getString(void)
	{
		const char *ret = 0;
		unsigned int size = getUINT();
		ret = &mData[mIndex];
		mIndex+=(size+1);
		assert(mIndex <= mDataLen );
		return ret;
	}

	RenderPacketType	mType;
	unsigned int		mIndex;
	unsigned int        mDataLen;
	char			   *mData;
};

class RenderPacketIndexedTriangleList : public RenderPacket
{
public:
	RenderPacketIndexedTriangleList(void)
	{
		mType = RPT_INDEXED_TRIANGLE_LIST;
	}

	RenderPacketIndexedTriangleList(const void *data,unsigned int dlen) : RenderPacket(data,dlen)
	{
		mType = (RenderPacketType)getUINT();
		assert(mType==RPT_INDEXED_TRIANGLE_LIST);
		mMaterial = getString();
		mTriCount     	= getUINT();
		mVertCount		= getUINT();
		mTransformCount = getUINT();
		mLocalToWorld44	= getFloatPointer();
		mPositions		= getFloatPointer();
		mNormals		= getFloatPointer();
		mColors			= getUINTPointer();
		mTangents		= getFloatPointer();
		mBiNormals		= getFloatPointer();
		mTexCoords0		= getFloatPointer();
		mTexCoords1		= getFloatPointer();
		mTexCoords2		= getFloatPointer();
		mTexCoords3		= getFloatPointer();
		mIndices		= getUINTPointer();
	}

	void renderIndexedTriangleMesh(const char *material,
		unsigned int transformCount,
		const float *localToWorld44,
		const float *positions,
		const float *normals,
		const unsigned int *colors,
		const float *tangents,
		const float *binormals,
		const float *texcoords0,
		const float *texcoords1,
		const float *texcoords2,
		const float *texcoords3,
		const unsigned int  *indices,
		unsigned int triCount,
		unsigned int vertCount)
	{
		store((unsigned int)mType);                // store the type field...
		store(material);
		store(triCount);
		store(vertCount);
		store(transformCount);
		store(localToWorld44,16*transformCount);
		store(positions,vertCount*3);
		store(normals,vertCount*3);
		store(colors,vertCount);
		store(tangents,vertCount*3);
		store(binormals,vertCount*3);
		store(texcoords0,vertCount*2);
		store(texcoords1,vertCount*2);
		store(texcoords2,vertCount*2);
		store(texcoords3,vertCount*2);
		store(indices,triCount*3);
	}

	const char *mMaterial;
	unsigned int mTransformCount;
	const float *mLocalToWorld44;
	const float *mPositions;
	const float *mNormals;
	const unsigned int *mColors;
	const float *mTangents;
	const float *mBiNormals;
	const float *mTexCoords0;
	const float *mTexCoords1;
	const float *mTexCoords2;
	const float *mTexCoords3;
	const unsigned int *mIndices;
	unsigned int mTriCount;
	unsigned int mVertCount;
};

#endif
