// This code contains NVIDIA Confidential Information and is disclosed 
// under the Mutual Non-Disclosure Agreement.
//
// Notice
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES 
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO 
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT, 
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable. 
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such 
// information or for any infringement of patents or other rights of third parties that may 
// result from its use. No license is granted by implication or otherwise under any patent 
// or patent rights of NVIDIA Corporation. Details are subject to change without notice. 
// This code supersedes and replaces all information previously supplied. 
// NVIDIA Corporation products are not authorized for use as critical 
// components in life support devices or systems without express written approval of 
// NVIDIA Corporation.
//
// Copyright © 2009 NVIDIA Corporation. All rights reserved.
// Copyright © 2002-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright © 2001-2006 NovodeX. All rights reserved.   

#ifndef __NV_SPIN_MUTEX__
#define __NV_SPIN_MUTEX__

#include "NvThread.h"
#include "NvAtomic.h"
#include "NvNoCopy.h"

namespace NVSHARE
{
	/*
	recursive spin lock.
	*/
	class SpinMutex
	{
	public:

		NX_INLINE SpinMutex()
		{
			lockCounter=-1;
			ownerThreadId=0;
			recursionCount=0;
		}

		NX_INLINE ~SpinMutex()
		{
			NX_ASSERT(lockCounter==-1);
			NX_ASSERT(ownerThreadId==0);
			NX_ASSERT(recursionCount==0);
		}

		NX_INLINE bool trylock()
		{
			return trylock(NVSHARE::Thread::getId());
		}

		NX_INLINE void lock()
		{

			//avoid repeated calls the NVSHARE::Thread::getId().
			Thread::Id currThreadId=NVSHARE::Thread::getId();

			while(!trylock(currThreadId))
				NxSpinLockPause();
		}


		NX_INLINE void unlock()
		{
			NX_ASSERT(ownerThreadId==NVSHARE::Thread::getId());
			NX_ASSERT(recursionCount>0);

			recursionCount--;
			if(recursionCount==0)
			{
				ownerThreadId=0;

				NVSHARE::atomicExchange(&lockCounter,-1);
			}
		}

	private:

		NX_INLINE bool trylock(Thread::Id currThreadId)
		{
			if(NVSHARE::atomicCompareExchange(&lockCounter,0,-1)==-1)
			{
				ownerThreadId=currThreadId;
				recursionCount=1;
				return true;
			}
			else if(ownerThreadId==currThreadId)
			{
				recursionCount++;
				return true;
			}
			else
				return false;
		}


		NxI32			lockCounter;
		Thread::Id		ownerThreadId;
		NxI32			recursionCount;
	};

	class SpinMutexLock : public NoCopy
	{
	public:

		NX_INLINE SpinMutexLock(SpinMutex &sm) : spinMutex(sm)
		{
			spinMutex.lock();
		}
		NX_INLINE ~SpinMutexLock()
		{
			spinMutex.unlock();
		}	

	private:
		SpinMutex &spinMutex;
	};
}
#endif
