#include "safestdio.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "UserMemAlloc.h"
#pragma warning(disable:4267)

/*!
**
** Copyright (c) 2007 by John W. Ratcliff mailto:jratcliff@infiniplex.net
**
** Portions of this source has been released with the PhysXViewer application, as well as
** Rocket, CreateDynamics, ODF, and as a number of sample code snippets.
**
** If you find this code useful or you are feeling particularily generous I would
** ask that you please go to http://www.amillionpixels.us and make a donation
** to Troy DeMolay.
**
** DeMolay is a youth group for young men between the ages of 12 and 21.
** It teaches strong moral principles, as well as leadership skills and
** public speaking.  The donations page uses the 'pay for pixels' paradigm
** where, in this case, a pixel is only a single penny.  Donations can be
** made for as small as $4 or as high as a $100 block.  Each person who donates
** will get a link to their own site as well as acknowledgement on the
** donations blog located here http://www.amillionpixels.blogspot.com/
**
** If you wish to contact me you can use the following methods:
**
** Skype Phone: 636-486-4040 (let it ring a long time while it goes through switches)
** Skype ID: jratcliff63367
** Yahoo: jratcliff63367
** AOL: jratcliff1961
** email: jratcliff@infiniplex.net
** Personal website: http://jratcliffscarab.blogspot.com
** Coding Website:   http://codesuppository.blogspot.com
** FundRaising Blog: http://amillionpixels.blogspot.com
** Fundraising site: http://www.amillionpixels.us
** New Temple Site:  http://newtemple.blogspot.com
**
**
** The MIT license:
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
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

#include "crc.h"


//********************************************************************************************
//***
//*** A wrapper interface for standard FILE IO services that provides support to read and
//** write 'files' to and from a buffer in memory.
//***
//********************************************************************************************


#include "FileInterfaceExt.h"

#ifdef WIN32
#include "IncludeWindows.h"
#include "utf.h"
#include <wchar.h>
#include <ShlObj.h>
#endif

#pragma warning(disable:4996) // Disabling stupid .NET deprecated warning.

#define DEFAULT_BUFFER_SIZE 8192
#define BUFFER_GROW_SIZE    1000000 // grow in 1 MB chunks

namespace FILESYS_S
{

#ifdef WIN32
// last forward or backward slash character, null if none found.
static const WCHAR *         lastSlash(const WCHAR *src) 
{
  const WCHAR *ret = 0;

  const WCHAR *dot = wcschr(src,L'\\');
  if  ( dot == 0 )
    dot = wcschr(src,L'/');
  while ( dot )
  {
    ret = dot;
    dot = wcschr(ret+1,L'\\');
    if ( dot == 0 )
      dot = wcschr(ret+1,L'/');
  }
  return ret;
}

//********************************************************************************************
//***
//*** Windows
//***
//********************************************************************************************
class _FILE_INTERFACE_FS : public NVSHARE::Memalloc
{
public:
	_FILE_INTERFACE_FS()
	{
		mExeDir = 0;
		mExeDirUTF8 = 0;
		mUserDir = 0;
		mUserDirUTF8 = 0;
		mRootDir = 0;
		mRootDirUTF8 = 0;

		mCurrentDir = wcsncpy_malloc( L"/", 1 );
		mCurrentDirLen = 1;
		mCurrentDirUTF8 = strncpy_malloc( "/", 1 );
		mCurrentDirUTF8Len = 1;
	}

	~_FILE_INTERFACE_FS(void)
	{
		release();
	}

	void	release()
	{
		if ( mExeDir )
		{
			MEMALLOC_FREE( mExeDir );
			mExeDir = 0;
		}
		mExeDirLen = 0;
		if ( mExeDirUTF8 )
		{
			MEMALLOC_FREE( mExeDirUTF8 );
			mExeDirUTF8 = 0;
		}
		mExeDirUTF8Len = 0;

		if ( mUserDir )
		{
			MEMALLOC_FREE( mUserDir );
			mUserDir = 0;
		}
		mUserDirLen = 0;
		if ( mUserDirUTF8 )
		{
			MEMALLOC_FREE( mUserDirUTF8 );
			mUserDirUTF8 = 0;
		}
		mUserDirUTF8Len = 0;

		if ( mRootDir )
		{
			MEMALLOC_FREE( mRootDir );
			mRootDir = 0;
		}
		mRootDirLen = 0;
		if ( mRootDirUTF8 )
		{
			MEMALLOC_FREE( mRootDirUTF8 );
			mRootDirUTF8 = 0;
		}
		mRootDirUTF8Len = 0;

		if ( mCurrentDir )
			MEMALLOC_FREE( mCurrentDir );

		if ( mCurrentDirUTF8 )
			MEMALLOC_FREE( mCurrentDirUTF8 );

		mCurrentDir = wcsncpy_malloc( L"/", 1 );
		mCurrentDirLen = 1;
		mCurrentDirUTF8 = strncpy_malloc( "/", 1 );
		mCurrentDirUTF8Len = 1;
	}

	bool	init()
	{
		release();

		// extract the exe directory.
		if ( !calcExeDir() )
		{
			release();
			return false;
		}

		// determine the current user dir.
		if ( !calcUserDir() )
		{
			release();
			return false;
		}

		// extract the current working dir.
		if ( !calcRootDir() )
		{
			release();
			return false;
		}

		return true;
	}

	bool	calcExeDir()
	{
		assert( !mExeDir && !mExeDirUTF8 );
		if ( mExeDir || mExeDirUTF8 )
			return false;

		// get the command line.
		LPWSTR cmdLine = ::GetCommandLineW();

		// skip over the opening quote, if any.
		if ( *cmdLine == L'"' )
			++cmdLine;

		// find the closing quote or space.
		const WCHAR* last = wcschr( cmdLine, L'"' );
		if ( !last )
		{
			last = wcschr( cmdLine, L' ' );
			if ( !last )
			{
				last = cmdLine + wcslen( cmdLine );
			}
		}
		assert( last );
		if ( !last )
			return false;

		// copy the first arg to a scratch buffer.
		WCHAR scratch[ 2048 ];
		wcsncpy_s( scratch, 2048, cmdLine, last - cmdLine );

		// find the last slash within the first arg.
		const WCHAR* slash = lastSlash( scratch );
		if ( !slash )
			return false;

		// copy up to and including the last slash.
		size_t len = slash - scratch + 1;
		mExeDir = sanitizePath( wcsncpy_malloc( cmdLine, len ), &len, false, true );
		mExeDirLen = len;
		mExeDirUTF8 = UTF_CONVERT::unicodeToUtf8( mExeDir );
		mExeDirUTF8Len = strlen( mExeDirUTF8 );

		return true;
	}

	bool	calcUserDir()
	{
		WCHAR userDir[ MAX_PATH + 2 ];
		if ( S_OK != SHGetFolderPath( NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, userDir ) )
			return false;

		size_t len = wcslen( userDir );
		if ( userDir[ len - 1 ] != L'\\' )
			userDir[ len++ ] = L'\\';

		mUserDir = sanitizePath( wcsncpy_malloc( userDir, len ), &len, false, true );
		mUserDirLen = len;
		mUserDirUTF8 = UTF_CONVERT::unicodeToUtf8( mUserDir );
		mUserDirUTF8Len = strlen( mUserDirUTF8 );
		return true;
	}

	bool	calcRootDir()
	{
		WCHAR workingDir[ MAX_PATH + 2 ];
		if ( !_wgetcwd( workingDir, MAX_PATH ) )
			return false;

		size_t len = wcslen( workingDir );
		if ( workingDir[ len - 1 ] != L'\\' )
			workingDir[ len++ ] = L'\\';

		mRootDir = sanitizePath( wcsncpy_malloc( workingDir, len ), &len, false, true );
		mRootDirLen = len;
		mRootDirUTF8 = UTF_CONVERT::unicodeToUtf8( mRootDir );
		mRootDirUTF8Len = strlen( mRootDirUTF8 );
		return true;
	}

	WCHAR *	wcsncpy_malloc( const WCHAR *str, size_t count )
	{
		WCHAR *ret = (WCHAR*)MEMALLOC_MALLOC( sizeof(WCHAR)*( count + 1 ) );
		wcsncpy( ret, str, count );
		ret[count] = 0;
		return ret;
	}

	char *	strncpy_malloc( const char *str, size_t count )
	{
		char *ret = (char*)MEMALLOC_MALLOC( sizeof(char)*( count + 1 ) );
		strncpy( ret, str, count );
		ret[count] = 0;
		return ret;
	}

	WCHAR * wcsncat_malloc( const WCHAR *srcA, size_t srcAlen, const WCHAR *srcB, size_t srcBlen )
	{
		size_t srcsize = srcAlen + srcBlen + 1;
		WCHAR *ret = (WCHAR *)MEMALLOC_MALLOC( sizeof(WCHAR)*( srcsize ) );
		wcsncpy_s( ret, srcsize, srcA, srcAlen );
		wcsncpy_s( ret + srcAlen, srcsize - srcAlen, srcB, srcBlen );
		return ret;
	}
	char * strncat_malloc( const char *srcA, size_t srcAlen, const char *srcB, size_t srcBlen )
	{
		size_t srcsize = srcAlen + srcBlen + 1;
		char *ret = (char *)MEMALLOC_MALLOC( sizeof(char)*( srcsize ) );
		strncpy_s( ret, srcsize, srcA, srcAlen );
		strncpy_s( ret + srcAlen, srcsize - srcAlen, srcB, srcBlen );
		return ret;
	}

	WCHAR*	sanitizePath( WCHAR* path, size_t *inPathLen, size_t *outPathLen, bool resolveToRoot )
	{
		size_t len;
		if ( inPathLen )
			len = *inPathLen;
		else
			len = wcslen( path );
		if ( len == 0 )
			return path;

		assert( len < 2048 );
		if ( len >= 2048 )
		{
			path[ 2047 ] = 0;
			len = 2047;
		}

		// lowercase the drive letter.
		if ( len >= 2 && path[1] == L':' )
			path[0] = (WCHAR)tolower( path[0] );

		// convert all \ to /
		for ( size_t i = 0; i < len; ++i )
		{
			if ( path[i] == L'\\' )
				path[i] = L'/';
		}

		// if the path does not start with a slash, then prepend the current
		// working directory.
		//
		// if the path begins with certain special characters, then those
		// characters are replaced with their associated values:
		//		~/foo/bar will resolve to the user directory, such as My Documents on Windows
		//		:/foo/bar will resolve to the exe directory
		if ( resolveToRoot )
		{
			if ( len >= 2 && path[1] == L'/' )
			{
				if ( path[0] == L'~' )
				{
					WCHAR *tmp = wcsncat_malloc( mUserDir, mUserDirLen, path + 2, len - 2 );
					MEMALLOC_FREE( path );
					path = tmp;
					len = mUserDirLen + len - 2;
				}
				else if ( path[0] == L':' )
				{
					WCHAR *tmp = wcsncat_malloc( mExeDir, mExeDirLen, path + 2, len - 2 );
					MEMALLOC_FREE( path );
					path = tmp;
					len = mExeDirLen + len - 2;
				}
			}
		}

		if ( outPathLen )
			*outPathLen = len;

		return path;
	}

	char*	sanitizeUTF8Path( char* path, size_t *inPathLen, size_t *outPathLen, bool resolveToRoot )
	{
		size_t len;
		if ( inPathLen )
			len = *inPathLen;
		else
			len = strlen( path );
		if ( len == 0 )
			return path;

		assert( len < 2048 );
		if ( len >= 2048 )
		{
			path[ 2047 ] = 0;
			len = 2047;
		}

		// lowercase the drive letter.
		if ( len >= 2 && path[1] == L':' )
			path[0] = (char)tolower( path[0] );

		// convert all \ to /
		for ( size_t i = 0; i < len; ++i )
		{
			if ( path[i] == '\\' )
				path[i] = '/';
		}

		// if the path does not start with a slash, then prepend the current
		// working directory.
		//
		// if the path begins with certain special characters, then those
		// characters are replaced with their associated values:
		//		~/foo/bar will resolve to the user directory, such as My Documents on Windows
		//		:/foo/bar will resolve to the exe directory
		if ( resolveToRoot )
		{
			// prepend current dir, unless at root, or unless this is an internet protocol path
			// (like http:// or ftp://)
			{
				if ( path[0] != '/' &&
					path[0] != '~' &&
					path[0] != ':' &&
					!strstr( path, "://" ) )
				{
					char *tmp = strncat_malloc( mCurrentDirUTF8, mCurrentDirUTF8Len, path, len );
					len = mCurrentDirUTF8Len + len;
					MEMALLOC_FREE( path );
					path = tmp;
				}
			}

			// resolve non-root first.
			{
				path = sanitizeUTF8Path( path, &len, &len, false );
			}

			char *tmp = 0;
			size_t tmpLen = 0;
			if ( len >= 2 && path[1] == '/' )
			{
				if ( path[0] == '~' )
				{
					tmp = strncat_malloc( mUserDirUTF8, mUserDirUTF8Len, path + 2, len - 2 );
					tmpLen = mUserDirUTF8Len + len - 2;
				}
				else if ( path[0] == ':' )
				{
					tmp = strncat_malloc( mExeDirUTF8, mExeDirUTF8Len, path + 2, len - 2 );
					tmpLen = mExeDirUTF8Len + len - 2;
				}
			}

			if ( tmp )
			{
				MEMALLOC_FREE( path );
				path = tmp;
				len = tmpLen;
			}
			
			if ( outPathLen )
				*outPathLen = len;
			return path;
		}

		// collapse consecutive slashes:  foo/bar//baz  =>  foo/bar/baz
		if ( len >= 2 )
		{
			char *at = path;
			char *last = path + len - 1;
			for ( ; at != last; )
			{
				if ( at[0] == '/' && at[1] == '/' )
				{
					memmove( at, at + 1, last - at );
					--len;
					path[ len ] = 0;
					last = path + len;
				}
				else
				{
					++at;
				}
			}
		}

		assert ( !resolveToRoot );
		if ( *path )
		{
			char scratch[ 2049 ];
			scratch[ 0 ] = '/';
			scratch[ 1 ] = 0;
			int leadingOffset = 1;
			if ( *path == '/' )
				leadingOffset = 0;

			char *dst = &scratch[1];
			size_t dstSize = 2048;

			char *at = path;
			char *pathEnd = path + len;

			while ( at && at < pathEnd )
			{
				assert( at < pathEnd );

				char *next = strchr( at, '/' );
				if ( !next )
					next = path + len;

				size_t segmentSize = next - at;
				bool copySegment = false;

				// collapse "../" without going above root.
				if ( segmentSize == 2 && at[0] == '.' && at[1] == '.' )
				{
					assert( dst > scratch );
					if ( dst == &scratch[1] )
						leadingOffset = 0;
					else
					{
						do
						{
							--dst;
							++dstSize;
							*dst = 0;
						}
						while ( dst[-1] != '/' );
					}
				}
				else
				{
					// handle "./" or ".../" or "..../" etc the same way: skip 'em.
					for ( char *i = at; at < next; ++at )
					{
						if ( *i != '.' )
						{
							copySegment = true;
							break;
						}
					}
				}

				if ( copySegment )
				{
					size_t cpySize = segmentSize;
					if ( *next )
						++cpySize;

					assert( dstSize >= cpySize );
					if ( strncpy_s( dst, dstSize, at, cpySize ) == 0 )
					{
						dst += cpySize;
						dstSize -= cpySize;
					}
				}
				at = next + 1;
			}
			char *src = scratch + leadingOffset;
			size_t srcSize = ( dst - src );
			if ( strncpy_s( path, len+1, src, srcSize ) == 0 )
			{
				len = srcSize;
			}
		}

		if ( outPathLen )
			*outPathLen = len;

		return path;
	}

	bool	isCleanPath( const char *path )
	{
		size_t len = strlen( path );
		len = len;
		// TODO
		return true;
	}

	void	setCurrentDir( const char *path )
	{
		size_t len = strlen( path ) + 1;
		char *tmp = strncat_malloc( path, len - 1, "/", 1 );
		tmp = sanitizeUTF8Path( tmp, &len, &len, true );

		if ( mCurrentDirUTF8 )
			MEMALLOC_FREE( mCurrentDirUTF8 );
		if ( mCurrentDir )
			MEMALLOC_FREE( mCurrentDir );

		mCurrentDirUTF8 = tmp;
		mCurrentDirUTF8Len = len;
		mCurrentDir = UTF_CONVERT::utf8ToUnicode( mCurrentDirUTF8 );
		mCurrentDirLen = wcslen( mCurrentDir );
	}

	bool	getFileTimestamp( const char *_path, NxU32 *outLow, NxU32 *outHigh )
	{
		if ( outLow )
			*outLow = 0;
		if ( outHigh )
			*outHigh = 0;

		HANDLE hFile = openWin32Path( _path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
		if ( hFile == INVALID_HANDLE_VALUE )
			return false;

		bool ret = false;

		// Retrieve the file times for the file.
		FILETIME ftCreate, ftAccess, ftWrite;
		if ( GetFileTime( hFile, &ftCreate, &ftAccess, &ftWrite ) )
		{
			if ( outLow )
				*outLow = ftWrite.dwLowDateTime;
			if ( outHigh )
				*outHigh = ftWrite.dwHighDateTime;
			ret = true;
		}

		CloseHandle( hFile );
		return ret;
	}

	bool	getFileTimestamp( const char *_path, NxU64& outTimestamp )
	{
		outTimestamp = 0;

		HANDLE hFile = openWin32Path( _path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
		if ( hFile == INVALID_HANDLE_VALUE )
			return false;

		bool ret = false;

		// Retrieve the file times for the file.
		FILETIME ftCreate, ftAccess, ftWrite;
		if ( GetFileTime( hFile, &ftCreate, &ftAccess, &ftWrite ) )
		{
			ULARGE_INTEGER ul;
			ul.LowPart =  ftWrite.dwLowDateTime;
			ul.HighPart =  ftWrite.dwHighDateTime;
			outTimestamp = ul.QuadPart;
			ret = true;
		}

		CloseHandle( hFile );
		return ret;
	}

	bool	calcFileCRC32( const char *_path, NxU32 &outCRC )
	{
		outCRC = 0;

		// TODO: optimize.
		NxU32 entireFileSize;
		void *entireFile = readEntireFile( _path, entireFileSize );
		if ( !entireFile )
			return false;

		outCRC = calcMemCRC32( (const NxU8*)entireFile, entireFileSize );

		MEMALLOC_FREE( entireFile );
		return true;
	}

	NxU32	calcMemCRC32( const NxU8 *mem, NxU32 memSize )
	{
		return computeCRC( mem, memSize );
	}

	void*	readEntireFile( const char *_path, NxU32 &outSize )
	{
		outSize = 0;

		HANDLE hFile = openWin32Path( _path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
		if ( hFile == INVALID_HANDLE_VALUE )
			return 0;

		DWORD loSize, hiSize;
		loSize = GetFileSize( hFile, &hiSize );

		void *ret = 0;
		if ( hiSize == 0 )
		{
			ret = MEMALLOC_MALLOC( loSize );

			DWORD bytesRead = 0;
			if ( !ReadFile( hFile, ret, loSize, &bytesRead, NULL ) || bytesRead	!= loSize )
			{
				MEMALLOC_FREE( ret );
				ret = 0;
			}
			else
			{
				outSize = loSize;
			}
		}

		CloseHandle( hFile );
		return ret;
	}

	bool	touchFile( const char *_path )
	{
		HANDLE hFile = openWin32Path( _path,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL );
		if ( hFile == INVALID_HANDLE_VALUE )
			return false;

		CloseHandle( hFile );
		return true;
	}

	bool	fileExists( const char *_path )
	{
		HANDLE hFile = openWin32Path( _path, 0, 0, NULL, OPEN_EXISTING, 0, NULL );
		if ( hFile == INVALID_HANDLE_VALUE )
			return false;

		CloseHandle( hFile );
		return true;
	}

	WCHAR *	calcWin32Path( WCHAR *path )
	{
		if ( *path && ( *path == '/' || *path == '\\' ) )
			++path;
		for ( WCHAR *at = path; *at; ++at )
		{
			if ( *at && *at == '/' )
				*at = '\\';
		}
		return path;
	}

	HANDLE	openWin32Path( 
		const char *_path,
		DWORD dwDesiredAccess,
		DWORD dwShareMode,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		DWORD dwCreationDisposition,
		DWORD dwFlagsAndAttributes,
		HANDLE hTemplateFile )
	{
		size_t len = strlen( _path );
		char *path = sanitizeUTF8Path( strncpy_malloc( _path, len ), &len, &len, true );
		WCHAR *wpath = UTF_CONVERT::utf8ToUnicode( path );

		HANDLE hFile = CreateFileW(
			calcWin32Path( wpath ),
			dwDesiredAccess,
			dwShareMode,
			lpSecurityAttributes,
			dwCreationDisposition,
			dwFlagsAndAttributes,
			hTemplateFile );

		MEMALLOC_FREE( wpath );
		MEMALLOC_FREE( path );

		return hFile;
	}

	// the directory of the exe.
	WCHAR			*mExeDir;
	size_t			mExeDirLen;
	char			*mExeDirUTF8;		
	size_t			mExeDirUTF8Len;

	// the user directory (such as "My Documents").
	WCHAR			*mUserDir;	
	size_t			mUserDirLen;
	char			*mUserDirUTF8;	
	size_t			mUserDirUTF8Len;

	// the root directory (application working directory).
	WCHAR			*mRootDir;	
	size_t			mRootDirLen;
	char			*mRootDirUTF8;	
	size_t			mRootDirUTF8Len;

	// the current directory (relative to root directory).
	WCHAR			*mCurrentDir;
	size_t			mCurrentDirLen;
	char			*mCurrentDirUTF8;
	size_t			mCurrentDirUTF8Len;
};
#else
//********************************************************************************************
//***
//*** Implement this platform!
//***
//********************************************************************************************
#error Implement support for this platform.
#endif

FILE_INTERFACE_FS * fss_open()
{
	_FILE_INTERFACE_FS *ret = 0;

	ret = MEMALLOC_NEW(_FILE_INTERFACE_FS)();
	assert( ret );
	if ( !ret || !ret->init() )
	{
		delete ret;
		ret = 0;
	}

	return (FILE_INTERFACE_FS *)ret;
}

void  fss_close(FILE_INTERFACE_FS *_fsh)
{
	if ( _fsh )
	{
		_FILE_INTERFACE_FS *fsh = (_FILE_INTERFACE_FS *)_fsh;
		delete fsh;
	}
}

const char * fss_getExeDir( FILE_INTERFACE_FS *_fsh )
{
	assert( _fsh );
	_FILE_INTERFACE_FS *fsh = (_FILE_INTERFACE_FS *)_fsh;

	return fsh->mExeDirUTF8;
}

const char * fss_getRootDir( FILE_INTERFACE_FS *_fsh )
{
	assert( _fsh );
	_FILE_INTERFACE_FS *fsh = (_FILE_INTERFACE_FS *)_fsh;

	return fsh->mRootDirUTF8;
}

const char * fss_getUserDir( FILE_INTERFACE_FS *_fsh )
{
	assert( _fsh );
	_FILE_INTERFACE_FS *fsh = (_FILE_INTERFACE_FS *)_fsh;

	return fsh->mUserDirUTF8;
}

const char * fss_getCurrentDir( FILE_INTERFACE_FS *_fsh )
{
	assert( _fsh );
	_FILE_INTERFACE_FS *fsh = (_FILE_INTERFACE_FS *)_fsh;

	return fsh->mCurrentDirUTF8;
}

void fss_setCurrentDir( FILE_INTERFACE_FS *_fsh, const char *path )
{
	assert( _fsh );
	_FILE_INTERFACE_FS *fsh = (_FILE_INTERFACE_FS *)_fsh;

	fsh->setCurrentDir( path );
}

char * fss_sanitizePath( FILE_INTERFACE_FS *_fsh, const char *path, bool resolveToRoot )
{
	assert( _fsh );
	_FILE_INTERFACE_FS *fsh = (_FILE_INTERFACE_FS *)_fsh;

	size_t pathlen = strlen( path );
	char *ret = (char *)MEMALLOC_MALLOC( pathlen + 1 );
	ret[ pathlen ] = 0;
	strncpy_s( ret, pathlen + 1, path, pathlen );

	return fsh->sanitizeUTF8Path( ret, &pathlen, NULL, resolveToRoot );
}

bool fss_fileTimestamp( FILE_INTERFACE_FS *_fsh, const char *path,
							NxU32 *outLowDateTime, NxU32 *outHighDateTime )
{
	assert( _fsh );
	_FILE_INTERFACE_FS *fsh = (_FILE_INTERFACE_FS *)_fsh;

	return fsh->getFileTimestamp( path, outLowDateTime, outHighDateTime );
}

bool fss_fileTimestamp( FILE_INTERFACE_FS *_fsh, const char *path, NxU64& outTimestamp )
{
	assert( _fsh );
	_FILE_INTERFACE_FS *fsh = (_FILE_INTERFACE_FS *)_fsh;

	bool ret = false;
	if ( fsh->getFileTimestamp( path, outTimestamp ) )
	{
		assert( (NxI64)outTimestamp >= 0 );
		if ( (NxI64)outTimestamp >= 0 )
		{
			ret = true;
		}
		else
		{
			outTimestamp = 0;
		}
	}
	return ret;
}

bool fss_fileCalcCRC32( FILE_INTERFACE_FS *_fsh, const char *path, NxU32 &outCRC32 )
{
	assert( _fsh );
	_FILE_INTERFACE_FS *fsh = (_FILE_INTERFACE_FS *)_fsh;

	return fsh->calcFileCRC32( path, outCRC32 );
}

NxU32 fss_memCalcCRC32( FILE_INTERFACE_FS *_fsh, const void *mem, NxU32 memSize )
{
	assert( _fsh );
	_FILE_INTERFACE_FS *fsh = (_FILE_INTERFACE_FS *)_fsh;

	return fsh->calcMemCRC32( (const NxU8*)mem, memSize );
}

void * fss_readEntireFile( FILE_INTERFACE_FS *_fsh, const char *path, NxU32 &outFileSize )
{
	assert( _fsh );
	_FILE_INTERFACE_FS *fsh = (_FILE_INTERFACE_FS *)_fsh;

	return fsh->readEntireFile( path, outFileSize );
}

bool fss_touchFile( FILE_INTERFACE_FS *_fsh, const char *path )
{
	assert( _fsh );
	_FILE_INTERFACE_FS *fsh = (_FILE_INTERFACE_FS *)_fsh;

	return fsh->touchFile( path );
}

bool fss_fileExists( FILE_INTERFACE_FS *_fsh, const char *path )
{
	assert( _fsh );
	_FILE_INTERFACE_FS *fsh = (_FILE_INTERFACE_FS *)_fsh;

	return fsh->fileExists( path );
}

}; // end of namespace


namespace FILESYS
{
static FILESYS_S::FILE_INTERFACE_FS*	gFS;
static int					gFS_refcount;

bool					fs_open()
{
	bool ret = true;
	if ( gFS )
	{
		++gFS_refcount;
	}
	else
	{
		gFS = FILESYS_S::fss_open();
		gFS_refcount = 1;
	}
	return ret;
}
void					fs_close()
{
	if ( gFS_refcount > 0 )
	{
		--gFS_refcount;
		if ( gFS_refcount == 0 )
		{
			FILESYS_S::fss_close( gFS );
			gFS = 0;
		}
	}
}
#define FSS_PASSTHRU_BEG( fnRetType, fnName, ... ) \
	fnRetType	fs_##fnName( __VA_ARGS__ ) \
	{
#define FSS_PASSTHRU_END( fnRetType, fnName, ... ) \
		assert( gFS != 0 ); \
		return FILESYS_S::fss_##fnName( gFS, ## __VA_ARGS__ ); \
	}

FSS_PASSTHRU_BEG( const char *, getExeDir )
FSS_PASSTHRU_END( const char *, getExeDir )
FSS_PASSTHRU_BEG( const char *, getRootDir )
FSS_PASSTHRU_END( const char *, getRootDir )
FSS_PASSTHRU_BEG( const char *, getUserDir )
FSS_PASSTHRU_END( const char *, getUserDir )
FSS_PASSTHRU_BEG( const char *, getCurrentDir )
FSS_PASSTHRU_END( const char *, getCurrentDir )
FSS_PASSTHRU_BEG( void, setCurrentDir,			const char *path )
FSS_PASSTHRU_END( void, setCurrentDir,			path )
FSS_PASSTHRU_BEG( char *, sanitizePath,			const char *path, bool resolveToRoot )
FSS_PASSTHRU_END( char *, sanitizePath,			path, resolveToRoot )
FSS_PASSTHRU_BEG( bool, fileTimestamp,		const char *path, NxU32 *outLowDateTime, NxU32 *outHighDateTime )
FSS_PASSTHRU_END( bool, fileTimestamp,		path, outLowDateTime, outHighDateTime )
FSS_PASSTHRU_BEG( bool, fileTimestamp,		const char *path, NxU64 &outTimestamp )
FSS_PASSTHRU_END( bool, fileTimestamp,		path, outTimestamp )
FSS_PASSTHRU_BEG( bool, fileCalcCRC32,			const char *path, NxU32 &outCRC32 )
FSS_PASSTHRU_END( bool, fileCalcCRC32,			path, outCRC32 )
FSS_PASSTHRU_BEG( NxU32, memCalcCRC32,			const void *mem, NxU32 memSize )
FSS_PASSTHRU_END( NxU32, memCalcCRC32,			mem, memSize )
FSS_PASSTHRU_BEG( void *, readEntireFile,		const char *path, NxU32 &outFileSize )
FSS_PASSTHRU_END( void *, readEntireFile,		path, outFileSize )
FSS_PASSTHRU_BEG( bool, touchFile,				const char *path )
FSS_PASSTHRU_END( bool, touchFile,				path )
FSS_PASSTHRU_BEG( bool, fileExists,				const char *path )
FSS_PASSTHRU_END( bool, fileExists,				path )

}; // end of namespace