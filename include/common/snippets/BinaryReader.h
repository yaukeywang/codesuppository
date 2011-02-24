#ifndef SNIPPETS_BINARY_READER_H
#define SNIPPETS_BINARY_READER_H

#include <assert.h>

#include "UserMemAlloc.h"

/*!
**
** Copyright (c) 2009 by John W. Ratcliff mailto:jratcliff@infiniplex.net
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

// This code snippet provides a way to read data from a binary file in memory,
// switching from little-endian to big-endian as needed (and vice-versa).

namespace NVSHARE
{

class BinaryReader
{
public:
	// report the reason for a parsing error, and the byte number where it occurred.
	virtual const char *	GetError(NxU32 &byteno) = 0;
	virtual bool			HasError() const = 0;
	virtual void			SetError(const char* err) = 0;

	virtual NxU32			GetMemSize() const = 0;
	virtual NxU32			GetRemaining() const = 0;

	virtual NxU32			GetReadPos() const = 0;
	virtual bool			SetReadPos( NxU32 pos ) = 0;

	virtual bool	ReadBytes(void *dst, NxU32 count) = 0;
	bool			Read(void *dst, NxU32 count)		{ return ReadBytes(dst, count); }

	virtual bool	ReadByte(NxU8 &v) = 0;
	bool			Read(NxU8 &v)						{ return ReadByte(v); }

	virtual bool	ReadInt16s(NxI16 *p, NxU32 count) = 0;
	virtual bool	ReadInt16(NxI16 &v) = 0;
	bool			Read(NxI16 &v)						{ return ReadInt16(v); }

	virtual bool	ReadUInt16s(NxU16 *p, NxU32 count) = 0;
	virtual bool	ReadUInt16(NxU16 &v) = 0;
	bool			Read(NxU16 &v)						{ return ReadUInt16(v); }

	virtual bool	ReadInt32s(NxI32 *p, NxU32 count) = 0;
	virtual bool	ReadInt32(NxI32 &v) = 0;
	bool			Read(NxI32 &v)						{ return ReadInt32(v); }

	virtual bool	ReadUInt32s(NxU32 *p, NxU32 count) = 0;
	virtual bool	ReadUInt32(NxU32 &v) = 0;
	bool			Read(NxU32 &v)						{ return ReadUInt32(v); }

	virtual bool	ReadFloats(NxF32 *p, NxU32 count) = 0;
	virtual bool	ReadFloat(NxF32 &v) = 0;
	bool			Read(NxF32 &v)						{ return ReadFloat(v); }

	virtual bool	ReadDoubles(NxF64 *p, NxU32 count) = 0;
	virtual bool	ReadDouble(NxF64 &v) = 0;
	bool			Read(NxF64 &v)						{ return ReadDouble(v); }

	bool			ReadFloat3(NxF32 *p)	{ return ReadFloats(p, 3); }
	bool			ReadDouble3(NxF64 *p)	{ return ReadDoubles(p, 3); }

	template< typename T >
	BinaryReader&	operator >> (T& v)		{ Read(v); return *this; }
};

BinaryReader * createBinaryReader(const void* mem, NxU32 memSize, bool littleEndianFormat = true);
void      releaseBinaryReader(BinaryReader *p);

}; // end of namespace

#endif
