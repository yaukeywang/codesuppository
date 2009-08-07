#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

#include "UserMemAlloc.h"
#include "HexMem.h"

static char hextable[256];

static void init(void)
{
  static bool first = true;
  if ( first )
  {
    memset(hextable,0,sizeof(hextable));
    for (NxU32 i='0'; i<='9'; i++)
    {
      hextable[i] = (char)(i-'0');
    }
    for (NxU32 i='a'; i<='f'; i++)
    {
      hextable[i] = (char)((i-'a')+10);
    }
    for (NxU32 i='A'; i<='F'; i++)
    {
      hextable[i] = (char)((i-'A')+10);
    }
    first = false;
  }
}


NxU8 * getHexMem(const char *str,size_t len,size_t &count)
{
  NxU8 *ret = 0;

  init();
  count = len/2;
  if ( count > 0 )
  {

    NxU8 *dest = MEMALLOC_NEW_ARRAY(unsigned char,count)[count];
    ret = dest;

    for (size_t i=0; i<count; i++)
    {
      *dest++ = (hextable[str[0]]<<4)| hextable[str[1]];
      str+=2;
    }
  }

  return ret;
}

NxU8 * getHexMem(const char *str,size_t &count)
{
  NxU8 * ret = 0;

  size_t len = strlen(str);
  ret = getHexMem(str,len,count);

  return ret;
}


NxU8 * getHexMem(const wchar_t *str,size_t size_in,size_t &count)
{

  NxU8 *ret = 0;

  init();
  count = size_in/2;

  if ( count > 0 )
  {

    NxU8 *dest = MEMALLOC_NEW_ARRAY(unsigned char,count)[count];
    ret = dest;

    for (size_t i=0; i<count; i++)
    {
      NxU8 c1 = (NxU8)(str[0]&0xFF);           // only give a crap about the low byte of the wide char
      NxU8 c2 = (NxU8)(str[1]&0xFF);
      *dest++ = (hextable[c1]<<4)| hextable[c2];
      str+=2;
    }
  }

  return ret;
}

void            deleteHexMem(const NxU8 *mem)
{
  delete []mem;
}
