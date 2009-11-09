#include "safestdio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <math.h>
#include <hash_map>
#include <vector>
#include <new>


#pragma warning(disable:4996)
#pragma warning(disable:4100)
#pragma warning(disable:4189)

#define MAX_BUFFER_STACK 2048 // maximum number of vertices we are allowed to create on the stack.

#include "RenderDebug.h"

#include <NxVec3.h>
#include <NxPlane.h>
#include <NxMat34.h>
#include <NxQuat.h>
#include <NxBounds3.h>

#define RENDER_DEBUG_NVSHARE RENDER_DEBUG_##NVSHARE

using namespace NVSHARE;

namespace RENDER_DEBUG_NVSHARE
{

class BlockInfo;
#ifdef __CELLOS_LV2__
#define stdext std
#endif
typedef stdext::hash_map<NxU32, BlockInfo*> BlockInfoHash;

	//   font info:
	//
	//Peter Holzmann, Octopus Enterprises
	//USPS: 19611 La Mar Court, Cupertino, CA 95014
	//UUCP: {hplabs!hpdsd,pyramid}!octopus!pete
	//Phone: 408/996-7746
	//
	//This distribution is made possible through the collective encouragement
	//of the Usenet Font Consortium, a mailing list that sprang to life to get
	//this accomplished and that will now most likely disappear into the mists
	//of time... Thanks are especially due to Jim Hurt, who provided the packed
	//font data for the distribution, along with a lot of other help.
	//
	//This file describes the Hershey Fonts in general, along with a description of
	//the other files in this distribution and a simple re-distribution restriction.
	//
	//USE RESTRICTION:
	//        This distribution of the Hershey Fonts may be used by anyone for
	//        any purpose, commercial or otherwise, providing that:
	//                1. The following acknowledgements must be distributed with
	//                        the font data:
	//                        - The Hershey Fonts were originally created by Dr.
	//                                A. V. Hershey while working at the U. S.
	//                                National Bureau of Standards.
	//                        - The format of the Font data in this distribution
	//                                was originally created by
	//                                        James Hurt
	//                                        Cognition, Inc.
	//                                        900 Technology Park Drive
	//                                        Billerica, MA 01821
	//                                        (mit-eddie!ci-dandelion!hurt)
	//                2. The font data in this distribution may be converted into
	//                        any other format *EXCEPT* the format distributed by
	//                        the U.S. NTIS (which organization holds the rights
	//                        to the distribution and use of the font data in that
	//                        particular format). Not that anybody would really
	//                        *want* to use their format... each point is described
	//                        in eight bytes as "xxx yyy:", where xxx and yyy are
	//                        the coordinate values as ASCII numbers.


#define FONT_VERSION 1

NxU8 g_font[6350] = {
  0x46,0x4F,0x4E,0x54,0x01,0x00,0x00,0x00,0x43,0x01,0x00,0x00,0x5E,0x00,0x00,0x00,0xC4,0x06,0x00,0x00,0x0A,0xD7,0x23,0x3C,0x3D,0x0A,0x57,0x3E,0x0A,0xD7,0x23,0x3C,
  0x28,0x5C,0x8F,0x3D,0x0A,0xD7,0x23,0x3C,0x08,0xD7,0xA3,0x3C,0x00,0x00,0x00,0x00,0x08,0xD7,0x23,0x3C,0x0A,0xD7,0x23,0x3C,0x00,0x00,0x00,0x00,0x0A,0xD7,0xA3,0x3C,
  0x08,0xD7,0x23,0x3C,0x00,0x00,0x00,0x00,0x3D,0x0A,0x57,0x3E,0x00,0x00,0x00,0x00,0x28,0x5C,0x0F,0x3E,0x0A,0xD7,0xA3,0x3D,0x3D,0x0A,0x57,0x3E,0x0A,0xD7,0xA3,0x3D,
  0x28,0x5C,0x0F,0x3E,0x0A,0xD7,0xA3,0x3D,0x00,0x00,0x80,0x3E,0x0C,0xD7,0x23,0x3C,0x29,0x5C,0x8F,0xBD,0x29,0x5C,0x0F,0x3E,0x00,0x00,0x80,0x3E,0x29,0x5C,0x8F,0x3D,
  0x29,0x5C,0x8F,0xBD,0x0C,0xD7,0x23,0x3C,0x8F,0xC2,0xF5,0x3D,0x9A,0x99,0x19,0x3E,0x8F,0xC2,0xF5,0x3D,0x00,0x00,0x00,0x00,0x8E,0xC2,0x75,0x3D,0x29,0x5C,0x0F,0x3E,
  0x8E,0xC2,0x75,0x3D,0xCD,0xCC,0x4C,0x3D,0x00,0x00,0x80,0x3E,0xCD,0xCC,0x4C,0x3D,0x0A,0xD7,0x23,0xBD,0xEC,0x51,0xB8,0x3D,0x00,0x00,0x80,0x3E,0xEC,0x51,0xB8,0x3D,
  0x0A,0xD7,0x23,0xBD,0x29,0x5C,0x0F,0x3E,0xEB,0x51,0x38,0x3E,0x8F,0xC2,0xF5,0x3D,0xCC,0xCC,0x4C,0x3E,0xEC,0x51,0xB8,0x3D,0x3D,0x0A,0x57,0x3E,0xCD,0xCC,0x4C,0x3D,
  0x3D,0x0A,0x57,0x3E,0x0C,0xD7,0xA3,0x3C,0xCC,0xCC,0x4C,0x3E,0x00,0x00,0x00,0x00,0xEB,0x51,0x38,0x3E,0x00,0x00,0x00,0x00,0x0A,0xD7,0x23,0x3E,0x0C,0xD7,0x23,0x3C,
  0x28,0x5C,0x0F,0x3E,0x0C,0xD7,0xA3,0x3C,0xB8,0x1E,0x05,0x3E,0x0A,0xD7,0x23,0x3D,0x8F,0xC2,0xF5,0x3D,0xCD,0xCC,0xCC,0x3D,0xCC,0xCC,0xCC,0x3D,0x8F,0xC2,0xF5,0x3D,
  0xEB,0x51,0xB8,0x3D,0xB8,0x1E,0x05,0x3E,0x0A,0xD7,0xA3,0x3D,0x29,0x5C,0x0F,0x3E,0x8E,0xC2,0xF5,0x3C,0x8F,0xC2,0xF5,0x3D,0x08,0xD7,0x23,0x3C,0xEC,0x51,0xB8,0x3D,
  0x00,0x00,0x00,0x00,0xCD,0xCC,0x4C,0x3D,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x8E,0xC2,0xF5,0x3C,0xEB,0x51,0x38,0x3E,0x3D,0x0A,0x57,0x3E,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x28,0x5C,0x8F,0x3D,0x5C,0x8F,0x42,0x3E,0x28,0x5C,0x8F,0x3D,0x7A,0x14,0x2E,0x3E,0x8E,0xC2,0x75,0x3D,0x99,0x99,0x19,0x3E,0x0A,0xD7,0x23,0x3D,
  0x28,0x5C,0x0F,0x3E,0x08,0xD7,0xA3,0x3C,0x28,0x5C,0x0F,0x3E,0x08,0xD7,0x23,0x3C,0xCC,0xCC,0x4C,0x3E,0x8E,0xC2,0xF5,0x3C,0x3D,0x0A,0x57,0x3E,0x28,0x5C,0x8F,0x3D,
  0xCC,0xCC,0x4C,0x3E,0xCC,0xCC,0xCC,0x3D,0x5C,0x8F,0x42,0x3E,0xB8,0x1E,0x05,0x3E,0x5C,0x8F,0x42,0x3E,0x0A,0xD7,0x23,0x3E,0xCC,0xCC,0x4C,0x3E,0x28,0x5C,0x0F,0x3E,
  0x28,0x5C,0x8F,0x3D,0x8F,0xC2,0xF5,0x3D,0x8E,0xC2,0x75,0x3D,0xAE,0x47,0xE1,0x3D,0x0A,0xD7,0x23,0x3D,0xAE,0x47,0xE1,0x3D,0x08,0xD7,0xA3,0x3C,0xB8,0x1E,0x05,0x3E,
  0x00,0x00,0x00,0x00,0x99,0x99,0x19,0x3E,0x00,0x00,0x00,0x00,0x7A,0x14,0x2E,0x3E,0x08,0xD7,0x23,0x3C,0xEB,0x51,0x38,0x3E,0x8E,0xC2,0xF5,0x3C,0xEB,0x51,0x38,0x3E,
  0xCC,0xCC,0x4C,0x3D,0x0A,0xD7,0x23,0x3E,0x28,0x5C,0x8F,0x3D,0xCC,0xCC,0x4C,0x3E,0x8F,0xC2,0xF5,0x3D,0xCC,0xCC,0x4C,0x3E,0xB8,0x1E,0x05,0x3E,0x5C,0x8F,0x42,0x3E,
  0x28,0x5C,0x0F,0x3E,0xEB,0x51,0x38,0x3E,0x28,0x5C,0x0F,0x3E,0x7A,0x14,0x2E,0x3E,0xB8,0x1E,0x05,0x3E,0x0A,0xD7,0x23,0x3E,0xAE,0x47,0xE1,0x3D,0x8E,0xC2,0xF5,0x3D,
  0x8E,0xC2,0xF5,0x3C,0xCC,0xCC,0xCC,0x3D,0x08,0xD7,0x23,0x3C,0x0A,0xD7,0xA3,0x3D,0x00,0x00,0x00,0x00,0x09,0xD7,0x23,0x3D,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x0A,0xD7,0x23,0x3D,0x08,0xD7,0x23,0x3C,0x0A,0xD7,0xA3,0x3D,0x08,0xD7,0xA3,0x3C,0xEB,0x51,0xB8,0x3D,0xEB,0x51,0xB8,0x3D,0xB8,0x1E,0x05,0x3E,0xCC,0xCC,0xCC,0x3D,
  0x28,0x5C,0x0F,0x3E,0xAD,0x47,0xE1,0x3D,0x0A,0xD7,0x23,0x3E,0xAD,0x47,0xE1,0x3D,0xEB,0x51,0x38,0x3E,0xCC,0xCC,0xCC,0x3D,0xCC,0xCC,0x4C,0x3E,0x8E,0xC2,0x75,0x3D,
  0xCC,0xCC,0x4C,0x3E,0xCC,0xCC,0x4C,0x3D,0xEB,0x51,0x38,0x3E,0xCC,0xCC,0x4C,0x3D,0x0A,0xD7,0x23,0x3E,0x8E,0xC2,0x75,0x3D,0xB8,0x1E,0x05,0x3E,0x0A,0xD7,0xA3,0x3D,
  0xCC,0xCC,0xCC,0x3D,0xB8,0x1E,0x05,0x3E,0x8E,0xC2,0xF5,0x3C,0x99,0x99,0x19,0x3E,0x08,0xD7,0x23,0x3C,0x7A,0x14,0x2E,0x3E,0x00,0x00,0x00,0x00,0x0A,0xD7,0x23,0x3C,
  0x5C,0x8F,0x42,0x3E,0x00,0x00,0x00,0x00,0xCC,0xCC,0x4C,0x3E,0x0A,0xD7,0xA3,0x3C,0xEB,0x51,0x38,0x3E,0x0A,0xD7,0x23,0x3C,0x0A,0xD7,0x23,0x3E,0x00,0x00,0x00,0x00,
  0x99,0x99,0x19,0x3E,0x29,0x5C,0x8F,0x3D,0x00,0x00,0x80,0x3E,0xCC,0xCC,0x4C,0x3D,0x1E,0x85,0x6B,0x3E,0x8F,0xC2,0xF5,0x3C,0xCC,0xCC,0x4C,0x3E,0x00,0x00,0x00,0x00,
  0xAE,0x47,0xE1,0x3D,0x00,0x00,0x00,0x00,0x28,0x5C,0x8F,0x3D,0x8F,0xC2,0xF5,0x3C,0x0C,0xD7,0xA3,0xBC,0xCC,0xCC,0x4C,0x3D,0xCE,0xCC,0x4C,0xBD,0x00,0x00,0x00,0x00,
  0x00,0x00,0x80,0x3E,0x0A,0xD7,0xA3,0x3C,0x1E,0x85,0x6B,0x3E,0x0A,0xD7,0x23,0x3D,0xCC,0xCC,0x4C,0x3E,0x8F,0xC2,0x75,0x3D,0x0A,0xD7,0x23,0x3E,0x29,0x5C,0x8F,0x3D,
  0xAE,0x47,0xE1,0x3D,0x29,0x5C,0x8F,0x3D,0x28,0x5C,0x8F,0x3D,0x8F,0xC2,0x75,0x3D,0x08,0xD7,0xA3,0x3C,0x0A,0xD7,0x23,0x3D,0x0C,0xD7,0xA3,0xBC,0x0A,0xD7,0xA3,0x3C,
  0xCE,0xCC,0x4C,0xBD,0x00,0x00,0x00,0x00,0x29,0x5C,0x8F,0xBD,0xCC,0xCC,0x4C,0x3D,0x99,0x99,0x19,0x3E,0xCC,0xCC,0x4C,0x3D,0x8E,0xC2,0xF5,0x3C,0x00,0x00,0x00,0x00,
  0x8F,0xC2,0xF5,0x3D,0xCC,0xCC,0xCC,0x3D,0x8E,0xC2,0x75,0x3D,0xCC,0xCC,0xCC,0x3D,0x8F,0xC2,0xF5,0x3D,0xEB,0x51,0xB8,0x3D,0xEB,0x51,0x38,0x3E,0x00,0x00,0x00,0x00,
  0xEB,0x51,0xB8,0x3D,0xEB,0x51,0x38,0x3E,0xEB,0x51,0xB8,0x3D,0x0A,0xD7,0xA3,0x3C,0x0A,0xD7,0x23,0x3D,0x0A,0xD7,0x23,0x3C,0x8E,0xC2,0xF5,0x3C,0x0A,0xD7,0x23,0x3C,
  0xCC,0xCC,0x4C,0x3D,0x0A,0xD7,0xA3,0x3C,0x08,0xD7,0xA3,0x3C,0xEB,0x51,0x38,0x3E,0x00,0x00,0x80,0x3E,0x90,0xC2,0x75,0x3D,0x3D,0x0A,0x57,0x3E,0x0C,0xD7,0x23,0x3C,
  0x7A,0x14,0x2E,0x3E,0x0C,0xD7,0x23,0x3C,0x0A,0xD7,0x23,0x3D,0x90,0xC2,0xF5,0x3C,0x08,0xD7,0x23,0x3C,0x90,0xC2,0x75,0x3D,0x00,0x00,0x00,0x00,0xAE,0x47,0xE1,0x3D,
  0x08,0xD7,0x23,0x3C,0xB8,0x1E,0x05,0x3E,0x0A,0xD7,0x23,0x3D,0x29,0x5C,0x0F,0x3E,0xEB,0x51,0xB8,0x3D,0x29,0x5C,0x0F,0x3E,0x8F,0xC2,0xF5,0x3D,0xB8,0x1E,0x05,0x3E,
  0x7A,0x14,0x2E,0x3E,0xAE,0x47,0xE1,0x3D,0xCC,0xCC,0x4C,0x3E,0x00,0x00,0x00,0x00,0x7A,0x14,0x2E,0x3E,0x0C,0xD7,0xA3,0x3C,0x5C,0x8F,0x42,0x3E,0x8F,0xC2,0xF5,0x3D,
  0x5C,0x8F,0x42,0x3E,0xB8,0x1E,0x05,0x3E,0x99,0x99,0x19,0x3E,0x8F,0xC2,0xF5,0x3D,0xB8,0x1E,0x05,0x3E,0x29,0x5C,0x0F,0x3E,0x00,0x00,0x00,0x00,0x0C,0xD7,0xA3,0x3C,
  0x3D,0x0A,0x57,0x3E,0xB8,0x1E,0x05,0x3E,0x3D,0x0A,0x57,0x3E,0x29,0x5C,0x8F,0x3D,0xB8,0x1E,0x05,0x3E,0xCD,0xCC,0xCC,0x3D,0xB8,0x1E,0x05,0x3E,0x8F,0xC2,0xF5,0x3D,
  0x8F,0xC2,0xF5,0x3D,0xB8,0x1E,0x05,0x3E,0xAE,0x47,0xE1,0x3D,0x29,0x5C,0x0F,0x3E,0x0A,0xD7,0xA3,0x3D,0xCD,0xCC,0xCC,0x3D,0x3D,0x0A,0x57,0x3E,0x9A,0x99,0x19,0x3E,
  0x28,0x5C,0x8F,0x3D,0xCD,0xCC,0xCC,0x3D,0x00,0x00,0x00,0x00,0x8F,0xC2,0xF5,0x3D,0x3D,0x0A,0x57,0x3E,0xCD,0xCC,0x4C,0x3D,0x28,0x5C,0x0F,0x3E,0xAE,0x47,0xE1,0x3D,
  0xB8,0x1E,0x05,0x3E,0x8F,0xC2,0xF5,0x3D,0xEB,0x51,0x38,0x3E,0x29,0x5C,0x8F,0x3D,0x00,0x00,0x00,0x00,0xB8,0x1E,0x05,0x3E,0x8E,0xC2,0x75,0x3D,0xB8,0x1E,0x05,0x3E,
  0x28,0x5C,0x8F,0x3D,0x8F,0xC2,0xF5,0x3D,0xCC,0xCC,0xCC,0x3D,0x8F,0xC2,0xF5,0x3C,0x8F,0xC2,0xF5,0x3D,0x0C,0xD7,0x23,0x3C,0xCC,0xCC,0xCC,0x3D,0x29,0x5C,0x0F,0x3E,
  0x3D,0x0A,0x57,0x3E,0x0C,0xD7,0x23,0x3C,0xEB,0x51,0x38,0x3E,0x0A,0xD7,0x23,0x3D,0xB8,0x1E,0x05,0x3E,0x0A,0xD7,0xA3,0x3D,0x8F,0xC2,0xF5,0x3D,0xAE,0x47,0xE1,0x3D,
  0xAE,0x47,0xE1,0x3D,0xB8,0x1E,0x05,0x3E,0xEB,0x51,0xB8,0x3D,0x29,0x5C,0x0F,0x3E,0x0A,0xD7,0x23,0x3D,0xB8,0x1E,0x05,0x3E,0x08,0xD7,0xA3,0x3C,0x0C,0xD7,0x23,0x3C,
  0xEB,0x51,0xB8,0x3D,0x90,0xC2,0xF5,0x3C,0xAE,0x47,0xE1,0x3D,0x90,0xC2,0x75,0x3D,0x8F,0xC2,0xF5,0x3D,0x8F,0xC2,0xF5,0x3D,0x28,0x5C,0x0F,0x3E,0xB8,0x1E,0x05,0x3E,
  0x0A,0xD7,0x23,0x3E,0xB8,0x1E,0x05,0x3E,0xEB,0x51,0x38,0x3E,0xB8,0x1E,0x05,0x3E,0x28,0x5C,0x0F,0x3E,0x8F,0xC2,0xF5,0x3D,0xAE,0x47,0xE1,0x3D,0xCD,0xCC,0xCC,0x3D,
  0xEB,0x51,0xB8,0x3D,0x29,0x5C,0x8F,0x3D,0x0A,0xD7,0xA3,0x3D,0x90,0xC2,0x75,0x3D,0x0A,0xD7,0xA3,0x3D,0x90,0xC2,0xF5,0x3C,0xEB,0x51,0xB8,0x3D,0x0C,0xD7,0x23,0x3C,
  0xAE,0x47,0xE1,0x3D,0x29,0x5C,0x8F,0x3D,0x3D,0x0A,0x57,0x3E,0x8F,0xC2,0xF5,0x3D,0x0A,0xD7,0x23,0x3D,0x0A,0xD7,0xA3,0x3C,0xAE,0x47,0xE1,0x3D,0x0A,0xD7,0x23,0x3E,
  0xEB,0x51,0x38,0x3E,0x0A,0xD7,0x23,0x3E,0x00,0x00,0x00,0x00,0xEB,0x51,0x38,0x3E,0x8F,0xC2,0xF5,0x3D,0xEB,0x51,0x38,0x3E,0x8E,0xC2,0x75,0x3D,0x0A,0xD7,0x23,0x3E,
  0xEB,0x51,0xB8,0x3D,0x0A,0xD7,0x23,0x3D,0x3D,0x0A,0x57,0x3E,0xAE,0x47,0xE1,0x3D,0x5C,0x8F,0x42,0x3E,0x8F,0xC2,0xF5,0x3D,0x7A,0x14,0x2E,0x3E,0x8F,0xC2,0xF5,0x3D,
  0x99,0x99,0x19,0x3E,0x8F,0xC2,0x75,0x3D,0xCC,0xCC,0xCC,0x3D,0x8F,0xC2,0x75,0x3D,0x28,0x5C,0x8F,0x3D,0xCC,0xCC,0x4C,0x3D,0x08,0xD7,0x23,0x3C,0x29,0x5C,0x8F,0x3D,
  0x08,0xD7,0x23,0x3C,0xEB,0x51,0xB8,0x3D,0x99,0x99,0x19,0x3E,0x28,0x5C,0x8F,0x3D,0x0A,0xD7,0x23,0x3E,0x0A,0xD7,0x23,0x3D,0x0A,0xD7,0x23,0x3E,0x09,0xD7,0xA3,0x3C,
  0x99,0x99,0x19,0x3E,0x00,0x00,0x00,0x00,0x0A,0xD7,0xA3,0x3D,0x08,0xD7,0x23,0x3C,0x8E,0xC2,0x75,0x3D,0x8E,0xC2,0xF5,0x3C,0xCC,0xCC,0x4C,0x3D,0x8E,0xC2,0x75,0x3D,
  0xCC,0xCC,0x4C,0x3D,0x0A,0xD7,0xA3,0x3D,0x8E,0xC2,0x75,0x3D,0xEB,0x51,0xB8,0x3D,0x0A,0xD7,0xA3,0x3D,0x09,0xD7,0xA3,0x3C,0x8E,0xC2,0x75,0x3D,0xCC,0xCC,0xCC,0x3D,
  0x0A,0xD7,0x23,0x3E,0xEB,0x51,0xB8,0x3D,0x8E,0xC2,0x75,0x3D,0xAE,0x47,0xE1,0x3D,0xCC,0xCC,0x4C,0x3D,0xB8,0x1E,0x05,0x3E,0xCC,0xCC,0x4C,0x3D,0x0A,0xD7,0x23,0x3E,
  0xCC,0xCC,0xCC,0x3D,0x0A,0xD7,0x23,0x3E,0x8F,0xC2,0xF5,0x3D,0x99,0x99,0x19,0x3E,0x99,0x99,0x19,0x3E,0x28,0x5C,0x0F,0x3E,0x7A,0x14,0x2E,0x3E,0x90,0xC2,0xF5,0x3C,
  0x28,0x5C,0x8F,0x3D,0x29,0x5C,0x0F,0x3E,0x99,0x99,0x19,0x3E,0xB8,0x1E,0x05,0x3E,0xB8,0x1E,0x05,0x3E,0xEC,0x51,0xB8,0x3D,0xAE,0x47,0xE1,0x3D,0x9A,0x99,0x19,0x3E,
  0x0A,0xD7,0x23,0x3E,0x00,0x00,0x00,0x00,0xB8,0x1E,0x05,0x3E,0x0C,0xD7,0xA3,0x3C,0x8E,0xC2,0xF5,0x3C,0x0A,0xD7,0x23,0x3D,0x08,0xD7,0x23,0x3C,0x9A,0x99,0x19,0x3E,
  0xCC,0xCC,0x4C,0x3D,0x29,0x5C,0x0F,0x3E,0xB8,0x1E,0x05,0x3E,0x0A,0xD7,0xA3,0x3D,0xAE,0x47,0xE1,0x3D,0x9A,0x99,0x19,0x3E,0x0A,0xD7,0xA3,0x3D,0xCD,0xCC,0xCC,0x3D,
  0x0A,0xD7,0xA3,0x3D,0x29,0x5C,0x0F,0x3E,0xAE,0x47,0xE1,0x3D,0xCC,0xCC,0xCC,0x3D,0xCC,0xCC,0x4C,0x3D,0xEB,0x51,0xB8,0x3D,0x08,0xD7,0xA3,0x3C,0x0A,0xD7,0xA3,0x3D,
  0x08,0xD7,0x23,0x3C,0x00,0x00,0x00,0x00,0xCC,0xCC,0x4C,0x3D,0xCD,0xCC,0x4C,0x3D,0x8F,0xC2,0xF5,0x3D,0x8F,0xC2,0xF5,0x3D,0x00,0x00,0x00,0x00,0x0A,0xD7,0x23,0x3E,
  0x3D,0x0A,0x57,0x3E,0x0A,0xD7,0x23,0x3E,0x0A,0xD7,0xA3,0x3D,0x0A,0xD7,0x23,0x3E,0xB8,0x1E,0x05,0x3E,0x29,0x5C,0x0F,0x3E,0x28,0x5C,0x0F,0x3E,0xB8,0x1E,0x05,0x3E,
  0x8F,0xC2,0xF5,0x3D,0xEC,0x51,0xB8,0x3D,0xCC,0xCC,0xCC,0x3D,0x00,0x00,0x00,0x00,0xCC,0xCC,0xCC,0x3D,0xEB,0x51,0xB8,0x3D,0x0A,0xD7,0x23,0x3D,0x9A,0x99,0x19,0x3E,
  0x0C,0xD7,0xA3,0xBC,0xCC,0xCC,0x4C,0x3E,0x3D,0x0A,0x57,0x3E,0x0A,0xD7,0x23,0x3C,0x00,0x00,0x80,0x3E,0x29,0x5C,0x0F,0x3E,0x90,0xC2,0xF5,0xBC,0x8F,0xC2,0x75,0x3D,
  0x00,0x00,0x80,0x3E,0x8F,0xC2,0x75,0x3D,0x29,0x5C,0x8F,0xBD,0x0A,0xD7,0xA3,0x3D,0x1E,0x85,0x6B,0x3E,0xEB,0x51,0x38,0x3E,0x29,0x5C,0x8F,0xBD,0x0A,0xD7,0xA3,0x3C,
  0x0A,0xD7,0x23,0x3E,0x0A,0xD7,0xA3,0x3C,0x8F,0xC2,0xF5,0x3D,0x0A,0xD7,0x23,0x3C,0xB8,0x1E,0x05,0x3E,0x8F,0xC2,0xF5,0x3C,0xB8,0x1E,0x05,0x3E,0x29,0x5C,0x8F,0x3D,
  0x28,0x5C,0x0F,0x3E,0x8F,0xC2,0xF5,0x3D,0x0A,0xD7,0xA3,0x3D,0xAE,0x47,0xE1,0x3D,0x8E,0xC2,0xF5,0x3C,0xEB,0x51,0xB8,0x3D,0x08,0xD7,0x23,0x3C,0xAE,0x47,0xE1,0x3D,
  0x8F,0xC2,0xF5,0x3D,0x8F,0xC2,0xF5,0x3C,0x7A,0x14,0x2E,0x3E,0x8F,0xC2,0xF5,0x3C,0x00,0x00,0x00,0x00,0x8F,0xC2,0xF5,0x3D,0x0C,0xD7,0xA3,0xBC,0xAE,0x47,0xE1,0x3D,
  0xCE,0xCC,0x4C,0xBD,0xCC,0xCC,0xCC,0x3D,0x8E,0xC2,0x75,0xBD,0x0A,0xD7,0xA3,0x3D,0x29,0x5C,0x8F,0xBD,0xCC,0xCC,0x4C,0x3D,0x29,0x5C,0x8F,0xBD,0x8F,0xC2,0xF5,0x3C,
  0x8E,0xC2,0x75,0xBD,0xAE,0x47,0xE1,0x3D,0xCC,0xCC,0xCC,0x3D,0xAE,0x47,0xE1,0x3D,0x00,0x00,0x00,0x00,0x0A,0xD7,0x23,0x3C,0xAE,0x47,0x61,0x3E,0xCC,0xCC,0x4C,0x3D,
  0xCC,0xCC,0x4C,0x3E,0xCC,0xCC,0x4C,0x3D,0xAE,0x47,0x61,0x3E,0xCC,0xCC,0x4C,0x3D,0x90,0xC2,0xF5,0xBC,0x0A,0xD7,0x23,0x3D,0x8E,0xC2,0x75,0xBD,0x0A,0xD7,0xA3,0x3C,
  0x29,0x5C,0x8F,0xBD,0x0A,0xD7,0x23,0x3D,0x0A,0xD7,0xA3,0x3D,0x0A,0xD7,0x23,0x3E,0x28,0x5C,0x0F,0x3E,0x3D,0x0A,0x57,0x3E,0xB8,0x1E,0x05,0x3E,0xAE,0x47,0x61,0x3E,
  0xCC,0xCC,0xCC,0x3D,0xAE,0x47,0x61,0x3E,0x00,0x00,0x00,0x00,0x8F,0xC2,0xF5,0x3D,0x29,0x5C,0x8F,0xBD,0x8E,0xC2,0xF5,0x3C,0x0A,0xD7,0xA3,0x3D,0x0A,0xD7,0xA3,0x3D,
  0x28,0x5C,0x8F,0x3D,0x08,0xD7,0x23,0x3C,0x08,0xD7,0x23,0x3C,0x8F,0xC2,0xF5,0x3C,0x0A,0xD7,0x23,0x3D,0xAE,0x47,0xE1,0x3D,0x28,0x5C,0x0F,0x3E,0x8F,0xC2,0xF5,0x3C,
  0x8E,0xC2,0x75,0x3E,0x0A,0xD7,0xA3,0x3C,0x7A,0x14,0x2E,0x3E,0x8F,0xC2,0xF5,0x3C,0x0A,0xD7,0x23,0x3E,0x0A,0xD7,0xA3,0x3C,0xCC,0xCC,0xCC,0x3D,0x0A,0xD7,0xA3,0x3C,
  0xAE,0x47,0x61,0x3E,0x8F,0xC2,0xF5,0x3C,0xEB,0x51,0x38,0x3E,0x0A,0xD7,0x23,0x3D,0x7A,0x14,0x2E,0x3E,0xCC,0xCC,0x4C,0x3D,0xB8,0x1E,0x05,0x3E,0x0A,0xD7,0x23,0x3D,
  0xAE,0x47,0xE1,0x3D,0x0A,0xD7,0x23,0x3D,0x28,0x5C,0x8F,0x3D,0xCC,0xCC,0x4C,0x3D,0xCC,0xCC,0x4C,0x3D,0x0A,0xD7,0xA3,0x3C,0x0C,0xD7,0xA3,0xBC,0x0A,0xD7,0xA3,0x3C,
  0x0A,0xD7,0x23,0xBD,0x0A,0xD7,0xA3,0x3C,0x0A,0xD7,0xA3,0x3D,0x0A,0xD7,0x23,0x3D,0x8E,0xC2,0x75,0x3D,0x0A,0xD7,0xA3,0x3C,0x8E,0xC2,0x75,0x3E,0x8F,0xC2,0xF5,0x3C,
  0x1E,0x85,0x6B,0x3E,0x0A,0xD7,0x23,0x3D,0x5C,0x8F,0x42,0x3E,0x8F,0xC2,0xF5,0x3C,0xCC,0xCC,0xCC,0x3D,0x8F,0xC2,0xF5,0x3C,0xAE,0x47,0x61,0x3E,0xCC,0xCC,0x4C,0x3D,
  0xEB,0x51,0xB8,0x3D,0x0A,0xD7,0xA3,0x3C,0x00,0x00,0x00,0x00,0x8F,0xC2,0xF5,0x3C,0x0A,0xD7,0x23,0xBD,0x0A,0xD7,0xA3,0x3C,0x8E,0xC2,0x75,0xBD,0xAE,0x47,0xE1,0x3D,
  0x0A,0xD7,0xA3,0x3D,0x7A,0x14,0x2E,0x3E,0x0A,0xD7,0xA3,0x3D,0xEB,0x51,0x38,0x3E,0xCC,0xCC,0xCC,0x3D,0xCC,0xCC,0x4C,0x3D,0xAE,0x47,0xE1,0x3D,0x28,0x5C,0x8F,0x3D,
  0xCC,0xCC,0xCC,0x3D,0xAE,0x47,0xE1,0x3D,0x28,0x5C,0x8F,0x3D,0x99,0x99,0x19,0x3E,0x8E,0xC2,0x75,0x3D,0x7A,0x14,0x2E,0x3E,0x28,0x5C,0x8F,0x3D,0xAE,0x47,0xE1,0x3D,
  0x3D,0x0A,0x57,0x3E,0x9A,0x99,0x19,0x3E,0x3D,0x0A,0x57,0x3E,0x21,0x0A,0x00,0x00,0x00,0x01,0x00,0x02,0x00,0x03,0x00,0x03,0x00,0x04,0x00,0x04,0x00,0x05,0x00,0x05,
  0x00,0x02,0x00,0x22,0x04,0x00,0x06,0x00,0x07,0x00,0x08,0x00,0x09,0x00,0x23,0x08,0x00,0x0A,0x00,0x0B,0x00,0x0C,0x00,0x0D,0x00,0x0E,0x00,0x0F,0x00,0x10,0x00,0x11,
  0x00,0x24,0x2A,0x00,0x12,0x00,0x13,0x00,0x14,0x00,0x15,0x00,0x16,0x00,0x17,0x00,0x17,0x00,0x18,0x00,0x18,0x00,0x19,0x00,0x19,0x00,0x1A,0x00,0x1A,0x00,0x1B,0x00,
  0x1B,0x00,0x1C,0x00,0x1C,0x00,0x1D,0x00,0x1D,0x00,0x1E,0x00,0x1E,0x00,0x1F,0x00,0x1F,0x00,0x20,0x00,0x20,0x00,0x21,0x00,0x21,0x00,0x22,0x00,0x22,0x00,0x11,0x00,
  0x11,0x00,0x23,0x00,0x23,0x00,0x24,0x00,0x24,0x00,0x25,0x00,0x25,0x00,0x26,0x00,0x26,0x00,0x05,0x00,0x05,0x00,0x27,0x00,0x25,0x34,0x00,0x28,0x00,0x29,0x00,0x19,
  0x00,0x2A,0x00,0x2A,0x00,0x2B,0x00,0x2B,0x00,0x2C,0x00,0x2C,0x00,0x2D,0x00,0x2D,0x00,0x2E,0x00,0x2E,0x00,0x1C,0x00,0x1C,0x00,0x1B,0x00,0x1B,0x00,0x2F,0x00,0x2F,
  0x00,0x30,0x00,0x30,0x00,0x19,0x00,0x19,0x00,0x31,0x00,0x31,0x00,0x32,0x00,0x32,0x00,0x33,0x00,0x33,0x00,0x34,0x00,0x34,0x00,0x28,0x00,0x35,0x00,0x36,0x00,0x36,
  0x00,0x37,0x00,0x37,0x00,0x38,0x00,0x38,0x00,0x39,0x00,0x39,0x00,0x3A,0x00,0x3A,0x00,0x3B,0x00,0x3B,0x00,0x3C,0x00,0x3C,0x00,0x3D,0x00,0x3D,0x00,0x3E,0x00,0x3E,
  0x00,0x35,0x00,0x26,0x3C,0x00,0x3F,0x00,0x40,0x00,0x40,0x00,0x41,0x00,0x41,0x00,0x42,0x00,0x42,0x00,0x43,0x00,0x43,0x00,0x44,0x00,0x44,0x00,0x11,0x00,0x11,0x00,
  0x45,0x00,0x45,0x00,0x46,0x00,0x46,0x00,0x47,0x00,0x47,0x00,0x48,0x00,0x48,0x00,0x05,0x00,0x05,0x00,0x02,0x00,0x02,0x00,0x49,0x00,0x49,0x00,0x10,0x00,0x10,0x00,
  0x4A,0x00,0x4A,0x00,0x4B,0x00,0x4B,0x00,0x4C,0x00,0x4C,0x00,0x4D,0x00,0x4D,0x00,0x4E,0x00,0x4E,0x00,0x4F,0x00,0x4F,0x00,0x50,0x00,0x50,0x00,0x08,0x00,0x08,0x00,
  0x51,0x00,0x51,0x00,0x52,0x00,0x52,0x00,0x53,0x00,0x53,0x00,0x54,0x00,0x54,0x00,0x55,0x00,0x55,0x00,0x56,0x00,0x56,0x00,0x57,0x00,0x57,0x00,0x58,0x00,0x27,0x0C,
  0x00,0x59,0x00,0x5A,0x00,0x5A,0x00,0x00,0x00,0x00,0x00,0x1A,0x00,0x1A,0x00,0x5B,0x00,0x5B,0x00,0x5C,0x00,0x5C,0x00,0x5D,0x00,0x28,0x12,0x00,0x5E,0x00,0x5F,0x00,
  0x5F,0x00,0x60,0x00,0x60,0x00,0x5C,0x00,0x5C,0x00,0x61,0x00,0x61,0x00,0x62,0x00,0x62,0x00,0x02,0x00,0x02,0x00,0x63,0x00,0x63,0x00,0x64,0x00,0x64,0x00,0x0D,0x00,
  0x29,0x12,0x00,0x65,0x00,0x66,0x00,0x66,0x00,0x67,0x00,0x67,0x00,0x68,0x00,0x68,0x00,0x69,0x00,0x69,0x00,0x6A,0x00,0x6A,0x00,0x6B,0x00,0x6B,0x00,0x6C,0x00,0x6C,
  0x00,0x6D,0x00,0x6D,0x00,0x6E,0x00,0x2A,0x06,0x00,0x6F,0x00,0x70,0x00,0x71,0x00,0x72,0x00,0x73,0x00,0x10,0x00,0x2B,0x04,0x00,0x74,0x00,0x25,0x00,0x75,0x00,0x76,
  0x00,0x2C,0x0C,0x00,0x77,0x00,0x78,0x00,0x78,0x00,0x49,0x00,0x49,0x00,0x79,0x00,0x79,0x00,0x77,0x00,0x77,0x00,0x7A,0x00,0x7A,0x00,0x29,0x00,0x2D,0x02,0x00,0x75,
  0x00,0x76,0x00,0x2E,0x08,0x00,0x79,0x00,0x49,0x00,0x49,0x00,0x78,0x00,0x78,0x00,0x77,0x00,0x77,0x00,0x79,0x00,0x2F,0x02,0x00,0x7B,0x00,0x6E,0x00,0x30,0x20,0x00,
  0x7C,0x00,0x60,0x00,0x60,0x00,0x7D,0x00,0x7D,0x00,0x71,0x00,0x71,0x00,0x75,0x00,0x75,0x00,0x7E,0x00,0x7E,0x00,0x7F,0x00,0x7F,0x00,0x80,0x00,0x80,0x00,0x47,0x00,
  0x47,0x00,0x81,0x00,0x81,0x00,0x82,0x00,0x82,0x00,0x83,0x00,0x83,0x00,0x84,0x00,0x84,0x00,0x85,0x00,0x85,0x00,0x86,0x00,0x86,0x00,0x08,0x00,0x08,0x00,0x7C,0x00,
  0x31,0x06,0x00,0x87,0x00,0x5B,0x00,0x5B,0x00,0x19,0x00,0x19,0x00,0x26,0x00,0x32,0x1A,0x00,0x5C,0x00,0x7D,0x00,0x7D,0x00,0x88,0x00,0x88,0x00,0x60,0x00,0x60,0x00,
  0x19,0x00,0x19,0x00,0x18,0x00,0x18,0x00,0x86,0x00,0x86,0x00,0x89,0x00,0x89,0x00,0x85,0x00,0x85,0x00,0x8A,0x00,0x8A,0x00,0x8B,0x00,0x8B,0x00,0x20,0x00,0x20,0x00,
  0x29,0x00,0x29,0x00,0x8C,0x00,0x33,0x1C,0x00,0x8D,0x00,0x8E,0x00,0x8E,0x00,0x8F,0x00,0x8F,0x00,0x90,0x00,0x90,0x00,0x91,0x00,0x91,0x00,0x92,0x00,0x92,0x00,0x93,
  0x00,0x93,0x00,0x11,0x00,0x11,0x00,0x56,0x00,0x56,0x00,0x81,0x00,0x81,0x00,0x47,0x00,0x47,0x00,0x26,0x00,0x26,0x00,0x05,0x00,0x05,0x00,0x02,0x00,0x02,0x00,0x49,
  0x00,0x34,0x06,0x00,0x94,0x00,0x62,0x00,0x62,0x00,0x95,0x00,0x94,0x00,0x96,0x00,0x35,0x20,0x00,0x97,0x00,0x8D,0x00,0x8D,0x00,0x0E,0x00,0x0E,0x00,0x1E,0x00,0x1E,
  0x00,0x98,0x00,0x98,0x00,0x09,0x00,0x09,0x00,0x99,0x00,0x99,0x00,0x92,0x00,0x92,0x00,0x93,0x00,0x93,0x00,0x11,0x00,0x11,0x00,0x56,0x00,0x56,0x00,0x81,0x00,0x81,
  0x00,0x47,0x00,0x47,0x00,0x26,0x00,0x26,0x00,0x05,0x00,0x05,0x00,0x02,0x00,0x02,0x00,0x49,0x00,0x36,0x2C,0x00,0x9A,0x00,0x86,0x00,0x86,0x00,0x08,0x00,0x08,0x00,
  0x7C,0x00,0x7C,0x00,0x60,0x00,0x60,0x00,0x7D,0x00,0x7D,0x00,0x71,0x00,0x71,0x00,0x62,0x00,0x62,0x00,0x78,0x00,0x78,0x00,0x7F,0x00,0x7F,0x00,0x80,0x00,0x80,0x00,
  0x9B,0x00,0x9B,0x00,0x46,0x00,0x46,0x00,0x45,0x00,0x45,0x00,0x9C,0x00,0x9C,0x00,0x9D,0x00,0x9D,0x00,0x9E,0x00,0x9E,0x00,0x73,0x00,0x73,0x00,0x8F,0x00,0x8F,0x00,
  0x54,0x00,0x54,0x00,0x9F,0x00,0x9F,0x00,0xA0,0x00,0xA0,0x00,0x62,0x00,0x37,0x04,0x00,0xA1,0x00,0x48,0x00,0x06,0x00,0xA1,0x00,0x38,0x38,0x00,0x19,0x00,0x1A,0x00,
  0x1A,0x00,0xA2,0x00,0xA2,0x00,0x5C,0x00,0x5C,0x00,0x2E,0x00,0x2E,0x00,0xA3,0x00,0xA3,0x00,0xA4,0x00,0xA4,0x00,0xA5,0x00,0xA5,0x00,0xA6,0x00,0xA6,0x00,0x35,0x00,
  0x35,0x00,0xA7,0x00,0xA7,0x00,0xA8,0x00,0xA8,0x00,0x24,0x00,0x24,0x00,0x25,0x00,0x25,0x00,0x26,0x00,0x26,0x00,0x05,0x00,0x05,0x00,0x02,0x00,0x02,0x00,0x49,0x00,
  0x49,0x00,0x62,0x00,0x62,0x00,0xA9,0x00,0xA9,0x00,0xAA,0x00,0xAA,0x00,0xAB,0x00,0xAB,0x00,0x90,0x00,0x90,0x00,0xAC,0x00,0xAC,0x00,0xAD,0x00,0xAD,0x00,0xAE,0x00,
  0xAE,0x00,0x17,0x00,0x17,0x00,0x18,0x00,0x18,0x00,0x19,0x00,0x39,0x2C,0x00,0xAF,0x00,0xB0,0x00,0xB0,0x00,0xB1,0x00,0xB1,0x00,0xB2,0x00,0xB2,0x00,0xB3,0x00,0xB3,
  0x00,0xB4,0x00,0xB4,0x00,0xB5,0x00,0xB5,0x00,0x07,0x00,0x07,0x00,0x5D,0x00,0x5D,0x00,0xA2,0x00,0xA2,0x00,0x60,0x00,0x60,0x00,0x7C,0x00,0x7C,0x00,0xB6,0x00,0xB6,
  0x00,0x50,0x00,0x50,0x00,0x9A,0x00,0x9A,0x00,0xAF,0x00,0xAF,0x00,0xA6,0x00,0xA6,0x00,0xB7,0x00,0xB7,0x00,0x46,0x00,0x46,0x00,0x9B,0x00,0x9B,0x00,0x26,0x00,0x26,
  0x00,0x05,0x00,0x05,0x00,0x78,0x00,0x3A,0x10,0x00,0x0E,0x00,0x61,0x00,0x61,0x00,0xA0,0x00,0xA0,0x00,0xB8,0x00,0xB8,0x00,0x0E,0x00,0x79,0x00,0x49,0x00,0x49,0x00,
  0x78,0x00,0x78,0x00,0x77,0x00,0x77,0x00,0x79,0x00,0x3B,0x14,0x00,0x0E,0x00,0x61,0x00,0x61,0x00,0xA0,0x00,0xA0,0x00,0xB8,0x00,0xB8,0x00,0x0E,0x00,0x77,0x00,0x78,
  0x00,0x78,0x00,0x49,0x00,0x49,0x00,0x79,0x00,0x79,0x00,0x77,0x00,0x77,0x00,0x7A,0x00,0x7A,0x00,0x29,0x00,0x3C,0x04,0x00,0xB9,0x00,0x75,0x00,0x75,0x00,0xBA,0x00,
  0x3D,0x04,0x00,0x71,0x00,0xBB,0x00,0x10,0x00,0xBC,0x00,0x3E,0x04,0x00,0x1B,0x00,0xBD,0x00,0xBD,0x00,0x29,0x00,0x3F,0x22,0x00,0x1C,0x00,0x87,0x00,0x87,0x00,0x59,
  0x00,0x59,0x00,0x1A,0x00,0x1A,0x00,0xBE,0x00,0xBE,0x00,0x08,0x00,0x08,0x00,0x50,0x00,0x50,0x00,0xBF,0x00,0xBF,0x00,0xC0,0x00,0xC0,0x00,0xC1,0x00,0xC1,0x00,0x99,
  0x00,0x99,0x00,0x73,0x00,0x73,0x00,0xC2,0x00,0xC2,0x00,0xC3,0x00,0x6B,0x00,0xC4,0x00,0xC4,0x00,0x80,0x00,0x80,0x00,0xC5,0x00,0xC5,0x00,0x6B,0x00,0x40,0x34,0x00,
  0x90,0x00,0xC6,0x00,0xC6,0x00,0xC7,0x00,0xC7,0x00,0xC8,0x00,0xC8,0x00,0xC9,0x00,0xC9,0x00,0x1D,0x00,0x1D,0x00,0x61,0x00,0x61,0x00,0xCA,0x00,0xCA,0x00,0xCB,0x00,
  0xCB,0x00,0xCC,0x00,0xCC,0x00,0xCD,0x00,0xCD,0x00,0xCE,0x00,0xCE,0x00,0xCF,0x00,0xC8,0x00,0x2E,0x00,0x2E,0x00,0xB5,0x00,0xB5,0x00,0x4A,0x00,0x4A,0x00,0xD0,0x00,
  0xD0,0x00,0xCC,0x00,0xD1,0x00,0xCF,0x00,0xCF,0x00,0xD2,0x00,0xD2,0x00,0xD3,0x00,0xD3,0x00,0xD4,0x00,0xD4,0x00,0x95,0x00,0x95,0x00,0xD5,0x00,0xD5,0x00,0xD6,0x00,
  0xD6,0x00,0xD7,0x00,0xD7,0x00,0xD8,0x00,0x41,0x06,0x00,0x08,0x00,0x29,0x00,0x08,0x00,0xBA,0x00,0xD9,0x00,0x9D,0x00,0x42,0x24,0x00,0x06,0x00,0x29,0x00,0x06,0x00,
  0x18,0x00,0x18,0x00,0x17,0x00,0x17,0x00,0x33,0x00,0x33,0x00,0xD8,0x00,0xD8,0x00,0xDA,0x00,0xDA,0x00,0xDB,0x00,0xDB,0x00,0x91,0x00,0x91,0x00,0xDC,0x00,0x61,0x00,
  0xDC,0x00,0xDC,0x00,0x9E,0x00,0x9E,0x00,0xA6,0x00,0xA6,0x00,0x35,0x00,0x35,0x00,0xA7,0x00,0xA7,0x00,0xA8,0x00,0xA8,0x00,0x24,0x00,0x24,0x00,0x25,0x00,0x25,0x00,
  0x29,0x00,0x43,0x22,0x00,0xDD,0x00,0x16,0x00,0x16,0x00,0x17,0x00,0x17,0x00,0x94,0x00,0x94,0x00,0x7C,0x00,0x7C,0x00,0x67,0x00,0x67,0x00,0x5B,0x00,0x5B,0x00,0x5C,
  0x00,0x5C,0x00,0xDE,0x00,0xDE,0x00,0xCA,0x00,0xCA,0x00,0x79,0x00,0x79,0x00,0xDF,0x00,0xDF,0x00,0xE0,0x00,0xE0,0x00,0x80,0x00,0x80,0x00,0x96,0x00,0x96,0x00,0x24,
  0x00,0x24,0x00,0x23,0x00,0x23,0x00,0xE1,0x00,0x44,0x18,0x00,0x06,0x00,0x29,0x00,0x06,0x00,0xB6,0x00,0xB6,0x00,0x50,0x00,0x50,0x00,0x9A,0x00,0x9A,0x00,0xAD,0x00,
  0xAD,0x00,0xE2,0x00,0xE2,0x00,0x93,0x00,0x93,0x00,0xD4,0x00,0xD4,0x00,0x45,0x00,0x45,0x00,0x46,0x00,0x46,0x00,0x9B,0x00,0x9B,0x00,0x29,0x00,0x45,0x08,0x00,0x06,
  0x00,0x29,0x00,0x06,0x00,0x8E,0x00,0x61,0x00,0xE3,0x00,0x29,0x00,0x39,0x00,0x46,0x06,0x00,0x06,0x00,0x29,0x00,0x06,0x00,0x8E,0x00,0x61,0x00,0xE3,0x00,0x47,0x26,
  0x00,0xDD,0x00,0x16,0x00,0x16,0x00,0x17,0x00,0x17,0x00,0x94,0x00,0x94,0x00,0x7C,0x00,0x7C,0x00,0x67,0x00,0x67,0x00,0x5B,0x00,0x5B,0x00,0x5C,0x00,0x5C,0x00,0xDE,
  0x00,0xDE,0x00,0xCA,0x00,0xCA,0x00,0x79,0x00,0x79,0x00,0xDF,0x00,0xDF,0x00,0xE0,0x00,0xE0,0x00,0x80,0x00,0x80,0x00,0x96,0x00,0x96,0x00,0x24,0x00,0x24,0x00,0x23,
  0x00,0x23,0x00,0xE1,0x00,0xE1,0x00,0xE4,0x00,0xE5,0x00,0xE4,0x00,0x48,0x06,0x00,0x06,0x00,0x29,0x00,0xA1,0x00,0x8C,0x00,0x61,0x00,0xE6,0x00,0x49,0x02,0x00,0x06,
  0x00,0x29,0x00,0x4A,0x12,0x00,0x94,0x00,0xE7,0x00,0xE7,0x00,0xE8,0x00,0xE8,0x00,0xE9,0x00,0xE9,0x00,0x80,0x00,0x80,0x00,0x48,0x00,0x48,0x00,0x05,0x00,0x05,0x00,
  0x02,0x00,0x02,0x00,0xEA,0x00,0xEA,0x00,0x62,0x00,0x4B,0x06,0x00,0x06,0x00,0x29,0x00,0xA1,0x00,0x62,0x00,0xEB,0x00,0x8C,0x00,0x4C,0x04,0x00,0x06,0x00,0x29,0x00,
  0x29,0x00,0xEC,0x00,0x4D,0x08,0x00,0x06,0x00,0x29,0x00,0x06,0x00,0x47,0x00,0xED,0x00,0x47,0x00,0xED,0x00,0xBA,0x00,0x4E,0x06,0x00,0x06,0x00,0x29,0x00,0x06,0x00,
  0x8C,0x00,0xA1,0x00,0x8C,0x00,0x4F,0x28,0x00,0x7C,0x00,0x67,0x00,0x67,0x00,0x5B,0x00,0x5B,0x00,0x5C,0x00,0x5C,0x00,0xDE,0x00,0xDE,0x00,0xCA,0x00,0xCA,0x00,0x79,
  0x00,0x79,0x00,0xDF,0x00,0xDF,0x00,0xE0,0x00,0xE0,0x00,0x80,0x00,0x80,0x00,0x96,0x00,0x96,0x00,0x24,0x00,0x24,0x00,0x23,0x00,0x23,0x00,0xE1,0x00,0xE1,0x00,0xEE,
  0x00,0xEE,0x00,0xEF,0x00,0xEF,0x00,0xDD,0x00,0xDD,0x00,0x16,0x00,0x16,0x00,0x17,0x00,0x17,0x00,0x94,0x00,0x94,0x00,0x7C,0x00,0x50,0x14,0x00,0x06,0x00,0x29,0x00,
  0x06,0x00,0x18,0x00,0x18,0x00,0x17,0x00,0x17,0x00,0x33,0x00,0x33,0x00,0xD8,0x00,0xD8,0x00,0xF0,0x00,0xF0,0x00,0xF1,0x00,0xF1,0x00,0xB0,0x00,0xB0,0x00,0xF2,0x00,
  0xF2,0x00,0xF3,0x00,0x51,0x2A,0x00,0x7C,0x00,0x67,0x00,0x67,0x00,0x5B,0x00,0x5B,0x00,0x5C,0x00,0x5C,0x00,0xDE,0x00,0xDE,0x00,0xCA,0x00,0xCA,0x00,0x79,0x00,0x79,
  0x00,0xDF,0x00,0xDF,0x00,0xE0,0x00,0xE0,0x00,0x80,0x00,0x80,0x00,0x96,0x00,0x96,0x00,0x24,0x00,0x24,0x00,0x23,0x00,0x23,0x00,0xE1,0x00,0xE1,0x00,0xEE,0x00,0xEE,
  0x00,0xEF,0x00,0xEF,0x00,0xDD,0x00,0xDD,0x00,0x16,0x00,0x16,0x00,0x17,0x00,0x17,0x00,0x94,0x00,0x94,0x00,0x7C,0x00,0xF4,0x00,0xF5,0x00,0x52,0x16,0x00,0x06,0x00,
  0x29,0x00,0x06,0x00,0x18,0x00,0x18,0x00,0x17,0x00,0x17,0x00,0x33,0x00,0x33,0x00,0xD8,0x00,0xD8,0x00,0xDA,0x00,0xDA,0x00,0xDB,0x00,0xDB,0x00,0x91,0x00,0x91,0x00,
  0xDC,0x00,0xDC,0x00,0x61,0x00,0x69,0x00,0x8C,0x00,0x53,0x26,0x00,0x16,0x00,0x17,0x00,0x17,0x00,0x18,0x00,0x18,0x00,0x19,0x00,0x19,0x00,0x1A,0x00,0x1A,0x00,0x1B,
  0x00,0x1B,0x00,0x1C,0x00,0x1C,0x00,0x1D,0x00,0x1D,0x00,0x1E,0x00,0x1E,0x00,0x1F,0x00,0x1F,0x00,0x20,0x00,0x20,0x00,0x21,0x00,0x21,0x00,0x22,0x00,0x22,0x00,0x11,
  0x00,0x11,0x00,0x23,0x00,0x23,0x00,0x24,0x00,0x24,0x00,0x25,0x00,0x25,0x00,0x26,0x00,0x26,0x00,0x05,0x00,0x05,0x00,0x27,0x00,0x54,0x04,0x00,0xB6,0x00,0x9B,0x00,
  0x06,0x00,0xA1,0x00,0x55,0x12,0x00,0x06,0x00,0x10,0x00,0x10,0x00,0x78,0x00,0x78,0x00,0x7F,0x00,0x7F,0x00,0x80,0x00,0x80,0x00,0x47,0x00,0x47,0x00,0x81,0x00,0x81,
  0x00,0x56,0x00,0x56,0x00,0x11,0x00,0x11,0x00,0xA1,0x00,0x56,0x04,0x00,0x06,0x00,0x47,0x00,0xED,0x00,0x47,0x00,0x57,0x08,0x00,0x06,0x00,0x26,0x00,0x94,0x00,0x26,
  0x00,0x94,0x00,0x3A,0x00,0xF6,0x00,0x3A,0x00,0x58,0x04,0x00,0x06,0x00,0x8C,0x00,0xA1,0x00,0x29,0x00,0x59,0x06,0x00,0x06,0x00,0xE3,0x00,0xE3,0x00,0x47,0x00,0xED,
  0x00,0xE3,0x00,0x5A,0x06,0x00,0xA1,0x00,0x29,0x00,0x06,0x00,0xA1,0x00,0x29,0x00,0x8C,0x00,0x5B,0x08,0x00,0x65,0x00,0x6E,0x00,0xF7,0x00,0x0B,0x00,0x65,0x00,0x5E,
  0x00,0x6E,0x00,0x0D,0x00,0x5C,0x02,0x00,0x06,0x00,0xF8,0x00,0x5D,0x08,0x00,0xF9,0x00,0xFA,0x00,0x5E,0x00,0x0D,0x00,0x65,0x00,0x5E,0x00,0x6E,0x00,0x0D,0x00,0x5E,
  0x04,0x00,0xFB,0x00,0x75,0x00,0xFB,0x00,0xBD,0x00,0x5F,0x02,0x00,0x6E,0x00,0xFC,0x00,0x60,0x0C,0x00,0xFD,0x00,0x07,0x00,0x07,0x00,0x71,0x00,0x71,0x00,0xB5,0x00,
  0xB5,0x00,0xFE,0x00,0xFE,0x00,0xFF,0x00,0xFF,0x00,0x71,0x00,0x61,0x1C,0x00,0xAC,0x00,0xEC,0x00,0xB0,0x00,0x90,0x00,0x90,0x00,0x09,0x00,0x09,0x00,0x98,0x00,0x98,
  0x00,0x00,0x01,0x00,0x01,0xB5,0x00,0xB5,0x00,0xCA,0x00,0xCA,0x00,0x10,0x00,0x10,0x00,0x78,0x00,0x78,0x00,0x7F,0x00,0x7F,0x00,0x26,0x00,0x26,0x00,0x47,0x00,0x47,
  0x00,0x46,0x00,0x46,0x00,0x45,0x00,0x62,0x1C,0x00,0x06,0x00,0x29,0x00,0x61,0x00,0x1E,0x00,0x1E,0x00,0x2D,0x00,0x2D,0x00,0x01,0x01,0x01,0x01,0x4C,0x00,0x4C,0x00,
  0xA5,0x00,0xA5,0x00,0x02,0x01,0x02,0x01,0x36,0x00,0x36,0x00,0x03,0x01,0x03,0x01,0x04,0x01,0x04,0x01,0x9B,0x00,0x9B,0x00,0x48,0x00,0x48,0x00,0x05,0x00,0x05,0x00,
  0x27,0x00,0x63,0x1A,0x00,0xB0,0x00,0x90,0x00,0x90,0x00,0x09,0x00,0x09,0x00,0x98,0x00,0x98,0x00,0x00,0x01,0x00,0x01,0xB5,0x00,0xB5,0x00,0xCA,0x00,0xCA,0x00,0x10,
  0x00,0x10,0x00,0x78,0x00,0x78,0x00,0x7F,0x00,0x7F,0x00,0x26,0x00,0x26,0x00,0x47,0x00,0x47,0x00,0x46,0x00,0x46,0x00,0x45,0x00,0x64,0x1C,0x00,0x97,0x00,0xEC,0x00,
  0xB0,0x00,0x90,0x00,0x90,0x00,0x09,0x00,0x09,0x00,0x98,0x00,0x98,0x00,0x00,0x01,0x00,0x01,0xB5,0x00,0xB5,0x00,0xCA,0x00,0xCA,0x00,0x10,0x00,0x10,0x00,0x78,0x00,
  0x78,0x00,0x7F,0x00,0x7F,0x00,0x26,0x00,0x26,0x00,0x47,0x00,0x47,0x00,0x46,0x00,0x46,0x00,0x45,0x00,0x65,0x20,0x00,0xCA,0x00,0x02,0x01,0x02,0x01,0x9E,0x00,0x9E,
  0x00,0x05,0x01,0x05,0x01,0x90,0x00,0x90,0x00,0x09,0x00,0x09,0x00,0x98,0x00,0x98,0x00,0x00,0x01,0x00,0x01,0xB5,0x00,0xB5,0x00,0xCA,0x00,0xCA,0x00,0x10,0x00,0x10,
  0x00,0x78,0x00,0x78,0x00,0x7F,0x00,0x7F,0x00,0x26,0x00,0x26,0x00,0x47,0x00,0x47,0x00,0x46,0x00,0x46,0x00,0x45,0x00,0x66,0x0A,0x00,0x08,0x00,0x7C,0x00,0x7C,0x00,
  0x67,0x00,0x67,0x00,0x06,0x01,0x06,0x01,0x07,0x01,0x07,0x00,0x01,0x01,0x67,0x26,0x00,0xAC,0x00,0x08,0x01,0x08,0x01,0x09,0x01,0x09,0x01,0x0A,0x01,0x0A,0x01,0x0B,
  0x01,0x0B,0x01,0x0C,0x01,0x0C,0x01,0x0D,0x01,0xB0,0x00,0x90,0x00,0x90,0x00,0x09,0x00,0x09,0x00,0x98,0x00,0x98,0x00,0x00,0x01,0x00,0x01,0xB5,0x00,0xB5,0x00,0xCA,
  0x00,0xCA,0x00,0x10,0x00,0x10,0x00,0x78,0x00,0x78,0x00,0x7F,0x00,0x7F,0x00,0x26,0x00,0x26,0x00,0x47,0x00,0x47,0x00,0x46,0x00,0x46,0x00,0x45,0x00,0x68,0x0E,0x00,
  0x06,0x00,0x29,0x00,0xF3,0x00,0x00,0x01,0x00,0x01,0x98,0x00,0x98,0x00,0x09,0x00,0x09,0x00,0x90,0x00,0x90,0x00,0x0E,0x01,0x0E,0x01,0x0F,0x01,0x69,0x0A,0x00,0x06,
  0x00,0x2F,0x00,0x2F,0x00,0x8D,0x00,0x8D,0x00,0x10,0x01,0x10,0x01,0x06,0x00,0x1D,0x00,0x04,0x00,0x6A,0x10,0x00,0xBE,0x00,0x11,0x01,0x11,0x01,0x7C,0x00,0x7C,0x00,
  0x12,0x01,0x12,0x01,0xBE,0x00,0x98,0x00,0x13,0x01,0x13,0x01,0x14,0x01,0x14,0x01,0x15,0x01,0x15,0x01,0x6E,0x00,0x6B,0x06,0x00,0x06,0x00,0x29,0x00,0x4D,0x00,0x49,
  0x00,0x16,0x01,0x0F,0x01,0x6C,0x02,0x00,0x06,0x00,0x29,0x00,0x6D,0x1A,0x00,0x07,0x00,0x29,0x00,0xF3,0x00,0x00,0x01,0x00,0x01,0x98,0x00,0x98,0x00,0x09,0x00,0x09,
  0x00,0x90,0x00,0x90,0x00,0x0E,0x01,0x0E,0x01,0x0F,0x01,0x0E,0x01,0xE2,0x00,0xE2,0x00,0x17,0x01,0x17,0x01,0x41,0x00,0x41,0x00,0x18,0x01,0x18,0x01,0x19,0x01,0x19,
  0x01,0x1A,0x01,0x6E,0x0E,0x00,0x07,0x00,0x29,0x00,0xF3,0x00,0x00,0x01,0x00,0x01,0x98,0x00,0x98,0x00,0x09,0x00,0x09,0x00,0x90,0x00,0x90,0x00,0x0E,0x01,0x0E,0x01,
  0x0F,0x01,0x6F,0x20,0x00,0x98,0x00,0x00,0x01,0x00,0x01,0xB5,0x00,0xB5,0x00,0xCA,0x00,0xCA,0x00,0x10,0x00,0x10,0x00,0x78,0x00,0x78,0x00,0x7F,0x00,0x7F,0x00,0x26,
  0x00,0x26,0x00,0x47,0x00,0x47,0x00,0x46,0x00,0x46,0x00,0x45,0x00,0x45,0x00,0x9C,0x00,0x9C,0x00,0x22,0x00,0x22,0x00,0xB0,0x00,0xB0,0x00,0x90,0x00,0x90,0x00,0x09,
  0x00,0x09,0x00,0x98,0x00,0x70,0x1C,0x00,0x07,0x00,0x6E,0x00,0x61,0x00,0x1E,0x00,0x1E,0x00,0x2D,0x00,0x2D,0x00,0x01,0x01,0x01,0x01,0x4C,0x00,0x4C,0x00,0xA5,0x00,
  0xA5,0x00,0x02,0x01,0x02,0x01,0x36,0x00,0x36,0x00,0x03,0x01,0x03,0x01,0x04,0x01,0x04,0x01,0x9B,0x00,0x9B,0x00,0x48,0x00,0x48,0x00,0x05,0x00,0x05,0x00,0x27,0x00,
  0x71,0x1C,0x00,0xAC,0x00,0x1B,0x01,0xB0,0x00,0x90,0x00,0x90,0x00,0x09,0x00,0x09,0x00,0x98,0x00,0x98,0x00,0x00,0x01,0x00,0x01,0xB5,0x00,0xB5,0x00,0xCA,0x00,0xCA,
  0x00,0x10,0x00,0x10,0x00,0x78,0x00,0x78,0x00,0x7F,0x00,0x7F,0x00,0x26,0x00,0x26,0x00,0x47,0x00,0x47,0x00,0x46,0x00,0x46,0x00,0x45,0x00,0x72,0x0A,0x00,0x07,0x00,
  0x29,0x00,0xCA,0x00,0xB5,0x00,0xB5,0x00,0x00,0x01,0x00,0x01,0x98,0x00,0x98,0x00,0x09,0x00,0x73,0x20,0x00,0xA5,0x00,0x90,0x00,0x90,0x00,0x01,0x01,0x01,0x01,0x2D,
  0x00,0x2D,0x00,0xFF,0x00,0xFF,0x00,0x61,0x00,0x61,0x00,0xA9,0x00,0xA9,0x00,0x1C,0x01,0x1C,0x01,0x1D,0x01,0x1D,0x01,0x72,0x00,0x72,0x00,0x37,0x00,0x37,0x00,0x03,
  0x01,0x03,0x01,0x46,0x00,0x46,0x00,0x9B,0x00,0x9B,0x00,0x48,0x00,0x48,0x00,0x1E,0x01,0x1E,0x01,0x27,0x00,0x74,0x0A,0x00,0x30,0x00,0x1F,0x01,0x1F,0x01,0xE0,0x00,
  0xE0,0x00,0x80,0x00,0x80,0x00,0x47,0x00,0x07,0x00,0x01,0x01,0x75,0x0E,0x00,0x07,0x00,0x49,0x00,0x49,0x00,0x1E,0x01,0x1E,0x01,0x07,0x01,0x07,0x01,0x80,0x00,0x80,
  0x00,0xE9,0x00,0xE9,0x00,0x37,0x00,0x20,0x01,0x0F,0x01,0x76,0x04,0x00,0x07,0x00,0x80,0x00,0xAC,0x00,0x80,0x00,0x77,0x08,0x00,0x07,0x00,0x48,0x00,0x09,0x00,0x48,
  0x00,0x09,0x00,0xEC,0x00,0x17,0x01,0xEC,0x00,0x78,0x04,0x00,0x07,0x00,0x0F,0x01,0x20,0x01,0x29,0x00,0x79,0x0C,0x00,0x1D,0x00,0x9B,0x00,0xAF,0x00,0x9B,0x00,0x9B,
  0x00,0x13,0x00,0x13,0x00,0x0D,0x01,0x0D,0x01,0x0B,0x00,0x0B,0x00,0x6E,0x00,0x7A,0x06,0x00,0x20,0x01,0x29,0x00,0x07,0x00,0x20,0x01,0x29,0x00,0x0F,0x01,0x7B,0x34,
  0x00,0x12,0x00,0x21,0x01,0x21,0x01,0x66,0x00,0x66,0x00,0x00,0x00,0x00,0x00,0x59,0x00,0x59,0x00,0x22,0x01,0x22,0x01,0x23,0x01,0x23,0x01,0x2D,0x00,0x2D,0x00,0x1F,
  0x00,0x1F,0x00,0x24,0x01,0x21,0x01,0x25,0x01,0x25,0x01,0x1A,0x00,0x1A,0x00,0x26,0x01,0x26,0x01,0x27,0x01,0x27,0x01,0x6F,0x00,0x6F,0x00,0x28,0x01,0x28,0x01,0x29,
  0x01,0x29,0x01,0x75,0x00,0x75,0x00,0x2A,0x01,0x2A,0x01,0x2B,0x01,0x2B,0x01,0x70,0x00,0x70,0x00,0xE0,0x00,0xE0,0x00,0x07,0x01,0x07,0x01,0x2C,0x01,0x2C,0x01,0x2D,
  0x01,0x2D,0x01,0x0D,0x01,0x2E,0x01,0x2F,0x01,0x7C,0x02,0x00,0x65,0x00,0x6E,0x00,0x7D,0x34,0x00,0x65,0x00,0x30,0x01,0x30,0x01,0x31,0x01,0x31,0x01,0xBE,0x00,0xBE,
  0x00,0x32,0x01,0x32,0x01,0x06,0x01,0x06,0x01,0xFD,0x00,0xFD,0x00,0x1D,0x00,0x1D,0x00,0x0E,0x00,0x0E,0x00,0x33,0x01,0x30,0x01,0x34,0x01,0x34,0x01,0x60,0x00,0x60,
  0x00,0x5B,0x00,0x5B,0x00,0x7D,0x00,0x7D,0x00,0x5D,0x00,0x5D,0x00,0xDE,0x00,0xDE,0x00,0xB5,0x00,0xB5,0x00,0x35,0x01,0x35,0x01,0x01,0x00,0x01,0x00,0xEA,0x00,0xEA,
  0x00,0x27,0x00,0x27,0x00,0x1E,0x01,0x1E,0x01,0x36,0x01,0x36,0x01,0x63,0x00,0x63,0x00,0x37,0x01,0x37,0x01,0x38,0x01,0x1C,0x01,0xCB,0x00,0x7E,0x28,0x00,0x10,0x00,
  0xCA,0x00,0xCA,0x00,0xB5,0x00,0xB5,0x00,0x9F,0x00,0x9F,0x00,0xEB,0x00,0xEB,0x00,0x69,0x00,0x69,0x00,0x39,0x01,0x39,0x01,0x9D,0x00,0x9D,0x00,0x95,0x00,0x95,0x00,
  0x3A,0x01,0x3A,0x01,0x3B,0x01,0xCA,0x00,0xA0,0x00,0xA0,0x00,0xAA,0x00,0xAA,0x00,0x3C,0x01,0x3C,0x01,0x3D,0x01,0x3D,0x01,0x3E,0x01,0x3E,0x01,0x9C,0x00,0x9C,0x00,
  0x3F,0x01,0x3F,0x01,0x40,0x01,0x40,0x01,0x3B,0x01,0x3B,0x01,0xBB,0x00
};

static inline NxF32 Dot(const NxF32 *A,const NxF32 *B)
{
	return A[0]*B[0] + A[1]*B[1] + A[2]*B[2];
}

static inline void Cross(NxF32 *result,const NxF32 *A,const NxF32 *B)
{
	result[0] = A[1]*B[2] - A[2]*B[1];
	result[1] = A[2]*B[0] - A[0]*B[2];
	result[2] = A[0]*B[1] - A[1]*B[0];
}

static inline NxF32 Normalize(NxF32 *r)
{
	NxF32 d = sqrtf(r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);
	NxF32 recip = 1.0f / d;
	r[0]*=recip;
	r[1]*=recip;
	r[2]*=recip;
	return d;
}

static inline NxF32 Magnitude(const NxF32 *v)
{
	return sqrtf( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
}

void computeLookAt(const NxF32 *eye,const NxF32 *look,const NxF32 *upVector,NxF32 *matrix)
{

	NxF32 row2[3];

	row2[0] = look[0] - eye[0];
	row2[1] = look[1] - eye[1];
	row2[2] = look[2] - eye[2];

	Normalize(row2);

	NxF32 row0[3];
	NxF32	row1[3];

	Cross(row0, upVector, row2 );
	Cross(row1, row2, row0 );

	Normalize(row0);
	Normalize(row1);

	matrix[0] = row0[0];
	matrix[1] = row0[1];
	matrix[2] = row0[2];
	matrix[3] = 0;

	matrix[4+0] = row1[0];
	matrix[4+1] = row1[1];
	matrix[4+2] = row1[2];
	matrix[4+3] = 0;

	matrix[8+0] = row2[0];
	matrix[8+1] = row2[1];
	matrix[8+2] = row2[2];
	matrix[8+3] = 0;

	matrix[12+0] = eye[0];
	matrix[12+1] = eye[1];
	matrix[12+2] = eye[2];
	matrix[12+3] = 1;

}


static inline const NxU8 * getUShort(const NxU8 *data,unsigned short &v)
{
    const unsigned short *src = (const unsigned short *)data;
    v = src[0];
    data+=sizeof(unsigned short);
	return data;
}

static inline const NxU8 * getUint(const NxU8 *data,NxU32 &v)
{
    const NxU32 *src = (const NxU32 *)data;
    v = src[0];
    data+=sizeof(NxU32);
	return data;
}

class FontChar
{
public:
  FontChar(void)
  {
    mIndexCount = 0;
    mIndices   = 0;
    mX1 = mY1 = mX2 = mY2 = 0;
  }

  const NxU8 * init(const NxU8 *data,const NxF32 *vertices)
  {
    data = getUShort(data,mIndexCount);
    mIndices = (const unsigned short *)data;
    data+=(mIndexCount*sizeof(unsigned short));
    for (NxU32 i=0; i<mIndexCount; i++)
    {
        NxU32 index = mIndices[i];
        const NxF32 *vertex = &vertices[index*2];
		assert( _finite(vertex[0]));
		assert( _finite(vertex[1]));
        if ( i == 0 )
        {
            mX1 = mX2 = vertex[0];
            mY1 = mY2 = vertex[1];
        }
        else
        {
            if ( vertex[0] < mX1 ) mX1 = vertex[0];
            if ( vertex[1] < mY1 ) mY1 = vertex[1];

            if ( vertex[0] > mX2 ) mX2 = vertex[0];
            if ( vertex[1] > mY2 ) mY2 = vertex[1];

        }
    }

	assert( _finite(mX1));
	assert( _finite(mX2));
	assert( _finite(mY1));
	assert( _finite(mY2));

    return data;
  }

  void vputc(const NxF32 *vertices,RenderDebug *debug,const NxMat34 &pose,NxF32 textScale,NxF32 &x,NxF32 &y)
  {
    if ( mIndices )
    {
        NxU32 lineCount = mIndexCount/2;
		NxF32 spacing = (mX2-mX1)+0.05f;
        for (NxU32 i=0; i<lineCount; i++)
        {
            unsigned short i1 = mIndices[i*2+0];
            unsigned short i2 = mIndices[i*2+1];

            const NxF32 *v1 = &vertices[i1*2];
            const NxF32 *v2 = &vertices[i2*2];

			NxVec3 p1(v1[0]+x,v1[1]+y,0);
			NxVec3 p2(v2[0]+x,v2[1]+y,0);
			p1*=textScale;
			p2*=textScale;
			pose.multiply(p1,p1);
			pose.multiply(p2,p2);

            debug->DebugLine(&p1.x,&p2.x);

        }
		x+=spacing;
    }
    else
    {
        x+=0.1f;
    }
  }

  NxF32 getWidth(void) const
  {
    NxF32 ret = 0.1f;
    if ( mIndexCount > 0 )
    {
        ret = (mX2-mX1)+0.05f;
    }
    return ret;
  }


  NxF32          mX1;
  NxF32          mX2;
  NxF32          mY1;
  NxF32          mY2;
  unsigned short mIndexCount;
  const unsigned short *mIndices;
};

class MyVectorFont 
{
public:
  MyVectorFont(void)
  {
    mVersion = 0;
    mVcount = 0;
    mCount = 0;
    mVertices = 0;
    initFont(g_font);
  }

  ~MyVectorFont(void)
  {
    release();
  }

  void release(void)
  {
    mVersion = 0;
    mVcount = 0;
    mCount = 0;
    mVertices = 0;
  }

  void initFont(const NxU8 *font)
  {
    release();
    if ( font[0] == 'F' && font[1] == 'O' && font[2] == 'N' && font[3] == 'T' )
    {
        font+=4;
        font = getUint(font,mVersion);
        if ( mVersion == FONT_VERSION )
        {
            font = getUint(font,mVcount);
            font = getUint(font,mCount);
            font = getUint(font,mIcount);
            NxU32 vsize = sizeof(NxF32)*mVcount*2;
            mVertices = (NxF32 *)font;
			font+=vsize;
            for (NxU32 i=0; i<mCount; i++)
            {
                NxU8 c = *font++;
                font = mCharacters[c].init(font,mVertices);
            }
        }
    }
  }

  virtual void vprintf(const char *buffer,RenderDebug *debug,const NxMat34 &pose,NxF32 textScale,bool centered)
  {
    const char *scan = buffer;
	NxF32 x = 0;
	NxF32 y = 0;
    if ( centered )
    {
        NxF32 wid=0;
        while ( *scan )
        {
			char c = *scan++;
            wid+=mCharacters[c].getWidth();
        }
        x = -wid*0.5f;
        scan = buffer;
    }
    while ( *scan )
    {
        char c = *scan++;
        mCharacters[c].vputc(mVertices,debug,pose,textScale,x,y);
    }
  }

private:
  NxU32    mVersion;
  NxU32    mVcount;
  NxU32    mCount;
  NxF32          *mVertices;
  NxU32    mIcount;
  FontChar        mCharacters[256];
};

enum ResourceType
{
    RENDER_SOLID_SHADED,
    RENDER_SOLID_SCREEN,
    RENDER_WIREFRAME,
    RENDER_SCREENSPACE,
};

#define RENDER_STATE_STACK_SIZE 64 // maximum size of the render state stack
#define MAX_RENDER_STATES 512 // don't ever expect more than 512 render state changes in a single frame.

class RenderState
{
public:

  RenderState(void)
  {
    mStates = 0;
    mColor = 0xFFFFFF;
    mDisplayTime = 0.0001f;
    mArrowColor = 0xFF0000;
    mArrowSize = 0.1f;
    mRenderScale = 1;
    mTextScale = 1;
  }

  RenderState(NxU32 s,NxU32 c,NxF32 d,NxU32 a,NxF32 as,NxF32 rs,NxF32 ts)
  {
    mStates = s;
    mColor = c;
    mDisplayTime = d;
    mArrowColor = a;
    mArrowSize = as;
    mRenderScale = rs;
    mTextScale = ts;
  }

  inline bool isScreen(void) const { return (mStates & DebugRenderState::ScreenSpace); };
  inline bool isUseZ(void) const { return !(mStates & DebugRenderState::NoZbuffer); };
  inline bool isSolid(void) const { return (mStates & (DebugRenderState::SolidShaded | DebugRenderState::SolidWireShaded)) ? true : false; };
  inline bool isClockwise(void) const { return !(mStates & DebugRenderState::CounterClockwise); };
  inline bool isWireframeOverlay(void) const { return (mStates & DebugRenderState::SolidWireShaded) ? true : false; };
  inline NxF32 getDisplayTime(void) const
  {
    return (mStates & DebugRenderState::InfiniteLifeSpan) ? NX_MAX_F32 : mDisplayTime;
  }

  inline bool isCentered(void) const { return (mStates & DebugRenderState::CenterText) ? true : false; };
  inline bool isCameraFacing(void) const { return (mStates & DebugRenderState::CameraFacing) ? true : false; };
  inline bool isCounterClockwise(void) const { return (mStates & DebugRenderState::CounterClockwise) ? true : false; };



  NxU32 mStates;
  NxU32 mColor;
  NxF32 mDisplayTime;
  NxU32 mArrowColor;
  NxF32 mArrowSize;
  NxF32 mRenderScale;
  NxF32 mTextScale;
};





const NxF32 FM_PI = 3.1415926535897932384626433832795028841971693993751f;
const NxF32 FM_DEG_TO_RAD = ((2.0f * FM_PI) / 360.0f);
const NxF32 FM_RAD_TO_DEG = (360.0f / (2.0f * FM_PI));

inline NxF32 degToRad(NxF32 deg) { return deg*FM_DEG_TO_RAD; };
inline NxF32 radToDeg(NxF32 rad) { return rad*FM_RAD_TO_DEG; };

inline void fm_cross(NxF32 *cross,const NxF32 *a,const NxF32 *b)
{
	cross[0] = a[1]*b[2] - a[2]*b[1];
	cross[1] = a[2]*b[0] - a[0]*b[2];
	cross[2] = a[0]*b[1] - a[1]*b[0];
}

inline NxF32 fm_distToPlane(const NxF32 *plane,const NxF32 *p) // computes the distance of this point from the plane.
{
	return p[0]*plane[0]+p[1]*plane[1]+p[2]*plane[2]+plane[3];
}

inline NxF32 fm_dot(const NxF32 *p1,const NxF32 *p2)
{
	return p1[0]*p2[0]+p1[1]*p2[1]+p1[2]*p2[2];
}

inline NxF32 fm_normalize(NxF32 *n) // normalize this vector
{
	NxF32 dist = (NxF32)sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
	if ( dist > 0.0000001f )
	{
		NxF32 mag = 1.0f / dist;
		n[0]*=mag;
		n[1]*=mag;
		n[2]*=mag;
	}
	else
	{
		n[0] = 1;
		n[1] = 0;
		n[2] = 0;
	}

	return dist;
}

inline void fm_quatToMatrix(const NxF32 *quat,NxF32 *matrix) // convert quaterinion rotation to matrix, zeros out the translation component.
{

	NxF32 xx = quat[0]*quat[0];
	NxF32 yy = quat[1]*quat[1];
	NxF32 zz = quat[2]*quat[2];
	NxF32 xy = quat[0]*quat[1];
	NxF32 xz = quat[0]*quat[2];
	NxF32 yz = quat[1]*quat[2];
	NxF32 wx = quat[3]*quat[0];
	NxF32 wy = quat[3]*quat[1];
	NxF32 wz = quat[3]*quat[2];

	matrix[0*4+0] = 1 - 2 * ( yy + zz );
	matrix[1*4+0] =     2 * ( xy - wz );
	matrix[2*4+0] =     2 * ( xz + wy );

	matrix[0*4+1] =     2 * ( xy + wz );
	matrix[1*4+1] = 1 - 2 * ( xx + zz );
	matrix[2*4+1] =     2 * ( yz - wx );

	matrix[0*4+2] =     2 * ( xz - wy );
	matrix[1*4+2] =     2 * ( yz + wx );
	matrix[2*4+2] = 1 - 2 * ( xx + yy );

	matrix[3*4+0] = matrix[3*4+1] = matrix[3*4+2] = (NxF32) 0.0f;
	matrix[0*4+3] = matrix[1*4+3] = matrix[2*4+3] = (NxF32) 0.0f;
	matrix[3*4+3] =(NxF32) 1.0f;

}


inline NxF32 fm_computePlane(const NxF32 *A,const NxF32 *B,const NxF32 *C,NxF32 *n) // returns D
{
	NxF32 vx = (B[0] - C[0]);
	NxF32 vy = (B[1] - C[1]);
	NxF32 vz = (B[2] - C[2]);

	NxF32 wx = (A[0] - B[0]);
	NxF32 wy = (A[1] - B[1]);
	NxF32 wz = (A[2] - B[2]);

	NxF32 vw_x = vy * wz - vz * wy;
	NxF32 vw_y = vz * wx - vx * wz;
	NxF32 vw_z = vx * wy - vy * wx;

	NxF32 mag = sqrt((vw_x * vw_x) + (vw_y * vw_y) + (vw_z * vw_z));

	if ( mag < 0.000001f )
	{
		mag = 0;
	}
	else
	{
		mag = 1.0f/mag;
	}

	NxF32 x = vw_x * mag;
	NxF32 y = vw_y * mag;
	NxF32 z = vw_z * mag;


	NxF32 D = 0.0f - ((x*A[0])+(y*A[1])+(z*A[2]));

	n[0] = x;
	n[1] = y;
	n[2] = z;

	return D;
}


inline void  fm_transform(const NxF32 matrix[16],const NxF32 v[3],NxF32 t[3]) // rotate and translate this point
{
	if ( matrix )
	{
		NxF32 tx = (matrix[0*4+0] * v[0]) +  (matrix[1*4+0] * v[1]) + (matrix[2*4+0] * v[2]) + matrix[3*4+0];
		NxF32 ty = (matrix[0*4+1] * v[0]) +  (matrix[1*4+1] * v[1]) + (matrix[2*4+1] * v[2]) + matrix[3*4+1];
		NxF32 tz = (matrix[0*4+2] * v[0]) +  (matrix[1*4+2] * v[1]) + (matrix[2*4+2] * v[2]) + matrix[3*4+2];
		t[0] = tx;
		t[1] = ty;
		t[2] = tz;
	}
	else
	{
		t[0] = v[0];
		t[1] = v[1];
		t[2] = v[2];
	}
}

inline void  fm_setTranslation(const NxF32 *translation,NxF32 *matrix)
{
	matrix[12] = translation[0];
	matrix[13] = translation[1];
	matrix[14] = translation[2];
}

// Reference, from Stan Melax in Game Gems I
//  Quaternion q;
//  vector3 c = CrossProduct(v0,v1);
//  NxF32   d = DotProduct(v0,v1);
//  NxF32   s = (NxF32)sqrt((1+d)*2);
//  q.x = c.x / s;
//  q.y = c.y / s;
//  q.z = c.z / s;
//  q.w = s /2.0f;
//  return q;
inline void fm_rotationArc(const NxF32 *v0,const NxF32 *v1,NxF32 *quat)
{
	NxF32 cross[3];

	fm_cross(cross,v0,v1);
	NxF32 d = fm_dot(v0,v1);
	NxF32 s = sqrt((1+d)*2);
	NxF32 recip = 1.0f / s;

	quat[0] = cross[0] * recip;
	quat[1] = cross[1] * recip;
	quat[2] = cross[2] * recip;
	quat[3] = s * 0.5f;

}

class MPoolExtra : public Memalloc
{
public:
	MPoolExtra(size_t mlen,const char *poolType,const char *file,NxI32 lineno)
	{
		mPoolType = poolType;
		mNext = 0;

		mData = (char *)MEMALLOC_MALLOC(mlen);

		memset(mData,0,mlen);
	}

	~MPoolExtra(void)
	{
		MEMALLOC_FREE(mData);
	}

	MPoolExtra *mNext;  // the 'next' block allocated.
	char      *mData;
	const char *mPoolType;
};


template <class Type > class Pool
{
public:
	Pool(void)
	{
		mPoolType  = "GENERIC-POOL";
		mHead      = 0;
		mTail      = 0;
		mFree      = 0;
		mData      = 0;
		mCurrent   = 0;
		mFreeCount = 0;
		mUsedCount = 0;
		mMaxUsed   = 0;
		mMaxItems  = 0;
		mGrowCount = 0;
		mStartCount = 0;
		mCurrentCount = 0;
		mInitialized = false;
	};

	~Pool(void)
	{
		Release();
	};


	void Release(void)
	{
		mHead = 0;
		mTail = 0;
		mFree = 0;

		// delete all of the memory blocks we allocated.
		MPoolExtra *extra = mData;
		while ( extra )
		{
			MPoolExtra *next = extra->mNext;
			delete extra;
			extra = next;
		}

		mData = 0;
		mCurrent = 0;
		mFreeCount = 0;
		mUsedCount = 0;
		mMaxUsed = 0;
		mInitialized = false;
	};

	void Set(NxI32 startcount,NxI32 growcount,NxI32 maxitems,const char *poolType,const char *file,NxI32 lineno)
	{
		mPoolType = poolType;
		mFile     = file;
		mLineNo   = lineno;

		Release();

		mMaxItems = maxitems;
		mGrowCount = growcount;
		mStartCount = startcount;
		mCurrentCount = startcount;

		if ( mStartCount > 0 )
		{
			mData =  MEMALLOC_NEW(MPoolExtra)(sizeof(Type)*mStartCount,mPoolType,mFile,mLineNo);
			Type *data = (Type *) mData->mData;
			{
				Type *t = (Type *)mData->mData;
				for (NxI32 i=0; i<mStartCount; i++)
				{
					new ( t ) Type;
					t++;
				}
			}
			mFree = data;
			mHead = 0;
			mTail = 0;
			NxI32 i;
			for (i=0; i<(startcount-1); i++)
			{
				data[i].SetNext( &data[i+1] );
				if ( i == 0 )
					data[i].SetPrevious( 0 );
				else
					data[i].SetPrevious( &data[i-1] );
			}

			data[i].SetNext(0);
			data[i].SetPrevious( &data[i-1] );
			mCurrent = 0;
			mFreeCount = startcount;
			mUsedCount = 0;
		}
		mInitialized = true;
	};


	Type * GetNext(bool &looped)
	{

		looped = false; // default value

		if ( !mHead ) return 0; //  there is no data to process.

		Type *ret;

		if ( !mCurrent )
		{
			ret = mHead;
			looped = true;
		}
		else
		{
			ret = mCurrent;
		}

		if ( ret ) mCurrent = ret->GetNext();


		return ret;
	};

	bool IsEmpty(void) const
	{
		if ( !mHead ) return true;
		return false;
	};

	NxI32 Begin(void)
	{
		mCurrent = mHead;
		return mUsedCount;
	};

	NxI32 GetUsedCount(void) const { return mUsedCount; };
	NxI32 GetFreeCount(void) const { return mFreeCount; };

	Type * GetNext(void)
	{
		if ( !mHead ) return 0; //  there is no data to process.

		Type *ret;

		if ( !mCurrent )
		{
			ret = mHead;
		}
		else
		{
			ret = mCurrent;
		}

		if ( ret ) mCurrent = ret->GetNext();


		return ret;
	};

	Type * Release(Type *t)
	{

		if ( t == mCurrent ) mCurrent = t->GetNext();

		if ( t == mTail )
		{
			mTail = t->GetPrevious(); // the new tail..
		}

		//  first patch old linked list.. his previous now points to his next
		Type *prev = t->GetPrevious();

		if ( prev )
		{
			Type *next = t->GetNext();
			prev->SetNext( next ); //  my previous now points to my next
			if ( next ) next->SetPrevious(prev);
			//  list is patched!
		}
		else
		{
			Type *next = t->GetNext();
			mHead = next;
			if ( mHead ) mHead->SetPrevious(0);
		}

		Type *temp = mFree; //  old head of free list.
		mFree = t; //  new head of linked list.
		t->SetPrevious(0);
		t->SetNext(temp);

		mUsedCount--;
		assert(mUsedCount >= 0);
		mFreeCount++;
		return mCurrent;
	};

	Type * GetFreeLink(void)
	{
		//  Free allocated items are always added to the head of the list
		if ( !mFree )
		{
			getMore();
		}

		Type *ret = mFree;

		if ( mFree )
		{
			mFree = ret->GetNext(); //  new head of free list.
			Type *temp = mHead; //  current head of list
			if ( mHead == 0 )  // if it's the first item then this is the head of the list.
			{
				mTail = ret;
				mHead = ret;        //  new head of list is this free one
				if ( temp ) temp->SetPrevious(ret);
				mHead->SetNext(temp);
				mHead->SetPrevious(0);
			}
			else
			{
				assert( mTail );
				assert( mTail->GetNext() == 0 );
				mTail->SetNext( ret );
				ret->SetPrevious( mTail );
				ret->SetNext(0);
				mTail = ret;
			}
			mUsedCount++;
			if ( mUsedCount > mMaxUsed ) mMaxUsed = mUsedCount;
			mFreeCount--;
		}
		return ret;
	};

	Type * getMore(void) // ok, we need to see if we can grow some more.
	{
		Type *ret = mFree;
		if ( ret == 0 && (mCurrentCount+mGrowCount) < mMaxItems && mGrowCount > 0 ) // ok..we are allowed to allocate some more...
		{
			MPoolExtra *pe = mData; // the old one...
			mData  = MEMALLOC_NEW(MPoolExtra)(sizeof(Type)*mGrowCount,mPoolType,mFile,mLineNo);
			{
				Type *t = (Type *)mData->mData;
				for (NxI32 i=0; i<mGrowCount; i++)
				{
					new ( t ) Type;
					t++;
				}
			}
			mData->mNext = pe; // he points to the old one.
			// done..memory allocated and added to singly linked list.

			Type *data = (Type *) mData->mData;
			mFree = data;     // new head of free list.
			NxI32 i;
			for (i=0; i<(mGrowCount-1); i++)
			{
				data[i].SetNext( &data[i+1] );
				if ( i == 0 )
					data[i].SetPrevious( 0 );
				else
					data[i].SetPrevious( &data[i-1] );
			}
			data[i].SetNext(0);
			data[i].SetPrevious( &data[i-1] );

			mFreeCount+=mGrowCount; // how many new free entries we have added...
			mCurrentCount+=mGrowCount;
		}
		return ret;
	}

	bool isInitialized(void) const { return mInitialized; };


//private:
	bool         mInitialized;
	NxI32        mMaxItems;
	NxI32        mGrowCount;
	NxI32        mStartCount;
	NxI32        mCurrentCount; // this is total allocated, not free/used

	Type       *mCurrent;
	MPoolExtra *mData;
	Type        *mHead;
	Type        *mTail;
	Type        *mFree;
	NxI32        mUsedCount;
	NxI32        mFreeCount;
	NxI32        mMaxUsed;
	const char *mPoolType;
	const char *mFile;
	NxI32         mLineNo;
};


const NxF32 debug_cylinder[32*9] =
{
0.0000f, 0.0000f, 0.0000f,    0.7071f, 0.7071f, 0.0000f,  1.0000f, 0.0000f, 0.0000f,
0.0000f, 0.0000f, 0.0000f,    0.0000f, 1.0000f, 0.0000f,  0.7071f, 0.7071f, 0.0000f,
0.0000f, 0.0000f, 0.0000f,    -0.7071f, 0.7071f, 0.0000f,  0.0000f, 1.0000f, 0.0000f,
0.0000f, 0.0000f, 0.0000f,    -1.0000f, 0.0000f, 0.0000f,  -0.7071f, 0.7071f, 0.0000f,
0.0000f, 0.0000f, 0.0000f,    -0.7071f, -0.7071f, 0.0000f,  -1.0000f, 0.0000f, 0.0000f,
0.0000f, 0.0000f, 0.0000f,    0.0000f, -1.0000f, 0.0000f,  -0.7071f, -0.7071f, 0.0000f,
0.0000f, 0.0000f, 0.0000f,    0.7071f, -0.7071f, 0.0000f,  0.0000f, -1.0000f, 0.0000f,
0.0000f, 0.0000f, 0.0000f,    1.0000f, 0.0000f, 0.0000f,  0.7071f, -0.7071f, 0.0000f,
1.0000f, 0.0000f, 0.0000f,    0.7071f, 0.7071f, 1.0000f,  1.0000f, 0.0000f, 1.0000f,
1.0000f, 0.0000f, 0.0000f,    0.7071f, 0.7071f, 0.0000f,  0.7071f, 0.7071f, 1.0000f,
0.7071f, 0.7071f, 0.0000f,    0.0000f, 1.0000f, 1.0000f,  0.7071f, 0.7071f, 1.0000f,
0.7071f, 0.7071f, 0.0000f,    0.0000f, 1.0000f, 0.0000f,  0.0000f, 1.0000f, 1.0000f,
0.0000f, 1.0000f, 0.0000f,    -0.7071f, 0.7071f, 1.0000f,  0.0000f, 1.0000f, 1.0000f,
0.0000f, 1.0000f, 0.0000f,    -0.7071f, 0.7071f, 0.0000f,  -0.7071f, 0.7071f, 1.0000f,
-0.7071f, 0.7071f, 0.0000f,    -1.0000f, 0.0000f, 1.0000f,  -0.7071f, 0.7071f, 1.0000f,
-0.7071f, 0.7071f, 0.0000f,    -1.0000f, 0.0000f, 0.0000f,  -1.0000f, 0.0000f, 1.0000f,
-1.0000f, 0.0000f, 0.0000f,    -0.7071f, -0.7071f, 1.0000f,  -1.0000f, 0.0000f, 1.0000f,
-1.0000f, 0.0000f, 0.0000f,    -0.7071f, -0.7071f, 0.0000f,  -0.7071f, -0.7071f, 1.0000f,
-0.7071f, -0.7071f, 0.0000f,    0.0000f, -1.0000f, 1.0000f,  -0.7071f, -0.7071f, 1.0000f,
-0.7071f, -0.7071f, 0.0000f,    0.0000f, -1.0000f, 0.0000f,  0.0000f, -1.0000f, 1.0000f,
0.0000f, -1.0000f, 0.0000f,    0.7071f, -0.7071f, 1.0000f,  0.0000f, -1.0000f, 1.0000f,
0.0000f, -1.0000f, 0.0000f,    0.7071f, -0.7071f, 0.0000f,  0.7071f, -0.7071f, 1.0000f,
 0.7071f, -0.7071f, 0.0000f,    1.0000f, 0.0000f, 1.0000f,  0.7071f, -0.7071f, 1.0000f,
0.7071f, -0.7071f, 0.0000f,    1.0000f, 0.0000f, 0.0000f,  1.0000f, 0.0000f, 1.0000f,
0.0000f, 0.0000f, 1.0000f,    1.0000f, 0.0000f, 1.0000f,  0.7071f, 0.7071f, 1.0000f,
0.0000f, 0.0000f, 1.0000f,    0.7071f, 0.7071f, 1.0000f,  0.0000f, 1.0000f, 1.0000f,
0.0000f, 0.0000f, 1.0000f,    0.0000f, 1.0000f, 1.0000f,  -0.7071f, 0.7071f, 1.0000f,
0.0000f, 0.0000f, 1.0000f,    -0.7071f, 0.7071f, 1.0000f,  -1.0000f, 0.0000f, 1.0000f,
0.0000f, 0.0000f, 1.0000f,    -1.0000f, 0.0000f, 1.0000f,  -0.7071f, -0.7071f, 1.0000f,
0.0000f, 0.0000f, 1.0000f,    -0.7071f, -0.7071f, 1.0000f,  0.0000f, -1.0000f, 1.0000f,
0.0000f, 0.0000f, 1.0000f,    0.0000f, -1.0000f, 1.0000f,  0.7071f, -0.7071f, 1.0000f,
0.0000f, 0.0000f, 1.0000f,    0.7071f, -0.7071f, 1.0000f,  1.0000f, 0.0000f, 1.0000f
};

const NxF32 debug_sphere[32*9] =
{
 0.0000f, 0.0000f, 1.0000f,    0.7071f, 0.0000f, 0.7071f,  0.0000f, 0.7071f, 0.7071f,
 0.7071f, 0.0000f, 0.7071f,    1.0000f, 0.0000f, 0.0000f,  0.7071f, 0.7071f, 0.0000f,
 0.7071f, 0.0000f, 0.7071f,    0.7071f, 0.7071f, 0.0000f,  0.0000f, 0.7071f, 0.7071f,
 0.0000f, 0.7071f, 0.7071f,    0.7071f, 0.7071f, 0.0000f,  0.0000f, 1.0000f, 0.0000f,
 0.0000f, 0.0000f, 1.0000f,    0.0000f, 0.7071f, 0.7071f,  -0.7071f, 0.0000f, 0.7071f,
 0.0000f, 0.7071f, 0.7071f,    0.0000f, 1.0000f, 0.0000f,  -0.7071f, 0.7071f, 0.0000f,
 0.0000f, 0.7071f, 0.7071f,    -0.7071f, 0.7071f, 0.0000f,  -0.7071f, 0.0000f, 0.7071f,
 -0.7071f, 0.0000f, 0.7071f,    -0.7071f, 0.7071f, 0.0000f,  -1.0000f, 0.0000f, 0.0000f,
 0.0000f, 0.0000f, 1.0000f,    -0.7071f, 0.0000f, 0.7071f,  0.0000f, -0.7071f, 0.7071f,
 -0.7071f, 0.0000f, 0.7071f,    -1.0000f, 0.0000f, 0.0000f,  -0.7071f, -0.7071f, 0.0000f,
 -0.7071f, 0.0000f, 0.7071f,    -0.7071f, -0.7071f, 0.0000f,  0.0000f, -0.7071f, 0.7071f,
 0.0000f, -0.7071f, 0.7071f,    -0.7071f, -0.7071f, 0.0000f,  0.0000f, -1.0000f, 0.0000f,
 0.0000f, 0.0000f, 1.0000f,    0.0000f, -0.7071f, 0.7071f,  0.7071f, 0.0000f, 0.7071f,
 0.0000f, -0.7071f, 0.7071f,    0.0000f, -1.0000f, 0.0000f,  0.7071f, -0.7071f, 0.0000f,
 0.0000f, -0.7071f, 0.7071f,    0.7071f, -0.7071f, 0.0000f,  0.7071f, 0.0000f, 0.7071f,
 0.7071f, 0.0000f, 0.7071f,    0.7071f, -0.7071f, 0.0000f,  1.0000f, 0.0000f, 0.0000f,
 0.0000f, 0.0000f, -1.0000f,    0.0000f, 0.7071f, -0.7071f,  0.7071f, 0.0000f, -0.7071f,
 0.0000f, 0.7071f, -0.7071f,    0.0000f, 1.0000f, 0.0000f,  0.7071f, 0.7071f, 0.0000f,
 0.0000f, 0.7071f, -0.7071f,    0.7071f, 0.7071f, 0.0000f,  0.7071f, 0.0000f, -0.7071f,
 0.7071f, 0.0000f, -0.7071f,    0.7071f, 0.7071f, 0.0000f,  1.0000f, 0.0000f, 0.0000f,
 0.0000f, 0.0000f, -1.0000f,    -0.7071f, 0.0000f, -0.7071f,  0.0000f, 0.7071f, -0.7071f,
 -0.7071f, 0.0000f, -0.7071f,    -1.0000f, 0.0000f, 0.0000f,  -0.7071f, 0.7071f, 0.0000f,
 -0.7071f, 0.0000f, -0.7071f,    -0.7071f, 0.7071f, 0.0000f,  0.0000f, 0.7071f, -0.7071f,
 0.0000f, 0.7071f, -0.7071f,    -0.7071f, 0.7071f, 0.0000f,  0.0000f, 1.0000f, 0.0000f,
 0.0000f, 0.0000f, -1.0000f,    0.0000f, -0.7071f, -0.7071f,  -0.7071f, 0.0000f, -0.7071f,
 0.0000f, -0.7071f, -0.7071f,    0.0000f, -1.0000f, 0.0000f,  -0.7071f, -0.7071f, 0.0000f,
 0.0000f, -0.7071f, -0.7071f,    -0.7071f, -0.7071f, 0.0000f,  -0.7071f, 0.0000f, -0.7071f,
 -0.7071f, 0.0000f, -0.7071f,    -0.7071f, -0.7071f, 0.0000f,  -1.0000f, 0.0000f, 0.0000f,
 0.0000f, 0.0000f, -1.0000f,    0.7071f, 0.0000f, -0.7071f,  0.0000f, -0.7071f, -0.7071f,
 0.7071f, 0.0000f, -0.7071f,    1.0000f, 0.0000f, 0.0000f,  0.7071f, -0.7071f, 0.0000f,
 0.7071f, 0.0000f, -0.7071f,    0.7071f, -0.7071f, 0.0000f,  0.0000f, -0.7071f, -0.7071f,
 0.0000f, -0.7071f, -0.7071f,    0.7071f, -0.7071f, 0.0000f,  0.0000f, -1.0000f, 0.0000f,
};

const NxF32 debug_halfsphere[16*9] =
{
 0.0000f, 0.0000f, 1.0000f,    0.7071f, 0.0000f, 0.7071f,  0.0000f, 0.7071f, 0.7071f,
 0.7071f, 0.0000f, 0.7071f,    1.0000f, 0.0000f, 0.0000f,  0.7071f, 0.7071f, 0.0000f,
 0.7071f, 0.0000f, 0.7071f,    0.7071f, 0.7071f, 0.0000f,  0.0000f, 0.7071f, 0.7071f,
 0.0000f, 0.7071f, 0.7071f,    0.7071f, 0.7071f, 0.0000f,  0.0000f, 1.0000f, 0.0000f,
 0.0000f, 0.0000f, 1.0000f,    0.0000f, 0.7071f, 0.7071f,  -0.7071f, 0.0000f, 0.7071f,
 0.0000f, 0.7071f, 0.7071f,    0.0000f, 1.0000f, 0.0000f,  -0.7071f, 0.7071f, 0.0000f,
 0.0000f, 0.7071f, 0.7071f,    -0.7071f, 0.7071f, 0.0000f,  -0.7071f, 0.0000f, 0.7071f,
 -0.7071f, 0.0000f, 0.7071f,    -0.7071f, 0.7071f, 0.0000f,  -1.0000f, 0.0000f, 0.0000f,
};

const NxF32 debug_point[3*6] =
{
	-1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
	 0.0f,-1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
	 0.0f, 0.0f,-1.0f,  0.0f, 0.0f, 1.0f
};

class BlockInfo;

class PrintText
{
public:
  PrintText(void)
  {
    mText = 0;
    mBlock = 0;

    mTransform.id();
	mLifeTime = 0.0f;
    mVisible = true;
  }

  PrintText(const RenderState &rs,const NxMat34 &xform,const char *text,BlockInfo *block)
  {
    mBlock = block;
    NxU32 slen = (NxU32)strlen(text);
    mText = (char *)MEMALLOC_MALLOC(slen+1);
    memcpy(mText,text,slen+1);
    mRenderState = rs;
    mTransform = xform;
    mLifeTime = rs.getDisplayTime();
    mVisible = true;
  }

  ~PrintText(void)
  {
    MEMALLOC_FREE(mText);
  }

  void setVisible(bool state) { mVisible = state; };

  bool process(NxF32 dtime,RenderDebug *debug,MyVectorFont &font,NxMat34 &pose,NxF32 textScale,bool centered)
  {
    if ( mVisible )
      font.vprintf(mText,debug,pose,textScale, centered );
    mLifeTime-=dtime;
    return mLifeTime <= 0;
  }

  PrintText * GetNext(void) const { return mNext; };
  PrintText * GetPrevious(void) const { return mPrevious; };
  void        SetNext(PrintText *p) { mNext = p; };
  void        SetPrevious(PrintText *p) { mPrevious = p; };

  bool        mVisible;
  NxF32       mLifeTime;
  BlockInfo  *mBlock;
  RenderState mRenderState;
  NxMat34     mTransform;
  char       *mText;
private:
  PrintText  *mNext;
  PrintText  *mPrevious;

};

class SolidTri
{
public:
	SolidTri(void)
	{
		mVisible = true;
		mNext     = 0;
		mPrevious = 0;
		mBlock = 0;
	}

	void Set(const NxF32 *p1,
			 const NxF32 *p2,
			 const NxF32 *p3,
			 NxU32 color,
			 NxF32 lifespan,
			 NxF32 renderScale,
			 NxI32 blockIndex,
             bool isScreenSpace)
	{
		mVisible = true;
        mIsScreenSpace = isScreenSpace;

		mBlock = blockIndex;
		mP1.mPos[0] = p1[0]*renderScale;
		mP1.mPos[1] = p1[1]*renderScale;
		mP1.mPos[2] = p1[2]*renderScale;

		mP2.mPos[0] = p2[0]*renderScale;
		mP2.mPos[1] = p2[1]*renderScale;
		mP2.mPos[2] = p2[2]*renderScale;

		if ( p3 )
		{
			mP3.mPos[0] = p3[0]*renderScale;
			mP3.mPos[1] = p3[1]*renderScale;
			mP3.mPos[2] = p3[2]*renderScale;
		}

		mP1.mColor = color;
		mP2.mColor = color;
		mP3.mColor = color;

		mLifeSpan = lifespan;

		fm_computePlane(mP3.mPos,mP2.mPos,mP1.mPos,mN1);

		mN2[0] = mN1[0];
		mN2[1] = mN1[1];
		mN2[2] = mN1[2];

		mN3[0] = mN1[0];
		mN3[1] = mN1[1];
		mN3[2] = mN1[2];

	}


	void Set(const NxF32 *p1,
					 const NxF32 *p2,
					 const NxF32 *p3,
					 const NxF32 *n1,
					 const NxF32 *n2,
					 const NxF32 *n3,
					 NxU32 color,
					 NxF32 lifespan,
					 NxF32 renderScale,
					 NxI32 blockIndex,
                     bool screenSpace)
	{
		mVisible = true;
        mIsScreenSpace = screenSpace;
		mBlock = blockIndex;
		mP1.mPos[0] = p1[0]*renderScale;
		mP1.mPos[1] = p1[1]*renderScale;
		mP1.mPos[2] = p1[2]*renderScale;

		mP2.mPos[0] = p2[0]*renderScale;
		mP2.mPos[1] = p2[1]*renderScale;
		mP2.mPos[2] = p2[2]*renderScale;

		mP3.mPos[0] = p3[0]*renderScale;
		mP3.mPos[1] = p3[1]*renderScale;
		mP3.mPos[2] = p3[2]*renderScale;

		mN1[0] = n1[0];
		mN1[1] = n1[1];
		mN1[2] = n1[2];

		mN2[0] = n2[0];
		mN2[1] = n2[1];
		mN2[2] = n2[2];

		mN3[0] = n3[0];
		mN3[1] = n3[1];
		mN3[2] = n3[2];


		mP1.mColor = color;
		mP2.mColor = color;
		mP3.mColor = color;

		mLifeSpan = lifespan;

	}


	inline RenderDebugSolidVertex * RenderSolid(NxF32 dtime,RenderDebugSolidVertex *current,bool &remove)
	{
		if (!mVisible) return current;

		current[0].mPos[0]   = mP1.mPos[0];
		current[0].mPos[1]   = mP1.mPos[1];
		current[0].mPos[2]   = mP1.mPos[2];

		current[0].mColor    = mP1.mColor;
		current[0].mNormal[0] = mN1[0];
		current[0].mNormal[1] = mN1[1];
		current[0].mNormal[2] = mN1[2];

		current[1].mPos[0]   = mP2.mPos[0];
		current[1].mPos[1]   = mP2.mPos[1];
		current[1].mPos[2]   = mP2.mPos[2];
		current[1].mColor    = mP2.mColor;
		current[1].mNormal[0] = mN2[0];
		current[1].mNormal[1] = mN2[1];
		current[1].mNormal[2] = mN2[2];

		current[2].mPos[0]   = mP3.mPos[0];
		current[2].mPos[1]   = mP3.mPos[1];
		current[2].mPos[2]   = mP3.mPos[2];
		current[2].mColor    = mP3.mColor;
		current[2].mNormal[0] = mN3[0];
		current[2].mNormal[1] = mN3[1];
		current[2].mNormal[2] = mN3[2];

		current+=3;

		mLifeSpan-=dtime;
		if ( mLifeSpan < 0 )
		{
			remove = true;
		}
		else
		{
			remove = false;
		}

		return current;

	}

	inline NxI32 getBlock(void) const { return mBlock; };

	inline void SetNext(SolidTri *tri) { mNext = tri; };
	inline void SetPrevious(SolidTri *tri) { mPrevious = tri; };

	inline SolidTri * GetNext(void) { return mNext; };
	inline SolidTri * GetPrevious(void) { return mPrevious; };

	inline void transform(const NxMat34 &mat,NxF32 *pos)
	{
		NxVec3 *p = (NxVec3 *)pos;
		NxVec3 *t = (NxVec3 *)pos;
		mat.multiply(*p,*t);
	}

	inline void rtransform(const NxMat34 &mat,NxF32 *pos)
	{
		NxVec3 *p = (NxVec3 *)pos;
		NxVec3 *t = (NxVec3 *)pos;
		mat.M.multiply(*p,*t);
	}


	inline void transform(const NxMat34 &mat)
	{
		transform(mat,mP1.mPos);
		transform(mat,mP2.mPos);
		transform(mat,mP3.mPos);
        rtransform(mat,mN1);
        rtransform(mat,mN2);
        rtransform(mat,mN3);
	}

	void setVisible(bool state)
	{
		mVisible = state;
	}

    inline bool isScreenSpace(void) const { return mIsScreenSpace; };

//private:
	SolidTri        *mNext;
	SolidTri        *mPrevious;
	RenderDebugVertex     mP1;
	RenderDebugVertex     mP2;
	RenderDebugVertex     mP3;
	NxF32           mLifeSpan;
	NxF32           mN1[3];
	NxF32           mN2[3];
	NxF32           mN3[3];
	NxI32           mBlock;
	bool            mVisible:1;
    bool            mIsScreenSpace:1;
};

enum LineTriFlag
{
	LTF_NONE = 0,
	LTF_USEZ = (1<<0),
	LTF_LINE = (1<<1),
	LTF_HIDDEN = (1<<2),
    LTF_SCREEN = (1<<3), // going to the screen
};

class LineTri
{
public:
	LineTri(void)
	{
		mNext     = 0;
		mPrevious = 0;
		mFlags    = LTF_USEZ;
		mBlock = 0;
	};

	inline void setVisible(bool state)
	{
		if ( state )
			mFlags&=~LTF_HIDDEN;
		else
			mFlags|=LTF_HIDDEN;
	}

	inline bool useZ(void) const
	{
		return mFlags & LTF_USEZ ? true : false;
	};

	inline bool isScreen(void) const
	{
		return (mFlags & LTF_SCREEN) ? true : false;
	};

	inline void Set(const NxF32 *p1,
					 const NxF32 *p2,
					 const NxF32 *p3,
					 NxU32 color,
					 NxF32 lifespan,
					 LineTriFlag flag,
					 NxF32 renderScale,
					 NxI32 blockIndex)
	{
		mBlock = blockIndex;
		mFlags = flag;
		mP1.mPos[0] = p1[0]*renderScale;
		mP1.mPos[1] = p1[1]*renderScale;
		mP1.mPos[2] = p1[2]*renderScale;

		mP2.mPos[0] = p2[0]*renderScale;
		mP2.mPos[1] = p2[1]*renderScale;
		mP2.mPos[2] = p2[2]*renderScale;

		if ( p3 )
		{
			mP3.mPos[0] = p3[0]*renderScale;
			mP3.mPos[1] = p3[1]*renderScale;
			mP3.mPos[2] = p3[2]*renderScale;
		}
		else
		{
			mFlags|=LTF_LINE; // if only two data points, it is a line segment and not a triangle.
		}

		mP1.mColor = color;
		mP2.mColor = color;
		mP3.mColor = color;

		mLifeSpan = lifespan;

	};


	inline RenderDebugVertex * RenderLine(NxF32 dtime,RenderDebugVertex *current,bool &remove,bool flush)
	{
		if ( mFlags & LTF_HIDDEN ) return current;

		current[0] = mP1;
		current[1] = mP2;

		if ( mFlags & LTF_LINE )
		{
			current+=2;
		}
		else
		{
			current[2] = mP2;
			current[3] = mP3;
			current[4] = mP3;
			current[5] = mP1;
			current+=6;
		}

		if ( flush )
		{
			mLifeSpan-=dtime;
			if ( mLifeSpan <= 0 )
			{
				remove = true;
			}
			else
			{
				remove = false;
			}
		}

		return current;

	}

	NxI32 getBlock(void) const { return mBlock; };

	inline void SetNext(LineTri *tri) { mNext = tri; };
	inline void SetPrevious(LineTri *tri) { mPrevious = tri; };

	LineTri * GetNext(void) { return mNext; };
	LineTri * GetPrevious(void) { return mPrevious; };

	void transform(const NxMat34 &mat,NxF32 *pos)
	{
		NxVec3 *p = (NxVec3 *)pos;
		NxVec3 *t = (NxVec3 *)pos;
		mat.multiply(*p,*t);
	}

	void transform(const NxMat34 &mat)
	{
		transform(mat,mP1.mPos);
		transform(mat,mP2.mPos);
		transform(mat,mP3.mPos);
	}


//private:
	LineTri              *mNext;
	LineTri              *mPrevious;
	RenderDebugVertex     mP1;
	RenderDebugVertex     mP2;
	RenderDebugVertex     mP3;
	NxF32                 mLifeSpan;
	NxU32                 mFlags;
	NxI32                 mBlock;
};

class BlockInfo : public NVSHARE::Memalloc
{
public:
  BlockInfo(void)
  {
    mPrintText = 0;
    mSolidTri = 0;
    mLineTri = 0;
	mHashValue = 0;
	mVisibleState = true;
	mScreenSpace = false;
  }
  NxU32 getHashValue(void) const
  {
	  return mHashValue;
  }
  NxMat34    mPose;  // transform for block of data
  SolidTri  *mSolidTri;
  LineTri   *mLineTri;
  PrintText *mPrintText;

  NxU32    mHashValue;
  bool     mVisibleState;
  bool	   mScreenSpace;
};

class MyRenderDebug : public RenderDebug, public Memalloc
{
public:
	MyRenderDebug(void)
	{

		mDebugTris.Set(8192,8192,100000,"RenderDebug->SolidTri",__FILE__,__LINE__); // initialize the wireframe triangle pool.
		mDebugLines.Set(8192,8192,100000,"RenderDebug->LineTri",__FILE__,__LINE__); // initialize the wireframe triangle pool.

		mUpdateCount = 0;
		mStackIndex = 0;
		mInitTris = false;
        mInitPrintText = false;
		mInitLines = false;
		mFrameTime = 1.0f/60.0f;
		mSolidCount  = 0;
        mSolidScreenCount = 0;
		mLineCount   = 0;
        mScreenCount = 0;
		mRenderCount = 0;
		mComputeBound = false; // set to non-zero if the application requests a bounding volume
		mBoundsChanged = true;
		mRenderBounds.setInfinite();
		mBlockIndex = 0;
		mUseBlock = 0;
        mCurrentBlock = 0;

	}

	virtual void  debugPolygon(NxU32 pcount,const NxVec3 *points)
	{
		if ( points )
			DebugPolygon(pcount,&points[0].x);
	}

	virtual void  debugLine(const NxVec3 &p1,const NxVec3 &p2)
	{
		DebugLine(&p1.x,&p2.x);
	}

	virtual void  debugOrientedLine(const NxVec3 &p1,const NxVec3 &p2,const NxMat34 &transform)
	{
		NxF32 matrix[16];
		transform.getColumnMajor44(matrix);
		DebugOrientedLine(&p1.x,&p2.x,matrix);
	}

	virtual void  debugRay(const NxVec3 &p1,const NxVec3 &p2)
	{
		DebugRay(&p1.x,&p2.x);
	}

	virtual void  debugCylinder(const NxVec3 &p1,const NxVec3 &p2,NxF32 radius)
	{
		DebugCylinder(&p1.x,&p2.x,radius);
	}

	virtual void  debugThickRay(const NxVec3 &p1,const NxVec3 &p2,NxF32 raySize=0.02f)
	{
		DebugThickRay(&p1.x,&p2.x,raySize);
	}

	virtual void  debugPlane(const NxPlane &plane,NxF32 radius1,NxF32 radius2)
	{
		DebugPlane(&plane.normal.x,radius1,radius2);
	}

	virtual void  debugTri(const NxVec3 &p1,const NxVec3 &p2,const NxVec3 &p3)
	{
	  DebugTri(&p1.x,&p2.x,&p3.x);
	}

	virtual void  debugTriNormals(const NxVec3 &p1,const NxVec3 &p2,const NxVec3 &p3,const NxVec3 &n1,const NxVec3 &n2,const NxVec3 &n3)
	{
		DebugTri(&p1.x,&p2.x,&p3.x,&n1.x,&n2.x,&n3.x);
	}

	virtual void  debugBound(const NxVec3 &bmin,const NxVec3 &bmax)
	{
		DebugBound(&bmin.x,&bmax.x);
	}

	virtual void  debugOrientedBound(const NxVec3 &sides,const NxMat34 &transform)
	{
		NxF32 matrix[16];
		transform.getColumnMajor44(matrix);
		DebugOrientedBound(&sides.x,matrix);
	}

	virtual void  debugOrientedBound(const NxVec3 &bmin,const NxVec3 &bmax,const NxVec3 &pos,const NxQuat &quat)
	{
		NxF32 q[4];
		quat.getXYZW(q);
		DebugOrientedBound(&bmin.x,&bmax.x,&pos.x,q);
	}

	virtual void  debugOrientedBound(const NxVec3 &bmin,const NxVec3 &bmax,const NxMat34 &xform)
	{
		NxF32 matrix[16];
		xform.getColumnMajor44(matrix);
		DebugOrientedBound(&bmin.x,&bmax.x,matrix);
	}

	virtual void  debugSphere(const NxVec3 &pos,NxF32 radius)
	{
		DebugSphere(&pos.x,radius);
	}

	virtual void  debugOrientedSphere(NxF32 radius,const NxMat34 &transform)
	{
		NxF32 matrix[16];
		transform.getColumnMajor44(matrix);
		DebugOrientedSphere(radius,matrix);
	}

	virtual void  debugCapsule(const NxVec3 &center,NxF32 radius,NxF32 height)
	{
		DebugCapsule(&center.x,radius,height);
	}

	virtual void  debugOrientedCapsule(NxF32 radius,NxF32 height,const NxMat34 &transform)
	{
		NxF32 matrix[16];
		transform.getColumnMajor44(matrix);
		DebugOrientedCapsule(radius,height,matrix);
	}

	virtual void  debugPoint(const NxVec3 &pos,NxF32 radius)
	{
		DebugPoint(&pos.x,radius);
	}

	virtual void  debugAxes(const NxMat34 &transform,NxF32 distance,NxF32 brightness)
	{
		NxF32 matrix[16];
		transform.getColumnMajor44(matrix);
		DebugAxes(matrix,distance,brightness);
	}

	virtual void  debugDetailedSphere(const NxVec3 &pos,NxF32 radius,NxU32 stepCount)
	{
		DebugDetailedSphere(&pos.x,radius,stepCount);
	}

	NxF32 getRenderScale(void)
	{
		return mCurrentState.mRenderScale;
	}

	void  setRenderScale(NxF32 scale)
	{
		mCurrentState.mRenderScale = scale;
	}

	virtual NxU32 getDebugColor(bool reset_index=false)
	{
		NxU32 colors[8] =
		{
			0xFF0000,
			0x00FF00,
			0x0000FF,
			0xFFFF00,
			0x00FFFF,
			0xFF00FF,
			0xFFFFFF,
			0x808080
		};

		static NxU32 cindex = 0;

		if ( reset_index )
			cindex = 0;

		NxU32 color = colors[cindex];
		cindex++;
		if ( cindex == 8 ) cindex = 0;

		return color;
	}


	virtual void destroy(void)
	{
		delete this;
	}

	virtual void release(void)
	{
		destroy();
	}

	virtual ~MyRenderDebug(void)
	{
        {
            NxI32 pcount = mPrintText.Begin();
            for (NxI32 i=0; i<pcount; i++)
            {
                PrintText *pt = mPrintText.GetNext();
                pt->~PrintText();
                mPrintText.Release(pt);
            }
        }

	}


    virtual void render(NxF32 dtime,RenderDebugInterface *iface)
    {

		mUpdateCount++;

#pragma warning(disable:4313)

		assert( mCurrentBlock == 0 );


		NxI32 tricount = mDebugTris.Begin();
		mSolidCount  = 0;
        mSolidScreenCount = 0;
		mLineCount   = 0;
        mScreenCount = 0;
		mRenderCount = 0;

		if ( tricount )
		{
            // solid shaded vertices in world space
			RenderDebugSolidVertex points[MAX_BUFFER_STACK];
			RenderDebugSolidVertex *start   = 0;
			RenderDebugSolidVertex *stop    = 0;
			RenderDebugSolidVertex *current = 0;

			stop    = &points[MAX_BUFFER_STACK-3];
			start = current = points;

            // solid shaded vertices in screen space
			RenderDebugSolidVertex spoints[MAX_BUFFER_STACK];
			RenderDebugSolidVertex *s_start   = 0;
			RenderDebugSolidVertex *s_stop    = 0;
			RenderDebugSolidVertex *s_current = 0;

			s_stop    = &spoints[MAX_BUFFER_STACK-3];
			s_start = s_current = spoints;

			for (NxI32 i=0; i<tricount; i++)
			{
				SolidTri *tri = mDebugTris.GetNext();

                if ( tri->isScreenSpace() )
				{
					bool remove;
					s_current = tri->RenderSolid(dtime,s_current,remove);
					if ( remove )
					{
						if ( tri->mBlock != 0 )
						{
							BlockInfoHash::iterator found = mBlocksHash.find( (NxU32)tri->mBlock );
							if ( found != mBlocksHash.end() )
							{
								BlockInfo *b = (*found).second;
								if ( b->mSolidTri == tri )
								{
									b->mSolidTri = tri->GetNext();
								}
							}
							else
							{
								assert(0);
							}
						}

						mDebugTris.Release(tri);
						mBoundsChanged = true;
					}
					if ( s_current >= s_stop )
					{
						NxU32 count = (NxU32)(s_current - s_start);
						iface->debugRenderTriangles(count/3,s_start,true,true);
						s_current = s_start;
					}
				}
                else
                {
					bool remove;
					current = tri->RenderSolid(dtime,current,remove);
					if ( remove )
					{
						if ( tri->mBlock != 0 )
						{
							BlockInfoHash::iterator found = mBlocksHash.find( (NxU32)tri->mBlock );
							if ( found != mBlocksHash.end() )
							{
								BlockInfo *b = (*found).second;
								if ( b->mSolidTri == tri )
								{
									b->mSolidTri = tri->GetNext();
								}
							}
							else
							{
								assert(0);
							}
						}

						mDebugTris.Release(tri);
						mBoundsChanged = true;
					}
					if ( current >= stop )
					{
						NxU32 count = (NxU32)(current - start);
						iface->debugRenderTriangles(count/3,start,true,false);
						if ( mComputeBound )
						{
							// JWR: TODO!!
						}
						current = start;
					}
                }
			}
			NxU32 count = (NxU32)(s_current - s_start);
			if ( count )
			{
				iface->debugRenderTriangles(count/3,s_start,false,true);
			}
			count = (NxU32)(current - start);
			if ( count )
			{
				iface->debugRenderTriangles(count/3,start,true,false);
				if ( mComputeBound )
				{
					//JWR:TODO!
				}
			}
		}

		// Process text after solid shaded has been processed.
		{
			NxI32 tcount = mPrintText.Begin();
			for (NxI32 i=0; i<tcount; i++)
			{
				PrintText *pt = mPrintText.GetNext();

				pushRenderState();
				mCurrentState = pt->mRenderState;
				setCurrentDisplayTime(0.0001f);
				removeFromCurrentState(DebugRenderState::InfiniteLifeSpan);
				NxMat34 pose  = pt->mTransform;
				if ( pt->mBlock )
				{
					pose.multiply(pose,pt->mBlock->mPose);
				}

				if ( mCurrentState.isCameraFacing() )
				{
					//void computeLookAt(const NxF32 *eye,const NxF32 *look,const NxF32 *upVector,NxF32 *matrix)
					NxVec3 upVector(0,1,0);
					NxF32 matrix[16];
					computeLookAt(&pose.t.x,&mEyePos.x,&upVector.x,matrix);
					pose.M.setColumnMajorStride4(matrix);
				}

				bool remove = pt->process(dtime,this,mVectorFont,pose,mCurrentState.mTextScale,mCurrentState.isCentered());

				popRenderState();

				if ( remove )
				{
					if ( pt->mBlock != 0 )
					{
						if ( pt->mBlock->mPrintText == pt )
						{
							pt->mBlock->mPrintText = pt->GetNext();
						}
					}
					pt->~PrintText(); // free up memory.
					mPrintText.Release(pt);
				}
			}
		}



		tricount = mDebugLines.Begin();

		if ( tricount )
		{
			RenderDebugVertex linePoints[MAX_BUFFER_STACK];
			RenderDebugVertex *stop    = 0;
			RenderDebugVertex *current = 0;
			RenderDebugVertex *start   = 0;

			stop    = &linePoints[MAX_BUFFER_STACK-6];
			current = start = linePoints;

			mLineCount   = 0;

			RenderDebugVertex screenPoints[MAX_BUFFER_STACK];
			RenderDebugVertex *screen_stop    = 0;
			RenderDebugVertex *screen_current = 0;
			RenderDebugVertex *screen_start   = 0;

			screen_stop    = &screenPoints[MAX_BUFFER_STACK-6];
			screen_current = screen_start = screenPoints;

			mScreenCount   = 0;


			for (NxI32 i=0; i<tricount; i++)
			{
				LineTri *tri = mDebugLines.GetNext();
                if ( tri->isScreen() )
                {
					bool remove;
					screen_current = tri->RenderLine(dtime,screen_current,remove,true);
					if ( remove )
					{
						if ( tri->mBlock != 0 )
						{
							BlockInfoHash::iterator found = mBlocksHash.find( (NxU32)tri->mBlock );
							if ( found != mBlocksHash.end() )
							{
								BlockInfo *b = (*found).second;
								if ( b->mLineTri == tri )
								{
									b->mLineTri = tri->GetNext();
								}
							}
							else
							{
								assert(0);
							}
						}
						mDebugLines.Release(tri);
						mBoundsChanged = true;
					}
					if ( screen_current >= screen_stop )
					{
						NxU32 count = (NxU32)(screen_current - screen_start);
						iface->debugRenderLines(count/2,screen_start,false,true);
						screen_current = screen_start;
					}
                }
                else
				{
					bool remove;
					current = tri->RenderLine(dtime,current,remove,true);
					if ( remove )
					{
						if ( tri->mBlock != 0 )
						{
							BlockInfoHash::iterator found = mBlocksHash.find( (NxU32)tri->mBlock );
							if ( found != mBlocksHash.end() )
							{
								BlockInfo *b = (*found).second;
								if ( b->mLineTri == tri )
								{
									b->mLineTri = tri->GetNext();
								}
							}
							else
							{
								assert(0);
							}
						}
						mDebugLines.Release(tri);
						mBoundsChanged = true;
					}
					if ( current >= stop )
					{
						NxU32 count = (NxU32)(current - start);
						iface->debugRenderLines(count/2,start,true,false);
						if ( mComputeBound )
						{
							// TODO TODO
						}
						current = start;
					}
				}
			}

			NxU32 count = (NxU32)(current - start);
			if ( count )
			{
				iface->debugRenderLines(count/2,start,true,false);
				if ( mComputeBound )
				{
					// TODO TODO
				}
			}
			count = (NxU32)(screen_current - screen_start);
			if ( count )
			{
				iface->debugRenderLines(count/2,screen_start,false,true);
			}

		}
	}

	virtual void DebugTri(const NxF32 *p1,const NxF32 *p2,const NxF32 *p3)
	{
        DebugTri(p1,p2,p3,0,0,0);
	}

	virtual void DebugTri(const NxF32 *p1,const NxF32 *p2,const NxF32 *p3,const NxF32 *n1,const NxF32 *n2,const NxF32 *n3)
	{
        bool wireframe = true;
        NxU32 wireColor = mCurrentState.mColor;

        if ( mCurrentState.isSolid() )
        {
    		SolidTri *tri = mDebugTris.GetFreeLink(); // pull a free triangle from the pool
    		if ( tri )
    		{
                if ( mCurrentBlock && mCurrentBlock->mSolidTri == 0 )
                {
                    mCurrentBlock->mSolidTri = tri;
                }
    			mBoundsChanged = true;
				if ( mCurrentState.isCounterClockwise() )
				{
					if ( n1 && n2 && n3 )
					{
						tri->Set(p3,p2,p1,n3,n2,n1,mCurrentState.mColor,mCurrentState.getDisplayTime(),mCurrentState.mRenderScale, mUseBlock, mCurrentState.isScreen() ); // set condition of the triangle
					}
					else
					{
						tri->Set(p3,p2,p1,mCurrentState.mColor,mCurrentState.getDisplayTime(),mCurrentState.mRenderScale, mUseBlock, mCurrentState.isScreen() ); // set condition of the triangle
					}
				}
				else
				{
					if ( n1 && n2 && n3 )
					{
     					tri->Set(p1,p2,p3,n1,n2,n3,mCurrentState.mColor,mCurrentState.getDisplayTime(),mCurrentState.mRenderScale, mUseBlock, mCurrentState.isScreen() ); // set condition of the triangle
					}
					else
					{
     					tri->Set(p1,p2,p3,mCurrentState.mColor,mCurrentState.getDisplayTime(),mCurrentState.mRenderScale, mUseBlock, mCurrentState.isScreen() ); // set condition of the triangle
					}
				}

    			if ( mCurrentBlock )
    			{
    				tri->transform(mCurrentBlock->mPose);
    			}

    		}
            wireframe = mCurrentState.isWireframeOverlay();
            wireColor = mCurrentState.mArrowColor;
        }
        if ( wireframe )
        {
    		LineTri *tri = mDebugLines.GetFreeLink(); // pull a free triangle from the pool
    		if ( tri )
    		{
                if ( mCurrentBlock && mCurrentBlock->mLineTri == 0 )
                {
                    mCurrentBlock->mLineTri = tri;
                }
    			mBoundsChanged = true;
                NxU32 flags = mCurrentState.isUseZ() ?  LTF_USEZ : LTF_NONE;
                flags|= mCurrentState.isScreen() ? LTF_SCREEN : 0;
    			tri->Set(p1,p2,p3,mCurrentState.mColor,mCurrentState.getDisplayTime(),(RENDER_DEBUG_NVSHARE::LineTriFlag)flags, mCurrentState.mRenderScale, mUseBlock ); // set condition of the triangle
    			if ( mCurrentBlock )
    			{
    				tri->transform(mCurrentBlock->mPose);
    			}

    		}
        }
	}

	void adjust(const NxF32 *p,NxF32 *d,NxF32 arrowSize)
	{
		d[0] = d[0]*arrowSize+p[0];
		d[1] = d[1]*arrowSize+p[1];
		d[2] = d[2]*arrowSize+p[2];
	}

	void DebugRay(const NxF32 *p1,const NxF32 *p2)
	{
		DebugLine(p1,p2);

		NxF32 dir[3];

		dir[0] = p2[0] - p1[0];
		dir[1] = p2[1] - p1[1];
		dir[2] = p2[2] - p1[2];

		NxF32 mag = fm_normalize(dir);
		NxF32 arrowSize = mCurrentState.mArrowSize;

		if ( arrowSize > (mag*0.2f) )
		{
			arrowSize = mag*0.2f;
		}

		NxF32 ref[3] = { 0, 1, 0 };

		NxF32 quat[4];

		fm_rotationArc(ref,dir,quat);

		NxF32 matrix[16];
		fm_quatToMatrix(quat,matrix);
		fm_setTranslation(p2,matrix);


		NxU32 pcount = 0;
		NxF32 points[24*3];
		NxF32 *dest = points;

		for (NxF32 a=30; a<=360; a+=30)
		{
			NxF32 r = a*FM_DEG_TO_RAD;
			NxF32 x = cosf(r)*arrowSize;
			NxF32 y = sinf(r)*arrowSize;

			dest[0] = x;
			dest[1] = -3*arrowSize;
			dest[2] = y;
			dest+=3;
			pcount++;
		}

		NxF32 *prev = &points[(pcount-1)*3];
		NxF32 *p = points;
		NxF32 center[3] = { 0, -2.5f*arrowSize, 0 };
		NxF32 top[3]    = { 0, 0, 0 };

		NxF32 _center[3];
		NxF32 _top[3];

		fm_transform(matrix,center,_center);
		fm_transform(matrix,top,_top);

        pushRenderState();
        addToCurrentState(DebugRenderState::SolidWireShaded);
        setCurrentColor(mCurrentState.mArrowColor,0xFFFFFF);

		for (NxU32 i=0; i<pcount; i++)
		{

			NxF32 _p[3];
			NxF32 _prev[3];
			fm_transform(matrix,p,_p);
			fm_transform(matrix,prev,_prev);

			DebugTri(_p,_center,_prev);
			DebugTri(_prev,_top,_p);

			prev = p;
			p+=3;
		}
        popRenderState();
	}

	virtual void DebugLine(const NxF32 *p1,const NxF32 *p2)
	{
		LineTri *tri = mDebugLines.GetFreeLink(); // pull a free triangle from the pool
		if ( tri )
		{
            if ( mCurrentBlock && mCurrentBlock->mLineTri == 0 )
            {
                mCurrentBlock->mLineTri = tri;
            }
			mBoundsChanged = true;
			NxU32 flags = mCurrentState.isUseZ() ?  LTF_USEZ : LTF_NONE;
			flags|= mCurrentState.isScreen() ? LTF_SCREEN : 0;
			NxF32 displayTime = mCurrentState.getDisplayTime();
			tri->Set(p1,p2,0, mCurrentState.mColor | 0xFF000000, displayTime,(RENDER_DEBUG_NVSHARE::LineTriFlag)flags, mCurrentState.mRenderScale, mUseBlock  ); // set condition of the triangle
			if ( mCurrentBlock )
			{
				tri->transform(mCurrentBlock->mPose);
			}

		}
	}
	void DebugOrientedLine(const NxF32 *p1,const NxF32 *p2,const NxF32 *transform)
	{
		NxF32 t1[3];
		NxF32 t2[3];
		fm_transform(transform,p1,t1);
		fm_transform(transform,p2,t2);
		DebugLine(t1,t2);
	}

   void DebugBound(const NxVec3 *box)
   {
        if ( mCurrentState.isSolid() )
        {
			DebugTri(&box[2].x,&box[1].x,&box[0].x);
			DebugTri(&box[3].x,&box[2].x,&box[0].x);

			DebugTri(&box[7].x,&box[2].x,&box[3].x);
			DebugTri(&box[7].x,&box[6].x,&box[2].x);

			DebugTri(&box[5].x,&box[1].x,&box[2].x);
			DebugTri(&box[5].x,&box[2].x,&box[6].x);

			DebugTri(&box[5].x,&box[4].x,&box[1].x);
			DebugTri(&box[4].x,&box[0].x,&box[1].x);

			DebugTri(&box[4].x,&box[6].x,&box[7].x);
			DebugTri(&box[4].x,&box[5].x,&box[6].x);

			DebugTri(&box[4].x,&box[7].x,&box[0].x);
			DebugTri(&box[7].x,&box[3].x,&box[0].x);
        }
        else
        {
			DebugLine(&box[0].x, &box[1].x);
			DebugLine(&box[1].x, &box[2].x);
			DebugLine(&box[2].x, &box[3].x);
			DebugLine(&box[3].x, &box[0].x);

			DebugLine(&box[4].x, &box[5].x);
			DebugLine(&box[5].x, &box[6].x);
			DebugLine(&box[6].x, &box[7].x);
			DebugLine(&box[7].x, &box[4].x);

			DebugLine(&box[0].x, &box[4].x);
			DebugLine(&box[1].x, &box[5].x);
			DebugLine(&box[2].x, &box[6].x);
			DebugLine(&box[3].x, &box[7].x);
		}
   }

	virtual void DebugBound(const NxF32 *bmin,const NxF32 *bmax)
	{
		NxVec3 box[8];

		box[0].set( bmin[0], bmin[1], bmin[2] );
		box[1].set( bmax[0], bmin[1], bmin[2] );
		box[2].set( bmax[0], bmax[1], bmin[2] );
		box[3].set( bmin[0], bmax[1], bmin[2] );
		box[4].set( bmin[0], bmin[1], bmax[2] );
		box[5].set( bmax[0], bmin[1], bmax[2] );
		box[6].set( bmax[0], bmax[1], bmax[2] );
		box[7].set( bmin[0], bmax[1], bmax[2] );

        DebugBound(box);

	}

	virtual void DebugOrientedBound(const NxF64 *_bmin,const NxF64 *_bmax,const NxF64 *_pos,const NxF64 *_rot)
    {
        NxF32 bmin[3];
        NxF32 bmax[3];
        NxF32 pos[3];
        NxF32 rot[4];
        bmin[0] = (NxF32)_bmin[0];
        bmin[1] = (NxF32)_bmin[1];
        bmin[2] = (NxF32)_bmin[2];
        bmax[0] = (NxF32)_bmax[0];
        bmax[1] = (NxF32)_bmax[1];
        bmax[2] = (NxF32)_bmax[2];
        pos[0] = (NxF32)_pos[0];
        pos[1] = (NxF32)_pos[1];
        pos[2] = (NxF32)_pos[2];
        rot[0] = (NxF32)_rot[0];
        rot[1] = (NxF32)_rot[1];
        rot[2] = (NxF32)_rot[2];
        rot[3] = (NxF32)_rot[3];
        DebugOrientedBound(bmin,bmax,pos,rot);
    }

	virtual void DebugOrientedBound(const NxF32 *bmin,const NxF32 *bmax,const NxF32 *pos,const NxF32 *rot)
	{
		NxMat34 m;
		NxQuat q;
		q.setXYZW(rot[0],rot[1],rot[2],rot[3]);
		m.M.fromQuat(q);
		NxVec3 p(pos);
		m.t = p;
		NxF32 matrix[16];
		m.getColumnMajor44(matrix);
		DebugOrientedBound(bmin,bmax,matrix);
	}

	virtual void DebugOrientedBound(const NxF64 *_bmin,const NxF64 *_bmax,const NxF64 *_xform)
    {
        NxF32 bmin[3];
        NxF32 bmax[3];
        bmin[0] = (NxF32) _bmin[0];
        bmin[1] = (NxF32)_bmin[1];
        bmin[2] = (NxF32)_bmin[2];
        bmax[0] = (NxF32)_bmax[0];
        bmax[1] = (NxF32)_bmax[1];
        bmax[2] = (NxF32)_bmax[2];
        NxF32 xform[16];
        for (NxU32 i=0; i<16; i++) xform[i] = (NxF32)_xform[0];
        DebugOrientedBound(bmin,bmax,xform);

    }

	virtual void DebugOrientedBound(const NxF32 *bmin,const NxF32 *bmax,const NxF32 *xform)
	{
		NxVec3 box[8];

		box[0].set( bmin[0], bmin[1], bmin[2] );
		box[1].set( bmax[0], bmin[1], bmin[2] );
		box[2].set( bmax[0], bmax[1], bmin[2] );
		box[3].set( bmin[0], bmax[1], bmin[2] );
		box[4].set( bmin[0], bmin[1], bmax[2] );
		box[5].set( bmax[0], bmin[1], bmax[2] );
		box[6].set( bmax[0], bmax[1], bmax[2] );
		box[7].set( bmin[0], bmax[1], bmax[2] );

		NxMat34 m;
		m.setColumnMajor44(xform);

		m.multiply(box[0],box[0]); // = m.Transform(box[0]);
		m.multiply(box[1],box[1]);
		m.multiply(box[2],box[2]);
		m.multiply(box[3],box[3]);
		m.multiply(box[4],box[4]);
		m.multiply(box[5],box[5]);
		m.multiply(box[6],box[6]);
		m.multiply(box[7],box[7]);

        DebugBound(box);

	}

	virtual void DebugOrientedBound(const NxF64 *_sides,const NxF64 *_transform)
    {
        NxF32 sides[3];
        NxF32 transform[16];
        sides[0] = (NxF32) _sides[0];
        sides[1] = (NxF32) _sides[1];
        sides[2] = (NxF32) _sides[2];
        for (NxU32 i=0; i<16; i++)
            transform[i] = (NxF32)_transform[i];

    }

	virtual void DebugOrientedBound(const NxF32 *sides,const NxF32 *transform)
	{
		NxVec3 box[8];

		NxF32 bmin[3];
		NxF32 bmax[3];

		bmin[0] = -sides[0]*0.5f;
		bmin[1] = -sides[1]*0.5f;
		bmin[2] = -sides[2]*0.5f;

		bmax[0] = +sides[0]*0.5f;
		bmax[1] = +sides[1]*0.5f;
		bmax[2] = +sides[2]*0.5f;

        DebugOrientedBound(bmin,bmax,transform);

	}

	virtual void DebugSphere(const NxF32 *pos,NxF32 radius)
	{
		const NxF32 *source = debug_sphere;

		for (NxI32 i=0; i<32; i++)
		{
			NxVec3 p1( source );
			source+=3;
			NxVec3 p2( source );
			source+=3;
			NxVec3 p3( source );
			source+=3;

			p1.x = p1.x*radius + pos[0];
			p1.y = p1.y*radius + pos[1];
			p1.z = p1.z*radius + pos[2];

			p2.x = p2.x*radius + pos[0];
			p2.y = p2.y*radius + pos[1];
			p2.z = p2.z*radius + pos[2];

			p3.x = p3.x*radius + pos[0];
			p3.y = p3.y*radius + pos[1];
			p3.z = p3.z*radius + pos[2];

			DebugTri(&p1.x,&p2.x,&p3.x);
		}
	}

	virtual void DebugHalfSphere(const NxF32 *pos,NxF32 radius)
	{
		const NxF32 *source = debug_halfsphere;

		for (NxI32 i=0; i<16; i++)
		{
			NxVec3 p1( source );
			source+=3;
			NxVec3 p2( source );
			source+=3;
			NxVec3 p3( source );
			source+=3;

			p1.x = p1.x*radius + pos[0];
			p1.y = p1.y*radius + pos[1];
			p1.z = p1.z*radius + pos[2];

			p2.x = p2.x*radius + pos[0];
			p2.y = p2.y*radius + pos[1];
			p2.z = p2.z*radius + pos[2];

			p3.x = p3.x*radius + pos[0];
			p3.y = p3.y*radius + pos[1];
			p3.z = p3.z*radius + pos[2];

			DebugTri(&p1.x,&p2.x,&p3.x);
		}
	}

	virtual void DebugPoint(const NxF32 *pos,NxF32 radius)
	{
		const NxF32 *source = debug_point;

		for (NxI32 i=0; i<3; i++)
		{
			NxVec3 p1( source );
			source+=3;
			NxVec3 p2( source );
			source+=3;

			p1.x = p1.x*radius + pos[0];
			p1.y = p1.y*radius + pos[1];
			p1.z = p1.z*radius + pos[2];

			p2.x = p2.x*radius + pos[0];
			p2.y = p2.y*radius + pos[1];
			p2.z = p2.z*radius + pos[2];

			DebugLine(&p1.x,&p2.x);
		}
	}

	virtual void DebugOrientedSphere(NxF32 radius,const NxF32 *transform)
	{
		const NxF32 *source = debug_sphere;

		NxMat34 m;
		m.setColumnMajor44(transform);

		for (NxI32 i=0; i<32; i++)
		{
			NxVec3 p1( source );
			source+=3;
			NxVec3 p2( source );
			source+=3;
			NxVec3 p3( source );
			source+=3;

			p1*=radius;
			p2*=radius;
			p3*=radius;

			m.multiply( p1, p1 );
			m.multiply( p2, p2 );
			m.multiply( p3, p3 );

			DebugTri(&p1.x,&p2.x,&p3.x);
		}
	}

	NxI32 ClampColor(NxI32 c)
	{
		if ( c < 0 )
			c = 0;
		else if ( c > 255 )
			c = 255;
		return c;
	};

	NxU32 GetColor(NxF32 r,NxF32 g,NxF32 b,NxF32 brightness)
	{
		NxI32 red   = NxI32(r*brightness*255.0f);
		NxI32 green = NxI32(g*brightness*255.0f);
		NxI32 blue  = NxI32(b*brightness*255.0f);
		red   = ClampColor(red);
		green = ClampColor(green);
		blue  = ClampColor(blue);
		NxU32 color = 0xFF000000 | (red<<16) | (green<<8) | blue;
		return color;
	}

	virtual void DebugAxes(const NxF32 *xform,NxF32 distance,NxF32 brightness)
	{
		NxMat34 m;
		m.setColumnMajor44(xform);

		NxU32 red    = GetColor(1,0,0,brightness);
		NxU32 green  = GetColor(0,1,0,brightness);
		NxU32 blue   = GetColor(0,0,1,brightness);
		NxU32 yellow = GetColor(1,1,0,brightness);

		NxVec3 px(distance*2,0,0);
		NxVec3 py(0,distance*1.5f,0);
		NxVec3 pz(0,0,distance);

		m.multiply(px,px);
		m.multiply(py,py);
		m.multiply(pz,pz);

		pushRenderState();
		addToCurrentState(DebugRenderState::SolidWireShaded);
		setCurrentColor(yellow,0xFFFFFF);
		setCurrentArrowSize(distance*0.1f);
		debugSphere(m.t,distance*0.1f);
		setCurrentColor(red,red);
		debugThickRay(m.t,px,distance*0.02f);
		setCurrentColor(green,green);
		debugThickRay(m.t,py,distance*0.02f);
		setCurrentColor(blue,blue);
		debugThickRay(m.t,pz,distance*0.02f);
		popRenderState();
	}

	virtual void reset(NxI32 blockIndex)
	{

		mBoundsChanged = true;

        if ( blockIndex > 0 ) // reseting a specific block!
        {
			BlockInfoHash::iterator found = mBlocksHash.find( (NxU32)blockIndex );
            if ( found != mBlocksHash.end() )
            {
				BlockInfo *b = (*found).second;
                SolidTri *t = b->mSolidTri;
                while ( t && t->getBlock() == blockIndex )
                {
                    SolidTri *n = t->GetNext();
                    mDebugTris.Release(t);
                    t = n;
                }
                LineTri *lt = b->mLineTri;
                while ( lt && lt->getBlock() == blockIndex )
                {
                    LineTri *n = lt->GetNext();
                    mDebugLines.Release(lt);
                    lt = n;
                }
                PrintText *pt = b->mPrintText;
                while (pt && pt->mBlock == b )
                {
					pt->~PrintText();
					PrintText *n = pt->GetNext();
					mPrintText.Release(pt);
                    pt = n;
                }
				mBlocksHash.erase(found);
				delete b;
            }
        }
        else
        {
     		NxI32 count = mDebugTris.Begin();
     		for (NxI32 i=0; i<count; i++)
     		{
     			SolidTri *wt = mDebugTris.GetNext();
     			if ( wt->getBlock() == blockIndex || blockIndex == -1 )
     			  mDebugTris.Release(wt);
     		}
     		count = mDebugLines.Begin();
     		for (NxI32 i=0; i<count; i++)
     		{
     			LineTri *wt = mDebugLines.GetNext();
     			if ( wt->getBlock() == blockIndex || blockIndex == -1 )
     			  mDebugLines.Release(wt);
     		}
			count = mPrintText.Begin();
			for (NxI32 i=0; i<count; i++)
			{
				PrintText *pt = mPrintText.GetNext();
     			if ( pt->mBlock == NULL )
				{
  				  pt->~PrintText();
				  mPrintText.Release(pt);
				}
			}

        }
	}

	void DebugCapsule(const NxF32 *pos,NxF32 radius,NxF32 height)
	{
		NxMat34 t;
		NxVec3 p(pos);
		t.t = p;
		NxF32 matrix[16];
		t.getColumnMajor44(matrix);
		DebugOrientedCapsule(radius,height,matrix);
	}

	void DebugOrientedCapsule(NxF32 radius,NxF32 height,const NxF32 *transform)
	{
		NxVec3 prev1;
		NxVec3 prev2;

		NxF32 h2 = height*0.5f;

		NxVec3 top(0,0,0);
		NxVec3 bottom(0,0,0);

		top.y+=(height*0.5f)+radius;
		bottom.y-=(height*0.5f)+radius;

		for (NxI32 a=0; a<=360; a+=15)
		{
			NxF32 r = (NxF32)a*FM_DEG_TO_RAD;

			NxF32 x = radius*cosf(r);
			NxF32 z = radius*sinf(r);

			NxVec3 p1(x,-h2,z);
			NxVec3 p2(x,h2,z);

			DebugOrientedLine( &p1.x, &p2.x, transform);

			DebugOrientedLine( &p2.x, &top.x, transform);
			DebugOrientedLine( &p1.x, &bottom.x, transform);

			if ( a != 0 )
			{
				DebugOrientedLine(&prev1.x, &p1.x, transform);
				DebugOrientedLine(&prev2.x, &p2.x, transform);
			}

			prev1 = p1;
			prev2 = p2;
		}

	}

	bool getWireFrame(void)
	{
		bool ret = false;

		return ret;
	}

	void debugPlane(const NxF32 *plane,NxF32 radius1,NxF32 radius2)
	{
#if 0
		NxF32 ref[3] = { 0, 1, 0 };

		NxF32 quat[4];
		fm_rotationArc(ref,plane,quat);

		NxF32 matrix[16];
		fm_quatToMatrix(quat,matrix);

		NxF32 stepsize = 360/20;

		NxF32 prev[3] = { 0, 0, 0 };

		NxF32 pos[3];
		NxF32 first[3];

		NxF32 origin[3] = { 0, -plane[3], 0 };
		NxF32 center[3];

		fm_transform(matrix,origin,center);
		fm_setTranslation(center,matrix);

		for (NxF32 d=0; d<360; d+=stepsize)
		{
			NxF32 a = d*FM_DEG_TO_RAD;

			NxF32 dx = cosf(a)*radius1;
			NxF32 dy = 0; //
			NxF32 dz = sinf(a)*radius2;

			NxF32 _pos[3] = { dx, dy, dz };

			fm_transform(matrix,_pos,pos);

			if ( spokes )
			{
				DebugLine(center,pos,color,duration,useZ);
			}

			if ( d == 0  )
			{
				first[0] = pos[0];
				first[1] = pos[1];
				first[2] = pos[2];
			}
			else
			{
				DebugLine(prev,pos,color,duration,useZ);
			}

			prev[0] = pos[0];
			prev[1] = pos[1];
			prev[2] = pos[2];


		}

		DebugLine(first,pos,color,duration,useZ);
#endif
	}


	void DebugPlane(const NxF32 *plane,NxF32 radius1,NxF32 radius2)
	{
		debugPlane(plane,radius1*0.25f, radius2*0.25f);
		debugPlane(plane,radius1*0.5f,  radius2*0.5f);
		debugPlane(plane,radius1*0.75f, radius2*0.75f);
		debugPlane(plane,radius1*1.0f,  radius2*1.0f);
	}

	void DebugThickRay(const NxF32 *p1,const NxF32 *p2,NxF32 raySize)
	{

        pushRenderState();
        removeFromCurrentState(DebugRenderState::SolidWireShaded);
        addToCurrentState(DebugRenderState::SolidShaded);

		NxF32 dir[3];

		dir[0] = p2[0] - p1[0];
		dir[1] = p2[1] - p1[1];
		dir[2] = p2[2] - p1[2];

		NxF32 mag = fm_normalize(dir);
		NxF32 arrowSize = mCurrentState.mArrowSize;

		if ( arrowSize > (mag*0.2f) )
		{
			arrowSize = mag*0.2f;
		}

		NxF32 ref[3] = { 0, 1, 0 };

		NxF32 quat[4];

		fm_rotationArc(ref,dir,quat);

		NxF32 matrix[16];
		fm_quatToMatrix(quat,matrix);
		fm_setTranslation(p2,matrix);


		NxU32 pcount = 0;
		NxF32 points[24*3];
		NxF32 *dest = points;

		for (NxF32 a=30; a<=360; a+=30)
		{
			NxF32 r = a*FM_DEG_TO_RAD;
			NxF32 x = cosf(r)*arrowSize;
			NxF32 y = sinf(r)*arrowSize;

			dest[0] = x;
			dest[1] = -3*arrowSize;
			dest[2] = y;
			dest+=3;
			pcount++;
		}

		NxF32 *prev = &points[(pcount-1)*3];
		NxF32 *p = points;
		NxF32 center[3] = { 0, -2.5f*arrowSize, 0 };
		NxF32 top[3]    = { 0, 0, 0 };

		NxF32 _center[3];
		NxF32 _top[3];

		fm_transform(matrix,center,_center);
		fm_transform(matrix,top,_top);

		DebugCylinder(p1,_center,raySize);

        popRenderState();
        pushRenderState();

        setCurrentColor(mCurrentState.mArrowColor,0xFFFFFF);

		for (NxU32 i=0; i<pcount; i++)
		{

			NxF32 _p[3];
			NxF32 _prev[3];
			fm_transform(matrix,p,_p);
			fm_transform(matrix,prev,_prev);

			DebugTri(_p,_center,_prev);
			DebugTri(_prev,_top,_p);

			prev = p;
			p+=3;
		}
        popRenderState();
	}


	void DebugCylinder(const NxF32 *p1,const NxF32 *p2,NxF32 radius)
	{
		NxF32 dir[3];

		dir[0] = p2[0] - p1[0];
		dir[1] = p2[1] - p1[1];
		dir[2] = p2[2] - p1[2];


		NxF32 ref[3] = { 0, 1, 0 };

		NxF32 quat[4];

		fm_rotationArc(ref,dir,quat);

		NxF32 matrix1[16];
		NxF32 matrix2[16];

		fm_quatToMatrix(quat,matrix1);
		fm_setTranslation(p2,matrix1);

		fm_quatToMatrix(quat,matrix2);
		fm_setTranslation(p1,matrix2);


		NxU32 pcount = 0;
		NxF32 points1[24*3];
		NxF32 points2[24*3];
		NxF32 *dest1 = points1;
		NxF32 *dest2 = points2;


		for (NxF32 a=30; a<=360; a+=30)
		{
			NxF32 r = a*FM_DEG_TO_RAD;
			NxF32 x = cosf(r)*radius;
			NxF32 y = sinf(r)*radius;

			NxF32 p[3] = { x, 0, y };

			fm_transform(matrix1,p,dest1);
			fm_transform(matrix2,p,dest2);

			dest1+=3;
			dest2+=3;
			pcount++;

		}
		assert( pcount < 24 );

		if ( mCurrentState.isSolid() )
		{
			NxF32 *prev1 = &points1[(pcount-1)*3];
			NxF32 *prev2 = &points2[(pcount-1)*3];

			NxF32 *scan1 = points1;
			NxF32 *scan2 = points2;

			for (NxU32 i=0; i<pcount; i++)
			{
				DebugTri(scan1,prev2,prev1);
				DebugTri(scan2,prev2,scan1);
				prev1 = scan1;
				prev2 = scan2;
				scan1+=3;
				scan2+=3;
			}

			DebugPolygon(pcount,points1);
			DebugPolygon(pcount,points2);

		}
		else
		{
			NxF32 *prev1 = &points1[(pcount-1)*3];
			NxF32 *prev2 = &points2[(pcount-1)*3];

			NxF32 *scan1 = points1;
			NxF32 *scan2 = points2;
			for (NxU32 i=0; i<pcount; i++)
			{
				DebugLine(scan1,scan2);

				DebugLine(p2,scan1);
				DebugLine(p1,scan2);

				DebugLine(prev1,scan1);
				DebugLine(prev2,scan2);

				prev1 = scan1;
				prev2 = scan2;

				scan1+=3;
				scan2+=3;
			}


		}
	}

	void DebugPolygon(NxU32 pcount,const NxF32 *points)
	{
		if ( pcount >= 3 )
		{
			if ( mCurrentState.isSolid() )
			{
				const NxF32 *p1 = points;
				const NxF32 *p2 = points+3;
				const NxF32 *p3 = points+6;
				DebugTri(p1,p2,p3);
				for (NxU32 i=3; i<pcount; i++)
				{
					p2 = p3;
					p3+=3;
					DebugTri(p1,p2,p3);
				}
			}
			else
			{
				const NxF32 *prev = &points[(pcount-1)*3];
				const NxF32 *p    = points;
				for (NxU32 i=0; i<pcount; i++)
				{
					DebugLine(prev,p);
					prev = p;
					p+=3;
				}
			}
		}
	}

	NxU32 getColor(NxU32 r,NxU32 g,NxU32 b,NxF32 percent)
	{
		NxU32 dr = (NxU32)((NxF32)r*percent);
		NxU32 dg = (NxU32)((NxF32)g*percent);
		NxU32 db = (NxU32)((NxF32)b*percent);
		r-=dr;
		g-=dg;
		b-=db;
		NxU32 c = (r<<16) | (g<<8) | b;
		return c;
	}

	void SwapYZ(NxVec3 &p)
	{
		NxF32 y = p.y;
		p.y = p.z;
		p.z = y;
	}

	void drawGrid(bool zup,NxU32 gridSize) // draw a grid.
	{
		NxI32  GRIDSIZE = gridSize;

		NxU32 c1 = getColor(133,153,181,0.1f);
		NxU32 c2 = getColor(133,153,181,0.3f);
		NxU32 c3 = getColor(133,153,181,0.5f);

		const NxF32 TSCALE   = 1.0f;

		NxF32 BASELOC = 0-0.05f;
		pushRenderState();

		for (NxI32 x=-GRIDSIZE; x<=GRIDSIZE; x++)
		{
			NxU32 c = c1;
			if ( (x%10) == 0 ) c = c2;
			if ( (x%GRIDSIZE) == 0 ) c = c3;

			NxVec3 p1( (NxF32)x,(NxF32) -GRIDSIZE, BASELOC );
			NxVec3 p2( (NxF32)x,(NxF32) +GRIDSIZE, BASELOC );

			p1*=TSCALE;
			p2*=TSCALE;

			SwapYZ(p1);
			SwapYZ(p2);
			setCurrentColor(c,c);
			debugLine(p1,p2);


		}

		for (NxI32 y=-GRIDSIZE; y<=GRIDSIZE; y++)
		{
			NxU32 c = c1;

			if ( (y%10) == 0 ) c = c2;
			if ( (y%GRIDSIZE) == 0 ) c = c3;

			NxVec3 p1((NxF32) -GRIDSIZE, (NxF32)y, BASELOC );
			NxVec3 p2( (NxF32)+GRIDSIZE, (NxF32)y, BASELOC );

			p1*=TSCALE;
			p2*=TSCALE;

			SwapYZ(p1);
			SwapYZ(p2);

			setCurrentColor(c,c);
			debugLine(p1,p2);
		}
		popRenderState();
	}


	void         setScreenSize(NxU32 screenX,NxU32 screenY)
	{
		mScreenWidth = screenX;
		mScreenHeight = screenY;
	}

	void         getScreenSize(NxU32 &screenX,NxU32 &screenY)
	{
		screenX = mScreenWidth;
		screenY = mScreenHeight;
	}

	const NxF32 *getEyePos(void)
	{
		return &mEyePos.x;
	}

	void         setViewProjectionMatrix(const NxF32 *view,const NxF32 *projection)
	{
		mViewMatrix.setColumnMajor44(view);
		mProjectionMatrix.setColumnMajor44(projection);
		mViewProjectionMatrix.multiply(mViewMatrix,mProjectionMatrix);
		// grab the world-space eye position.
		NxMat34 viewInverse;
		if ( mViewMatrix.getInverse( viewInverse ) )
			viewInverse.multiply( NxVec3( 0.0f, 0.0f, 0.0f ), mEyePos );
	}

	const NxF32 *getViewProjectionMatrix(void) const
	{
		mViewProjectionMatrix.getColumnMajor44(mViewProjectionMatrix44);
		return mViewProjectionMatrix44;
	}

	const NxF32 *getViewMatrix(void) const
	{
		mViewMatrix.getColumnMajor44(mViewMatrix44);
		return mViewMatrix44;
	}

	const NxF32 *getProjectionMatrix(void) const
	{
		mProjectionMatrix.getColumnMajor44(mProjectionMatrix44);
		return mProjectionMatrix44;
	}

	bool         screenToWorld(NxI32 sx,NxI32 sy,NxF32 *world,NxF32 *direction)
	{
		bool ret = false;

	#if 0
		NxI32 wid = (NxI32) mScreenWidth;
		NxI32 hit = (NxI32) mScrenHeight;

		if ( sx >= 0 && sx <= wid && sy >= 0 && sy <= hit )
		{
			NxVec3 vPickRayDir;
			NxVec3 vPickRayOrig;

			NxVec3 ptCursor(sx,sy,0);

			// Compute the vector of the pick ray in screen space

			D3DXVECTOR3 v;

			v.x =  ( ( ( 2.0f * ptCursor.x ) / wid  ) - 1 ) / mProjection._11;
			v.y = -( ( ( 2.0f * ptCursor.y ) / hit ) - 1 )  / mProjection._22;
			v.z =  1.0f;

			// Get the inverse view matrix
			D3DXMATRIXA16 m;
			D3DXMatrixInverse( &m, NULL, &mView );

			// Transform the screen space pick ray into 3D space
			vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
			vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
			vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;

			D3DXVec3Normalize(&vPickRayDir,&vPickRayDir);

			vPickRayOrig.x = m._41;
			vPickRayOrig.y = m._42;
			vPickRayOrig.z = m._43;

			world[0] = vPickRayOrig.x;
			world[1] = vPickRayOrig.y;
			world[2] = vPickRayOrig.z;

			direction[0] = vPickRayDir.x;
			direction[1] = vPickRayDir.y;
			direction[2] = vPickRayDir.z;

			ret = true;
		}
	#endif

		return ret;
	}

		virtual void DebugLine(const NxF64 *_p1,const NxF64 *_p2)
		{
			NxF32 p1[3];
			NxF32 p2[3];
			p1[0] = (NxF32)_p1[0];
			p1[1] = (NxF32)_p1[1];
			p1[2] = (NxF32)_p1[2];

			p2[0] = (NxF32)_p2[0];
			p2[1] = (NxF32)_p2[1];
			p2[2] = (NxF32)_p2[2];

			DebugLine(p1,p2);

		}
			virtual void DebugRay(const NxF64 *_p1,const NxF64 *_p2)
			{
				NxF32 p1[3];
				NxF32 p2[3];
				p1[0] = (NxF32)_p1[0];
				p1[1] = (NxF32)_p1[1];
				p1[2] = (NxF32)_p1[2];

				p2[0] = (NxF32)_p2[0];
				p2[1] = (NxF32)_p2[1];
				p2[2] = (NxF32)_p2[2];

				DebugRay(p1,p2);
			}

	virtual void DebugTri(const NxF64 *_p1,const NxF64 *_p2,const NxF64 *_p3)
	{
		NxF32 p1[3];
		NxF32 p2[3];
		NxF32 p3[3];
		p1[0] = (NxF32)_p1[0]; p1[1] = (NxF32)_p1[1]; p1[2] = (NxF32)_p1[2];
		p2[0] = (NxF32)_p2[0]; p2[1] = (NxF32)_p2[1]; p2[2] = (NxF32)_p2[2];
		p3[0] = (NxF32)_p3[0]; p3[1] = (NxF32)_p3[1]; p3[2] = (NxF32)_p3[2];
		DebugTri(p1,p2,p3);
	}

	virtual void DebugTri(const NxF64 *_p1,const NxF64 *_p2,const NxF64 *_p3,const NxF64 *_n1,const NxF64 *_n2,const NxF64 *_n3)
	{
		NxF32 p1[3];
		NxF32 p2[3];
		NxF32 p3[3];

        NxF32 n1[3];
        NxF32 n2[3];
        NxF32 n3[3];

		p1[0] = (NxF32)_p1[0]; p1[1] = (NxF32)_p1[1]; p1[2] = (NxF32)_p1[2];
		p2[0] = (NxF32)_p2[0]; p2[1] = (NxF32)_p2[1]; p2[2] = (NxF32)_p2[2];
		p3[0] = (NxF32)_p3[0]; p3[1] = (NxF32)_p3[1]; p3[2] = (NxF32)_p3[2];

		n1[0] = (NxF32)_n1[0]; n1[1] = (NxF32)_n1[1]; n1[2] = (NxF32)_n1[2];
		n2[0] = (NxF32)_n2[0]; n2[1] = (NxF32)_n2[1]; n2[2] = (NxF32)_n2[2];
		n3[0] = (NxF32)_n3[0]; n3[1] = (NxF32)_n3[1]; n3[2] = (NxF32)_n3[2];

		DebugTri(p1,p2,p3,n1,n2,n3);
	}

	virtual void DebugBound(const NxF64 *bmin,const NxF64 *bmax)
	{
		NxF32 b1[3] = { (NxF32)bmin[0], (NxF32)bmin[1],(NxF32)bmin[2] };
		NxF32 b2[3] = { (NxF32)bmax[0], (NxF32)bmax[1],(NxF32)bmax[2] };
		DebugBound(b1,b2);
	}

	virtual void DebugSphere(const NxF64 *_pos,NxF64 radius,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true,bool solid=false)
	{
		NxF32 pos[3];
		pos[0] = (NxF32)_pos[0];
		pos[1] = (NxF32)_pos[1];
		pos[2] = (NxF32)_pos[2];
		DebugSphere(pos,(NxF32)radius);
	}

	void getSpherePoint(NxF32 *point,NxU32 x,NxU32 y,const NxF32 *center,NxF32 radius,NxF32 scale,NxU32 stepCount)
	{
		if ( x == stepCount ) x = 0;
		if ( y == stepCount ) y = 0;
		NxF32 a = (NxF32)x*scale;
		NxF32 b = (NxF32)y*scale;
		NxF32 tpos[3];

		tpos[0] = sinf(a)*cosf(b);
		tpos[1] = sinf(a)*sinf(b);
		tpos[2] = cosf(a);

		point[0] = center[0]+tpos[0]*radius;
		point[1] = center[1]+tpos[1]*radius;
		point[2] = center[2]+tpos[2]*radius;

	}

	void DebugDetailedSphere(const NxF32 *pos,NxF32 radius,NxU32 stepCount)
	{
		const NxF32 pi = 3.1415926535897932384626433832795028841971693993751f;
		const NxF32 pi2 = pi*2.0f;

		NxF32 scale = pi2 / stepCount;

		for (NxU32 y=0; y<stepCount; y++)
		{
			for (NxU32 x=0; x<stepCount; x++)
			{
				NxF32 p1[3];
				NxF32 p2[3];
				NxF32 p3[3];
				NxF32 p4[3];

				getSpherePoint(p1,x,y,pos,radius,scale,stepCount);
				getSpherePoint(p2,x+1,y,pos,radius,scale,stepCount);
				getSpherePoint(p3,x+1,y+1,pos,radius,scale,stepCount);
				getSpherePoint(p4,x,y+1,pos,radius,scale,stepCount);

				DebugTri(p1,p2,p3);
				DebugTri(p1,p3,p4);
			}
		}
	}

	virtual NxBounds3 getBounds(void) const
	{
		if ( mBoundsChanged )
		{
			mBoundsChanged = false;
			mComputeBound = true;
		}
		return mRenderBounds;
	}

	virtual void  setFrameTime(NxF32 dtime)
	{
		mFrameTime = dtime;
	}

	virtual void  setPose(const NxMat34 &pose)
	{
		mPose = pose;
	}

    inline void initPrintText(void)
    {
        if ( !mInitPrintText )
        {
            mInitPrintText = true;
            mPrintText.Set(64,64,65536,"RenderDebug->PrintText",__FILE__,__LINE__);
        }
    }

	virtual NxI32 beginDrawGroup(const NxMat34 &pose)
	{
		pushRenderState();
		setRenderState(DebugRenderState::InfiniteLifeSpan,0xFFFFFF,0.0001f,0xFF0000,0.1f,mCurrentState.mRenderScale,mCurrentState.mTextScale);

		mBlockIndex++;
		mUseBlock = mBlockIndex;
        mCurrentBlock = MEMALLOC_NEW(BlockInfo);
        mCurrentBlock->mHashValue = mBlockIndex;
		mCurrentBlock->mPose = pose;

		mBlocksHash[ mBlockIndex ] = mCurrentBlock;

		return mBlockIndex;
	}

	virtual void  endDrawGroup(void)
	{
		mUseBlock = 0;
        mCurrentBlock = 0;
		popRenderState();
	}

	virtual void  setDrawGroupVisible(NxI32 blockId,bool state)
	{
		BlockInfoHash::iterator found = mBlocksHash.find( (NxU32)blockId );
		if ( found != mBlocksHash.end() )
		{
			BlockInfo *b = (*found).second;
			if ( b->mVisibleState != state )
			{
				b->mVisibleState = state;
				if ( b->mSolidTri )
				{
					SolidTri *t = b->mSolidTri;
					do
					{
						t->setVisible(state);
						t = t->mNext;
					} while ( t && t->getBlock() == blockId );
				}
				if ( b->mLineTri )
				{
					LineTri *t = b->mLineTri;
					do
					{
						t->setVisible(state);
						t = t->mNext;
					} while ( t && t->getBlock() == blockId );
				}
                if ( b->mPrintText )
                {
                    PrintText *pt = b->mPrintText;
                    do
                    {
                        pt->setVisible(state);
                        pt = pt->GetNext();
                    } while ( pt && pt->mBlock == b );
                }
			}
		}
	}

	virtual void  setDrawGroupPose(NxI32 blockId,const NxMat34 &pose)
	{
		BlockInfoHash::iterator found = mBlocksHash.find( (NxU32) blockId );
        if ( found != mBlocksHash.end() )
        {
			BlockInfo *b = (*found).second;
            if ( memcmp(&pose,&b->mPose,sizeof(NxMat34)) != 0 ) // if the pose has changed...
            {
                NxMat34 inverse;
                b->mPose.getInverse(inverse);
				NxMat34 tpose;
				tpose.multiply(pose,inverse);
				b->mPose = pose;

				if ( b->mSolidTri )
				{
					SolidTri *t = b->mSolidTri;
					do
					{
						t->transform(tpose);
						t = t->mNext;
					} while ( t && t->getBlock() == blockId );
				}
				if ( b->mLineTri )
				{
					LineTri *t = b->mLineTri;
					do
					{
						t->transform(tpose);
						t = t->mNext;
					} while ( t && t->getBlock() == blockId );
				}

            }

        }
	}

	virtual void  debugBound(const NxBounds3 &b)
	{
		DebugBound(&b.min.x,&b.max.x);
	}

	virtual void  eulerToMatrix(const NxVec3 &angles,NxMat33 &rot) // angles are in degrees
	{
        NxQuat q;
        eulerToQuat(angles,q);
        rot.fromQuat(q);
	}

	virtual void  eulerToQuat(const NxVec3 &angles,NxQuat &q) // angles are in degrees.
	{
    	NxF32 roll  = angles.x*0.5f*FM_DEG_TO_RAD;
    	NxF32 pitch = angles.y*0.5f*FM_DEG_TO_RAD;
    	NxF32 yaw   = angles.z*0.5f*FM_DEG_TO_RAD;

    	NxF32 cr = cosf(roll);
    	NxF32 cp = cosf(pitch);
    	NxF32 cy = cosf(yaw);

    	NxF32 sr = sinf(roll);
    	NxF32 sp = sinf(pitch);
    	NxF32 sy = sinf(yaw);

    	NxF32 cpcy = cp * cy;
    	NxF32 spsy = sp * sy;
    	NxF32 spcy = sp * cy;
    	NxF32 cpsy = cp * sy;

    	NxF32 x   = ( sr * cpcy - cr * spsy);
    	NxF32 y   = ( cr * spcy + sr * cpsy);
    	NxF32 z   = ( cr * cpsy - sr * spcy);
    	NxF32 w   = cr * cpcy + sr * spsy;
        q.setXYZW(x,y,z,w);

	}

    virtual void debugArc(const NxVec3 &center,const NxVec3 &p1,const NxVec3 &p2,NxF32 arrowSize,bool showRoot)
    {
        if ( showRoot )
        {
			pushRenderState();
			setCurrentColor(0xFFFFFF,0xFFFFFF);
		    debugLine(center,p1);
		    debugLine(center,p2);
			popRenderState();
        }

        NxVec3 v1 = p1-center;
        NxF32 d1 = v1.normalize();
        NxVec3 v2 = p2-center;
        NxF32 d2 = v2.normalize();
        NxF32 quat[4];

        fm_rotationArc(&v1.x,&v2.x,quat);

		NxQuat q1;
		q1.id();
        NxQuat q2;
        q2.setXYZW(quat);

        NxVec3 prev;

		NxI32 count = 0;
		for (NxF32 st=0; st<=(1.01f); st+=0.05f)
		{
			NxF32 d = ((d2-d1)*st)+d1;
			NxQuat q;
			q.slerp(st,q1,q2);
			NxMat34 m;
			m.M.fromQuat(q);
			m.t = center;
			NxVec3 t = v1*d;
			m.multiply(t,t);
            if ( st != 0 )
            {
				if ( count == 20 )
					debugRay(prev,t);
				else
					debugLine(prev,t);
            }
            prev = t;
			count++;
		}
    }

    virtual void debugThickArc(const NxVec3 &center,
								const NxVec3 &p1,
								const NxVec3 &p2,
								NxF32 thickness,
								bool showRoot)
    {
		pushRenderState();
		addToCurrentState(DebugRenderState::SolidShaded);

        if ( showRoot )
        {
			pushRenderState();
			setCurrentColor(0xFFFFFF,0xFFFFFF);
		    debugLine(center,p1);
		    debugLine(center,p2);
			popRenderState();
        }

        NxVec3 v1 = p1-center;
        NxF32 d1 = v1.normalize();
        NxVec3 v2 = p2-center;
        NxF32 d2 = v2.normalize();
        NxF32 quat[4];

        fm_rotationArc(&v1.x,&v2.x,quat);

		NxQuat q1;
		q1.id();
        NxQuat q2;
        q2.setXYZW(quat);


        NxVec3 prev;


		NxI32 count = 0;
		for (NxF32 st=0; st<=(1.01f); st+=0.05f)
		{
			NxF32 d = ((d2-d1)*st)+d1;
			NxQuat q;
			q.slerp(st,q1,q2);
			NxMat34 m;
			m.M.fromQuat(q);
			m.t = center;
			NxVec3 t = v1*d;
			m.multiply(t,t);
            if ( st != 0 )
            {
				if ( count == 20 )
					debugThickRay(prev,t,thickness);
				else
					debugCylinder(prev,t,thickness);
            }
            prev = t;
			count++;
		}
		popRenderState();
    }

	virtual void debugGraph(NxU32 numPoints, NxF32 * points, NxF32 graphMax, NxF32 graphXPos, NxF32 graphYPos, NxF32 graphWidth, NxF32 graphHeight, NxU32 colorSwitchIndex)
	{
		pushRenderState();
		addToCurrentState(DebugRenderState::ScreenSpace);
		addToCurrentState(DebugRenderState::NoZbuffer);
		setCurrentTextScale(0.1f);

		/* Argh solid rendering not supported!
		//graph background:
		const NxU32 black = 0;
		addToCurrentState(DebugRenderState::SolidShaded);
		setCurrentColor(black, black);
		NxVec3 bacgroundQuad[4] = { NxVec3(0,0,0), NxVec3(1,0,0), NxVec3(1,1,0), NxVec3(0,1,0) };
		debugPolygon(4, bacgroundQuad);
		*/
		const NxU32 green = 0x0000ff00;

		removeFromCurrentState(DebugRenderState::SolidShaded);


		NxF32 lastX = graphXPos;
		NxF32 lastY = graphYPos;
		for (NxU32 i = 0; i < numPoints; i++)
		{
			NxF32 pointY = points[i];
			pointY = graphYPos + pointY * graphHeight / graphMax;	//scale to screen
			NxF32 x = graphXPos + graphWidth * i / numPoints;

			if (colorSwitchIndex == i)
				setCurrentColor(mCurrentState.mArrowColor, mCurrentState.mColor);	//swap the colors


			debugLine(NxVec3(lastX,lastY,0), NxVec3(x,pointY,0));
			lastY = pointY;
			lastX = x;
		}

		//graph axes
		setCurrentColor(green, green);
		debugLine(NxVec3(graphXPos,graphYPos,0), NxVec3(graphXPos,graphYPos+graphHeight,0));	//screen space test line
		debugLine(NxVec3(graphXPos,graphYPos,0), NxVec3(graphXPos+graphWidth,graphYPos,0));

		popRenderState();


	}

    virtual void  pushRenderState(void)
    {
        assert( mStackIndex < RENDER_STATE_STACK_SIZE );
        if ( mStackIndex < RENDER_STATE_STACK_SIZE )
        {
            mRenderStateStack[mStackIndex] = mCurrentState;
            mStackIndex++;
        }
    }

    virtual void  popRenderState(void)
    {
        assert(mStackIndex);
        if ( mStackIndex > 0 )
        {
            mStackIndex--;
            mCurrentState = mRenderStateStack[mStackIndex];
        }
    }

	virtual void  setRenderState(NxU32 states,  // combination of render state flags
	                             NxU32 color, // base color
                                 NxF32 displayTime,
	                             NxU32 arrowColor, // secondary color, usually used for arrow head
                                 NxF32 arrowSize,
								 NxF32 renderScale,
                                 NxF32 textScale)
    {
        new ( &mCurrentState ) RenderState(states,color,displayTime,arrowColor,arrowSize,renderScale,textScale);
    }

	virtual void  setCurrentArrowSize(NxF32 arrowSize)
	{
        mCurrentState.mArrowSize = arrowSize;
	}

	virtual NxU32 getRenderState(NxU32 &color,NxF32 &displayTime,NxU32 &arrowColor,NxF32 &arrowSize,NxF32 &renderScale,NxF32 &textScale) const
    {
        color = mCurrentState.mColor;
        displayTime = mCurrentState.mDisplayTime;
        arrowColor = mCurrentState.mArrowColor;
        arrowSize = mCurrentState.mArrowSize;
		renderScale = mCurrentState.mRenderScale;
		return mCurrentState.mStates;
    }

	virtual void  addToCurrentState(DebugRenderState::Enum state) // OR this state flag into the current state.
	{
        mCurrentState.mStates|=state;
	}

	virtual void  removeFromCurrentState(DebugRenderState::Enum state) // Remove this bit flat from the current state
	{
        mCurrentState.mStates&=~state;
	}


	virtual void  setCurrentColor(NxU32 color,NxU32 arrowColor)
	{
        mCurrentState.mColor = color;
        mCurrentState.mArrowColor = arrowColor;
	}

	virtual void  setCurrentDisplayTime(NxF32 displayTime)
	{
        mCurrentState.mDisplayTime = displayTime;
	}

	virtual void  setCurrentState(NxU32 states)
	{
      mCurrentState.mStates = states;
	}

	virtual NxU32 getCurrentState(void) const
	{
        return mCurrentState.mStates;
	}

	virtual void DebugSphere(const NxF64 *_pos,NxF64 radius)
	{
        NxF32 pos[3];
        pos[0] = (NxF32)_pos[0];
        pos[1] = (NxF32)_pos[1];
        pos[2] = (NxF32)_pos[2];
        DebugSphere(pos,(NxF32)radius);
	}

	virtual void DebugHalfSphere(const NxF64 *_pos,NxF64 radius)
	{
        NxF32 pos[3];
        pos[0] = (NxF32)_pos[0];
        pos[1] = (NxF32)_pos[1];
        pos[2] = (NxF32)_pos[2];
        DebugSphere(pos,(NxF32)radius);
	}

	virtual void DebugPoint(const NxF64 *_pos,NxF64 radius)
	{
        NxF32 pos[3];
        pos[0] = (NxF32)_pos[0];
        pos[1] = (NxF32)_pos[1];
        pos[2] = (NxF32)_pos[2];
        DebugSphere(pos,(NxF32)radius);
	}

	virtual void DebugOrientedSphere(NxF64 radius,const NxF64 *_transform)
	{
        NxF32 transform[16];
        for (NxU32 i=0; i<16; i++)
            transform[i] = (NxF32)_transform[i];
        DebugOrientedSphere((NxF32)radius,transform);
	}

	virtual void DebugCylinder(const NxF64 *_p1,const NxF64 *_p2,NxF64 radius)
	{
		NxF32 p1[3];
		NxF32 p2[3];
		p1[0] = (NxF32)_p1[0];
		p1[1] = (NxF32)_p1[1];
		p1[2] = (NxF32)_p1[2];

		p2[0] = (NxF32)_p2[0];
		p2[1] = (NxF32)_p2[1];
		p2[2] = (NxF32)_p2[2];
		DebugCylinder(p1,p2,(NxF32)radius);
	}

	virtual void DebugPolygon(NxU32 pcount,const NxF64 *_points) 
	{
		#define MAX_POLY_POINTS 64
		NxF32 points[MAX_POLY_POINTS*3];
		if ( pcount < MAX_POLY_POINTS )
		{
			for (NxU32 i=0; i<pcount*3; i++)
				points[i] = (NxF32)_points[i];
			DebugPolygon(pcount,points);
		}

	}

	virtual void DebugOrientedLine(const NxF64 *_p1,const NxF64 *_p2,const NxF64 *_transform) 
	{
		NxF32 p1[3];
		NxF32 p2[3];
		p1[0] = (NxF32)_p1[0];
		p1[1] = (NxF32)_p1[1];
		p1[2] = (NxF32)_p1[2];

		p2[0] = (NxF32)_p2[0];
		p2[1] = (NxF32)_p2[1];
		p2[2] = (NxF32)_p2[2];
		NxF32 transform[16];
		for (NxU32 i=0; i<16; i++)
			transform[i] = (NxF32)_transform[i];
		DebugOrientedLine(p1,p2,transform);
	}

	virtual void DebugThickRay(const NxF64 *_p1,const NxF64 *_p2,NxF64 raySize) 
	{
		NxF32 p1[3];
		NxF32 p2[3];
		p1[0] = (NxF32)_p1[0];
		p1[1] = (NxF32)_p1[1];
		p1[2] = (NxF32)_p1[2];

		p2[0] = (NxF32)_p2[0];
		p2[1] = (NxF32)_p2[1];
		p2[2] = (NxF32)_p2[2];
		DebugThickRay(p1,p2,(NxF32)raySize);
	}

	virtual void DebugPlane(const NxF64 *_plane,NxF64 radius1,NxF64 radius2) 
	{
		NxF32 plane[4];
		plane[0] = (NxF32)_plane[0];
		plane[1] = (NxF32)_plane[1];
		plane[2] = (NxF32)_plane[2];
		plane[3] = (NxF32)_plane[3];
		DebugPlane(plane,(NxF32)radius1,(NxF32)radius2);
	}

	virtual void DebugCapsule(const NxF64 *_pos,NxF64 radius,NxF64 height) // assumes Y-up as the dominant axis, same as the PhysX SDK
	{
		NxF32 pos[3];
		pos[0] = (NxF32)_pos[0];
		pos[1] = (NxF32)_pos[1];
		pos[2] = (NxF32)_pos[2];
		DebugCapsule(pos,(NxF32)radius,(NxF32)height);
	}

	virtual void DebugOrientedCapsule(NxF64 radius,NxF64 height,const NxF64 *_transform) 
	{
		NxF32 transform[16];
		for (NxU32 i=0; i<16; i++)
			transform[i] = (NxF32)_transform[i];
		DebugOrientedCapsule((NxF32)radius,(NxF32)height,transform);
	}

	virtual void DebugAxes(const NxF64 *_transform,NxF64 distance,NxF64 brightness) 
	{
		NxF32 transform[16];
		for (NxU32 i=0; i<16; i++)
			transform[i] = (NxF32)_transform[i];
		DebugAxes(transform,(NxF32)distance,(NxF32)brightness);
	}

    virtual void debugOrientedText(const NxVec3 &pos,const NxQuat &rot,const char *fmt,...)
    {
	    char wbuff[8192];
        wbuff[8191] = 0;
	    _vsnprintf(wbuff,8191, fmt, (char *)(&fmt+1));
        NxMat34 mat;
        mat.M.fromQuat(rot);
        mat.t = pos;
        addText(mat,wbuff);
    }

    virtual void debugOrientedText(const NxMat34 &xform,const char *fmt,...)
    {
	    char wbuff[8192];
        wbuff[8191] = 0;
	    _vsnprintf(wbuff,8191, fmt, (char *)(&fmt+1));
        addText(xform,wbuff);
    }

    virtual void debugText(const NxVec3 &pos,const char *fmt,...)
    {
	    char wbuff[8192];
        wbuff[8191] = 0;
	    _vsnprintf(wbuff,8191, fmt, (char *)(&fmt+1));
        NxMat34 mat;
        mat.id();
        mat.t = pos;
        addText(mat,wbuff);
    }

    virtual void debugText(NxF32 x,NxF32 y,NxF32 z,const char *fmt,...)
    {
	    char wbuff[8192];
        wbuff[8191] = 0;
	    _vsnprintf(wbuff,8191, fmt, (char *)(&fmt+1));
        NxMat34 mat;
        mat.id();
        mat.t.set(x,y,z);
        addText(mat,wbuff);
    }

    void addText(const NxMat34 &xform,const char *text)
    {
        initPrintText();
        PrintText *pt = mPrintText.GetFreeLink();
        if ( pt )
        {
            if ( mCurrentBlock && mCurrentBlock->mPrintText == 0 )
            {
                mCurrentBlock->mPrintText = pt;
            }
            new ( pt ) PrintText(mCurrentState,xform,text,mCurrentBlock);
        }
    }

	virtual void  setCurrentTextScale(NxF32 textScale) 
	{
		mCurrentState.mTextScale = textScale;
	}

	virtual void setViewMatrix(const NxMat34 &view)
	{
        mViewMatrix = view;
        updateVP();
	}

	virtual void setProjectionMatrix(const NxMat34 &projection)
	{
        mProjectionMatrix = projection;
        updateVP();
	}

    void updateVP(void)
    {
    	mViewProjectionMatrix.multiply(mViewMatrix,mProjectionMatrix);
    	// grab the world-space eye position.
    	NxMat34 viewInverse;
    	if ( mViewMatrix.getInverse( viewInverse ) )
    		viewInverse.multiply( NxVec3( 0.0f, 0.0f, 0.0f ), mEyePos );
    }


	virtual void debugRect2d(NxF32 x1,NxF32 y1,NxF32 x2,NxF32 y2) 
	{
		NxVec3 points[4];
		points[0].set(x1,y1,0);
		points[1].set(x2,y1,0);
		points[2].set(x2,y2,0);
		points[3].set(x1,y2,0);
		debugPolygon(4,points);
	}

	virtual NxU32 getUpdateCount(void) const { return mUpdateCount; };

private:

	NxI32                             mBlockIndex;
	NxI32                             mUseBlock;
	bool                              mInitTris:1;
    bool                              mInitPrintText:1;
	bool                              mInitLines:1;
	mutable bool                      mComputeBound:1; // only compute bounds if someone is asking for it!
	mutable bool                      mBoundsChanged:1; // semaphore to indicate that bounding volume may have changed and should be recomputed

	NxMat34                           mPose;

	NxF32                             mFrameTime;
	Pool< LineTri >                   mDebugLines;       // the pool to work from.
	Pool< SolidTri >                  mDebugTris;       // the pool to work from.
    Pool< PrintText >                 mPrintText;

	NxU32                             mScreenWidth;
	NxU32                             mScreenHeight;

	NxMat34                           mViewMatrix;
	NxMat34                           mProjectionMatrix;
	NxMat34                           mViewProjectionMatrix;

	mutable NxF32                     mViewMatrix44[16];
	mutable NxF32                     mProjectionMatrix44[16];
	mutable NxF32                     mViewProjectionMatrix44[16];

	NxVec3                            mEyePos;

	NxU32                             mLineCount;
	NxU32                             mScreenCount;
    NxU32                             mSolidScreenCount;
	NxU32                             mSolidCount;
	NxU32                             mRenderCount;

	NxBounds3                         mBounds;
    BlockInfo                        *mCurrentBlock;
    BlockInfoHash                     mBlocksHash;

    NxU32                             mStackIndex;
    RenderState                       mCurrentState;
    RenderState                       mRenderStateStack[RENDER_STATE_STACK_SIZE];

    MyVectorFont                      mVectorFont;
    NxMat34                           mView;
    NxMat34                           mProjection;
	NxU32                             mUpdateCount;
	NxBounds3                         mRenderBounds;

};

};  // end of namespace

using namespace RENDER_DEBUG_NVSHARE;

namespace NVSHARE
{

RenderDebug *gRenderDebug=0;

RenderDebug * createRenderDebug(void)
{
    MyRenderDebug *m = MEMALLOC_NEW(MyRenderDebug);
    return static_cast< RenderDebug *>(m);
}

void          releaseRenderDebug(RenderDebug *rd)
{
    MyRenderDebug *m = static_cast< MyRenderDebug *>(rd);
    delete m;
}

}; // end of namespace