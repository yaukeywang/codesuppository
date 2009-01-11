#ifndef BMP_H
#define BMP_H

#include "common/snippets/UserMemAlloc.h"

void * loadBMP(const char *fname, HeI32 &wid, HeI32 &hit, HeI32 &bpp);
void * loadBMP(const HeU8 *data, HeI32 &wid, HeI32 &hit, HeI32 &Bpp);
bool   saveBMP(const char *fname,const HeU8 *data, HeI32 wid, HeI32 hit, HeI32 Bpp);
void   swapRGB(HeU8 *dest, const HeU8 *src, HeU32 size);
void   swapVertically(HeU8 *dest, const HeU8 *src, HeI32 wid, HeI32 hit, HeI32 Bpp);


#endif
