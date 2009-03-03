#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <math.h>
#include <wchar.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#if defined(LINUX)
#include "linux_compat.h"
#endif

#pragma warning(disable:4996)


static inline void getDateTime(char *da,char *tim)
{
	time_t ltime;
	struct tm *today;

	_tzset();
	time( &ltime );
	today = localtime( &ltime );

	strftime( da, 128,"%A, %B %d, %Y", today );
	strftime( tim, 128,"%I:%M %p",     today );
}

#define MAXNUMERIC 32  // JWR  support up to 16 32 character long numeric formated strings
#define MAXFNUM    16

static	char  gFormat[MAXNUMERIC*MAXFNUM];
static int    gIndex=0;

static const char * formatNumber(int number) // JWR  format this integer into a fancy comma delimited string
{
  char * dest = &gFormat[gIndex*MAXNUMERIC];
  gIndex++;
  if ( gIndex == MAXFNUM ) gIndex = 0;

  char scratch[512];

#if defined (LINUX_GENERIC)
  snprintf(scratch, 10, "%d", number);
#else
  itoa(number,scratch,10);
#endif

  char *str = dest;
  unsigned int len = (unsigned int)strlen(scratch);
  for (unsigned int i=0; i<len; i++)
  {
    int place = (len-1)-i;
    *str++ = scratch[i];
    if ( place && (place%3) == 0 ) *str++ = ',';
  }
  *str = 0;

  return dest;
}



static void stripFraction(char *fraction)
{
  size_t len = strlen(fraction);
  if ( len > 0 )
  {
    len--;
    while ( len )
    {
      if ( fraction[len] == '0' )
      {
        fraction[len] = 0;
        len--;
      }
      else
      {
        break;
      }
    }
  }
}

static const char * getFloat(float v)
{
  static int index=0;
  static char _ret[64*8];

  char *ret = &_ret[index*64];
  index++;
  if ( index == 8 )
  {
    index = 0;
  }

  int ivalue = (int)v;

  if ( ivalue == 0 )
  {
    char fraction[512];
    sprintf(fraction,"%0.9f", v );
    stripFraction(fraction);
    strcpy(ret,fraction);
  }
  else
  {
    v-=(float)ivalue;
    v = fabsf(v);
    if (v < 0.00001f ) 
      v = 0;


    const char *temp = formatNumber(ivalue);
    if ( v != 0 )
    {
      char fraction[512];
      sprintf(fraction,"%0.9f", v );
      assert( fraction[0] == '0' );
      assert( fraction[1] == '.' );
      stripFraction(fraction);
      char scratch[512];
      sprintf(scratch,"%s%s", temp, &fraction[1] );
      strcpy(ret,scratch);
    }
    else
    {
      strcpy(ret,temp);
    }
  }
  if ( strcmp(ret,"0.") == 0 )
  {
    ret = "0";
  }
  return ret;
}


#include "SAS_log.h"


namespace SPATIAL_AWARENESS_SYSTEM
{

SAS_Log::SAS_Log(SpatialAwarenessObserver *observer)
{
  mParent = Factory::Create(SAS_LAZY_KDTREE,observer);

  char date[512];
  char time[512];

  char fname[512];

  getDateTime(date,time);

  sprintf(fname,"SAS_LOG_%s_%s.txt", date, time );

  char *scan = fname;
  while ( *scan )
  {
    char c = *scan;
    if ( c == 32 ) c = '_';
    if ( c == ',' ) c = '_';
    if ( c == ':' ) c = '_';
    *scan = c;
    scan++;
  }
  strupr(fname);

  mFph = fopen(fname,"wb");
}

SAS_Log::~SAS_Log(void)
{
  Factory::Destroy(mParent);
  if ( mFph )
  {
    fclose(mFph);
  }
}

HeSize SAS_Log::getMemoryUsage()
{
  return mParent->getMemoryUsage() + sizeof *this;
}

void SAS_Log::Pump(SecondsType time_elapsed_since_last_pump)
{
  if ( mFph )
  {
    fprintf(mFph,"Pump(%s)\r\n", getFloat((float)time_elapsed_since_last_pump) );
    fflush(mFph);
  }
  mParent->Pump(time_elapsed_since_last_pump);
}

bool SAS_Log::SetActive(ActiveType new_active_flag)
{
  bool ret = false;

  if ( mFph )
  {
    fprintf(mFph,"SetActive(%s)\r\n", new_active_flag ? "TRUE" : "FALSE" );
    fflush(mFph);
  }
  ret = mParent->SetActive(new_active_flag);

  return ret;
}

bool SAS_Log::SetName(NameType new_name)
{
  bool ret = false;

  if ( mFph )
  {
    char scratch[512];
   	wcstombs(scratch,new_name, 512 );
    fprintf(mFph,"SetName(%s)\r\n", scratch );
    fflush(mFph);
  }
  ret = mParent->SetName(new_name);

  return ret;
}

bool SAS_Log::SetUpdatePeriod(SecondsType new_update_period)
{
  bool ret = false;

  if ( mFph )
  {
    fprintf(mFph,"SetUpdatePeriod(%s)\r\n", getFloat((float)new_update_period ));
    fflush(mFph);
  }
  ret = mParent->SetUpdatePeriod(new_update_period);

  return ret;
}

bool SAS_Log::AddEntity(ID entity_ID)
{
  bool ret = false;

  if ( mFph )
  {
    fprintf(mFph,"AddEntity(%lld)\r\n", entity_ID );
    fflush(mFph);
  }
  ret = mParent->AddEntity(entity_ID);

  return ret;
}

bool SAS_Log::DeleteEntity(ID entity_ID,bool flushMessagesImmediately)
{
  bool ret = false;

  if ( mFph )
  {
    fprintf(mFph,"DeleteEntity(%lld,%s)\r\n", entity_ID, flushMessagesImmediately ? "true" : "false" );
    fflush(mFph);
  }
  ret = mParent->DeleteEntity(entity_ID,flushMessagesImmediately);

  return ret;
}

bool SAS_Log::UpdateEntityPosition(ID entity_ID, const float position_vec3[3])
{
  bool ret = false;

  if ( mFph )
  {
    fprintf(mFph,"UpdateEntityPosition(%lld,%s,%s,%s)\r\n", entity_ID, getFloat(position_vec3[0]), getFloat(position_vec3[1]), getFloat(position_vec3[2]));
    fflush(mFph);
  }
  ret = mParent->UpdateEntityPosition(entity_ID,position_vec3);

  return ret;
}

bool SAS_Log::UpdateEntityAwarenessRange(ID entity_ID, float range_f)
{
  bool ret = false;

  if ( mFph )
  {
    fprintf(mFph,"UpdateEntityAwarenessRange(%lld,%s)\r\n", entity_ID, getFloat(range_f) );
    fflush(mFph);
  }
  ret = mParent->UpdateEntityAwarenessRange(entity_ID,range_f);

  return ret;
}

unsigned int SAS_Log::iterateAwareness(ID entity,IDVector &list)
{
  unsigned int ret = 0;

  if ( mFph )
  {
    fprintf(mFph,"iterateAwareness(%lld)\r\n", entity );
    fflush(mFph);
  }
  ret = mParent->iterateAwareness(entity,list);

  return ret;
}

unsigned int SAS_Log::iterateAll(SpatialAwarenessIteratorCallback *callback)
{
  unsigned int ret = 0;

  if ( mFph )
  {
    fprintf(mFph,"iterateAll(%08IX)\r\n", callback );
    fflush(mFph);
  }
  ret = mParent->iterateAll(callback);

  return ret;
}

bool SAS_Log::setProperty(const char *key,const char *value)
{
  bool ret = false;

  if ( mFph )
  {
    fprintf(mFph,"setProperty(%s,%s)\r\n", key, value );
    fflush(mFph);
  }
  ret = mParent->setProperty(key,value);

  return ret;
}

bool SAS_Log::setProperty(ID entity,const char *key,const char *value)
{
  bool ret = false;

  if ( mFph )
  {
    fprintf(mFph,"setProperty(%lld,%s,%s)\r\n", entity, key, value );
    fflush(mFph);
  }
  ret = mParent->setProperty(entity,key,value);

  return ret;
}

SpatialAwarenessStrategy SAS_Log::getStrategy(void) const
{
  SpatialAwarenessStrategy ret;

  ret = SAS_LOG;

  return ret;
}

bool SAS_Log::UpdateEntityAwarenessTime(ID entity_ID,SecondsType time)
{
  bool ret = false;

  if ( mFph )
  {
    fprintf(mFph,"UpdateEntityAwarenessTime(%lld,%s)\r\n", entity_ID, getFloat((float)time) );
    fflush(mFph);
  }
  ret = mParent->UpdateEntityAwarenessTime(entity_ID,time);

  return ret;
}

unsigned int SAS_Log::iterateAwareOf(ID entity,IDVector &list)
{
  unsigned int ret = 0;

  if ( mFph )
  {
    fprintf(mFph,"iterateAwarenessOf(%lld)\r\n", entity );
    fflush(mFph);
  }

  ret = mParent->iterateAwareOf(entity,list);

  return ret;

}



}; // end of namespace
