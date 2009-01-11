#pragma warning(disable:4996)
#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN
#if defined(WIN32)
#include <windows.h>
#else
#include "linux_compat.h"
#endif

void * loadFileBMP(const char *fname, HeI32 &wid, HeI32 &hit, HeI32 &bpp)
{
	FILE *fph = fopen(fname,"rb");
	if ( !fph ) return 0;
	fseek(fph,0L,SEEK_END);
	HeI32 len = ftell(fph);
	if ( !len )
	{
		fclose(fph);
		return 0;
	}
	fseek(fph,0L,SEEK_SET);
  HeU8 *mem = MEMALLOC_NEW_ARRAY(unsigned char,len)[len];
	fread(mem, len, 1, fph);
	fclose(fph);

  void *ret = loadBMP(mem,wid,hit,bpp);

	delete mem;
	return ret;
}


void *loadBMP(const HeU8 *data, HeI32 &wid, HeI32 &hit, HeI32 &Bpp)
{
  //First things first, init working variables
  if ( !data ) return NULL;
  const HeU8 *p = data;   //working pointer into DIB
  wid = 0;
  hit = 0;
  Bpp = 0;

  //Note: We declare and use a BITMAPINFOHEADER, even though the header
  //  may be of one of the newer types, because we don't need to know the
  //  actual header size in order to find the bitmap bits.  (The offset
  //  is given by bfOffBits)
  BITMAPFILEHEADER *filehdr;
  BITMAPINFOHEADER *infohdr;

  filehdr = (BITMAPFILEHEADER *) p;
  infohdr = (BITMAPINFOHEADER *) (p + sizeof(BITMAPFILEHEADER));
//  char *blah = (char*)(infohdr + sizeof(infohdr));

  if ( infohdr->biSize == sizeof(BITMAPCOREHEADER) )
  {
    //Old-style OS/2 bitmap header, we don't support it
    return NULL;
  }
  else
  {
    wid = infohdr->biWidth;
    hit = infohdr->biHeight;
    Bpp = (infohdr->biBitCount / 8);
  }

//  if ( Bpp != 1 && Bpp != 3 ) return NULL;    //We only support 8bit and 24bit files


  //Set pointer to beginning of packed pixel data
  p = data + filehdr->bfOffBits;

  //FIXME: This assumes a non-compressed bitmap (no RLE)
  HeI32 siz;
#if 0
  HeI32 remainder = wid % 4;
  if (remainder != 0)
    siz = (wid + (4-remainder)) * hit * Bpp;
  else
    siz = wid * hit * Bpp;
#else
  HeI32 linesize = ((((Bpp * wid)-1)/4)+1)*4;
  siz = hit * linesize;
#endif
  HeU8 *mem = MEMALLOC_NEW_ARRAY(unsigned char,siz)[siz];

  assert (mem);

  if ( Bpp == 1 )
  {

    const HeU8 *base_source = &p[(hit-1)*wid];
    HeU8 *base_dest   = mem;

    for (HeI32 y=0; y<hit; y++)
    {
      HeU8 *dest = base_dest;
      const HeU8 *source = base_source;
      memcpy(dest,source,wid);
      base_dest+=(wid);
      base_source-=(wid);
    }



  }
  else
  {
    const HeU8 *base_source = &p[(hit-1)*wid*3];
    HeU8 *base_dest   = mem;

    for (HeI32 y=0; y<hit; y++)
    {
      HeU8 *dest = base_dest;
      const HeU8 *source = base_source;

      for (HeI32 x=0; x<wid; x++)
      {
        dest[0] = source[2];
        dest[1] = source[1];
        dest[2] = source[0];
        dest+=3;
        source+=3;
      }

      base_dest+=(wid*3);
      base_source-=(wid*3);


    }

  }
  return mem;
}



void swapRGB(HeU8 *dest, const HeU8 *src, HeU32 size)
{
  const HeU8 *s = src;
  HeU8 *d = dest;

  for ( HeU32 i=0; i< size; i+=3  )
  {
    HeU8 tmp = s[2];   //in case src and dest point to the same place
    d[2] = s[0];
    d[1] = s[1];
    d[0] = tmp;
    d += 3;
    s += 3;
  }
}


void swapVertically(HeU8 *dest, const HeU8 *src, HeI32 wid, HeI32 hit, HeI32 Bpp)
{
  const HeU8 *s = src;

  HeI32 rowSize = wid * Bpp;

  HeU8 *d = dest + (rowSize * (hit-1));

  for ( HeI32 r=0 ; r < hit; ++r )
  {
    memcpy(d, s, rowSize);
    s += rowSize;
    d -= rowSize;
  }
}


bool saveBMP(const char *fname, const HeU8 *inputdata, HeI32 wid, HeI32 hit, HeI32 Bpp)
{
  bool ret = false;

  HeU8 *data = 0;

  if ( Bpp == 1 )
  {
    data = (HeU8 *) inputdata;
  }
  else
  {

    const HeU8 *source;
    HeU8 *dest   = MEMALLOC_NEW_ARRAY(unsigned char,wid*hit*3)[wid*hit*3];
    data = dest; // data to save has flipped RGB order.

    HeI32 bwid = wid*3;

    source = &inputdata[(hit-1)*bwid];

    for (HeI32 y=0; y<hit; y++)
    {
      memcpy(dest,source,bwid);
      dest+=bwid;
      source-=bwid;
    }

    //swap RGB!!
    {
      HeU8 *swap = data;
      HeI32 size = wid*hit;
      for (HeI32 i=0; i<size; i++)
      {
        HeU8 c = swap[0];
        swap[0] = swap[2];
        swap[2] = c;
        swap+=3;
      }
    }
  }


  BITMAPFILEHEADER filehdr;
  BITMAPINFOHEADER infohdr;

  DWORD offset = sizeof(filehdr) + sizeof(infohdr);
  if ( Bpp == 1 )
  {
    //Leave room in file for the color table
    offset += (256 * sizeof(RGBQUAD));
  }

  DWORD sizeImage;
  HeI32 remainder = wid % 4;
  HeI32 linesize;
  if (remainder != 0)
    linesize = wid + (4-remainder);
  else
    linesize = wid;
  sizeImage = linesize * hit * Bpp;

  filehdr.bfType = *((WORD*)"BM");
  filehdr.bfSize = offset + sizeImage;
  filehdr.bfReserved1 = 0;
  filehdr.bfReserved2 = 0;
  filehdr.bfOffBits = offset;

  infohdr.biSize = sizeof(infohdr);
  infohdr.biWidth = wid;
  infohdr.biHeight = hit;
  infohdr.biPlanes = 1;
  infohdr.biBitCount = (WORD)(Bpp * 8);
  infohdr.biCompression = BI_RGB;
  infohdr.biSizeImage = sizeImage;
  infohdr.biClrUsed = 0;
  infohdr.biClrImportant = 0;
  infohdr.biXPelsPerMeter = 72;
  infohdr.biYPelsPerMeter = 72;

  HeU32 writtencount = 0;

  FILE *fph = fopen(fname,"rb");
  if ( fph )
  {
    fwrite(&filehdr, sizeof(filehdr),1,fph);
    writtencount += sizeof(filehdr);

    fwrite(&infohdr, sizeof(infohdr),1,fph);
    writtencount += sizeof(infohdr);

    if ( Bpp == 1 )
    {
      //Generate a greyscale color table for this image
      RGBQUAD rgbq;
      rgbq.rgbReserved = 0;
      for ( HeI32 i=0; i<256; i++ )
      {
        rgbq.rgbBlue = (BYTE)i;
        rgbq.rgbGreen = (BYTE)i;
        rgbq.rgbRed = (BYTE)i;
        fwrite(&rgbq, sizeof(RGBQUAD),1,fph);
        writtencount += sizeof(RGBQUAD);
      }
    }

    HeU8 *p = data;
    for ( HeU32 i = 0; i < sizeImage; i+=linesize)
    {
      fwrite((void*)p, linesize,1,fph);
      p+=linesize;
      writtencount+=linesize;
    }

    fclose(fph);

    ret = true;

    assert ( writtencount == filehdr.bfSize );
  }

  if ( Bpp == 3 )
  {
    delete data;
  }
  return ret;
}

