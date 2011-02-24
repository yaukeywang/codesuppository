#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TestFileInterface.h"
#include "FileInterfaceExt.h"
using namespace FILESYS;
#include "SQLiteFS.h"
using namespace SQLITE_FS;

static SQLiteFS* sqfs;

void
testSqliteFS_frame()
{
	if ( sqfs )
		sqfs->pump();
}

void testSqliteFS(void)
{
	SEND_TEXT_MESSAGE(0,"Demonstrates how to use the SQLiteFS system, originally published on Feb 24, 2011\r\n");

	if ( !sqfs )
		sqfs = createSQLiteFS( "testSqliteFS.db", true );
	if ( sqfs )
	{
		class MyWriteFile : public IWriteFile
		{
		public:
			MyWriteFile( const char *path )
			{
				mFqn = fs_sanitizePath( path, true );
				mInfo.fqn = mFqn;
				mInfo.fqnSize = strlen( mInfo.fqn );
				mMem = 0;
			}

			virtual ~MyWriteFile()
			{
				release();
			}

			void release()
			{
				mInfo = SQLiteFS_file();
				MEMALLOC_FREE( mFqn );
				if ( mMem )
					MEMALLOC_FREE( mMem );
			}

			virtual void fetchFQN( SQLiteFS_file& info )
			{
				SEND_TEXT_MESSAGE(0,"Begin file write\r\n");
				info.fqn = mFqn;
				info.fqnSize = strlen( mFqn );
			}

			// WARNING: Must be threadsafe!  Executes in a different thread!
			virtual bool fetchFileData( SQLiteFS_file& info )
			{
				bool ret = false;

				if ( !mInfo.fileMem )
				{
					if ( fs_fileExists( mInfo.fqn ) )
					{
						if ( fs_fileTimestamp( mInfo.fqn, mInfo.timestamp ) )
						{
							assert( !mMem );
							mMem = fs_readEntireFile( mInfo.fqn, mInfo.fileSize );
							mInfo.fileMem = mMem;
							if ( mInfo.fileMem )
							{
								mInfo.crc32 = fs_memCalcCRC32( mInfo.fileMem, mInfo.fileSize );
								ret = true;
							}
						}
					}
				}

				if ( ret )
				{
					if ( mInfo.fqn && mInfo.fileMem )
					{
						info = mInfo;
						ret = true;
					}
				}
				return ret;
			}

			void onFilePut( const SQLiteFS_file& file, bool succeeded )
			{
				// bypass unreferenced params warning.
				(file);
				if ( succeeded )
					SEND_TEXT_MESSAGE(0,"File (%s) WRITE (%d) bytes: SUCCESS\r\n", file.fqn, file.fileSize);
				else
					SEND_TEXT_MESSAGE(0,"File (%s) WRITE (%d) bytes: FAIL\r\n", file.fqn, file.fileSize);
				delete this;
			}

			SQLiteFS_file		mInfo;
			char				*mFqn;
			void				*mMem;
		};

		class MyReadFile : public IReadFile
		{
		public:
			MyReadFile( const char *path )
			{
				mFqn = fs_sanitizePath( path, true );
			}

			virtual ~MyReadFile()
			{
				MEMALLOC_FREE( mFqn );
			}

			virtual void fetchFQN( SQLiteFS_file& info )
			{
				info.fqn = mFqn;
				info.fqnSize = strlen( mFqn );
				SEND_TEXT_MESSAGE(0,"Begin file read\r\n");
			}

			virtual void onFileRead( const SQLiteFS_file &file, bool succeeded )
			{
				// bypass unreferenced params warning.
				(file);
				if ( succeeded )
					SEND_TEXT_MESSAGE(0,"File (%s) READ (%d) bytes: SUCCESS\r\n", file.fqn, file.fileSize);
				else
					SEND_TEXT_MESSAGE(0,"File (%s) READ (%d) bytes: FAIL\r\n", file.fqn, file.fileSize);
				delete this;
			}

			char				*mFqn;
			void				*mMem;
		};

		const char* testFiles[] =
		{
			"MeshImport_x86",
			"MeshImportARM_x64",
			"MeshImportARM_x86",
			"MeshImportEzm_x86",
			"MeshImportFbx_x86",
			"MeshImportObj_x86",
			"MeshImportOgre_x86",
			"MeshImportPSK_x86",
			0
		};

		bool kWriteTest = true;
		if ( kWriteTest )
		{
			// Asynchronously read the files into memory, then put them into the database,
			// all in a background thread.
			for ( const char** curFile = testFiles;
				*curFile;
				++curFile )
			{
				char scratch[ 1024 ];
				strcpy( scratch, *curFile );

				strcat( scratch, ".dll" );
				sqfs->putFile( new MyWriteFile( scratch ) );

				strcat( scratch, ".pdb" );
				sqfs->putFile( new MyWriteFile( scratch ) );
			}
		}

		bool kReadTest = true;
		if ( kReadTest )
		{
			// Asynchronously read each of the files.
			for ( const char** curFile = testFiles;
				*curFile;
				++curFile )
			{
				char scratch[ 1024 ];
				strcpy( scratch, *curFile );

				strcat( scratch, ".dll" );
				sqfs->getFile( new MyReadFile( scratch ) );

				strcat( scratch, ".pdb" );
				sqfs->getFile( new MyReadFile( scratch ) );
			}
		}

		//releaseSQLiteFS( sqfs );
		//sqfs = 0;
	}
}

void testFileInterface(void)
{
	SEND_TEXT_MESSAGE(0,"Demonstrates how to use the FILE_INTERFACE_FS class, originally published on Feb 23, 2011\r\n");

	if ( !fs_open() )
	{
		assert( !"could not initialize FileInterfaceExt" );
		SEND_TEXT_MESSAGE(0,"Could not initialize FileInterfaceExt");
		return;
	}

	SEND_TEXT_MESSAGE(0, fs_getExeDir() );
	SEND_TEXT_MESSAGE(0, "\r\n" );
	SEND_TEXT_MESSAGE(0, fs_getUserDir() );
	SEND_TEXT_MESSAGE(0, "\r\n" );
	SEND_TEXT_MESSAGE(0, fs_getRootDir() );
	SEND_TEXT_MESSAGE(0, "\r\n" );

	{
		// result:		/~/bar/bug/baz/baq.exe
		char *tmp = fs_sanitizePath( "../~\\foo\\..///bar//bug/baz//baq.exe", true );
		SEND_TEXT_MESSAGE(0, tmp );
		SEND_TEXT_MESSAGE(0, "\r\n" );
		MEMALLOC_FREE( tmp );
	}

	{
		// result:		c:/Users/Shawn/Docuemnts/bar/
		char *tmp = fs_sanitizePath( "~\\foo\\..///bar//bug/.//..", true );
		SEND_TEXT_MESSAGE(0, tmp );
		SEND_TEXT_MESSAGE(0, "\r\n" );
		MEMALLOC_FREE( tmp );
	}

	{
		// result:		c:/googlecode/codesuppository/bin/win32/bar/bug/blah.exe
		char *tmp = fs_sanitizePath( ":\\foo\\..///bar//bug/.//blah.exe", true );
		SEND_TEXT_MESSAGE(0, tmp );
		SEND_TEXT_MESSAGE(0, "\r\n" );
		MEMALLOC_FREE( tmp );
	}

	{
		// result:		/media/kunas/some texture.tga
		char *tmp = fs_sanitizePath( "\\media\\kunas/ability_01/..\\some texture.tga", true );
		SEND_TEXT_MESSAGE(0, tmp );
		SEND_TEXT_MESSAGE(0, "\r\n" );
		MEMALLOC_FREE( tmp );
	}

	fs_setCurrentDir( "media\\kunas" );
	SEND_TEXT_MESSAGE(0, "Setting current dir to: " );
	SEND_TEXT_MESSAGE(0, fs_getCurrentDir() );
	SEND_TEXT_MESSAGE(0, "\r\n" );

	{
		// result:		/media/kunas/some texture.tga
		char *tmp = fs_sanitizePath( "ability_01/..\\some texture.tga", true );
		SEND_TEXT_MESSAGE(0, tmp );
		SEND_TEXT_MESSAGE(0, "\r\n" );
		MEMALLOC_FREE( tmp );
	}

	fs_setCurrentDir( "" );
	SEND_TEXT_MESSAGE(0, "Setting current dir to: " );
	SEND_TEXT_MESSAGE(0, fs_getCurrentDir() );
	SEND_TEXT_MESSAGE(0, "\r\n" );

	{
		// result:		/media/kunas/some texture.tga
		char *tmp = fs_sanitizePath( "media\\kunas/ability_01/..\\some texture.tga", true );
		SEND_TEXT_MESSAGE(0, tmp );
		SEND_TEXT_MESSAGE(0, "\r\n" );
		MEMALLOC_FREE( tmp );
	}

	{
		char *tmp = "test/test.txt";
		if ( fs_fileExists( tmp ) )
		{
#if 0
			NxU32 tsLow, tsHigh;
			if ( fs_fileTimestamp( tmp, &tsLow, &tsHigh ) )
#else
			NxU64 ts;
			if ( fs_fileTimestamp( tmp, ts ) )
#endif
			{
				SEND_TEXT_MESSAGE(0, "Got timestamp for file '" );
				SEND_TEXT_MESSAGE(0, tmp );
				SEND_TEXT_MESSAGE(0, "'\r\n" );

				NxU32 crc;
				if ( fs_fileCalcCRC32( tmp, crc ) )
				{
					SEND_TEXT_MESSAGE(0, "Got CRC32 for file '" );
					SEND_TEXT_MESSAGE(0, tmp );
					SEND_TEXT_MESSAGE(0, "'\r\n" );
				}
				else
				{
					SEND_TEXT_MESSAGE(0, "Failed to get CRC32 for file '" );
					SEND_TEXT_MESSAGE(0, tmp );
					SEND_TEXT_MESSAGE(0, "'\r\n" );
				}
			}
			else
			{
				SEND_TEXT_MESSAGE(0, "Failed to get timestamp for file '" );
				SEND_TEXT_MESSAGE(0, tmp );
				SEND_TEXT_MESSAGE(0, "'\r\n" );
			}
		}
	}

	fs_close();
}

