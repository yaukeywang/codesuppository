#ifndef SNIPPETS_SQLITE_FS_H
#define SNIPPETS_SQLITE_FS_H

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

// This code snippet provides a way to read and write files from an SQLite
// database asynchronously.

namespace SQLITE_FS
{
typedef NxU8	SqU8;
typedef NxU16	SqU16;
typedef NxU32	SqU32;
typedef NxU64	SqU64;

typedef NxI8	SqI8;
typedef NxI16	SqI16;
typedef NxI32	SqI32;
typedef NxI64	SqI64;

struct SQLiteFS_file
{
	SqU64			timestamp;
	SqU32			crc32;

	const char		*fqn;
	SqU32			fqnSize;

	const void		*fileMem;
	SqU32			fileSize;

	SQLiteFS_file()
	{
		timestamp = 0;
		crc32 = 0;
		fqn = 0;
		fqnSize = 0;
		fileMem = 0;
		fileSize = 0;
	}
};

class IReadFile
{
public:
	virtual ~IReadFile() {}

	// executes in the main application thread.
	// You must set the 'fqn' and 'fqnSize' member variables.
	virtual void	fetchFQN( SQLiteFS_file& info )=0;

	// executes in the main application thread.
	virtual void	onFileRead( const SQLiteFS_file &file, bool succeeded )=0;
};

class IWriteFile
{
public:
	virtual ~IWriteFile() {}

	// executes in the main application thread.
	// You must set the 'fqn' and 'fqnSize' member variables.
	virtual void	fetchFQN( SQLiteFS_file& info )=0;

	// WARNING: Must be threadsafe!  Executes in a different thread!
	// You must set the 'fileMem' and 'fileSize' member variables at a minimum.
	// Additionally you may specify the 'timestamp' and 'crc32' member variables.
	virtual bool	fetchFileData( SQLiteFS_file& info )=0;

	// executes in the main application thread.
	// Called after a file is finished being inserted into the database.
	virtual void	onFilePut( const SQLiteFS_file &file, bool succeeded )=0;
};

class SQLiteFS
{
public:
	// asynchronously inserts a file into the database.  Replaces any existing file.
	virtual void	putFile( IWriteFile* fp )=0;

	// asynchronously fetches a file from the database.
	virtual void	getFile( IReadFile *fp )=0;

	// asynchronously delete a file from the database.  Currently no callback
	// is supported.
	virtual void	delFile( const char *fqn, size_t fqnSize )=0;

	// call this once per frame.
	virtual void	pump()=0;
};

SQLiteFS * createSQLiteFS( const char *utf8path, bool createIfNew = false, bool wipeDatabaseAndRecreate = false );
SQLiteFS * openSQLiteFS( const char *utf8path );
void      releaseSQLiteFS( SQLiteFS *p );

}; // end of namespace

#endif
