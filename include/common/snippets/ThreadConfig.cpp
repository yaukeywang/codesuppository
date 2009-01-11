#include "ThreadConfig.h"

/*!
**
** Copyright (c) 2009 by John W. Ratcliff mailto:jratcliffscarab@gmail.com
**
** If you find this code useful or you are feeling particularily generous I would
** ask that you please go to http://www.amillionpixels.us and make a donation
** to Troy DeMolay.
**
** Skype ID: jratcliff63367
** Yahoo: jratcliff63367
** AOL: jratcliff1961
** email: jratcliffscarab@gmail.com
** Personal website: http://jratcliffscarab.blogspot.com
** Coding Website:   http://codesuppository.blogspot.com
** FundRaising Blog: http://amillionpixels.blogspot.com
** Fundraising site: http://www.amillionpixels.us
** New Temple Site:  http://newtemple.blogspot.com
**
**
** The MIT license:
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
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

#if defined(WIN32)

#define _WIN32_WINNT 0x400
#include <windows.h>

#pragma comment(lib,"winmm.lib")

//	#ifndef _WIN32_WINNT

//	#endif
//	#include <windows.h>
//#include <winbase.h>
#endif

#if defined(_XBOX)
	#include "NxXBOX.h"
#endif

#if defined(__APPLE__) || defined(LINUX)
	#include <pthread.h>
#endif

namespace THREAD_CONFIG
{

unsigned int tc_timeGetTime(void)
{
  return timeGetTime();
}

void   tc_sleep(unsigned int ms)
{
  Sleep(ms);
}

void tc_spinloop()
{
   #ifdef LINUX
      __asm { pause };
   #else
      __asm { pause };
   #endif
}

void tc_interlockedExchange(void *dest, const __int64 exchange)
{
   #ifdef LINUX
      __asm
      {
         mov      ebx, dword ptr [exchange]
         mov      ecx, dword ptr [exchange + 4]
         mov      edi, dest
         mov      eax, dword ptr [edi]
         mov      edx, dword ptr [edi + 4]
         jmp      start
      retry:
         pause
      start:
         lock cmpxchg8b [edi]
         jnz      retry
      };
   #else
      __asm
      {
         mov      ebx, dword ptr [exchange]
         mov      ecx, dword ptr [exchange + 4]
         mov      edi, dest
         mov      eax, dword ptr [edi]
         mov      edx, dword ptr [edi + 4]
         jmp      start
      retry:
         pause
      start:
         lock cmpxchg8b [edi]
         jnz      retry
      };
   #endif
}

int tc_interlockedCompareExchange(void *dest,int exchange,int compare)
{
   #ifdef LINUX
      char _ret;
      //
      __asm
      {
         mov      edx, [dest]
         mov      eax, [compare]
         mov      ecx, [exchange]

         lock cmpxchg [edx], ecx

         setz    al
         mov     byte ptr [_ret], al
      }
      //
      return _ret;
   #else
      char _ret;
      //
      __asm
      {
         mov      edx, [dest]
         mov      eax, [compare]
         mov      ecx, [exchange]

         lock cmpxchg [edx], ecx

         setz    al
         mov     byte ptr [_ret], al
      }
      //
      return _ret;
   #endif
}

int tc_interlockedCompareExchange(void *dest, const int exchange1, const int exchange2, const int compare1, const int compare2)
{
   #ifdef LINUX
      char _ret;
      //
      __asm
      {
         mov     ebx, [exchange1]
         mov     ecx, [exchange2]
         mov     edi, [dest]
         mov     eax, [compare1]
         mov     edx, [compare2]
         lock cmpxchg8b [edi]
         setz    al
         mov     byte ptr [_ret], al
      }
      //
      return _ret;
   #else
      char _ret;
      //
      __asm
      {
         mov     ebx, [exchange1]
         mov     ecx, [exchange2]
         mov     edi, [dest]
         mov     eax, [compare1]
         mov     edx, [compare2]
         lock cmpxchg8b [edi]
         setz    al
         mov     byte ptr [_ret], al
      }
      //
      return _ret;
   #endif
}

class MyThreadMutex : public ThreadMutex
{
public:
  MyThreadMutex(void)
  {
    #if defined(WIN32) || defined(_XBOX)
  	InitializeCriticalSection(&m_Mutex);
    #elif defined(__APPLE__) || defined(LINUX)
  	pthread_mutex_init(&m_Mutex, 0);
    #endif
  }

  ~MyThreadMutex(void)
  {
    #if defined(WIN32) || defined(_XBOX)
  	DeleteCriticalSection(&m_Mutex);
    #elif defined(__APPLE__) || defined(LINUX)
  	pthread_mutex_destroy(&m_Mutex);
    #endif
  }

  void lock(void)
  {
    #if defined(WIN32) || defined(_XBOX)
  	EnterCriticalSection(&m_Mutex);
    #elif defined(__APPLE__) || defined(LINUX)
  	pthread_mutex_lock(&m_Mutex);
    #endif
  }

  bool tryLock(void)
  {
  	bool bRet = false;
    #if defined(WIN32) || defined(_XBOX)
  	//assert(("TryEnterCriticalSection seems to not work on XP???", 0));
  	bRet = TryEnterCriticalSection(&m_Mutex) ? true : false;
    #elif defined(__APPLE__) || defined(LINUX)
  	pthread_mutex_trylock(&m_Mutex)
    #endif
  	return bRet;
  }

  void unlock(void)
  {
    #if defined(WIN32) || defined(_XBOX)
  	LeaveCriticalSection(&m_Mutex);
    #elif defined(__APPLE__) || defined(LINUX)
  	pthread_mutex_unlock(&m_Mutex)
    #endif
  }

private:
  #if defined(WIN32) || defined(_XBOX)
	CRITICAL_SECTION m_Mutex;
	#elif defined(__APPLE__) || defined(LINUX)
	pthread_mutex_t  m_Mutex;
	#endif
};

ThreadMutex * tc_createThreadMutex(void)
{
  MyThreadMutex *m = new MyThreadMutex;
  return static_cast< ThreadMutex *>(m);
}

void          tc_releaseThreadMutex(ThreadMutex *tm)
{
  MyThreadMutex *m = static_cast< MyThreadMutex *>(tm);
  delete m;
}


static unsigned long __stdcall _ThreadWorkerFunc(LPVOID arg);

class MyThread : public Thread
{
public:
  MyThread(ThreadInterface *iface)
  {
    mInterface = iface;
   	mThread     = CreateThread(0, 0, _ThreadWorkerFunc, this, 0, 0);
  }

  ~MyThread(void)
  {
    if ( mThread )
    {
      CloseHandle(mThread);
      mThread = 0;
    }
  }

  void onJobExecute(void)
  {
    mInterface->threadMain();
  }

private:
  ThreadInterface *mInterface;
  HANDLE           mThread;
};


Thread      * tc_createThread(ThreadInterface *tinterface)
{
  MyThread *m = new MyThread(tinterface);
  return static_cast< Thread *>(m);
}

void          tc_releaseThread(Thread *t)
{
  MyThread *m = static_cast<MyThread *>(t);
  delete m;
}

static unsigned long __stdcall _ThreadWorkerFunc(LPVOID arg)
{
  MyThread *worker = (MyThread *) arg;
	worker->onJobExecute();
  return 0;
}


class MyThreadEvent : public ThreadEvent
{
public:
  MyThreadEvent(void)
  {
    mEvent = ::CreateEventA(NULL,TRUE,TRUE,"ThreadEvent");
  }

  ~MyThreadEvent(void)
  {
    if ( mEvent )
    {
      ::CloseHandle(mEvent);
    }
  }

  virtual void setEvent(void)  // signal the event
  {
    if ( mEvent )
    {
      ::SetEvent(mEvent);
    }
  }

  void resetEvent(void)
  {
    if ( mEvent )
    {
      ::ResetEvent(mEvent);
    }
  }

  virtual void waitForSingleObject(unsigned int ms)
  {
    if ( mEvent )
    {
      ::WaitForSingleObject(mEvent,ms);
    }
  }

private:
  HANDLE mEvent;
};

ThreadEvent * tc_createThreadEvent(void)
{
  MyThreadEvent *m = new MyThreadEvent;
  return static_cast<ThreadEvent *>(m);
}

void  tc_releaseThreadEvent(ThreadEvent *t)
{
  MyThreadEvent *m = static_cast< MyThreadEvent *>(t);
  delete m;
}

}; // end of namespace
