#include <stdio.h>
#ifdef WIN32
#	define NOMINMAX
#	include <windows.h>
#endif

#if defined(_XBOX)
#	include <xtl.h>
#endif

#include "NvTiming.h"

#if defined(__CELLOS_LV2__) || defined(_XBOX) || defined(LINUX) || defined(__PPCGEKKO__)
	NxU32 timeGetTime();
#elif defined(WIN32) || defined(_WIN64)

#ifndef NOMINMAX
#	define NOMINMAX
#endif

#include <windows.h>


#endif

#if defined(__CELLOS_LV2__)

#include <sys/sys_time.h>
#include <sys/time_util.h>

namespace NVSHARE
{

typedef union _LARGE_INTEGER {
    uint64_t QuadPart;
} LARGE_INTEGER;

inline void QueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount){
	SYS_TIMEBASE_GET(lpPerformanceCount->QuadPart);
}

inline void QueryPerformanceFrequency(LARGE_INTEGER *lpFrequency){
	lpFrequency->QuadPart = sys_time_get_timebase_frequency();
}

}; // end of namespace

#endif //defined(__CELLOS_LV2__)

#if defined(LINUX)

#include <stdint.h>
#include <time.h>

namespace NVSHARE
{

typedef union _LARGE_INTEGER
{
	uint64_t QuadPart;
} LARGE_INTEGER;

inline void QueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount)
{
	lpPerformanceCount->QuadPart = clock();
}

inline void QueryPerformanceFrequency(LARGE_INTEGER *lpFrequency)
{
	lpFrequency->QuadPart = CLOCKS_PER_SEC;
}

}; // end of namespace

#endif // defined(LINUX)


#if defined(__CELLOS_LV2__)

#include <sys/sys_time.h>
#include <sys/time_util.h>

namespace NVSHARE
{

NxU32 timeGetTime()
{
	static uint64_t ulScale=0;
	uint64_t ulTime;

	if (ulScale==0) {
		ulScale = sys_time_get_timebase_frequency() / 1000;
	}

#ifdef __SNC__
	ulTime=__builtin_mftb();
#else
	asm __volatile__ ("mftb %0" : "=r" (ulTime) : : "memory");
#endif

	return ulTime/ulScale;
}

}; // end of namespace

#elif defined(LINUX)

#include <sys/time.h>

naemspace NVSHARE
{

NxU32 timeGetTime()
{
        timeval tim;
	gettimeofday(&tim, NULL);
	NxU32 ms = (tim.tv_sec*1000)+(tim.tv_usec/1000.0);
	return ms;
}

}; // end of namespace

#elif defined(_XBOX)

namespace NVSHARE
{

NxU32 timeGetTime()
{
	LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    NxU32 ticksPerMillisecond = (NxU32)(freq.QuadPart/1000);

	LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (NxU32)(counter.QuadPart/ticksPerMillisecond);
}

}; // end of namespace

#elif defined(__PPCGEKKO__)
#include <revolution/os.h>

namespace NVSHARE
{

NxU32 timeGetTime()
{
	return OSTicksToMilliseconds(OSGetTick());
}

}; // end of namespace

#endif

namespace NVSHARE
{

NxF32 getCurrentTime()
{
	unsigned int currentTime = timeGetTime();
	return (NxF32)(currentTime)*0.001f;
}

}; // end of namespace

#if defined(__PPCGEKKO__)

namespace NVSHARE
{

NxF32 getElapsedTime()
{
	static OSTick previousTime;

	static bool init = false;
	if(!init){
 	    previousTime = OSGetTick();
		init=true;
	}
	return (NxF32) OSTicksToMilliseconds(OSDiffTick(OSGetTick(),previousTime))/1000.0f;

}

}; // end of namespace

#else
#if !defined(LINUX)

namespace NVSHARE
{

NxF32 getElapsedTime()
{
	static LARGE_INTEGER previousTime;
	static LARGE_INTEGER freq;
	static bool init = false;
	if(!init){
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&previousTime);
		init=true;
	}
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	NxU32 elapsedTime = (NxU32)(currentTime.QuadPart - previousTime.QuadPart);
	previousTime = currentTime;
	return (NxF32)(elapsedTime)/(freq.QuadPart);
}

}; // end of namepace

#else

namespace NVSHARE
{

NxF32 getElapsedTime()
{
	static timeval previousTime;
	static bool init = false;
	if(!init){
 	        gettimeofday(&previousTime, NULL);
		init=true;
	}
	timeval currentTime;
	gettimeofday(&currentTime, NULL);
	double elapsedTime = (currentTime.tv_sec+currentTime.tv_usec/1000000.0) - (previousTime.tv_sec+previousTime.tv_usec/1000000.0);
	previousTime = currentTime;
	return (NxF32)elapsedTime;
}

}; // end of namespace

#endif
#endif

