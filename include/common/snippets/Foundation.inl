// Contains source code to various header files.

#ifndef NX_FOUNDATION_NX
#define NX_FOUNDATION_NX
/*----------------------------------------------------------------------------*\
|
|						Public Interface to Ageia PhysX Technology
|
|							     www.ageia.com
|
\*----------------------------------------------------------------------------*/

/** \addtogroup foundation
  @{
*/

/**
DLL export macros
*/
#ifndef NX_C_EXPORT
	#define NX_C_EXPORT extern "C"
#endif

#ifndef NX_CALL_CONV
	#if defined WIN32
		#define NX_CALL_CONV __cdecl
	#elif defined LINUX
		#define NX_CALL_CONV
    #elif defined __APPLE__
        #define NX_CALL_CONV
	#elif defined __CELLOS_LV2__
		#define NX_CALL_CONV
	#elif defined _XBOX
        #define NX_CALL_CONV
	#elif defined(__PPCGEKKO__)
        #define NX_CALL_CONV
	#else
		#error custom definition of NX_CALL_CONV for your OS needed!
	#endif
#endif

#if	  defined NX32
#elif defined NX64
#elif defined _WIN64
	#ifdef NX32
		#error PhysX SDK: Platforms pointer size ambiguous!  The defines WIN64 and NX32 are in conflict.
	#endif
	#define NX64
#elif defined WIN32
	#ifdef NX64
		#error PhysX SDK: Platforms pointer size ambiguous!  The defines WIN32 and NX64 are in conflict.
	#endif
	#define NX32
#elif defined __CELLOS_LV2__
	#ifdef __LP32__
            #define NX32
    #else
            #define NX64
    #endif
#elif defined _XBOX
	#define NX32
#elif defined LINUX
        #define NX32
#elif defined(__PPCGEKKO__)
	#define NX32
#else
	#error PhysX SDK: Platforms pointer size ambiguous.  Please define NX32 or Nx64 in the compiler settings!
#endif


#if !defined __CELLOS_LV2__
	#define NX_COMPILE_TIME_ASSERT(exp)	extern char NX_CompileTimeAssert[ size_t((exp) ? 1 : -1) ]
#elif defined(__PPCGEKKO__)
	#define NX_COMPILE_TIME_ASSERT(exp)	typedef char NX_CompileTimeAssert[ (exp) ? 1 : -1 ]
#else
    // GCC4 don't like the line above
	#define _CELL_NX_COMPILE_TIME_NAME_(x) NX_CompileTimeAssert ## x
	#define _CELL_NX_COMPILE_TIME_NAME(x) _CELL_NX_COMPILE_TIME_NAME_(x)
	#define NX_COMPILE_TIME_ASSERT(exp) extern char _CELL_NX_COMPILE_TIME_NAME(__LINE__)[ (exp) ? 1 : -1]
#endif


#if _MSC_VER
	#define NX_MSVC		// Compiling with VC++
	#if _MSC_VER >= 1400
		#define NX_VC8
	#elif _MSC_VER >= 1300
		#define NX_VC7		// Compiling with VC7
	#else
		#define NX_VC6		// Compiling with VC6
		#define __FUNCTION__	"Undefined"
	#endif
#endif

/**
 Nx SDK misc defines.
*/

#define	NX_UNREFERENCED_PARAMETER(P) (P)

//NX_INLINE
#if (_MSC_VER>=1000)
	#define NX_INLINE __forceinline	//alternative is simple inline
	#pragma inline_depth( 255 )

	#include <string.h>
	#include <stdlib.h>
	#pragma intrinsic(memcmp)
	#pragma intrinsic(memcpy)
	#pragma intrinsic(memset)
	#pragma intrinsic(strcat)
	#pragma intrinsic(strcmp)
	#pragma intrinsic(strcpy)
	#pragma intrinsic(strlen)
	#pragma intrinsic(abs)
	#pragma intrinsic(labs)
#elif defined(__MWERKS__)
	//optional: #pragma always_inline on
	#define NX_INLINE inline
#else
	#define NX_INLINE inline
#endif

	#define NX_DELETE(x)	delete x
	#define NX_DELETE_SINGLE(x)	if (x) { delete x;		x = NULL; }
	#define NX_DELETE_ARRAY(x)	if (x) { delete []x;	x = NULL; }

	template<class Type> NX_INLINE void NX_Swap(Type& a, Type& b)
		{
		const Type c = a; a = b; b = c;
		}

/**
\brief Error codes

These error codes are passed to #NxUserOutputStream

@see NxUserOutputStream
*/

enum NxErrorCode
	{
	/**
	\brief no error
	*/
	NXE_NO_ERROR			= 0,
	/**
	\brief method called with invalid parameter(s)
	*/
	NXE_INVALID_PARAMETER	= 1,
	/**
	\brief method was called at a time when an operation is not possible
	*/
	NXE_INVALID_OPERATION	= 2,
	/**
	\brief method failed to allocate some memory
	*/
	NXE_OUT_OF_MEMORY		= 3,
	/**
	\brief The library failed for some reason.
	
	Usually because you have passed invalid values like NaNs into the system, which are not checked for.
	*/
	NXE_INTERNAL_ERROR		= 4,

	/**
	\brief an assertion failed.
	*/
	NXE_ASSERTION			= 107,

	/**
	\brief An informational message.

	Only emitted when NX_USER_DEBUG_MODE is defined.
	*/
	NXE_DB_INFO				= 205,
	/**
	\brief a warning message for the user to help with debugging

	Only emitted when NX_USER_DEBUG_MODE is defined.
	*/
	NXE_DB_WARNING			= 206,
	/**
	\brief the message should simply be printed without any additional infos (line number, etc).

	Only emitted when NX_USER_DEBUG_MODE is defined.
	*/
	NXE_DB_PRINT			= 208

	};

/**
\brief These errors are returned by the NxCreatePhysicsSDK() function
*/
enum NxSDKCreateError
	{
	/**
	\brief No errors occurred when creating the Physics SDK.
	*/
	NXCE_NO_ERROR = 0,

	/**
	\brief Unable to find the PhysX libraries. The PhysX drivers are not installed correctly.
	*/
	NXCE_PHYSX_NOT_FOUND = 1,

	//wrong version of the SDK is being used
	/**
	\brief The application supplied a version number that does not match with the libraries.
	*/
	NXCE_WRONG_VERSION = 2,

	/**
	\brief The supplied SDK descriptor is invalid.
	*/
	NXCE_DESCRIPTOR_INVALID = 3,

	/**
	\brief A PhysX card was found, but there are problems when communicating with the card.
	*/
	NXCE_CONNECTION_ERROR = 4,

	/**
	\brief A PhysX card was found, but it did not reset (or initialize) properly.
	*/
	NXCE_RESET_ERROR = 5,

	/**
	\brief A PhysX card was found, but it is already in use by another application.
	*/
	NXCE_IN_USE_ERROR = 6,

	/**
	\brief A PhysX card was found, but there are issues with loading the firmware.
	*/
	NXCE_BUNDLE_ERROR = 7

	};

#if _MSC_VER
//get rid of browser info warnings 
#pragma warning( disable : 4786 )  //identifier was truncated to '255' characters in the debug information
#pragma warning( disable : 4251 )  //class needs to have dll-interface to be used by clients of class
#endif

//files to always include:
#ifdef LINUX
#include <time.h>
#include <string.h>
#include <stdlib.h>
#elif __APPLE__
#include <time.h>
#elif __CELLOS_LV2__
	#include <string.h>
#elif defined(__PPCGEKKO__)
	#include <dolphin.h>
	#include <string.h>
	#include <stdlib.h>
#endif

#define	NX_SIGN_BITMASK		0x80000000

#define NX_DEBUG_MALLOC 0

// Don't use inline for alloca !!!
#ifdef WIN32
	#include <malloc.h>
	#define NxAlloca(x)	_alloca(x)
#elif LINUX
	#include <malloc.h>
	#define NxAlloca(x)	alloca(x)
#elif __APPLE__
	#include <alloca.h>
	#include <stdlib.h>
	#define NxAlloca(x)	alloca(x)
#elif __CELLOS_LV2__
	#include <alloca.h>
	#include <stdlib.h>
	#define NxAlloca(x)	alloca(x)
#elif _XBOX
	#include <malloc.h>
	#define NxAlloca(x)	_alloca(x)
#elif defined(__PPCGEKKO__)
	#include <alloca.h>
	#define NxAlloca(x)	alloca(x)
#endif

/**
\brief Used to specify a thread priority.
*/
enum NxThreadPriority 
{
	/**
	\brief High priority
	*/
	NX_TP_HIGH		    =0,

    /**
	\brief Above Normal priority
	*/
	NX_TP_ABOVE_NORMAL  =1,

	/**
	\brief Normal/default priority
	*/
	NX_TP_NORMAL	    =2,

    /**
	\brief Below Normal priority
	*/
    NX_TP_BELOW_NORMAL  =3,

	/**
	\brief Low priority.
	*/
	NX_TP_LOW		    =4,

	NX_TP_FORCE_DWORD = 0xffFFffFF
};

 /** @} */
#endif
//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND

#ifndef NX_FOUNDATION_NXSIMPLETYPES
#define NX_FOUNDATION_NXSIMPLETYPES
/*----------------------------------------------------------------------------*\
|
|						Public Interface to Ageia PhysX Technology
|
|							     www.ageia.com
|
\*----------------------------------------------------------------------------*/
/** \addtogroup foundation
  @{
*/

// Platform specific types:
//Design note: Its OK to use int for general loop variables and temps.

#ifdef WIN32
	typedef __int64				NxI64;
	typedef signed int			NxI32;
	typedef signed short		NxI16;
	typedef signed char			NxI8;

	typedef unsigned __int64	NxU64;
	typedef unsigned int		NxU32;
	typedef unsigned short		NxU16;
	typedef unsigned char		NxU8;

	typedef float				NxF32;
	typedef double				NxF64;

#elif LINUX
	typedef long long			NxI64;
	typedef signed int			NxI32;
	typedef signed short		NxI16;
	typedef signed char			NxI8;

	typedef unsigned long long	NxU64;
	typedef unsigned int		NxU32;
	typedef unsigned short		NxU16;
	typedef unsigned char		NxU8;

	typedef float				NxF32;
	typedef double				NxF64;

#elif __APPLE__
	typedef long long			NxI64;
	typedef signed int			NxI32;
	typedef signed short		NxI16;
	typedef signed char			NxI8;

	typedef unsigned long long	NxU64;
	typedef unsigned int		NxU32;
	typedef unsigned short		NxU16;
	typedef unsigned char		NxU8;

	typedef float				NxF32;
	typedef double				NxF64;

#elif __CELLOS_LV2__
	typedef long long			NxI64;
	typedef signed int			NxI32;
	typedef signed short		NxI16;
	typedef signed char			NxI8;

	typedef unsigned long long	NxU64;
	typedef unsigned int		NxU32;
	typedef unsigned short		NxU16;
	typedef unsigned char		NxU8;

	typedef float				NxF32;
	typedef double				NxF64;

#elif _XBOX
	typedef __int64				NxI64;
	typedef signed int			NxI32;
	typedef signed short		NxI16;
	typedef signed char			NxI8;

	typedef unsigned __int64	NxU64;
	typedef unsigned int		NxU32;
	typedef unsigned short		NxU16;
	typedef unsigned char		NxU8;

	typedef float				NxF32;
	typedef double				NxF64;
    
#elif defined(__PPCGEKKO__)
	typedef long long			NxI64;
	typedef signed int			NxI32;
	typedef signed short		NxI16;
	typedef signed char			NxI8;

	typedef unsigned long long	NxU64;
	typedef unsigned int		NxU32;
	typedef unsigned short		NxU16;
	typedef unsigned char		NxU8;

	typedef float				NxF32;
	typedef double				NxF64;

#else
	#error Unknown platform!
#endif

union NxU32F32
{
	NxU32 u;
	NxF32 f;
};

#if __APPLE__
    NX_COMPILE_TIME_ASSERT(sizeof(bool)==4);    // PPC has 4 byte bools
#else
	NX_COMPILE_TIME_ASSERT(sizeof(bool)==1);	// ...otherwise things might fail with VC++ 4.2 !
#endif
	NX_COMPILE_TIME_ASSERT(sizeof(NxI8)==1);
	NX_COMPILE_TIME_ASSERT(sizeof(NxU8)==1);
	NX_COMPILE_TIME_ASSERT(sizeof(NxI16)==2);
	NX_COMPILE_TIME_ASSERT(sizeof(NxU16)==2);
	NX_COMPILE_TIME_ASSERT(sizeof(NxI32)==4);
	NX_COMPILE_TIME_ASSERT(sizeof(NxU32)==4);
	NX_COMPILE_TIME_ASSERT(sizeof(NxI64)==8);
	NX_COMPILE_TIME_ASSERT(sizeof(NxU64)==8);
#if defined(NX64)
	NX_COMPILE_TIME_ASSERT(sizeof(void*)==8);
#else
	NX_COMPILE_TIME_ASSERT(sizeof(void*)==4);
#endif

	// Type ranges
	#define	NX_MAX_I8			0x7f			//max possible sbyte value
	#define	NX_MIN_I8			0x80			//min possible sbyte value
	#define	NX_MAX_U8			0xff			//max possible ubyte value
	#define	NX_MIN_U8			0x00			//min possible ubyte value
	#define	NX_MAX_I16			0x7fff			//max possible sword value
	#define	NX_MIN_I16			0x8000			//min possible sword value
	#define	NX_MAX_U16			0xffff			//max possible uword value
	#define	NX_MIN_U16			0x0000			//min possible uword value
	#define	NX_MAX_I32			0x7fffffff		//max possible sdword value
	#define	NX_MIN_I32			0x80000000		//min possible sdword value
	#define	NX_MAX_U32			0xffffffff		//max possible udword value
	#define	NX_MIN_U32			0x00000000		//min possible udword value
	#define	NX_MAX_F32			FLT_MAX			//max possible float value
	#define	NX_MIN_F32			(-FLT_MAX)		//min possible float value
	#define	NX_MAX_F64			DBL_MAX			//max possible double value
	#define	NX_MIN_F64			(-DBL_MAX)		//min possible double value

	#define NX_EPS_F32			FLT_EPSILON		//smallest number not zero
	#define NX_EPS_F64			DBL_EPSILON		//smallest number not zero

	#define NX_IEEE_1_0			0x3f800000		//integer representation of 1.0
	#define NX_IEEE_255_0		0x437f0000		//integer representation of 255.0
	#define NX_IEEE_MAX_F32		0x7f7fffff		//integer representation of MAX_NXFLOAT
	#define NX_IEEE_MIN_F32		0xff7fffff		//integer representation of MIN_NXFLOAT

	typedef int	NX_BOOL;
	#define NX_FALSE			0
	#define NX_TRUE				1

	#define	NX_MIN(a, b)		((a) < (b) ? (a) : (b))			//Returns the min value between a and b
	#define	NX_MAX(a, b)		((a) > (b) ? (a) : (b))			//Returns the max value between a and b

 /** @} */
#endif
//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND





#ifndef NX_FOUNDATION_NXF
#define NX_FOUNDATION_NXF
/*----------------------------------------------------------------------------*\
|
|						Public Interface to Ageia PhysX Technology
|
|							     www.ageia.com
|
\*----------------------------------------------------------------------------*/
/** \addtogroup foundation
  @{
*/


#ifndef NX_FOUNDATION_USE_F64

typedef NxF32 NxReal;

#define NxPi	NxPiF32
#define NxHalfPi NxHalfPiF32
#define NxTwoPi NxTwoPiF32
#define NxInvPi NxInvPiF32

#define	NX_MAX_REAL			NX_MAX_F32
#define	NX_MIN_REAL			NX_MIN_F32
#define NX_EPS_REAL			NX_EPS_F32

#else

typedef NxF64 NxReal;

#define NxPi	NxPiF64
#define NxHalfPi NxHalfPiF64
#define NxTwoPi NxTwoPiF64
#define NxInvPi NxInvPiF64

#define	NX_MAX_REAL			NX_MAX_F64
#define	NX_MIN_REAL			NX_MIN_F64
#define NX_EPS_REAL			NX_EPS_F64
#endif

 /** @} */
#endif
//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND


#ifndef NX_FOUNDATION_NXASSERT
#define NX_FOUNDATION_NXASSERT
/*----------------------------------------------------------------------------*\
|
|						Public Interface to Ageia PhysX Technology
|
|							     www.ageia.com
|
\*----------------------------------------------------------------------------*/
/** \addtogroup foundation
  @{
*/


/**
This ANSI assert is included so that NX_ASSERTs can continue to appear in user side
code, where the custom assert in Assert.h would not work.
*/

#include <assert.h>
#ifndef NX_ASSERT
  #ifdef _DEBUG
    #define NX_ASSERT(x) assert(x)
  #else
    #define NX_ASSERT(x) {}
  #endif
#endif

 /** @} */
#endif
//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND


#ifndef NX_FOUNDATION_NXMATH
#define NX_FOUNDATION_NXMATH
/*----------------------------------------------------------------------------*\
|
|						Public Interface to Ageia PhysX Technology
|
|							     www.ageia.com
|
\*----------------------------------------------------------------------------*/
/** \addtogroup foundation
  @{
*/
#ifdef __PPCGEKKO__
#include "wii/NxMath_Wii.h"
#else
#include <math.h>
#include <float.h>
#include <stdlib.h>	//for rand()

#ifdef _XBOX
#include <ppcintrinsics.h> //for fpmin,fpmax, sqrt etc
#endif

#ifdef log2
#undef log2
#endif

//constants
static const NxF64 NxPiF64		= 3.141592653589793;
static const NxF64 NxHalfPiF64	= 1.57079632679489661923;
static const NxF64 NxTwoPiF64	= 6.28318530717958647692;
static const NxF64 NxInvPiF64	= 0.31830988618379067154;
//we can get bad range checks if we use double prec consts to check single prec results.
static const NxF32 NxPiF32		= 3.141592653589793f;
static const NxF32 NxHalfPiF32	= 1.57079632679489661923f;
static const NxF32 NxTwoPiF32	= 6.28318530717958647692f;
static const NxF32 NxInvPiF32	= 0.31830988618379067154f;


#if defined(min) || defined(max)
#error Error: min or max is #defined, probably in <windows.h>.  Put #define NOMINMAX before including windows.h to suppress windows global min,max macros.
#endif

/**
\brief Static class with stateless scalar math routines.
*/
class NxMath
	{
	public:

// Type conversion and rounding
		/**
		\brief Returns true if the two numbers are within eps of each other.
		*/
		NX_INLINE static bool equals(NxF32,NxF32,NxF32 eps);

		/**
		\brief Returns true if the two numbers are within eps of each other.
		*/
		NX_INLINE static bool equals(NxF64,NxF64,NxF64 eps);
		/**
		\brief The floor function returns a floating-point value representing the largest integer that is less than or equal to x.
		*/
		NX_INLINE static NxF32 floor(NxF32);
		/**
		\brief The floor function returns a floating-point value representing the largest integer that is less than or equal to x.
		*/
		NX_INLINE static NxF64 floor(NxF64);


		/**
		\brief The ceil function returns a single value representing the smallest integer that is greater than or equal to x. 
		*/
		NX_INLINE static NxF32 ceil(NxF32);
		/**
		\brief The ceil function returns a double value representing the smallest integer that is greater than or equal to x. 
		*/
		NX_INLINE static NxF64 ceil(NxF64);

		/**
		\brief Truncates the float to an integer.
		*/
		NX_INLINE static NxI32 trunc(NxF32);
		/**
		\brief Truncates the double precision float to an integer.
		*/
		NX_INLINE static NxI32 trunc(NxF64);


		/**
		\brief abs returns the absolute value of its argument. 
		*/
		NX_INLINE static NxF32 abs(NxF32);
		/**
		\brief abs returns the absolute value of its argument. 
		*/
		NX_INLINE static NxF64 abs(NxF64);
		/**
		\brief abs returns the absolute value of its argument. 
		*/
		NX_INLINE static NxI32 abs(NxI32);


		/**
		\brief sign returns the sign of its argument. The sign of zero is undefined.
		*/
		NX_INLINE static NxF32 sign(NxF32);
		/**
		\brief sign returns the sign of its argument. The sign of zero is undefined.
		*/
		NX_INLINE static NxF64 sign(NxF64);
		/**
		\brief sign returns the sign of its argument. The sign of zero is undefined.
		*/
		NX_INLINE static NxI32 sign(NxI32);


		/**
		\brief The return value is the greater of the two specified values. 
		*/
		NX_INLINE static NxF32 max(NxF32,NxF32);
		/**
		\brief The return value is the greater of the two specified values. 
		*/
		NX_INLINE static NxF64 max(NxF64,NxF64);
		/**
		\brief The return value is the greater of the two specified values. 
		*/
		NX_INLINE static NxI32 max(NxI32,NxI32);
		/**
		\brief The return value is the greater of the two specified values. 
		*/
		NX_INLINE static NxU32 max(NxU32,NxU32);
		/**
		\brief The return value is the greater of the two specified values. 
		*/
		NX_INLINE static NxU16 max(NxU16,NxU16);
		
		
		/**
		\brief The return value is the lesser of the two specified values. 
		*/
		NX_INLINE static NxF32 min(NxF32,NxF32);
		/**
		\brief The return value is the lesser of the two specified values. 
		*/
		NX_INLINE static NxF64 min(NxF64,NxF64);
		/**
		\brief The return value is the lesser of the two specified values. 
		*/
		NX_INLINE static NxI32 min(NxI32,NxI32);
		/**
		\brief The return value is the lesser of the two specified values. 
		*/
		NX_INLINE static NxU32 min(NxU32,NxU32);
		/**
		\brief The return value is the lesser of the two specified values. 
		*/
		NX_INLINE static NxU16 min(NxU16,NxU16);
		
		/**
		\brief mod returns the floating-point remainder of x / y. 
		
		If the value of y is 0.0, mod returns a quiet NaN.
		*/
		NX_INLINE static NxF32 mod(NxF32 x, NxF32 y);
		/**
		\brief mod returns the floating-point remainder of x / y. 
		
		If the value of y is 0.0, mod returns a quiet NaN.
		*/
		NX_INLINE static NxF64 mod(NxF64 x, NxF64 y);

		/**
		\brief Clamps v to the range [hi,lo]
		*/
		NX_INLINE static NxF32 clamp(NxF32 v, NxF32 hi, NxF32 low);
		/**
		\brief Clamps v to the range [hi,lo]
		*/
		NX_INLINE static NxF64 clamp(NxF64 v, NxF64 hi, NxF64 low);
		/**
		\brief Clamps v to the range [hi,lo]
		*/
		NX_INLINE static NxU32 clamp(NxU32 v, NxU32 hi, NxU32 low);
		/**
		\brief Clamps v to the range [hi,lo]
		*/
		NX_INLINE static NxI32 clamp(NxI32 v, NxI32 hi, NxI32 low);

		//!powers
		/**
		\brief Square root.
		*/
		NX_INLINE static NxF32 sqrt(NxF32);
		/**
		\brief Square root.
		*/
		NX_INLINE static NxF64 sqrt(NxF64);
		
		/**
		\brief reciprocal square root.
		*/
		NX_INLINE static NxF32 recipSqrt(NxF32);
		/**
		\brief reciprocal square root.
		*/
		NX_INLINE static NxF64 recipSqrt(NxF64);
		
		/**
		\brief Calculates x raised to the power of y.
		*/
		NX_INLINE static NxF32 pow(NxF32 x, NxF32 y);
		/**
		\brief Calculates x raised to the power of y.
		*/
		NX_INLINE static NxF64 pow(NxF64 x, NxF64 y);
		
		
		/**
		\brief Calculates e^n
		*/
		NX_INLINE static NxF32 exp(NxF32);
		/**
		\brief Calculates e^n
		*/
		NX_INLINE static NxF64 exp(NxF64);
		
		/**
		\brief Calculates logarithms.
		*/
		NX_INLINE static NxF32 logE(NxF32);
		/**
		\brief Calculates logarithms.
		*/
		NX_INLINE static NxF64 logE(NxF64);
		/**
		\brief Calculates logarithms.
		*/
		NX_INLINE static NxF32 log2(NxF32);
		/**
		\brief Calculates logarithms.
		*/
		NX_INLINE static NxF64 log2(NxF64);
		/**
		\brief Calculates logarithms.
		*/
		NX_INLINE static NxF32 log10(NxF32);
		/**
		\brief Calculates logarithms.
		*/
		NX_INLINE static NxF64 log10(NxF64);

		//!trigonometry -- all angles are in radians.
		
		/**
		\brief Converts degrees to radians.
		*/
		NX_INLINE static NxF32 degToRad(NxF32);
		/**
		\brief Converts degrees to radians.
		*/
		NX_INLINE static NxF64 degToRad(NxF64);

		/**
		\brief Converts radians to degrees.
		*/
		NX_INLINE static NxF32 radToDeg(NxF32);
		/**
		\brief Converts radians to degrees.
		*/
		NX_INLINE static NxF64 radToDeg(NxF64);

		/**
		\brief Sine of an angle.

		<b>Unit:</b> Radians
		*/
		NX_INLINE static NxF32 sin(NxF32);
		/**
		\brief Sine of an angle.

		<b>Unit:</b> Radians
		*/
		NX_INLINE static NxF64 sin(NxF64);
		
		/**
		\brief Cosine of an angle.

		<b>Unit:</b> Radians
		*/
		NX_INLINE static NxF32 cos(NxF32);
		/**
		\brief Cosine of an angle.

		<b>Unit:</b> Radians
		*/
		NX_INLINE static NxF64 cos(NxF64);

		/**
		\brief Computes both the sin and cos.

		<b>Unit:</b> Radians
		*/
		NX_INLINE static void sinCos(NxF32, NxF32 & sin, NxF32 & cos);

		/**
		\brief Computes both the sin and cos.

		<b>Unit:</b> Radians
		*/
		NX_INLINE static void sinCos(NxF64, NxF64 & sin, NxF64 & cos);


		/**
		\brief Tangent of an angle.

		<b>Unit:</b> Radians
		*/
		NX_INLINE static NxF32 tan(NxF32);
		/**
		\brief Tangent of an angle.

		<b>Unit:</b> Radians
		*/
		NX_INLINE static NxF64 tan(NxF64);
		
		/**
		\brief Arcsine.
		
		Returns angle between -PI/2 and PI/2 in radians

		<b>Unit:</b> Radians
		*/
		NX_INLINE static NxF32 asin(NxF32);
		/**
		\brief Arcsine.
		
		Returns angle between -PI/2 and PI/2 in radians

		<b>Unit:</b> Radians
		*/
		NX_INLINE static NxF64 asin(NxF64);

		/**
		\brief Arccosine.
		
		Returns angle between 0 and PI in radians

		<b>Unit:</b> Radians
		*/
		NX_INLINE static NxF32 acos(NxF32);
		
		/**
		\brief Arccosine.
		
		Returns angle between 0 and PI in radians

		<b>Unit:</b> Radians
		*/
		NX_INLINE static NxF64 acos(NxF64);
		
		/**
		\brief ArcTangent.
		
		Returns angle between -PI/2 and PI/2 in radians

		<b>Unit:</b> Radians
		*/
		NX_INLINE static NxF32 atan(NxF32);
		/**
		\brief ArcTangent.
		
		Returns angle between -PI/2 and PI/2 in radians

		<b>Unit:</b> Radians
		*/
		NX_INLINE static NxF64 atan(NxF64);

		/**
		\brief Arctangent of (x/y) with correct sign.
		
		Returns angle between -PI and PI in radians

		<b>Unit:</b> Radians
		*/
		NX_INLINE static NxF32 atan2(NxF32 x, NxF32 y);
		/**
		\brief Arctangent of (x/y) with correct sign.
		
		Returns angle between -PI and PI in radians

		<b>Unit:</b> Radians
		*/
		NX_INLINE static NxF64 atan2(NxF64 x, NxF64 y);

		//random numbers
		
		/**
		\brief uniform random number in [a,b]
		*/
		NX_INLINE static NxF32 rand(NxF32 a,NxF32 b);
		
		/**
		\brief uniform random number in [a,b]
		*/
		NX_INLINE static NxI32 rand(NxI32 a,NxI32 b);

		/**
		\brief hashing: hashes an array of n 32 bit values	to a 32 bit value.
		
		Because the output bits are uniformly distributed, the caller may mask
		off some of the bits to index into a hash table	smaller than 2^32.
		*/
		NX_INLINE static NxU32 hash(const NxU32 * array, NxU32 n);

		/**
		\brief hash32
		*/
		NX_INLINE static int hash32(int);

		/**
		\brief returns true if the passed number is a finite floating point number as opposed to INF, NAN, etc.
		*/
		NX_INLINE static bool isFinite(NxF32 x);
		
		/**
		\brief returns true if the passed number is a finite floating point number as opposed to INF, NAN, etc.
		*/
		NX_INLINE static bool isFinite(NxF64 x);
	};

/*
Many of these are just implemented as NX_INLINE calls to the C lib right now,
but later we could replace some of them with some approximations or more
clever stuff.
*/
NX_INLINE bool NxMath::equals(NxF32 a,NxF32 b,NxF32 eps)
	{
	const NxF32 diff = NxMath::abs(a - b);
	return (diff < eps);
	}

NX_INLINE bool NxMath::equals(NxF64 a,NxF64 b,NxF64 eps)
	{
	const NxF64 diff = NxMath::abs(a - b);
	return (diff < eps);
	}

NX_INLINE NxF32 NxMath::floor(NxF32 a)
	{
	return ::floorf(a);
	}

NX_INLINE NxF64 NxMath::floor(NxF64 a)
	{
	return ::floor(a);
	}

NX_INLINE NxF32 NxMath::ceil(NxF32 a)
	{
	return ::ceilf(a);
	}

NX_INLINE NxF64 NxMath::ceil(NxF64 a)
	{
	return ::ceil(a);
	}

NX_INLINE NxI32 NxMath::trunc(NxF32 a)
	{
	return (NxI32) a;	// ### PT: this actually depends on FPU settings
	}

NX_INLINE NxI32 NxMath::trunc(NxF64 a)
	{
	return (NxI32) a;	// ### PT: this actually depends on FPU settings
	}

NX_INLINE NxF32 NxMath::abs(NxF32 a)
	{
	return ::fabsf(a);
	}

NX_INLINE NxF64 NxMath::abs(NxF64 a)
	{
	return ::fabs(a);
	}

NX_INLINE NxI32 NxMath::abs(NxI32 a)
	{
	return ::abs(a);
	}

NX_INLINE NxF32 NxMath::sign(NxF32 a)
	{
	return (a >= 0.0f) ? 1.0f : -1.0f;
	}

NX_INLINE NxF64 NxMath::sign(NxF64 a)
	{
	return (a >= 0.0) ? 1.0 : -1.0;
	}

NX_INLINE NxI32 NxMath::sign(NxI32 a)
	{
	return (a >= 0) ? 1 : -1;
	}

NX_INLINE NxF32 NxMath::max(NxF32 a,NxF32 b)
	{
#ifdef _XBOX
	return (NxF32)fpmax(a, b);
#else
	return (a < b) ? b : a;
#endif
	}

NX_INLINE NxF64 NxMath::max(NxF64 a,NxF64 b)
	{
#ifdef _XBOX
	return (NxF64)fpmax(a, b);
#else
	return (a < b) ? b : a;
#endif
	}

NX_INLINE NxI32 NxMath::max(NxI32 a,NxI32 b)
	{
	return (a < b) ? b : a;
	}

NX_INLINE NxU32 NxMath::max(NxU32 a,NxU32 b)
	{
	return (a < b) ? b : a;
	}

NX_INLINE NxU16 NxMath::max(NxU16 a,NxU16 b)
	{
	return (a < b) ? b : a;
	}

NX_INLINE NxF32 NxMath::min(NxF32 a,NxF32 b)
	{
#ifdef _XBOX
	return (NxF32)fpmin(a, b);
#else
	return (a < b) ? a : b;
#endif
	}

NX_INLINE NxF64 NxMath::min(NxF64 a,NxF64 b)
	{
#ifdef _XBOX
	return (NxF64)fpmin(a, b);
#else
	return (a < b) ? a : b;
#endif
	}

NX_INLINE NxI32 NxMath::min(NxI32 a,NxI32 b)
	{
	return (a < b) ? a : b;
	}

NX_INLINE NxU32 NxMath::min(NxU32 a,NxU32 b)
	{
	return (a < b) ? a : b;
	}

NX_INLINE NxU16 NxMath::min(NxU16 a,NxU16 b)
	{
	return (a < b) ? a : b;
	}

NX_INLINE NxF32 NxMath::mod(NxF32 x, NxF32 y)
	{
	return (NxF32)::fmod(x,y);
	}

NX_INLINE NxF64 NxMath::mod(NxF64 x, NxF64 y)
	{
	return ::fmod(x,y);
	}

NX_INLINE NxF32 NxMath::clamp(NxF32 v, NxF32 hi, NxF32 low)
	{
	if (v > hi) 
		return hi;
	else if (v < low) 
		return low;
	else
		return v;
	}

NX_INLINE NxF64 NxMath::clamp(NxF64 v, NxF64 hi, NxF64 low)
	{
	if (v > hi) 
		return hi;
	else if (v < low) 
		return low;
	else
		return v;
	}

NX_INLINE NxU32 NxMath::clamp(NxU32 v, NxU32 hi, NxU32 low)
	{
	if (v > hi) 
		return hi;
	else if (v < low) 
		return low;
	else
		return v;
	}

NX_INLINE NxI32 NxMath::clamp(NxI32 v, NxI32 hi, NxI32 low)
	{
	if (v > hi) 
		return hi;
	else if (v < low) 
		return low;
	else
		return v;
	}

#ifdef _XBOX
NX_INLINE NxF32 NxMath::sqrt(NxF32 a)
	{
	return __fsqrts(a);
	}

NX_INLINE NxF64 NxMath::sqrt(NxF64 a)
	{
	return __fsqrt(a);
	}
#else
NX_INLINE NxF32 NxMath::sqrt(NxF32 a)
	{
	return ::sqrtf(a);
	}

NX_INLINE NxF64 NxMath::sqrt(NxF64 a)
	{
	return ::sqrt(a);
	}
#endif

NX_INLINE NxF32 NxMath::recipSqrt(NxF32 a)
	{
	return 1.0f/::sqrtf(a);
	}

NX_INLINE NxF64 NxMath::recipSqrt(NxF64 a)
	{
	return 1.0/::sqrt(a);
	}


NX_INLINE NxF32 NxMath::pow(NxF32 x, NxF32 y)
	{
	return ::powf(x,y);
	}

NX_INLINE NxF64 NxMath::pow(NxF64 x, NxF64 y)
	{
	return ::pow(x,y);
	}

NX_INLINE NxF32 NxMath::exp(NxF32 a)
	{
	return ::expf(a);
	}

NX_INLINE NxF64 NxMath::exp(NxF64 a)
	{
	return ::exp(a);
	}

NX_INLINE NxF32 NxMath::logE(NxF32 a)
	{
	return ::logf(a);
	}

NX_INLINE NxF64 NxMath::logE(NxF64 a)
	{
	return ::log(a);
	}

NX_INLINE NxF32 NxMath::log2(NxF32 a)
	{
	const NxF32 ln2 = (NxF32)0.693147180559945309417;
    return ::logf(a) / ln2;
	}

NX_INLINE NxF64 NxMath::log2(NxF64 a)
	{
	const NxF64 ln2 = (NxF64)0.693147180559945309417;
    return ::log(a) / ln2;
	}

NX_INLINE NxF32 NxMath::log10(NxF32 a)
	{
	return (NxF32)::log10(a);
	}

NX_INLINE NxF64 NxMath::log10(NxF64 a)
	{
	return ::log10(a);
	}

NX_INLINE NxF32 NxMath::degToRad(NxF32 a)
	{
	return (NxF32)0.01745329251994329547 * a;
	}

NX_INLINE NxF64 NxMath::degToRad(NxF64 a)
	{
	return (NxF64)0.01745329251994329547 * a;
	}

NX_INLINE NxF32 NxMath::radToDeg(NxF32 a)
	{
	return (NxF32)57.29577951308232286465 * a;
	}

NX_INLINE NxF64 NxMath::radToDeg(NxF64 a)
	{
	return (NxF64)57.29577951308232286465 * a;
	}

NX_INLINE NxF32 NxMath::sin(NxF32 a)
	{
	return ::sinf(a);
	}

NX_INLINE NxF64 NxMath::sin(NxF64 a)
	{
	return ::sin(a);
	}

NX_INLINE NxF32 NxMath::cos(NxF32 a)
	{
	return ::cosf(a);
	}

NX_INLINE NxF64 NxMath::cos(NxF64 a)
	{
	return ::cos(a);
	}

// Calling fsincos instead of fsin+fcos
NX_INLINE void NxMath::sinCos(NxF32 f, NxF32& s, NxF32& c)
	{
#if defined(WIN32) && !defined(_WIN64)
		NxF32 localCos, localSin;
		NxF32 local = f;
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

NX_INLINE void NxMath::sinCos(NxF64 a, NxF64 & s, NxF64 & c)
	{
	s = ::sin(a);
	c = ::cos(a);
	}

NX_INLINE NxF32 NxMath::tan(NxF32 a)
	{
	return ::tanf(a);
	}

NX_INLINE NxF64 NxMath::tan(NxF64 a)
	{
	return ::tan(a);
	}

NX_INLINE NxF32 NxMath::asin(NxF32 f)
	{
	// Take care of FPU inaccuracies
	if(f>=1.0f)	return (NxF32)NxHalfPiF32;
	if(f<=-1.0f)return -(NxF32)NxHalfPiF32;
				return ::asinf(f);
	}

NX_INLINE NxF64 NxMath::asin(NxF64 f)
	{
	// Take care of FPU inaccuracies
	if(f>=1.0)	return (NxF32)NxHalfPiF64;
	if(f<=-1.0)	return -(NxF32)NxHalfPiF64;
				return ::asin(f);
	}

NX_INLINE NxF32 NxMath::acos(NxF32 f)
	{
	// Take care of FPU inaccuracies
	if(f>=1.0f)	return 0.0f;
	if(f<=-1.0f)return (NxF32)NxPiF32;
				return ::acosf(f);
	}

NX_INLINE NxF64 NxMath::acos(NxF64 f)
	{
	// Take care of FPU inaccuracies
	if(f>=1.0)	return 0.0;
	if(f<=-1.0)	return (NxF64)NxPiF64;
				return ::acos(f);
	}

NX_INLINE NxF32 NxMath::atan(NxF32 a)
	{
	return ::atanf(a);
	}

NX_INLINE NxF64 NxMath::atan(NxF64 a)
	{
	return ::atan(a);
	}

NX_INLINE NxF32 NxMath::atan2(NxF32 x, NxF32 y)
	{
	return ::atan2f(x,y);
	}

NX_INLINE NxF64 NxMath::atan2(NxF64 x, NxF64 y)
	{
	return ::atan2(x,y);
	}

NX_INLINE NxF32 NxMath::rand(NxF32 a,NxF32 b)
	{
	const NxF32 r = (NxF32)::rand()/((NxF32)RAND_MAX+1);
	return r*(b-a) + a;
	}

NX_INLINE NxI32 NxMath::rand(NxI32 a,NxI32 b)
	{
	return a + (NxI32)(::rand()%(b-a));
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
#define NX_HASH_MIX(a,b,c) \
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
NX_INLINE NxU32 NxMath::hash(const NxU32 *k, NxU32 length)
//register ub4 *k;        /* the key */
//register ub4  length;   /* the length of the key, in ub4s */
	{
	NxU32 a,b,c,len;

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
	  NX_HASH_MIX(a,b,c);
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
	NX_HASH_MIX(a,b,c);
	/*-------------------------------------------- report the result */
	return c;
	}
#undef NX_HASH_MIX

NX_INLINE int NxMath::hash32(int key)
	{
	key += ~(key << 15);
	key ^=  (key >> 10);
	key +=  (key << 3);
	key ^=  (key >> 6);
	key += ~(key << 11);
	key ^=  (key >> 16);
	return key;
	}


NX_INLINE bool NxMath::isFinite(NxF32 f)
	{
	#if defined(_MSC_VER)
	return (0 == ((_FPCLASS_SNAN | _FPCLASS_QNAN | _FPCLASS_NINF | _FPCLASS_PINF) & _fpclass(f) ));
	#elif defined(__CELLOS_LV2__)
	return (!(isnan(f)||isinf(f)));
	#elif defined(__MWERKS__)
	return isfinite(f);
	#else
	return true;
	#endif
	
	}

NX_INLINE bool NxMath::isFinite(NxF64 f)
	{
	#if defined(_MSC_VER)
	return (0 == ((_FPCLASS_SNAN | _FPCLASS_QNAN | _FPCLASS_NINF | _FPCLASS_PINF) & _fpclass(f) ));
	#elif defined(__CELLOS_LV2__)
	return (!(isnan(f)||isinf(f)));
	#elif defined(__MWERKS__)
	return isfinite(f);
	#else
	return true;
	#endif
	}

 /** @} */
#endif
#endif
//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND


#ifndef NX_FOUNDATION_NXVEC3
#define NX_FOUNDATION_NXVEC3
/*----------------------------------------------------------------------------*\
|
|						Public Interface to Ageia PhysX Technology
|
|							     www.ageia.com
|
\*----------------------------------------------------------------------------*/

/** \addtogroup foundation
  @{
*/

#ifdef __PPCGEKKO__
#include "wii\NxVec3_Wii.h"
#else

class  NxMat33;


/**
\brief Enum to classify an axis.
*/
	enum NxAxisType
	{
		NX_AXIS_PLUS_X,
		NX_AXIS_MINUS_X,
		NX_AXIS_PLUS_Y,
		NX_AXIS_MINUS_Y,
		NX_AXIS_PLUS_Z,
		NX_AXIS_MINUS_Z,
		NX_AXIS_ARBITRARY
	};


class NxVec3;

/** \cond Exclude from documentation */
typedef struct _Nx3F32
{
	NxReal x, y, z;

	NX_INLINE const _Nx3F32& operator=(const NxVec3& d);
} Nx3F32;
/** \endcond */

/**
\brief 3 Element vector class.

This is a vector class with public data members.
This is not nice but it has become such a standard that hiding the xyz data members
makes it difficult to reuse external code that assumes that these are public in the library.
The vector class can be made to use float or double precision by appropriately defining NxReal.
This has been chosen as a cleaner alternative to a template class.
*/
class NxVec3
	{
	public:
	//!Constructors

	/**
	\brief default constructor leaves data uninitialized.
	*/
	NX_INLINE NxVec3();

	/**
	\brief Assigns scalar parameter to all elements.

	Useful to initialize to zero or one.

	\param[in] a Value to assign to elements.
	*/
	explicit NX_INLINE NxVec3(NxReal a);

	/**
	\brief Initializes from 3 scalar parameters.

	\param[in] nx Value to initialize X component.
	\param[in] ny Value to initialize Y component.
	\param[in] nz Value to initialize Z component.
	*/
	NX_INLINE NxVec3(NxReal nx, NxReal ny, NxReal nz);
	
	/**
	\brief Initializes from Nx3F32 data type.

	\param[in] a Value to initialize with.
	*/
	NX_INLINE NxVec3(const Nx3F32 &a);

	/**
	\brief Initializes from an array of scalar parameters.

	\param[in] v Value to initialize with.
	*/
	NX_INLINE NxVec3(const NxReal v[]);

	/**
	\brief Copy constructor.
	*/
	NX_INLINE NxVec3(const NxVec3& v);

	/**
	\brief Assignment operator.
	*/
	NX_INLINE const NxVec3& operator=(const NxVec3&);

	/**
	\brief Assignment operator.
	*/
	NX_INLINE const NxVec3& operator=(const Nx3F32&);

	/**
	\brief Access the data as an array.

	\return Array of 3 floats.
	*/
	NX_INLINE const NxReal *get() const;
	
	/**
	\brief Access the data as an array.

	\return Array of 3 floats.
	*/
	NX_INLINE NxReal* get();

	/**
	\brief writes out the 3 values to dest.

	\param[out] dest Array to write elements to.
	*/
	NX_INLINE void get(NxF32 * dest) const;

	/**
	\brief writes out the 3 values to dest.
	*/
	NX_INLINE void get(NxF64 * dest) const;

	NX_INLINE NxReal& operator[](int index);
	NX_INLINE NxReal  operator[](int index) const;

	//Operators
	/**
	\brief true if all the members are smaller.
	*/
	NX_INLINE bool operator< (const NxVec3&) const;
	/**
	\brief returns true if the two vectors are exactly equal.

	use equal() to test with a tolerance.
	*/
	NX_INLINE bool operator==(const NxVec3&) const;
	/**
	\brief returns true if the two vectors are exactly unequal.

	use !equal() to test with a tolerance.
	*/
	NX_INLINE bool operator!=(const NxVec3&) const;

/*	NX_INLINE const NxVec3 &operator +=(const NxVec3 &);
	NX_INLINE const NxVec3 &operator -=(const NxVec3 &);
	NX_INLINE const NxVec3 &operator *=(NxReal);
	NX_INLINE const NxVec3 &operator /=(NxReal);
*/
//Methods
	NX_INLINE void  set(const NxVec3 &);

//legacy methods:
	NX_INLINE void setx(const NxReal & d);
	NX_INLINE void sety(const NxReal & d);
	NX_INLINE void setz(const NxReal & d);

	/**
	\brief this = -a
	*/
	NX_INLINE void  setNegative(const NxVec3 &a);

	/**
	\brief this = -this
	*/
	NX_INLINE void  setNegative();

	/**
	\brief reads 3 consecutive values from the ptr passed
	*/
	NX_INLINE void  set(const NxF32 *);

	/**
	\brief reads 3 consecutive values from the ptr passed
	*/
	NX_INLINE void  set(const NxF64 *);
	NX_INLINE void  set(NxReal, NxReal, NxReal);
	NX_INLINE void  set(NxReal);

	NX_INLINE void  zero();
	
	/**
	\brief tests for exact zero vector
	*/
	NX_INLINE NX_BOOL isZero()	const
		{
		if((x != 0.0f) || (y != 0.0f) || (z != 0.0f))	return NX_FALSE;
		return NX_TRUE;
		}

	NX_INLINE void  setPlusInfinity();
	NX_INLINE void  setMinusInfinity();

	/**
	\brief this = element wise min(this,other)
	*/
	NX_INLINE void min(const NxVec3 &);
	/**
	\brief this = element wise max(this,other)
	*/
	NX_INLINE void max(const NxVec3 &);

	/**
	\brief this = a + b
	*/
	NX_INLINE void  add(const NxVec3 & a, const NxVec3 & b);
	/**
	\brief this = a - b
	*/
	NX_INLINE void  subtract(const NxVec3 &a, const NxVec3 &b);
	/**
	\brief this = s * a;
	*/
	NX_INLINE void  multiply(NxReal s,  const NxVec3 & a);

	/**
	\brief this[i] = a[i] * b[i], for all i.
	*/
	NX_INLINE void  arrayMultiply(const NxVec3 &a, const NxVec3 &b);


	/**
	\brief this = s * a + b;
	*/
	NX_INLINE void  multiplyAdd(NxReal s, const NxVec3 & a, const NxVec3 & b);

	/**
	\brief normalizes the vector
	*/
	NX_INLINE NxReal normalize();

	/**
	\brief sets the vector's magnitude
	*/
	NX_INLINE void	setMagnitude(NxReal);

	/**
	\brief snaps to closest axis
	*/
	NX_INLINE NxU32			closestAxis()	const;

	/**
	\brief snaps to closest axis
	*/
	NX_INLINE NxAxisType	snapToClosestAxis();

//const methods
	/**
	\brief returns true if all 3 elems of the vector are finite (not NAN or INF, etc.)
	*/
	NX_INLINE bool isFinite() const;

	/**
	\brief returns the scalar product of this and other.
	*/
	NX_INLINE NxReal dot(const NxVec3 &other) const;

	/**
	\brief compares orientations (more readable, user-friendly function)
	*/
	NX_INLINE bool sameDirection(const NxVec3 &) const;

	/**
	\brief returns the magnitude
	*/
	NX_INLINE NxReal magnitude() const;

	/**
	\brief returns the squared magnitude
	
	Avoids calling sqrt()!
	*/
	NX_INLINE NxReal magnitudeSquared() const;

	/**
	\brief returns (this - other).magnitude();
	*/
	NX_INLINE NxReal distance(const NxVec3 &) const;

	/**
	\brief returns (this - other).magnitudeSquared();
	*/
	NX_INLINE NxReal distanceSquared(const NxVec3 &v) const;

	/**
	\brief this = left x right
	*/
	NX_INLINE void cross(const NxVec3 &left, const NxVec3 & right);

	/**
	\brief Stuff magic values in the point, marking it as explicitly not used.
	*/
	NX_INLINE void setNotUsed();

	/**
	\brief Checks the point is marked as not used
	*/
	NX_BOOL isNotUsed() const;

	/**
	\brief returns true if this and arg's elems are within epsilon of each other.
	*/
	NX_INLINE bool equals(const NxVec3 &, NxReal epsilon) const;

	/**
	\brief negation
	*/
	NxVec3 operator -() const;
	/**
	\brief vector addition
	*/
	NxVec3 operator +(const NxVec3 & v) const;
	/**
	\brief vector difference
	*/
	NxVec3 operator -(const NxVec3 & v) const;
	/**
	\brief scalar post-multiplication
	*/
	NxVec3 operator *(NxReal f) const;
	/**
	\brief scalar division
	*/
	NxVec3 operator /(NxReal f) const;
	/**
	\brief vector addition
	*/
	NxVec3&operator +=(const NxVec3& v);
	/**
	\brief vector difference
	*/
	NxVec3&operator -=(const NxVec3& v);
	/**
	\brief scalar multiplication
	*/
	NxVec3&operator *=(NxReal f);
	/**
	\brief scalar division
	*/
	NxVec3&operator /=(NxReal f);
	/**
	\brief cross product
	*/
	NxVec3 cross(const NxVec3& v) const;

	/**
	\brief cross product
	*/
	NxVec3 operator^(const NxVec3& v) const;
	/**
	\brief dot product
	*/
	NxReal      operator|(const NxVec3& v) const;

	NxReal x,y,z;
	};

/** \cond Exclude from documentation */
NX_INLINE const _Nx3F32& Nx3F32::operator=(const NxVec3& d)
	{
	x=d.x;
	y=d.y;
	z=d.z;
	return *this;
	}
/** \endcond */

//NX_INLINE implementations:
NX_INLINE NxVec3::NxVec3(NxReal v) : x(v), y(v), z(v)
	{
	}

NX_INLINE NxVec3::NxVec3(NxReal _x, NxReal _y, NxReal _z) : x(_x), y(_y), z(_z)
	{
	}

NX_INLINE NxVec3::NxVec3(const Nx3F32 &d) : x(d.x), y(d.y), z(d.z)
	{
	}

NX_INLINE NxVec3::NxVec3(const NxReal v[]) : x(v[0]), y(v[1]), z(v[2])
	{
	}


NX_INLINE NxVec3::NxVec3(const NxVec3 &v) : x(v.x), y(v.y), z(v.z)
	{
	}


NX_INLINE NxVec3::NxVec3()
	{
	//default constructor leaves data uninitialized.
	}


NX_INLINE const NxVec3& NxVec3::operator=(const NxVec3& v)
	{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
	}

NX_INLINE const NxVec3& NxVec3::operator=(const Nx3F32& d)
	{
	x = d.x;
	y = d.y;
	z = d.z;
	return *this;
	}

// Access the data as an array.

NX_INLINE const NxReal* NxVec3::get() const
	{
	return &x;
	}


NX_INLINE NxReal* NxVec3::get()
	{
	return &x;
	}

 
NX_INLINE void  NxVec3::get(NxF32 * v) const
	{
	v[0] = (NxF32)x;
	v[1] = (NxF32)y;
	v[2] = (NxF32)z;
	}

 
NX_INLINE void  NxVec3::get(NxF64 * v) const
	{
	v[0] = (NxF64)x;
	v[1] = (NxF64)y;
	v[2] = (NxF64)z;
	}


NX_INLINE NxReal& NxVec3::operator[](int index)
	{
	NX_ASSERT(index>=0 && index<=2);
	return (&x)[index];
	}


NX_INLINE NxReal  NxVec3::operator[](int index) const
	{
	NX_ASSERT(index>=0 && index<=2);
	return (&x)[index];
	}

 
NX_INLINE void NxVec3::setx(const NxReal & d) 
	{ 
	x = d; 
	}

 
NX_INLINE void NxVec3::sety(const NxReal & d) 
	{ 
	y = d; 
	}

 
NX_INLINE void NxVec3::setz(const NxReal & d) 
	{ 
	z = d; 
	}

//Operators
 
NX_INLINE bool NxVec3::operator< (const NxVec3&v) const
	{
	return ((x < v.x)&&(y < v.y)&&(z < v.z));
	}

 
NX_INLINE bool NxVec3::operator==(const NxVec3& v) const
	{
	return ((x == v.x)&&(y == v.y)&&(z == v.z));
	}

 
NX_INLINE bool NxVec3::operator!=(const NxVec3& v) const
	{
	return ((x != v.x)||(y != v.y)||(z != v.z));
	}

//Methods
 
NX_INLINE void  NxVec3::set(const NxVec3 & v)
	{
	x = v.x;
	y = v.y;
	z = v.z;
	}

 
NX_INLINE void  NxVec3::setNegative(const NxVec3 & v)
	{
	x = -v.x;
	y = -v.y;
	z = -v.z;
	}

 
NX_INLINE void  NxVec3::setNegative()
	{
	x = -x;
	y = -y;
	z = -z;
	}


 
NX_INLINE void  NxVec3::set(const NxF32 * v)
	{
	x = (NxReal)v[0];
	y = (NxReal)v[1];
	z = (NxReal)v[2];
	}

 
NX_INLINE void  NxVec3::set(const NxF64 * v)
	{
	x = (NxReal)v[0];
	y = (NxReal)v[1];
	z = (NxReal)v[2];
	}


 
NX_INLINE void  NxVec3::set(NxReal _x, NxReal _y, NxReal _z)
	{
	this->x = _x;
	this->y = _y;
	this->z = _z;
	}

 
NX_INLINE void NxVec3::set(NxReal v)
	{
	x = v;
	y = v;
	z = v;
	}

 
NX_INLINE void  NxVec3::zero()
	{
	x = y = z = 0.0;
	}

 
NX_INLINE void  NxVec3::setPlusInfinity()
	{
	x = y = z = NX_MAX_F32; //TODO: this may be double too, but here we can't tell!
	}

 
NX_INLINE void  NxVec3::setMinusInfinity()
	{
	x = y = z = NX_MIN_F32; //TODO: this may be double too, but here we can't tell!
	}

 
NX_INLINE void NxVec3::max(const NxVec3 & v)
	{
	x = NxMath::max(x, v.x);
	y = NxMath::max(y, v.y);
	z = NxMath::max(z, v.z);
	}

 
NX_INLINE void NxVec3::min(const NxVec3 & v)
	{
	x = NxMath::min(x, v.x);
	y = NxMath::min(y, v.y);
	z = NxMath::min(z, v.z);
	}




NX_INLINE void  NxVec3::add(const NxVec3 & a, const NxVec3 & b)
	{
	x = a.x + b.x;
	y = a.y + b.y;
	z = a.z + b.z;
	}


NX_INLINE void  NxVec3::subtract(const NxVec3 &a, const NxVec3 &b)
	{
	x = a.x - b.x;
	y = a.y - b.y;
	z = a.z - b.z;
	}


NX_INLINE void  NxVec3::arrayMultiply(const NxVec3 &a, const NxVec3 &b)
	{
	x = a.x * b.x;
	y = a.y * b.y;
	z = a.z * b.z;
	}


NX_INLINE void  NxVec3::multiply(NxReal s,  const NxVec3 & a)
	{
	x = a.x * s;
	y = a.y * s;
	z = a.z * s;
	}


NX_INLINE void  NxVec3::multiplyAdd(NxReal s, const NxVec3 & a, const NxVec3 & b)
	{
	x = s * a.x + b.x;
	y = s * a.y + b.y;
	z = s * a.z + b.z;
	}

 
NX_INLINE NxReal NxVec3::normalize()
	{
	NxReal m = magnitude();
	if (m)
		{
		const NxReal il =  NxReal(1.0) / m;
		x *= il;
		y *= il;
		z *= il;
		}
	return m;
	}

 
NX_INLINE void NxVec3::setMagnitude(NxReal length)
	{
	NxReal m = magnitude();
	if(m)
		{
		NxReal newLength = length / m;
		x *= newLength;
		y *= newLength;
		z *= newLength;
		}
	}

 
NX_INLINE NxAxisType NxVec3::snapToClosestAxis()
	{
	const NxReal almostOne = 0.999999f;
			if(x >=  almostOne) { set( 1.0f,  0.0f,  0.0f);	return NX_AXIS_PLUS_X ; }
	else	if(x <= -almostOne) { set(-1.0f,  0.0f,  0.0f);	return NX_AXIS_MINUS_X; }
	else	if(y >=  almostOne) { set( 0.0f,  1.0f,  0.0f);	return NX_AXIS_PLUS_Y ; }
	else	if(y <= -almostOne) { set( 0.0f, -1.0f,  0.0f);	return NX_AXIS_MINUS_Y; }
	else	if(z >=  almostOne) { set( 0.0f,  0.0f,  1.0f);	return NX_AXIS_PLUS_Z ; }
	else	if(z <= -almostOne) { set( 0.0f,  0.0f, -1.0f);	return NX_AXIS_MINUS_Z; }
	else													return NX_AXIS_ARBITRARY;
	}


NX_INLINE NxU32 NxVec3::closestAxis() const
	{
	const NxF32* vals = &x;
	NxU32 m = 0;
	if(NxMath::abs(vals[1]) > NxMath::abs(vals[m])) m = 1;
	if(NxMath::abs(vals[2]) > NxMath::abs(vals[m])) m = 2;
	return m;
	}


//const methods
 
NX_INLINE bool NxVec3::isFinite() const
	{
	return NxMath::isFinite(x) && NxMath::isFinite(y) && NxMath::isFinite(z);
	}

 
NX_INLINE NxReal NxVec3::dot(const NxVec3 &v) const
	{
	return x * v.x + y * v.y + z * v.z;
	}

 
NX_INLINE bool NxVec3::sameDirection(const NxVec3 &v) const
	{
	return x*v.x + y*v.y + z*v.z >= 0.0f;
	}

 
NX_INLINE NxReal NxVec3::magnitude() const
	{
	return NxMath::sqrt(x * x + y * y + z * z);
	}

 
NX_INLINE NxReal NxVec3::magnitudeSquared() const
	{
	return x * x + y * y + z * z;
	}

 
NX_INLINE NxReal NxVec3::distance(const NxVec3 & v) const
	{
	NxReal dx = x - v.x;
	NxReal dy = y - v.y;
	NxReal dz = z - v.z;
	return NxMath::sqrt(dx * dx + dy * dy + dz * dz);
	}

 
NX_INLINE NxReal NxVec3::distanceSquared(const NxVec3 &v) const
	{
	NxReal dx = x - v.x;
	NxReal dy = y - v.y;
	NxReal dz = z - v.z;
	return dx * dx + dy * dy + dz * dz;
	}

 
NX_INLINE void NxVec3::cross(const NxVec3 &left, const NxVec3 & right)	//prefered version, w/o temp object.
	{
	// temps needed in case left or right is this.
	NxReal a = (left.y * right.z) - (left.z * right.y);
	NxReal b = (left.z * right.x) - (left.x * right.z);
	NxReal c = (left.x * right.y) - (left.y * right.x);

	x = a;
	y = b;
	z = c;
	}

 
NX_INLINE bool NxVec3::equals(const NxVec3 & v, NxReal epsilon) const
	{
	return 
		NxMath::equals(x, v.x, epsilon) &&
		NxMath::equals(y, v.y, epsilon) &&
		NxMath::equals(z, v.z, epsilon);
	}


 
NX_INLINE NxVec3 NxVec3::operator -() const
	{
	return NxVec3(-x, -y, -z);
	}

 
NX_INLINE NxVec3 NxVec3::operator +(const NxVec3 & v) const
	{
	return NxVec3(x + v.x, y + v.y, z + v.z);	// RVO version
	}

 
NX_INLINE NxVec3 NxVec3::operator -(const NxVec3 & v) const
	{
	return NxVec3(x - v.x, y - v.y, z - v.z);	// RVO version
	}



NX_INLINE NxVec3 NxVec3::operator *(NxReal f) const
	{
	return NxVec3(x * f, y * f, z * f);	// RVO version
	}


NX_INLINE NxVec3 NxVec3::operator /(NxReal f) const
	{
		f = NxReal(1.0) / f; return NxVec3(x * f, y * f, z * f);
	}


NX_INLINE NxVec3& NxVec3::operator +=(const NxVec3& v)
	{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
	}


NX_INLINE NxVec3& NxVec3::operator -=(const NxVec3& v)
	{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
	}


NX_INLINE NxVec3& NxVec3::operator *=(NxReal f)
	{
	x *= f;
	y *= f;
	z *= f;
	
	return *this;
	}


NX_INLINE NxVec3& NxVec3::operator /=(NxReal f)
	{
	f = 1.0f/f;
	x *= f;
	y *= f;
	z *= f;
	
	return *this;
	}


NX_INLINE NxVec3 NxVec3::cross(const NxVec3& v) const
	{
	NxVec3 temp;
	temp.cross(*this,v);
	return temp;
	}


NX_INLINE NxVec3 NxVec3::operator^(const NxVec3& v) const
	{
	NxVec3 temp;
	temp.cross(*this,v);
	return temp;
	}


NX_INLINE NxReal NxVec3::operator|(const NxVec3& v) const
	{
	return x * v.x + y * v.y + z * v.z;
	}

/**
scalar pre-multiplication
*/

NX_INLINE NxVec3 operator *(NxReal f, const NxVec3& v)
	{
	return NxVec3(f * v.x, f * v.y, f * v.z);
	}

 /** @} */
#endif
#endif
//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND


#ifndef NX_FOUNDATION_NxQuatT
#define NX_FOUNDATION_NxQuatT
/*----------------------------------------------------------------------------*\
|
|						Public Interface to Ageia PhysX Technology
|
|							     www.ageia.com
|
\*----------------------------------------------------------------------------*/
/** \addtogroup foundation
  @{
*/
#ifdef __PPCGEKKO__
#include "wii\NxQuat_Wii.h"
#else

/**
\brief This is a quaternion class. For more information on quaternion mathematics
consult a mathematics source on complex numbers.
 
*/

class NxQuat
	{
	public:
	/**
	\brief Default constructor, does not do any initialization.
	*/
	NX_INLINE NxQuat();

	/**
	\brief Copy constructor.
	*/
	NX_INLINE NxQuat(const NxQuat&);

	/**
	\brief copies xyz elements from v, and scalar from w (defaults to 0).
	*/
	NX_INLINE NxQuat(const NxVec3& v, NxReal w = 0);

	/**
	\brief creates from angle-axis representation.

	note that if Angle > 360 the resulting rotation is Angle mod 360.
	
	<b>Unit:</b> Degrees
	*/
	NX_INLINE NxQuat(const NxReal angle, const NxVec3 & axis);

	/**
	\brief Creates from orientation matrix.

	\param[in] m Rotation matrix to extract quaternion from.
	*/
	NX_INLINE NxQuat(const class NxMat33 &m); /* defined in NxMat33.h */


	/**
	\brief Set the quaternion to the identity rotation.
	*/
	NX_INLINE void id();

	/**
	\brief Test if the quaternion is the identity rotation.
	*/
	NX_INLINE bool isIdentityRotation() const;

	//setting:

	/**
	\brief Set the members of the quaternion, in order WXYZ
	*/
	NX_INLINE void setWXYZ(NxReal w, NxReal x, NxReal y, NxReal z);

	/**
	\brief Set the members of the quaternion, in order XYZW
	*/
	NX_INLINE void setXYZW(NxReal x, NxReal y, NxReal z, NxReal w);

	/**
	\brief Set the members of the quaternion, in order WXYZ
	*/
	NX_INLINE void setWXYZ(const NxReal *);

	/**
	\brief Set the members of the quaternion, in order XYZW
	*/
	NX_INLINE void setXYZW(const NxReal *);

	NX_INLINE NxQuat& operator=  (const NxQuat&);

	/**
	\brief Implicitly extends vector by a 0 w element.
	*/
	NX_INLINE NxQuat& operator=  (const NxVec3&);

	NX_INLINE void setx(const NxReal& d);
	NX_INLINE void sety(const NxReal& d);
	NX_INLINE void setz(const NxReal& d);
	NX_INLINE void setw(const NxReal& d);

	NX_INLINE void getWXYZ(NxF32 *) const;
	NX_INLINE void getXYZW(NxF32 *) const;

	NX_INLINE void getWXYZ(NxF64 *) const;
	NX_INLINE void getXYZW(NxF64 *) const;

	/**
	\brief returns true if all elements are finite (not NAN or INF, etc.)
	*/
	NX_INLINE bool isFinite() const;

	/**
	\brief sets to the quat [0,0,0,1]
	*/
	NX_INLINE void zero();

	/**
	\brief creates a random unit quaternion.
	*/
	NX_INLINE void random();
	/**
	\brief creates from angle-axis representation.

	Note that if Angle > 360 the resulting rotation is Angle mod 360.
	
	<b>Unit:</b> Degrees
	*/
	NX_INLINE void fromAngleAxis(NxReal angle, const NxVec3 & axis);

	/**
	\brief Creates from angle-axis representation.

	Axis must be normalized!
	
	<b>Unit:</b> Radians
	*/
	NX_INLINE void fromAngleAxisFast(NxReal AngleRadians, const NxVec3 & axis);

	/**
	\brief Sets this to the opposite rotation of this.
	*/
	NX_INLINE void invert();

	/**
	\brief Fetches the Angle/axis given by the NxQuat.

	<b>Unit:</b> Degrees
	*/
	NX_INLINE void getAngleAxis(NxReal& Angle, NxVec3 & axis) const;

	/**
	\brief Gets the angle between this quat and the identity quaternion.

	<b>Unit:</b> Degrees
	*/
	NX_INLINE NxReal getAngle() const;

	/**
	\brief Gets the angle between this quat and the argument

	<b>Unit:</b> Degrees
	*/
	NX_INLINE NxReal getAngle(const NxQuat &) const;

	/**
	\brief This is the squared 4D vector length, should be 1 for unit quaternions.
	*/
	NX_INLINE NxReal magnitudeSquared() const;

	/**
	\brief returns the scalar product of this and other.
	*/
	NX_INLINE NxReal dot(const NxQuat &other) const;

	//modifiers:
	/**
	\brief maps to the closest unit quaternion.
	*/
	NX_INLINE void normalize();

	/*
	\brief assigns its own conjugate to itself.

	\note for unit quaternions, this is the inverse.
	*/
	NX_INLINE void conjugate();

	/**
	this = a * b
	*/
	NX_INLINE void multiply(const NxQuat& a, const NxQuat& b);

	/**
	this = a * v
	v is interpreted as quat [xyz0]
	*/
	NX_INLINE void multiply(const NxQuat& a, const NxVec3& v);

	/**
	this = slerp(t, a, b)
	*/
	NX_INLINE void slerp(const NxReal t, const NxQuat& a, const NxQuat& b);

	/**
	rotates passed vec by rot expressed by unit quaternion.  overwrites arg with the result.
	*/
	NX_INLINE void rotate(NxVec3 &) const;

	/**
	rotates passed vec by this (assumed unitary)
	*/
	NX_INLINE const NxVec3 rot(const NxVec3 &) const;

	/**
	inverse rotates passed vec by this (assumed unitary)
	*/
	NX_INLINE const NxVec3 invRot(const NxVec3 &) const;

	/**
	transform passed vec by this rotation (assumed unitary) and translation p
	*/
	NX_INLINE const NxVec3 transform(const NxVec3 &v, const NxVec3 &p) const;

	/**
	inverse rotates passed vec by this (assumed unitary)
	*/
	NX_INLINE const NxVec3 invTransform(const NxVec3 &v, const NxVec3 &p) const;


	/**
	rotates passed vec by opposite of rot expressed by unit quaternion.  overwrites arg with the result.
	*/
	NX_INLINE void inverseRotate(NxVec3 &) const;



	/**
	negates all the elements of the quat.  q and -q represent the same rotation.
	*/
	NX_INLINE void negate();
	NX_INLINE NxQuat operator -() const; 

	NX_INLINE NxQuat& operator*= (const NxQuat&);
	NX_INLINE NxQuat& operator+= (const NxQuat&);
	NX_INLINE NxQuat& operator-= (const NxQuat&);
	NX_INLINE NxQuat& operator*= (const NxReal s);

	/** the quaternion elements */
    NxReal x,y,z,w;

	/** quaternion multiplication */
	NX_INLINE NxQuat operator *(const NxQuat &) const; 

	/** quaternion addition */
	NX_INLINE NxQuat operator +(const NxQuat &) const; 

	/** quaternion subtraction */
	NX_INLINE NxQuat operator -(const NxQuat &) const; 

	/** quaternion conjugate */
	NX_INLINE NxQuat operator !() const; 

    /* 
	ops we decided not to implement:
	bool  operator== (const NxQuat&) const;
	NxVec3  operator^  (const NxQuat& r_h_s) const;//same as normal quat rot, but casts itself into a vector.  (doesn't compute w term)
	NxQuat  operator*  (const NxVec3& v) const;//implicitly extends vector by a 0 w element.
	NxQuat  operator*  (const NxReal Scale) const;
	*/

	friend class NxMat33;
	private:
		NX_INLINE NxQuat(NxReal ix, NxReal iy, NxReal iz, NxReal iw);
	};




NX_INLINE NxQuat::NxQuat()
	{
	//nothing
	}


NX_INLINE NxQuat::NxQuat(const NxQuat& q) : x(q.x), y(q.y), z(q.z), w(q.w)
	{
	}


NX_INLINE NxQuat::NxQuat(const NxVec3& v, NxReal s)						// copy constructor, assumes w=0 
	{
	x = v.x;
	y = v.y;
	z = v.z;
	w = s;
	}


NX_INLINE NxQuat::NxQuat(const NxReal angle, const NxVec3 & axis)				// creates a NxQuat from an Angle axis -- note that if Angle > 360 the resulting rotation is Angle mod 360
	{
	fromAngleAxis(angle,axis);
	}


NX_INLINE void NxQuat::id()
	{
	x = NxReal(0);
	y = NxReal(0);
	z = NxReal(0);
	w = NxReal(1);
	}

NX_INLINE  bool NxQuat::isIdentityRotation() const
{
	return x==0 && y==0 && z==0 && fabsf(w)==1;
}


NX_INLINE void NxQuat::setWXYZ(NxReal sw, NxReal sx, NxReal sy, NxReal sz)
	{
	x = sx;
	y = sy;
	z = sz;
	w = sw;
	}


NX_INLINE void NxQuat::setXYZW(NxReal sx, NxReal sy, NxReal sz, NxReal sw)
	{
	x = sx;
	y = sy;
	z = sz;
	w = sw;
	}


NX_INLINE void NxQuat::setWXYZ(const NxReal * d)
	{
	x = d[1];
	y = d[2];
	z = d[3];
	w = d[0];
	}


NX_INLINE void NxQuat::setXYZW(const NxReal * d)
	{
	x = d[0];
	y = d[1];
	z = d[2];
	w = d[3];
	}


NX_INLINE void NxQuat::getWXYZ(NxF32 *d) const
	{
	d[1] = (NxF32)x;
	d[2] = (NxF32)y;
	d[3] = (NxF32)z;
	d[0] = (NxF32)w;
	}


NX_INLINE void NxQuat::getXYZW(NxF32 *d) const
	{
	d[0] = (NxF32)x;
	d[1] = (NxF32)y;
	d[2] = (NxF32)z;
	d[3] = (NxF32)w;
	}


NX_INLINE void NxQuat::getWXYZ(NxF64 *d) const
	{
	d[1] = (NxF64)x;
	d[2] = (NxF64)y;
	d[3] = (NxF64)z;
	d[0] = (NxF64)w;
	}


NX_INLINE void NxQuat::getXYZW(NxF64 *d) const
	{
	d[0] = (NxF64)x;
	d[1] = (NxF64)y;
	d[2] = (NxF64)z;
	d[3] = (NxF64)w;
	}

//const methods
 
NX_INLINE bool NxQuat::isFinite() const
	{
	return NxMath::isFinite(x) 
		&& NxMath::isFinite(y) 
		&& NxMath::isFinite(z)
		&& NxMath::isFinite(w);
	}



NX_INLINE void NxQuat::zero()
	{
	x = NxReal(0.0);
	y = NxReal(0.0);
	z = NxReal(0.0);
	w = NxReal(1.0);
	}


NX_INLINE void NxQuat::negate()
	{
	x = -x;
	y = -y;
	z = -z;
	w = -w;
	}

NX_INLINE NxQuat NxQuat::operator-() const
	{
	return NxQuat(-x,-y,-z,-w);
	}


NX_INLINE void NxQuat::random()
	{
	x = NxMath::rand(NxReal(0.0),NxReal(1.0));
	y = NxMath::rand(NxReal(0.0),NxReal(1.0));
	z = NxMath::rand(NxReal(0.0),NxReal(1.0));
	w = NxMath::rand(NxReal(0.0),NxReal(1.0));
	normalize();
	}


NX_INLINE void NxQuat::fromAngleAxis(NxReal Angle, const NxVec3 & axis)			// set the NxQuat by Angle-axis (see AA constructor)
	{
	x = axis.x;
	y = axis.y;
	z = axis.z;

	// required: Normalize the axis

	const NxReal i_length =  NxReal(1.0) / NxMath::sqrt( x*x + y*y + z*z );
	
	x = x * i_length;
	y = y * i_length;
	z = z * i_length;

	// now make a clQuaternionernion out of it
	NxReal Half = NxMath::degToRad(Angle * NxReal(0.5));

	w = NxMath::cos(Half);//this used to be w/o deg to rad.
	const NxReal sin_theta_over_two = NxMath::sin(Half );
	x = x * sin_theta_over_two;
	y = y * sin_theta_over_two;
	z = z * sin_theta_over_two;
	}

NX_INLINE void NxQuat::fromAngleAxisFast(NxReal AngleRadians, const NxVec3 & axis)
	{
	NxReal s;
	NxMath::sinCos(AngleRadians * 0.5f, s, w);
	x = axis.x * s;
	y = axis.y * s;
	z = axis.z * s;
	}

NX_INLINE void NxQuat::invert()
	{
	x = -x;
	y = -y;
	z = -z;
	}

NX_INLINE void NxQuat::setx(const NxReal& d) 
	{ 
	x = d;
	}


NX_INLINE void NxQuat::sety(const NxReal& d) 
	{ 
	y = d;
	}


NX_INLINE void NxQuat::setz(const NxReal& d) 
	{ 
	z = d;
	}


NX_INLINE void NxQuat::setw(const NxReal& d) 
	{ 
	w = d;
	}


NX_INLINE void NxQuat::getAngleAxis(NxReal& angle, NxVec3 & axis) const
	{
	//return axis and angle of rotation of quaternion
    angle = NxMath::acos(w) * NxReal(2.0);		//this is getAngle()
    NxReal sa = NxMath::sqrt(NxReal(1.0) - w*w);
	if (sa)
		{
		axis.set(x/sa,y/sa,z/sa);
		angle = NxMath::radToDeg(angle);
		}
	else
		axis.set(NxReal(1.0),NxReal(0.0),NxReal(0.0));

	}



NX_INLINE NxReal NxQuat::getAngle() const
	{
	return NxMath::acos(w) * NxReal(2.0);
	}



NX_INLINE NxReal NxQuat::getAngle(const NxQuat & q) const
	{
	return NxMath::acos(dot(q)) * NxReal(2.0);
	}


NX_INLINE NxReal NxQuat::magnitudeSquared() const

//modifiers:
	{
	return x*x + y*y + z*z + w*w;
	}


NX_INLINE NxReal NxQuat::dot(const NxQuat &v) const
	{
	return x * v.x + y * v.y + z * v.z  + w * v.w;
	}


NX_INLINE void NxQuat::normalize()											// convert this NxQuat to a unit clQuaternionernion
	{
	const NxReal mag = NxMath::sqrt(magnitudeSquared());
	if (mag)
		{
		const NxReal imag = NxReal(1.0) / mag;
		
		x *= imag;
		y *= imag;
		z *= imag;
		w *= imag;
		}
	}


NX_INLINE void NxQuat::conjugate()											// convert this NxQuat to a unit clQuaternionernion
	{
	x = -x;
	y = -y;
	z = -z;
	}


NX_INLINE void NxQuat::multiply(const NxQuat& left, const NxQuat& right)		// this = a * b
	{
	NxReal a,b,c,d;

	a =left.w*right.w - left.x*right.x - left.y*right.y - left.z*right.z;
	b =left.w*right.x + right.w*left.x + left.y*right.z - right.y*left.z;
	c =left.w*right.y + right.w*left.y + left.z*right.x - right.z*left.x;
	d =left.w*right.z + right.w*left.z + left.x*right.y - right.x*left.y;

	w = a;
	x = b;
	y = c;
	z = d;
	}


NX_INLINE void NxQuat::multiply(const NxQuat& left, const NxVec3& right)		// this = a * b
	{
	NxReal a,b,c,d;

	a = - left.x*right.x - left.y*right.y - left.z *right.z;
	b =   left.w*right.x + left.y*right.z - right.y*left.z;
	c =   left.w*right.y + left.z*right.x - right.z*left.x;
	d =   left.w*right.z + left.x*right.y - right.x*left.y;

	w = a;
	x = b;
	y = c;
	z = d;
	}

NX_INLINE void NxQuat::slerp(const NxReal t, const NxQuat& left, const NxQuat& right) // this = slerp(t, a, b)
	{
	const NxReal	quatEpsilon = (NxReal(1.0e-8f));

	*this = left;

	NxReal cosine = 
		x * right.x + 
		y * right.y + 
		z * right.z + 
		w * right.w;		//this is left.dot(right)

	NxReal sign = NxReal(1);
	if (cosine < 0)
		{
		cosine = - cosine;
		sign = NxReal(-1);
		}

	NxReal Sin = NxReal(1) - cosine*cosine;

	if(Sin>=quatEpsilon*quatEpsilon)	
		{
		Sin = NxMath::sqrt(Sin);
		const NxReal angle = NxMath::atan2(Sin, cosine);
		const NxReal i_sin_angle = NxReal(1) / Sin;



		NxReal lower_weight = NxMath::sin(angle*(NxReal(1)-t)) * i_sin_angle;
		NxReal upper_weight = NxMath::sin(angle * t) * i_sin_angle * sign;

		w = (w * (lower_weight)) + (right.w * (upper_weight));
		x = (x * (lower_weight)) + (right.x * (upper_weight));
		y = (y * (lower_weight)) + (right.y * (upper_weight));
		z = (z * (lower_weight)) + (right.z * (upper_weight));
		}
	}


NX_INLINE void NxQuat::rotate(NxVec3 & v) const						//rotates passed vec by rot expressed by quaternion.  overwrites arg ith the result.
	{
	//NxReal msq = NxReal(1.0)/magnitudeSquared();	//assume unit quat!
	NxQuat myInverse;
	myInverse.x = -x;//*msq;
	myInverse.y = -y;//*msq;
	myInverse.z = -z;//*msq;
	myInverse.w =  w;//*msq;

	//v = ((*this) * v) ^ myInverse;

	NxQuat left;
	left.multiply(*this,v);
	v.x =left.w*myInverse.x + myInverse.w*left.x + left.y*myInverse.z - myInverse.y*left.z;
	v.y =left.w*myInverse.y + myInverse.w*left.y + left.z*myInverse.x - myInverse.z*left.x;
	v.z =left.w*myInverse.z + myInverse.w*left.z + left.x*myInverse.y - myInverse.x*left.y;
	}


NX_INLINE void NxQuat::inverseRotate(NxVec3 & v) const				//rotates passed vec by opposite of rot expressed by quaternion.  overwrites arg ith the result.
	{
	//NxReal msq = NxReal(1.0)/magnitudeSquared();	//assume unit quat!
	NxQuat myInverse;
	myInverse.x = -x;//*msq;
	myInverse.y = -y;//*msq;
	myInverse.z = -z;//*msq;
	myInverse.w =  w;//*msq;

	//v = (myInverse * v) ^ (*this);
	NxQuat left;
	left.multiply(myInverse,v);
	v.x =left.w*x + w*left.x + left.y*z - y*left.z;
	v.y =left.w*y + w*left.y + left.z*x - z*left.x;
	v.z =left.w*z + w*left.z + left.x*y - x*left.y;
	}


NX_INLINE NxQuat& NxQuat::operator=  (const NxQuat& q)
	{
	x = q.x;
	y = q.y;
	z = q.z;
	w = q.w;
	return *this;
	}

#if 0
NX_INLINE NxQuat& NxQuat::operator=  (const NxVec3& v)		//implicitly extends vector by a 0 w element.
	{
	x = v.x;
	y = v.y;
	z = v.z;
	w = NxReal(1.0);
	return *this;
	}
#endif

NX_INLINE NxQuat& NxQuat::operator*= (const NxQuat& q)
	{
	NxReal xx[4]; //working Quaternion
	xx[0] = w*q.w - q.x*x - y*q.y - q.z*z;
	xx[1] = w*q.x + q.w*x + y*q.z - q.y*z;
	xx[2] = w*q.y + q.w*y + z*q.x - q.z*x;
	z=w*q.z + q.w*z + x*q.y - q.x*y;

	w = xx[0];
	x = xx[1];
	y = xx[2];
	return *this;
	}


NX_INLINE NxQuat& NxQuat::operator+= (const NxQuat& q)
	{
	x+=q.x;
	y+=q.y;
	z+=q.z;
	w+=q.w;
	return *this;
	}


NX_INLINE NxQuat& NxQuat::operator-= (const NxQuat& q)
	{
	x-=q.x;
	y-=q.y;
	z-=q.z;
	w-=q.w;
	return *this;
	}


NX_INLINE NxQuat& NxQuat::operator*= (const NxReal s)
	{
	x*=s;
	y*=s;
	z*=s;
	w*=s;
	return *this;
	}

NX_INLINE NxQuat::NxQuat(NxReal ix, NxReal iy, NxReal iz, NxReal iw)
{
	x = ix;
	y = iy;
	z = iz;
	w = iw;
}

NX_INLINE NxQuat NxQuat::operator*(const NxQuat &q) const
{
	return NxQuat(w*q.x + q.w*x + y*q.z - q.y*z,
				  w*q.y + q.w*y + z*q.x - q.z*x,
				  w*q.z + q.w*z + x*q.y - q.x*y,
				  w*q.w - x*q.x - y*q.y - z*q.z);
}

NX_INLINE NxQuat NxQuat::operator+(const NxQuat &q) const
{
	return NxQuat(x+q.x,y+q.y,z+q.z,w+q.w);
}

NX_INLINE NxQuat NxQuat::operator-(const NxQuat &q) const
{
	return NxQuat(x-q.x,y-q.y,z-q.z,w-q.w);
}

NX_INLINE NxQuat NxQuat::operator!() const
{
	return NxQuat(-x,-y,-z,w);
}



NX_INLINE const NxVec3 NxQuat::rot(const NxVec3 &v) const
    {
	NxVec3 qv(x,y,z);

	return (v*(w*w-0.5f) + (qv^v)*w + qv*(qv|v))*2;
    }

NX_INLINE const NxVec3 NxQuat::invRot(const NxVec3 &v) const
    {
	NxVec3 qv(x,y,z);

	return (v*(w*w-0.5f) - (qv^v)*w + qv*(qv|v))*2;
    }



NX_INLINE const NxVec3 NxQuat::transform(const NxVec3 &v, const NxVec3 &p) const
    {
	return rot(v)+p;
    }

NX_INLINE const NxVec3 NxQuat::invTransform(const NxVec3 &v, const NxVec3 &p) const
    {
	return invRot(v-p);
    }

 /** @} */
#endif
#endif
//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND

#ifndef NX_FOUNDATION_NX9F32
#define NX_FOUNDATION_NX9F32
/*----------------------------------------------------------------------------*\
|
|						Public Interface to Ageia PhysX Technology
|
|							     www.ageia.com
|
\*----------------------------------------------------------------------------*/
/** \addtogroup foundation
  @{
*/

//Exclude file from docs
/** \cond */

//the file name of this header is legacy due to pain of renaming file in repository.

class Nx9Real
	{
	
	public:
        struct S
			{
#ifndef TRANSPOSED_MAT33
			NxReal        _11, _12, _13;
			NxReal        _21, _22, _23;
			NxReal        _31, _32, _33;
#else
			NxReal        _11, _21, _31;
			NxReal        _12, _22, _32;
			NxReal        _13, _23, _33;
#endif
			};
	
    union 
		{
		S s;
		NxReal m[3][3];
		};
	};

/** \endcond */
 /** @} */
#endif
//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND


#ifndef NX_FOUNDATION_NxMat33T
#define NX_FOUNDATION_NxMat33T
/*----------------------------------------------------------------------------*\
|
|						Public Interface to Ageia PhysX Technology
|
|							     www.ageia.com
|
\*----------------------------------------------------------------------------*/
/** \addtogroup foundation
  @{
*/
#ifdef __PPCGEKKO__
#include "wii\NxMat33_Wii.h"
#else

/**
\brief Identifies a special matrix. Can be passed to the #NxMat33 constructor.
*/
enum NxMatrixType
	{
	/**
	\brief Matrix of all zeros.
	*/
	NX_ZERO_MATRIX,

	/**
	\brief Identity matrix.
	*/
	NX_IDENTITY_MATRIX
	};


typedef Nx9Real Mat33DataType;	//takes Nx9Real, Nx12Real, Nx16Real

/**
\brief 3x3 Matrix Class.

 The idea of the matrix/vector classes is to partition them into two parts:
 One is the data structure which may have different formatting (3x3, 3x4, 4x4),
 row or column major.  The other is a template class which has all the operators
 but is storage format independent.

 This way it should be easier to change formats depending on what is faster/slower
 on a particular platform.

 Design issue: We use nameless struct/unions here.
 Design issue: this used to be implemented with a template.  This had no benefit
 but it added syntactic complexity.  Currently we just use a typedef and a preprocessor switch 
 to change between different memory layouts.

 The matrix math in this class is storage format (row/col major) independent as far
 as the user is concerned.
 When the user wants to get/set raw data, he needs to specify what order the data is
 coming in.  
 
*/
class NxMat33
	{
	public:
	NX_INLINE NxMat33();

	/**
	\param type Special matrix type to initialize with.

	@see NxMatrixType
	*/
	NX_INLINE NxMat33(NxMatrixType type);
	NX_INLINE NxMat33(const NxVec3 &row0, const NxVec3 &row1, const NxVec3 &row2);

	NX_INLINE NxMat33(const NxMat33&m);
	NX_INLINE NxMat33(const NxQuat &m);
	NX_INLINE ~NxMat33();
	NX_INLINE const NxMat33& operator=(const NxMat33 &src);

	// Access elements

	//low level data access, single or double precision, with eventual translation:
	//for dense 9 element data
	NX_INLINE void setRowMajor(const NxF32 *);
	NX_INLINE void setRowMajor(const NxF32 d[][3]);
	NX_INLINE void setColumnMajor(const NxF32 *);
	NX_INLINE void setColumnMajor(const NxF32 d[][3]);
	NX_INLINE void getRowMajor(NxF32 *) const;
	NX_INLINE void getRowMajor(NxF32 d[][3]) const;
	NX_INLINE void getColumnMajor(NxF32 *) const;
	NX_INLINE void getColumnMajor(NxF32 d[][3]) const;

	NX_INLINE void setRowMajor(const NxF64 *);
	NX_INLINE void setRowMajor(const NxF64 d[][3]);
	NX_INLINE void setColumnMajor(const NxF64 *);
	NX_INLINE void setColumnMajor(const NxF64 d[][3]);
	NX_INLINE void getRowMajor(NxF64 *) const;
	NX_INLINE void getRowMajor(NxF64 d[][3]) const;
	NX_INLINE void getColumnMajor(NxF64 *) const;
	NX_INLINE void getColumnMajor(NxF64 d[][3]) const;


	//for loose 4-padded data.
	NX_INLINE void setRowMajorStride4(const NxF32 *);
	NX_INLINE void setRowMajorStride4(const NxF32 d[][4]);
	NX_INLINE void setColumnMajorStride4(const NxF32 *);
	NX_INLINE void setColumnMajorStride4(const NxF32 d[][4]);
	NX_INLINE void getRowMajorStride4(NxF32 *) const;
	NX_INLINE void getRowMajorStride4(NxF32 d[][4]) const;
	NX_INLINE void getColumnMajorStride4(NxF32 *) const;
	NX_INLINE void getColumnMajorStride4(NxF32 d[][4]) const;

	NX_INLINE void setRowMajorStride4(const NxF64 *);
	NX_INLINE void setRowMajorStride4(const NxF64 d[][4]);
	NX_INLINE void setColumnMajorStride4(const NxF64 *);
	NX_INLINE void setColumnMajorStride4(const NxF64 d[][4]);
	NX_INLINE void getRowMajorStride4(NxF64 *) const;
	NX_INLINE void getRowMajorStride4(NxF64 d[][4]) const;
	NX_INLINE void getColumnMajorStride4(NxF64 *) const;
	NX_INLINE void getColumnMajorStride4(NxF64 d[][4]) const;


	NX_INLINE void setRow(int row, const NxVec3 &);
	NX_INLINE void setColumn(int col, const NxVec3 &);
	NX_INLINE void getRow(int row, NxVec3 &) const;
	NX_INLINE void getColumn(int col, NxVec3 &) const;

	NX_INLINE NxVec3 getRow(int row) const;
	NX_INLINE NxVec3 getColumn(int col) const;


	//element access:
    NX_INLINE NxReal & operator()(int row, int col);
    NX_INLINE const NxReal & operator() (int row, int col) const;

	/**
	\brief returns true for identity matrix
	*/
	NX_INLINE bool isIdentity() const;

	/**
	\brief returns true for zero matrix
	*/
	NX_INLINE bool isZero() const;

	/**
	\brief returns true if all elems are finite (not NAN or INF, etc.)
	*/
	NX_INLINE bool isFinite() const;

	//create special matrices:

	/**
	\brief sets this matrix to the zero matrix.
	*/
	NX_INLINE void zero();

	/**
	\brief sets this matrix to the identity matrix.
	*/
	NX_INLINE void id();

	/**
	\brief this = -this
	*/
	NX_INLINE void setNegative();

	/**
	\brief sets this matrix to the diagonal matrix.
	*/
	NX_INLINE void diagonal(const NxVec3 &vec);

	/**
	\brief Sets this matrix to the Star(Skew Symetric) matrix.

	So that star(v) * x = v.cross(x) .
	*/
	NX_INLINE void star(const NxVec3 &vec);


	NX_INLINE void fromQuat(const NxQuat &);
	NX_INLINE void toQuat(NxQuat &) const;

	//modifications:

	NX_INLINE const NxMat33 &operator +=(const NxMat33 &s);
	NX_INLINE const NxMat33 &operator -=(const NxMat33 &s);
	NX_INLINE const NxMat33 &operator *=(NxReal s);
	NX_INLINE const NxMat33 &operator /=(NxReal s);

	/*
	Gram-Schmidt orthogonalization to correct numerical drift, plus column normalization
	Caution: I believe the current implementation does not work right!
	*/
//	NX_INLINE void orthonormalize();


	/**
	\brief returns determinant
	*/
	NX_INLINE NxReal determinant() const;

	/**
	\brief assigns inverse to dest.
	
	Returns false if singular (i.e. if no inverse exists), setting dest to identity.
	*/
	NX_INLINE bool getInverse(NxMat33& dest) const;

	/**
	\brief this = transpose(other)

	this == other is OK.
	*/
	NX_INLINE void setTransposed(const NxMat33& other);

	/**
	\brief this = transpose(this)
	*/
	NX_INLINE void setTransposed();

	/**
	\brief this = this * [ d.x 0 0; 0 d.y 0; 0 0 d.z];
	*/
	NX_INLINE void multiplyDiagonal(const NxVec3 &d);

	/**
	\brief this = transpose(this) * [ d.x 0 0; 0 d.y 0; 0 0 d.z];
	*/
	NX_INLINE void multiplyDiagonalTranspose(const NxVec3 &d);

	/**
	\brief dst = this * [ d.x 0 0; 0 d.y 0; 0 0 d.z];
	*/
	NX_INLINE void multiplyDiagonal(const NxVec3 &d, NxMat33 &dst) const;

	/**
	\brief dst = transpose(this) * [ d.x 0 0; 0 d.y 0; 0 0 d.z];
	*/
	NX_INLINE void multiplyDiagonalTranspose(const NxVec3 &d, NxMat33 &dst) const;

	/**
	\brief dst = this * src
	*/
	NX_INLINE void multiply(const NxVec3 &src, NxVec3 &dst) const;
	/**
	\brief dst = transpose(this) * src
	*/
	NX_INLINE void multiplyByTranspose(const NxVec3 &src, NxVec3 &dst) const;

	/**
	\brief this = a + b
	*/
	NX_INLINE void  add(const NxMat33 & a, const NxMat33 & b);
	/***
	\brief this = a - b
	*/
	NX_INLINE void  subtract(const NxMat33 &a, const NxMat33 &b);
	/**
	\brief this = s * a;
	*/
	NX_INLINE void  multiply(NxReal s,  const NxMat33 & a);
	/**
	\brief this = left * right
	*/
	NX_INLINE void multiply(const NxMat33& left, const NxMat33& right);
	/**
	\brief this = transpose(left) * right

	\note #multiplyByTranspose() is faster.
	*/
	NX_INLINE void multiplyTransposeLeft(const NxMat33& left, const NxMat33& right);
	/**
	\brief this = left * transpose(right)
	
	\note faster than #multiplyByTranspose().
	*/
	NX_INLINE void multiplyTransposeRight(const NxMat33& left, const NxMat33& right);

	/**
	\brief this = left * transpose(right)
	*/
	NX_INLINE void multiplyTransposeRight(const NxVec3 &left, const NxVec3 &right);

	/**
	\brief this = rotation matrix around X axis

	<b>Unit:</b> Radians
	*/
	NX_INLINE void rotX(NxReal angle);

	/**
	\brief this = rotation matrix around Y axis

	<b>Unit:</b> Radians
	*/
	NX_INLINE void rotY(NxReal angle);

	/**
	\brief this = rotation matrix around Z axis

	<b>Unit:</b> Radians
	*/
	NX_INLINE void rotZ(NxReal angle);


	//overloaded multiply, and transposed-multiply ops:

	/**
	\brief returns transpose(this)*src
	*/
	NX_INLINE NxVec3 operator%  (const NxVec3 & src) const;
	/**
	\brief matrix vector product
	*/
	NX_INLINE NxVec3 operator*  (const NxVec3 & src) const;
	/**
	\brief matrix product
	*/
	NX_INLINE NxMat33&	operator*= (const NxMat33& mat);
	/**
	\brief matrix difference
	*/
	NX_INLINE NxMat33	operator-  (const NxMat33& mat)	const;
	/**
	\brief matrix addition
	*/
	NX_INLINE NxMat33	operator+  (const NxMat33& mat)	const;
	/**
	\brief matrix product
	*/
	NX_INLINE NxMat33	operator*  (const NxMat33& mat)	const;
	/**
	\brief matrix scalar product
	*/
	NX_INLINE NxMat33	operator*  (float s)				const;

	private:
	Mat33DataType data;
	};


NX_INLINE NxMat33::NxMat33()
	{
	}


NX_INLINE NxMat33::NxMat33(NxMatrixType type)
	{
		switch(type)
		{
			case NX_ZERO_MATRIX:		zero();	break;
			case NX_IDENTITY_MATRIX:	id();	break;
		}
	}


NX_INLINE NxMat33::NxMat33(const NxMat33& a)
	{
	data = a.data;
	}


NX_INLINE NxMat33::NxMat33(const NxQuat &q)
	{
	fromQuat(q);
	}

NX_INLINE NxMat33::NxMat33(const NxVec3 &row0, const NxVec3 &row1, const NxVec3 &row2)
{
	data.s._11 = row0.x;  data.s._12 = row0.y;  data.s._13 = row0.z;
	data.s._21 = row1.x;  data.s._22 = row1.y;  data.s._23 = row1.z;
	data.s._31 = row2.x;  data.s._32 = row2.y;  data.s._33 = row2.z;
}


NX_INLINE NxMat33::~NxMat33()
	{
	//nothing
	}


NX_INLINE const NxMat33& NxMat33::operator=(const NxMat33 &a)
	{
	data = a.data;
	return *this;
	}


NX_INLINE void NxMat33::setRowMajor(const NxF32* d)
	{
	//we are also row major, so this is a direct copy
	data.s._11 = (NxReal)d[0];
	data.s._12 = (NxReal)d[1];
	data.s._13 = (NxReal)d[2];

	data.s._21 = (NxReal)d[3];
	data.s._22 = (NxReal)d[4];
	data.s._23 = (NxReal)d[5];

	data.s._31 = (NxReal)d[6];
	data.s._32 = (NxReal)d[7];
	data.s._33 = (NxReal)d[8];
	}


NX_INLINE void NxMat33::setRowMajor(const NxF32 d[][3])
	{
	//we are also row major, so this is a direct copy
	data.s._11 = (NxReal)d[0][0];
	data.s._12 = (NxReal)d[0][1];
	data.s._13 = (NxReal)d[0][2];

	data.s._21 = (NxReal)d[1][0];
	data.s._22 = (NxReal)d[1][1];
	data.s._23 = (NxReal)d[1][2];

	data.s._31 = (NxReal)d[2][0];
	data.s._32 = (NxReal)d[2][1];
	data.s._33 = (NxReal)d[2][2];
	}


NX_INLINE void NxMat33::setColumnMajor(const NxF32* d)
	{
	//we are column major, so copy transposed.
	data.s._11 = (NxReal)d[0];
	data.s._12 = (NxReal)d[3];
	data.s._13 = (NxReal)d[6];

	data.s._21 = (NxReal)d[1];
	data.s._22 = (NxReal)d[4];
	data.s._23 = (NxReal)d[7];

	data.s._31 = (NxReal)d[2];
	data.s._32 = (NxReal)d[5];
	data.s._33 = (NxReal)d[8];
	}


NX_INLINE void NxMat33::setColumnMajor(const NxF32 d[][3])
	{
	//we are column major, so copy transposed.
	data.s._11 = (NxReal)d[0][0];
	data.s._12 = (NxReal)d[1][0];
	data.s._13 = (NxReal)d[2][0];

	data.s._21 = (NxReal)d[0][1];
	data.s._22 = (NxReal)d[1][1];
	data.s._23 = (NxReal)d[2][1];

	data.s._31 = (NxReal)d[0][2];
	data.s._32 = (NxReal)d[1][2];
	data.s._33 = (NxReal)d[2][2];
	}


NX_INLINE void NxMat33::getRowMajor(NxF32* d) const
	{
	//we are also row major, so this is a direct copy
	d[0] = (NxF32)data.s._11;
	d[1] = (NxF32)data.s._12;
	d[2] = (NxF32)data.s._13;

	d[3] = (NxF32)data.s._21;
	d[4] = (NxF32)data.s._22;
	d[5] = (NxF32)data.s._23;

	d[6] = (NxF32)data.s._31;
	d[7] = (NxF32)data.s._32;
	d[8] = (NxF32)data.s._33;
	}


NX_INLINE void NxMat33::getRowMajor(NxF32 d[][3]) const
	{
	//we are also row major, so this is a direct copy
	d[0][0] = (NxF32)data.s._11;
	d[0][1] = (NxF32)data.s._12;
	d[0][2] = (NxF32)data.s._13;

	d[1][0] = (NxF32)data.s._21;
	d[1][1] = (NxF32)data.s._22;
	d[1][2] = (NxF32)data.s._23;

	d[2][0] = (NxF32)data.s._31;
	d[2][1] = (NxF32)data.s._32;
	d[2][2] = (NxF32)data.s._33;
	}


NX_INLINE void NxMat33::getColumnMajor(NxF32* d) const
	{
	//we are column major, so copy transposed.
	d[0] = (NxF32)data.s._11;
	d[3] = (NxF32)data.s._12;
	d[6] = (NxF32)data.s._13;

	d[1] = (NxF32)data.s._21;
	d[4] = (NxF32)data.s._22;
	d[7] = (NxF32)data.s._23;

	d[2] = (NxF32)data.s._31;
	d[5] = (NxF32)data.s._32;
	d[8] = (NxF32)data.s._33;
	}


NX_INLINE void NxMat33::getColumnMajor(NxF32 d[][3]) const
	{
	//we are column major, so copy transposed.
	d[0][0] = (NxF32)data.s._11;
	d[1][0] = (NxF32)data.s._12;
	d[2][0] = (NxF32)data.s._13;

	d[0][1] = (NxF32)data.s._21;
	d[1][1] = (NxF32)data.s._22;
	d[2][1] = (NxF32)data.s._23;

	d[0][2] = (NxF32)data.s._31;
	d[1][2] = (NxF32)data.s._32;
	d[2][2] = (NxF32)data.s._33;
	}


NX_INLINE void NxMat33::setRowMajorStride4(const NxF32* d)
	{
	//we are also row major, so this is a direct copy
	//however we've got to skip every 4th element.
	data.s._11 = (NxReal)d[0];
	data.s._12 = (NxReal)d[1];
	data.s._13 = (NxReal)d[2];

	data.s._21 = (NxReal)d[4];
	data.s._22 = (NxReal)d[5];
	data.s._23 = (NxReal)d[6];

	data.s._31 = (NxReal)d[8];
	data.s._32 = (NxReal)d[9];
	data.s._33 = (NxReal)d[10];
	}


NX_INLINE void NxMat33::setRowMajorStride4(const NxF32 d[][4])
	{
	//we are also row major, so this is a direct copy
	data.s._11 = (NxReal)d[0][0];
	data.s._12 = (NxReal)d[0][1];
	data.s._13 = (NxReal)d[0][2];

	data.s._21 = (NxReal)d[1][0];
	data.s._22 = (NxReal)d[1][1];
	data.s._23 = (NxReal)d[1][2];

	data.s._31 = (NxReal)d[2][0];
	data.s._32 = (NxReal)d[2][1];
	data.s._33 = (NxReal)d[2][2];
	}


NX_INLINE void NxMat33::setColumnMajorStride4(const NxF32* d)
	{
	//we are column major, so copy transposed.
	//however we've got to skip every 4th element.
	data.s._11 = (NxReal)d[0];
	data.s._12 = (NxReal)d[4];
	data.s._13 = (NxReal)d[8];

	data.s._21 = (NxReal)d[1];
	data.s._22 = (NxReal)d[5];
	data.s._23 = (NxReal)d[9];

	data.s._31 = (NxReal)d[2];
	data.s._32 = (NxReal)d[6];
	data.s._33 = (NxReal)d[10];
	}


NX_INLINE void NxMat33::setColumnMajorStride4(const NxF32 d[][4])
	{
	//we are column major, so copy transposed.
	data.s._11 = (NxReal)d[0][0];
	data.s._12 = (NxReal)d[1][0];
	data.s._13 = (NxReal)d[2][0];

	data.s._21 = (NxReal)d[0][1];
	data.s._22 = (NxReal)d[1][1];
	data.s._23 = (NxReal)d[2][1];

	data.s._31 = (NxReal)d[0][2];
	data.s._32 = (NxReal)d[1][2];
	data.s._33 = (NxReal)d[2][2];
	}


NX_INLINE void NxMat33::getRowMajorStride4(NxF32* d) const
	{
	//we are also row major, so this is a direct copy
	//however we've got to skip every 4th element.
	d[0] = (NxF32)data.s._11;
	d[1] = (NxF32)data.s._12;
	d[2] = (NxF32)data.s._13;

	d[4] = (NxF32)data.s._21;
	d[5] = (NxF32)data.s._22;
	d[6] = (NxF32)data.s._23;

	d[8] = (NxF32)data.s._31;
	d[9] = (NxF32)data.s._32;
	d[10]= (NxF32)data.s._33;
	}


NX_INLINE void NxMat33::getRowMajorStride4(NxF32 d[][4]) const
	{
	//we are also row major, so this is a direct copy
	d[0][0] = (NxF32)data.s._11;
	d[0][1] = (NxF32)data.s._12;
	d[0][2] = (NxF32)data.s._13;

	d[1][0] = (NxF32)data.s._21;
	d[1][1] = (NxF32)data.s._22;
	d[1][2] = (NxF32)data.s._23;

	d[2][0] = (NxF32)data.s._31;
	d[2][1] = (NxF32)data.s._32;
	d[2][2] = (NxF32)data.s._33;
	}


NX_INLINE void NxMat33::getColumnMajorStride4(NxF32* d) const
	{
	//we are column major, so copy transposed.
	//however we've got to skip every 4th element.
	d[0] = (NxF32)data.s._11;
	d[4] = (NxF32)data.s._12;
	d[8] = (NxF32)data.s._13;

	d[1] = (NxF32)data.s._21;
	d[5] = (NxF32)data.s._22;
	d[9] = (NxF32)data.s._23;

	d[2] = (NxF32)data.s._31;
	d[6] = (NxF32)data.s._32;
	d[10]= (NxF32)data.s._33;
	}


NX_INLINE void NxMat33::getColumnMajorStride4(NxF32 d[][4]) const
	{
	//we are column major, so copy transposed.
	d[0][0] = (NxF32)data.s._11;
	d[1][0] = (NxF32)data.s._12;
	d[2][0] = (NxF32)data.s._13;

	d[0][1] = (NxF32)data.s._21;
	d[1][1] = (NxF32)data.s._22;
	d[2][1] = (NxF32)data.s._23;

	d[0][2] = (NxF32)data.s._31;
	d[1][2] = (NxF32)data.s._32;
	d[2][2] = (NxF32)data.s._33;
	}


NX_INLINE void NxMat33::setRowMajor(const NxF64*d)
	{
	//we are also row major, so this is a direct copy
	data.s._11 = (NxReal)d[0];
	data.s._12 = (NxReal)d[1];
	data.s._13 = (NxReal)d[2];

	data.s._21 = (NxReal)d[3];
	data.s._22 = (NxReal)d[4];
	data.s._23 = (NxReal)d[5];

	data.s._31 = (NxReal)d[6];
	data.s._32 = (NxReal)d[7];
	data.s._33 = (NxReal)d[8];
	}


NX_INLINE void NxMat33::setRowMajor(const NxF64 d[][3])
	{
	//we are also row major, so this is a direct copy
	data.s._11 = (NxReal)d[0][0];
	data.s._12 = (NxReal)d[0][1];
	data.s._13 = (NxReal)d[0][2];

	data.s._21 = (NxReal)d[1][0];
	data.s._22 = (NxReal)d[1][1];
	data.s._23 = (NxReal)d[1][2];

	data.s._31 = (NxReal)d[2][0];
	data.s._32 = (NxReal)d[2][1];
	data.s._33 = (NxReal)d[2][2];
	}


NX_INLINE void NxMat33::setColumnMajor(const NxF64*d)
	{
	//we are column major, so copy transposed.
	data.s._11 = (NxReal)d[0];
	data.s._12 = (NxReal)d[3];
	data.s._13 = (NxReal)d[6];

	data.s._21 = (NxReal)d[1];
	data.s._22 = (NxReal)d[4];
	data.s._23 = (NxReal)d[7];

	data.s._31 = (NxReal)d[2];
	data.s._32 = (NxReal)d[5];
	data.s._33 = (NxReal)d[8];
	}


NX_INLINE void NxMat33::setColumnMajor(const NxF64 d[][3])
	{
	//we are column major, so copy transposed.
	data.s._11 = (NxReal)d[0][0];
	data.s._12 = (NxReal)d[1][0];
	data.s._13 = (NxReal)d[2][0];

	data.s._21 = (NxReal)d[0][1];
	data.s._22 = (NxReal)d[1][1];
	data.s._23 = (NxReal)d[2][1];

	data.s._31 = (NxReal)d[0][2];
	data.s._32 = (NxReal)d[1][2];
	data.s._33 = (NxReal)d[2][2];
	}


NX_INLINE void NxMat33::getRowMajor(NxF64*d) const
	{
	//we are also row major, so this is a direct copy
	d[0] = (NxF64)data.s._11;
	d[1] = (NxF64)data.s._12;
	d[2] = (NxF64)data.s._13;

	d[3] = (NxF64)data.s._21;
	d[4] = (NxF64)data.s._22;
	d[5] = (NxF64)data.s._23;

	d[6] = (NxF64)data.s._31;
	d[7] = (NxF64)data.s._32;
	d[8] = (NxF64)data.s._33;
	}


NX_INLINE void NxMat33::getRowMajor(NxF64 d[][3]) const
	{
	//we are also row major, so this is a direct copy
	d[0][0] = (NxF64)data.s._11;
	d[0][1] = (NxF64)data.s._12;
	d[0][2] = (NxF64)data.s._13;

	d[1][0] = (NxF64)data.s._21;
	d[1][1] = (NxF64)data.s._22;
	d[1][2] = (NxF64)data.s._23;

	d[2][0] = (NxF64)data.s._31;
	d[2][1] = (NxF64)data.s._32;
	d[2][2] = (NxF64)data.s._33;
	}


NX_INLINE void NxMat33::getColumnMajor(NxF64*d) const
	{
	//we are column major, so copy transposed.
	d[0] = (NxF64)data.s._11;
	d[3] = (NxF64)data.s._12;
	d[6] = (NxF64)data.s._13;

	d[1] = (NxF64)data.s._21;
	d[4] = (NxF64)data.s._22;
	d[7] = (NxF64)data.s._23;

	d[2] = (NxF64)data.s._31;
	d[5] = (NxF64)data.s._32;
	d[8] = (NxF64)data.s._33;
	}


NX_INLINE void NxMat33::getColumnMajor(NxF64 d[][3]) const
	{
	//we are column major, so copy transposed.
	d[0][0] = (NxF64)data.s._11;
	d[1][0] = (NxF64)data.s._12;
	d[2][0] = (NxF64)data.s._13;

	d[0][1] = (NxF64)data.s._21;
	d[1][1] = (NxF64)data.s._22;
	d[2][1] = (NxF64)data.s._23;

	d[0][2] = (NxF64)data.s._31;
	d[1][2] = (NxF64)data.s._32;
	d[2][2] = (NxF64)data.s._33;
	}


NX_INLINE void NxMat33::setRowMajorStride4(const NxF64*d)
	{
	//we are also row major, so this is a direct copy
	//however we've got to skip every 4th element.
	data.s._11 = (NxReal)d[0];
	data.s._12 = (NxReal)d[1];
	data.s._13 = (NxReal)d[2];

	data.s._21 = (NxReal)d[4];
	data.s._22 = (NxReal)d[5];
	data.s._23 = (NxReal)d[6];

	data.s._31 = (NxReal)d[8];
	data.s._32 = (NxReal)d[9];
	data.s._33 = (NxReal)d[10];
	}


NX_INLINE void NxMat33::setRowMajorStride4(const NxF64 d[][4])
	{
	//we are also row major, so this is a direct copy
	data.s._11 = (NxReal)d[0][0];
	data.s._12 = (NxReal)d[0][1];
	data.s._13 = (NxReal)d[0][2];

	data.s._21 = (NxReal)d[1][0];
	data.s._22 = (NxReal)d[1][1];
	data.s._23 = (NxReal)d[1][2];

	data.s._31 = (NxReal)d[2][0];
	data.s._32 = (NxReal)d[2][1];
	data.s._33 = (NxReal)d[2][2];
	}


NX_INLINE void NxMat33::setColumnMajorStride4(const NxF64*d)
	{
	//we are column major, so copy transposed.
	//however we've got to skip every 4th element.
	data.s._11 = (NxReal)d[0];
	data.s._12 = (NxReal)d[4];
	data.s._13 = (NxReal)d[8];

	data.s._21 = (NxReal)d[1];
	data.s._22 = (NxReal)d[5];
	data.s._23 = (NxReal)d[9];

	data.s._31 = (NxReal)d[2];
	data.s._32 = (NxReal)d[6];
	data.s._33 = (NxReal)d[10];
	}


NX_INLINE void NxMat33::setColumnMajorStride4(const NxF64 d[][4])
	{
	//we are column major, so copy transposed.
	data.s._11 = (NxReal)d[0][0];
	data.s._12 = (NxReal)d[1][0];
	data.s._13 = (NxReal)d[2][0];

	data.s._21 = (NxReal)d[0][1];
	data.s._22 = (NxReal)d[1][1];
	data.s._23 = (NxReal)d[2][1];

	data.s._31 = (NxReal)d[0][2];
	data.s._32 = (NxReal)d[1][2];
	data.s._33 = (NxReal)d[2][2];
	}


NX_INLINE void NxMat33::getRowMajorStride4(NxF64*d) const
	{
	//we are also row major, so this is a direct copy
	//however we've got to skip every 4th element.
	d[0] = (NxF64)data.s._11;
	d[1] = (NxF64)data.s._12;
	d[2] = (NxF64)data.s._13;

	d[4] = (NxF64)data.s._21;
	d[5] = (NxF64)data.s._22;
	d[6] = (NxF64)data.s._23;

	d[8] = (NxF64)data.s._31;
	d[9] = (NxF64)data.s._32;
	d[10]= (NxF64)data.s._33;
	}


NX_INLINE void NxMat33::getRowMajorStride4(NxF64 d[][4]) const
	{
	//we are also row major, so this is a direct copy
	d[0][0] = (NxF64)data.s._11;
	d[0][1] = (NxF64)data.s._12;
	d[0][2] = (NxF64)data.s._13;

	d[1][0] = (NxF64)data.s._21;
	d[1][1] = (NxF64)data.s._22;
	d[1][2] = (NxF64)data.s._23;

	d[2][0] = (NxF64)data.s._31;
	d[2][1] = (NxF64)data.s._32;
	d[2][2] = (NxF64)data.s._33;
	}


NX_INLINE void NxMat33::getColumnMajorStride4(NxF64*d) const

	{
	//we are column major, so copy transposed.
	//however we've got to skip every 4th element.
	d[0] = (NxF64)data.s._11;
	d[4] = (NxF64)data.s._12;
	d[8] = (NxF64)data.s._13;

	d[1] = (NxF64)data.s._21;
	d[5] = (NxF64)data.s._22;
	d[9] = (NxF64)data.s._23;

	d[2] = (NxF64)data.s._31;
	d[6] = (NxF64)data.s._32;
	d[10]= (NxF64)data.s._33;
	}


NX_INLINE void NxMat33::getColumnMajorStride4(NxF64 d[][4]) const
	{
	//we are column major, so copy transposed.
	d[0][0] = (NxF64)data.s._11;
	d[1][0] = (NxF64)data.s._12;
	d[2][0] = (NxF64)data.s._13;

	d[0][1] = (NxF64)data.s._21;
	d[1][1] = (NxF64)data.s._22;
	d[2][1] = (NxF64)data.s._23;

	d[0][2] = (NxF64)data.s._31;
	d[1][2] = (NxF64)data.s._32;
	d[2][2] = (NxF64)data.s._33;
	}


NX_INLINE void NxMat33::setRow(int row, const NxVec3 & v)
	{
#ifndef TRANSPOSED_MAT33
	data.m[row][0] = v.x;
	data.m[row][1] = v.y;
	data.m[row][2] = v.z;
#else
	data.m[0][row] = v.x;
	data.m[1][row] = v.y;
	data.m[2][row] = v.z;
#endif
	}


NX_INLINE void NxMat33::setColumn(int col, const NxVec3 & v)
	{
#ifndef TRANSPOSED_MAT33
	data.m[0][col] = v.x;
	data.m[1][col] = v.y;
	data.m[2][col] = v.z;
#else
	data.m[col][0] = v.x;
	data.m[col][1] = v.y;
	data.m[col][2] = v.z;
#endif
	}


NX_INLINE void NxMat33::getRow(int row, NxVec3 & v) const
	{
#ifndef TRANSPOSED_MAT33
	v.x = data.m[row][0];
	v.y = data.m[row][1];
	v.z = data.m[row][2];
#else
	v.x = data.m[0][row];
	v.y = data.m[1][row];
	v.z = data.m[2][row];
#endif
	}


NX_INLINE void NxMat33::getColumn(int col, NxVec3 & v) const
	{
#ifndef TRANSPOSED_MAT33
	v.x = data.m[0][col];
	v.y = data.m[1][col];
	v.z = data.m[2][col];
#else
	v.x = data.m[col][0];
	v.y = data.m[col][1];
	v.z = data.m[col][2];
#endif
	}


NX_INLINE NxVec3 NxMat33::getRow(int row) const
{
#ifndef TRANSPOSED_MAT33
	return NxVec3(data.m[row][0],data.m[row][1],data.m[row][2]);
#else
	return NxVec3(data.m[0][row],data.m[1][row],data.m[2][row]);
#endif
}

NX_INLINE NxVec3 NxMat33::getColumn(int col) const
{
#ifndef TRANSPOSED_MAT33
	return NxVec3(data.m[0][col],data.m[1][col],data.m[2][col]);
#else
	return NxVec3(data.m[col][0],data.m[col][1],data.m[col][2]);
#endif
}

NX_INLINE NxReal & NxMat33::operator()(int row, int col)
	{
#ifndef TRANSPOSED_MAT33
	return data.m[row][col];
#else
	return data.m[col][row];
#endif
	}


NX_INLINE const NxReal & NxMat33::operator() (int row, int col) const
	{
#ifndef TRANSPOSED_MAT33
	return data.m[row][col];
#else
	return data.m[col][row];
#endif
	}

//const methods


NX_INLINE bool NxMat33::isIdentity() const
	{
	if(data.s._11 != 1.0f)		return false;
	if(data.s._12 != 0.0f)		return false;
	if(data.s._13 != 0.0f)		return false;

	if(data.s._21 != 0.0f)		return false;
	if(data.s._22 != 1.0f)		return false;
	if(data.s._23 != 0.0f)		return false;

	if(data.s._31 != 0.0f)		return false;
	if(data.s._32 != 0.0f)		return false;
	if(data.s._33 != 1.0f)		return false;

	return true;
	}


NX_INLINE bool NxMat33::isZero() const
	{
	if(data.s._11 != 0.0f)		return false;
	if(data.s._12 != 0.0f)		return false;
	if(data.s._13 != 0.0f)		return false;

	if(data.s._21 != 0.0f)		return false;
	if(data.s._22 != 0.0f)		return false;
	if(data.s._23 != 0.0f)		return false;

	if(data.s._31 != 0.0f)		return false;
	if(data.s._32 != 0.0f)		return false;
	if(data.s._33 != 0.0f)		return false;

	return true;
	}


NX_INLINE bool NxMat33::isFinite() const
	{
	return NxMath::isFinite(data.s._11)
	&& NxMath::isFinite(data.s._12)
	&& NxMath::isFinite(data.s._13)

	&& NxMath::isFinite(data.s._21)
	&& NxMath::isFinite(data.s._22)
	&& NxMath::isFinite(data.s._23)

	&& NxMath::isFinite(data.s._31)
	&& NxMath::isFinite(data.s._32)
	&& NxMath::isFinite(data.s._33);
	}



NX_INLINE void NxMat33::zero()
	{
	data.s._11 = NxReal(0.0);
	data.s._12 = NxReal(0.0);
	data.s._13 = NxReal(0.0);

	data.s._21 = NxReal(0.0);
	data.s._22 = NxReal(0.0);
	data.s._23 = NxReal(0.0);

	data.s._31 = NxReal(0.0);
	data.s._32 = NxReal(0.0);
	data.s._33 = NxReal(0.0);
	}


NX_INLINE void NxMat33::id()
	{
	data.s._11 = NxReal(1.0);
	data.s._12 = NxReal(0.0);
	data.s._13 = NxReal(0.0);

	data.s._21 = NxReal(0.0);
	data.s._22 = NxReal(1.0);
	data.s._23 = NxReal(0.0);

	data.s._31 = NxReal(0.0);
	data.s._32 = NxReal(0.0);
	data.s._33 = NxReal(1.0);
	}


NX_INLINE void NxMat33::setNegative()
	{
	data.s._11 = -data.s._11;
	data.s._12 = -data.s._12;
	data.s._13 = -data.s._13;

	data.s._21 = -data.s._21;
	data.s._22 = -data.s._22;
	data.s._23 = -data.s._23;

	data.s._31 = -data.s._31;
	data.s._32 = -data.s._32;
	data.s._33 = -data.s._33;
	}


NX_INLINE void NxMat33::diagonal(const NxVec3 &v)
	{
	data.s._11 = v.x;
	data.s._12 = NxReal(0.0);
	data.s._13 = NxReal(0.0);

	data.s._21 = NxReal(0.0);
	data.s._22 = v.y;
	data.s._23 = NxReal(0.0);

	data.s._31 = NxReal(0.0);
	data.s._32 = NxReal(0.0);
	data.s._33 = v.z;
	}


NX_INLINE void NxMat33::star(const NxVec3 &v)
	{
	data.s._11 = NxReal(0.0);	data.s._12 =-v.z;	data.s._13 = v.y;
	data.s._21 = v.z;	data.s._22 = NxReal(0.0);	data.s._23 =-v.x;
	data.s._31 =-v.y;	data.s._32 = v.x;	data.s._33 = NxReal(0.0);
	}


NX_INLINE void NxMat33::fromQuat(const NxQuat & q)
	{
	const NxReal w = q.w;
	const NxReal x = q.x;
	const NxReal y = q.y;
	const NxReal z = q.z;

	data.s._11 = NxReal(1.0) - y*y*NxReal(2.0) - z*z*NxReal(2.0);
	data.s._12 = x*y*NxReal(2.0) - w*z*NxReal(2.0);	
	data.s._13 = x*z*NxReal(2.0) + w*y*NxReal(2.0);	

	data.s._21 = x*y*NxReal(2.0) + w*z*NxReal(2.0);	
	data.s._22 = NxReal(1.0) - x*x*NxReal(2.0) - z*z*NxReal(2.0);	
	data.s._23 = y*z*NxReal(2.0) - w*x*NxReal(2.0);	
	
	data.s._31 = x*z*NxReal(2.0) - w*y*NxReal(2.0);	
	data.s._32 = y*z*NxReal(2.0) + w*x*NxReal(2.0);	
	data.s._33 = NxReal(1.0) - x*x*NxReal(2.0) - y*y*NxReal(2.0);	
	}


NX_INLINE void NxMat33::toQuat(NxQuat & q) const					// set the NxQuat from a rotation matrix
	{
    NxReal tr, s;
    tr = data.s._11 + data.s._22 + data.s._33;
    if(tr >= 0)
		{
		s = (NxReal)NxMath::sqrt(tr +1);
		q.w = NxReal(0.5) * s;
		s = NxReal(0.5) / s;
		q.x = ((*this)(2,1) - (*this)(1,2)) * s;
		q.y = ((*this)(0,2) - (*this)(2,0)) * s;
		q.z = ((*this)(1,0) - (*this)(0,1)) * s;
		}
    else
		{
		int i = 0; 
		if (data.s._22 > data.s._11)
			i = 1; 
		if(data.s._33 > (*this)(i,i))
			i=2; 
		switch (i)
			{
			case 0:
				s = (NxReal)NxMath::sqrt((data.s._11 - (data.s._22 + data.s._33)) + 1);
				q.x = NxReal(0.5) * s;
				s = NxReal(0.5) / s;
				q.y = ((*this)(0,1) + (*this)(1,0)) * s; 
				q.z = ((*this)(2,0) + (*this)(0,2)) * s;
				q.w = ((*this)(2,1) - (*this)(1,2)) * s;
				break;
			case 1:
				s = (NxReal)NxMath::sqrt((data.s._22 - (data.s._33 + data.s._11)) + 1);
				q.y = NxReal(0.5) * s;
				s = NxReal(0.5) / s;
				q.z = ((*this)(1,2) + (*this)(2,1)) * s;
				q.x = ((*this)(0,1) + (*this)(1,0)) * s;
				q.w = ((*this)(0,2) - (*this)(2,0)) * s;
				break;
			case 2:
				s = (NxReal)NxMath::sqrt((data.s._33 - (data.s._11 + data.s._22)) + 1);
				q.z = NxReal(0.5) * s;
				s = NxReal(0.5) / s;
				q.x = ((*this)(2,0) + (*this)(0,2)) * s;
				q.y = ((*this)(1,2) + (*this)(2,1)) * s;
				q.w = ((*this)(1,0) - (*this)(0,1)) * s;
			}
		}
	}
/*

NX_INLINE void NxMat33::orthonormalize()	//Gram-Schmidt orthogonalization to correct numerical drift, plus column normalization
	{
	//TODO: This is buggy!
	NxVec3 w,t1,t2,t3;
	NxReal norm_sq;

    const NxReal m=3;			//m := linalg[rowdim](A);
    const NxReal n=3;			//n := linalg[coldim](A);
	int i, j, k = 0;				//k := 0;


    Mat33d v = *this;				//v := linalg[col](A, 1 .. n); -- 3 column vectors indexable
    NxVec3 norm_u_sq;
																//# orthogonalize v[i]
    for (i=0; i<n; i++)//for i to n do
		{
        v.getColumn(i,w);		//i-th column
        for (j=0; j<k; j++)									//# pull w along projection of v[i] with u[j]
			{
			this->getColumn(j,t1);
			this->getColumn(j,t2);
			v.getColumn(i,t3);
			NxVec3 temp = (t2 * (NxReal(1.0)/norm_u_sq[j]));
			NxVec3 temp2 = temp  * t3.dot( t1 );
			w -= temp;	
			}
																//        # compute norm of orthogonalized v[i]
      norm_sq = w.Dot(w);

		if (norm_sq != NxReal(0.0)) 
			{													//           # linearly independent new orthogonal vector 
																//       # add to list of u and norm_u_sq
			this->SetColumn(i,w);									//u = [op(u), evalm(w)];
            norm_u_sq[i] = norm_sq;						//norm_u_sq = [op(norm_u_sq), norm_sq];
            k ++;
			}
		}
	

	NxVec3 temp;													//may want to do this in-place -- dunno if optimizer does this for me
	for (i=0; i<3; i++)
		{
		getColumn(i,temp);
		temp.normalize();
		setColumn(i,temp);
		}
	}
	*/


NX_INLINE void NxMat33::setTransposed(const NxMat33& other)
	{
	//gotta special case in-place case
	if (this != &other)
		{
		data.s._11 = other.data.s._11;
		data.s._12 = other.data.s._21;
		data.s._13 = other.data.s._31;

		data.s._21 = other.data.s._12;
		data.s._22 = other.data.s._22;
		data.s._23 = other.data.s._32;

		data.s._31 = other.data.s._13;
		data.s._32 = other.data.s._23;
		data.s._33 = other.data.s._33;
		}
	else
		{
		NxReal tx, ty, tz;
		tx = data.s._21;	data.s._21 = other.data.s._12;	data.s._12 = tx;
		ty = data.s._31;	data.s._31 = other.data.s._13;	data.s._13 = ty;
		tz = data.s._32;	data.s._32 = other.data.s._23;	data.s._23 = tz;
		}
	}


NX_INLINE void NxMat33::setTransposed()
	{
		NX_Swap(data.s._12, data.s._21);
		NX_Swap(data.s._23, data.s._32);
		NX_Swap(data.s._13, data.s._31);
	}


NX_INLINE void NxMat33::multiplyDiagonal(const NxVec3 &d)
	{
	data.s._11 *= d.x;
	data.s._12 *= d.y;
	data.s._13 *= d.z;

	data.s._21 *= d.x;
	data.s._22 *= d.y;
	data.s._23 *= d.z;

	data.s._31 *= d.x;
	data.s._32 *= d.y;
	data.s._33 *= d.z;
	}


NX_INLINE void NxMat33::multiplyDiagonalTranspose(const NxVec3 &d)
	{
		NxReal temp;
		data.s._11 = data.s._11 * d.x;
		data.s._22 = data.s._22 * d.y;
		data.s._33 = data.s._33 * d.z;

		temp = data.s._21 * d.y;
		data.s._21 = data.s._12 * d.x;
		data.s._12 = temp;

		temp = data.s._31 * d.z;
		data.s._31 = data.s._13 * d.x;
		data.s._13 = temp;
		
		temp = data.s._32 * d.z;
		data.s._32 = data.s._23 * d.y;
		data.s._23 = temp;
	}


NX_INLINE void NxMat33::multiplyDiagonal(const NxVec3 &d, NxMat33& dst) const
	{
	dst.data.s._11 = data.s._11 * d.x;
	dst.data.s._12 = data.s._12 * d.y;
	dst.data.s._13 = data.s._13 * d.z;

	dst.data.s._21 = data.s._21 * d.x;
	dst.data.s._22 = data.s._22 * d.y;
	dst.data.s._23 = data.s._23 * d.z;

	dst.data.s._31 = data.s._31 * d.x;
	dst.data.s._32 = data.s._32 * d.y;
	dst.data.s._33 = data.s._33 * d.z;
	}


NX_INLINE void NxMat33::multiplyDiagonalTranspose(const NxVec3 &d, NxMat33& dst) const
	{
	dst.data.s._11 = data.s._11 * d.x;
	dst.data.s._12 = data.s._21 * d.y;
	dst.data.s._13 = data.s._31 * d.z;

	dst.data.s._21 = data.s._12 * d.x;
	dst.data.s._22 = data.s._22 * d.y;
	dst.data.s._23 = data.s._32 * d.z;

	dst.data.s._31 = data.s._13 * d.x;
	dst.data.s._32 = data.s._23 * d.y;
	dst.data.s._33 = data.s._33 * d.z;
	}


NX_INLINE void NxMat33::multiply(const NxVec3 &src, NxVec3 &dst) const
	{
	NxReal x,y,z;	//so it works if src == dst
	x = data.s._11 * src.x + data.s._12 * src.y + data.s._13 * src.z;
	y = data.s._21 * src.x + data.s._22 * src.y + data.s._23 * src.z;
	z = data.s._31 * src.x + data.s._32 * src.y + data.s._33 * src.z;

	dst.x = x;
	dst.y = y;
	dst.z = z;	
	}


NX_INLINE void NxMat33::multiplyByTranspose(const NxVec3 &src, NxVec3 &dst) const
	{
	NxReal x,y,z;	//so it works if src == dst
	x = data.s._11 * src.x + data.s._21 * src.y + data.s._31 * src.z;
	y = data.s._12 * src.x + data.s._22 * src.y + data.s._32 * src.z;
	z = data.s._13 * src.x + data.s._23 * src.y + data.s._33 * src.z;

	dst.x = x;
	dst.y = y;
	dst.z = z;	
	}


NX_INLINE void NxMat33::add(const NxMat33 & a, const NxMat33 & b)
	{
	data.s._11 = a.data.s._11 + b.data.s._11;
	data.s._12 = a.data.s._12 + b.data.s._12;
	data.s._13 = a.data.s._13 + b.data.s._13;

	data.s._21 = a.data.s._21 + b.data.s._21;
	data.s._22 = a.data.s._22 + b.data.s._22;
	data.s._23 = a.data.s._23 + b.data.s._23;

	data.s._31 = a.data.s._31 + b.data.s._31;
	data.s._32 = a.data.s._32 + b.data.s._32;
	data.s._33 = a.data.s._33 + b.data.s._33;
	}


NX_INLINE void NxMat33::subtract(const NxMat33 &a, const NxMat33 &b)
	{
	data.s._11 = a.data.s._11 - b.data.s._11;
	data.s._12 = a.data.s._12 - b.data.s._12;
	data.s._13 = a.data.s._13 - b.data.s._13;

	data.s._21 = a.data.s._21 - b.data.s._21;
	data.s._22 = a.data.s._22 - b.data.s._22;
	data.s._23 = a.data.s._23 - b.data.s._23;

	data.s._31 = a.data.s._31 - b.data.s._31;
	data.s._32 = a.data.s._32 - b.data.s._32;
	data.s._33 = a.data.s._33 - b.data.s._33;
	}


NX_INLINE void NxMat33::multiply(NxReal d,  const NxMat33 & a)
	{
	data.s._11 = a.data.s._11 * d;
	data.s._12 = a.data.s._12 * d;
	data.s._13 = a.data.s._13 * d;

	data.s._21 = a.data.s._21 * d;
	data.s._22 = a.data.s._22 * d;
	data.s._23 = a.data.s._23 * d;

	data.s._31 = a.data.s._31 * d;
	data.s._32 = a.data.s._32 * d;
	data.s._33 = a.data.s._33 * d;
	}


NX_INLINE void NxMat33::multiply(const NxMat33& left, const NxMat33& right)
	{
	NxReal a,b,c, d,e,f, g,h,i;
	//note: temps needed so that x.multiply(x,y) works OK.
	a =left.data.s._11 * right.data.s._11 +left.data.s._12 * right.data.s._21 +left.data.s._13 * right.data.s._31;
	b =left.data.s._11 * right.data.s._12 +left.data.s._12 * right.data.s._22 +left.data.s._13 * right.data.s._32;
	c =left.data.s._11 * right.data.s._13 +left.data.s._12 * right.data.s._23 +left.data.s._13 * right.data.s._33;

	d =left.data.s._21 * right.data.s._11 +left.data.s._22 * right.data.s._21 +left.data.s._23 * right.data.s._31;
	e =left.data.s._21 * right.data.s._12 +left.data.s._22 * right.data.s._22 +left.data.s._23 * right.data.s._32;
	f =left.data.s._21 * right.data.s._13 +left.data.s._22 * right.data.s._23 +left.data.s._23 * right.data.s._33;

	g =left.data.s._31 * right.data.s._11 +left.data.s._32 * right.data.s._21 +left.data.s._33 * right.data.s._31;
	h =left.data.s._31 * right.data.s._12 +left.data.s._32 * right.data.s._22 +left.data.s._33 * right.data.s._32;
	i =left.data.s._31 * right.data.s._13 +left.data.s._32 * right.data.s._23 +left.data.s._33 * right.data.s._33;


	data.s._11 = a;
	data.s._12 = b;
	data.s._13 = c;

	data.s._21 = d;
	data.s._22 = e;
	data.s._23 = f;

	data.s._31 = g;
	data.s._32 = h;
	data.s._33 = i;
	}


NX_INLINE void NxMat33::multiplyTransposeLeft(const NxMat33& left, const NxMat33& right)
	{
	NxReal a,b,c, d,e,f, g,h,i;
	//note: temps needed so that x.multiply(x,y) works OK.
	a =left.data.s._11 * right.data.s._11 +left.data.s._21 * right.data.s._21 +left.data.s._31 * right.data.s._31;
	b =left.data.s._11 * right.data.s._12 +left.data.s._21 * right.data.s._22 +left.data.s._31 * right.data.s._32;
	c =left.data.s._11 * right.data.s._13 +left.data.s._21 * right.data.s._23 +left.data.s._31 * right.data.s._33;

	d =left.data.s._12 * right.data.s._11 +left.data.s._22 * right.data.s._21 +left.data.s._32 * right.data.s._31;
	e =left.data.s._12 * right.data.s._12 +left.data.s._22 * right.data.s._22 +left.data.s._32 * right.data.s._32;
	f =left.data.s._12 * right.data.s._13 +left.data.s._22 * right.data.s._23 +left.data.s._32 * right.data.s._33;

	g =left.data.s._13 * right.data.s._11 +left.data.s._23 * right.data.s._21 +left.data.s._33 * right.data.s._31;
	h =left.data.s._13 * right.data.s._12 +left.data.s._23 * right.data.s._22 +left.data.s._33 * right.data.s._32;
	i =left.data.s._13 * right.data.s._13 +left.data.s._23 * right.data.s._23 +left.data.s._33 * right.data.s._33;

	data.s._11 = a;
	data.s._12 = b;
	data.s._13 = c;

	data.s._21 = d;
	data.s._22 = e;
	data.s._23 = f;

	data.s._31 = g;
	data.s._32 = h;
	data.s._33 = i;
	}


NX_INLINE void NxMat33::multiplyTransposeRight(const NxMat33& left, const NxMat33& right)
	{
	NxReal a,b,c, d,e,f, g,h,i;
	//note: temps needed so that x.multiply(x,y) works OK.
	a =left.data.s._11 * right.data.s._11 +left.data.s._12 * right.data.s._12 +left.data.s._13 * right.data.s._13;
	b =left.data.s._11 * right.data.s._21 +left.data.s._12 * right.data.s._22 +left.data.s._13 * right.data.s._23;
	c =left.data.s._11 * right.data.s._31 +left.data.s._12 * right.data.s._32 +left.data.s._13 * right.data.s._33;

	d =left.data.s._21 * right.data.s._11 +left.data.s._22 * right.data.s._12 +left.data.s._23 * right.data.s._13;
	e =left.data.s._21 * right.data.s._21 +left.data.s._22 * right.data.s._22 +left.data.s._23 * right.data.s._23;
	f =left.data.s._21 * right.data.s._31 +left.data.s._22 * right.data.s._32 +left.data.s._23 * right.data.s._33;

	g =left.data.s._31 * right.data.s._11 +left.data.s._32 * right.data.s._12 +left.data.s._33 * right.data.s._13;
	h =left.data.s._31 * right.data.s._21 +left.data.s._32 * right.data.s._22 +left.data.s._33 * right.data.s._23;
	i =left.data.s._31 * right.data.s._31 +left.data.s._32 * right.data.s._32 +left.data.s._33 * right.data.s._33;

	data.s._11 = a;
	data.s._12 = b;
	data.s._13 = c;

	data.s._21 = d;
	data.s._22 = e;
	data.s._23 = f;

	data.s._31 = g;
	data.s._32 = h;
	data.s._33 = i;
	}


NX_INLINE void NxMat33::multiplyTransposeRight(const NxVec3 &left, const NxVec3 &right)
	{
	data.s._11 = left.x * right.x;
	data.s._12 = left.x * right.y;
	data.s._13 = left.x * right.z;

	data.s._21 = left.y * right.x;
	data.s._22 = left.y * right.y;
	data.s._23 = left.y * right.z;

	data.s._31 = left.z * right.x;
	data.s._32 = left.z * right.y;
	data.s._33 = left.z * right.z;
	}

NX_INLINE void NxMat33::rotX(NxReal angle)
	{
	NxReal Cos = cosf(angle);
	NxReal Sin = sinf(angle);
	id();
	data.m[1][1] = data.m[2][2] = Cos;
	data.m[1][2] = -Sin;
	data.m[2][1] = Sin;
	}

NX_INLINE void NxMat33::rotY(NxReal angle)
	{
	NxReal Cos = cosf(angle);
	NxReal Sin = sinf(angle);
	id();
	data.m[0][0] = data.m[2][2] = Cos;
	data.m[0][2] = Sin;
	data.m[2][0] = -Sin;
	}

NX_INLINE void NxMat33::rotZ(NxReal angle)
	{
	NxReal Cos = cosf(angle);
	NxReal Sin = sinf(angle);
	id();
	data.m[0][0] = data.m[1][1] = Cos;
	data.m[0][1] = -Sin;
	data.m[1][0] = Sin;
	}

NX_INLINE NxVec3  NxMat33::operator%(const NxVec3 & src) const
	{
	NxVec3 dest;
	this->multiplyByTranspose(src, dest);
	return dest;
	}


NX_INLINE NxVec3  NxMat33::operator*(const NxVec3 & src) const
	{
	NxVec3 dest;
	this->multiply(src, dest);
	return dest;
	}


NX_INLINE const NxMat33 &NxMat33::operator +=(const NxMat33 &d)
	{
	data.s._11 += d.data.s._11;
	data.s._12 += d.data.s._12;
	data.s._13 += d.data.s._13;

	data.s._21 += d.data.s._21;
	data.s._22 += d.data.s._22;
	data.s._23 += d.data.s._23;

	data.s._31 += d.data.s._31;
	data.s._32 += d.data.s._32;
	data.s._33 += d.data.s._33;
	return *this;
	}


NX_INLINE const NxMat33 &NxMat33::operator -=(const NxMat33 &d)
	{
	data.s._11 -= d.data.s._11;
	data.s._12 -= d.data.s._12;
	data.s._13 -= d.data.s._13;

	data.s._21 -= d.data.s._21;
	data.s._22 -= d.data.s._22;
	data.s._23 -= d.data.s._23;

	data.s._31 -= d.data.s._31;
	data.s._32 -= d.data.s._32;
	data.s._33 -= d.data.s._33;
	return *this;
	}


NX_INLINE const NxMat33 &NxMat33::operator *=(NxReal f)
	{
	data.s._11 *= f;
	data.s._12 *= f;
	data.s._13 *= f;

	data.s._21 *= f;
	data.s._22 *= f;
	data.s._23 *= f;

	data.s._31 *= f;
	data.s._32 *= f;
	data.s._33 *= f;
	return *this;
	}


NX_INLINE const NxMat33 &NxMat33::operator /=(NxReal x)
	{
	NxReal f = NxReal(1.0) / x;
	data.s._11 *= f;
	data.s._12 *= f;
	data.s._13 *= f;

	data.s._21 *= f;
	data.s._22 *= f;
	data.s._23 *= f;

	data.s._31 *= f;
	data.s._32 *= f;
	data.s._33 *= f;
	return *this;
	}


NX_INLINE NxReal NxMat33::determinant() const
	{
	return  data.s._11*data.s._22*data.s._33 + data.s._12*data.s._23*data.s._31 + data.s._13*data.s._21*data.s._32 
		  - data.s._13*data.s._22*data.s._31 - data.s._12*data.s._21*data.s._33 - data.s._11*data.s._23*data.s._32;
	}


bool NxMat33::getInverse(NxMat33& dest) const
	{
	NxReal b00,b01,b02,b10,b11,b12,b20,b21,b22;

	b00 = data.s._22*data.s._33-data.s._23*data.s._32;	b01 = data.s._13*data.s._32-data.s._12*data.s._33;	b02 = data.s._12*data.s._23-data.s._13*data.s._22;
	b10 = data.s._23*data.s._31-data.s._21*data.s._33;	b11 = data.s._11*data.s._33-data.s._13*data.s._31;	b12 = data.s._13*data.s._21-data.s._11*data.s._23;
	b20 = data.s._21*data.s._32-data.s._22*data.s._31;	b21 = data.s._12*data.s._31-data.s._11*data.s._32;	b22 = data.s._11*data.s._22-data.s._12*data.s._21;
	


	/*
	compute determinant: 
	NxReal d =   a00*a11*a22 + a01*a12*a20 + a02*a10*a21	- a02*a11*a20 - a01*a10*a22 - a00*a12*a21;
				0				1			2			3				4			5

	this is a subset of the multiplies done above:

	NxReal d = b00*a00				+		b01*a10						 + b02 * a20;
	NxReal d = (a11*a22-a12*a21)*a00 +		(a02*a21-a01*a22)*a10		 + (a01*a12-a02*a11) * a20;

	NxReal d = a11*a22*a00-a12*a21*a00 +		a02*a21*a10-a01*a22*a10		 + a01*a12*a20-a02*a11*a20;
			0			5					2			4					1			3
	*/

	NxReal d = b00*data.s._11		+		b01*data.s._21				 + b02 * data.s._31;
	
	if (d == NxReal(0.0))		//singular?
		{
		dest.id();
		return false;
		}
	
	d = NxReal(1.0)/d;

	//only do assignment at the end, in case dest == this:


	dest.data.s._11 = b00*d; dest.data.s._12 = b01*d; dest.data.s._13 = b02*d;
	dest.data.s._21 = b10*d; dest.data.s._22 = b11*d; dest.data.s._23 = b12*d;
	dest.data.s._31 = b20*d; dest.data.s._32 = b21*d; dest.data.s._33 = b22*d;

	return true;
	}


NX_INLINE NxMat33&	NxMat33::operator*= (const NxMat33& mat)
	{
	this->multiply(*this, mat);
	return *this;
	}


NX_INLINE NxMat33	NxMat33::operator-  (const NxMat33& mat)	const
	{
	NxMat33 temp;
	temp.subtract(*this, mat);
	return temp;
	}


NX_INLINE NxMat33	NxMat33::operator+  (const NxMat33& mat)	const
	{
	NxMat33 temp;
	temp.add(*this, mat);
	return temp;
	}


NX_INLINE NxMat33	NxMat33::operator*  (const NxMat33& mat)	const
	{
	NxMat33 temp;
	temp.multiply(*this, mat);
	return temp;
	}


NX_INLINE NxMat33	NxMat33::operator*  (float s)			const
	{
	NxMat33 temp;
	temp.multiply(s, *this);
	return temp;
	}

NX_INLINE NxQuat::NxQuat(const class NxMat33 &m)
{
	m.toQuat(*this);
}

 /** @} */
#endif//__PPCGEKKO__
#endif
//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND


#ifndef NX_FOUNDATION_NxMat34T
#define NX_FOUNDATION_NxMat34T
/*----------------------------------------------------------------------------*\
|
|						Public Interface to Ageia PhysX Technology
|
|							     www.ageia.com
|
\*----------------------------------------------------------------------------*/
/** \addtogroup foundation
  @{
*/


/**
\brief Combination of a 3x3 rotation matrix and a translation vector.

homogenous transform class composed of a matrix and a vector.
*/

class NxMat34
	{
	public:
	/**
	\brief [ M t ]
	*/
	NxMat33 M;
	NxVec3 t;

	/**
	\brief by default M is inited and t isn't.  Use this ctor to either init or not init in full.
	*/
	NX_INLINE explicit NxMat34(bool init = true);

	NX_INLINE NxMat34(const NxMat33& rot, const NxVec3& trans) : M(rot), t(trans)
		{
		}

	NX_INLINE void zero();

	NX_INLINE void id();

	/**
	\brief returns true for identity matrix
	*/
	NX_INLINE bool isIdentity() const;

	/**
	\brief returns true if all elems are finite (not NAN or INF, etc.)
	*/
	NX_INLINE bool isFinite() const;

	/**
	\brief assigns inverse to dest. 
	
	Returns false if singular (i.e. if no inverse exists), setting dest to identity.  dest may equal this.
	*/
	NX_INLINE bool getInverse(NxMat34& dest) const;

	/**
	\brief same as #getInverse(), but assumes that M is orthonormal
	*/
	NX_INLINE bool getInverseRT(NxMat34& dest) const;

	/**
	\brief dst = this * src
	*/
	NX_INLINE void multiply(const NxVec3 &src, NxVec3 &dst) const;

	/**
	\brief operator wrapper for multiply
	*/
	NX_INLINE NxVec3 operator*  (const NxVec3 & src) const { NxVec3 dest; multiply(src, dest); return dest; }
	/**
	\brief dst = inverse(this) * src	-- assumes M is rotation matrix!!!
	*/
	NX_INLINE void multiplyByInverseRT(const NxVec3 &src, NxVec3 &dst) const;

	/**
	\brief operator wrapper for multiplyByInverseRT
	*/
	NX_INLINE NxVec3 operator%  (const NxVec3 & src) const { NxVec3 dest; multiplyByInverseRT(src, dest); return dest; }

	/**
	\brief this = left * right	
	*/
	NX_INLINE void multiply(const NxMat34& left, const NxMat34& right);

	/**
	\brief this = inverse(left) * right	-- assumes M is rotation matrix!!!
	*/
	NX_INLINE void multiplyInverseRTLeft(const NxMat34& left, const NxMat34& right);

	/**
	\brief this = left * inverse(right)	-- assumes M is rotation matrix!!!
	*/
	NX_INLINE void multiplyInverseRTRight(const NxMat34& left, const NxMat34& right);

	/**
	\brief operator wrapper for multiply
	*/
	NX_INLINE NxMat34 operator*  (const NxMat34 & right) const { NxMat34 dest(false); dest.multiply(*this, right); return dest; }

	/**
	\brief convert from a matrix format appropriate for rendering
	*/
	NX_INLINE void setColumnMajor44(const NxF32 *);
	/**
	\brief convert from a matrix format appropriate for rendering
	*/
	NX_INLINE void setColumnMajor44(const NxF32 d[4][4]);
	/**
	\brief convert to a matrix format appropriate for rendering
	*/
	NX_INLINE void getColumnMajor44(NxF32 *) const;
	/**
	\brief convert to a matrix format appropriate for rendering
	*/
	NX_INLINE void getColumnMajor44(NxF32 d[4][4]) const;
	/**
	\brief set the matrix given a row major matrix.
	*/
	NX_INLINE void setRowMajor44(const NxF32 *);
	/**
	\brief set the matrix given a row major matrix.
	*/
	NX_INLINE void setRowMajor44(const NxF32 d[4][4]);
	/**
	\brief retrieve the matrix in a row major format.
	*/
	NX_INLINE void getRowMajor44(NxF32 *) const;
	/**
	\brief retrieve the matrix in a row major format.
	*/
	NX_INLINE void getRowMajor44(NxF32 d[4][4]) const;
	};


NX_INLINE NxMat34::NxMat34(bool init)
	{
	if (init)
	{
		t.zero();
		M.id();
	}
	}


NX_INLINE void NxMat34::zero()
	{
	M.zero();
	t.zero();
	}


NX_INLINE void NxMat34::id()
	{
	M.id();
	t.zero();
	}


NX_INLINE bool NxMat34::isIdentity() const
	{
	if(!M.isIdentity())	return false;
	if(!t.isZero())		return false;
	return true;
	}


NX_INLINE bool NxMat34::isFinite() const
	{
	if(!M.isFinite())	return false;
	if(!t.isFinite())	return false;
	return true;
	}


NX_INLINE bool NxMat34::getInverse(NxMat34& dest) const
	{
	// inv(this) = [ inv(M) , inv(M) * -t ]
	bool status = M.getInverse(dest.M);
	dest.M.multiply(t * -1.0f, dest.t); 
	return status;
	}


NX_INLINE bool NxMat34::getInverseRT(NxMat34& dest) const
	{
	// inv(this) = [ M' , M' * -t ]
	dest.M.setTransposed(M);
	dest.M.multiply(t * -1.0f, dest.t); 
	return true;
	}



NX_INLINE void NxMat34::multiply(const NxVec3 &src, NxVec3 &dst) const
	{
	dst = M * src + t;
	}


NX_INLINE void NxMat34::multiplyByInverseRT(const NxVec3 &src, NxVec3 &dst) const
	{
	//dst = M' * src - M' * t = M' * (src - t)
	M.multiplyByTranspose(src - t, dst);
	}


NX_INLINE void NxMat34::multiply(const NxMat34& left, const NxMat34& right)
	{
	//[aR at] * [bR bt] = [aR * bR		aR * bt + at]  NOTE: order of operations important so it works when this ?= left ?= right.
	t = left.M * right.t + left.t;
	M.multiply(left.M, right.M);
	}


NX_INLINE void NxMat34::multiplyInverseRTLeft(const NxMat34& left, const NxMat34& right)
	{
	//[aR' -aR'*at] * [bR bt] = [aR' * bR		aR' * bt  - aR'*at]	//aR' ( bt  - at )	NOTE: order of operations important so it works when this ?= left ?= right.
	t = left.M % (right.t - left.t);
	M.multiplyTransposeLeft(left.M, right.M);
	}


NX_INLINE void NxMat34::multiplyInverseRTRight(const NxMat34& left, const NxMat34& right)
	{
	//[aR at] * [bR' -bR'*bt] = [aR * bR'		-aR * bR' * bt + at]	NOTE: order of operations important so it works when this ?= left ?= right.
	M.multiplyTransposeRight(left.M, right.M);
	t = left.t - M * right.t;
	}

NX_INLINE void NxMat34::setColumnMajor44(const NxF32 * d) 
	{
	M.setColumnMajorStride4(d);
    t.x = d[12];
	t.y = d[13];
	t.z = d[14];
	}

NX_INLINE void NxMat34::setColumnMajor44(const NxF32 d[4][4]) 
	{
	M.setColumnMajorStride4(d);
    t.x = d[3][0];
	t.y = d[3][1];
	t.z = d[3][2];
	}

NX_INLINE void NxMat34::getColumnMajor44(NxF32 * d) const
	{
	M.getColumnMajorStride4(d);
    d[12] = t.x;
	d[13] = t.y;
	d[14] = t.z;
	d[3] = d[7] = d[11] = 0.0f;
	d[15] = 1.0f;
	}

NX_INLINE void NxMat34::getColumnMajor44(NxF32 d[4][4]) const
	{
	M.getColumnMajorStride4(d);
    d[3][0] = t.x;
	d[3][1] = t.y;
	d[3][2] = t.z;
	d[0][3] = d[1][3] = d[2][3] = 0.0f;
	d[3][3] = 1.0f;
	}

NX_INLINE void NxMat34::setRowMajor44(const NxF32 * d) 
	{
	M.setRowMajorStride4(d);
    t.x = d[3];
	t.y = d[7];
	t.z = d[11];
	}

NX_INLINE void NxMat34::setRowMajor44(const NxF32 d[4][4])
	{
	M.setRowMajorStride4(d);
    t.x = d[0][3];
	t.y = d[1][3];
	t.z = d[2][3];
	}

NX_INLINE void NxMat34::getRowMajor44(NxF32 * d) const
	{
	M.getRowMajorStride4(d);
    d[3] = t.x;
	d[7] = t.y;
	d[11] = t.z;
	d[12] = d[13] = d[14] = 0.0f;
	d[15] = 1.0f;
	}

NX_INLINE void NxMat34::getRowMajor44(NxF32 d[4][4]) const
	{
	M.getRowMajorStride4(d);
    d[0][3] = t.x;
	d[1][3] = t.y;
	d[2][3] = t.z;
	d[3][0] = d[3][1] = d[3][2] = 0.0f;
	d[3][3] = 1.0f;
	}

 /** @} */
#endif
//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND

#ifndef NX_FOUNDATION_NXPLANE
#define NX_FOUNDATION_NXPLANE
/*----------------------------------------------------------------------------*\
|
|						Public Interface to Ageia PhysX Technology
|
|							     www.ageia.com
|
\*----------------------------------------------------------------------------*/
/** \addtogroup foundation
  @{
*/


/**
\brief Representation of a plane.

 Plane equation used: a*x + b*y + c*z + d = 0
*/
class NxPlane
	{
	public:
	/**
	\brief Constructor
	*/
	NX_INLINE NxPlane()
		{
		}

	/**
	\brief Constructor from a normal and a distance
	*/
	NX_INLINE NxPlane(NxF32 nx, NxF32 ny, NxF32 nz, NxF32 _d)
		{
		set(nx, ny, nz, _d);
		}

	/**
	\brief Constructor from a point on the plane and a normal
	*/
	NX_INLINE NxPlane(const NxVec3& p, const NxVec3& n)
		{
		set(p, n);
		}

	/**
	\brief Constructor from three points
	*/
	NX_INLINE NxPlane(const NxVec3& p0, const NxVec3& p1, const NxVec3& p2)
		{
		set(p0, p1, p2);
		}

	/**
	\brief Constructor from a normal and a distance
	*/
	NX_INLINE NxPlane(const NxVec3& _n, NxF32 _d) : normal(_n), d(_d)
		{
		}

	/**
	\brief Copy constructor
	*/
	NX_INLINE NxPlane(const NxPlane& plane) : normal(plane.normal), d(plane.d)
		{
		}

	/**
	\brief Destructor
	*/
	NX_INLINE ~NxPlane()
		{
		}

	/**
	\brief Sets plane to zero.
	*/
	NX_INLINE NxPlane& zero()
		{
		normal.zero();
		d = 0.0f;
		return *this;
		}

	NX_INLINE NxPlane& set(NxF32 nx, NxF32 ny, NxF32 nz, NxF32 _d)
		{
		normal.set(nx, ny, nz);
		d = _d;
		return *this;
		}

	NX_INLINE NxPlane& set(const NxVec3& _normal, NxF32 _d)
		{
		normal = _normal;
		d = _d;
		return *this;
		}

	NX_INLINE NxPlane& set(const NxVec3& p, const NxVec3& _n)
		{
		normal = _n;
		// Plane equation: a*x + b*y + c*z + d = 0
		// p belongs to plane so:
		//     a*p.x + b*p.y + c*p.z + d = 0
		// <=> (n|p) + d = 0
		// <=> d = - (n|p)
		d = - p.dot(_n);
		return *this;
		}

	/**
	 \brief Computes the plane equation from 3 points.
	 */
	NxPlane& set(const NxVec3& p0, const NxVec3& p1, const NxVec3& p2)
		{
		NxVec3 Edge0 = p1 - p0;
		NxVec3 Edge1 = p2 - p0;

		normal = Edge0.cross(Edge1);
		normal.normalize();

		// See comments in set() for computation of d
		d = -p0.dot(normal);

		return	*this;
		}

	NX_INLINE NxF32 distance(const NxVec3& p) const
		{
		// Valid for plane equation a*x + b*y + c*z + d = 0
		return p.dot(normal) + d;
		}

	NX_INLINE bool belongs(const NxVec3& p) const
		{
		return fabsf(distance(p)) < (1.0e-7f);
		}

	/**
	\brief projects p into the plane
	*/
	NX_INLINE NxVec3 project(const NxVec3 & p) const
		{
		// Pretend p is on positive side of plane, i.e. plane.distance(p)>0.
		// To project the point we have to go in a direction opposed to plane's normal, i.e.:
		return p - normal * distance(p);
//		return p + normal * distance(p);
		}

	/**
	\brief find an arbitrary point in the plane
	*/
	NX_INLINE NxVec3 pointInPlane() const
		{
		// Project origin (0,0,0) to plane:
		// (0) - normal * distance(0) = - normal * ((p|(0)) + d) = -normal*d
		return - normal * d;
//		return normal * d;
		}

	NX_INLINE void normalize()
		{
			NxF32 Denom = 1.0f / normal.magnitude();
			normal.x	*= Denom;
			normal.y	*= Denom;
			normal.z	*= Denom;
			d			*= Denom;
		}

	NX_INLINE operator NxVec3() const
		{
		return normal;
		}

	NX_INLINE void transform(const NxMat34 & _transform, NxPlane & transformed) const
		{
		transformed.normal = _transform.M * normal;
		transformed.d = d - (_transform.t | transformed.normal);
		}

	NX_INLINE void inverseTransform(const NxMat34 & _transform, NxPlane & transformed) const
		{
		transformed.normal = _transform.M % normal;
		NxVec3 it = _transform.M %  _transform.t;
		transformed.d = d + (it | transformed.normal);
		}

	NxVec3	normal;		//!< The normal to the plane
	NxF32	d;			//!< The distance from the origin
	};

 /** @} */
#endif
//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND

#ifndef NX_FOUNDATION_NXBOUNDS3
#define NX_FOUNDATION_NXBOUNDS3
/*----------------------------------------------------------------------------*\
|
|						Public Interface to Ageia PhysX Technology
|
|							     www.ageia.com
|
\*----------------------------------------------------------------------------*/
/** \addtogroup foundation
  @{
*/


/**
	\brief Class representing 3D range or axis aligned bounding box.

	Stored as minimum and maximum extent corners. Alternate representation
	would be center and dimensions.
	May be empty or nonempty. If not empty, min <= max has to hold.
*/
class NxBounds3
	{
	public:
	NX_INLINE NxBounds3();
	NX_INLINE ~NxBounds3();

	/**
	\brief Sets empty to true
	*/
	NX_INLINE void setEmpty();

	/**
	\brief Sets infinite bounds
	*/
	NX_INLINE void setInfinite();
	
	/**
	\brief low level assignment.

	\param minx Minimum X value
	\param miny Minimum Y value
	\param minz Minimum Z value
	\param maxx Maximum X value
	\param maxy Maximum Y value
	\param maxz Maximum Z value
	*/
	NX_INLINE void set(NxReal minx, NxReal miny, NxReal minz, NxReal maxx, NxReal maxy,NxReal maxz);

	/**
	\brief vector assignment.

	\param min Minimum point of bounds.
	\param max Maximum point of bounds.
	*/
	NX_INLINE void set(const NxVec3& min, const NxVec3& max);

	/**
	\brief expands the volume to include v

	\param v Point to expand to.
	*/
	NX_INLINE void include(const NxVec3& v);

	/**
	\brief sets this to the union of this and b2.

	\param b2 Bounds to perform union with.
	*/
	NX_INLINE void combine(const NxBounds3& b2);

	/**
	\brief sets this to the AABB of the OBB passed.

	\param orientation Orientation of the OBB.
	\param translation Translation of the OBB.
	\param halfDims radii of the OBB.
	*/
	NX_INLINE void boundsOfOBB(const NxMat33& orientation, const NxVec3& translation, const NxVec3& halfDims);

	/**
	\brief transforms this volume as if it was an axis aligned bounding box, and then assigns the results' bounds to this.

	\param orientation Orientation to apply.
	\param translation Translation to apply(applied after orientation transform)
	*/
	NX_INLINE void transform(const NxMat33& orientation, const NxVec3& translation);
	
	NX_INLINE bool isEmpty() const;

	/**
	\brief indicates whether the intersection of this and b is empty or not.

	\param b Bounds to test for intersection.
	*/
	NX_INLINE bool intersects(const NxBounds3& b) const;

	/**
	\brief indicates whether the intersection of this and b is empty or not in the plane orthogonal to the axis passed (X = 0, Y = 1 or Z = 2).

	\param b Bounds to test for intersection.
	\param axisToIgnore Axis to ignore when performing the intersection test.
	*/
	NX_INLINE bool intersects2D(const NxBounds3& b, unsigned axisToIgnore) const;

	/**
	\brief indicates if these bounds contain v.

	\param v Point to test against bounds.
	*/
	NX_INLINE bool contain(const NxVec3& v) const;

	/**
	\brief returns the center of this axis aligned box.

	\param center The center of the bounds.
	*/
	NX_INLINE void getCenter(NxVec3& center) const;

	/**
	\brief returns the dimensions (width/height/depth) of this axis aligned box.

	\param dims The dimensions of the bounds.
	*/
	NX_INLINE void getDimensions(NxVec3& dims) const;

	/**
	\brief returns the extents, which are half of the width/height/depth.

	\param extents The extents/radii of the bounds.
	*/
	NX_INLINE void getExtents(NxVec3& extents) const;

	/**
	\brief setups an AABB from center & extents vectors.

	\param c Center vector
	\param e Extents vector
	*/
	NX_INLINE void setCenterExtents(const NxVec3& c, const NxVec3& e);

	/**
	\brief scales the AABB.

	\param scale Factor to scale AABB by.
	*/
	NX_INLINE void scale(NxF32 scale);

	/** 
	fattens the AABB in all 3 dimensions by the given distance. 
	*/
	NX_INLINE void fatten(NxReal distance);


	//NX_INLINE void combine(NxReal extension);

	NxVec3 min, max;
	};


NX_INLINE NxBounds3::NxBounds3()
	{
	// Default to empty boxes for compatibility TODO: PT: remove this if useless
	setEmpty();
	}


NX_INLINE NxBounds3::~NxBounds3()
	{
	//nothing
	}


NX_INLINE void NxBounds3::setEmpty()
	{
	// We know use this particular pattern for empty boxes
	set(NX_MAX_REAL, NX_MAX_REAL, NX_MAX_REAL,
		NX_MIN_REAL, NX_MIN_REAL, NX_MIN_REAL);
	}

NX_INLINE void NxBounds3::setInfinite()
	{
	set(NX_MIN_REAL, NX_MIN_REAL, NX_MIN_REAL,
		NX_MAX_REAL, NX_MAX_REAL, NX_MAX_REAL);
	}

NX_INLINE void NxBounds3::set(NxReal minx, NxReal miny, NxReal minz, NxReal maxx, NxReal maxy,NxReal maxz)
	{
	min.set(minx, miny, minz);
	max.set(maxx, maxy, maxz);
	}

NX_INLINE void NxBounds3::set(const NxVec3& _min, const NxVec3& _max)
	{
	min = _min;
	max = _max;
	}

NX_INLINE void NxBounds3::include(const NxVec3& v)
	{
	max.max(v);
	min.min(v);
	}

NX_INLINE void NxBounds3::combine(const NxBounds3& b2)
	{
	// - if we're empty, min = MAX,MAX,MAX => min will be b2 in all cases => it will copy b2, ok
	// - if b2 is empty, the opposite happens => keep us unchanged => ok
	// => same behavior as before, automatically
	min.min(b2.min);
	max.max(b2.max);
	}

NX_INLINE void NxBounds3::boundsOfOBB(const NxMat33& orientation, const NxVec3& translation, const NxVec3& halfDims)
	{
	NxReal dimx = halfDims[0];
	NxReal dimy = halfDims[1];
	NxReal dimz = halfDims[2];

	NxReal x = NxMath::abs(orientation(0,0) * dimx) + NxMath::abs(orientation(0,1) * dimy) + NxMath::abs(orientation(0,2) * dimz);
	NxReal y = NxMath::abs(orientation(1,0) * dimx) + NxMath::abs(orientation(1,1) * dimy) + NxMath::abs(orientation(1,2) * dimz);
	NxReal z = NxMath::abs(orientation(2,0) * dimx) + NxMath::abs(orientation(2,1) * dimy) + NxMath::abs(orientation(2,2) * dimz);

	set(-x + translation[0], -y + translation[1], -z + translation[2], x + translation[0], y + translation[1], z + translation[2]);
	}

NX_INLINE void NxBounds3::transform(const NxMat33& orientation, const NxVec3& translation)
	{
	// convert to center and extents form
	NxVec3 center, extents;
	getCenter(center);
	getExtents(extents);

	center = orientation * center + translation;
	boundsOfOBB(orientation, center, extents);
	}

NX_INLINE bool NxBounds3::isEmpty() const
	{
	// Consistency condition for (Min, Max) boxes: min < max
	// TODO: PT: should we test against the explicit pattern ?
	if(min.x < max.x)	return false;
	if(min.y < max.y)	return false;
	if(min.z < max.z)	return false;
	return true;
	}

NX_INLINE bool NxBounds3::intersects(const NxBounds3& b) const
	{
	if ((b.min.x > max.x) || (min.x > b.max.x)) return false;
	if ((b.min.y > max.y) || (min.y > b.max.y)) return false;
	if ((b.min.z > max.z) || (min.z > b.max.z)) return false;
	return true;
	}

NX_INLINE bool NxBounds3::intersects2D(const NxBounds3& b, unsigned axis) const
	{
	// TODO: PT: could be static and like this:
	// static unsigned i[3] = { 1,2,0,1 };
	// const unsigned ii = i[axis];
	// const unsigned jj = i[axis+1];
	const unsigned i[3] = { 1,0,0 };
	const unsigned j[3] = { 2,2,1 };
	const unsigned ii = i[axis];
	const unsigned jj = j[axis];
	if ((b.min[ii] > max[ii]) || (min[ii] > b.max[ii])) return false;
	if ((b.min[jj] > max[jj]) || (min[jj] > b.max[jj])) return false;
	return true;
	}

NX_INLINE bool NxBounds3::contain(const NxVec3& v) const
	{
	if ((v.x < min.x) || (v.x > max.x)) return false;
	if ((v.y < min.y) || (v.y > max.y)) return false;
	if ((v.z < min.z) || (v.z > max.z)) return false;
	return true;
	}

NX_INLINE void NxBounds3::getCenter(NxVec3& center) const
	{
	center.add(min,max);
	center *= NxReal(0.5);
	}

NX_INLINE void NxBounds3::getDimensions(NxVec3& dims) const
	{
	dims.subtract(max,min);
	}

NX_INLINE void NxBounds3::getExtents(NxVec3& extents) const
	{
	extents.subtract(max,min);
	extents *= NxReal(0.5);
	}

NX_INLINE void NxBounds3::setCenterExtents(const NxVec3& c, const NxVec3& e)
	{
	min = c - e;
	max = c + e;
	}

NX_INLINE void NxBounds3::scale(NxF32 _scale)
	{
	NxVec3 center, extents;
	getCenter(center);
	getExtents(extents);
	setCenterExtents(center, extents * _scale);
	}

NX_INLINE void NxBounds3::fatten(NxReal distance)
	{
	min.x -= distance;
	min.y -= distance;
	min.z -= distance;

	max.x += distance;
	max.y += distance;
	max.z += distance;
	}

#endif
//AGCOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2005 AGEIA Technologies.
// All rights reserved. www.ageia.com
///////////////////////////////////////////////////////////////////////////
//AGCOPYRIGHTEND


//NVCHANGE_BEGIN:JWR
/**
 *
 * Copyright 1998-2009 Epic Games, Inc. All Rights Reserved.
 */

#ifndef USER_MEMALLOC_H

#define USER_MEMALLOC_H


//Header file defines memory allocation macros used by the ApexClothing integration

#include <new>

#ifndef NULL
#define NULL 0
#endif

#define USER_STL std
#define USER_STL_EXT stdext

#define MEMALLOC_NEW(x) new x
#define MEMALLOC_NEW_ARRAY(x,y) new x

#define MEMALLOC_DELETE(x,y) delete y;
#define MEMALLOC_DELETE_ARRAY(x,y) delete []y;

#define MEMALLOC_MALLOC_TYPE(x,t,f,l) ::malloc(x)
#define MEMALLOC_MALLOC_TAGGED(x,t) ::malloc(x)
#define MEMALLOC_MALLOC(x) ::malloc(x)
#define MEMALLOC_FREE(x) ::free(x)
#define MEMALLOC_REALLOC(x,y) ::realloc(x,y)


#endif
//NVCHANGE_END:JWR


#ifndef HASHFUNCTION_H
#define HASHFUNCTION_H


// Disable pointer trnc warning as we do this on purpose.
#pragma warning(push)
#pragma warning(disable: 4311)

/*!
Central definition of hash functions
*/

// Hash functions
template<class T>
NxU32 HashFunction(const T& key)
{
	return (NxU32)key;
}

// Thomas Wang's 32 bit mix
// http://www.cris.com/~Ttwang/tech/inthash.htm
template<>
inline NxU32 HashFunction<NxU32>(const NxU32& key)
{
	NxU32 k = key;
	k += ~(k << 15);
	k ^= (k >> 10);
	k += (k << 3);
	k ^= (k >> 6);
	k += ~(k << 11);
	k ^= (k >> 16);
	return (NxU32)k;
}

template<>
inline NxU32 HashFunction<NxI32>(const NxI32& key)
{
	return HashFunction<NxU32>((NxU32)key);
}

// Thomas Wang's 64 bit mix
// http://www.cris.com/~Ttwang/tech/inthash.htm
template<>
inline NxU32 HashFunction<NxU64>(const NxU64& key)
{
	NxU64 k = key;
	k += ~(k << 32);
	k ^= (k >> 22);
	k += ~(k << 13);
	k ^= (k >> 8);
	k += (k << 3);
	k ^= (k >> 15);
	k += ~(k << 27);
	k ^= (k >> 31);
	return (NxU32)k;
}

// Helper for pointer hashing
template<int size>
NxU32 PointerHash(void* ptr);

template<>
inline NxU32 PointerHash<4>(void* ptr)
{
	return HashFunction<NxU32>(reinterpret_cast<NxU32>(ptr));
}


template<>
inline NxU32 PointerHash<8>(void* ptr)
{
	return HashFunction<NxU32>(reinterpret_cast<NxU32>(ptr));
}

// Hash function for pointers
template<class T>
inline NxU32 HashFunction(T* key)
{
	return PointerHash<sizeof(const void*)>(key);
}

#pragma warning(pop)

#endif


#ifndef SORTED_SET_H
#define SORTED_SET_H


/*!
A template class for handling sets. A set is a sorted array where
all elements are unique. The interface to a set is very similar to
the interface of an Array. Please consult the Array documentation for
usage.

A set is optimized for concatenation and intersection with other
sets and it is also a very efficient way of keeping a list of unique
elements.
*/
template <class T>
class SortedSet
{
public:
	static const int SET_DEFAULT_SIZE = 4;

	/*!
	Default Set constructor.
	Allocates enough room for SET_DEFAULT_SIZE elements.
	*/
	SortedSet() :
		mData(0), mCapacity(SET_DEFAULT_SIZE), mSize(0)
	{
		if (mCapacity > 0)
		{
			mData = reinterpret_cast<T*> (MEMALLOC_MALLOC(sizeof(T)*mCapacity));
		}
	}

	/*!
	Set constructor.
	Allocates enough room for size elements.

	\param size
	Size of set to be defined.
	*/
	explicit SortedSet(NxU32 size) :
		mData(0), mCapacity(size), mSize(0)
	{
		if (mCapacity > 0)
		{
			mData = reinterpret_cast<T*> (MEMALLOC_MALLOC(sizeof(T)*mCapacity));
		}
	}

	/*!
	Set copy constructor.

	\param s
	The Set that will be copied.
	*/
	SortedSet(const SortedSet<T> &s) :
		mData(0), mCapacity(s.mCapacity), mSize(s.mSize)
	{
		if(mCapacity > 0)
		{
			mData = reinterpret_cast<T*> (MEMALLOC_MALLOC(sizeof(T)*mCapacity));
			memcpy(mData, s.mData, mCapacity*sizeof(T));
		}
	}

	/*!
	Default destructor.
	*/
	~SortedSet()
  {
		MEMALLOC_FREE(mData);
	}

	/*!
	Set assignment operator.
	\param s
	The set that will be copied
	\return
	A reference to the assigned set.
	*/
	inline const SortedSet<T>& operator= (const SortedSet<T>& s)
	{
		if(&s == this)
			return *this;

		mSize = s.mSize;

		if(mCapacity < mSize)
		{
			mCapacity = s.mCapacity;
			MEMALLOC_FREE(mData);
			mData = reinterpret_cast<T*> (NX_ALLOC_PERSISTENT(sizeof(T)*mCapacity));
		}
		if(mSize > 0)
			memcpy(mData, s.mData, mSize*sizeof(T));

		return *this;
	}

	/*!
	Return an element from this set. Operation is O(1).
	\param i
	The index of the element that will be returned.
	\return
	Element i in the set.
	*/
	inline const T& get(NxU32 i) const
	{
		return mData[i];
	}

	/*!
	Return an element from this set. Operation is O(1).
	\param i
	The index of the element that will be returned.
	\return
	Element i in the set.
	*/
	inline T& get(NxU32 i)
	{
		return mData[i];
	}

	/*!
	Set indexing operator.
	\param i
	The index of the element that will be returned.
	\return
	The element i in the array.
	*/
	inline const T& operator[] (NxU32 i) const
	{
		return get(i);
	}

	/*!
	Set indexing operator.
	\param i
	The index of the element that will be returned.
	\return
	The element i in the array.
	*/
	inline T& operator[] (NxU32 i)
	{
		return get(i);
	}

	/*!
	Returns the sets array representation.
	\return
	The sets representation.
	*/
	inline const T* getPtr() const
	{
		return mData;
	}

	/*!
	Returns the number of entries in the set. This can, and probably will,
	differ from the array size.
	\return
	The number of of entries in the set.
	*/
	inline NxU32 getSize() const
	{
		return mSize;
	}

	/*!
	Clears the set.
	*/
	inline void clear()
	{
		mSize=0;
	}

	/*!
	Clear the set and release the memory
	*/
	inline void release()
	{
		mSize = 0;
		mCapacity = 0;
		MEMALLOC_FREE(mData);
		mData = 0;
	}


	/*!
	Return index of a specific element.

	\param a
	The element to search for.
	\return
	The index of this element, or -1 if not found.
	*/
	inline NxI32 getIndexOf(const T &a) const
	{
		NxI32 l=0;
		NxI32 r=mSize-1;
		NxI32 i;
		for(;r>=l;)
		{
			i=(l+r)/2;
			if(mData[i]>a)
			{
				r=i-1;
			}
			else
			{
				if(mData[i]==a)
				{
					return i;
				}
				l=i+1;
			}
		}
		return -1;
	}

	/*!
	Determine if this set contains a specific element.

	\param a
	The element
	\return
	True if the element is in the set.
	False if the element is not in the set.
	*/
	inline bool contains(const T &a)
	{
		return getIndexOf(a)!=-1;
	}

	/*!
	Insert a element to the set if it does not already exists within it.
	If this Set is full it will be expanded to hold the new element.

	\param a
	The element that will be added to this set.
	\param index
	Optional pointer to index where element is inserted.
	\return
	True if the element was inserted.
	False if the element was already in the set.
	*/
	inline bool insert(const T &a, NxU32* index = NULL)
	{
		int l=0;
		int r=mSize-1;
		int i;
		for(;r>=l;)
		{
			i=(l+r)/2;
			if(mData[i]>a)
			{
				r=i-1;
			}
			else
			{
				if(mData[i]==a)
				{
					if (index)
						*index=i;

					return false;
				}
				l=i+1;
			}
		}

		//move the elements [l count-1] to [l+1 count]
		if(mCapacity<=mSize)
			grow(1+mCapacity*2);

		if (mSize-l > 0)
			memmove(&mData[l+1],&mData[l],(mSize-l)*sizeof(T));

		mData[l]=a;
		mSize++;

		if (index)
			*index=l;

		return true;
	}

	/*!
	Remove one element from the set, if it exists.
	\param a
	The element that will be subtracted from this set.
	\return
	True if the element was removed. False if no 
	element was found.
	*/
	inline bool remove(const T &a) 
	{
		int l=0;
		int r=mSize-1;
		int i;
		for(;r>=l;) 
		{
			i=(l+r)/2;
			if(mData[i]>a) 
			{
				r=i-1;
			}
			else 
			{
				if(mData[i]==a) 
				{
					memmove(&mData[i],&mData[i+1],(mSize-i-1)*sizeof(T));
					mSize--;
					return true;
				}
				l=i+1;
			}
		}
		return false;
	}

	/*!
	Subtracts the element on position i from the set.
	Operation is O(n). Fastest removal on high index elements.
	\param i
	The position of the element that will be subtracted from this set.
	\return
	The element that was removed.
	*/
	inline T removeElement(NxU32 i) 
	{
		T tmp = mData[i];
		memmove(mData+i,mData+i+1,(mSize-i-1)*sizeof(T));
		mSize--;
		return tmp;
	}

private:

	/*!
	Resizes the available memory for the array representing the set.

	\param capacity
	The number of entries that the set should be able to hold.
	*/	
	inline void grow(NxU32 capacity) 
	{
		if(this->mCapacity < capacity)
    {
			T* newData = reinterpret_cast<T*>(MEMALLOC_MALLOC(capacity*sizeof(T)));
			memcpy(newData, mData, mSize*sizeof(T));

			MEMALLOC_FREE(mData);
			mData = newData;

			//initialize new entries
			for(NxU32 i = mCapacity; i < capacity; i++)
			{
				new ((void*)(mData+i)) T;
			}
			mCapacity = capacity;
		}
	}

	T *					mData;
	NxU32				mCapacity;
	NxU32				mSize;
};

#endif


#ifndef SIMPLE_POOL_H
#define SIMPLE_POOL_H


/*!
Simple allocation pool. Ported from LowLevel PxcSimplePool.
*/
template<class T>
class SimplePool
{
public:
	SimplePool (NxU32 elementsPerSlab = 32)
		: mElementsPerSlab (elementsPerSlab), mSlabSize (mElementSize * elementsPerSlab),
		mFreeElement (0)
	{
	}

	~SimplePool ()
	{
		disposeElements();
	}

	inline T* construct()
	{
		T* t = reinterpret_cast<T*>(allocateElement());

		if(t == 0)
			return 0;

		new (t) T;

		return t;
	}

	template<class A1>
	inline T* construct(const A1& a)
	{
		T* t = reinterpret_cast<T*>(allocateElement());

		if(t == 0)
			return 0;

		new (t) T (a);

		return t;
	}

	template<class A1>
	inline T* construct(A1& a)
	{
		T* t = reinterpret_cast<T*>(allocateElement());

		if(t == 0)
			return 0;

		new (t) T (a);

		return t;
	}

	template<class A1, class A2>
		inline T* construct(A1& a, A2& b)
	{
		T* t = reinterpret_cast<T*>(allocateElement());

		if(t == 0)
			return 0;

		new (t) T (a, b);

		return t;
	}

	template<class A1, class A2, class A3>
	inline T* construct(A1& a, A2& b, A3& c)
	{
		T* t = reinterpret_cast<T*>(allocateElement());

		if(t == 0)
			return 0;

		new (t) T (a, b, c);

		return t;
	}

	inline void destroy(T* const p)
	{
		if(p == 0)
			return;

		p->~T();
		freeElement(p);
	}

protected:
	struct FreeList
	{
		FreeList* mNext;
	};

	//Some static properties
	enum
	{
		mElementSize = sizeof(T)
	};

    // All the allocated slabs, sorted by pointer
	SortedSet<void*> mAllocatedSlabs;

	NxU32 mElementsPerSlab;
	NxU32 mSlabSize;

	FreeList* mFreeElement; // Head of free-list


	// Helper function to get bitmap of allocated elements

	// Allocate a slab and segregate it into the freelist
	inline void allocateSlab ()
	{
		NxU8* slab = reinterpret_cast<NxU8*>(MEMALLOC_MALLOC(mSlabSize));

		//Save the slab ptr
		mAllocatedSlabs.insert(slab);

		// Build a chain of nodes for the freelist
		FreeList* nextFree = mFreeElement;
		NxU8* node = slab + (mElementsPerSlab - 1) * mElementSize;
		for (; node >= slab; node -= mElementSize)
		{
			FreeList* element = reinterpret_cast<FreeList*> (node);
			element->mNext = nextFree;
			nextFree = element;
		}
		mFreeElement = nextFree;
	}

	// Free a slab
	inline void freeSlab (void* slab)
	{
		mAllocatedSlabs.remove(slab);
		MEMALLOC_FREE(slab);
	}

	// Free all slabs
	inline void freeAllSlabs ()
	{
		for(NxU32 i = 0; i < mAllocatedSlabs.getSize(); ++i)
		{
			MEMALLOC_FREE(mAllocatedSlabs[i]);
		}
		mAllocatedSlabs.release();

		mFreeElement = 0;
	}

	// Allocate space for single object
	inline void* allocateElement ()
	{
		if(mFreeElement == 0)
		{
			allocateSlab();
		}
		void* element = mFreeElement;

		mFreeElement = mFreeElement->mNext;

		return element;
	}

	// Put space for a single element back in the lists
	inline void freeElement (void* p)
	{
		FreeList* element = reinterpret_cast<FreeList*>(p);
		element->mNext = mFreeElement;
		mFreeElement = element;
	}

	/*
	Cleanup method. Go through all active slabs and call destructor for live objects,
	then free their memory
	*/
	inline void disposeElements ()
	{
		// Build a set of the free nodes
		SortedSet<void*> freeNodeSet;
		while(mFreeElement)
		{
			freeNodeSet.insert(mFreeElement);
			mFreeElement = mFreeElement->mNext;
		}

		if(freeNodeSet.getSize() != mElementsPerSlab * mAllocatedSlabs.getSize())
		{
			for(NxU32 i = 0; i < mAllocatedSlabs.getSize(); ++i)
			{
				NxU8* slab = reinterpret_cast<NxU8*>(mAllocatedSlabs[i]);
				for(NxU32 elId = 0; elId < mElementsPerSlab; ++elId)
				{
					void* element = slab + elId*mElementSize;
					if(freeNodeSet.contains(element))
						continue; //already freed, skip it

					T* t = reinterpret_cast<T*>(element);
          if ( t )
          {
					  t->~T(); // Destroy
          }
				}
			}
		}

		freeAllSlabs();
	}
};

#endif


#ifndef HASHSET_H
#define HASHSET_H

#include <string.h>
#include <assert.h>

/*!
A generic hash set.

A hash set is a specialization of a hash map where the element and the
key are the same thing, so it is a unique non-associative container.

This hashmap have the requirement of needing a hash function and a equality
predicate to function.

Warning: Requires number of buckets to be power of 2
*/
template<class T>
class HashSet
{
public:
	HashSet(size_t initialBucketCount=2048)
		: mNumBuckets(initialBucketCount), mNumElements(0), mBuckets(0)
	{
		// ensure Pow2 buckets
		assert((mNumBuckets & (mNumBuckets-1)) == 0);

		//Allocate the bucket-list
		mBuckets = reinterpret_cast<ElementType**>(MEMALLOC_MALLOC(sizeof(ElementType*)*mNumBuckets));
		memset(mBuckets, 0, sizeof(ElementType*)*mNumBuckets);
	}

	~HashSet()
	{
		MEMALLOC_FREE(mBuckets);
	}

	/*!
	Insert element into hash set.

	Returns true if element is inserted, false if it already exists in the set
	*/
	inline bool insert(const T& entry)
	{
    bool ret = false;

		// See if it exists a bucket
		NxU32 bucketID = getBucketIndex(entry);

		if(mBuckets[bucketID])
		{
			//have a bucket, check elements and insert if needed
			ElementType* e = mBuckets[bucketID];
			while(e)
			{
				if(e->mEntry == entry)
					return false;	//exists, no need to add

				e = e->mNext;
			}

			// Does not have, so add it
			e = mHashElementPool.construct();
			e->mEntry = entry;
			e->mNext = mBuckets[bucketID];
			mBuckets[bucketID] = e;
			mNumElements++;
			ret = true; //added
		}
		else
		{
			// No bucket, add one
			ElementType* e = mHashElementPool.construct();
			e->mEntry = entry;
			mBuckets[bucketID] = e;
			mNumElements++;
			ret = true; //added
		}

		return ret;
	}

  inline bool exists(const T& entry)
  {
    bool ret = false;

		if(mNumElements > 0 )
    {
  		// See if it exists a bucket
  		NxU32 bucketID = getBucketIndex(entry);
  		if(mBuckets[bucketID])
  		{
  			//have a bucket, check elements and insert if needed
  			ElementType* el = mBuckets[bucketID];
        while ( el )
        {
    			if(el->mEntry == entry)
  	  		{
            ret = true;
          }
          el = el->mNext;
        }
      }
		}

		return ret;
  }

  inline bool exists(NxU32 hashValue,T& entry)
  {
    bool ret = false;

		if(mNumElements > 0 )
    {
  		// See if it exists a bucket
  		NxU32 bucketID = getBucketIndexHash(hashValue);
  		if(mBuckets[bucketID])
  		{
  			//have a bucket, check elements and insert if needed
  			ElementType* el = mBuckets[bucketID];
        while ( el )
        {
    			if(el->mEntry->getHashValue() == hashValue )
  	  		{
            entry = el->mEntry;
            ret = true;
            break;
          }
          el = el->mNext;
        }
      }
		}

		return ret;
  }

	/*!
	Remove element from hash set
	*/
	inline bool remove(const T& entry)
	{
		if(mNumElements == 0)
			return false; //cannot remove from empty set

		// See if it exists a bucket
		NxU32 bucketID = getBucketIndex(entry);

		if(mBuckets[bucketID])
		{
			//have a bucket, check elements and insert if needed
			ElementType* el = mBuckets[bucketID];
			if(el->mEntry == entry)
			{
				//Bucket, so remove it
				mBuckets[bucketID] = el->mNext;
				mHashElementPool.destroy(el);
				mNumElements--;
				return true;
			}

			ElementType* prevEl = el;
			el = el->mNext;
			while(el)
			{
				if(el->mEntry == entry)
				{
					//unlink
					prevEl->mNext = el->mNext;
					mHashElementPool.destroy(el);
					mNumElements--;
					return true;
				}

				prevEl = el; el = el->mNext;
			}
		}

		return false;
	}

	/*!
	Clear out the set
	*/
	inline void clear()
	{
		if(mNumElements == 0)
			return; //already empty

		for(NxU32 i = 0; i < mNumBuckets; i++)
		{
			ElementType* e = mBuckets[i];
			while(e)
			{
				ElementType* prevE = e;
				e = e->mNext;

				mHashElementPool.destroy(prevE);
			}
		}

		memset(mBuckets, 0, sizeof(ElementType*)*mNumBuckets);
		mNumElements = 0;
	}

	/*!
	Get number of elements in hash set
	*/
	inline NxU32 getSize() const
	{
		return mNumElements;
	}

	/*!
	Try to set the number of buckets to a new size.
	Remember it should be power of 2.

	Returns false if not being able to resize;
	*/
	inline bool tryResize(NxU32 newBucketCount)
	{
		if(mNumElements > 0 || newBucketCount == mNumBuckets)
			return false;

		assert((newBucketCount & (newBucketCount-1)) == 0);

		mNumBuckets = newBucketCount;
		MEMALLOC_FREE(mBuckets);

		mBuckets = reinterpret_cast<ElementType**>(MEMALLOC_MALLOC(sizeof(ElementType*)*mNumBuckets));
		memset(mBuckets, 0, sizeof(ElementType*)*mNumBuckets);

		return true;
	}
private:
	NxU32			mNumBuckets;
	NxU32			mNumElements;

	template<class T2>
	struct HashElement
	{
		HashElement()
			: mNext(0)
		{
		}

		T2				mEntry;
		HashElement<T2>*	mNext;
	};
	typedef HashElement<T>	ElementType;

	SimplePool<ElementType>	mHashElementPool;

	ElementType**	mBuckets;

	//Helpers
	inline NxU32 getBucketIndex(const T& entry)
	{
		return HashFunction(entry->getHashValue()) & (mNumBuckets-1);
	}

	inline NxU32 getBucketIndexHash(NxU32 hashValue)
	{
		return HashFunction(hashValue) & (mNumBuckets-1);
	}

	template<class T3>
	friend class HashSetIterator;
};


/*!
Iterate over the content of a hash set
*/
template<class T>
class HashSetIterator
{
public:
	HashSetIterator(const HashSet<T>& hash)
		: mHash(hash), mBucket(0), mCurrentElement(0)
	{
	}


	/*!
	Get one element from the hash.

	Returns true if any element was returned, else false
	*/
	inline bool get(T& element)
	{
		//try to find next element

		if(mCurrentElement)
		{
			mCurrentElement = mCurrentElement->mNext;
			if(!mCurrentElement) mBucket++;	//next one
		}

		//find new bucket
		if(!mCurrentElement)
		{
			while(!mHash.mBuckets[mBucket] && mBucket < mHash.mNumBuckets)
				mBucket++;

			if(mHash.mNumBuckets == mBucket)
				return false;

			mCurrentElement = mHash.mBuckets[mBucket];
		}

		element = mCurrentElement->mEntry;
		return true;
	}


private:
	typedef HashSet<T>			HashType;
	const HashType&					mHash;
	NxU32							mBucket;
	typename HashType::ElementType*	mCurrentElement;
};

#endif
