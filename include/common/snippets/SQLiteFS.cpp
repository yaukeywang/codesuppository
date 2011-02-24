#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "SQLiteFS.h"
#pragma warning(disable:4996)
#pragma warning(disable:4100)
#pragma warning(disable:4505) // unreferenced functions

#include "sqlite3.h"

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

#include "sqlite3.h"
#include "sqlite3async.h"
#include "ThreadConfig.h"
#include "FileInterfaceExt.h"
#include "JobSwarm.h"
using namespace FILESYS;

// determines whether sqlite uses the asynchronous write engine.
#define USE_ASYNC	0

namespace SQLITE_FS
{
class MySQLiteFSThreadInterface : public THREAD_CONFIG::ThreadInterface
{
public:
	MySQLiteFSThreadInterface()
	{
		mRunning = 0;
	}

	virtual void threadMain()
	{
		mRunning = 1;
		sqlite3async_run();
		mRunning = 0;
	}

	virtual bool halt()
	{
		int rc = sqlite3async_control( SQLITEASYNC_HALT, SQLITEASYNC_HALT_IDLE );
		if ( rc != SQLITE_OK )
			return false;

		while ( mRunning )
		{
			sqlite3_sleep( 20 );
		}

		return true;
	}

	volatile int		mRunning;
};
static MySQLiteFSThreadInterface*	gSqliteAsyncThreadInterface = 0;
static THREAD_CONFIG::Thread*		gSqliteAsyncThread = 0;
static int							gSqliteAsyncRefCount = 0;

namespace SQLITE
{
	static int findCol( sqlite3_stmt* stmt, int numCols, const char *name )
	{
		for ( int idx = 0; idx < numCols; idx++ )
		{
			int type = sqlite3_column_type( stmt, idx );
			type = type;

			const char* tmp = sqlite3_column_name( stmt, idx );
			if ( strcmp( name, tmp ) == 0 )
				return idx;
		}

		return -1;
	}

	static const SqU8* getBlob( sqlite3_stmt* stmt, int numCols, int blobIdx, size_t& outBlobSize )
	{
		assert( !( blobIdx < 0 || blobIdx > numCols-1 ) );
		if ( blobIdx < 0 || blobIdx > numCols-1 )
		{
			outBlobSize = 0;
			return 0;
		}

		int type = sqlite3_column_type( stmt, blobIdx );
		type = type;

		int sz = sqlite3_column_bytes( stmt, blobIdx );
		assert( sz >= 0 );
		if ( sz < 0 )
			sz = 0;
		outBlobSize = (size_t)sz;
		return (const SqU8*)sqlite3_column_blob( stmt, blobIdx );
	}

	static const SqU8* getBlob( sqlite3_stmt* stmt, int numCols, const char *blobName, size_t& outBlobSize )
	{
		int blobIdx = findCol( stmt, numCols, blobName );
		return getBlob( stmt, numCols, blobIdx, outBlobSize );
	}
}

class MySQLiteFS : public SQLiteFS
{
	enum EJobType
	{
		JT_GET_FILE,
		JT_PUT_FILE,
		JT_DEL_FILE,
	};

	class MySQLiteJob : public JOB_SWARM::JobSwarmInterface
	{
	public:
		MySQLiteJob(
			EJobType		type,
			MySQLiteFS		*fs,
			IReadFile		*reader,
			IWriteFile		*writer,
			sqlite3			*db,
			sqlite3_stmt	*stmt,
			const char		*fqn,
			size_t			fqnSize,
			MySQLiteJob		*prevJob )
		{
			mType = type;
			assert( mType == JT_GET_FILE || mType == JT_PUT_FILE || mType == JT_DEL_FILE );

			mFS = fs;
			mReader = reader;
			mWriter = writer;
			mDb = db;
			mStmt = stmt;

			assert( fqn );
			mInfo.fqnSize = fqnSize;
			mFqn = (char *)MEMALLOC_MALLOC( sizeof( char ) * fqnSize + 1 );
			memcpy( mFqn, fqn, fqnSize );
			mFqn[ mInfo.fqnSize ] = 0;
			mInfo.fqn = mFqn;

			mErr = false;
			mStarted = false;
			mFinished = false;
			mJob = 0;

			mPrevJob = prevJob;
		}

		~MySQLiteJob()
		{
			assert( mFinished );
			MEMALLOC_FREE( mFqn );
		}

		void start()
		{
			assert( !mJob );
			if ( !mJob )
			{
				mStarted = true;
				mJob = mFS->mJobCtx->createSwarmJob( this, 0, 0 );
			}
		}

		// not necessary.
#if 0
		void stop()
		{
			if ( mJob )
			{
				JOB_SWARM::SwarmJob *job = mJob;
				mJob = 0;
				mFS->mJobCtx->cancel( job );
			}
		}
#endif

		// RUNS IN ANOTHER THREAD!! MUST BE THREAD SAFE!
		virtual void job_process( void *userData, NxI32 userId )
		{
			if ( mErr )
				return;

			// Wait for the main application to fully process the previous job
			// before beginning execution of a new job.
			//
			// Here is a simple example of why this is is necessary.  Imagine
			// issuing an asynchronous read for file A followed immediately by file B.
			// This will cause two JT_GET_FILE jobs to be in the queue simultaneously.
			// Now the reason we must wait is because each JT_GET_FILE job shares
			// the same "sqlite3_stmt" query object.  That query object is responsible
			// for returning a pointer to the file data blob.
			//
			// Sequence of events:
			//
			//	= Main thread =
			//		App issues read request for file A
			//		"read file A" enters the job queue
			//		App issues read request for file B
			//		"read file B" enters the job queue
			//
			//	= Worker thread =
			//		Begin processing file A
			//		Finish processing file A; add to "pending notification" queue
			//		Begin processing file B 
			//		^----- this step is the reason we must wait.  If we don't,
			//			then when file B begins processing, it will reset the
			//			shared sqlite3_stmt, invalidating its pointers to file A's data.
			//		Finish processing file B; add to "pending notification" queue
			//		File B is pushed into the "pending notification queue"
			//
			//	= Main thread =
			//		App is notified of file A, processes it
			//		App is notified of file B, processes it
			//
			if ( mPrevJob )
			{
				while ( !mPrevJob->mFinished )
				{
					sqlite3_sleep( 50 );
				}
			}

			if ( mType == JT_GET_FILE )
			{
				getFile( mReader );
			}
			else if ( mType == JT_PUT_FILE )
			{
				putFile( mWriter );
			}
			else if ( mType == JT_DEL_FILE )
			{
				delFile();
			}
		}

		// runs in primary thread of the context
		virtual void job_onFinish( void *userData, NxI32 userId )
		{
			if ( mPrevJob )
			{
				assert( mPrevJob->mFinished );
				delete mPrevJob;
				mPrevJob = 0;
			}
			if ( mType == JT_GET_FILE )
			{
				mReader->onFileRead( mInfo, !mErr );
				mReader = 0;
			}
			else if ( mType == JT_PUT_FILE )
			{
				mWriter->onFilePut( mInfo, !mErr );
				mWriter = 0;
			}
			mFinished = true;
		}

		// runs in primary thread of the context
		virtual void job_onCancel( void *userData, NxI32 userId )
		{
			mErr = true;
			job_onFinish( userData, userId );
		}

		bool	getFile( IReadFile* fp )
		{
			sqlite3_reset( mStmt );
			if ( SQLITE_OK != sqlite3_bind_text( mStmt, 1, mInfo.fqn, mInfo.fqnSize, SQLITE_STATIC ) )
			{
				mErr = true;
				return false;
			}

			int rowIdx = -1;

			// Only process the first row.
			for ( int ret = sqlite3_step( mStmt );
				ret != SQLITE_DONE;
				ret = sqlite3_step( mStmt ) )
			{
				switch ( ret )
				{
				case SQLITE_DONE:
					assert( false );
					break;
				case SQLITE_ROW:
					{
						if ( rowIdx < 0 )
						{
							int numCols = sqlite3_column_count( mStmt );
							SqU32 chkSize = 0;
							for ( int i = 0; i < numCols; ++i )
							{
								switch ( i )
								{
									// timestamp
								case 0:	mInfo.timestamp = (SqU64)sqlite3_column_int64( mStmt, i ); break;

									// crc
								case 1: mInfo.crc32 = (SqU32)sqlite3_column_int( mStmt, i ); break;

									// size
								case 2: chkSize = (SqU32)sqlite3_column_int( mStmt, i ); break;

									// data
								case 3: mInfo.fileMem = (const void *)SQLITE::getBlob( mStmt, numCols, i, mInfo.fileSize ); break;
								}
							}
							assert( chkSize == mInfo.fileSize );
						}
					}
					break;
				default:
					{
						assert( ret != SQLITE_DONE );
						mErr = true;
					}
					return false;
				}
			}
			return !mErr;
		}

		bool	putFile( IWriteFile* fp )
		{
			bool ret = false;
			if ( fp )
			{
				// fetch the file data.
				if ( fp->fetchFileData( mInfo ) )
				{
					// insert the file data.
					ret = insertFile();
				}
			}
			return ret;
		}

		bool	insertFile()
		{
			assert( mType == JT_PUT_FILE );
			sqlite3_reset( mStmt );
			if ( !sqchk( sqlite3_bind_text( mStmt, 1, mInfo.fqn, mInfo.fqnSize, SQLITE_STATIC ) ) )
				return false;
			if ( !sqchk( sqlite3_bind_int64( mStmt, 2, (sqlite3_int64)mInfo.timestamp ) ) )
				return false;
			if ( !sqchk( sqlite3_bind_int( mStmt, 3, (int)mInfo.crc32 ) ) )
				return false;
			if ( !sqchk( sqlite3_bind_int( mStmt, 4, (int)mInfo.fileSize ) ) )
				return false;
			if ( !sqchk( sqlite3_bind_blob( mStmt, 5, mInfo.fileMem, (int)mInfo.fileSize, SQLITE_STATIC ) ) )
				return false;
			if ( !sqchk( sqlite3_step( mStmt ), SQLITE_DONE ) )
				return false;
			return true;
		}

		bool	delFile()
		{
			sqlite3_reset( mStmt );
			if ( !sqchk( sqlite3_bind_text( mStmt, 1, mInfo.fqn, mInfo.fqnSize, SQLITE_STATIC ) ) )
				return false;
			if ( !sqchk( sqlite3_step( mStmt ), SQLITE_DONE ) )
				return false;
			return false;
		}


		bool	sqchk( int rc, int expected = SQLITE_OK )
		{
			if ( rc != expected )
			{
				const char *errmsg = sqlite3_errmsg( mDb );
				fprintf( stderr, "sqlite3_prepare failed: %s\n", errmsg );
				assert( !"SQLite result failed" );
				return false;
			}
			return true;
		}

	private:
		MySQLiteFS		*mFS;
		JOB_SWARM::SwarmJob	*mJob;
		IReadFile		*mReader;
		IWriteFile		*mWriter;
		sqlite3			*mDb;
		sqlite3_stmt	*mStmt;
		MySQLiteJob		*mPrevJob;
		char			*mFqn;
		SQLiteFS_file	mInfo;
		bool			mErr;
		bool			mStarted;
		volatile bool	mFinished;
		EJobType		mType;
	};

public:
	MySQLiteFS( sqlite3* db, char *path )
	{
		mDb = db;
		mPath = path;
		mProcInsertFile = 0;
		mProcDeleteFile = 0;
		mProcReadFile = 0;
		mPrevJob = 0;

		// create a thread to handle database reads.
		mJobCtx = JOB_SWARM::createJobSwarmContext( 1 );
	}

	virtual ~MySQLiteFS(void)
	{
		JOB_SWARM::releaseJobSwarmContext( mJobCtx );
		mJobCtx = 0;
		freeSQL( mProcInsertFile );
		freeSQL( mProcDeleteFile );
		freeSQL( mProcReadFile );
		sqlite3_close( mDb );
		MEMALLOC_FREE( mPath );
	}

	bool init()
	{
		if ( !initSQL( mProcInsertFile, "INSERT OR REPLACE INTO fs_files VALUES(?,?,?,?,?)" ) )
			return false;
		if ( !initSQL( mProcDeleteFile, "DELETE from fs_files WHERE fqn=?" ) )
			return false;
		if ( !initSQL( mProcReadFile, "SELECT timestamp,crc,size,data FROM fs_files WHERE fqn=?" ) )
			return false;
		return true;
	}

	bool initSQL( sqlite3_stmt * &outStmt, char *SQL )
	{
		const char * outTail;
		if ( !outStmt )
		{
			if ( sqchk( sqlite3_prepare( mDb, SQL, (int)strlen( SQL ), &outStmt, &outTail ) ) )
				return true;
		}
		outStmt = 0;
		return false;
	}

	void freeSQL( sqlite3_stmt * &outStmt )
	{
		if ( outStmt )
		{
			sqlite3_finalize( outStmt );
			outStmt = 0;
		}
	}

	virtual void	delFile( const char *fqn, size_t fqnSize )
	{
		SQLiteFS_file info;
		info.fqn = fqn;
		info.fqnSize = fqnSize;

		MySQLiteJob* job = MEMALLOC_NEW( MySQLiteJob )( JT_DEL_FILE, this, NULL, NULL, mDb, mProcDeleteFile, info.fqn, info.fqnSize, mPrevJob );
		mPrevJob = job;
		job->start();
	}

	virtual void	putFile( IWriteFile* fp )
	{
		assert( fp );
		if ( fp )
		{
			SQLiteFS_file info;
			fp->fetchFQN( info );
			assert( info.fqn );
			if ( info.fqn )
			{
				MySQLiteJob* job = MEMALLOC_NEW( MySQLiteJob )( JT_PUT_FILE, this, NULL, fp, mDb, mProcInsertFile, info.fqn, info.fqnSize, mPrevJob );
				mPrevJob = job;
				job->start();
			}
		}
	}

	virtual void	getFile( IReadFile *fp )
	{
		assert( fp );
		if ( fp )
		{
			SQLiteFS_file info;
			fp->fetchFQN( info );
			assert( info.fqn );
			if ( info.fqn )
			{
				MySQLiteJob* job = MEMALLOC_NEW( MySQLiteJob )( JT_GET_FILE, this, fp, NULL, mDb, mProcReadFile, info.fqn, info.fqnSize, mPrevJob );
				mPrevJob = job;
				job->start();
			}
		}
	}

	virtual void	pump()
	{
		if ( mJobCtx )
			mJobCtx->processSwarmJobs();
	}

	// debug error checking.
	bool	sqchk( int rc, int expected = SQLITE_OK )
	{
		if ( rc != expected )
		{
			const char *errmsg = sqlite3_errmsg( mDb );
			fprintf( stderr, "sqlite3_prepare failed: %s\n", errmsg );
			assert( !"SQLite result failed" );
			return false;
		}
		return true;
	}

	JOB_SWARM::JobSwarmContext	*mJobCtx;

	MySQLiteJob		*mPrevJob;

	sqlite3_stmt	*mProcInsertFile;
	sqlite3_stmt	*mProcDeleteFile;
	sqlite3_stmt	*mProcReadFile;

	sqlite3			*mDb;
	char			*mPath;
};

static bool openAsyncSQLite3()
{
	if ( gSqliteAsyncRefCount == 0 )
	{
		if ( sqlite3async_initialize( 0, 0 ) != SQLITE_OK )
		{
			assert( !"could not initialize asynchronous SQLite3" );
			return false;
		}
		sqlite3async_control( SQLITEASYNC_HALT, SQLITEASYNC_HALT_NEVER );
		assert( !gSqliteAsyncThreadInterface && !gSqliteAsyncThread );
		gSqliteAsyncThreadInterface = MEMALLOC_NEW( MySQLiteFSThreadInterface );
		gSqliteAsyncThread = tc_createThread( gSqliteAsyncThreadInterface );
	}
	++gSqliteAsyncRefCount;
	return true;
}

static void closeAsyncSQLite3()
{
	if ( gSqliteAsyncRefCount > 0 )
	{
		--gSqliteAsyncRefCount;
		if ( gSqliteAsyncRefCount == 0 )
		{
			if ( !gSqliteAsyncThreadInterface->halt() )
			{
				++gSqliteAsyncRefCount;
			}
			else
			{
				tc_releaseThread( gSqliteAsyncThread );
				gSqliteAsyncThread = 0;

				delete gSqliteAsyncThreadInterface;
				gSqliteAsyncThreadInterface = 0;

				sqlite3async_shutdown();
			}
		}
	}
}

static int unnecessaryCallback( void *NotUsed, int argc, char **argv, char **azColName )
{
#if 0
	int i;
	for ( i=0; i < argc; i++ )
	{
		printf( "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL" );
	}
	printf("\n");
#endif
	return 0;
}

static bool justExecuteSQL( sqlite3 *db, const char *sqlStatement )
{
	bool ret = true;
	char *zErrMsg = 0;
	int rc = sqlite3_exec( db, sqlStatement, unnecessaryCallback, 0, &zErrMsg );
	if ( rc != SQLITE_OK )
	{
		sqlite3_free( zErrMsg );
		zErrMsg = 0;
		ret = false;
	}
	return ret;
}


static void recreateSQLiteFS( sqlite3 *db )
{
	bool succeeded;
	// destroy any existing table.
	succeeded = justExecuteSQL( db, "DROP TABLE fs_files;" );

	// create the table.
	{
		char query[ 4096 ];
		strcpy( query, "CREATE TABLE fs_files( ");
		strcat( query, "fqn         VARCHAR(512) UNIQUE, " );
		strcat( query, "timestamp   INT8, " );
		strcat( query, "crc         INT, " );
		strcat( query, "size        INT, " );
		strcat( query, "data        BLOB); " );
		succeeded = justExecuteSQL( db, query );
		assert( succeeded );
	}
	
	// create an index for the fqn column (for efficient lookups).
	succeeded = justExecuteSQL( db, "CREATE INDEX fqn_index ON fs_files( fqn );" );
	assert( succeeded );
}


SQLiteFS * createSQLiteFS( const char *utf8path, bool createIfNew, bool wipeDatabaseAndRecreate )
{
	if ( !fs_open() )
	{
		assert( !"could not initialize FileInterfaceExt" );
		return 0;
	}

	if ( !openAsyncSQLite3() )
		return 0;

	char *path = fs_sanitizePath( utf8path, true );

	bool recreate = false;
	if ( createIfNew && !fs_fileExists( utf8path ) )
	{
		if ( fs_touchFile( utf8path ) )
			recreate = true;
		else
		{
			assert( !"could not create the SQLite database" );
			MEMALLOC_FREE( path );
			path = 0;
		}
	}

	sqlite3 *db = 0;
	if ( path )
	{
		int rc = sqlite3_open_v2(
			path[0] == '/' ? path+1 : path,
			&db,
			SQLITE_OPEN_READWRITE,
#if USE_ASYNC
			"sqlite3async"
#else
			"win32"
#endif
			);
		if ( rc )
		{
			fprintf( stderr, "Can't open database: %s\n", sqlite3_errmsg( db ) );
			sqlite3_close( db );
			db = 0;
			MEMALLOC_FREE( path );
			path = 0;
		}
	}

	if ( !db )
	{
		closeAsyncSQLite3();
		return 0;
	}

	// recreate the database schema, if necessary.
	if ( recreate || wipeDatabaseAndRecreate )
	{
		recreateSQLiteFS( db );
	}

	MySQLiteFS *f = new MySQLiteFS( db, path );
	if ( !f->init() )
	{
		assert( !"SQLiteFS init failed" );
		releaseSQLiteFS( f );
		f = 0;
	}
	return static_cast< SQLiteFS *>(f);
}


void      releaseSQLiteFS( SQLiteFS *f )
{
	if ( f )
	{
		MySQLiteFS *m = static_cast< MySQLiteFS *>(f);
		delete m;

		fs_close();

		closeAsyncSQLite3();
	}
}

}; // end of namespace