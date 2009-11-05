#ifndef NV_INTRINSICS_H
#define NV_INTRINSICS_H

// this file is for internal intrinsics - that is, intrinsics that are used in
// cross platform code but do not appear in the API

#ifndef __CELLOS_LV2__
	#error "This file should only be included by ps3 builds!!"
#endif

#include "UserMemAlloc.h"

#pragma intrinsic(_BitScanForward)
namespace NVSHARE
{
	inline unsigned int lowestSetBit32(unsigned int v)
	{
		static const NxU32 MultiplyDeBruijnBitPosition[32] = 
		{
			0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
			31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
		};
		
		return MultiplyDeBruijnBitPosition[((v & -v) * 0x077CB531UL) >> 27];
	}
}


#endif
