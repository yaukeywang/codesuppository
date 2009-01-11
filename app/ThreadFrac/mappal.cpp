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
    HeU8 r;
    HeU8 g;
    HeU8 b;
};

typedef USER_STL::vector< Col > ColVector;



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
  for (HeI32 i=0; i<SMOOTH_SIZE*3; i++) mEntries[i] = 1.0f;
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

  for (HeI32 i=0; i<256; i++)
	{
	  bool eof = readline(fph,string);
    if (eof ) break;
    HeI32 red,green,blue;
    if ( *string != '#' )
    {
  		HeI32 count = sscanf(string,"%d %d %d",&red,&green,&blue);
      if ( count == 3 )
      {
        Col c;
        c.r = HeU8(red);
        c.g = HeU8(green);
        c.b = HeU8(blue);
        list.push_back(c);
      }
    }
  }
  fclose(fph);

  {

    HeI32 size = list.size();
    if ( !size ) return false;

    for (HeI32 i=0; i<SMOOTH_SIZE; i++)
    {
      // ok, compute the index...
	    HeI32 scount = size-1;

      HeI32 iloc1 = (scount*i) / SMOOTH_SIZE;
      HeI32 iloc2 = iloc1+1;

      // current color...
      // next color..
      // now slerp between them...

      HeF32 f0 = HeF32(i);

      HeF32 f1 = HeF32(SMOOTH_SIZE*(iloc1)) /  HeF32(scount);
      HeF32 f2 = HeF32(SMOOTH_SIZE*(iloc2)) /  HeF32(scount);

      if ( iloc2 >= size ) iloc2 = size-1;

      HeF32 r1,g1,b1;
      HeF32 r2,g2,b2;

      r1 = HeF32( list[iloc1].r ) * RATIO;
      g1 = HeF32( list[iloc1].g ) * RATIO;
      b1 = HeF32( list[iloc1].b ) * RATIO;

      r2 = HeF32( list[iloc2].r ) * RATIO;
      g2 = HeF32( list[iloc2].g ) * RATIO;
      b2 = HeF32( list[iloc2].b ) * RATIO;


      // slerp along f1-f2
      f0-=f1;
      f1 = f2-f1;
      HeF32 recip1 = f0 / f1;
      HeF32 recip2 = 1.0f - recip1;

      HeF32 r = r1*recip2 + r2*recip1;
      HeF32 g = g1*recip2 + g2*recip1;
      HeF32 b = b1*recip2 + b2*recip1;

      HeI32 index = i*3;

      mEntries[index+0] = r;
      mEntries[index+1] = g;
      mEntries[index+2] = b;

    }
  }

  return true;
}

