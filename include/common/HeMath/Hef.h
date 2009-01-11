#ifndef HE_FOUNDATION_NXF
#define HE_FOUNDATION_NXF

#include "../snippets/He.h"

#ifndef HE_FOUNDATION_USE_F64

typedef HeF32 HeReal;

#define HePi	HePiF32
#define HeHalfPi HeHalfPiF32
#define HeTwoPi HeTwoPiF32
#define HeInvPi HeInvPiF32

#define	HE_MAX_REAL			HE_MAX_F32
#define	HE_MIN_REAL			HE_MIN_F32
#define HE_EPS_REAL			HE_EPS_F32

#else

typedef HeF64 HeReal;

#define HePi	HePiF64
#define HeHalfPi HeHalfPiF64
#define HeTwoPi HeTwoPiF64
#define HeInvPi HeInvPiF64

#define	HE_MAX_REAL			HE_MAX_F64
#define	HE_MIN_REAL			HE_MIN_F64
#define HE_EPS_REAL			HE_EPS_F64
#endif

#endif
