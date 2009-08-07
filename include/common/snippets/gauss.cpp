#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "gauss.h"

#define DEFAULT_MIN_MAX 1e10

Gauss::Gauss(void)
{
  mFlags             = 0;
  mMean              = 0;
  mStandardDeviation = 0;
  mMin               = -DEFAULT_MIN_MAX;
  mMax               = +DEFAULT_MIN_MAX;
  Reset();
}


Gauss::Gauss(const char *value)
{
  mFlags             = 0;
  mMean              = 0;
  mStandardDeviation = 0;
  mMin               = -DEFAULT_MIN_MAX;
  mMax               = +DEFAULT_MIN_MAX;
  Reset();
  Set(value);
}


Gauss::Gauss(NxF32 f) // has no random deviation
{
  mFlags             = 0;
  mMean              = f;
  mStandardDeviation = 0;
  mMin               = -DEFAULT_MIN_MAX;
  mMax               = +DEFAULT_MIN_MAX;
  Reset();
}

void Gauss::Reset(void)
{
  mCurrent = 0;
  ClearGaussFlag(GF_SECOND);
  Rand::setSeed(0); // init random number generator
};

NxF32 Gauss::Get(void)
{
  return Get(*this);
};

NxF32 Gauss::Get(Rand &r)
{
  if ( !HasGaussFlag(GF_STDEV) ) return mMean;

  NxF32 v;

  if ( HasGaussFlag(GF_LINEAR) )
  {
    v = mMean + (ranfloat()*mStandardDeviation - (mStandardDeviation*0.5f));
  }
  else
  {
    v = RandGauss(&r)*mStandardDeviation + mMean;
  }

  if ( HasGaussFlag(GF_MIN) )
  {
    if ( v < mMin ) v = mMin;
  }

  if ( HasGaussFlag(GF_MAX) )
  {
    if ( v > mMax ) v = mMax;
  }

  mCurrent = v;

  return v;
};

NxF32 Gauss::GetCurrent(void) const
{
  return mCurrent;
};

// Set from ascii string.
const char * Gauss::Set(const char *arg)
{
  mFlags = 0;
  char *end;
  bool linear;
  strtogmd( (char *)arg, &end, mMean, mStandardDeviation, mMin, mMax, linear );

  if ( mMean != 0.0f ) SetGaussFlag(GF_MEAN);
  if ( mStandardDeviation != 0.0f ) SetGaussFlag(GF_STDEV);
  if ( mMin != -DEFAULT_MIN_MAX )   SetGaussFlag(GF_MIN);
  if ( mMax != +DEFAULT_MIN_MAX )   SetGaussFlag(GF_MAX);

  if ( linear ) SetGaussFlag(GF_LINEAR);

  mCurrent = mMean;

  srand();
  return end;
};

// convert gaussian into valid gaussian string.
void Gauss::GetString(String &str) const
{
  char scratch[256];

  char prefix[2] = { 0, 0 };

  if ( HasGaussFlag(GF_LINEAR) ) prefix[0] = '!';

  if ( HasGaussFlag( (GaussFlag) (GF_STDEV | GF_MIN | GF_MAX) ) )
  {
    if ( HasGaussFlag( (GaussFlag) (GF_MIN | GF_MAX) ) )
    {
      sprintf(scratch,"%s%0.3f:%0.3f<%0.3f:%0.3f>",prefix,mMean,mStandardDeviation,mMin,mMax);
    }
    else
    {
      sprintf(scratch,"%s%0.3f:%0.3f",prefix,mMean,mStandardDeviation);
    }
  }
  else
  {
    sprintf(scratch,"%s%0.3f",prefix,mMean);
  }
  str = scratch;
};

// generate fresh gaussian pair or return last valid one.
NxF32 Gauss::RandGauss(Rand *r)
{
  if( HasGaussFlag(GF_SECOND) )
  {
    ClearGaussFlag(GF_SECOND);
    return mGauss2;
  }

  NxF32 x1;
  NxF32 x2;
  NxF32 w;

  do
  {
    x1 = 2.0f * r->ranf() - 1.0f;
    x2 = 2.0f * r->ranf() - 1.0f;
    w = x1 * x1 + x2 * x2;
  } while ( w >= 1.0f );

  w = sqrtf( (-2.0f * logf( w ) ) / w );

  mGauss1 = x1 * w;
  mGauss2 = x2 * w;

  SetGaussFlag(GF_SECOND);

  return mGauss1;
};


// convert string to gaussian number.  Return code
// indicates number of arguments found.
NxI32 Gauss::strtogmd(const char* spec,
                    char** end,
                    NxF32& mean,
                    NxF32& deviation,
                    NxF32& min,
                    NxF32& max,
                    bool &linear ) const
{
  char* pos;
  min  = -DEFAULT_MIN_MAX;
  max  = +DEFAULT_MIN_MAX;
  linear = false;

  if ( *spec == '!' )
  {
    linear = true;
    spec++;
  }

  mean = (NxF32) strtod( (char*) spec, &pos );

  deviation = 0;

  if( pos == spec )
  {
    mean = 0;
    if( end != 0 )
    {
      *end = (char *)spec;
    }
    return 0;
  }
  if( *pos != ':' )
  {
    if( end != 0 )
    {
      *end = pos;
    }
    return 1;
  }
  char* devp = pos + 1;
  deviation = (NxF32) strtod( devp, &pos );
  if( pos == devp )
  {
    if( end != 0 )
    {
      *end = devp - 1;
    }
    return 1;
  }

  if( *pos != '[' && *pos != '<' )
  {
    if( end != 0 )
    {
      *end = pos;
    }
    return 2;
  }

  char* minp = pos + 1;
  char* maxp;
  min = (NxF32) strtod( minp, &maxp );
  if( *maxp != ',' && *maxp != ':' )
  {
    if( end != 0 )
    {
      *end = pos;
    }
    return 2;
  }
  ++maxp;
  char* endp;
  max = (NxF32) strtod( maxp, &endp );
  if( *endp != ']' && *endp != '>' )
  {
    if( end != 0 )
    {
      *end = pos;
    }
    return 2;
  }
  if( minp == maxp - 1 )
  {
    min = -DEFAULT_MIN_MAX;
  }
  if( maxp == endp )
  {
    max = +DEFAULT_MIN_MAX;
  }
  if( end != 0 )
  {
    *end = endp + 1;
  }
  return 4;
};

static Rand frand;

NxF32 ranfloat(void)
{
  return frand.ranf();
}



void Gauss::srand(void)
{
  static Rand rand;
  Rand::setSeed( rand.get() ); // randomize
}


