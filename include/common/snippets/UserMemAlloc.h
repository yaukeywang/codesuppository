//NVCHANGE_BEGIN:JWR
/**
 *
 * Copyright 1998-2009 Epic Games, Inc. All Rights Reserved.
 */

#ifndef USER_MEMALLOC_H

#define USER_MEMALLOC_H

#include "NxSimpleTypes.h"

//Header file defines memory allocation macros used by the ApexClothing integration

#include <new>

#ifndef NULL
#define NULL 0
#endif

#define USE_RDESTL 0

#if USE_RDESTL

#include <rde_vector.h>
#include <rde_list.h>
#include <rde_hash_map.h>
#include <rde_algorithm.h>
#include <rde_string.h>

#define USER_STL rde
#define USER_STL_EXT rde

#else

#define USER_STL std
#define USER_STL_EXT stdext

#include <vector>
#include <list>
#include <hash_map>
#include <algorithm>
#include <string>

#endif

#define MEMALLOC_NEW(x) new x

#define MEMALLOC_MALLOC(x) ::malloc(x)
#define MEMALLOC_MALLOC_TYPE(x,t,f,l) ::malloc(x)
#define MEMALLOC_FREE(x) ::free(x)
#define MEMALLOC_REALLOC(x,y) ::realloc(x,y)

#pragma warning(push)
#pragma warning(disable:4100)

/**
Subclasses of this base class automatically take part in user memory management
Should be called Allocateable but then we collide with Ice::Allocateable.
*/
class Memalloc
{
public:
	NX_INLINE void* operator new(size_t size);
	NX_INLINE void* operator new[](size_t size);
	NX_INLINE void* operator new(size_t size, const char* fileName, int line, const char* className);
	NX_INLINE void* operator new[](size_t size, const char* fileName, int line, const char* className);

	NX_INLINE void  operator delete(void* p, const char*, int, const char*);
	NX_INLINE void  operator delete[](void* p, const char*, int, const char*);

	NX_INLINE void  operator delete(void* p);
	NX_INLINE void  operator delete[](void* p);
};

NX_INLINE void* Memalloc::operator new(size_t size)
{
    return MEMALLOC_MALLOC(size);
}

NX_INLINE void* Memalloc::operator new[](size_t size)
{
    return MEMALLOC_MALLOC(size);
}

NX_INLINE void Memalloc::operator delete(void* p)
{
    MEMALLOC_FREE(p);
}

NX_INLINE void Memalloc::operator delete[](void* p)
{
    MEMALLOC_FREE(p);
}

NX_INLINE void* Memalloc::operator new(size_t size, const char* fileName, int line, const char* className)
{
    return MEMALLOC_MALLOC_TYPE(size,className,fileName,line);
}

NX_INLINE void* Memalloc::operator new[](size_t size, const char* fileName, int line, const char* className)
{
    return MEMALLOC_MALLOC_TYPE(size,className,fileName,line);
}

NX_INLINE void Memalloc::operator delete(void* p, const char*, int, const char*)
{
    MEMALLOC_FREE(p);
}

NX_INLINE void Memalloc::operator delete[](void* p, const char*, int, const char*)
{
    MEMALLOC_FREE(p);
}

#pragma warning(pop)


#endif
//NVCHANGE_END:JWR
