#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <wchar.h>

#include "ResourceInterface.h"
#include "UserMemAlloc.h"
#include "filesystem.h"
#include "pool.h"

using namespace NVSHARE;

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
**
**
** The MIT license:
**
** Permission is hereby granted, MEMALLOC_FREE of charge, to any person obtaining a copy
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

#pragma warning(disable:4100)


namespace RESOURCE_INTERFACE
{

class LocalResourceHandle : public ResourceInfo
{
public:
  LocalResourceHandle(void)
  {
    mFQN = 0;
    mUserData = 0;
    mNext = 0;
    mPrevious = 0;
    mCallback = 0;
    mOptions = 0;
    mVersion = -1;
  }

  ~LocalResourceHandle(void)
  {
    release();
  }

  void init(const char *fqn,void *userData,const void *mem,NxU32 len,ResourceInterfaceCallback *iface,const char *options)
  {
    NxU32 wlen = strlen(fqn);
    mFQN = (char *) MEMALLOC_MALLOC(sizeof(char)*(wlen+1));
    strcpy((char *)mFQN,fqn);
    mUserData = userData;
    mData     = (void *)mem;
    mLen      = len;
    mCallback = iface;
    mOptions  = 0;
    if ( options )
    {
      size_t len = strlen(options);
      mOptions = (char *)MEMALLOC_MALLOC(len+1);
      strcpy(mOptions,options);
    }
  }

  void release(void)
  {
    delete[]mFQN;
    delete mData;
    MEMALLOC_FREE(mOptions);
    mOptions = 0;
    mFQN = 0;
    mUserData = 0;
    mCallback = 0;
  }

  LocalResourceHandle * GetNext(void) const { return mNext; };
  LocalResourceHandle * GetPrevious(void) const { return mPrevious; };
  void             SetNext(LocalResourceHandle *next) { mNext = next; };
  void             SetPrevious(LocalResourceHandle *previous) { mPrevious = previous; };

  LocalResourceHandle  *mNext;
  LocalResourceHandle  *mPrevious;
  ResourceInterfaceCallback *mCallback;
  void *mUserData;
  char *mOptions;
};


#define MAX_HANDLES 1000000
#define START_HANDLES 10
#define GROW_HANDLES  10

class FileResourceInterface : public ResourceInterface, public NVSHARE::Memalloc
{
public:
  FileResourceInterface(void)
  {
    mHandles.Set(START_HANDLES,GROW_HANDLES,MAX_HANDLES,"Resource->ResourceHandle",__FILE__,__LINE__);
  }
  ~FileResourceInterface(void)
  {
    NxI32 hcount = mHandles.Begin();
    for (NxI32 i=0; i<hcount; i++)
    {
      LocalResourceHandle *h = mHandles.GetNext();
      assert(h);
      if ( h )
      {
        h->release();
        mHandles.Release(h);
      }
    }
  }

	RESOURCE_HANDLE     getResource(const char *fqn,void *userData,ResourceInterfaceCallback *iface,ResourceInterfaceFlag flag,const char *options);
  bool                putResource(const char *fqn,const void *data,size_t len,void *userData,const char *comment,ResourceInterfaceFlag flag,const char *options);
  bool                releaseResource(RESOURCE_HANDLE handle,RESOURCE_INTERFACE::ResourceUpdate update);
  bool                getResourceInfo(RESOURCE_HANDLE handle,ResourceInfo &info);
  void                pump(void);

  NxI32               releaseResourceHandles(ResourceInterfaceCallback *iface) // release all resources that were registered with this callback address, returns number that were released.  Useful in destructors for guaranteed cleanup.
  {
    NxI32 ret = 0;

    NxI32 rcount = mHandles.Begin();
    for (NxI32 i=0; i<rcount; i++)
    {
      LocalResourceHandle *rh = mHandles.GetNext();
      assert(rh);
      if ( rh )
      {
        if ( rh->mCallback == iface )
        {
          rh->release();
          mHandles.Release();
          ret++;
        }
      }
    }
    return ret;
  }

private:
  Pool< LocalResourceHandle > mHandles;

};

RESOURCE_HANDLE  FileResourceInterface::getResource(const char *fqn,void *userData,ResourceInterfaceCallback *iface,ResourceInterfaceFlag flag,const char *options)
{
  void *ret = 0;
  NxU32 len=0;
  if ( fqn )
  {
    const char *fname = fqn;

#if LOCAL_STRING
    fname = localString(fname);
#else
    if ( gFileSystem )
    {
      fname = gFileSystem->FileOpenString(fname,true);
    }
#endif

    FILE *fph = fopen(fname,"rb");
    if ( fph == 0 )
    {
      fph = fopen(fqn,"rb");
    }


    if ( fph )
    {
      fseek(fph,0L,SEEK_END);
      len = ftell(fph);
      fseek(fph,0L,SEEK_SET);
      if ( len == 0 )
      {
        fclose(fph);
      }
      else
      {
        ret = (char *)MEMALLOC_MALLOC(len);

        if ( ret )
        {
          size_t r = fread(ret,len,1,fph);
          if ( r == 1 )
          {
          }
          else
          {
            delete ret;
            ret = 0;
          }
          fclose(fph);
        }
      }
    }
  }

  if ( ret )
  {
    LocalResourceHandle *rd = mHandles.GetFreeLink();
    assert(rd);
    if ( rd )
    {
      rd->init(fqn,userData,ret,len,iface,options);
      ret = rd;
      if ( iface )
      {
        ResourceUpdate update = iface->notifyResource(rd, rd->mData, (NxU32)rd->mLen, rd->mUserData, rd->mOptions, 0 );
        assert(update != RU_DO_NOT_RELEASE );
        if ( update != RU_DO_NOT_RELEASE )
        {
          releaseResource(rd,update);
          ret = 0;
        }
      }
    }
  }
  else
  {
    if ( iface )
    {
      LocalResourceHandle *rd = mHandles.GetFreeLink();
      assert(rd);
      if ( rd )
      {
        rd->init(fqn,userData,0,0,iface, options);
        ResourceUpdate update = iface->notifyResource(rd, rd->mData, (NxU32)rd->mLen, rd->mUserData, rd->mOptions, 0 );
        assert( update != RU_DO_NOT_RELEASE );
        if ( update != RU_DO_NOT_RELEASE )
        {
          releaseResource(rd,update);
          ret = 0;
        }
      }
    }
    else
    {
      ret = 0;
    }
  }


  return ret;
}

bool FileResourceInterface::releaseResource(RESOURCE_HANDLE mem,RESOURCE_INTERFACE::ResourceUpdate update)
{
  bool ret = false;

  if ( mem )
  {
    LocalResourceHandle *rd = (LocalResourceHandle *) mem;
    rd->release();
    mHandles.Release(rd);
    ret = true;
  }

  return ret;
}


bool  FileResourceInterface::getResourceInfo(RESOURCE_HANDLE handle,ResourceInfo &info)
{
  bool ret = false;
  if ( handle )
  {
    LocalResourceHandle *rd = (LocalResourceHandle *) handle;
    info = *rd;
    if ( info.mData )
    {
      ret = true;
    }
  }
  return ret;
}

bool FileResourceInterface::putResource(const char *fqn,const void *data,size_t len,void * userData,const char * comment,ResourceInterfaceFlag flag,const char *options)
{
  bool ret = false;
  if ( fqn && data && len )
  {
    const char *fname = fqn;

#if LOCAL_STRING
    fname = localString(fname);
#else
    if ( gFileSystem )
    {
      fname = gFileSystem->FileOpenString(fname,false);
    }
#endif


    FILE *fph = fopen(fname,"wb");
    if ( fph )
    {
      size_t r = fwrite(data,len,1,fph);
      if ( r == 1 )
      {
        ret = true;
      }
      fclose(fph);
    }
  }
  return ret;
}

void FileResourceInterface::pump(void)
{
}

}; // end of namespace


static RESOURCE_INTERFACE::FileResourceInterface *gFileResourceInterface=0;

using namespace RESOURCE_INTERFACE;

ResourceInterface * createDefaultResourceInterface(void) // will create a default class that just uses standard file IO routines to the current directory.  Good for testing.  Requires the CPP to be included.
{
  if ( gFileResourceInterface == 0 )
  {
    gFileResourceInterface = MEMALLOC_NEW(FileResourceInterface);
    gResourceInterface = gFileResourceInterface;
  }
  return gResourceInterface;
}

void releaseDefaultResourceInterface(void)
{
  delete gFileResourceInterface;
  gFileResourceInterface = 0;
  gResourceInterface = 0;
}


void * getFileSynchronous(const char *fname,NxU32 &len)
{
  void * ret = 0;
  len = 0;

  if ( gResourceInterface )
  {
    RESOURCE_INTERFACE::RESOURCE_HANDLE handle = gResourceInterface->getResource(fname,0,0,RIF_NONE);
    if ( handle )
    {
      RESOURCE_INTERFACE::ResourceInfo info;
      bool ok = gResourceInterface->getResourceInfo(handle,info);
      if ( ok && info.mLen )
      {
        len = (NxU32) info.mLen;
        char *mem = (char *)MEMALLOC_MALLOC(len);
        memcpy(mem,info.mData,len);
        gResourceInterface->releaseResource(handle,RESOURCE_INTERFACE::RU_RELEASE_NO_UPDATE);
        ret = mem;
      }
    }
  }
  return ret;
}

