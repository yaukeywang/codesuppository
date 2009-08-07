#pragma once
#ifndef _GRANNYBASE_RANDOMNUMBERS_H
#define _GRANNYBASE_RANDOMNUMBERS_H

#include "NxSimpleTypes.h"

#ifndef NX_MAX_I64
#define NX_MAX_I64      0x7fffffffffffffffLL
#endif

#ifndef NX_MAX_U64
#define NX_MAX_U64      0xffffffffffffffffLL
#endif

#ifndef NX_MIN_U64
#define NX_MIN_U64      0
#endif

#ifndef NX_MIN_I64
#define NX_MIN_I64     0x8000000000000000LL
#endif


namespace Simutronics 
{

class RandomNumbers 
{
  public:
    static NxF32 get_float(NxF32 hi = 1.0f,NxF32 lo = 0.0f);
    static NxI32 get_int(NxI32 hi=NX_MAX_I32,NxI32 lo=0);
    static NxU32 get_uint(NxU32 hi=NX_MAX_U32,NxU32 lo=0);
    static NxI64 get_int64(NxI64 hi=NX_MAX_I64,NxI64 lo=0);
    static NxU64 get_uint64(NxU64 hi=NX_MAX_U64,NxU64 lo=0 );
    static NxI32 get_int_inclusive(NxI32 hi,NxI32 lo);
    static NxU32 get_uint_inclusive(NxU32 hi,NxU32 lo);
    static NxI64 get_int64_inclusive(NxI64 hi,NxI64 lo);
    static NxU64 get_uint64_inclusive(NxU64 hi,NxU64 lo);
};

} // end of simutronics namespace

#endif
