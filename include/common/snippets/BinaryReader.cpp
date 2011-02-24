#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "BinaryReader.h"
#pragma warning(disable:4996)


/*!
**
** Copyright (c) 2009 by John W. Ratcliff mailto:jratcliffscarab@gmail.com
**
** The MIT license:
**
** Permission is hereby granted, MEMALLOC_FREE of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is furnished
** to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.

** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
** WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

namespace NVSHARE
{

class MyBinaryReader : public BinaryReader
{
public:
	MyBinaryReader(const void* mem, NxU32 size)
	{
		mMem = (const NxU8*)mem;
		mMemSize = size;
		mMemRead = 0;

		mError = 0;
	}

	virtual ~MyBinaryReader(void)
	{
		release();
	}

	void release(void)
	{
	}

	virtual bool ReadBytes(void *dst, NxU32 count)
	{
		const void *src = TryAdvance(count);
		if ( !src )
		{
			memset(dst, 0, count);
			return false;
		}

		memcpy(dst, src, count);
		return true;
	}

	virtual bool ReadByte(NxU8 &v)
	{
		const void *src = TryAdvance(1);
		if ( !src )
		{
			v = 0;
			return false;
		}

		v = *(NxU8*)src;
		return true;
	}

	virtual bool ReadInt16s(NxI16 *p, NxU32 count)
	{
		return Advance2Endian(p, count);
	}

	virtual bool ReadInt16(NxI16 &v)
	{
		return Advance2Endian(&v, 1);
	}

	virtual bool ReadUInt16s(NxU16 *p, NxU32 count)
	{
		return Advance2Endian(p, count);
	}

	virtual bool ReadUInt16(NxU16 &v)
	{
		return Advance2Endian(&v, 1);
	}

	virtual bool ReadInt32s(NxI32 *p, NxU32 count)
	{
		return Advance4Endian(p, count);
	}

	virtual bool ReadInt32(NxI32 &v)
	{
		return Advance4Endian(&v, 1);
	}

	virtual bool ReadUInt32s(NxU32 *p, NxU32 count)
	{
		return Advance4Endian(p, count);
	}

	virtual bool ReadUInt32(NxU32 &v)
	{
		return Advance4Endian(&v, 1);
	}

	virtual bool ReadFloats(NxF32 *p, NxU32 count)
	{
		return Advance4Endian(p, count);
	}

	virtual bool ReadFloat(NxF32 &v)
	{
		return Advance4Endian(&v, 1);
	}

	virtual bool ReadDoubles(NxF64 *p, NxU32 count)
	{
		return Advance8Endian(p, count);
	}

	virtual bool ReadDouble(NxF64 &v)
	{
		return Advance8Endian(&v, 1);
	}


	virtual void Read2Endian(void *dst, const void *src, NxU32 count) { memcpy(dst, src, 2*count); }
	virtual void Read4Endian(void *dst, const void *src, NxU32 count) { memcpy(dst, src, 4*count); }
	virtual void Read8Endian(void *dst, const void *src, NxU32 count) { memcpy(dst, src, 8*count); }


	const char * GetError(NxU32 &byteno)
	{
		const char *ret = mError;
		byteno = mByteNo;
		return ret;
	}

	bool	HasError() const
	{
		return mError != 0;
	}

	void	SetError(const char* err)
	{
		assert( err );
		if ( err )
		{
			mError = err;
			mByteNo = mMemRead;
		}
	}

	NxU32 GetMemSize() const
	{
		return mMemSize;
	}

	NxU32 GetRemaining() const
	{
		assert( mMemRead <= mMemSize );
		return mMemSize - mMemRead;
	}

	virtual NxU32			GetReadPos() const
	{
		return mMemRead;
	}

	virtual bool			SetReadPos( NxU32 pos )
	{
		if ( HasError() )
			return false;

		assert( pos <= mMemSize );
		if ( pos > mMemSize )
		{
			SetError( "Set read pos past end of buffer." );
			mMemRead = mMemSize;
			return false;
		}

		mMemRead = pos;
		return true;
	}

private:
	const void*	TryAdvance(NxU32 count)
	{
		const void* ret = 0;
		if ( !mError )
		{
			if ( mMemRead + count > mMemSize )
			{
				SetError( "Read buffer overrun" );
			}
			else
			{
				ret = (const void*)( mMem + mMemRead );
				mMemRead += count;
			}
		}
		return ret;
	}

	template< typename T >
	bool	Advance2Endian(T *dst, NxU32 count)
	{
		assert( sizeof( T ) == 2 );
		const void* src = TryAdvance( 2*count );
		if ( !src )
		{
			for ( NxU32 i = 0; i < count; ++i )
				dst[ i ] = 0;
			return false;
		}

		Read2Endian((void *)dst, src, count);
		return true;
	}

	template< typename T >
	bool	Advance4Endian(T *dst, NxU32 count)
	{
		assert( sizeof( T ) == 4 );
		const void* src = TryAdvance( 4*count );
		if ( !src )
		{
			for ( NxU32 i = 0; i < count; ++i )
				dst[ i ] = 0;
			return false;
		}

		Read4Endian((void *)dst, src, count);
		return true;
	}

	template< typename T >
	bool	Advance8Endian(T *dst, NxU32 count)
	{
		assert( sizeof( T ) == 8 );
		const void* src = TryAdvance( 8*count );
		if ( !src )
		{
			for ( NxU32 i = 0; i < count; ++i )
				dst[ i ] = 0;
			return false;
		}

		Read8Endian((void *)dst, src, count);
		return true;
	}

	const NxU8	*mMem;
	NxU32		mMemSize;
	NxU32		mMemRead;

	const char	*mError;
	NxU32		mByteNo;
};

class SwapEndianBinaryReader : public MyBinaryReader
{
public:
	SwapEndianBinaryReader(const void* mem, NxU32 size) : MyBinaryReader( mem, size )
	{
	}

	virtual ~SwapEndianBinaryReader()
	{
	}

	void Swap2(NxU8 *dst, const NxU8 *src)
	{
		dst[1] = src[0];
		dst[0] = src[1];
	}

	void Swap4(NxU8 *dst, const NxU8 *src)
	{
		dst[3] = src[0];
		dst[2] = src[1];
		dst[1] = src[2];
		dst[0] = src[3];
	}

	void Swap8(NxU8 *dst, const NxU8 *src)
	{
		dst[7] = src[0];
		dst[6] = src[1];
		dst[5] = src[2];
		dst[4] = src[3];
		dst[3] = src[4];
		dst[2] = src[5];
		dst[1] = src[6];
		dst[0] = src[7];
	}

	virtual void Read2Endian(void *pdst, const void *psrc, NxU32 count)
	{
		NxU16* dst = (NxU16*)pdst;
		const NxU16* src = (const NxU16*)psrc;

		for ( NxU32 i = 0; i < count; ++i )
			Swap2( (NxU8*)( dst + i ), (const NxU8*)( src + i ) );
	}

	virtual void Read4Endian(void *pdst, const void *psrc, NxU32 count)
	{
		NxU32* dst = (NxU32*)pdst;
		const NxU32* src = (const NxU32*)psrc;

		for ( NxU32 i = 0; i < count; ++i )
			Swap4( (NxU8*)( dst + i ), (const NxU8*)( src + i ) );
	}

	virtual void Read8Endian(void *pdst, const void *psrc, NxU32 count)
	{
		NxU64* dst = (NxU64*)pdst;
		const NxU64* src = (const NxU64*)psrc;

		for ( NxU32 i = 0; i < count; ++i )
			Swap8( (NxU8*)( dst + i ), (const NxU8*)( src + i ) );
	}
};

static bool IsLittleEndian()
{
	int i = 1;
	return *((char*)&i)!=0;
}

BinaryReader * createBinaryReader(const void* mem, NxU32 memSize, bool littleEndianFormat)
{
	MyBinaryReader *f;
	if ( IsLittleEndian() == littleEndianFormat )
		f = new MyBinaryReader(mem, memSize);
	else
		f = new SwapEndianBinaryReader(mem, memSize);

	return static_cast< BinaryReader *>(f);
}

void      releaseBinaryReader(BinaryReader *f)
{
	MyBinaryReader *m = static_cast< MyBinaryReader *>(f);
	delete m;
}

}; // end of namespace