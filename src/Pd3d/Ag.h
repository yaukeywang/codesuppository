
#ifndef AG_H
#define AG_H

#define NOMINMAX

///
/// -- Ag platform/gui originally based upon gamebasis library (2001-2004) in design, structure, and spirit.
///

///
/// -- determine platform and create a more consistent define for it
///
#if defined(WIN32)

	#define AGPLATFORM_W2K

#elif defined(_XBOX)

	#define AGPLATFORM_XBOX2

#else

	#error Unknown platform

#endif


#if defined(AGPLATFORM_W2K) //-- windows specific defines

	#include <cassert>

	#define AGINLINE __inline
	#define null 0L
	#define AGENABLE_USESTLCONTAINERS

#elif defined(AGPLATFORM_XBOX2)

	#include <cassert>

	#define AGINLINE __inline
	#define null 0L
	#define AGENABLE_USESTLCONTAINERS

#else

	#error Unknown platform

#endif


///
/// -- struct forward declarations
///
struct AgColor;
struct AgInt2;
struct AgInt4;
struct AgFloat2;
struct AgFloat4;


///
/// -- class forward declarations
///

class AgAbstractButton;
class AgAbstractGuiListener;
class AgBitmap;
class AgCursorPositionChanged;
class AgDataReader;
class AgFill;
class AgJoyMoved;
class AgJoyPressed;
class AgJoyReleased;
class AgKeyPressed;
class AgKeyReleased;
class AgLayoutContext;
class AgMouseMoved;
class AgMousePressed;
class AgMouseReleased;
class AgPaintGraphics;
class AgPlatform;
class AgPlatformW2K;
class AgPlatformXBOX2;
class AgPushButton;
class AgReader;
class AgReaderOfCBF;
class AgRectWidget;
class AgSlider;
class AgSolidFill;
class AgSurface;
class AgTexture;
class AgTextureFont;
class AgWindow;
class AgWriter;


///
/// -- typedef declarations
///
typedef NxU8 uchar;
typedef NxU32  uint;

#if defined(AGENABLE_SHORTHANDTYPES)
typedef AgInt2   int2;
typedef AgInt4   int4;
typedef AgFloat2 float2;
typedef AgFloat4 float4;
#endif

///
/// -- global constants
///
const NxF32 AGFLOAT_OPAQUEALPHA=1;

///
/// -- global functions
///

NxI32 dprintf(const char* format,...);


///
/// -- global inline functions
///

AGINLINE NxI32 Ag_maxi(NxI32 a,NxI32 b)
{
	if(a>b)
	{
		return a;
	}
	
	return b;
}

AGINLINE NxI32 Ag_mini(NxI32 a,NxI32 b)
{
	if(a<b)
	{
		return a;
	}

	return b;
}


#endif



