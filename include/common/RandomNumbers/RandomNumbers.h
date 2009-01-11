#pragma once
#ifndef _GRANNYBASE_RANDOMNUMBERS_H
#define _GRANNYBASE_RANDOMNUMBERS_H

#include "../snippets/HeSimpleTypes.h"

namespace Simutronics 
{

class RandomNumbers 
{
  public:
    static HeF32 get_float(HeF32 hi = 1.0f,HeF32 lo = 0.0f);
    static HeI32 get_int(HeI32 hi=HE_MAX_I32,HeI32 lo=0);
    static HeU32 get_uint(HeU32 hi=HE_MAX_U32,HeU32 lo=0);
    static HeI64 get_int64(HeI64 hi=HE_MAX_I64,HeI64 lo=0);
    static HeU64 get_uint64(HeU64 hi=HE_MAX_U64,HeU64 lo=0 );
};

} // end of simutronics namespace

#endif
