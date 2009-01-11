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

namespace FIRE_STORM_RESOURCE
{

/*!
 * \brief
 * FS_RESOURCE_HANDLE is the typedef for an opaque pointer for resource assets.
 */
typedef void * FS_RESOURCE_HANDLE;

class FireStormResourceInfo;

enum FireStormResourceUpdate
{
  FSRU_RELEASE_NO_UPDATE,
  FSRU_RELEASE_UPDATES_FROM_OTHERS_ONLY,
  FSRU_RELEASE_UPDATE,
  FSRU_DO_NOT_RELEASE,
};

/*!
 * \brief
 * FireStormResourceInterface is a pure virtual interface to receive important notification events about resources.
 *
 */
class FireStormResourceInterface
{
public:
  /*!
	 * \brief
	 * notifyResource is the callback performed to the application whenever a resource is loaded, or changed.
	 *
	 * \param handle
	 * This is the handle to the resource.  @see FS_RESOURCE_HANDLE
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
	virtual FireStormResourceUpdate notifyResource(FS_RESOURCE_HANDLE handle,
	                            const void *mem,
	                            HeU32 len,
	                            void *userData,
                              const char *source_options,
                              const char *resource_options) = 0;

  virtual void notifyResourceInfo(FireStormResourceInfo *) {};
  virtual void notifyDirectoryComplete(void) { };
  virtual void notifyResourceChanged(const wchar_t *fqn)
  {

  }

};

#ifdef WIN32
typedef HeI64 HeU64;
#else
typedef HeI64 HeU64;
#endif

class FireStormResourceInfo
{
public:
  FireStormResourceInfo(void)
  {
    mFQN = 0;
    mData = 0;
    mLen = 0;
    mGuid = 0;
    mRepository = 0;
    mResourceLocation = 0;
    mVersion = 0;
    mTimeToRetrieve = 0;
    mUserData = 0;
    de_name = de_version = de_scn = de_ctime = de_cuser = de_ltime = de_luser = de_node_type = de_guid = de_deleted = de_locked = d_fn_size = d_fn_hash_method = d_fn_hash_value = md_fn_size = md_fn_hash_method = md_fn_hash_value = ux_repository_root = ux_root_name = ca_comment = ac_category = de_owner = de_package = 0;
  }
  const wchar_t *mFQN;
  void          *mData;
  HeU64          mLen;
  HeU64          mGuid;
  const wchar_t *mRepository;
  HeI32            mResourceLocation;
  HeU64          mVersion;
  HeF32          mTimeToRetrieve;
  void          *mUserData;

// detailed info about a single file, provided with directory searches.
  const wchar_t       *file_fqn;         // the fully qualified name.
  const wchar_t       *de_name;
  const wchar_t       *de_version;
  const wchar_t       *de_scn;
  const wchar_t       *de_ctime;
  const wchar_t       *de_cuser;
  const wchar_t       *de_ltime;
  const wchar_t       *de_luser;
  const wchar_t       *de_node_type;
  const wchar_t       *de_guid;
  const wchar_t       *de_deleted;
  const wchar_t       *de_locked;
  const wchar_t       *d_fn_size;
  const wchar_t       *d_fn_hash_method;
  const wchar_t       *d_fn_hash_value;
  const wchar_t       *md_fn_size;
  const wchar_t       *md_fn_hash_method;
  const wchar_t       *md_fn_hash_value;
  const wchar_t       *ux_repository_root;
  const wchar_t       *ux_root_name;
  const wchar_t       *ca_comment;
  const wchar_t       *ac_category;
  const wchar_t       *de_owner;
  const wchar_t       *de_package;
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
	virtual FS_RESOURCE_HANDLE  getResource(const wchar_t *fqn,
	                                        void *userData,
	                                        FireStormResourceInterface *iface,
	                                        ResourceInterfaceFlag flag=RIF_NONE,
	                                        const char *options=0) = 0;

  virtual bool                getFireStormResourceInfo(FS_RESOURCE_HANDLE handle,FireStormResourceInfo &info) = 0;

  virtual bool                putResource(const wchar_t *fqn,const void *data,size_t len,void *userData,const wchar_t *comment,ResourceInterfaceFlag flag=RIF_NONE,const char *options=0) = 0;

  virtual bool                deleteResource(const wchar_t * /* fqn */,const wchar_t * /* comment */)
  {
    return false;
  }


  virtual bool                releaseResource(FS_RESOURCE_HANDLE handle,FIRE_STORM_RESOURCE::FireStormResourceUpdate update) = 0;
  virtual HeI32               releaseResourceHandles(FireStormResourceInterface *iface) = 0; // release all resources that were registered with this callback address, returns number that were released.  Useful in destructors for guaranteed cleanup.
  virtual void                pump(void) = 0;
  virtual HeI32               getFileVersionNumber(const wchar_t* /*fqn*/,bool /*getIfMissing*/) { return -1; }; // optional method if we support file versioning.  Returns 0 if file version unavailble or -1 if this feature is not supported.  If 'getIfMissing' true, then retrive the file from the remote location.

  bool                putResource(const char *fqn,const void *data,size_t len,void *userData,const char *comment,ResourceInterfaceFlag flag=RIF_NONE,const char *options=0)
  {
    wchar_t wfqn[512];
    wchar_t wcomment[512];
   	mbstowcs( wfqn, fqn, 512 );
    wcomment[0] = 0;
    if ( comment )
      mbstowcs( wcomment, comment, 512 );
    return putResource(wfqn,data,len,userData,wcomment,flag,options);
  }

  FS_RESOURCE_HANDLE  getResource(const char *fqn,void *userData,FireStormResourceInterface *iface,ResourceInterfaceFlag flag=RIF_NONE,const char *options=0)
  {
    wchar_t wfqn[512];
    mbstowcs( wfqn, fqn, 512 );
    return getResource(wfqn,userData,iface,flag,options);
  }

  HeI32 getFileVersionNumber(const char* fqn,bool getIfMissing)
  {
    wchar_t wfqn[512];
    mbstowcs( wfqn, fqn, 512 );
    return getFileVersionNumber(wfqn,getIfMissing);
  }


};

};// end of namespace

extern FIRE_STORM_RESOURCE::ResourceInterface *gResourceInterface; // optional reference to a global variable

FIRE_STORM_RESOURCE::ResourceInterface * createDefaultResourceInterface(void); // will create a default class that just uses standard file IO routines to the current directory.  Good for testing.  Requires the CPP to be included.
void releaseDefaultResourceInterface(void);

void * getFileSynchronous(const char *fname,HeU32 &len);


#pragma warning(pop)

#endif
