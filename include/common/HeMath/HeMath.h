#ifndef HE_FOUNDATION_NXMATH
#define HE_FOUNDATION_NXMATH

#include <math.h>
#include <float.h>
#include <stdlib.h>	//for rand()

#ifdef _XBOX
#include <ppcintrinsics.h> //for fpmin,fpmax, sqrt etc
#endif

#include "../snippets/He.h"

#ifdef log2
#undef log2
#endif

//constants
static const HeF64 HePiF64		= 3.141592653589793;
static const HeF64 HeHalfPiF64	= 1.57079632679489661923;
static const HeF64 HeTwoPiF64	= 6.28318530717958647692;
static const HeF64 HeInvPiF64	= 0.31830988618379067154;
//we can get bad range checks if we use double prec consts to check single prec results.
static const HeF32 HePiF32		= 3.141592653589793f;
static const HeF32 HeHalfPiF32	= 1.57079632679489661923f;
static const HeF32 HeTwoPiF32	= 6.28318530717958647692f;
static const HeF32 HeInvPiF32	= 0.31830988618379067154f;



/**
\brief Static class with stateless scalar math routines.
*/
class HeMath
	{
	public:

// Type conversion and rounding
		/**
		\brief Returns true if the two numbers are within eps of each other.
		*/
		HE_INLINE static bool equals(HeF32,HeF32,HeF32 eps);

		/**
		\brief Returns true if the two numbers are within eps of each other.
		*/
		HE_INLINE static bool equals(HeF64,HeF64,HeF64 eps);
		/**
		\brief The floor function returns a floating-point value representing the largest integer that is less than or equal to x.
		*/
		HE_INLINE static HeF32 floor(HeF32);
		/**
		\brief The floor function returns a floating-point value representing the largest integer that is less than or equal to x.
		*/
		HE_INLINE static HeF64 floor(HeF64);


		/**
		\brief The ceil function returns a single value representing the smallest integer that is greater than or equal to x. 
		*/
		HE_INLINE static HeF32 ceil(HeF32);
		/**
		\brief The ceil function returns a double value representing the smallest integer that is greater than or equal to x. 
		*/
		HE_INLINE static HeF64 ceil(HeF64);

		/**
		\brief Truncates the float to an integer.
		*/
		HE_INLINE static HeI32 trunc(HeF32);
		/**
		\brief Truncates the double precision float to an integer.
		*/
		HE_INLINE static HeI32 trunc(HeF64);


		/**
		\brief abs returns the absolute value of its argument. 
		*/
		HE_INLINE static HeF32 abs(HeF32);
		/**
		\brief abs returns the absolute value of its argument. 
		*/
		HE_INLINE static HeF64 abs(HeF64);
		/**
		\brief abs returns the absolute value of its argument.
		*/
		HE_INLINE static HeI32 abs(HeI32);


		/**
		\brief sign returns the sign of its argument. The sign of zero is undefined.
		*/
		HE_INLINE static HeF32 sign(HeF32);
		/**
		\brief sign returns the sign of its argument. The sign of zero is undefined.
		*/
		HE_INLINE static HeF64 sign(HeF64);
		/**
		\brief sign returns the sign of its argument. The sign of zero is undefined.
		*/
		HE_INLINE static HeI32 sign(HeI32);


		/**
		\brief The return value is the greater of the two specified values. 
		*/
		HE_INLINE static HeF32 vmax(HeF32,HeF32);
		/**
		\brief The return value is the greater of the two specified values.
		*/
		HE_INLINE static HeF64 vmax(HeF64,HeF64);
		/**
		\brief The return value is the greater of the two specified values. 
		*/
		HE_INLINE static HeI32 vmax(HeI32,HeI32);
		/**
		\brief The return value is the greater of the two specified values. 
		*/
		HE_INLINE static HeU32 vmax(HeU32,HeU32);
		/**
		\brief The return value is the greater of the two specified values. 
		*/
		HE_INLINE static HeU16 vmax(HeU16,HeU16);
		
		
		/**
		\brief The return value is the lesser of the two specified values. 
		*/
		HE_INLINE static HeF32 vmin(HeF32,HeF32);
		/**
		\brief The return value is the lesser of the two specified values. 
		*/
		HE_INLINE static HeF64 vmin(HeF64,HeF64);
		/**
		\brief The return value is the lesser of the two specified values. 
		*/
		HE_INLINE static HeI32 vmin(HeI32,HeI32);
		/**
		\brief The return value is the lesser of the two specified values. 
		*/
		HE_INLINE static HeU32 vmin(HeU32,HeU32);
		/**
		\brief The return value is the lesser of the two specified values. 
		*/
		HE_INLINE static HeU16 vmin(HeU16,HeU16);
		
		/**
		\brief mod returns the floating-point remainder of x / y. 
		
		If the value of y is 0.0, mod returns a quiet NaN.
		*/
		HE_INLINE static HeF32 mod(HeF32 x, HeF32 y);
		/**
		\brief mod returns the floating-point remainder of x / y. 
		
		If the value of y is 0.0, mod returns a quiet NaN.
		*/
		HE_INLINE static HeF64 mod(HeF64 x, HeF64 y);

		/**
		\brief Clamps v to the range [hi,lo]
		*/
		HE_INLINE static HeF32 clamp(HeF32 v, HeF32 hi, HeF32 low);
		/**
		\brief Clamps v to the range [hi,lo]
		*/
		HE_INLINE static HeF64 clamp(HeF64 v, HeF64 hi, HeF64 low);
		/**
		\brief Clamps v to the range [hi,lo]
		*/
		HE_INLINE static HeU32 clamp(HeU32 v, HeU32 hi, HeU32 low);
		/**
		\brief Clamps v to the range [hi,lo]
		*/
		HE_INLINE static HeI32 clamp(HeI32 v, HeI32 hi, HeI32 low);

		//!powers
		/**
		\brief Square root.
		*/
		HE_INLINE static HeF32 sqrt(HeF32);
		/**
		\brief Square root.
		*/
		HE_INLINE static HeF64 sqrt(HeF64);
		
		/**
		\brief reciprocal square root.
		*/
		HE_INLINE static HeF32 recipSqrt(HeF32);
		/**
		\brief reciprocal square root.
		*/
		HE_INLINE static HeF64 recipSqrt(HeF64);
		
		/**
		\brief Calculates x raised to the power of y.
		*/
		HE_INLINE static HeF32 pow(HeF32 x, HeF32 y);
		/**
		\brief Calculates x raised to the power of y.
		*/
		HE_INLINE static HeF64 pow(HeF64 x, HeF64 y);
		
		
		/**
		\brief Calculates e^n
		*/
		HE_INLINE static HeF32 exp(HeF32);
		/**
		\brief Calculates e^n
		*/
		HE_INLINE static HeF64 exp(HeF64);
		
		/**
		\brief Calculates logarithms.
		*/
		HE_INLINE static HeF32 logE(HeF32);
		/**
		\brief Calculates logarithms.
		*/
		HE_INLINE static HeF64 logE(HeF64);
		/**
		\brief Calculates logarithms.
		*/
		HE_INLINE static HeF32 log2(HeF32);
		/**
		\brief Calculates logarithms.
		*/
		HE_INLINE static HeF64 log2(HeF64);
		/**
		\brief Calculates logarithms.
		*/
		HE_INLINE static HeF32 log10(HeF32);
		/**
		\brief Calculates logarithms.
		*/
		HE_INLINE static HeF64 log10(HeF64);

		//!trigonometry -- all angles are in radians.
		
		/**
		\brief Converts degrees to radians.
		*/
		HE_INLINE static HeF32 degToRad(HeF32);
		/**
		\brief Converts degrees to radians.
		*/
		HE_INLINE static HeF64 degToRad(HeF64);

		/**
		\brief Converts radians to degrees.
		*/
		HE_INLINE static HeF32 radToDeg(HeF32);
		/**
		\brief Converts radians to degrees.
		*/
		HE_INLINE static HeF64 radToDeg(HeF64);

		/**
		\brief Sine of an angle.

		<b>Unit:</b> Radians
		*/
		HE_INLINE static HeF32 sin(HeF32);
		/**
		\brief Sine of an angle.

		<b>Unit:</b> Radians
		*/
		HE_INLINE static HeF64 sin(HeF64);
		
		/**
		\brief Cosine of an angle.

		<b>Unit:</b> Radians
		*/
		HE_INLINE static HeF32 cos(HeF32);
		/**
		\brief Cosine of an angle.

		<b>Unit:</b> Radians
		*/
		HE_INLINE static HeF64 cos(HeF64);

		/**
		\brief Computes both the sin and cos.

		<b>Unit:</b> Radians
		*/
		HE_INLINE static void sinCos(HeF32, HeF32 & sin, HeF32 & cos);

		/**
		\brief Computes both the sin and cos.

		<b>Unit:</b> Radians
		*/
		HE_INLINE static void sinCos(HeF64, HeF64 & sin, HeF64 & cos);


		/**
		\brief Tangent of an angle.

		<b>Unit:</b> Radians
		*/
		HE_INLINE static HeF32 tan(HeF32);
		/**
		\brief Tangent of an angle.

		<b>Unit:</b> Radians
		*/
		HE_INLINE static HeF64 tan(HeF64);
		
		/**
		\brief Arcsine.
		
		Returns angle between -PI/2 and PI/2 in radians

		<b>Unit:</b> Radians
		*/
		HE_INLINE static HeF32 asin(HeF32);
		/**
		\brief Arcsine.
		
		Returns angle between -PI/2 and PI/2 in radians

		<b>Unit:</b> Radians
		*/
		HE_INLINE static HeF64 asin(HeF64);
		
		/**
		\brief Arccosine.
		
		Returns angle between 0 and PI in radians

		<b>Unit:</b> Radians
		*/
		HE_INLINE static HeF32 acos(HeF32);
		
		/**
		\brief Arccosine.
		
		Returns angle between 0 and PI in radians

		<b>Unit:</b> Radians
		*/
		HE_INLINE static HeF64 acos(HeF64);
		
		/**
		\brief ArcTangent.
		
		Returns angle between -PI/2 and PI/2 in radians

		<b>Unit:</b> Radians
		*/
		HE_INLINE static HeF32 atan(HeF32);
		/**
		\brief ArcTangent.
		
		Returns angle between -PI/2 and PI/2 in radians

		<b>Unit:</b> Radians
		*/
		HE_INLINE static HeF64 atan(HeF64);

		/**
		\brief Arctangent of (x/y) with correct sign.
		
		Returns angle between -PI and PI in radians

		<b>Unit:</b> Radians
		*/
		HE_INLINE static HeF32 atan2(HeF32 x, HeF32 y);
		/**
		\brief Arctangent of (x/y) with correct sign.

		Returns angle between -PI and PI in radians

		<b>Unit:</b> Radians
		*/
		HE_INLINE static HeF64 atan2(HeF64 x, HeF64 y);

		//random numbers

		/**
		\brief uniform random number in [a,b]
		*/
		HE_INLINE static HeF32 rand(HeF32 a,HeF32 b);

		/**
		\brief uniform random number in [a,b]
		*/
		HE_INLINE static HeI32 rand(HeI32 a,HeI32 b);

		/**
		\brief hashing: hashes an array of n 32 bit values	to a 32 bit value.

		Because the output bits are uniformly distributed, the caller may mask
		off some of the bits to index into a hash table	smaller than 2^32.
		*/
		HE_INLINE static HeU32 hash(const HeU32 * array, HeU32 n);

		/**
		\brief hash32
		*/
		HE_INLINE static int hash32(int);

		/**
		\brief returns true if the passed number is a finite floating point number as opposed to INF, NAN, etc.
		*/
		HE_INLINE static bool isFinite(HeF32 x);

		/**
		\brief returns true if the passed number is a finite floating point number as opposed to INF, NAN, etc.
		*/
		HE_INLINE static bool isFinite(HeF64 x);

	};

/*
Many of these are just implemented as HE_INLINE calls to the C lib right now,
but later we could replace some of them with some approximations or more
clever stuff.
*/
HE_INLINE bool HeMath::equals(HeF32 a,HeF32 b,HeF32 eps)
	{
	const HeF32 diff = HeMath::abs(a - b);
	return (diff < eps);
	}

HE_INLINE bool HeMath::equals(HeF64 a,HeF64 b,HeF64 eps)
	{
	const HeF64 diff = HeMath::abs(a - b);
	return (diff < eps);
	}

HE_INLINE HeF32 HeMath::floor(HeF32 a)
	{
	return ::floorf(a);
	}

HE_INLINE HeF64 HeMath::floor(HeF64 a)
	{
	return ::floor(a);
	}

HE_INLINE HeF32 HeMath::ceil(HeF32 a)
	{
	return ::ceilf(a);
	}

HE_INLINE HeF64 HeMath::ceil(HeF64 a)
	{
	return ::ceil(a);
	}

HE_INLINE HeI32 HeMath::trunc(HeF32 a)
	{
	return (HeI32) a;	// ### PT: this actually depends on FPU settings
	}

HE_INLINE HeI32 HeMath::trunc(HeF64 a)
	{
	return (HeI32) a;	// ### PT: this actually depends on FPU settings
	}

HE_INLINE HeF32 HeMath::abs(HeF32 a)
	{
	return ::fabsf(a);
	}

HE_INLINE HeF64 HeMath::abs(HeF64 a)
	{
	return ::fabs(a);
	}

HE_INLINE HeI32 HeMath::abs(HeI32 a)
	{
	return ::abs(a);
	}

HE_INLINE HeF32 HeMath::sign(HeF32 a)
	{
	return (a >= 0.0f) ? 1.0f : -1.0f;
	}

HE_INLINE HeF64 HeMath::sign(HeF64 a)
	{
	return (a >= 0.0) ? 1.0 : -1.0;
	}

HE_INLINE HeI32 HeMath::sign(HeI32 a)
	{
	return (a >= 0) ? 1 : -1;
	}

HE_INLINE HeF32 HeMath::vmax(HeF32 a,HeF32 b)
	{
#ifdef _XBOX
	return (HeF32)fpmax(a, b);
#else
	return (a < b) ? b : a;
#endif
	}

HE_INLINE HeF64 HeMath::vmax(HeF64 a,HeF64 b)
	{
#ifdef _XBOX
	return (HeF64)fpmax(a, b);
#else
	return (a < b) ? b : a;
#endif
	}

HE_INLINE HeI32 HeMath::vmax(HeI32 a,HeI32 b)
	{
	return (a < b) ? b : a;
	}

HE_INLINE HeU32 HeMath::vmax(HeU32 a,HeU32 b)
	{
	return (a < b) ? b : a;
	}

HE_INLINE HeU16 HeMath::vmax(HeU16 a,HeU16 b)
	{
	return (a < b) ? b : a;
	}

HE_INLINE HeF32 HeMath::vmin(HeF32 a,HeF32 b)
	{
#ifdef _XBOX
	return (HeF32)fpmin(a, b);
#else
	return (a < b) ? a : b;
#endif
	}

HE_INLINE HeF64 HeMath::vmin(HeF64 a,HeF64 b)
	{
#ifdef _XBOX
	return (HeF64)fpmin(a, b);
#else
	return (a < b) ? a : b;
#endif
	}

HE_INLINE HeI32 HeMath::vmin(HeI32 a,HeI32 b)
	{
	return (a < b) ? a : b;
	}

HE_INLINE HeU32 HeMath::vmin(HeU32 a,HeU32 b)
	{
	return (a < b) ? a : b;
	}

HE_INLINE HeU16 HeMath::vmin(HeU16 a,HeU16 b)
	{
	return (a < b) ? a : b;
	}

HE_INLINE HeF32 HeMath::mod(HeF32 x, HeF32 y)
	{
	return (HeF32)::fmod(x,y);
	}

HE_INLINE HeF64 HeMath::mod(HeF64 x, HeF64 y)
	{
	return ::fmod(x,y);
	}

HE_INLINE HeF32 HeMath::clamp(HeF32 v, HeF32 hi, HeF32 low)
	{
	if (v > hi) 
		return hi;
	else if (v < low) 
		return low;
	else
		return v;
	}

HE_INLINE HeF64 HeMath::clamp(HeF64 v, HeF64 hi, HeF64 low)
	{
	if (v > hi) 
		return hi;
	else if (v < low) 
		return low;
	else
		return v;
	}

HE_INLINE HeU32 HeMath::clamp(HeU32 v, HeU32 hi, HeU32 low)
	{
	if (v > hi) 
		return hi;
	else if (v < low) 
		return low;
	else
		return v;
	}

HE_INLINE HeI32 HeMath::clamp(HeI32 v, HeI32 hi, HeI32 low)
	{
	if (v > hi) 
		return hi;
	else if (v < low) 
		return low;
	else
		return v;
	}

HE_INLINE HeF32 HeMath::sqrt(HeF32 a)
	{
	return ::sqrtf(a);
	}

HE_INLINE HeF64 HeMath::sqrt(HeF64 a)
	{
	return ::sqrt(a);
	}

HE_INLINE HeF32 HeMath::recipSqrt(HeF32 a)
	{
	return 1.0f/::sqrtf(a);
	}

HE_INLINE HeF64 HeMath::recipSqrt(HeF64 a)
	{
	return 1.0/::sqrt(a);
	}

HE_INLINE HeF32 HeMath::pow(HeF32 x, HeF32 y)
	{
	return ::powf(x,y);
	}

HE_INLINE HeF64 HeMath::pow(HeF64 x, HeF64 y)
	{
	return ::pow(x,y);
	}

HE_INLINE HeF32 HeMath::exp(HeF32 a)
	{
	return ::expf(a);
	}

HE_INLINE HeF64 HeMath::exp(HeF64 a)
	{
	return ::exp(a);
	}

HE_INLINE HeF32 HeMath::logE(HeF32 a)
	{
	return ::logf(a);
	}

HE_INLINE HeF64 HeMath::logE(HeF64 a)
	{
	return ::log(a);
	}

HE_INLINE HeF32 HeMath::log2(HeF32 a)
	{
	const HeF32 ln2 = (HeF32)0.693147180559945309417;
    return ::logf(a) / ln2;
	}

HE_INLINE HeF64 HeMath::log2(HeF64 a)
	{
	const HeF64 ln2 = (HeF64)0.693147180559945309417;
    return ::log(a) / ln2;
	}

HE_INLINE HeF32 HeMath::log10(HeF32 a)
	{
	return (HeF32)::log10(a);
	}

HE_INLINE HeF64 HeMath::log10(HeF64 a)
	{
	return ::log10(a);
	}

HE_INLINE HeF32 HeMath::degToRad(HeF32 a)
	{
	return (HeF32)0.01745329251994329547 * a;
	}

HE_INLINE HeF64 HeMath::degToRad(HeF64 a)
	{
	return (HeF64)0.01745329251994329547 * a;
	}

HE_INLINE HeF32 HeMath::radToDeg(HeF32 a)
	{
	return (HeF32)57.29577951308232286465 * a;
	}

HE_INLINE HeF64 HeMath::radToDeg(HeF64 a)
	{
	return (HeF64)57.29577951308232286465 * a;
	}

HE_INLINE HeF32 HeMath::sin(HeF32 a)
	{
	return ::sinf(a);
	}

HE_INLINE HeF64 HeMath::sin(HeF64 a)
	{
	return ::sin(a);
	}

HE_INLINE HeF32 HeMath::cos(HeF32 a)
	{
	return ::cosf(a);
	}

HE_INLINE HeF64 HeMath::cos(HeF64 a)
	{
	return ::cos(a);
	}

// Calling fsincos instead of fsin+fcos
HE_INLINE void HeMath::sinCos(HeF32 f, HeF32& s, HeF32& c)
	{
#ifdef WIN32
		HeF32 localCos, localSin;
		HeF32 local = f;
		_asm	fld		local
		_asm	fsincos
		_asm	fstp	localCos
		_asm	fstp	localSin
		c = localCos;
		s = localSin;
#else
		c = cosf(f);
		s = sinf(f);
#endif
	}

HE_INLINE void HeMath::sinCos(HeF64 a, HeF64 & s, HeF64 & c)
	{
	s = ::sin(a);
	c = ::cos(a);
	}

HE_INLINE HeF32 HeMath::tan(HeF32 a)
	{
	return ::tanf(a);
	}

HE_INLINE HeF64 HeMath::tan(HeF64 a)
	{
	return ::tan(a);
	}

HE_INLINE HeF32 HeMath::asin(HeF32 f)
	{
	// Take care of FPU inaccuracies
	if(f>=1.0f)	return (HeF32)HeHalfPiF32;
	if(f<=-1.0f)return -(HeF32)HeHalfPiF32;
				return ::asinf(f);
	}

HE_INLINE HeF64 HeMath::asin(HeF64 f)
	{
	// Take care of FPU inaccuracies
	if(f>=1.0)	return (HeF32)HeHalfPiF64;
	if(f<=-1.0)	return -(HeF32)HeHalfPiF64;
				return ::asin(f);
	}

HE_INLINE HeF32 HeMath::acos(HeF32 f)
	{
	// Take care of FPU inaccuracies
	if(f>=1.0f)	return 0.0f;
	if(f<=-1.0f)return (HeF32)HePiF32;
				return ::acosf(f);
	}

HE_INLINE HeF64 HeMath::acos(HeF64 f)
	{
	// Take care of FPU inaccuracies
	if(f>=1.0)	return 0.0;
	if(f<=-1.0)	return (HeF64)HePiF64;
				return ::acos(f);
	}

HE_INLINE HeF32 HeMath::atan(HeF32 a)
	{
	return ::atanf(a);
	}

HE_INLINE HeF64 HeMath::atan(HeF64 a)
	{
	return ::atan(a);
	}

HE_INLINE HeF32 HeMath::atan2(HeF32 x, HeF32 y)
	{
	return ::atan2f(x,y);
	}

HE_INLINE HeF64 HeMath::atan2(HeF64 x, HeF64 y)
	{
	return ::atan2(x,y);
	}

HE_INLINE HeF32 HeMath::rand(HeF32 a,HeF32 b)
	{
	const HeF32 r = (HeF32)::rand()/((HeF32)RAND_MAX+1);
	return r*(b-a) + a;
	}

HE_INLINE HeI32 HeMath::rand(HeI32 a,HeI32 b)
	{
	return a + (HeI32)(::rand()%(b-a));
	}

/*
--------------------------------------------------------------------
lookup2.c, by Bob Jenkins, December 1996, Public Domain.
hash(), hash2(), hash3, and mix() are externally useful functions.
Routines to test the hash are included if SELF_TEST is defined.
You can use this free for any purpose.  It has no warranty.
--------------------------------------------------------------------
--------------------------------------------------------------------
mix -- mix 3 32-bit values reversibly.
For every delta with one or two bit set, and the deltas of all three
  high bits or all three low bits, whether the original value of a,b,c
  is almost all zero or is uniformly distributed,
* If mix() is run forward or backward, at least 32 bits in a,b,c
  have at least 1/4 probability of changing.
* If mix() is run forward, every bit of c will change between 1/3 and
  2/3 of the time.  (Well, 22/100 and 78/100 for some 2-bit deltas.)
mix() was built out of 36 single-cycle latency instructions in a 
  structure that could supported 2x parallelism, like so:
      a -= b; 
      a -= c; x = (c>>13);
      b -= c; a ^= x;
      b -= a; x = (a<<8);
      c -= a; b ^= x;
      c -= b; x = (b>>13);
      ...
  Unfortunately, superscalar Pentiums and Sparcs can't take advantage 
  of that parallelism.  They've also turned some of those single-cycle
  latency instructions into multi-cycle latency instructions.  Still,
  this is the fastest good hash I could find.  There were about 2^^68
  to choose from.  I only looked at a billion or so.
--------------------------------------------------------------------
*/
#define HE_HASH_MIX(a,b,c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}

/*
--------------------------------------------------------------------
 This works on all machines.  hash2() is identical to hash() on 
 little-endian machines, except that the length has to be measured
 in ub4s instead of bytes.  It is much faster than hash().  It 
 requires
 -- that the key be an array of ub4's, and
 -- that all your machines have the same endianness, and
 -- that the length be the number of ub4's in the key
--------------------------------------------------------------------
*/
HE_INLINE HeU32 HeMath::hash(const HeU32 *k, HeU32 length)
//register ub4 *k;        /* the key */
//register ub4  length;   /* the length of the key, in ub4s */
	{
	HeU32 a,b,c,len;

	/* Set up the internal state */
	len = length;
	a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
	c = 0;           /* the previous hash value */

	/*---------------------------------------- handle most of the key */
	while (len >= 3)
	{
	  a += k[0];
	  b += k[1];
	  c += k[2];
	  HE_HASH_MIX(a,b,c);
	  k += 3; len -= 3;
	}

	/*-------------------------------------- handle the last 2 ub4's */
	c += length;
	switch(len)              /* all the case statements fall through */
	{
	 /* c is reserved for the length */
	case 2 : b+=k[1];
	case 1 : a+=k[0];
	 /* case 0: nothing left to add */
	}
	HE_HASH_MIX(a,b,c);
	/*-------------------------------------------- report the result */
	return c;
	}
#undef HE_HASH_MIX

HE_INLINE int HeMath::hash32(int key)
	{
	key += ~(key << 15);
	key ^=  (key >> 10);
	key +=  (key << 3);
	key ^=  (key >> 6);
	key += ~(key << 11);
	key ^=  (key >> 16);
	return key;
	}


HE_INLINE bool HeMath::isFinite(HeF32 f)
	{
	#if defined(_MSC_VER)
	return (0 == ((_FPCLASS_SNAN | _FPCLASS_QNAN | _FPCLASS_NINF | _FPCLASS_PINF) & _fpclass(f) ));
	#elif defined(__CELLOS_LV2__)
	return (!(isnan(f)||isinf(f)));
	#else
	return true;
	#endif
	
	}

HE_INLINE bool HeMath::isFinite(HeF64 f)
	{
	#if defined(_MSC_VER)
	return (0 == ((_FPCLASS_SNAN | _FPCLASS_QNAN | _FPCLASS_NINF | _FPCLASS_PINF) & _fpclass(f) ));
	#elif defined(__CELLOS_LV2__)
	return (!(isnan(f)||isinf(f)));
	#else
	return true;
	#endif
	}


#endif
