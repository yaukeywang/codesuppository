#ifndef NV_INTRINSICS_H
#define NV_INTRINSICS_H

#include <PPCIntrinsics.h>
// this file is for internal intrinsics - that is, intrinsics that are used in
// cross platform code but do not appear in the API

#ifndef _XBOX
	#error "This file should only be included by xbox builds!!"
#endif

#include "UserMemAlloc.h"

namespace NVSHARE
{
	inline unsigned int lowestSetBit32(unsigned int v)
	{
		static const NxU32 MultiplyDeBruijnBitPosition[32] = 
		{
			0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
			31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
		};
		int w = v;
		return MultiplyDeBruijnBitPosition[((w & -w) * 0x077CB531UL) >> 27];
	}
}


#endif
