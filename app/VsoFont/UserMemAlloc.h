//NVCHANGE_BEGIN:JWR
/**
 *
 * Copyright 1998-2009 Epic Games, Inc. All Rights Reserved.
 */

#ifndef USER_MEMALLOC_H

#define USER_MEMALLOC_H

//Header file defines memory allocation macros used by the ApexClothing integration

#include <new>
#define HE_USE_MEMORY_TRACKING 0

#ifndef NULL
#define NULL 0
#endif

#define USER_STL std
#define USER_STL_EXT stdext

#define HE_REPLACE_GLOBAL_NEW_DELETE 0

void * mallocAlign(size_t size,size_t alignment);

#define MEMALLOC_NEW(x) new x
#define MEMALLOC_NEW_ARRAY(x,y) new x

#define MEMALLOC_DELETE(x,y) delete y;
#define MEMALLOC_DELETE_ARRAY(x,y) delete []y;

#define MEMALLOC_MALLOC_TYPE(x,t,f,l) ::malloc(x)
#define MEMALLOC_MALLOC_TAGGED(x,t) ::malloc(x)
#define MEMALLOC_MALLOC(x) ::malloc(x)
#define MEMALLOC_FREE(x) ::free(x)
#define MEMALLOC_REALLOC(x,y) ::realloc(x,y)
#define MEMALLOC_MALLOC_ALIGN(x,y,t,f,l) mallocAlign(x,y)

#define DEFINE_MEMORYPOOL_IN_CLASS(x)
#define IMPLEMENT_MEMORYPOOL_IN_CLASS(x)


#endif
//NVCHANGE_END:JWR
