#include <stdio.h>

#include "HeStreaming.h"
#include "../snippets/UserMemAlloc.h"

#pragma warning(disable:4189)
#pragma warning(disable:4996)

UserStream::UserStream(const char	*filename, bool	load): fp(NULL)
{
	fp = fopen(filename, load	?	"rb" : "wb");
}

UserStream::~UserStream()
{
	if (fp)
	{
		fclose(fp);
	}
}

// Loading API
HeU8 UserStream::readByte()const
{
	HeU8 b;
	size_t r = fread(&b, sizeof(HeU8), 1,	fp);
	HE_ASSERT(r);
	return b;
}

HeU16	UserStream::readWord()const
{
	HeU16	w;
	size_t r = fread(&w, sizeof(HeU16),	1, fp);
	HE_ASSERT(r);
	return w;
}

HeU32	UserStream::readDword()const
{
	HeU32	d;
	size_t r = fread(&d, sizeof(HeU32),	1, fp);
	HE_ASSERT(r);
	return d;
}

HeF32	UserStream::readFloat()const
{
	HeF32 f;
	size_t r = fread(&f, sizeof(HeF32), 1,	fp);
	HE_ASSERT(r);
	return f;
}

HeF64 UserStream::readDouble()const
{
	HeF64	f;
	size_t r = fread(&f, sizeof(HeF64),	1, fp);
	HE_ASSERT(r);
	return f;
}

void UserStream::readBuffer(void *buffer,	HeU32	size)const
{
	size_t w = fread(buffer, size, 1,	fp);
	HE_ASSERT(w);
}

// Saving	API
HeStream &UserStream::storeByte(HeU8 b)
{
	size_t w = fwrite(&b,	sizeof(HeU8),	1, fp);
	HE_ASSERT(w);
	return	*this;
}

HeStream &UserStream::storeWord(HeU16	w)
{
	size_t ww	=	fwrite(&w, sizeof(HeU16),	1, fp);
	HE_ASSERT(ww);
	return	*this;
}

HeStream &UserStream::storeDword(HeU32 d)
{
	size_t w = fwrite(&d,	sizeof(HeU32), 1,	fp);
	HE_ASSERT(w);
	return	*this;
}

HeStream &UserStream::storeFloat(HeF32	f)
{
	size_t w = fwrite(&f,	sizeof(HeF32),	1, fp);
	HE_ASSERT(w);
	return	*this;
}

HeStream &UserStream::storeDouble(HeF64	f)
{
	size_t w = fwrite(&f,	sizeof(HeF64), 1,	fp);
	HE_ASSERT(w);
	return	*this;
}

HeStream &UserStream::storeBuffer(const	void *buffer,	HeU32	size)
{
	size_t w = fwrite(buffer,	size,	1, fp);
	HE_ASSERT(w);
	return	*this;
}




MemoryWriteBuffer::MemoryWriteBuffer():	currentSize(0),	maxSize(0),	data(NULL){}

MemoryWriteBuffer::~MemoryWriteBuffer()
{
	MEMALLOC_FREE(data);
}

void MemoryWriteBuffer::clear()
{
	currentSize	=	0;
}

HeStream &MemoryWriteBuffer::storeByte(HeU8	b)
{
	storeBuffer(&b,	sizeof(HeU8));
	return	*this;
}
HeStream &MemoryWriteBuffer::storeWord(HeU16 w)
{
	storeBuffer(&w,	sizeof(HeU16));
	return	*this;
}
HeStream &MemoryWriteBuffer::storeDword(HeU32	d)
{
	storeBuffer(&d,	sizeof(HeU32));
	return	*this;
}
HeStream &MemoryWriteBuffer::storeFloat(HeF32 f)
{
	storeBuffer(&f,	sizeof(HeF32));
	return	*this;
}
HeStream &MemoryWriteBuffer::storeDouble(HeF64 f)
{
	storeBuffer(&f,	sizeof(HeF64));
	return	*this;
}
HeStream &MemoryWriteBuffer::storeBuffer(const void	*buffer, HeU32 size)
{
	HeU32	expectedSize = currentSize + size;
	if (expectedSize > maxSize)
	{
		maxSize	=	expectedSize + 4096;

		HeU8 *newData	=	(HeU8 *)MEMALLOC_MALLOC(sizeof(HeU8)*maxSize);
		HE_ASSERT(newData	!= NULL);

		if (data)
		{
			memcpy(newData,	data,	currentSize);
			MEMALLOC_FREE(data);
		}
		data = newData;
	}
	memcpy(data	+	currentSize, buffer, size);
	currentSize	+= size;
	return	*this;
}


MemoryReadBuffer::MemoryReadBuffer(const HeU8	*data):	buffer(data){}

MemoryReadBuffer::~MemoryReadBuffer()
{
	// We	don't	own	the	data =>	no delete
}

HeU8 MemoryReadBuffer::readByte()const
{
	HeU8 b;
	memcpy(&b, buffer, sizeof(HeU8));
	buffer +=	sizeof(HeU8);
	return b;
}

HeU16	MemoryReadBuffer::readWord()const
{
	HeU16	w;
	memcpy(&w, buffer, sizeof(HeU16));
	buffer +=	sizeof(HeU16);
	return w;
}

HeU32	MemoryReadBuffer::readDword()const
{
	HeU32	d;
	memcpy(&d, buffer, sizeof(HeU32));
	buffer +=	sizeof(HeU32);
	return d;
}

HeF32	MemoryReadBuffer::readFloat()const
{
	HeF32	f;
	memcpy(&f, buffer, sizeof(HeF32));
	buffer +=	sizeof(HeF32);
	return f;
}

HeF64 MemoryReadBuffer::readDouble()const
{
	HeF64 f;
	memcpy(&f, buffer, sizeof(HeF64));
	buffer +=	sizeof(HeF64);
	return f;
}

void MemoryReadBuffer::readBuffer(void *dest,	HeU32	size)const
{
	memcpy(dest, buffer, size);
	buffer +=	size;
}

ImportMemoryWriteBuffer::ImportMemoryWriteBuffer():	currentSize(0),	maxSize(0),	data(NULL){}

ImportMemoryWriteBuffer::~ImportMemoryWriteBuffer()
{
	if (data !=	NULL)
	{
		MEMALLOC_FREE(data);
		data = NULL;
	}
}

void ImportMemoryWriteBuffer::clear()
{
	currentSize	=	0;
}

HeStream &ImportMemoryWriteBuffer::storeByte(HeU8	b)
{
	storeBuffer(&b,	sizeof(HeU8));
	return	*this;
}

HeStream &ImportMemoryWriteBuffer::storeWord(HeU16 w)
{
	storeBuffer(&w,	sizeof(HeU16));
	return	*this;
}

HeStream &ImportMemoryWriteBuffer::storeDword(HeU32	d)
{
	storeBuffer(&d,	sizeof(HeU32));
	return	*this;
}

HeStream &ImportMemoryWriteBuffer::storeFloat(HeF32 f)
{
	storeBuffer(&f,	sizeof(HeF32));
	return	*this;
}

HeStream &ImportMemoryWriteBuffer::storeDouble(HeF64 f)
{
	storeBuffer(&f,	sizeof(HeF64));
	return	*this;
}

HeStream &ImportMemoryWriteBuffer::storeBuffer(const void	*buffer, HeU32 size)
{
	HeU32	expectedSize = currentSize + size;
	if (expectedSize > maxSize)
	{
		maxSize	=	expectedSize + 4096;
		HeU8 *newData	=	(HeU8 *) MEMALLOC_MALLOC(sizeof(HeU8)*maxSize);
		HE_ASSERT(newData	!= NULL);

		if (data)
		{
			memcpy(newData,	data,	currentSize);
			MEMALLOC_FREE(data);
		}
		data = newData;
	}
	memcpy(data	+	currentSize, buffer, size);
	currentSize	+= size;
	return	*this;
}


ImportMemoryReadBuffer::ImportMemoryReadBuffer(const HeU8	*data):	buffer(data){}

ImportMemoryReadBuffer::~ImportMemoryReadBuffer()
{
	// We	don't	own	the	data =>	no delete
}

HeU8 ImportMemoryReadBuffer::readByte()const
{
	HeU8 b;
	memcpy(&b, buffer, sizeof(HeU8));
	buffer +=	sizeof(HeU8);
	return b;
}

HeU16	ImportMemoryReadBuffer::readWord()const
{
	HeU16	w;
	memcpy(&w, buffer, sizeof(HeU16));
	buffer +=	sizeof(HeU16);
	return w;
}

HeU32	ImportMemoryReadBuffer::readDword()const
{
	HeU32	d;
	memcpy(&d, buffer, sizeof(HeU32));
	buffer +=	sizeof(HeU32);
	return d;
}

HeF32	ImportMemoryReadBuffer::readFloat()const
{
	HeF32	f;
	memcpy(&f, buffer, sizeof(HeF32));
	buffer +=	sizeof(HeF32);
	return f;
}

HeF64 ImportMemoryReadBuffer::readDouble()const
{
	HeF64 f;
	memcpy(&f, buffer, sizeof(HeF64));
	buffer +=	sizeof(HeF64);
	return f;
}

void ImportMemoryReadBuffer::readBuffer(void *dest,	HeU32	size)const
{
	memcpy(dest, buffer, size);
	buffer +=	size;
}

