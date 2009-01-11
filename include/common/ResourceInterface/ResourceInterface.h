#ifndef RESOURCE_INTERFACE_H

#define RESOURCE_INTERFACE_H

#pragma warning(disable:4996)
#include "../snippets/UserMemAlloc.h"
#include <stdlib.h>
#include <wchar.h>

#ifndef LOCAL_STRING
#define LOCAL_STRING 0 // debug aid
#endif

#pragma warning(push)
#pragma warning(disable:4100)

namespace RESOURCE_INTERFACE
{

/*!
 * \brief
 * RESOURCE_HANDLE is the typedef for an opaque pointer for resource assets.
 */
typedef void * RESOURCE_HANDLE;

class ResourceInfo;

enum ResourceUpdate
{
  RU_RELEASE_NO_UPDATE,
  RU_RELEASE_UPDATES_FROM_OTHERS_ONLY,
  RU_RELEASE_UPDATE,
  RU_DO_NOT_RELEASE,
};

/*!
 * \brief
 * ResourceInterfaceCallback is a pure virtual interface to receive important notification events about resources.
 *
 */
class ResourceInterfaceCallback
{
public:
  /*!
	 * \brief
	 * notifyResource is the callback performed to the application whenever a resource is loaded, or changed.
	 *
	 * \param handle
	 * This is the handle to the resource.  @see RESOURCE_HANDLE
	 *
	 * \param data
	 * This is a pointer to the raw resource data.
	 *
	 * \param len
	 * Defines the length of the data resource.
	 *
	 * \param userData
	 * This is the optional userData pointer field that was passed in with the original resource request.
	 *
	 * The application should inherit this pure-virtual interface to receive notifications about resource updates.
	 *
	 */
	virtual ResourceUpdate notifyResource(RESOURCE_HANDLE handle,
	                            const void *mem,
	                            HeU32 len,
	                            void *userData,
                              const char *source_options,
                              const char *resource_options) = 0;

  virtual void notifyResourceInfo(ResourceInfo *) {};
  virtual void notifyDirectoryComplete(void) { };
  virtual void notifyResourceChanged(const char *fqn)
  {

  }

};

class ResourceInfo
{
public:
  ResourceInfo(void)
  {
    mFQN      = 0;
    mData     = 0;
    mLen      = 0;
    mVersion  = 0;
    mUserData = 0;
  }
  const char    *mFQN;
  void          *mData;
  HeU64          mLen;
  HeI64          mVersion;
  void          *mUserData;
};



// a pure virtual interface to serve 'resources'.
/*!
 * \brief
 * Write brief comment for ResourceInterface here.
 *
 * Write detailed description for ResourceInterface here.
 *
 * \remarks
 * Write remarks for ResourceInterface here.
 *
 * \see
 * Separate items with the '|' character.
 */

enum ResourceInterfaceFlag
{
  RIF_NONE         = 0,
  RIF_LOCAL_ONLY   = (1<<0),
  RIF_VERSION_ONLY = (1<<1),
};

class ResourceInterface
{
public:
	virtual RESOURCE_HANDLE  getResource(const char *fqn,
                                       void *userData,
                                       ResourceInterfaceCallback *iface,
                                       ResourceInterfaceFlag flag=RIF_NONE,
                                       const char *options=0) = 0;

  virtual bool                getResourceInfo(RESOURCE_HANDLE handle,ResourceInfo &info) = 0;

  virtual bool                putResource(const char *fqn,const void *data,size_t len,void *userData,const char *comment,ResourceInterfaceFlag flag=RIF_NONE,const char *options=0) = 0;

  virtual bool                deleteResource(const char * /* fqn */,const char * /* comment */)
  {
    return false;
  }


  virtual bool                releaseResource(RESOURCE_HANDLE handle,RESOURCE_INTERFACE::ResourceUpdate update) = 0;
  virtual HeI32               releaseResourceHandles(ResourceInterfaceCallback *iface) = 0; // release all resources that were registered with this callback address, returns number that were released.  Useful in destructors for guaranteed cleanup.
  virtual void                pump(void) = 0;
  virtual HeI32               getFileVersionNumber(const char* /*fqn*/,bool /*getIfMissing*/) { return -1; }; // optional method if we support file versioning.  Returns 0 if file version unavailble or -1 if this feature is not supported.  If 'getIfMissing' true, then retrive the file from the remote location.

};

};// end of namespace

extern RESOURCE_INTERFACE::ResourceInterface *gResourceInterface; // optional reference to a global variable

RESOURCE_INTERFACE::ResourceInterface * createDefaultResourceInterface(void); // will create a default class that just uses standard file IO routines to the current directory.  Good for testing.  Requires the CPP to be included.
void releaseDefaultResourceInterface(void);

void * getFileSynchronous(const char *fname,HeU32 &len);


#pragma warning(pop)

#endif
