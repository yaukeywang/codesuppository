#include "../snippets/UserMemAlloc.h"

namespace JOB_SWARM
{
class JobSwarmContext;
};

JOB_SWARM::JobSwarmContext *gJobSwarmContext=0;

// replace global new/delete
#if HE_REPLACE_GLOBAL_NEW_DELETE

bool globalNewDeleteExists(void)
{
  return true;
}

//==============================================================================
//
//  Finally, the public versions of global operators new and delete.  These
//  functions include appropriate DejaLib instrumentation.
//
//==============================================================================
#pragma warning(disable:4100)

void* operator new ( size_t Size )
{
#if HE_USE_MEMORY_TRACKING
  return SYSTEM_SERVICES::init()->malloc((unsigned int)Size,0,"GlobalNew", __FILE__,__LINE__, SYSTEM_SERVICES::MAT_NEW );
#else
  return MEMALLOC_MALLOC(Size);
#endif
}

void* operator new [] ( size_t Size )
{
#if HE_USE_MEMORY_TRACKING
  return SYSTEM_SERVICES::init()->malloc((unsigned int)Size,0,"GlobalNewArray",__FILE__,__LINE__, SYSTEM_SERVICES::MAT_NEW_ARRAY );
#else
  return MEMALLOC_MALLOC(Size);
#endif
}

void operator delete ( void* pMemory )
{
#if HE_USE_MEMORY_TRACKING
  if ( pMemory ) SYSTEM_SERVICES::init()->free(pMemory,SYSTEM_SERVICES::MAT_NEW);
#else
  MEMALLOC_FREE(pMemory);
#endif
}

//==============================================================================
void operator delete [] ( void* pMemory )
{
#if HE_USE_MEMORY_TRACKING
   if ( pMemory ) SYSTEM_SERVICES::init()->free(pMemory,SYSTEM_SERVICES::MAT_NEW_ARRAY);
#else
  MEMALLOC_FREE(pMemory);
#endif
}

#endif
