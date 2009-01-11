/*----------------------------------------------------------------------
		Copyright (c) 2004 Open Dynamics Framework Group
					www.physicstools.org
		All rights reserved.

		Redistribution and use in source and binary forms, with or without modification, are permitted provided
		that the following conditions are met:

		Redistributions of source code must retain the above copyright notice, this list of conditions
		and the following disclaimer.

		Redistributions in binary form must reproduce the above copyright notice,
		this list of conditions and the following disclaimer in the documentation
		and/or other materials provided with the distribution.

		Neither the name of the Open Dynamics Framework Group nor the names of its contributors may
		be used to endorse or promote products derived from this software without specific prior written permission.

		THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES,
		INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
		DISCLAIMED. IN NO EVENT SHALL THE INTEL OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
		EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
		LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
		IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
		THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common/snippets/UserMemAlloc.h"
#include "timedevent.h"


#include "gauss.h"
#pragma warning(disable:4786)

#include <set>

namespace TIMED_EVENT
{

//==================================================================================
//==================================================================================
class TimedEvent
{
public:
	TimedEvent( TimedEventInterface *callback,
				HeI32 id, HeF32 duetime,
				HeI32 repeat_count,const Gauss &repeat_time,
				void *user_data, HeI32 user_id );

	HeF32 GetDueTime(void) const;
	HeI32   GetId(void) const;

	void * GetUserData(void);
	HeI32    GetUserId(void);

	bool Process(HeF32 curtime);

	TimedEventInterface *mCallback;
	HeF32                mDueTime;
	HeI32                  mRepeatCount;
	Gauss                mRepeatTime; // use a gaussian number so that the repeat time can be randomized
	HeI32                  mId;
	void                *mUserData;
	HeI32                  mUserId;
};

//==================================================================================
inline HeF32 TimedEvent::GetDueTime(void) const
{
	return mDueTime;
}

//==================================================================================
inline HeI32 TimedEvent::GetId(void) const
{
	return mId;
}

//==================================================================================
inline void * TimedEvent::GetUserData(void)
{
	return mUserData;
}

//==================================================================================
inline HeI32 TimedEvent::GetUserId(void)
{
	return mUserId;
}

//==================================================================================
//==================================================================================
class TimedEventLess
{
public:

	bool operator()(const TimedEvent *p1,const TimedEvent *p2) const;
};


#if HE_USE_MEMORY_TRACKING
typedef USER_STL::set< TimedEvent *, USER_STL::GlobalMemoryPool, TimedEventLess > TimedEventSet;
#else
typedef USER_STL::set< TimedEvent *, TimedEventLess > TimedEventSet;
#endif

//==================================================================================
//==================================================================================
class TimedEventFactory
{
public:
	TimedEventFactory(void);
	~TimedEventFactory(void);

	HeI32 PostTimedEvent(TimedEventInterface *callback,
										 HeF32 duetime,
										 HeI32 repeatcount,
										 const Gauss &repeat_time,
										 void *user_data,
										 HeI32   user_id);


	HeI32 Process(HeF32 dtime); // process timed events.

	HeI32 cancelAll(void) // cancel all timed events
  {
    HeI32 ret = mEvents.size();
    TimedEventSet::iterator i;
    for (i=mEvents.begin(); i!=mEvents.end(); i++)
    {
      TimedEvent *te = (*i);
      if ( te->mCallback )
        te->mCallback->deleteEventCallback( te->mUserData, te->mUserId );
      delete te;
    }
    mEvents.clear();
    return ret;
  }

  HeI32 cancelAll(TimedEventInterface *callback) // cancel all timed events with this callback.
  {
    HeI32 ret = 0;

    TimedEventSet::iterator i = mEvents.begin();
    TimedEventSet::iterator kill;

    while ( i != mEvents.end() )
    {
      kill = i;  // this is the one we *might* kill
      i++;       // increment the iterator now.
      TimedEvent *te = (*kill);  // get the timed event we might delete.
      if ( te->mCallback == callback ) // if it has the same callback, then
      {
        ret++;
        te->mCallback->deleteEventCallback( te->mUserData, te->mUserId ); // notify caller that the event is going away.
        delete te;
        mEvents.erase(kill); // delete the STL set value.
      }
    }
    return ret;
  }

  bool cancelTimedEvent(HeI32 id)
  {
    bool ret = false;
    TimedEventSet::iterator i;
    for (i=mEvents.begin(); i!=mEvents.end(); i++)
    {
      TimedEvent *te = (*i);
      if ( te->mId == id )
      {
        ret = true;
        delete te;
        mEvents.erase(i);
        break;
      }
    }
    return ret;
  }

private:
	HeI32           mId;
	HeF32         mCurrentTime;
	TimedEventSet mEvents;
};


//==================================================================================
TimedEventFactory::TimedEventFactory(void)
{
	mId = 0;
	mCurrentTime = 0;
}

//==================================================================================
TimedEventFactory::~TimedEventFactory(void)
{
	cancelAll();
}

//==================================================================================
HeI32 TimedEventFactory::PostTimedEvent(TimedEventInterface *callback,
										 HeF32 duetime,
										 HeI32 repeatcount,
										 const Gauss &repeat_time,
										 void *user_data,
										 HeI32   user_id)
{
	mId++;

	HeI32 ret = mId;

	TimedEvent *te = MEMALLOC_NEW(TimedEvent)(callback,ret,mCurrentTime+duetime,repeatcount,repeat_time,user_data,user_id);
	mEvents.insert(te);

	return ret;
}

//==================================================================================
HeI32 TimedEventFactory::Process(HeF32 dtime) // process timed events.
{
  HeI32 ret = 0;

	mCurrentTime += dtime;

	while ( !mEvents.empty() )
	{
		TimedEventSet::iterator i = mEvents.begin();
		TimedEvent *te = (*i);

		if ( mCurrentTime < te->GetDueTime() )
			break;

    ret++;

		bool alive = te->Process(mCurrentTime);

		mEvents.erase(i);

		if ( alive )
		{
			mEvents.insert( te );
		}
		else
		{
      te->mCallback->deleteEventCallback(te->mUserData,te->mUserId);
			delete te;
		}
	}
  return ret;
}

TimedEvent::TimedEvent(TimedEventInterface *callback,
					   HeI32 id, HeF32 duetime,
					   HeI32 repeat_count,const Gauss &repeat_time,
					   void *user_data,
             HeI32 user_id) :
	mCallback( callback ),
	mDueTime( duetime ),
	mRepeatCount( repeat_count ),
	mRepeatTime( repeat_time ),
	mId( id ),
	mUserData( user_data ),
	mUserId( user_id )
{
}

//==================================================================================
bool TimedEvent::Process(HeF32 curtime)
{
	bool alive = false;

	if ( mRepeatCount )
	{
		mDueTime = curtime + mRepeatTime.Get();
		--mRepeatCount;
		alive = true;
	}

	if ( mCallback )
	{
		bool ok = mCallback->timedEventCallback(this->mUserData, this->mUserId, alive);
		if ( !ok )
			alive = false;
	}

	return alive;
}

//==================================================================================
bool TimedEventLess::operator()(const TimedEvent *p1,const TimedEvent *p2) const
{
	if ( p1->GetDueTime() < p2->GetDueTime() )
		return true;
	if ( p1->GetDueTime() > p2->GetDueTime() )
		return false;
	if ( p1->GetId() < p2->GetId() )
		return true;

	return false;
}


// here is the C decl wrapper interface

TimedEventFactory * createTimedEventFactory(void)
{
  TimedEventFactory *tf = MEMALLOC_NEW(TimedEventFactory);
  return tf;
}

bool releaseTimedEventFactory(TimedEventFactory *factory)
{
  bool ret = false;

  if ( factory )
  {
    ret = true;
    delete factory;
  }
  return ret;
}

HeI32 postTimedEvent(TimedEventFactory *factory,
                   TimedEventInterface *callback,
									 HeF32 duetime,                 // by default, trigger it in one second.
									 HeI32 repeat_count,
									 HeF32 repeat_time,
									 void *user_data,
									 HeI32   user_id)
{
  HeI32 ret = 0;

  Gauss g1(duetime);
  Gauss g2((HeF32)repeat_count);
  Gauss g3(repeat_time);

  ret = postTimedEvent(factory,callback,g1,g2,g3,user_data,user_id);

  return ret;
}


HeI32 process(TimedEventFactory *factory,HeF32 dtime) // process timed events based on this delta time since the last time we were called.
{
  HeI32 ret  = 0;

  if ( factory )
  {
    ret = factory->Process(dtime);
  }

  return ret;
}

HeI32 cancelAll(TimedEventFactory *factory,TimedEventInterface *callback) // cancel all events that are pending with this callback.
{
  HeI32 ret = 0;

  if ( factory )
  {
    ret = factory->cancelAll(callback);
  }

  return ret;
}

bool cancelTimedEvent(TimedEventFactory *factory,HeI32 id) // cancel a specific timed event based on its id number.
{
  bool ret = false;

  if ( factory )
  {
    ret = factory->cancelTimedEvent(id);
  }

  return ret;
}

HeI32 cancelAll(TimedEventFactory *factory)        // cancel all timed events including global events.
{
  HeI32 ret = 0;

  if ( factory )
  {
    ret = factory->cancelAll();
  }

  return ret;
}

HeI32 postTimedEvent(TimedEventFactory *factory,
                   TimedEventInterface *callback,
									 const Gauss &duetime,                 // by default, trigger it in one second.
									 const Gauss &repeatcount,
									 const Gauss &repeat_time,
									 void *user_data,
									 HeI32   user_id)
{

  HeI32 ret = 0;

  HE_ASSERT(callback); // you *must* have a callback to use the timed event system.
  HE_ASSERT(factory);
  if ( callback && factory )
  {
    Gauss g1 = duetime;
    Gauss g2 = repeatcount;
    
    ret = factory->PostTimedEvent(callback,
                                  g1.Get(),
                                  (HeI32)g2.Get(),
                                  repeat_time,user_data,user_id);
  }

  return ret;
}



}; // end of namespace
