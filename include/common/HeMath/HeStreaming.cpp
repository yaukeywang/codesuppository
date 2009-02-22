#include <stdio.h>

#include "HeStreaming.h"
#include "../snippets/UserMemAlloc.h"

#pragma warning(disable:4189)
#pragma warning(disable:4996)

HeUserStream::HeUserStream(const char	*filename, bool	load): fp(NULL)
{
	fp = fopen(filename, load	?	"rb" : "wb");
}

HeUserStream::~HeUserStream()
{
	if (fp)
	{
		fclose(fp);
	}
}

// Loading API
HeU8 HeUserStream::readByte()const
{
	HeU8 b;
	size_t r = fread(&b, sizeof(HeU8), 1,	fp);
	HE_ASSERT(r);
	return b;
}

HeU16	HeUserStream::readWord()const
{
	HeU16	w;
	size_t r = fread(&w, sizeof(HeU16),	1, fp);
	HE_ASSERT(r);
	return w;
}

HeU32	HeUserStream::readDword()const
{
	HeU32	d;
	size_t r = fread(&d, sizeof(HeU32),	1, fp);
	HE_ASSERT(r);
	return d;
}

HeF32	HeUserStream::readFloat()const
{
	HeF32 f;
	size_t r = fread(&f, sizeof(HeF32), 1,	fp);
	HE_ASSERT(r);
	return f;
}

HeF64 HeUserStream::readDouble()const
{
	HeF64	f;
	size_t r = fread(&f, sizeof(HeF64),	1, fp);
	HE_ASSERT(r);
	return f;
}

void HeUserStream::readBuffer(void *buffer,	HeU32	size)const
{
	size_t w = fread(buffer, size, 1,	fp);
	HE_ASSERT(w);
}

// Saving	API
HeStream &HeUserStream::storeByte(HeU8 b)
{
	size_t w = fwrite(&b,	sizeof(HeU8),	1, fp);
	HE_ASSERT(w);
	return	*this;
}

HeStream &HeUserStream::storeWord(HeU16	w)
{
	size_t ww	=	fwrite(&w, sizeof(HeU16),	1, fp);
	HE_ASSERT(ww);
	return	*this;
}

HeStream &HeUserStream::storeDword(HeU32 d)
{
	size_t w = fwrite(&d,	sizeof(HeU32), 1,	fp);
	HE_ASSERT(w);
	return	*this;
}

HeStream &HeUserStream::storeFloat(HeF32	f)
{
	size_t w = fwrite(&f,	sizeof(HeF32),	1, fp);
	HE_ASSERT(w);
	return	*this;
}

HeStream &HeUserStream::storeDouble(HeF64	f)
{
	size_t w = fwrite(&f,	sizeof(HeF64), 1,	fp);
	HE_ASSERT(w);
	return	*this;
}

HeStream &HeUserStream::storeBuffer(const	void *buffer,	HeU32	size)
{
	size_t w = fwrite(buffer,	size,	1, fp);
	HE_ASSERT(w);
	return	*this;
}




HeMemoryWriteBuffer::HeMemoryWriteBuffer():	currentSize(0),	maxSize(0),	data(NULL){}

HeMemoryWriteBuffer::~HeMemoryWriteBuffer()
{
	MEMALLOC_FREE(data);
}

void HeMemoryWriteBuffer::clear()
{
	currentSize	=	0;
}

HeStream &HeMemoryWriteBuffer::storeByte(HeU8	b)
{
	storeBuffer(&b,	sizeof(HeU8));
	return	*this;
}
HeStream &HeMemoryWriteBuffer::storeWord(HeU16 w)
{
	storeBuffer(&w,	sizeof(HeU16));
	return	*this;
}
HeStream &HeMemoryWriteBuffer::storeDword(HeU32	d)
{
	storeBuffer(&d,	sizeof(HeU32));
	return	*this;
}
HeStream &HeMemoryWriteBuffer::storeFloat(HeF32 f)
{
	storeBuffer(&f,	sizeof(HeF32));
	return	*this;
}
HeStream &HeMemoryWriteBuffer::storeDouble(HeF64 f)
{
	storeBuffer(&f,	sizeof(HeF64));
	return	*this;
}
HeStream &HeMemoryWriteBuffer::storeBuffer(const void	*buffer, HeU32 size)
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


HeMemoryReadBuffer::HeMemoryReadBuffer(const HeU8	*data):	buffer(data){}

HeMemoryReadBuffer::~HeMemoryReadBuffer()
{
	// We	don't	own	the	data =>	no delete
}

HeU8 HeMemoryReadBuffer::readByte()const
{
	HeU8 b;
	memcpy(&b, buffer, sizeof(HeU8));
	buffer +=	sizeof(HeU8);
	return b;
}

HeU16	HeMemoryReadBuffer::readWord()const
{
	HeU16	w;
	memcpy(&w, buffer, sizeof(HeU16));
	buffer +=	sizeof(HeU16);
	return w;
}

HeU32	HeMemoryReadBuffer::readDword()const
{
	HeU32	d;
	memcpy(&d, buffer, sizeof(HeU32));
	buffer +=	sizeof(HeU32);
	return d;
}

HeF32	HeMemoryReadBuffer::readFloat()const
{
	HeF32	f;
	memcpy(&f, buffer, sizeof(HeF32));
	buffer +=	sizeof(HeF32);
	return f;
}

HeF64 HeMemoryReadBuffer::readDouble()const
{
	HeF64 f;
	memcpy(&f, buffer, sizeof(HeF64));
	buffer +=	sizeof(HeF64);
	return f;
}

void HeMemoryReadBuffer::readBuffer(void *dest,	HeU32	size)const
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

