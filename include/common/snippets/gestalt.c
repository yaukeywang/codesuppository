// GESTALT.C : A fuzzy compare.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma warning(disable:4701)

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


// Performs a 'fuzzy' comparison between two strings.  Returns how
// 'alike' they are expressed as a percentage match.
//
// Written originally by John W. Ratcliff for Dr. Dobbs Journal
// of Software Tools Volume 13, 7, July 1988
//
// Pages 46, 47, 59-51, 68-72
// http://www.ddj.com/184407970?pgno=5
//
// http://www.codesuppository.blogspot.com/
//
// If you appreciate my little snippets of source code
// please donate a few bucks to my kids youth group fundraising
// website located at http://www.amillionpixels.us/


#include "gestalt.h"
#include <string.h>

#ifdef  __cplusplus
extern "C" {
#endif

static int GCsubstr(const char *st1,const char *end1,const char *st2,const char *end2)
{
  const char *s1;
  const char *s2;
  int max = 0;
  const char *b1 = end1;
  const char *b2 = end2;
  int i;
	const char *a1;
	const char *a2;


  if (end1 <= st1) return 0;
  if (end2 <= st2) return 0;
  if (end1 == (st1+1) && end2 == (st2+1) ) return 0;


  for (a1 = st1; a1 < b1; a1++)
  {
    for (a2 = st2; a2 < b2; a2++)
    {
      if (*a1 == *a2)
			{

				for (i=1; a1[i] && (a1[i] == a2[i]); i++);

        if (i > max)
				{
					max = i; s1 = a1; s2 = a2;
					b1 = end1 - max; b2 = end2 - max;
				}

			}
    }
  }

  if (!max) return 0;

  max += GCsubstr(s1+max, end1, s2+max, end2);
  max += GCsubstr(st1, s1, st2, s2);

  return max;
}


int FuzzyCompare(const char *s1,const char *s2)
{
	int l1,l2;

  if ( strcmp(s1,s2) == 0 ) return 100;

	l1 = (int)strlen(s1);
	l2 = (int)strlen(s2);

  if (l1 == 1)
    if (l2 == 1)
      if (*s1 == *s2)
        return(100);

  return(200 * GCsubstr(s1, s1+l1, s2, s2+l2)/ (l1+l2));
}


#ifdef  __cplusplus
}
#endif
