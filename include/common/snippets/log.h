#ifndef LOG_H

#define LOG_H

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


#include <vector>
#include <stdio.h>
#include <string>
#include <queue>

#include "UserMemAlloc.h"

#define MAXNUMERIC 32  // JWR  support up to 16 32 character long numeric formated strings
#define MAXFNUM    16

#if defined(LINUX)
#define _vsnprintf vsnprintf
#endif

class TrapLog
{
public:
  virtual void logMessage(const char *str) = 0;
};

typedef USER_STL::queue< std::string > StringQueue;

class Log
{
public:

	Log(void);
	Log(const char *fname,bool buffer=true);
	~Log(void);

	void Display(const char *fmt, ...);
	void LogFile(const char *fmt, ...);

	void AddBuffer(char *buffer,const char *fmt, ...);

	const char * FormatNumber(NxI32 number); // JWR  format this integer into a fancy comma delimited string

	void Flush(void);

	const char * GetLogMessage(void); // spool report output messages...
	bool         HasLogMessage(void) const { return mHaveLogMessages; };

  void setTrapLog(TrapLog *log);
  TrapLog * getTrapLog(void) const { return mTrapLog; };

  void setEcho(bool state) { mEcho = state; };
  bool getEcho(void) const { return mEcho; };
  void setBufferMessages(bool state) { mBufferMessages = state; };
  bool getBufferMessages(void) const { return mBufferMessages; };

private:
	NxI32              mLogLevel;
	FILE            *mFph;
	NxI32              mIndex;
	char             mFormat[MAXNUMERIC*MAXFNUM];
	bool             mBufferMessages;
	NxI32              mLogFrame;
  char            *mDest;
	char             mBuffer[1024];
  char             mTempBuffer[1024];
	bool             mHaveLogMessages;
	NxI32              mLogIndex;
  StringQueue      mLogMessages;
	TrapLog         *mTrapLog;
  bool             mEcho;
};

extern Log *gLog;

#endif
