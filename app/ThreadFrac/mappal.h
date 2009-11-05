#ifndef MAPPAL_H

#define MAPPAL_H


#include "UserMemAlloc.h"

// CodeSnippet provided by John W. Ratcliff
// on March 23, 2006.
//
// mailto: jratcliff@infiniplex.net
//
// Personal website: http://jratcliffscarab.blogspot.com
// Coding Website:   http://codesuppository.blogspot.com
// FundRaising Blog: http://amillionpixels.blogspot.com
// Fundraising site: http://www.amillionpixels.us
// New Temple Site:  http://newtemple.blogspot.com
//
// Reads a '.PAL' test file and interpolates it across
// a 256 color palette range.
//
// Provides methods to return the colors as either
// float r,g,b or as unsigned char r,g,b.

#include <stdio.h>

#define SMOOTH_SHIFT 4
#define SMOOTH_SIZE (256<<SMOOTH_SHIFT) // leave room for 2048 entries...

class MapPal
{
public:
  MapPal(void) { };
  MapPal(const char *name);
  ~MapPal(void);

  bool Load(const char *name); // name of map file

  void Get(NxI32 i,NxF32 &red,NxF32 &green,NxF32 &blue) const
  {
    NxI32 evenOdd = i/SMOOTH_SIZE;
    i = i&(SMOOTH_SIZE-1);
    if ( evenOdd&1 )
    {

      i = (SMOOTH_SIZE-1)-i;
    }
    NxI32 idx = i*3;
    red   = mEntries[idx];
    green = mEntries[idx+1];
    blue  = mEntries[idx+2];
  };

  void Get(NxI32 i,NxU8 &ir,NxU8 &ig,NxU8 &ib) const
  {
    NxF32 r,g,b;

    Get(i,r,g,b);
    ir = (NxU8)(r*255.0f);
    ig = (NxU8)(g*255.0f);
    ib = (NxU8)(b*255.0f);
  };


  NxU32 Get(NxI32 i) const
  {
    NxF32 r,g,b;

    Get(i,r,g,b);
    NxU32 ir = (NxU32)(r*255.0f);
    NxU32 ig = (NxU32)(g*255.0f);
    NxU32 ib = (NxU32)(b*255.0f);
    NxU32 c = 0xFF000000 | (ir<<16) | (ig<<8) | ib;
    return c;
  };


  NxF32 Slerp(NxF32 c,NxF32 i,NxF32 a)
  {
    NxF32 d = i-c;
    d*=a;
    return c+d;
  }

  void Desaturate(NxF32 amount)
  {
    for (NxI32 j=0; j<SMOOTH_SIZE; j++)
    {

      NxF32 r = mEntries[j*3+0];
      NxF32 g = mEntries[j*3+1];
      NxF32 b = mEntries[j*3+2];

      NxF32 i = r*0.3f + g*0.6f + b*0.1f;
      if ( i > 1 ) i = 1;

      mEntries[j*3+0] = Slerp(r,i,amount);
      mEntries[j*3+1] = Slerp(g,i,amount);
      mEntries[j*3+2] = Slerp(b,i,amount);
    }
  };

private:
  bool readline(FILE *fph,char *inbuff);	/* read a line, strip the comment if any */

  NxF32 mEntries[SMOOTH_SIZE*3]; // holds palette entries as floating point scalers
};


#endif
