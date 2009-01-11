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
#include <stdarg.h>

#ifdef WIN32
#include <windows.h>
#endif

#include "common/snippets/UserMemAlloc.h"
#include "log.h"

IMPLEMENT_MEMORYPOOL_IN_CLASS(Log);

#define USE_LOG 1

Log *gLog=0;

static  char buff[8192];

Log::Log(void)
{
  mDest = mBuffer;
  mEcho = false;
	mTrapLog = 0;
	mLogIndex = 0;
	mHaveLogMessages = false;
	mBuffer[0] = 0;
	mLogFrame = 0;
	mIndex = 0;
	mLogLevel = 0;
	mFph = 0;
	mBufferMessages = false;
#if USE_LOG
	mFph = fopen("log.txt","wb");
#endif
}

Log::Log(const char *fname,bool buffermessages)
{
  mDest = mBuffer;
  mEcho = false;
	mTrapLog = 0;
	mLogIndex = 0;
	mHaveLogMessages = false;
	mBuffer[0] = 0;
	mLogFrame = 0;
	mIndex = 0;
	mLogLevel = 0;
	mBufferMessages = buffermessages;
  mFph = 0;
#if USE_LOG
  if ( fname )
  	mFph = fopen(fname,"wb");
#endif
}

Log::~Log(void)
{
	gLog = 0;
}


void Log::Display(const char *fmt, ... )
{
	char wbuff[8192];
  wbuff[8191] = 0;
	_vsnprintf(wbuff,8191, fmt, (char *)(&fmt+1));

	const char *source = wbuff;

	char *dest = buff;
	char p = 0;
	HeU32 len = (HeU32)strlen(source);

  bool lineFeed = false;

	for (HeU32 i=0; i<len; i++)
	{
		char c = *source++;

		if ( c == 10 && p != 13 )
		{
			*dest++ = 13;
      *mDest++ = 13;
    }

    if ( p == 13 )
    {
      lineFeed = true;
		}

		*dest++ = c;
    if ( mBufferMessages )
    {
      *mDest++ = c;
      if ( (mDest-mBuffer) >= 1023 ) lineFeed = true; // if it hits the end of our buffer..
    }

		p = c;
	}

	*dest = 0;
  *mDest = 0;

	if ( mTrapLog )
	{
		mTrapLog->logMessage(buff);
	}

  if ( mEcho )
  {
    printf("%s", buff );
  }

  HE_ASSERT( mFph );
	if ( mFph )
	{
		fprintf(mFph,"%s",buff);
		fflush(mFph);
	}

#if defined(_XBOX)
	HeI32 dprintf(const char* format,...);
	dprintf(buff);
#endif


	if ( mBufferMessages && lineFeed )
	{
    if ( strlen(mBuffer))
    {
      std::string str = mBuffer;
      mLogMessages.push(str);
      mHaveLogMessages = true;
    }
    mBuffer[0] = 0;
		mDest = mBuffer;
	}

#ifdef WIN32
  OutputDebugStringA(buff);
#endif
}

void Log::LogFile(const char *fmt, ... )
{
	char wbuff[8192];
  wbuff[8191] = 0;
	_vsnprintf(wbuff,8191, fmt, (char *)(&fmt+1));

  HE_ASSERT( mFph );
	if ( mFph )
	{
		fprintf(mFph,"%s",wbuff);
		fflush(mFph);
	}

}


const char * Log::FormatNumber(HeI32 number) // JWR  format this integer into a fancy comma delimited string
{
	char * dest = &mFormat[mIndex*MAXNUMERIC];
	mIndex++;
	if ( mIndex == MAXFNUM ) mIndex = 0;

	char scratch[512];

#if defined (LINUX_GENERIC) || defined(LINUX)
	snprintf(scratch, 10, "%d", number);
#else
	itoa(number,scratch,10);
#endif

	char *str = dest;
	HeU32 len = (HeU32)strlen(scratch);
	for (HeU32 i=0; i<len; i++)
	{
		HeI32 place = (len-1)-i;
		*str++ = scratch[i];
		if ( place && (place%3) == 0 ) *str++ = ',';
	}
	*str = 0;

	return dest;
}


void Log::Flush(void)
{
	if ( mFph )
	{
		fflush(mFph);
	}
}


const char * Log::GetLogMessage(void) // spool report output messages...
{
	const char *ret = 0;

  if ( mLogMessages.empty() )
  {
    mHaveLogMessages = false;
  }
  else
  {
    std::string &str = mLogMessages.front();
    strncpy(mTempBuffer,str.c_str(),1023);
    ret = mTempBuffer;
    mLogMessages.pop();
  }

	return ret;
}


void Log::AddBuffer(char *buffer,const char *fmt, ... )
{
	char wbuff[8192];
  wbuff[8191] = 0;
	_vsnprintf(wbuff, 8191, fmt, (char *)(&fmt+1));

	strcat(buffer,wbuff);


}

void Log::setTrapLog(TrapLog *log)
{ 
  mTrapLog = log; 
};


