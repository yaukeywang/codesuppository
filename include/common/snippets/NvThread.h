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

#ifndef NV_THREAD_H
#define NV_THREAD_H

#include "NxSimpleTypes.h"
#include "UserMemalloc.h"

// dsequeira: according to existing comment here (David Black would be my guess)
// "This is useful to reduce bus contention on tight spin locks. And it needs
// to be a macro as the xenon compiler often ignores even __forceinline." What's not
// clear is why a pause function needs inlining...? (TODO: check with XBox team)

#if defined(NX_WINDOWS) 
#	define NxSpinLockPause() __asm pause
#elif defined(NX_X360)
#	define NxSpinLockPause() __asm nop
#elif defined(NX_LINUX)
#   define NxSpinLockPause() asm ("nop")
#elif defined(NX_CELL)
#    ifdef __SNC__
#        warning Sauce -- define this!
#        define NxSpinLockPause() 
#    else // __SNC__
#	    define NxSpinLockPause() asm ("nop") // don't know if it's correct yet...
#    endif // __SNC__
#define PX_TLS_MAX_SLOTS 64
#elif defined(NX_WII)
#	define NxSpinLockPause() asm { nop } // don't know if it's correct yet...
#endif

namespace NVSHARE
{
	/**
	\brief Used to specify a thread priority.
	  SJB - TODO: Kill this when foundation is properly ported to 'new world order'
	*/
	struct NxThreadPriority
	{
		enum Enum
		{
			/**
			\brief High priority
			*/
			E_HIGH		    =0,

			/**
			\brief Above Normal priority
			*/
			E_ABOVE_NORMAL  =1,

			/**
			\brief Normal/default priority
			*/
			E_NORMAL	    =2,

			/**
			\brief Below Normal priority
			*/
			E_BELOW_NORMAL  =3,

			/**
			\brief Low priority.
			*/
			E_LOW		    =4,

			E_FORCE_DWORD = 0xffFFffFF
		};
	};

	/**
	Thread abstraction API
	*/

	class Thread : public Memalloc
	{
	public:
		static const NxU32 DEFAULT_STACK_SIZE;
		typedef		size_t	Id;								// space for a pointer or an integer
		typedef		void*	(*ExecuteFn)(void *);

		static Id getId();
			
		/**  
		Construct (but do not start) the thread object. Executes in the context
		of the spawning thread
		*/

		Thread();

		/**  
		Construct and start the the thread, passing the given arg to the given fn. (pthread style)
		*/

		Thread(ExecuteFn fn, void *arg);


		/**
		Deallocate all resources associated with the thread. Should be called in the
		context of the spawning thread.
		*/

		virtual ~Thread();


		/**
		start the thread running. Called in the context of the spawning thread.
		*/

		void start(NxU32 stackSize);

		/**
		Violently kill the current thread. Blunt instrument, not recommended since
		it can leave all kinds of things unreleased (stack, memory, mutexes...) Should
		be called in the context of the spawning thread.
		*/

		void kill();

		/**
		The virtual execute() method is the user defined function that will
		run in the new thread. Called in the context of the spawned thread.
		*/

		virtual void execute(void);

		/**
		stop the thread. Signals the spawned thread that it should stop, so the 
		thread should check regularly
		*/

		void signalQuit();

		/**
		Wait for a thread to stop. Should be called in the context of the spawning
		thread. Returns false if the thread has not been started.
		*/

		bool waitForQuit();

		/**
		check whether the thread is signalled to quit. Called in the context of the
		spawned thread.
		*/

		bool quitIsSignalled();

		/**
		Cleanly shut down this thread. Called in the context of the spawned thread.
		*/

		void quit();

		/**
		Change the affinity mask for this thread.  Zero means use some SDK defined default.
		For platforms that do not support affinity masks (Xbox360) an explicit processor index may alternatively be provided.

		Returns previous mask if successful, or zero on failure
		*/	
		virtual NxU32 setAffinityMask(NxU32 mask, NxU32 procIDHint = 0xffffffff);


		void setPriority(NxThreadPriority::Enum prio);

		/** Put the current thread to sleep for the given number of milliseconds */

		static void sleep(NxU32 ms);

		/** Yield the current thread's slot on the CPU */

		static void yield();

	private:
		class ThreadImpl *mImpl;
	};


	NxU32			TlsAlloc();
	void			TlsFree(NxU32 index);
	void *			TlsGet(NxU32 index);
	NxU32			TlsSet(NxU32 index,void *value);
};



#endif
