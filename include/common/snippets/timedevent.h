#ifndef TIMED_EVENT_H

#define TIMED_EVENT_H

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

#include "common/snippets/UserMemAlloc.h"

class Gauss;

namespace TIMED_EVENT
{

class TimedEvent;


class TimedEventInterface
{
public:
	virtual bool timedEventCallback(void *user_data,HeI32 user_id,bool alive) = 0; //true if supposed to continue, false if done.
	virtual void deleteEventCallback(void *user_data,HeI32 user_id) = 0; // notification that an event got cancelled, in case data has to be deleted.

private:
};

class TimedEventFactory;

TimedEventFactory * createTimedEventFactory(void);

bool                releaseTimedEventFactory(TimedEventFactory *factory);

// times are gaussians to introduce randomization into event triggers.
HeI32 postTimedEvent(TimedEventFactory *factory,
                   TimedEventInterface *callback,
									 const Gauss &duetime,                 // by default, trigger it in one second.
									 const Gauss &repeatcount,
									 const Gauss &repeat_time,
									 void *user_data,
									 HeI32   user_id);

// explicit time
HeI32 postTimedEvent(TimedEventFactory *factory,
                   TimedEventInterface *callback,
									 HeF32 duetime,                 // by default, trigger it in one second.
									 HeI32 repeatcount,
									 HeF32 repeat_time,
									 void *user_data,
									 HeI32   user_id);


HeI32 process(TimedEventFactory *factory,HeF32 dtime); // process timed events based on this delta time since the last time we were called. returns the number of events that got fired.

HeI32 cancelAll(TimedEventFactory *factory,TimedEventInterface *callback); // cancel all events that are pending with this callback.
bool cancelTimedEvent(TimedEventFactory *factory,HeI32 id); // cancel a specific timed event based on its id number.
HeI32 cancelAll(TimedEventFactory *factory);        // cancel all timed events including global events.



};


#endif
