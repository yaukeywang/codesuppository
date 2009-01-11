#ifndef BITMAP_H

#define BITMAP_H

#include "common/snippets/UserMemAlloc.h"

class Color8
{
public:
  Color8(void)
  {
    r = g = b = 0;
  }
  Color8(HeU8 _r,HeU8 _g,HeU8 _b)
  {
    r = _r;
    g = _g;
    b = _b;
  }

  inline HeI32 distanceSquared(const Color8 &c) const
  {
    HeI32 dr = (HeI32)r-(HeI32)c.r;
    HeI32 dg = (HeI32)g-(HeI32)c.g;
    HeI32 db = (HeI32)b-(HeI32)c.b;
    return dr*dr+dg*dg+db*db;
  }

  HeU8 r;
  HeU8 g;
  HeU8 b;
};

class Palette256
{
public:
  Palette256(void)
  {
    mColorCount = 1;
  }

  HeU8 getNearestColor(const Color8 &c)
  {

    const HeI32 CLOSE_ENOUGH = (2*2*2);

    const Color8 *scan = mColors;

    HeI32 nearloc = -1;
    HeI32 nearest = 0x7FFFFFFF;

    for (HeI32 i=0; i<mColorCount; i++,scan++)
    {
      HeI32 dist = c.distanceSquared(*scan);
      if ( dist < nearest )
      {
        nearest = dist;
        nearloc = i;
      }
    }

    HeU8 ret = (HeU8) nearloc;

    if ( nearest > CLOSE_ENOUGH && mColorCount < 256 )
    {
      if ( mColorCount < 256 )
      {
        mColors[mColorCount] = c;
        ret = (HeU8)mColorCount;
        mColorCount++;
      }
    }
    return (HeU8)ret;
  }

  const HeU8 * getPalette(void) const { return &mColors[0].r; };

private:
  HeI32     mColorCount;
  Color8  mColors[256];
};

class BitMap256
{
public:
  BitMap256(HeI32 wid,HeI32 hit)
  {
    mWidth  = wid;
    mHeight = hit;
    mBuffer = (HeU8 *)MEMALLOC_MALLOC(mWidth*mHeight);
    memset(mBuffer,0,mWidth*mHeight);
  }

  BitMap256(void)
  {
    if ( mBuffer )
    {
      MEMALLOC_FREE(mBuffer);
    }
  }

  HeU8 getNearestColor(const Color8 &c)  { return mPalette.getNearestColor(c); };

  HeU8 getNearestColor(HeU32 color) 
  {
    Color8 c;
    c.r = (HeU8)((color>>16)&0xFF);
    c.g = (HeU8)((color>>8)&0xFF);
    c.b = (HeU8)(color&0xFF);
    return mPalette.getNearestColor(c);
  }

  void plotPixel(HeI32 x,HeI32 y,HeU8 color)
  {
    mBuffer[y*mWidth+x] = color;
  }

  void plotPixelSafe(HeI32 x,HeI32 y,HeU8 color)
  {
    if ( x >= 0 && x < mWidth && y >= 0 && y < mHeight )
    {
      mBuffer[y*mWidth+x] = color;
    }
  }

  bool saveGIF(const char *fname);

private:
  HeI32            mWidth;
  HeI32            mHeight;
  HeU8 *mBuffer;
  Palette256     mPalette;
};

#endif
