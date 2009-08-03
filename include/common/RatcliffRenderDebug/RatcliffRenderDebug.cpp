#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <math.h>
#include <new>

#pragma warning(disable:4996)
#pragma warning(disable:4100)
#pragma warning(disable:4189)

#include <math.h>

namespace RENDER_DEBUG
{

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

unsigned char g_font[6350] = {
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

static inline float Dot(const float *A,const float *B)
{
	return A[0]*B[0] + A[1]*B[1] + A[2]*B[2];
}

static inline void Cross(float *result,const float *A,const float *B)
{
	result[0] = A[1]*B[2] - A[2]*B[1];
	result[1] = A[2]*B[0] - A[0]*B[2];
	result[2] = A[0]*B[1] - A[1]*B[0];
}

static inline float Normalize(float *r)
{
	float d = sqrtf(r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);
	float recip = 1.0f / d;
	r[0]*=recip;
	r[1]*=recip;
	r[2]*=recip;
	return d;
}

static inline float Magnitude(const float *v)
{
	return sqrtf( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
}

void computeLookAt(const float *eye,const float *look,const float *upVector,float *matrix)
{

	float row2[3];

	row2[0] = look[0] - eye[0];
	row2[1] = look[1] - eye[1];
	row2[2] = look[2] - eye[2];

	Normalize(row2);

	float row0[3];
	float	row1[3];

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


static inline const unsigned char * getUShort(const unsigned char *data,unsigned short &v)
{
    const unsigned short *src = (const unsigned short *)data;
    v = src[0];
    data+=sizeof(unsigned short);
	return data;
}

static inline const unsigned char * getUint(const unsigned char *data,unsigned int &v)
{
    const unsigned int *src = (const unsigned int *)data;
    v = src[0];
    data+=sizeof(unsigned int);
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

  const unsigned char * init(const unsigned char *data,const float *vertices)
  {
    data = getUShort(data,mIndexCount);
    mIndices = (const unsigned short *)data;
    data+=(mIndexCount*sizeof(unsigned short));
    for (unsigned int i=0; i<mIndexCount; i++)
    {
        unsigned int index = mIndices[i];
        const float *vertex = &vertices[index*2];
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

  void vputc(const float *vertices,RatcliffRenderDebug *debug,const JrMat44 &pose,float textScale,float &x,float &y)
  {
    if ( mIndices )
    {
        unsigned int lineCount = mIndexCount/2;
		float spacing = (mX2-mX1)+0.05f;
        for (unsigned int i=0; i<lineCount; i++)
        {
            unsigned short i1 = mIndices[i*2+0];
            unsigned short i2 = mIndices[i*2+1];

            const float *v1 = &vertices[i1*2];
            const float *v2 = &vertices[i2*2];

			NxVec3 p1(v1[0]+x,v1[1]+y,0);
			NxVec3 p2(v2[0]+x,v2[1]+y,0);
			p1*=textScale;
			p2*=textScale;
			pose.multiply(p1,p1);
			pose.multiply(p2,p2);

            debug->debugLine(p1,p2);

        }
		x+=spacing;
    }
    else
    {
        x+=0.1f;
    }
  }

  float getWidth(void) const
  {
    float ret = 0.1f;
    if ( mIndexCount > 0 )
    {
        ret = (mX2-mX1)+0.05f;
    }
    return ret;
  }


  float          mX1;
  float          mX2;
  float          mY1;
  float          mY2;
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

  void initFont(const unsigned char *font)
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
            unsigned int vsize = sizeof(float)*mVcount*2;
            mVertices = (float *)font;
			font+=vsize;
            for (unsigned int i=0; i<mCount; i++)
            {
                unsigned char c = *font++;
                font = mCharacters[c].init(font,mVertices);
            }
        }
    }
  }

  virtual void vprintf(const char *buffer,RatcliffRenderDebug *debug,const JrMat44 &pose,float textScale,bool centered)
  {
    const char *scan = buffer;
	float x = 0;
	float y = 0;
    if ( centered )
    {
        float wid=0;
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
  unsigned int    mVersion;
  unsigned int    mVcount;
  unsigned int    mCount;
  float          *mVertices;
  unsigned int    mIcount;
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

  RenderState(unsigned int s,unsigned int c,float d,unsigned int a,float as,float rs,float ts)
  {
    mStates = s;
    mColor = c;
    mDisplayTime = d;
    mArrowColor = a;
    mArrowSize = as;
    mRenderScale = rs;
    mTextScale = ts;
  }

  inline bool isScreen(void) const { return (mStates & NxApexDebugRenderState::ScreenSpace); };
  inline bool isUseZ(void) const { return !(mStates & NxApexDebugRenderState::NoZbuffer); };
  inline bool isSolid(void) const { return (mStates & (NxApexDebugRenderState::SolidShaded | NxApexDebugRenderState::SolidWireShaded)) ? true : false; };
  inline bool isClockwise(void) const { return !(mStates & NxApexDebugRenderState::CounterClockwise); };
  inline bool isWireframeOverlay(void) const { return (mStates & NxApexDebugRenderState::SolidWireShaded) ? true : false; };
  inline float getDisplayTime(void) const
  {
    return (mStates & NxApexDebugRenderState::InfiniteLifeSpan) ? NX_MAX_F32 : mDisplayTime;
  }

  inline bool isCentered(void) const { return (mStates & NxApexDebugRenderState::CenterText) ? true : false; };
  inline bool isCameraFacing(void) const { return (mStates & NxApexDebugRenderState::CameraFacing) ? true : false; };
  inline bool isCounterClockwise(void) const { return (mStates & NxApexDebugRenderState::CounterClockwise) ? true : false; };



  unsigned int mStates;
  unsigned int mColor;
  float mDisplayTime;
  unsigned int mArrowColor;
  float mArrowSize;
  float mRenderScale;
  float mTextScale;
};



#define MAX_BUFFER_STACK 2048 // maximum number of vertices we are allowed to create on the stack.

class RenderDebugSolidVertex
{
public:
	float	mPos[3];
	float mNormal[3];
	unsigned int mColor;
};

class RenderDebugVertex
{
public:

	float mPos[3];
	unsigned int mColor;
};


const float FM_PI = 3.1415926535897932384626433832795028841971693993751f;
const float FM_DEG_TO_RAD = ((2.0f * FM_PI) / 360.0f);
const float FM_RAD_TO_DEG = (360.0f / (2.0f * FM_PI));

inline float degToRad(float deg) { return deg*FM_DEG_TO_RAD; };
inline float radToDeg(float rad) { return rad*FM_RAD_TO_DEG; };

inline void fm_cross(float *cross,const float *a,const float *b)
{
	cross[0] = a[1]*b[2] - a[2]*b[1];
	cross[1] = a[2]*b[0] - a[0]*b[2];
	cross[2] = a[0]*b[1] - a[1]*b[0];
}

inline float fm_distToPlane(const float *plane,const float *p) // computes the distance of this point from the plane.
{
	return p[0]*plane[0]+p[1]*plane[1]+p[2]*plane[2]+plane[3];
}

inline float fm_dot(const float *p1,const float *p2)
{
	return p1[0]*p2[0]+p1[1]*p2[1]+p1[2]*p2[2];
}

inline float fm_normalize(float *n) // normalize this vector
{
	float dist = (float)sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
	if ( dist > 0.0000001f )
	{
		float mag = 1.0f / dist;
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

inline void fm_quatToMatrix(const float *quat,float *matrix) // convert quaterinion rotation to matrix, zeros out the translation component.
{

	float xx = quat[0]*quat[0];
	float yy = quat[1]*quat[1];
	float zz = quat[2]*quat[2];
	float xy = quat[0]*quat[1];
	float xz = quat[0]*quat[2];
	float yz = quat[1]*quat[2];
	float wx = quat[3]*quat[0];
	float wy = quat[3]*quat[1];
	float wz = quat[3]*quat[2];

	matrix[0*4+0] = 1 - 2 * ( yy + zz );
	matrix[1*4+0] =     2 * ( xy - wz );
	matrix[2*4+0] =     2 * ( xz + wy );

	matrix[0*4+1] =     2 * ( xy + wz );
	matrix[1*4+1] = 1 - 2 * ( xx + zz );
	matrix[2*4+1] =     2 * ( yz - wx );

	matrix[0*4+2] =     2 * ( xz - wy );
	matrix[1*4+2] =     2 * ( yz + wx );
	matrix[2*4+2] = 1 - 2 * ( xx + yy );

	matrix[3*4+0] = matrix[3*4+1] = matrix[3*4+2] = (float) 0.0f;
	matrix[0*4+3] = matrix[1*4+3] = matrix[2*4+3] = (float) 0.0f;
	matrix[3*4+3] =(float) 1.0f;

}


inline float fm_computePlane(const float *A,const float *B,const float *C,float *n) // returns D
{
	float vx = (B[0] - C[0]);
	float vy = (B[1] - C[1]);
	float vz = (B[2] - C[2]);

	float wx = (A[0] - B[0]);
	float wy = (A[1] - B[1]);
	float wz = (A[2] - B[2]);

	float vw_x = vy * wz - vz * wy;
	float vw_y = vz * wx - vx * wz;
	float vw_z = vx * wy - vy * wx;

	float mag = sqrt((vw_x * vw_x) + (vw_y * vw_y) + (vw_z * vw_z));

	if ( mag < 0.000001f )
	{
		mag = 0;
	}
	else
	{
		mag = 1.0f/mag;
	}

	float x = vw_x * mag;
	float y = vw_y * mag;
	float z = vw_z * mag;


	float D = 0.0f - ((x*A[0])+(y*A[1])+(z*A[2]));

	n[0] = x;
	n[1] = y;
	n[2] = z;

	return D;
}


inline void  fm_transform(const float matrix[16],const float v[3],float t[3]) // rotate and translate this point
{
	if ( matrix )
	{
		float tx = (matrix[0*4+0] * v[0]) +  (matrix[1*4+0] * v[1]) + (matrix[2*4+0] * v[2]) + matrix[3*4+0];
		float ty = (matrix[0*4+1] * v[0]) +  (matrix[1*4+1] * v[1]) + (matrix[2*4+1] * v[2]) + matrix[3*4+1];
		float tz = (matrix[0*4+2] * v[0]) +  (matrix[1*4+2] * v[1]) + (matrix[2*4+2] * v[2]) + matrix[3*4+2];
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

inline void  fm_setTranslation(const float *translation,float *matrix)
{
	matrix[12] = translation[0];
	matrix[13] = translation[1];
	matrix[14] = translation[2];
}

// Reference, from Stan Melax in Game Gems I
//  Quaternion q;
//  vector3 c = CrossProduct(v0,v1);
//  float   d = DotProduct(v0,v1);
//  float   s = (float)sqrt((1+d)*2);
//  q.x = c.x / s;
//  q.y = c.y / s;
//  q.z = c.z / s;
//  q.w = s /2.0f;
//  return q;
inline void fm_rotationArc(const float *v0,const float *v1,float *quat)
{
	float cross[3];

	fm_cross(cross,v0,v1);
	float d = fm_dot(v0,v1);
	float s = sqrt((1+d)*2);
	float recip = 1.0f / s;

	quat[0] = cross[0] * recip;
	quat[1] = cross[1] * recip;
	quat[2] = cross[2] * recip;
	quat[3] = s * 0.5f;

}

class MPoolExtra 
{
public:
	MPoolExtra(size_t mlen,const char *poolType,const char *file,int lineno)
	{
		mPoolType = poolType;
		mNext = 0;

		mData = (char *)MEMALLOC_MALLOC((sizeof(char)*mlen) );

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

	void Set(int startcount,int growcount,int maxitems,const char *poolType,const char *file,int lineno)
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
			mData =  new MPoolExtra(sizeof(Type)*mStartCount,mPoolType,mFile,mLineNo);
			Type *data = (Type *) mData->mData;
			{
				Type *t = (Type *)mData->mData;
				for (int i=0; i<mStartCount; i++)
				{
					new ( t ) Type;
					t++;
				}
			}
			mFree = data;
			mHead = 0;
			mTail = 0;
			int i;
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

	int Begin(void)
	{
		mCurrent = mHead;
		return mUsedCount;
	};

	int GetUsedCount(void) const { return mUsedCount; };
	int GetFreeCount(void) const { return mFreeCount; };

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
			mData  = new MPoolExtra(sizeof(Type)*mGrowCount,mPoolType,mFile,mLineNo);
			{
				Type *t = (Type *)mData->mData;
				for (int i=0; i<mGrowCount; i++)
				{
					new ( t ) Type;
					t++;
				}
			}
			mData->mNext = pe; // he points to the old one.
			// done..memory allocated and added to singly linked list.

			Type *data = (Type *) mData->mData;
			mFree = data;     // new head of free list.
			int i;
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
	int        mMaxItems;
	int        mGrowCount;
	int        mStartCount;
	int        mCurrentCount; // this is total allocated, not free/used

	Type       *mCurrent;
	MPoolExtra *mData;
	Type        *mHead;
	Type        *mTail;
	Type        *mFree;
	int        mUsedCount;
	int        mFreeCount;
	int        mMaxUsed;
	const char *mPoolType;
	const char *mFile;
	int         mLineNo;
};


const float debug_cylinder[32*9] =
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

const float debug_sphere[32*9] =
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

const float debug_halfsphere[16*9] =
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

const float debug_point[3*6] =
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

  PrintText(const RenderState &rs,const JrMat44 &xform,const char *text,BlockInfo *block)
  {
    mBlock = block;
    unsigned int slen = (unsigned int)strlen(text);
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

  bool process(float dtime,RatcliffRenderDebug *debug,MyVectorFont &font,JrMat44 &pose,float textScale,bool centered)
  {
    debug->removeFromCurrentState(NxApexDebugRenderState::InfiniteLifeSpan);
    debug->setCurrentDisplayTime(0.0001f);

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
  float       mLifeTime;
  BlockInfo  *mBlock;
  RenderState mRenderState;
  JrMat44     mTransform;
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

	void Set(const float *p1,
			 const float *p2,
			 const float *p3,
			 unsigned int color,
			 float lifespan,
			 float renderScale,
			 int blockIndex,
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


	void Set(const float *p1,
					 const float *p2,
					 const float *p3,
					 const float *n1,
					 const float *n2,
					 const float *n3,
					 unsigned int color,
					 float lifespan,
					 float renderScale,
					 int blockIndex,
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


	inline RenderDebugSolidVertex * RenderSolid(float dtime,RenderDebugSolidVertex *current,bool &remove)
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

	inline int getBlock(void) const { return mBlock; };

	inline void SetNext(SolidTri *tri) { mNext = tri; };
	inline void SetPrevious(SolidTri *tri) { mPrevious = tri; };

	inline SolidTri * GetNext(void) { return mNext; };
	inline SolidTri * GetPrevious(void) { return mPrevious; };

	inline void transform(const JrMat44 &mat,float *pos)
	{
		NxVec3 *p = (NxVec3 *)pos;
		NxVec3 *t = (NxVec3 *)pos;
		mat.multiply(*p,*t);
	}

	inline void rtransform(const JrMat44 &mat,float *pos)
	{
		NxVec3 *p = (NxVec3 *)pos;
		NxVec3 *t = (NxVec3 *)pos;
		mat.M.multiply(*p,*t);
	}


	inline void transform(const JrMat44 &mat)
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
	float           mLifeSpan;
	float           mN1[3];
	float           mN2[3];
	float           mN3[3];
	int           mBlock;
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

	inline void Set(const float *p1,
					 const float *p2,
					 const float *p3,
					 unsigned int color,
					 float lifespan,
					 LineTriFlag flag,
					 float renderScale,
					 int blockIndex)
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


	inline RenderDebugVertex * RenderLine(float dtime,RenderDebugVertex *current,bool &remove,bool flush)
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
			if ( mLifeSpan < 0 )
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

	int getBlock(void) const { return mBlock; };

	inline void SetNext(LineTri *tri) { mNext = tri; };
	inline void SetPrevious(LineTri *tri) { mPrevious = tri; };

	LineTri * GetNext(void) { return mNext; };
	LineTri * GetPrevious(void) { return mPrevious; };

	void transform(const JrMat44 &mat,float *pos)
	{
		NxVec3 *p = (NxVec3 *)pos;
		NxVec3 *t = (NxVec3 *)pos;
		mat.multiply(*p,*t);
	}

	void transform(const JrMat44 &mat)
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
	float                 mLifeSpan;
	unsigned int                 mFlags;
	int                 mBlock;
};

class BlockInfo 
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
  unsigned int getHashValue(void) const
  {
	  return mHashValue;
  }
  JrMat44    mPose;  // transform for block of data
  SolidTri  *mSolidTri;
  LineTri   *mLineTri;
  PrintText *mPrintText;

  unsigned int    mHashValue;
  bool     mVisibleState;
  bool	   mScreenSpace;
};

class JohnRatcliffRenderDebug
{
public:
	JohnRatcliffRenderDebug(void)
	{
		mStackIndex = 0;
		mLines = 0;
        mScreen = 0;
		mSolid = 0;
        mSolidScreen = 0;
		mLineResource = 0;
        mScreenResource = 0;
		mMaxLineBuffers = 0;
        mMaxScreenBuffers = 0;
		mSolidResource = 0;
        mSolidScreenResource = 0;
		mMaxSolidBuffers = 0;
		mMaxSolidScreenBuffers = 0;
		mResources = 0;
        mResourceTypes = 0;
		mLinePoints = 0;
        mScreenPoints = 0;
		mSolidPoints = 0;
        mSolidScreenPoints = 0;
		mDesc = desc;
		mFrameTime = 1.0f/60.0f;
		mApexSDK = a;
		mWireFrameMaterial = INVALID_RESOURCE_ID;
		mSolidShadedMaterial = INVALID_RESOURCE_ID;
		mManager = 0;
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
  		 mDebugTris.Set(512,1024,2000000,"RenderDebug->SolidTri",__FILE__,__LINE__); // initialize the wireframe triangle pool.
         mPrintText.Set(512,1024,2000000,"RenderDebug->PrintText",__FILE__,__LINE__);
	    mDebugLines.Set(512,1024,2000000,"RenderDebug->LineTri",__FILE__,__LINE__); // initialize the wireframe triangle pool.

	}

	void setApexSDK(ApexSDK *a)
	{
		mApexSDK = a;
	}

	float getRenderScale(void)
	{
		return mCurrentState.mRenderScale;
	}

	void  setRenderScale(float scale)
	{
		mCurrentState.mRenderScale = scale;
	}

	virtual unsigned int getDebugColor(bool reset_index=false)
	{
		unsigned int colors[8] =
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

		static unsigned int cindex = 0;

		if ( reset_index )
			cindex = 0;

		unsigned int color = colors[cindex];
		cindex++;
		if ( cindex == 8 ) cindex = 0;

		return color;
	}


	virtual ~JohnRatcliffRenderDebug(void)
	{
        {
            int pcount = mPrintText.Begin();
            for (int i=0; i<pcount; i++)
            {
                PrintText *pt = mPrintText.GetNext();
                pt->~PrintText();
                mPrintText.Release(pt);
            }
        }

	}


private:

	void addSolidScreen(RenderDebugSolidVertex *vertices,unsigned int count,NxUserRenderResourceManager &rmanage)
    {
    }

	void addSolid(RenderDebugSolidVertex *vertices,unsigned int count,NxUserRenderResourceManager &rmanage,NxBounds3 *b)
	{
	}

	void addWire(RenderDebugVertex *vertices,unsigned int count,bool /*zpass*/,NxUserRenderResourceManager &rmanage,NxBounds3 *b)
	{
	}


	void addScreen(RenderDebugVertex *vertices,unsigned int count,bool /*zpass*/,NxUserRenderResourceManager &rmanage)
	{
	}

	virtual void render(float dtime,RatcliffRenderDebugInterface *iface)
	{
		int tricount = mDebugTris.Begin();
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

			stop    = &points[mDesc.mBufferSize-3];
			start = current = points;

            // solid shaded vertices in screen space
			RenderDebugSolidVertex spoints[MAX_BUFFER_STACK];
			RenderDebugSolidVertex *s_start   = 0;
			RenderDebugSolidVertex *s_stop    = 0;
			RenderDebugSolidVertex *s_current = 0;

			s_stop    = &spoints[mDesc.mBufferSize-3];
			s_start = s_current = spoints;

			for (int i=0; i<tricount; i++)
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
							BlockInfo *b;
							if ( mBlocksHash.exists((unsigned int)tri->mBlock,b) )
							{
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
						unsigned int count = (unsigned int)(s_current - s_start);
						addSolidScreen(s_start,count,rmanage );
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
							BlockInfo *b;
							if ( mBlocksHash.exists((unsigned int)tri->mBlock,b) )
							{
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
						unsigned int count = (unsigned int)(current - start);
						addSolid(start,count,rmanage, mComputeBound ? &mRenderBounds : 0 );
						current = start;
					}
                }
			}
			unsigned int count = (unsigned int)(s_current - s_start);
			if ( count )
			{
				addSolidScreen(s_start,count,rmanage );
			}
			count = (unsigned int)(current - start);
			if ( count )
			{
				addSolid(start,count,rmanage, mComputeBound ? &mRenderBounds : 0 );
			}
		}

		// Process text after solid shaded has been processed.
		{
			int tcount = mPrintText.Begin();
			for (int i=0; i<tcount; i++)
			{
				PrintText *pt = mPrintText.GetNext();

				pushRenderState();
				mCurrentState = pt->mRenderState;
				JrMat44 pose  = pt->mTransform;
				if ( pt->mBlock )
				{
					pose.multiply(pose,pt->mBlock->mPose);
				}

				if ( mCurrentState.isCameraFacing() )
				{
					//void computeLookAt(const float *eye,const float *look,const float *upVector,float *matrix)
					NxVec3 upVector(0,1,0);
					float matrix[16];
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

			stop    = &linePoints[mDesc.mBufferSize-6];
			current = start = linePoints;

			mLineCount   = 0;

			RenderDebugVertex screenPoints[MAX_BUFFER_STACK];
			RenderDebugVertex *screen_stop    = 0;
			RenderDebugVertex *screen_current = 0;
			RenderDebugVertex *screen_start   = 0;

			screen_stop    = &screenPoints[mDesc.mBufferSize-6];
			screen_current = screen_start = screenPoints;

			mScreenCount   = 0;


			for (int i=0; i<tricount; i++)
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
							BlockInfo *b;
							if ( mBlocksHash.exists((unsigned int)tri->mBlock,b) )
							{
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
						unsigned int count = (unsigned int)(screen_current - screen_start);
						addScreen(screen_start,count,false,rmanage);
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
							BlockInfo *b;
							if ( mBlocksHash.exists((unsigned int)tri->mBlock,b) )
							{
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
						unsigned int count = (unsigned int)(current - start);
						addWire(start,count,true,rmanage,mComputeBound ? &mRenderBounds : 0);
						current = start;
					}
				}
			}

			unsigned int count = (unsigned int)(current - start);
			if ( count )
			{
				addWire(start,count,true,rmanage, mComputeBound ? &mRenderBounds : 0);
			}
			count = (unsigned int)(screen_current - screen_start);
			if ( count )
			{
				addScreen(screen_start,count,false,rmanage);
			}

		}
	}

	virtual void debugTri(const float *p1,const float *p2,const float *p3)
	{
        debugTri(p1,p2,p3,0,0,0);
	}

	virtual void debugTri(const float *p1,const float *p2,const float *p3,const float *n1,const float *n2,const float *n3)
	{
		ApexRenderable::renderDataLock();

        bool wireframe = true;
        unsigned int wireColor = mCurrentState.mColor;

        if ( mCurrentState.isSolid() )
        {
    		initDebugTris();
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
    		initDebugLines();
    		LineTri *tri = mDebugLines.GetFreeLink(); // pull a free triangle from the pool
    		if ( tri )
    		{
                if ( mCurrentBlock && mCurrentBlock->mLineTri == 0 )
                {
                    mCurrentBlock->mLineTri = tri;
                }
    			mBoundsChanged = true;
                unsigned int flags = mCurrentState.isUseZ() ?  LTF_USEZ : LTF_NONE;
                flags|= mCurrentState.isScreen() ? LTF_SCREEN : 0;
    			tri->Set(p1,p2,p3,mCurrentState.mColor,mCurrentState.getDisplayTime(),(RENDER_DEBUG::LineTriFlag)flags, mCurrentState.mRenderScale, mUseBlock ); // set condition of the triangle
    			if ( mCurrentBlock )
    			{
    				tri->transform(mCurrentBlock->mPose);
    			}

    		}
        }
		ApexRenderable::renderDataUnLock();
	}

	void adjust(const float *p,float *d,float arrowSize)
	{
		d[0] = d[0]*arrowSize+p[0];
		d[1] = d[1]*arrowSize+p[1];
		d[2] = d[2]*arrowSize+p[2];
	}

	void DebugRay(const float *p1,const float *p2)
	{
		DebugLine(p1,p2);

		float dir[3];

		dir[0] = p2[0] - p1[0];
		dir[1] = p2[1] - p1[1];
		dir[2] = p2[2] - p1[2];

		float mag = fm_normalize(dir);
		float arrowSize = mCurrentState.mArrowSize;

		if ( arrowSize > (mag*0.2f) )
		{
			arrowSize = mag*0.2f;
		}

		float ref[3] = { 0, 1, 0 };

		float quat[4];

		fm_rotationArc(ref,dir,quat);

		float matrix[16];
		fm_quatToMatrix(quat,matrix);
		fm_setTranslation(p2,matrix);


		unsigned int pcount = 0;
		float points[24*3];
		float *dest = points;

		for (float a=30; a<=360; a+=30)
		{
			float r = a*FM_DEG_TO_RAD;
			float x = cosf(r)*arrowSize;
			float y = sinf(r)*arrowSize;

			dest[0] = x;
			dest[1] = -3*arrowSize;
			dest[2] = y;
			dest+=3;
			pcount++;
		}

		float *prev = &points[(pcount-1)*3];
		float *p = points;
		float center[3] = { 0, -2.5f*arrowSize, 0 };
		float top[3]    = { 0, 0, 0 };

		float _center[3];
		float _top[3];

		fm_transform(matrix,center,_center);
		fm_transform(matrix,top,_top);

        pushRenderState();
        addToCurrentState(NxApexDebugRenderState::SolidWireShaded);
        setCurrentColor(mCurrentState.mArrowColor,0xFFFFFF);

		for (unsigned int i=0; i<pcount; i++)
		{

			float _p[3];
			float _prev[3];
			fm_transform(matrix,p,_p);
			fm_transform(matrix,prev,_prev);

			DebugTri(_p,_center,_prev);
			DebugTri(_prev,_top,_p);

			prev = p;
			p+=3;
		}
        popRenderState();
	}

	virtual void DebugLine(const float *p1,const float *p2)
	{
		ApexRenderable::renderDataLock();
		initDebugLines();
		LineTri *tri = mDebugLines.GetFreeLink(); // pull a free triangle from the pool
		if ( tri )
		{
            if ( mCurrentBlock && mCurrentBlock->mLineTri == 0 )
            {
                mCurrentBlock->mLineTri = tri;
            }
			mBoundsChanged = true;
			unsigned int flags = mCurrentState.isUseZ() ?  LTF_USEZ : LTF_NONE;
			flags|= mCurrentState.isScreen() ? LTF_SCREEN : 0;
			tri->Set(p1,p2,0, mCurrentState.mColor | 0xFF000000, mCurrentState.getDisplayTime(),(RENDER_DEBUG::LineTriFlag)flags, mCurrentState.mRenderScale, mUseBlock  ); // set condition of the triangle
			if ( mCurrentBlock )
			{
				tri->transform(mCurrentBlock->mPose);
			}

		}
		ApexRenderable::renderDataUnLock();
	}
	void DebugOrientedLine(const float *p1,const float *p2,const float *transform)
	{
		float t1[3];
		float t2[3];
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

	virtual void DebugBound(const float *bmin,const float *bmax)
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
        float bmin[3];
        float bmax[3];
        float pos[3];
        float rot[4];
        bmin[0] = (float)_bmin[0];
        bmin[1] = (float)_bmin[1];
        bmin[2] = (float)_bmin[2];
        bmax[0] = (float)_bmax[0];
        bmax[1] = (float)_bmax[1];
        bmax[2] = (float)_bmax[2];
        pos[0] = (float)_pos[0];
        pos[1] = (float)_pos[1];
        pos[2] = (float)_pos[2];
        rot[0] = (float)_rot[0];
        rot[1] = (float)_rot[1];
        rot[2] = (float)_rot[2];
        rot[3] = (float)_rot[3];
        DebugOrientedBound(bmin,bmax,pos,rot);
    }

	virtual void DebugOrientedBound(const float *bmin,const float *bmax,const float *pos,const float *rot)
	{
		JrMat44 m;
		NxQuat q;
		q.setXYZW(rot[0],rot[1],rot[2],rot[3]);
		m.M.fromQuat(q);
		NxVec3 p(pos);
		m.t = p;
		float matrix[16];
		m.getColumnMajor44(matrix);
		DebugOrientedBound(bmin,bmax,matrix);
	}

	virtual void DebugOrientedBound(const NxF64 *_bmin,const NxF64 *_bmax,const NxF64 *_xform)
    {
        float bmin[3];
        float bmax[3];
        bmin[0] = (float) _bmin[0];
        bmin[1] = (float)_bmin[1];
        bmin[2] = (float)_bmin[2];
        bmax[0] = (float)_bmax[0];
        bmax[1] = (float)_bmax[1];
        bmax[2] = (float)_bmax[2];
        float xform[16];
        for (unsigned int i=0; i<16; i++) xform[i] = (float)_xform[0];
        DebugOrientedBound(bmin,bmax,xform);

    }

	virtual void DebugOrientedBound(const float *bmin,const float *bmax,const float *xform)
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

		JrMat44 m;
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
        float sides[3];
        float transform[16];
        sides[0] = (float) _sides[0];
        sides[1] = (float) _sides[1];
        sides[2] = (float) _sides[2];
        for (unsigned int i=0; i<16; i++)
            transform[i] = (float)_transform[i];

    }

	virtual void DebugOrientedBound(const float *sides,const float *transform)
	{
		NxVec3 box[8];

		float bmin[3];
		float bmax[3];

		bmin[0] = -sides[0]*0.5f;
		bmin[1] = -sides[1]*0.5f;
		bmin[2] = -sides[2]*0.5f;

		bmax[0] = +sides[0]*0.5f;
		bmax[1] = +sides[1]*0.5f;
		bmax[2] = +sides[2]*0.5f;

        DebugOrientedBound(bmin,bmax,transform);

	}

	virtual void DebugSphere(const float *pos,float radius)
	{
		const float *source = debug_sphere;

		for (int i=0; i<32; i++)
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

	virtual void DebugHalfSphere(const float *pos,float radius)
	{
		const float *source = debug_halfsphere;

		for (int i=0; i<16; i++)
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

	virtual void DebugPoint(const float *pos,float radius)
	{
		const float *source = debug_point;

		for (int i=0; i<3; i++)
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

	virtual void DebugOrientedSphere(float radius,const float *transform)
	{
		const float *source = debug_sphere;

		JrMat44 m;
		m.setColumnMajor44(transform);

		for (int i=0; i<32; i++)
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

	int ClampColor(int c)
	{
		if ( c < 0 )
			c = 0;
		else if ( c > 255 )
			c = 255;
		return c;
	};

	unsigned int GetColor(float r,float g,float b,float brightness)
	{
		int red   = int(r*brightness*255.0f);
		int green = int(g*brightness*255.0f);
		int blue  = int(b*brightness*255.0f);
		red   = ClampColor(red);
		green = ClampColor(green);
		blue  = ClampColor(blue);
		unsigned int color = 0xFF000000 | (red<<16) | (green<<8) | blue;
		return color;
	}

	virtual void DebugAxes(const float *xform,float distance,float brightness)
	{
		JrMat44 m;
		m.setColumnMajor44(xform);

		unsigned int red    = GetColor(1,0,0,brightness);
		unsigned int green  = GetColor(0,1,0,brightness);
		unsigned int blue   = GetColor(0,0,1,brightness);
		unsigned int yellow = GetColor(1,1,0,brightness);

		NxVec3 px(distance*2,0,0);
		NxVec3 py(0,distance*1.5f,0);
		NxVec3 pz(0,0,distance);

		m.multiply(px,px);
		m.multiply(py,py);
		m.multiply(pz,pz);

		pushRenderState();
		addToCurrentState(NxApexDebugRenderState::SolidWireShaded);
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

	virtual void reset(int blockIndex)
	{
		ApexRenderable::renderDataLock();

		mBoundsChanged = true;

        if ( blockIndex > 0 ) // reseting a specific block!
        {
            BlockInfo *b;
            if ( mBlocksHash.exists((unsigned int)blockIndex,b) )
            {
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
					mPrintText.Release(pt);
                    PrintText *n = pt->GetNext();
                    pt = n;
                }
                mBlocksHash.remove(b);
                mBlocks.destroy(b);
            }
        }
        else
        {
     		int count = mDebugTris.Begin();
     		for (int i=0; i<count; i++)
     		{
     			SolidTri *wt = mDebugTris.GetNext();
     			if ( wt->getBlock() == blockIndex || blockIndex == -1 )
     			  mDebugTris.Release(wt);
     		}
     		count = mDebugLines.Begin();
     		for (int i=0; i<count; i++)
     		{
     			LineTri *wt = mDebugLines.GetNext();
     			if ( wt->getBlock() == blockIndex || blockIndex == -1 )
     			  mDebugLines.Release(wt);
     		}
        }
		ApexRenderable::renderDataUnLock();
	}

#if 0 // TODO THIS SHOULD BE DEAD CODE!
	void DebugSolidTri(const float *p1,const float *p2,const float *p3,const float *n1,const float *n2,const float *n3,unsigned int color,float duration)
	{
		ApexRenderable::renderDataLock();
		initDebugTris();
		SolidTri *tri = mDebugTris.GetFreeLink(); // pull a free triangle from the pool
		if ( tri )
		{
            if ( mCurrentBlock && mCurrentBlock->mSolidTri == 0 )
            {
                mCurrentBlock->mSolidTri = tri;
            }
			mBoundsChanged = true;
			tri->Set(p1,p2,p3,n1,n2,n3,color | 0xFF000000, duration, mCurrentState.mRenderScale, mUseBlock ); // set condition of the triangle
			if ( mCurrentBlock )
			{
				tri->transform(mCurrentBlock->mPose);
			}

		}
		ApexRenderable::renderDataUnLock();
	}
#endif

	void DebugCapsule(const float *pos,float radius,float height)
	{
		JrMat44 t;
		NxVec3 p(pos);
		t.t = p;
		float matrix[16];
		t.getColumnMajor44(matrix);
		DebugOrientedCapsule(radius,height,matrix);
	}

	void DebugOrientedCapsule(float radius,float height,const float *transform)
	{
		NxVec3 prev1;
		NxVec3 prev2;

		float h2 = height*0.5f;

		NxVec3 top(0,0,0);
		NxVec3 bottom(0,0,0);

		top.y+=(height*0.5f)+radius;
		bottom.y-=(height*0.5f)+radius;

		for (int a=0; a<=360; a+=15)
		{
			float r = (float)a*FM_DEG_TO_RAD;

			float x = radius*cosf(r);
			float z = radius*sinf(r);

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

	void debugPlane(const float *plane,float radius1,float radius2)
	{
#if 0
		float ref[3] = { 0, 1, 0 };

		float quat[4];
		fm_rotationArc(ref,plane,quat);

		float matrix[16];
		fm_quatToMatrix(quat,matrix);

		float stepsize = 360/20;

		float prev[3] = { 0, 0, 0 };

		float pos[3];
		float first[3];

		float origin[3] = { 0, -plane[3], 0 };
		float center[3];

		fm_transform(matrix,origin,center);
		fm_setTranslation(center,matrix);

		for (float d=0; d<360; d+=stepsize)
		{
			float a = d*FM_DEG_TO_RAD;

			float dx = cosf(a)*radius1;
			float dy = 0; //
			float dz = sinf(a)*radius2;

			float _pos[3] = { dx, dy, dz };

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


	void DebugPlane(const float *plane,float radius1,float radius2)
	{
		debugPlane(plane,radius1*0.25f, radius2*0.25f);
		debugPlane(plane,radius1*0.5f,  radius2*0.5f);
		debugPlane(plane,radius1*0.75f, radius2*0.75f);
		debugPlane(plane,radius1*1.0f,  radius2*1.0f);
	}

	void DebugThickRay(const float *p1,const float *p2,float raySize)
	{

        pushRenderState();
        removeFromCurrentState(NxApexDebugRenderState::SolidWireShaded);
        addToCurrentState(NxApexDebugRenderState::SolidShaded);

		float dir[3];

		dir[0] = p2[0] - p1[0];
		dir[1] = p2[1] - p1[1];
		dir[2] = p2[2] - p1[2];

		float mag = fm_normalize(dir);
		float arrowSize = mCurrentState.mArrowSize;

		if ( arrowSize > (mag*0.2f) )
		{
			arrowSize = mag*0.2f;
		}

		float ref[3] = { 0, 1, 0 };

		float quat[4];

		fm_rotationArc(ref,dir,quat);

		float matrix[16];
		fm_quatToMatrix(quat,matrix);
		fm_setTranslation(p2,matrix);


		unsigned int pcount = 0;
		float points[24*3];
		float *dest = points;

		for (float a=30; a<=360; a+=30)
		{
			float r = a*FM_DEG_TO_RAD;
			float x = cosf(r)*arrowSize;
			float y = sinf(r)*arrowSize;

			dest[0] = x;
			dest[1] = -3*arrowSize;
			dest[2] = y;
			dest+=3;
			pcount++;
		}

		float *prev = &points[(pcount-1)*3];
		float *p = points;
		float center[3] = { 0, -2.5f*arrowSize, 0 };
		float top[3]    = { 0, 0, 0 };

		float _center[3];
		float _top[3];

		fm_transform(matrix,center,_center);
		fm_transform(matrix,top,_top);

		DebugCylinder(p1,_center,raySize);

        popRenderState();
        pushRenderState();

        setCurrentColor(mCurrentState.mArrowColor,0xFFFFFF);

		for (unsigned int i=0; i<pcount; i++)
		{

			float _p[3];
			float _prev[3];
			fm_transform(matrix,p,_p);
			fm_transform(matrix,prev,_prev);

			DebugTri(_p,_center,_prev);
			DebugTri(_prev,_top,_p);

			prev = p;
			p+=3;
		}
        popRenderState();
	}


	void DebugCylinder(const float *p1,const float *p2,float radius)
	{
		float dir[3];

		dir[0] = p2[0] - p1[0];
		dir[1] = p2[1] - p1[1];
		dir[2] = p2[2] - p1[2];


		float ref[3] = { 0, 1, 0 };

		float quat[4];

		fm_rotationArc(ref,dir,quat);

		float matrix1[16];
		float matrix2[16];

		fm_quatToMatrix(quat,matrix1);
		fm_setTranslation(p2,matrix1);

		fm_quatToMatrix(quat,matrix2);
		fm_setTranslation(p1,matrix2);


		unsigned int pcount = 0;
		float points1[24*3];
		float points2[24*3];
		float *dest1 = points1;
		float *dest2 = points2;


		for (float a=30; a<=360; a+=30)
		{
			float r = a*FM_DEG_TO_RAD;
			float x = cosf(r)*radius;
			float y = sinf(r)*radius;

			float p[3] = { x, 0, y };

			fm_transform(matrix1,p,dest1);
			fm_transform(matrix2,p,dest2);

			dest1+=3;
			dest2+=3;
			pcount++;

		}
		assert( pcount < 24 );

		if ( mCurrentState.isSolid() )
		{
			float *prev1 = &points1[(pcount-1)*3];
			float *prev2 = &points2[(pcount-1)*3];

			float *scan1 = points1;
			float *scan2 = points2;

			for (unsigned int i=0; i<pcount; i++)
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
			float *prev1 = &points1[(pcount-1)*3];
			float *prev2 = &points2[(pcount-1)*3];

			float *scan1 = points1;
			float *scan2 = points2;
			for (unsigned int i=0; i<pcount; i++)
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

	void DebugPolygon(unsigned int pcount,const float *points)
	{
		if ( pcount >= 3 )
		{
			if ( mCurrentState.isSolid() )
			{
				const float *p1 = points;
				const float *p2 = points+3;
				const float *p3 = points+6;
				DebugTri(p1,p2,p3);
				for (unsigned int i=3; i<pcount; i++)
				{
					p2 = p3;
					p3+=3;
					DebugTri(p1,p2,p3);
				}
			}
			else
			{
				const float *prev = &points[(pcount-1)*3];
				const float *p    = points;
				for (unsigned int i=0; i<pcount; i++)
				{
					DebugLine(prev,p);
					prev = p;
					p+=3;
				}
			}
		}
	}

	unsigned int getColor(unsigned int r,unsigned int g,unsigned int b,float percent)
	{
		unsigned int dr = (unsigned int)((float)r*percent);
		unsigned int dg = (unsigned int)((float)g*percent);
		unsigned int db = (unsigned int)((float)b*percent);
		r-=dr;
		g-=dg;
		b-=db;
		unsigned int c = (r<<16) | (g<<8) | b;
		return c;
	}

	void SwapYZ(NxVec3 &p)
	{
		float y = p.y;
		p.y = p.z;
		p.z = y;
	}

	void drawGrid(bool zup,unsigned int gridSize) // draw a grid.
	{
		int  GRIDSIZE = gridSize;

		unsigned int c1 = getColor(133,153,181,0.1f);
		unsigned int c2 = getColor(133,153,181,0.3f);
		unsigned int c3 = getColor(133,153,181,0.5f);

		const float TSCALE   = 1.0f;

		float BASELOC = 0-0.05f;
		pushRenderState();

		for (int x=-GRIDSIZE; x<=GRIDSIZE; x++)
		{
			unsigned int c = c1;
			if ( (x%10) == 0 ) c = c2;
			if ( (x%GRIDSIZE) == 0 ) c = c3;

			NxVec3 p1( (float)x,(float) -GRIDSIZE, BASELOC );
			NxVec3 p2( (float)x,(float) +GRIDSIZE, BASELOC );

			p1*=TSCALE;
			p2*=TSCALE;

			SwapYZ(p1);
			SwapYZ(p2);
			setCurrentColor(c,c);
			debugLine(p1,p2);


		}

		for (int y=-GRIDSIZE; y<=GRIDSIZE; y++)
		{
			unsigned int c = c1;

			if ( (y%10) == 0 ) c = c2;
			if ( (y%GRIDSIZE) == 0 ) c = c3;

			NxVec3 p1((float) -GRIDSIZE, (float)y, BASELOC );
			NxVec3 p2( (float)+GRIDSIZE, (float)y, BASELOC );

			p1*=TSCALE;
			p2*=TSCALE;

			SwapYZ(p1);
			SwapYZ(p2);

			setCurrentColor(c,c);
			debugLine(p1,p2);
		}
		popRenderState();
	}


	void         setScreenSize(unsigned int screenX,unsigned int screenY)
	{
		mScreenWidth = screenX;
		mScreenHeight = screenY;
	}

	void         getScreenSize(unsigned int &screenX,unsigned int &screenY)
	{
		screenX = mScreenWidth;
		screenY = mScreenHeight;
	}

	const float *getEyePos(void)
	{
		return &mEyePos.x;
	}

	void         setViewProjectionMatrix(const float *view,const float *projection)
	{
		mViewMatrix.setColumnMajor44(view);
		mProjectionMatrix.setColumnMajor44(projection);
		mViewProjectionMatrix.multiply(mViewMatrix,mProjectionMatrix);
		// grab the world-space eye position.
		JrMat44 viewInverse;
		if ( mViewMatrix.getInverse( viewInverse ) )
			viewInverse.multiply( NxVec3( 0.0f, 0.0f, 0.0f ), mEyePos );
	}

	const float *getViewProjectionMatrix(void) const
	{
		mViewProjectionMatrix.getColumnMajor44(mViewProjectionMatrix44);
		return mViewProjectionMatrix44;
	}

	const float *getViewMatrix(void) const
	{
		mViewMatrix.getColumnMajor44(mViewMatrix44);
		return mViewMatrix44;
	}

	const float *getProjectionMatrix(void) const
	{
		mProjectionMatrix.getColumnMajor44(mProjectionMatrix44);
		return mProjectionMatrix44;
	}

	bool         screenToWorld(int sx,int sy,float *world,float *direction)
	{
		bool ret = false;

	#if 0
		int wid = (int) mScreenWidth;
		int hit = (int) mScrenHeight;

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
			float p1[3];
			float p2[3];
			p1[0] = (float)_p1[0];
			p1[1] = (float)_p1[1];
			p1[2] = (float)_p1[2];

			p2[0] = (float)_p2[0];
			p2[1] = (float)_p2[1];
			p2[2] = (float)_p2[2];

			DebugLine(p1,p2);

		}
			virtual void DebugRay(const NxF64 *_p1,const NxF64 *_p2)
			{
				float p1[3];
				float p2[3];
				p1[0] = (float)_p1[0];
				p1[1] = (float)_p1[1];
				p1[2] = (float)_p1[2];

				p2[0] = (float)_p2[0];
				p2[1] = (float)_p2[1];
				p2[2] = (float)_p2[2];

				DebugRay(p1,p2);
			}

	virtual void DebugTri(const NxF64 *_p1,const NxF64 *_p2,const NxF64 *_p3)
	{
		float p1[3];
		float p2[3];
		float p3[3];
		p1[0] = (float)_p1[0]; p1[1] = (float)_p1[1]; p1[2] = (float)_p1[2];
		p2[0] = (float)_p2[0]; p2[1] = (float)_p2[1]; p2[2] = (float)_p2[2];
		p3[0] = (float)_p3[0]; p3[1] = (float)_p3[1]; p3[2] = (float)_p3[2];
		DebugTri(p1,p2,p3);
	}

	virtual void DebugTri(const NxF64 *_p1,const NxF64 *_p2,const NxF64 *_p3,const NxF64 *_n1,const NxF64 *_n2,const NxF64 *_n3)
	{
		float p1[3];
		float p2[3];
		float p3[3];

        float n1[3];
        float n2[3];
        float n3[3];

		p1[0] = (float)_p1[0]; p1[1] = (float)_p1[1]; p1[2] = (float)_p1[2];
		p2[0] = (float)_p2[0]; p2[1] = (float)_p2[1]; p2[2] = (float)_p2[2];
		p3[0] = (float)_p3[0]; p3[1] = (float)_p3[1]; p3[2] = (float)_p3[2];

		n1[0] = (float)_n1[0]; n1[1] = (float)_n1[1]; n1[2] = (float)_n1[2];
		n2[0] = (float)_n2[0]; n2[1] = (float)_n2[1]; n2[2] = (float)_n2[2];
		n3[0] = (float)_n3[0]; n3[1] = (float)_n3[1]; n3[2] = (float)_n3[2];

		DebugTri(p1,p2,p3,n1,n2,n3);
	}

	virtual void DebugBound(const NxF64 *bmin,const NxF64 *bmax)
	{
		float b1[3] = { (float)bmin[0], (float)bmin[1],(float)bmin[2] };
		float b2[3] = { (float)bmax[0], (float)bmax[1],(float)bmax[2] };
		DebugBound(b1,b2);
	}

	virtual void DebugSphere(const NxF64 *_pos,NxF64 radius,unsigned int color=0xFFFFFFFF,float duration=0.001f,bool useZ=true,bool solid=false)
	{
		float pos[3];
		pos[0] = (float)_pos[0];
		pos[1] = (float)_pos[1];
		pos[2] = (float)_pos[2];
		DebugSphere(pos,(float)radius);
	}

	void getSpherePoint(float *point,unsigned int x,unsigned int y,const float *center,float radius,float scale,unsigned int stepCount)
	{
		if ( x == stepCount ) x = 0;
		if ( y == stepCount ) y = 0;
		float a = (float)x*scale;
		float b = (float)y*scale;
		float tpos[3];

		tpos[0] = sinf(a)*cosf(b);
		tpos[1] = sinf(a)*sinf(b);
		tpos[2] = cosf(a);

		point[0] = center[0]+tpos[0]*radius;
		point[1] = center[1]+tpos[1]*radius;
		point[2] = center[2]+tpos[2]*radius;

	}

	void DebugDetailedSphere(const float *pos,float radius,unsigned int stepCount)
	{
		const float pi = 3.1415926535897932384626433832795028841971693993751f;
		const float pi2 = pi*2.0f;

		float scale = pi2 / stepCount;

		for (unsigned int y=0; y<stepCount; y++)
		{
			for (unsigned int x=0; x<stepCount; x++)
			{
				float p1[3];
				float p2[3];
				float p3[3];
				float p4[3];

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

	virtual void dispatchRenderResources(NxUserRenderer &renderer,NxReal lod)
	{
		Render(renderer);
	}


	void lockRenderResources()						{ ApexRenderable::renderDataLock();	}
	void unlockRenderResources()					{ ApexRenderable::renderDataUnLock(); }

	virtual void updateRenderResources(void *userRenderData)
	{
		if ( mApexSDK )
		{
			NxUserRenderResourceManager *m = mApexSDK->getUserRenderResourceManager();
			if ( m )
			{

				if ( mComputeBound )
				{
					mBounds.setEmpty();
				}

				update(mFrameTime,*m);

				mComputeBound = false; // it's been computed so zero out the semaphore
			}
		}
	}

	virtual void  setFrameTime(float dtime)
	{
		mFrameTime = dtime;
	}

	virtual void  setPose(const JrMat44 &pose)
	{
		mPose = pose;
	}

	virtual int beginDrawGroup(const JrMat44 &pose)
	{
		pushRenderState();
		setRenderState(NxApexDebugRenderState::InfiniteLifeSpan,0xFFFFFF,0.0001f,0xFF0000,0.1f,mCurrentState.mRenderScale,mCurrentState.mTextScale);

		mBlockIndex++;
		mUseBlock = mBlockIndex;
        mCurrentBlock = mBlocks.construct();
        mCurrentBlock->mHashValue = mBlockIndex;
		mCurrentBlock->mPose = pose;

        mBlocksHash.insert(mCurrentBlock);

		return mBlockIndex;
	}

	virtual void  endDrawGroup(void)
	{
		mUseBlock = 0;
        mCurrentBlock = 0;
		popRenderState();
	}

	virtual void  setDrawGroupVisible(int blockId,bool state)
	{
		BlockInfo *b;
		if ( mBlocksHash.exists((unsigned int)blockId,b) )
		{
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

	virtual void  setDrawGroupPose(int blockId,const JrMat44 &pose)
	{
        BlockInfo *b;
        if ( mBlocksHash.exists((unsigned int)blockId,b) )
        {
            if ( memcmp(&pose,&b->mPose,sizeof(JrMat44)) != 0 ) // if the pose has changed...
            {
                JrMat44 inverse;
                b->mPose.getInverse(inverse);
				JrMat44 tpose;
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

	virtual void  addDebugRenderable(const NxDebugRenderable &debugRenderable)
	{
		// Points
		pushRenderState();

		const unsigned int         numPoints = debugRenderable.getNbPoints();
		const NxDebugPoint *points    = debugRenderable.getPoints();
		for(unsigned int i=0; i<numPoints; i++)
		{
			const NxDebugPoint &point = points[i];
			debugPoint(point.p,0.01f);
		}

		// Lines
		const unsigned int        numLines = debugRenderable.getNbLines();
		const NxDebugLine *lines    = debugRenderable.getLines();
		for(unsigned int i=0; i<numLines; i++)
		{
			const NxDebugLine &line = lines[i];
			debugLine(line.p0,line.p1);
		}

		// Triangles
		const unsigned int            numTriangles = debugRenderable.getNbTriangles();
		const NxDebugTriangle *triangles    = debugRenderable.getTriangles();
		for(unsigned int i=0; i<numTriangles; i++)
		{
			const NxDebugTriangle &triangle = triangles[i];
			setCurrentColor(triangle.color,mCurrentState.mArrowColor);
			debugTri(triangle.p0,triangle.p1,triangle.p2);
		}
		popRenderState();
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
    	float roll  = angles.x*0.5f*FM_DEG_TO_RAD;
    	float pitch = angles.y*0.5f*FM_DEG_TO_RAD;
    	float yaw   = angles.z*0.5f*FM_DEG_TO_RAD;

    	float cr = cosf(roll);
    	float cp = cosf(pitch);
    	float cy = cosf(yaw);

    	float sr = sinf(roll);
    	float sp = sinf(pitch);
    	float sy = sinf(yaw);

    	float cpcy = cp * cy;
    	float spsy = sp * sy;
    	float spcy = sp * cy;
    	float cpsy = cp * sy;

    	float x   = ( sr * cpcy - cr * spsy);
    	float y   = ( cr * spcy + sr * cpsy);
    	float z   = ( cr * cpsy - sr * spcy);
    	float w   = cr * cpcy + sr * spsy;
        q.setXYZW(x,y,z,w);

	}

    virtual void debugArc(const NxVec3 &center,const NxVec3 &p1,const NxVec3 &p2,float arrowSize,bool showRoot)
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
        float d1 = v1.normalize();
        NxVec3 v2 = p2-center;
        float d2 = v2.normalize();
        float quat[4];

        fm_rotationArc(&v1.x,&v2.x,quat);

		NxQuat q1;
		q1.id();
        NxQuat q2;
        q2.setXYZW(quat);

        NxVec3 prev;

		int count = 0;
		for (float st=0; st<=(1.01f); st+=0.05f)
		{
			float d = ((d2-d1)*st)+d1;
			NxQuat q;
			q.slerp(st,q1,q2);
			JrMat44 m;
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
								float thickness,
								bool showRoot)
    {
		pushRenderState();
		addToCurrentState(NxApexDebugRenderState::SolidShaded);

        if ( showRoot )
        {
			pushRenderState();
			setCurrentColor(0xFFFFFF,0xFFFFFF);
		    debugLine(center,p1);
		    debugLine(center,p2);
			popRenderState();
        }

        NxVec3 v1 = p1-center;
        float d1 = v1.normalize();
        NxVec3 v2 = p2-center;
        float d2 = v2.normalize();
        float quat[4];

        fm_rotationArc(&v1.x,&v2.x,quat);

		NxQuat q1;
		q1.id();
        NxQuat q2;
        q2.setXYZW(quat);


        NxVec3 prev;


		int count = 0;
		for (float st=0; st<=(1.01f); st+=0.05f)
		{
			float d = ((d2-d1)*st)+d1;
			NxQuat q;
			q.slerp(st,q1,q2);
			JrMat44 m;
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

	virtual void debugGraph(unsigned int numPoints, float * points, float graphMax, float graphXPos, float graphYPos, float graphWidth, float graphHeight, unsigned int colorSwitchIndex)
	{
		pushRenderState();
		addToCurrentState(NxApexDebugRenderState::ScreenSpace);
		addToCurrentState(NxApexDebugRenderState::NoZbuffer);
		setCurrentTextScale(0.1f);

		/* Argh solid rendering not supported!
		//graph background:
		const unsigned int black = 0;
		addToCurrentState(NxApexDebugRenderState::SolidShaded);
		setCurrentColor(black, black);
		NxVec3 bacgroundQuad[4] = { NxVec3(0,0,0), NxVec3(1,0,0), NxVec3(1,1,0), NxVec3(0,1,0) };
		debugPolygon(4, bacgroundQuad);
		*/
		const unsigned int green = 0x0000ff00;

		removeFromCurrentState(NxApexDebugRenderState::SolidShaded);


		float lastX = graphXPos;
		float lastY = graphYPos;
		for (unsigned int i = 0; i < numPoints; i++)
		{
			float pointY = points[i];
			pointY = graphYPos + pointY * graphHeight / graphMax;	//scale to screen
			float x = graphXPos + graphWidth * i / numPoints;

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

	virtual void  setRenderState(unsigned int states,  // combination of render state flags
	                             unsigned int color, // base color
                                 float displayTime,
	                             unsigned int arrowColor, // secondary color, usually used for arrow head
                                 float arrowSize,
								 float renderScale,
                                 float textScale)
    {
        new ( &mCurrentState ) RenderState(states,color,displayTime,arrowColor,arrowSize,renderScale,textScale);
    }

	virtual void  setCurrentArrowSize(float arrowSize)
	{
        mCurrentState.mArrowSize = arrowSize;
	}

	virtual unsigned int getRenderState(unsigned int &color,float &displayTime,unsigned int &arrowColor,float &arrowSize,float &renderScale,float &textScale) const
    {
        color = mCurrentState.mColor;
        displayTime = mCurrentState.mDisplayTime;
        arrowColor = mCurrentState.mArrowColor;
        arrowSize = mCurrentState.mArrowSize;
		renderScale = mCurrentState.mRenderScale;
		return mCurrentState.mStates;
    }

	virtual void  addToCurrentState(NxApexDebugRenderState::Enum state) // OR this state flag into the current state.
	{
        mCurrentState.mStates|=state;
	}

	virtual void  removeFromCurrentState(NxApexDebugRenderState::Enum state) // Remove this bit flat from the current state
	{
        mCurrentState.mStates&=~state;
	}


	virtual void  setCurrentColor(unsigned int color,unsigned int arrowColor)
	{
        mCurrentState.mColor = color;
        mCurrentState.mArrowColor = arrowColor;
	}

	virtual void  setCurrentDisplayTime(float displayTime)
	{
        mCurrentState.mDisplayTime = displayTime;
	}

	virtual void  setCurrentState(unsigned int states)
	{
      mCurrentState.mStates = states;
	}

	virtual unsigned int getCurrentState(void) const
	{
        return mCurrentState.mStates;
	}

	virtual void DebugSphere(const NxF64 *_pos,NxF64 radius)
	{
        float pos[3];
        pos[0] = (float)_pos[0];
        pos[1] = (float)_pos[1];
        pos[2] = (float)_pos[2];
        DebugSphere(pos,(float)radius);
	}

	virtual void DebugHalfSphere(const NxF64 *_pos,NxF64 radius)
	{
        float pos[3];
        pos[0] = (float)_pos[0];
        pos[1] = (float)_pos[1];
        pos[2] = (float)_pos[2];
        DebugSphere(pos,(float)radius);
	}

	virtual void DebugPoint(const NxF64 *_pos,NxF64 radius)
	{
        float pos[3];
        pos[0] = (float)_pos[0];
        pos[1] = (float)_pos[1];
        pos[2] = (float)_pos[2];
        DebugSphere(pos,(float)radius);
	}

	virtual void DebugOrientedSphere(NxF64 radius,const NxF64 *_transform)
	{
        float transform[16];
        for (unsigned int i=0; i<16; i++)
            transform[i] = (float)_transform[i];
        DebugOrientedSphere((float)radius,transform);
	}

	virtual void DebugCylinder(const NxF64 *_p1,const NxF64 *_p2,NxF64 radius)
	{
		float p1[3];
		float p2[3];
		p1[0] = (float)_p1[0];
		p1[1] = (float)_p1[1];
		p1[2] = (float)_p1[2];

		p2[0] = (float)_p2[0];
		p2[1] = (float)_p2[1];
		p2[2] = (float)_p2[2];
		DebugCylinder(p1,p2,(float)radius);
	}

	virtual void DebugPolygon(unsigned int pcount,const NxF64 *_points) 
	{
		#define MAX_POLY_POINTS 64
		float points[MAX_POLY_POINTS*3];
		if ( pcount < MAX_POLY_POINTS )
		{
			for (unsigned int i=0; i<pcount*3; i++)
				points[i] = (float)_points[i];
			DebugPolygon(pcount,points);
		}

	}

	virtual void DebugOrientedLine(const NxF64 *_p1,const NxF64 *_p2,const NxF64 *_transform) 
	{
		float p1[3];
		float p2[3];
		p1[0] = (float)_p1[0];
		p1[1] = (float)_p1[1];
		p1[2] = (float)_p1[2];

		p2[0] = (float)_p2[0];
		p2[1] = (float)_p2[1];
		p2[2] = (float)_p2[2];
		float transform[16];
		for (unsigned int i=0; i<16; i++)
			transform[i] = (float)_transform[i];
		DebugOrientedLine(p1,p2,transform);
	}

	virtual void DebugThickRay(const NxF64 *_p1,const NxF64 *_p2,NxF64 raySize) 
	{
		float p1[3];
		float p2[3];
		p1[0] = (float)_p1[0];
		p1[1] = (float)_p1[1];
		p1[2] = (float)_p1[2];

		p2[0] = (float)_p2[0];
		p2[1] = (float)_p2[1];
		p2[2] = (float)_p2[2];
		DebugThickRay(p1,p2,(float)raySize);
	}

	virtual void DebugPlane(const NxF64 *_plane,NxF64 radius1,NxF64 radius2) 
	{
		float plane[4];
		plane[0] = (float)_plane[0];
		plane[1] = (float)_plane[1];
		plane[2] = (float)_plane[2];
		plane[3] = (float)_plane[3];
		DebugPlane(plane,(float)radius1,(float)radius2);
	}

	virtual void DebugCapsule(const NxF64 *_pos,NxF64 radius,NxF64 height) // assumes Y-up as the dominant axis, same as the PhysX SDK
	{
		float pos[3];
		pos[0] = (float)_pos[0];
		pos[1] = (float)_pos[1];
		pos[2] = (float)_pos[2];
		DebugCapsule(pos,(float)radius,(float)height);
	}

	virtual void DebugOrientedCapsule(NxF64 radius,NxF64 height,const NxF64 *_transform) 
	{
		float transform[16];
		for (unsigned int i=0; i<16; i++)
			transform[i] = (float)_transform[i];
		DebugOrientedCapsule((float)radius,(float)height,transform);
	}

	virtual void DebugAxes(const NxF64 *_transform,NxF64 distance,NxF64 brightness) 
	{
		float transform[16];
		for (unsigned int i=0; i<16; i++)
			transform[i] = (float)_transform[i];
		DebugAxes(transform,(float)distance,(float)brightness);
	}

    virtual void debugOrientedText(const NxVec3 &pos,const NxQuat &rot,const char *fmt,...)
    {
	    char wbuff[8192];
        wbuff[8191] = 0;
	    _vsnprintf(wbuff,8191, fmt, (char *)(&fmt+1));
        JrMat44 mat;
        mat.M.fromQuat(rot);
        mat.t = pos;
        addText(mat,wbuff);
    }

    virtual void debugOrientedText(const JrMat44 &xform,const char *fmt,...)
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
        JrMat44 mat;
        mat.id();
        mat.t = pos;
        addText(mat,wbuff);
    }

    virtual void debugText(float x,float y,float z,const char *fmt,...)
    {
	    char wbuff[8192];
        wbuff[8191] = 0;
	    _vsnprintf(wbuff,8191, fmt, (char *)(&fmt+1));
        JrMat44 mat;
        mat.id();
        mat.t.set(x,y,z);
        addText(mat,wbuff);
    }

    void addText(const JrMat44 &xform,const char *text)
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

	virtual void  setCurrentTextScale(float textScale) 
	{
		mCurrentState.mTextScale = textScale;
	}

	virtual void setViewMatrix(const JrMat44 &view)
	{
        mViewMatrix = view;
        updateVP();
	}

	virtual void setProjectionMatrix(const JrMat44 &projection)
	{
        mProjectionMatrix = projection;
        updateVP();
	}

    void updateVP(void)
    {
    	mViewProjectionMatrix.multiply(mViewMatrix,mProjectionMatrix);
    	// grab the world-space eye position.
    	JrMat44 viewInverse;
    	if ( mViewMatrix.getInverse( viewInverse ) )
    		viewInverse.multiply( NxVec3( 0.0f, 0.0f, 0.0f ), mEyePos );
    }


	virtual void debugRect2d(float x1,float y1,float x2,float y2) 
	{
		NxVec3 points[4];
		points[0].set(x1,y1,0);
		points[1].set(x2,y1,0);
		points[2].set(x2,y2,0);
		points[3].set(x1,y2,0);
		debugPolygon(4,points);
	}


private:

	int                             mBlockIndex;
	int                             mUseBlock;
	mutable bool                      mComputeBound:1; // only compute bounds if someone is asking for it!
	mutable bool                      mBoundsChanged:1; // semaphore to indicate that bounding volume may have changed and should be recomputed

	JrMat44                           mPose;

	float                             mFrameTime;
	Pool< LineTri >                   mDebugLines;       // the pool to work from.
	Pool< SolidTri >                  mDebugTris;       // the pool to work from.
    Pool< PrintText >                 mPrintText;

	unsigned int                             mScreenWidth;
	unsigned int                             mScreenHeight;

	JrMat44                           mViewMatrix;
	JrMat44                           mProjectionMatrix;
	JrMat44                           mViewProjectionMatrix;

	mutable float                     mViewMatrix44[16];
	mutable float                     mProjectionMatrix44[16];
	mutable float                     mViewProjectionMatrix44[16];

	NxVec3                            mEyePos;

	NxUserRenderResourceManager      *mManager;

	RenderDebugVertex                *mScreenPoints;
	RenderDebugVertex                *mLinePoints;
	RenderDebugSolidVertex           *mSolidPoints;
	RenderDebugSolidVertex           *mSolidScreenPoints;

	unsigned int                             mLineCount;
	unsigned int                             mScreenCount;
    unsigned int                             mSolidScreenCount;
	unsigned int                             mSolidCount;
	unsigned int                             mRenderCount;

	unsigned int                             mMaxLineBuffers;
	unsigned int                             mMaxScreenBuffers;
	unsigned int                             mMaxSolidBuffers;
	unsigned int                             mMaxSolidScreenBuffers;
	unsigned int                             mMaxRenderResources;

	NxUserRenderResourceDesc         *mScreen;
	NxUserRenderResourceDesc         *mLines;
	NxUserRenderResourceDesc         *mSolid;
	NxUserRenderResourceDesc         *mSolidScreen;

	NxUserRenderResource            **mLineResource;
	NxUserRenderResource            **mScreenResource;
	NxUserRenderResource            **mSolidScreenResource;
	NxUserRenderResource            **mSolidResource;

	NxUserRenderResource            **mResources;
    ResourceType                     *mResourceTypes;

	NxResID                           mWireFrameMaterial;
	NxResID                           mSolidShadedMaterial;
	ApexSDK                          *mApexSDK;
	NxBounds3                         mBounds;
    BlockInfo                        *mCurrentBlock;
    ApexHashSet< BlockInfo *>         mBlocksHash;
    ApexSimplePool< BlockInfo >       mBlocks;

    unsigned int                             mStackIndex;
    RenderState                       mCurrentState;
    RenderState                       mRenderStateStack[RENDER_STATE_STACK_SIZE];

    MyVectorFont                      mVectorFont;
    JrMat44                           mView;
    JrMat44                           mProjection;

};

};  // end of namespace

using namespace RENDER_DEBUG;

RatcliffRenderDebug * createRatcliffRenderDebug(void)
{
	JohnRatcliffRenderDebug *m = MEMALLOC_NEW(JohnRatcliffRenderDebug);
	return static_cast< RatcliffRenderDebug *>(m);
}

void releaseRatcliffRenderDebug(RatcliffRenderDebug *rrd)
{
    JohnRatcliffRenderDebug *jrrd = static_cast< JohnRatcliffRenderDebug *>(rrd);
    MEMALLOC_DELETE(JohnRatcliffRenderDebug,jrrd);
}
