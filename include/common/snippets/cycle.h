#ifndef CYCLE_H

#define CYCLE_H

/** @file cycle.h
 *  @brief This is a utility class that counts instruction cycles using the RTDSC instruction.
 *
 * This utility class is used by the embedded performance monitor and profiler to assist in
 * gathering performance data on the engine during runtime.
 *
 *  @author John W. Ratcliff
*/

#include "UserMemAlloc.h"

// a gross approximation of 1 millisecond of clock cycles.
const NxU64 CYCLES_MS=(2000000000/100);

class Cycle
{
public:
  Cycle(void)
  {
    mTime = 0;
  }

  static NxU64 getRTDSC(void)
  {
#if WIN32
    NxU64 tm = 0;
  	__asm
	  {
		  push	edx
  		push	eax
	  	_emit 0x0f
  		_emit 0x31
	  	mov		dword ptr [tm],eax
  		mov		dword ptr [tm+4],edx
  		pop eax
  		pop edx
  	}
    return tm;
#else
     unsigned int a, d;
     asm("cpuid");
     asm volatile("rdtsc" : "=a" (a), "=d" (d));

     return (((NxU64)a) | (((NxU64)d) << 32));

#endif
  }

  void Begin(void)
  {
    mTime = getRTDSC();
  };

  NxU64 End(void)
  {
    NxU64 etime = getRTDSC();
    return etime-mTime;
  }

private:
  NxU64   mTime;
};


#endif
