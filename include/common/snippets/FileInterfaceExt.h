#ifndef FILE_INTERFACE_EXT_H

#define FILE_INTERFACE_EXT_H

#include "UserMemAlloc.h"

namespace FILESYS_S
{

typedef struct
{
	void *mInterface;
} FILE_INTERFACE_FS;

FILE_INTERFACE_FS *		fss_open();
void					fss_close( FILE_INTERFACE_FS *fsh );

// returns the exe directory.
const char *			fss_getExeDir( FILE_INTERFACE_FS *fsh );

// returns the current working directory.
const char *			fss_getRootDir( FILE_INTERFACE_FS *fsh );

// returns the user directory (e.g. "c:/Users/Shawn/")
const char *			fss_getUserDir( FILE_INTERFACE_FS *fsh );

// returns the current relative working directory.
const char *			fss_getCurrentDir( FILE_INTERFACE_FS *fsh );

// sets the relative working directory.
void					fss_setCurrentDir( FILE_INTERFACE_FS *fsh, const char *path );

// allocates and returns a normalized path (allocated using MEMALLOC_MALLOC)
char *					fss_sanitizePath( FILE_INTERFACE_FS *fsh, const char *path, bool resolveToRoot );

// retrieves the "last write time" associated with a file.
bool					fss_fileTimestamp( FILE_INTERFACE_FS *fsh, const char *path,
											NxU32 *outLowDateTime, NxU32 *outHighDateTime );
bool					fss_fileTimestamp( FILE_INTERFACE_FS *fsh, const char *path, NxU64& outTimestamp );

// calculates the CRC32 of a file.
bool					fss_fileCalcCRC32( FILE_INTERFACE_FS *fsh, const char *path, NxU32 &outCRC32 );
NxU32					fss_memCalcCRC32( FILE_INTERFACE_FS *fsh, const void *mem, NxU32 memSize );

// reads an entire file into memory (allocated using MEMALLOC_MALLOC)
void *					fss_readEntireFile( FILE_INTERFACE_FS *fsh, const char *path, NxU32 &outFileSize );

// if a file already exists at the specified path, returns true.  Otherwise, tries to create the file.
bool					fss_touchFile( FILE_INTERFACE_FS *fsh, const char *path );

// returns whether a file exists at the specified path.
bool					fss_fileExists( FILE_INTERFACE_FS *fsh, const char *path );

}; // end of namespace


namespace FILESYS
{
bool					fs_open();
void					fs_close();

// returns the exe directory.
const char *			fs_getExeDir();

// returns the current working directory.
const char *			fs_getRootDir();

// returns the user directory (e.g. "c:/Users/Shawn/")
const char *			fs_getUserDir();

// returns the current relative working directory.
const char *			fs_getCurrentDir();

// sets the relative working directory.
void					fs_setCurrentDir( const char *path );

// allocates and returns a normalized path (allocated using MEMALLOC_MALLOC)
char *					fs_sanitizePath( const char *path, bool resolveToRoot );

// retrieves the "last write time" associated with a file.
bool					fs_fileTimestamp( const char *path,
										NxU32 *outLowDateTime, NxU32 *outHighDateTime );
bool					fs_fileTimestamp( const char *path, NxU64& outTimestamp );

// calculates the CRC32 of a file.
bool					fs_fileCalcCRC32( const char *path, NxU32 &outCRC32 );
NxU32					fs_memCalcCRC32( const void *mem, NxU32 memSize );

// reads an entire file into memory (allocated using MEMALLOC_MALLOC)
void *					fs_readEntireFile( const char *path, NxU32 &outFileSize );

// if a file already exists at the specified path, returns true.  Otherwise, tries to create the file.
bool					fs_touchFile( const char *path );

// returns whether a file exists at the specified path.
bool					fs_fileExists( const char *path );
}

#endif
