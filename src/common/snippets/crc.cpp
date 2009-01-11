#include "crc.h"

/*!  
** 
** Copyright (c) 2007 by John W. Ratcliff mailto:jratcliff@infiniplex.net
**
** Portions of this source has been released with the PhysXViewer application, as well as 
** Rocket, CreateDynamics, ODF, and as a number of sample code snippets.
**
** If you find this code useful or you are feeling particularily generous I would
** ask that you please go to http://www.amillionpixels.us and make a donation
** to Troy DeMolay.
**
** DeMolay is a youth group for young men between the ages of 12 and 21.  
** It teaches strong moral principles, as well as leadership skills and 
** public speaking.  The donations page uses the 'pay for pixels' paradigm
** where, in this case, a pixel is only a single penny.  Donations can be
** made for as small as $4 or as high as a $100 block.  Each person who donates
** will get a link to their own site as well as acknowledgement on the
** donations blog located here http://www.amillionpixels.blogspot.com/
**
** If you wish to contact me you can use the following methods:
**
** Skype Phone: 636-486-4040 (let it ring a long time while it goes through switches)
** Skype ID: jratcliff63367
** Yahoo: jratcliff63367
** AOL: jratcliff1961
** email: jratcliff@infiniplex.net
**
**
** The MIT license:
**
** Permission is hereby granted, MEMALLOC_FREE of charge, to any person obtaining a copy 
** of this software and associated documentation files (the "Software"), to deal 
** in the Software without restriction, including without limitation the rights 
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
** copies of the Software, and to permit persons to whom the Software is furnished 
** to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all 
** copies or substantial portions of the Software.

** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
** WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#define QUOTIENT 0x04c11db7

static HeU32 crctab[256];


static inline HeU32 myhtonl(HeU32 n_ecx)
{
  HeU32 n_eax = n_ecx;                 //mov         eax,ecx 
  HeU32 n_edx = n_ecx;                 //mov         edx,ecx 
  n_edx = n_edx << 16;           //shl         edx,10h 
  n_eax = n_eax & 0x0FF00;       //and         eax,0FF00h 
  n_eax = n_eax | n_edx;         //or          eax,edx 
  n_edx = n_ecx;                 // mov         edx,ecx 
  n_edx = n_edx & 0x0FF0000;     //and edx,0FF0000h 
  n_ecx = n_ecx >> 16;           //shr         ecx,10h 
  n_edx = n_edx | n_ecx;         //or          edx,ecx 
  n_eax = n_eax << 8;            //shl         eax,8 
  n_edx = n_edx >> 8;            //shr         edx,8 
  n_eax|=n_edx;                  //  71AB2BE9  or          eax,edx 
  return n_eax;
}

static void crc32_init(void)
{
  HeI32 i,j;

  HeU32 crc;

  for (i = 0; i < 256; i++)
  {
    crc = i << 24;
    for (j = 0; j < 8; j++)
    {
      if (crc & 0x80000000)
        crc = (crc << 1) ^ QUOTIENT;
      else
        crc = crc << 1;
    }
    crctab[i] = myhtonl(crc);
  }
}

inline void getRand(HeU32 &current)
{
  current = (current * 214013L + 2531011L) & 0x7fffffff;
};



static HeU32 crc(const HeU8 *data, HeI32 len)
{
  HeU32        result;
  HeU32        *p = (HeU32 *)data;
  HeU32        *e = (HeU32 *)(data + len);

  result = ~*p++;
  result = result ^ len;

  const HeU32 *tmp = (const HeU32 *) data;
  HeU32 current = *tmp & len;

  while( p<e )
  {
    getRand(current);
#if defined(LITTLE_ENDIAN)
    result = crctab[result & 0xff] ^ result >> 8;
    result = crctab[result & 0xff] ^ result >> 8;
    result = crctab[result & 0xff] ^ result >> 8;
    result = crctab[result & 0xff] ^ result >> 8;
    result ^= *p++;
    result ^= current;
#else
    result = crctab[result >> 24] ^ result << 8;
    result = crctab[result >> 24] ^ result << 8;
    result = crctab[result >> 24] ^ result << 8;
    result = crctab[result >> 24] ^ result << 8;
    result ^= *p++;
    result ^= current;
#endif
    current &= result; // feed the result back into the random number seed, this forces the random sequence to drift with the input charcter stream.
  }

  return ~result;
}



HeU32 computeCRC(const HeU8 *data,HeU32 len)
{
  HeU32 ret = 0;

  static bool first = true;
  if ( first )
  {
    crc32_init();
    first = false;
  }

  if ( len < 4 )
  {
    HeU8 mem[4] = { 0, 0, 0, 0 };
    for (HeU32 i=0; i<len; i++)
      mem[i] = data[i];
    ret = crc(mem,4);
  }
  else
  {
    ret = crc(data,len);
  }
  return ret;
}

