#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "bitmap.h"
#include "sgif.h"


bool BitMap256::saveGIF(const char *fname)
{
  bool ret = false;

  save_screen(fname,mBuffer,mWidth,mHeight,mPalette.getPalette());

  return ret;
}
