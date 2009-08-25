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

#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef WIN32
#include <windows.h>
#include <windowsx.h>
#endif

#ifdef LINUX_GENERIC
#include <sys/types.h>
#include <sys/dir.h>
#endif

#include "ffind.h"
#include "wildcard.h"

class InternalFind
{
public:
#ifdef WIN32
  WIN32_FIND_DATAA finddata;
  HANDLE hFindNext;
  NxI32 bFound;
#endif
#ifdef LINUX_GENERIC
  DIR      *mDir;
#endif
};

FileFind::FileFind(WildCard *wild,const char *dirname)
{
  mWildCard = wild;
  if ( dirname )
    sprintf(mSearchName,"%s\\*.*",dirname);
  else
    sprintf(mSearchName,"*.*");
  mInternalFind = MEMALLOC_NEW(InternalFind);
}

FileFind::~FileFind(void)
{
  delete mInternalFind;
}


bool FileFind::FindFirst(USER_STL::string &name)
{
#ifdef WIN32
  mInternalFind->hFindNext = FindFirstFileA(mSearchName, &mInternalFind->finddata);
  if ( mInternalFind->hFindNext == INVALID_HANDLE_VALUE )
		return false;
  mInternalFind->bFound = 1; // have an initial file to check.
  return FindNext(name);
#endif

#ifdef LINUX_GENERIC
  mInternalFind->mDir = opendir(".");
  return FindNext(name);
#endif
  //return false;
}

bool FileFind::FindNext(String &name)
{
#ifdef WIN32
  while ( mInternalFind->bFound )
  {
    mInternalFind->bFound = FindNextFileA(mInternalFind->hFindNext, &mInternalFind->finddata);
    if ( (mInternalFind->finddata.cFileName[0] != '.') && !(mInternalFind->finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
    {


      //gOutput->Display("DIRECTORY ENTRY: %s\n",mInternalFind->finddata.cFileName);

      name = mInternalFind->finddata.cFileName;


  		strlwr((char*)name.c_str());

      if ( mWildCard )
      {
        if ( isMatch(mWildCard,name.c_str()) ) return true;
      }
      else
        return true;
    }
  }
  FindClose(mInternalFind->hFindNext);
#endif

#ifdef LINUX_GENERIC

  if ( mInternalFind->mDir )
  {
    while ( 1 )
    {

      struct direct *di = readdir( mInternalFind->mDir );

      if ( !di )
      {
        closedir( mInternalFind->mDir );
        mInternalFind->mDir = 0;
        return false;
      }

      //gOutput->Display("DIRECTORY ENTRY: %s\n",di->d_name);

      if ( strcmp(di->d_name,".") == 0 || strcmp(di->d_name,"..") == 0 )
      {
        // skip it!
      }
      else
      {
        name = di->d_name;
    		stringutils::strlwr((char*)name.c_str());
        if ( mWildCard )
        {
          if ( mWildCard->Match(name.c_str()) ) return true;
        }
        else
          return true;
      }
    }
  }
#endif

  return false; // finished search.
}


NxI32 deleteFiles(const String &str)
{
  WildCard *wild = createWildCard(str.c_str());
  StringVector list;
  if ( isWild(wild) )
  {
    FileFind ffind(wild);
    ffind.GetFiles(list);
  }
  else
  {
    list.push_back(str);
  }

  StringVector::iterator i;

  for (i=list.begin(); i!=list.end(); ++i)
  {
#ifdef WIN32
    DeleteFileA( (*i).c_str() );
#endif
#ifdef LINUX_GENERIC
    assert(0); // not implemented on unix yet, not needed though.
#endif
  }
  releaseWildCard(wild);

  NxI32 ret = list.size();

  return ret;
}


void FileFind::GetFiles(StringVector &list)  // get all files.
{

	USER_STL::string str;
  if ( FindFirst(str) )
  {
    list.push_back(str);
    while ( FindNext(str) )
    {
      list.push_back(str);
    }
  }
}

