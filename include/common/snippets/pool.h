#ifndef POOL_H

#define POOL_H

#include "UserMemAlloc.h"
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
** Personal website: http://jratcliffscarab.blogspot.com
** Coding Website:   http://codesuppository.blogspot.com
** FundRaising Blog: http://amillionpixels.blogspot.com
** Fundraising site: http://www.amillionpixels.us
** New Temple Site:  http://newtemple.blogspot.com
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





/** @file pool.h

 *  @brief A template class to handle high-speed iteration, allocation, and deallocation of fixed sized objects.
 *
 *  @author John W. Ratcliff
*/
#include <assert.h>
#include <string.h>

#include "UserMemAlloc.h"

#pragma warning(push)
#pragma warning(disable:4100)

class MPoolExtra
{
public:
  MPoolExtra(size_t mlen,const char *poolType,const char *file,NxI32 lineno)
  {
    mPoolType = poolType;
    mNext = 0;

    mData = (char *)MEMALLOC_MALLOC_TYPE(sizeof(char)*mlen,poolType,file,lineno);

    memset(mData,0,mlen);
  }

  ~MPoolExtra(void)
  {
    MEMALLOC_FREE(mData);
  }

  MPoolExtra *mNext;  // the 'next' block allocated.
  char      *mData;
  const char *mPoolType;
};


template <class Type > class Pool
{
public:
	Pool(void)
	{
    mPoolType  = "GENERIC-POOL";
		mHead      = 0;
    mTail      = 0;
		mFree      = 0;
		mData      = 0;
		mCurrent   = 0;
		mFreeCount = 0;
		mUsedCount = 0;
		mMaxUsed   = 0;
    mMaxItems  = 0;
    mGrowCount = 0;
    mStartCount = 0;
    mCurrentCount = 0;
    mInitialized = false;
	};

	~Pool(void)
	{
    Release();
	};


	void Release(void)
	{
		mHead = 0;
    mTail = 0;
		mFree = 0;

    // delete all of the memory blocks we allocated.
    MPoolExtra *extra = mData;
    while ( extra )
    {
      MPoolExtra *next = extra->mNext;
      delete extra;
      extra = next;
    }

		mData = 0;
		mCurrent = 0;
		mFreeCount = 0;
		mUsedCount = 0;
		mMaxUsed = 0;
    mInitialized = false;
	};

	void Set(NxI32 startcount,NxI32 growcount,NxI32 maxitems,const char *poolType,const char *file,NxI32 lineno)
	{
    mPoolType = poolType;
    mFile     = file;
    mLineNo   = lineno;

    Release();

		mMaxItems = maxitems;
    mGrowCount = growcount;
    mStartCount = startcount;
    mCurrentCount = startcount;

    if ( mStartCount > 0 )
    {
      mData = MEMALLOC_NEW(MPoolExtra)(sizeof(Type)*mStartCount,mPoolType,mFile,mLineNo);
      Type *data = (Type *) mData->mData;
      {
        Type *t = (Type *)mData->mData;
        for (NxI32 i=0; i<mStartCount; i++)
        {
          new ( t ) Type;
          t++;
        }
      }
		  mFree = data;
		  mHead = 0;
      mTail = 0;
		  NxI32 i;
		  for (i=0; i<(startcount-1); i++)
		  {
			  data[i].SetNext( &data[i+1] );
			  if ( i == 0 )
				  data[i].SetPrevious( 0 );
			  else
				  data[i].SetPrevious( &data[i-1] );
		  }

		  data[i].SetNext(0);
		  data[i].SetPrevious( &data[i-1] );
		  mCurrent = 0;
		  mFreeCount = startcount;
		  mUsedCount = 0;
    }
    mInitialized = true;
	};


	Type * GetNext(bool &looped)
	{

		looped = false; // default value

		if ( !mHead ) return 0; //  there is no data to process.

		Type *ret;

		if ( !mCurrent )
		{
			ret = mHead;
			looped = true;
		}
		else
		{
			ret = mCurrent;
		}

		if ( ret ) mCurrent = ret->GetNext();


		return ret;
	};

	bool IsEmpty(void) const
	{
		if ( !mHead ) return true;
		return false;
	};

	NxI32 Begin(void)
	{
		mCurrent = mHead;
		return mUsedCount;
	};

	NxI32 GetUsedCount(void) const { return mUsedCount; };
	NxI32 GetFreeCount(void) const { return mFreeCount; };

	Type * GetNext(void)
	{
		if ( !mHead ) return 0; //  there is no data to process.

		Type *ret;

		if ( !mCurrent )
		{
			ret = mHead;
		}
		else
		{
			ret = mCurrent;
		}

		if ( ret ) mCurrent = ret->GetNext();


		return ret;
	};

	Type * Release(Type *t)
	{

		if ( t == mCurrent ) mCurrent = t->GetNext();

    if ( t == mTail )
    {
      mTail = t->GetPrevious(); // the new tail..
    }

		//  first patch old linked list.. his previous now points to his next
		Type *prev = t->GetPrevious();

		if ( prev )
		{
			Type *next = t->GetNext();
			prev->SetNext( next ); //  my previous now points to my next
			if ( next ) next->SetPrevious(prev);
			//  list is patched!
		}
		else
		{
			Type *next = t->GetNext();
			mHead = next;
			if ( mHead ) mHead->SetPrevious(0);
		}

		Type *temp = mFree; //  old head of MEMALLOC_FREE list.
		mFree = t; //  new head of linked list.
		t->SetPrevious(0);
		t->SetNext(temp);

		mUsedCount--;
    assert(mUsedCount >= 0);
		mFreeCount++;
    return mCurrent;
	};

	Type * GetFreeLink(void)
	{
		//  Free allocated items are always added to the head of the list
		if ( !mFree )
    {
      getMore();
    }

		Type *ret = mFree;

    if ( mFree )
    {
  		mFree = ret->GetNext(); //  new head of MEMALLOC_FREE list
  		Type *temp = mHead; //  current head of list
      if ( mHead == 0 )  // if it's the first item then this is the head of the list.
      {
        mTail = ret;
    		mHead = ret;        //  new head of list is this MEMALLOC_FREE one
    		if ( temp ) temp->SetPrevious(ret);
    		mHead->SetNext(temp);
    		mHead->SetPrevious(0);
      }
      else
      {
        assert( mTail );
        assert( mTail->GetNext() == 0 );
        mTail->SetNext( ret );
        ret->SetPrevious( mTail );
        ret->SetNext(0);
        mTail = ret;
      }
  		mUsedCount++;
  		if ( mUsedCount > mMaxUsed ) mMaxUsed = mUsedCount;
  		mFreeCount--;
    }
 		return ret;
	};

  Type * getMore(void) // ok, we need to see if we can grow some more.
  {
    Type *ret = mFree;
    if ( ret == 0 && (mCurrentCount+mGrowCount) < mMaxItems && mGrowCount > 0 ) // ok..we are allowed to allocate some more...
    {
      MPoolExtra *pe = mData; // the old one...
      mData  = MEMALLOC_NEW(MPoolExtra)(sizeof(Type)*mGrowCount,mPoolType,mFile,mLineNo);
      {
        Type *t = (Type *)mData->mData;
        for (NxI32 i=0; i<mGrowCount; i++)
        {
          new ( t ) Type;
          t++;
        }
      }
      mData->mNext = pe; // he points to the old one.
      // done..memory allocated and added to singly linked list.

      Type *data = (Type *) mData->mData;
  		mFree = data;     // new head of MEMALLOC_FREE list.
  		NxI32 i;
	  	for (i=0; i<(mGrowCount-1); i++)
  		{
	  		data[i].SetNext( &data[i+1] );
		  	if ( i == 0 )
			  	data[i].SetPrevious( 0 );
  			else
	  			data[i].SetPrevious( &data[i-1] );
		  }
  		data[i].SetNext(0);
	  	data[i].SetPrevious( &data[i-1] );

      mFreeCount+=mGrowCount; // how many new MEMALLOC_FREE entries we have added...
      mCurrentCount+=mGrowCount;
    }
    return ret;
  }

  bool isInitialized(void) const { return mInitialized; };

//private:
  bool         mInitialized;
	NxI32        mMaxItems;
  NxI32        mGrowCount;
  NxI32        mStartCount;
  NxI32        mCurrentCount; // this is total allocated, not MEMALLOC_FREE/used

	Type       *mCurrent;
	MPoolExtra *mData;
	Type        *mHead;
  Type        *mTail;
	Type        *mFree;
	NxI32        mUsedCount;
	NxI32        mFreeCount;
	NxI32        mMaxUsed;
  const char *mPoolType;
  const char *mFile;
  NxI32         mLineNo;
};


#pragma warning(pop)

#endif
