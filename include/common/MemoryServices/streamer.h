#ifndef STREAMER_H

#define STREAMER_H

#include "UserMemAlloc.h"
#include <assert.h>
#include <string.h>

#define DEFAULT_BUFFER_SIZE 2048 // default is a 2k buffer off of the stack.  No memory allocation performed unless we gro past this size.

class Streamer
{
public:
  Streamer(const void *mem,HeU32 len) // read access
  {
    HE_ASSERT(mem);
    mAllocOk = false;
		mMyAlloc = false;
		mRead = true; // default is read access.
		mData = (char *) mem;
		mLen  = len;
		mLoc  = 0;
    mGrowSize = DEFAULT_BUFFER_SIZE;
  }

  Streamer(void *mem,HeU32 len) // write access
  {
    mAllocOk = false;
		mMyAlloc = false;
		mRead = false; // default is read access.
		mData = (char *) mem;
		mLen  = len;
		mLoc  = 0;
  	if ( mem == 0 || len == 0 )
		{
      mAllocOk = true;
			mData = mBuffer;
			mLen  = DEFAULT_BUFFER_SIZE;
		}
    mGrowSize = DEFAULT_BUFFER_SIZE;
  }

  Streamer(const Streamer &s)
  {
    mAllocOk = true;
    mMyAlloc = true;
    mRead    = s.mRead;
    mData    = 0;
    mLen     = s.mLen;
    mLoc     = s.mLoc;
    mGrowSize = s.mGrowSize;
    if ( mLen )
    {
      mData = (char *)MEMALLOC_MALLOC(mLen);
      memcpy(mData,s.mData,mLen);
    }
  }

  ~Streamer(void)
  {
  	if ( mMyAlloc )
  	{
  		MEMALLOC_FREE(mData);
  	}
  }

  bool         read(HeU32 &v)
  {
    return readData(&v,sizeof(v));
  }

  bool         read(bool &v)
  {
    return readData(&v,sizeof(v));
  }


  bool         read(HeF32 &v)
  {
    return readData(&v,sizeof(v));
  }

  bool         read(HeU64 &v)
  {
    return readData(&v,sizeof(v));
  }

  const char * readString(void)
  {
    const char *ret = 0;

    HeU32 len;
    if ( read(len) )
    {
      if ( (mLoc+len+1) <= mLen )
      {
        ret = &mData[mLoc];
        mLoc+=(len+1);
      }
    }

    return ret;
  }

  bool         write(HeU32 v)
  {
    return writeData(&v,sizeof(v));
  }

  bool         write(bool v)
  {
    return writeData(&v,sizeof(v));
  }


  bool         write(HeU64 v)
  {
    return writeData(&v,sizeof(v));
  }

  bool         write(HeF32 v)
  {
    return writeData(&v,sizeof(v));
  }

  bool         writeString(const char *v)
  {
    bool ret = false;

    if ( v == 0 ) v = "";
    HeU32 len = (HeU32)strlen(v);
    ret = write(len);
    if ( ret )
    {
      ret = write(v,len+1);
    }

    return ret;
  }

  void * getWriteBuffer(HeU32 &len)
  {
    void *ret = 0;
    len = 0;

    HE_ASSERT(!mRead);
    if ( !mRead )
    {
      ret = mData;
      len = (HeU32)mLoc;
    }
    return ret;
  }

  bool         write(const char *v,HeU32 len)
  {
    return writeData(v,len);
  }



  bool readData(void *_data,size_t size)
  {
  	bool ret = false;

    HE_ASSERT(mRead);
    if ( mRead )
    {
      char *data = (char *)_data;
    	if ( (mLoc+size) <= mLen )
    	{
    		memcpy(data, &mData[mLoc], size );
    		mLoc+=size;
        ret = true;
    	}
    }
    return ret;
  }

  bool writeData(const void *_data,size_t size)
  {
  	bool ret = false;

    HE_ASSERT( !mRead );
    if ( !mRead )
    {

      const char *data = (const char *)_data;

  		if ( (mLoc+size) >= mLen && mAllocOk ) // grow it
  		{
        mGrowSize*=2;
        size_t grow = mGrowSize;
        if ( size > grow ) grow = size+mGrowSize;

  			size_t newLen = mLen+grow;

  			char *data = (char *)MEMALLOC_MALLOC(newLen);
  			memcpy(data,mData,mLoc);
        if ( mMyAlloc )
        {
          MEMALLOC_FREE(mData);
        }
        mMyAlloc = true;
  			mData = data;
  			mLen  = newLen;
  		}

    	if ( (mLoc+size) <= mLen )
    	{
    		memcpy(&mData[mLoc],data,size);
    		mLoc+=size;
    		ret = true;
    	}
    }
  	return ret;
  }

  bool setLoc(HeU32 loc)
  {
    bool ret = false;
    HE_ASSERT( loc < mLen );
    if ( loc < mLen )
    {
      mLoc = loc;
      ret = true;
    }
    return ret;
  }

  const void * getReadAddress(HeU32 size) //gets the address of the memory, advances the read counter, but doesn't do a memcpy.
  {
  	const void *ret = 0;

    HE_ASSERT(mRead);
    if ( mRead )
    {
    	if ( (mLoc+size) <= mLen )
    	{
        ret = &mData[mLoc];
    		mLoc+=size;
    	}
    }
    return ret;
  }

  size_t getLoc(void) const { return mLoc; };

private:
  char     *mData;
  size_t    mGrowSize;
  size_t    mLen;
  size_t    mLoc;
  bool      mRead;
  bool      mAllocOk;
	bool      mMyAlloc;
  char      mBuffer[DEFAULT_BUFFER_SIZE];
};



#endif
