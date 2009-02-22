#ifndef APPLICATION_SYSTEM_SERVICES_H

#define APPLICATION_SYSTEM_SERVICES_H

#include "../snippets/SystemServices.h"
#include "../snippets/UserMemAlloc.h"
#include "../snippets/SendTextMessage.h"
#include "../snippets/JobSwarm.h"
#include "../ResourceInterface/ResourceInterface.h"

#include "memalloc.h"

#pragma warning(push)
#pragma warning(disable:4100)

namespace SYSTEM_SERVICES
{

class ApplicationSystemServices : public SystemServices
{
public:
  ApplicationSystemServices(void)
  {
    gSystemServices = static_cast< SystemServices *>(this);
  }

  virtual SendTextMessage *                       getSendTextMessage(void)
  {
    return gSendTextMessage;
  }

  virtual RESOURCE_INTERFACE::ResourceInterface * getResourceInterface(void)
  {
    return gResourceInterface;
  };  // application provides an optional resource acquistion interface

  virtual JOB_SWARM::JobSwarmContext *            getJobSwarmContext(void)
  {
    return gJobSwarmContext;
  };  // application provides an optional multi-threaded job interface

  virtual void *                                  malloc(size_t size,size_t align,const char *type,const char *file,int lineno,MemoryAllocationType mtype)
  {
#if HE_USE_MEMORY_TRACKING
    return MEMALLOC::malloc(MEMALLOC::gMemAlloc,size,align,type,file,lineno,(MEMALLOC::MemAllocType)mtype);
#else
    return ::malloc(size);
#endif
  }

  virtual void          free(void *mem,MemoryAllocationType mtype)
  {
#if HE_USE_MEMORY_TRACKING
    return MEMALLOC::free(MEMALLOC::gMemAlloc,mem,(MEMALLOC::MemAllocType)mtype);
#else
    ::free(mem);
#endif
  }

  virtual void *        realloc(void *mem,size_t size,const char *file,int lineno)
  {
#if HE_USE_MEMORY_TRACKING
    return MEMALLOC::realloc(MEMALLOC::gMemAlloc,mem,size,file,lineno);
#else
    return ::realloc(mem,size);
#endif
  }


};

}; // end of namespace

#pragma warning(pop)

#endif
