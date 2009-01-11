#ifndef	HE_STREAMING_H
#define	HE_STREAMING_H

#include "HeStream.h"
#include "../snippets/HeSimpleTypes.h"
#include <stdio.h>

class	UserStream:	public HeStream
{
	 public:
		UserStream(const char	*filename, bool	load);
		virtual	~UserStream();

		virtual	HeU8 readByte()const;
		virtual	HeU16	readWord()const;
		virtual	HeU32	readDword()const;
		virtual	HeF32	readFloat()const;
		virtual	HeF64 readDouble()const;
		virtual	void readBuffer(void *buffer,	HeU32	size)const;

		virtual	HeStream &storeByte(HeU8 b);
		virtual	HeStream &storeWord(HeU16	w);
		virtual	HeStream &storeDword(HeU32 d);
		virtual	HeStream &storeFloat(HeF32	f);
		virtual	HeStream &storeDouble(HeF64	f);
		virtual	HeStream &storeBuffer(const	void *buffer,	HeU32	size);

		FILE *fp;
};

class	MemoryWriteBuffer: public	HeStream
{
	 public:
		MemoryWriteBuffer();
		virtual	~MemoryWriteBuffer();
		void clear();

		virtual	HeU8 readByte()const
		{
			HE_ASSERT(0);
			return 0;
		}
		virtual	HeU16	readWord()const
		{
			HE_ASSERT(0);
			return 0;
		}
		virtual	HeU32	readDword()const
		{
			HE_ASSERT(0);
			return 0;
		}
		virtual	HeF32	readFloat()const
		{
			HE_ASSERT(0);
			return 0.0f;
		}
		virtual	HeF64 readDouble()const
		{
			HE_ASSERT(0);
			return 0.0;
		}
		virtual	void readBuffer(void * /*buffer*/,	HeU32	/*size*/)const
		{
			HE_ASSERT(0);
		}

		virtual	HeStream &storeByte(HeU8 b);
		virtual	HeStream &storeWord(HeU16	w);
		virtual	HeStream &storeDword(HeU32 d);
		virtual	HeStream &storeFloat(HeF32	f);
		virtual	HeStream &storeDouble(HeF64	f);
		virtual	HeStream &storeBuffer(const	void *buffer,	HeU32	size);

		HeU32	currentSize;
		HeU32	maxSize;
		HeU8 *data;
};

class	MemoryReadBuffer:	public HeStream
{
	 public:
		MemoryReadBuffer(const HeU8	*data);
		virtual	~MemoryReadBuffer();

		virtual	HeU8 readByte()const;
		virtual	HeU16	readWord()const;
		virtual	HeU32	readDword()const;
		virtual	HeF32	readFloat()const;
		virtual	HeF64 readDouble()const;
		virtual	void readBuffer(void *buffer,	HeU32	size)const;

		virtual	HeStream &storeByte(HeU8 /*b*/)
		{
			HE_ASSERT(0);
			return	*this;
		}
		virtual	HeStream &storeWord(HeU16	/*w*/)
		{
			HE_ASSERT(0);
			return	*this;
		}
		virtual	HeStream &storeDword(HeU32 /*d*/)
		{
			HE_ASSERT(0);
			return	*this;
		}
		virtual	HeStream &storeFloat(HeF32	/*f*/)
		{
			HE_ASSERT(0);
			return	*this;
		}
		virtual	HeStream &storeDouble(HeF64	/*f*/)
		{
			HE_ASSERT(0);
			return	*this;
		}
		virtual	HeStream &storeBuffer(const	void * /*buffer*/,	HeU32	/*size*/)
		{
			HE_ASSERT(0);
			return	*this;
		}

		mutable	const	HeU8 *buffer;
};

class	ImportMemoryWriteBuffer: public	HeStream
{
	public:
		ImportMemoryWriteBuffer();
		virtual	~ImportMemoryWriteBuffer();
		void clear();

		virtual	HeU8 readByte()const
		{
			HE_ASSERT(0);
			return 0;
		}
		virtual	HeU16	readWord()const
		{
			HE_ASSERT(0);
			return 0;
		}
		virtual	HeU32	readDword()const
		{
			HE_ASSERT(0);
			return 0;
		}
		virtual	HeF32	readFloat()const
		{
			HE_ASSERT(0);
			return 0.0f;
		}
		virtual	HeF64 readDouble()const
		{
			HE_ASSERT(0);
			return 0.0;
		}
		virtual	void readBuffer(void * /*buffer*/,	HeU32	/*size*/)const
		{
			HE_ASSERT(0);
		}

		virtual	HeStream &storeByte(HeU8 b);
		virtual	HeStream &storeWord(HeU16	w);
		virtual	HeStream &storeDword(HeU32 d);
		virtual	HeStream &storeFloat(HeF32	f);
		virtual	HeStream &storeDouble(HeF64	f);
		virtual	HeStream &storeBuffer(const	void *buffer,	HeU32	size);

		HeU32	currentSize;
		HeU32	maxSize;
		HeU8 *data;
};

class	ImportMemoryReadBuffer:	public HeStream
{
	public:
		ImportMemoryReadBuffer(const HeU8	*data);
		virtual	~ImportMemoryReadBuffer();

		virtual	HeU8 readByte()const;
		virtual	HeU16	readWord()const;
		virtual	HeU32	readDword()const;
		virtual	HeF32	readFloat()const;
		virtual	HeF64 readDouble()const;
		virtual	void readBuffer(void *buffer,	HeU32	size)const;

		virtual	HeStream &storeByte(HeU8 /*b*/)
		{
			HE_ASSERT(0);
			return	*this;
		}
		virtual	HeStream &storeWord(HeU16	/*w*/)
		{
			HE_ASSERT(0);
			return	*this;
		}
		virtual	HeStream &storeDword(HeU32 /*d*/)
		{
			HE_ASSERT(0);
			return	*this;
		}
		virtual	HeStream &storeFloat(HeF32	/*f*/)
		{
			HE_ASSERT(0);
			return	*this;
		}
		virtual	HeStream &storeDouble(HeF64	/*f*/)
		{
			HE_ASSERT(0);
			return	*this;
		}
		virtual	HeStream &storeBuffer(const	void * /*buffer*/,	HeU32	/*size*/)
		{
			HE_ASSERT(0);
			return	*this;
		}

		mutable	const	HeU8 *buffer;
};


#endif
