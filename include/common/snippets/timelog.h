#ifndef TIME_LOG_H

#define TIME_LOG_H


// creates a log file that is time-stamped.  Useful for server/debug logging of stuff.
// No CPP, the entire implementation is in the header.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#pragma comment(lib,"Ws2_32.lib")
#else
#define _tzset tzset
#define _vsnprintf vsnprintf
#endif

#pragma warning(push)
#pragma warning(disable:4996)

class TimeLog
{
public:
  TimeLog(void)
  {
    mFph = 0;
    mSpec = "TimeLog_";
  }

  TimeLog(const char *spec)
  {
    mSpec = spec;
    mFph = 0;
  }

  ~TimeLog(void)
  {
    endLog();
  }

  void beginLog(const char *fname)
  {
    endLog();
    char date_time[128];
    getDateTime(date_time);
    char scratch[512];
    sprintf(scratch,"%s_%s.log", fname, date_time);
    mFph = fopen(scratch,"wb");
  }

  void endLog(void)
  {
    if ( mFph )
    {
      fclose(mFph);
      mFph = 0;
    }
  }


  static void getDateTime(char *date_time)
  {
  	time_t ltime;
  	struct tm *today;
  	_tzset();
  	time( &ltime );
  	today = localtime( &ltime );
  	strftime( date_time, 128,"%A-%B-%d-%Y-%I-%M-%S-%p", today );
  }

  void logTime(const char *label) // write the current time to the log file.
  {
    checkLog();
    if ( mFph )
    {
      char date_time[128];
      getDateTime(date_time);
      fprintf(mFph,"%s : %s\r\n", label, date_time);
      fflush(mFph);
    }
  }

  void log(const char *fmt, ... )
  {
    checkLog();
    if ( mFph )
    {
    	char wbuff[8192];
      wbuff[8191] = 0;
    	_vsnprintf(wbuff,8191, fmt, (char *)(&fmt+1));
      fprintf(mFph,"%s", wbuff );
      fflush(mFph);
    }
  }

  void checkLog(void)
  {
    if ( mFph == 0 )
    {
      beginLog(mSpec);
    }
  }


  static bool getMachineName(char *name,HeU32 maxLen)
  {
    bool ret = false;

    name[0] = 0;
#ifdef WIN32
    gethostname(name,maxLen);
    ret = true;
#endif
    return ret;
  }

private:
  FILE  *mFph;
  const char *mSpec; // must be a persistent pointer!
};

#pragma warning(pop)

#endif
