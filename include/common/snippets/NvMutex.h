#ifndef NV_MUTEX_H
#define NV_MUTEX_H


#ifdef WIN32
#include <windows.h>
#endif

#ifdef _XBOX
#include <xtl.h>
#endif

#if defined(__APPLE__) || defined(LINUX)
#include <pthread.h>
#endif

namespace NVSHARE
{

class NvMutex
{
public:
     inline NvMutex(void)
     {
       #if defined(_WIN32) || defined(_XBOX)
  	   InitializeCriticalSection(&m_Mutex);
       #elif defined(__APPLE__) || defined(LINUX)
	   pthread_mutexattr_t mta;
	   pthread_mutexattr_init(&mta);
	   pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);
	   pthread_mutex_init(&m_Mutex, &mta);
	   pthread_mutexattr_destroy(&mta);
       #endif
    }

	inline ~NvMutex(void)
    {
      #if defined(_WIN32) || defined(_XBOX)
      DeleteCriticalSection(&m_Mutex);
      #elif defined(__APPLE__) || defined(LINUX)
      pthread_mutex_destroy(&m_Mutex);
      #endif
    }

	// Blocking Lock.
	inline void Lock(void)
    {
      #if defined(_WIN32) || defined(_XBOX)
      EnterCriticalSection(&m_Mutex);
      #elif defined(__APPLE__) || defined(LINUX)
      pthread_mutex_lock(&m_Mutex);
      #endif
    }

	// Unlock.
	inline void Unlock(void)
    {
      #if defined(_WIN32) || defined(_XBOX)
      LeaveCriticalSection(&m_Mutex);
      #elif defined(__APPLE__) || defined(LINUX)
      pthread_mutex_unlock(&m_Mutex);
      #endif
    }

private:
	#if defined(_WIN32) || defined(_XBOX)
	CRITICAL_SECTION m_Mutex;
	#elif defined(__APPLE__) || defined(LINUX)
	pthread_mutex_t  m_Mutex;
	#endif
};


/* 3.0 Foundation class version of NVSHARE::Mutex */

class MutexImpl	
{
public:

	/**
	The constructor for Mutex creates a mutex. It is initially unlocked.
	*/
	MutexImpl();

	/**
	The destructor for Mutex deletes the mutex.
	*/
	~MutexImpl();

	/**
	Acquire (lock) the mutex. If the mutex is already locked
	by another thread, this method blocks until the mutex is
	unlocked.
	*/
	bool lock();

	/**
	Acquire (lock) the mutex. If the mutex is already locked
	by another thread, this method returns false without blocking.
	*/
	bool trylock();

	/**
	Release (unlock) the mutex.
	*/
	bool unlock();

	/**
	Return size of this class.
	*/
	static size_t getSize();
};

template <typename Alloc = ReflectionAllocator<MutexImpl> >
class MutexT : protected Alloc, public UserAllocated
{
public:

	class ScopedLock : public UserAllocated, private NVSHARE::NoCopy
	{
		NVSHARE::MutexT<Alloc>& mMutex;
	public:
		NX_INLINE	ScopedLock(MutexT<Alloc> &mutex): mMutex(mutex) { mMutex.lock(); }
		NX_INLINE	~ScopedLock() { mMutex.unlock(); }
	};

	/**
	The constructor for Mutex creates a mutex. It is initially unlocked.
	*/
	MutexT(const Alloc& alloc = Alloc())
		: Alloc(alloc)
	{
		mImpl = (MutexImpl*)Alloc::allocate(MutexImpl::getSize(), __FILE__, __LINE__);
		new(mImpl) MutexImpl();
	}

	/**
	The destructor for Mutex deletes the mutex.
	*/
	~MutexT()
	{
		mImpl->~MutexImpl();
		Alloc::deallocate(mImpl);
	}

	/**
	Acquire (lock) the mutex. If the mutex is already locked
	by another thread, this method blocks until the mutex is
	unlocked.
	*/
	bool lock()		const	{ return mImpl->lock(); }

	/**
	Acquire (lock) the mutex. If the mutex is already locked
	by another thread, this method returns false without blocking.
	*/
	bool trylock()	const	{ return mImpl->trylock(); }

	/**
	Release (unlock) the mutex.
	*/
	bool unlock()	const	{ return mImpl->unlock(); }

private:
	MutexImpl* mImpl;
};

typedef MutexT<> Mutex;

}; //
#endif
