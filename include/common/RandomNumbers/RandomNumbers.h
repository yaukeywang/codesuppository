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
    static HeI32 get_int_inclusive(HeI32 hi,HeI32 lo);
    static HeU32 get_uint_inclusive(HeU32 hi,HeU32 lo);
    static HeI64 get_int64_inclusive(HeI64 hi,HeI64 lo);
    static HeU64 get_uint64_inclusive(HeU64 hi,HeU64 lo);
};

} // end of simutronics namespace

#endif
