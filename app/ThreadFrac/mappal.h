#ifndef MAPPAL_H

#define MAPPAL_H


#include "common/snippets/HeSimpleTypes.h"

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

  void Get(HeI32 i,HeF32 &red,HeF32 &green,HeF32 &blue) const
  {
    HeI32 evenOdd = i/SMOOTH_SIZE;
    i = i&(SMOOTH_SIZE-1);
    if ( evenOdd&1 )
    {

      i = (SMOOTH_SIZE-1)-i;
    }
    HeI32 idx = i*3;
    red   = mEntries[idx];
    green = mEntries[idx+1];
    blue  = mEntries[idx+2];
  };

  void Get(HeI32 i,HeU8 &ir,HeU8 &ig,HeU8 &ib) const
  {
    HeF32 r,g,b;

    Get(i,r,g,b);
    ir = (HeU8)(r*255.0f);
    ig = (HeU8)(g*255.0f);
    ib = (HeU8)(b*255.0f);
  };


  HeU32 Get(HeI32 i) const
  {
    HeF32 r,g,b;

    Get(i,r,g,b);
    HeU32 ir = (HeU32)(r*255.0f);
    HeU32 ig = (HeU32)(g*255.0f);
    HeU32 ib = (HeU32)(b*255.0f);
    HeU32 c = 0xFF000000 | (ir<<16) | (ig<<8) | ib;
    return c;
  };


  HeF32 Slerp(HeF32 c,HeF32 i,HeF32 a)
  {
    HeF32 d = i-c;
    d*=a;
    return c+d;
  }

  void Desaturate(HeF32 amount)
  {
    for (HeI32 j=0; j<SMOOTH_SIZE; j++)
    {

      HeF32 r = mEntries[j*3+0];
      HeF32 g = mEntries[j*3+1];
      HeF32 b = mEntries[j*3+2];

      HeF32 i = r*0.3f + g*0.6f + b*0.1f;
      if ( i > 1 ) i = 1;

      mEntries[j*3+0] = Slerp(r,i,amount);
      mEntries[j*3+1] = Slerp(g,i,amount);
      mEntries[j*3+2] = Slerp(b,i,amount);
    }
  };

private:
  bool readline(FILE *fph,char *inbuff);	/* read a line, strip the comment if any */

  HeF32 mEntries[SMOOTH_SIZE*3]; // holds palette entries as floating point scalers
};


#endif
