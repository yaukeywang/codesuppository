#ifndef USER_MEMALLOC_H

#define USER_MEMALLOC_H


#define HE_USE_MEMORY_TRACKING 1

#ifndef NULL
#define NULL 0
#endif

#include "HeSimpleTypes.h"


#if HE_USE_MEMORY_TRACKING

#define HE_USE_FIXED_POOL_ALLOCATOR 1
#define HE_REPLACE_GLOBAL_NEW_DELETE 1

#include "../snippets/UserMemAlloc.h"
#include "../MemoryServices/MemoryContainer.h"
#include "../MemoryServices/memalloc.h"

#define MEMALLOC_NEW(x) new ( MEMALLOC::malloc(MEMALLOC::gMemAlloc,sizeof(x),0,#x,__FILE__,__LINE__,MEMALLOC::MAT_NEW) )x
#define MEMALLOC_NEW_ARRAY(x,y) new ( MEMALLOC::malloc(MEMALLOC::gMemAlloc,sizeof(x)*(y)+sizeof(size_t),0,#x,__FILE__,__LINE__,MEMALLOC::MAT_NEW_ARRAY) )x

#define MEMALLOC_DELETE(x,y) delete y;
#define MEMALLOC_DELETE_ARRAY(x,y) delete []y;

#define MEMALLOC_MALLOC_TYPE(x,t,f,l) MEMALLOC::malloc(MEMALLOC::gMemAlloc,x,0,t,f,l,MEMALLOC::MAT_MALLOC)
#define MEMALLOC_MALLOC_TAGGED(x,t) MEMALLOC::malloc(MEMALLOC::gMemAlloc,x,0,t,__FILE__,__LINE__,MEMALLOC::MAT_MALLOC)
#define MEMALLOC_MALLOC(x) MEMALLOC::malloc(MEMALLOC::gMemAlloc,x,0,__FILE__,__FILE__,__LINE__,MEMALLOC::MAT_MALLOC)
#define MEMALLOC_FREE(x) MEMALLOC::free(MEMALLOC::gMemAlloc,x,MEMALLOC::MAT_MALLOC)
#define MEMALLOC_REALLOC(x,y) MEMALLOC::realloc(MEMALLOC::gMemAlloc,x,y,__FILE__,__LINE__)
#define MEMALLOC_HEAP_CHECK() MEMALLOC::heapCheck(MEMALLOC::gMemAlloc)
#define MEMALLOC_REPORT(x,y,z) MEMALLOC::report(MEMALLOC::gMemAlloc,x,y,z)
#define MEMALLOC_GET_HEAP_SIZE(x) MEMALLOC::getMemoryUsed(MEMALLOC::gMemAlloc,x)
#define MEMALLOC_SET_SEND_TEXT_MESSAGE(x) MEMALLOC::setSendTextMessage(MEMALLOC::gMemAlloc,x)
#define MEMALLOC_MALLOC_ALIGN(x,y,t,f,l) MEMALLOC::malloc(MEMALLOC::gMemAlloc,x,y,t,f,l,MEMALLOC::MAT_MALLOC)
#define MEMALLOC_FRAME_BEGIN(x) MEMALLOC::frameBegin(MEMALLOC::gMemAlloc,x)
#define MEMALLOC_FRAME_END(x,y) MEMALLOC::frameEnd(MEMALLOC::gMemAlloc,x,y)
#define MEMALLOC_SET_MEMORY_SHUTDOWN(x) MEMALLOC::setMemoryShutdown(x)
#define MEMALLOC_CORE_DUMP(x,y,z) MEMALLOC::coreDump(MEMALLOC::gMemAlloc,x,y,z)
#define MEMALLOC_PROCESS_CORE_DUMP(x,y,z) MEMALLOC::coreDump(MEMALLOC::gMemAlloc,x,y,z)

#define USER_STL MemoryContainer

#else


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
#define MEMALLOC_HEAP_CHECK()
#define MEMALLOC_REPORT(x,y,z) x; y; z;
#define MEMALLOC_GET_HEAP_SIZE(x) (x = 0)
#define MEMALLOC_SET_SEND_TEXT_MESSAGE(x)
#define MEMALLOC_MALLOC_ALIGN(x,y,t,f,l) mallocAlign(x,y)
#define MEMALLOC_FRAME_BEGIN(x) x;
#define MEMALLOC_FRAME_END(x,y) x; y;
#define MEMALLOC_CORE_DUMP(x,y,z) x; y; z;
#define MEMALLOC_PROCESS_CORE_DUMP(x,y,z)
#define MEMALLOC_HEAP_COMPACT()
#define MEMALLOC_SET_MEMORY_SHUTDOWN(x)

#define DEFINE_MEMORYPOOL_IN_CLASS(x)
#define IMPLEMENT_MEMORYPOOL_IN_CLASS(x)

#define USER_STL std

#endif

#endif
