#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "mappal.h"
#include "common/snippets/UserMemAlloc.h"

#include <vector>

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


class Col
{
  public:
    NxU8 r;
    NxU8 g;
    NxU8 b;
};

typedef std::vector< Col > ColVector;



bool MapPal::readline(FILE *fph,char *inbuff)	/* read a line, strip the comment if any */
{
	char *buff;
	do
	{
		buff = inbuff;
		do
		{
			if (fgets(buff,256,fph) == NULL) return true;
		} while (*buff == '\0');
	} while ( *inbuff == 0xA );
	return false;
}

MapPal::MapPal(const char *name)
{
  for (NxI32 i=0; i<SMOOTH_SIZE*3; i++) mEntries[i] = 1.0f;
  Load(name);
}

MapPal::~MapPal(void)
{
}

bool MapPal::Load(const char *name)
{
	FILE *fph;
	char string[256];

  #define RATIO (1.0f/256.0f)

	fph = fopen(name, "rb");

  if ( !fph ) return false;

  ColVector list;

  for (NxI32 i=0; i<256; i++)
	{
	  bool eof = readline(fph,string);
    if (eof ) break;
    NxI32 red,green,blue;
    if ( *string != '#' )
    {
  		NxI32 count = sscanf(string,"%d %d %d",&red,&green,&blue);
      if ( count == 3 )
      {
        Col c;
        c.r = NxU8(red);
        c.g = NxU8(green);
        c.b = NxU8(blue);
        list.push_back(c);
      }
    }
  }
  fclose(fph);

  {

    NxI32 size = list.size();
    if ( !size ) return false;

    for (NxI32 i=0; i<SMOOTH_SIZE; i++)
    {
      // ok, compute the index...
	    NxI32 scount = size-1;

      NxI32 iloc1 = (scount*i) / SMOOTH_SIZE;
      NxI32 iloc2 = iloc1+1;

      // current color...
      // next color..
      // now slerp between them...

      NxF32 f0 = NxF32(i);

      NxF32 f1 = NxF32(SMOOTH_SIZE*(iloc1)) /  NxF32(scount);
      NxF32 f2 = NxF32(SMOOTH_SIZE*(iloc2)) /  NxF32(scount);

      if ( iloc2 >= size ) iloc2 = size-1;

      NxF32 r1,g1,b1;
      NxF32 r2,g2,b2;

      r1 = NxF32( list[iloc1].r ) * RATIO;
      g1 = NxF32( list[iloc1].g ) * RATIO;
      b1 = NxF32( list[iloc1].b ) * RATIO;

      r2 = NxF32( list[iloc2].r ) * RATIO;
      g2 = NxF32( list[iloc2].g ) * RATIO;
      b2 = NxF32( list[iloc2].b ) * RATIO;


      // slerp along f1-f2
      f0-=f1;
      f1 = f2-f1;
      NxF32 recip1 = f0 / f1;
      NxF32 recip2 = 1.0f - recip1;

      NxF32 r = r1*recip2 + r2*recip1;
      NxF32 g = g1*recip2 + g2*recip1;
      NxF32 b = b1*recip2 + b2*recip1;

      NxI32 index = i*3;

      mEntries[index+0] = r;
      mEntries[index+1] = g;
      mEntries[index+2] = b;

    }
  }

  return true;
}

