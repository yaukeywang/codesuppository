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

#include "../../include/common/snippets/UserMemAlloc.h"

class Cycle
{
public:
  Cycle(void)
  {
    mTime = 0;
  }

  static HeU64 getRTDSC(void)
  {
    HeU64 tm = 0;
#if WIN32
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
#endif
    return tm;
  }

  void Begin(void)
  {
    mTime = getRTDSC();
  };

  HeU64 End(void)
  {
    HeU64 delta = 0;

    HeU64 etime = getRTDSC();
    return etime-mTime;
  }

private:
  HeU64   mTime;
};


#endif
