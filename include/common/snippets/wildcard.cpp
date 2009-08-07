#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "UserMemAlloc.h"
#include "wildcard.h"
#include "regexp.h"

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
** Personal website: http://jratcliffscarab.blogspot.com
** Coding Website:   http://codesuppository.blogspot.com
** FundRaising Blog: http://amillionpixels.blogspot.com
** Fundraising site: http://www.amillionpixels.us
** New Temple Site:  http://newtemple.blogspot.com
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



#include <string>
#include <vector>

typedef std::string String;

class RegularExpression;

// Performs a conventional DOS style wildcard compare, with
// ? and * notation, by converting it into a regular expression match.

typedef USER_STL::vector< RegularExpression *> RegularExpressionVector;

class WildCard
{
public:
  WildCard(const char *wild);
  ~WildCard(void);
  bool IsWild(void) { return mIsWild; }; // see if this is a wildcard string.
  bool Match(const char *test);
private:
  bool mIsWild;
  String mMatch; // match string.
  String mWild;
  RegularExpression *mRegExp;
};


WildCard::WildCard(const char *wild)
{
  mMatch = wild;
  mIsWild = false;
  mWild = wild;

  std::string expression = "^";

  NxU32 len = strlen(wild);

  for (NxU32 i=0; i<len; i++)
  {
    switch ( wild[i] )
    {
      case '?':
        expression+='.'; // regular expression notation.
        mIsWild = true;
        break;
      case '*':
        expression+=".*";
        mIsWild = true;
        break;
      case '.':
        expression+='\\';
        expression+=wild[i];
        break;
      case ';':
        expression+='|';
        mIsWild = true;
        break;
      default:
        expression+=wild[i];

    }
  }

  if ( mIsWild )
  {
	  expression+='$';
    mRegExp = MEMALLOC_NEW(RegularExpression)(expression.c_str());
  }
  else
  {
    mRegExp = 0;
  }

}

WildCard::~WildCard(void)
{
  delete mRegExp;
}

bool WildCard::Match(const char *test)
{
  bool ret = false;
  if ( mRegExp )
  {
    ret = mRegExp->Match(test); // perfrom regular expression test
  }
  else
  {
    if ( stricmp(test,mWild.c_str()) == 0 )
      ret = true;
  }
  return ret;
}


WildCard * createWildCard(const char *str)
{
  WildCard *ret = 0;

  ret = MEMALLOC_NEW(WildCard)(str);

  return ret;
}

void       releaseWildCard(WildCard *wc)
{
  delete wc;
}

bool       isWild(WildCard *card)
{
  bool ret = false;
  if ( card )
  {
    ret = card->IsWild();
  }
  return ret;
}

bool       isMatch(WildCard *card,const char *str)
{
  bool ret = false;

  if ( card )
  {
    ret = card->Match(str);
  }

  return ret;

}


