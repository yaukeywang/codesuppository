#ifndef GAUSS_H

#define GAUSS_H

// ** This class is a Gaussian Random Number Generator
// ** A Gaussian number is expressed as having a mean and standard
// ** deviation and generates random numbers which conform to the standard
// ** bell curve distribution.  Additionally this class applies an optional
// ** minimum and maximum clamping range for all numbers generated.
// ** The Guassian class inherits the deterministic random number class.
// ** Therefore all gaussian sequences can be deterministic given the same
// ** initial random number seed.
// **
// ** The ASCII format for a gaussian number is as follows:
// **
// ** mean:std<min:max>
// **
// ** Examples of valid gaussian numbers.
// **
// **  30          (Means 30 with a standard deviation of 0)
// **  30:5        (Means 30 with a standard deviation of +/- 5)
// **  30:5<10>    (Means 30, stdev +/-5 minimum value of 10
// **  30:5<10:40> (Means 30, stdev +/-5, min value 10 max value 40)
// ** !30:5        (Means 30, +/-5 using a straight linear function!

#include <string>

#include "UserMemAlloc.h"
#include "rand.h"

typedef std::string String;

enum GaussFlag
{
  GF_MEAN    = (1<<0),
  GF_STDEV   = (1<<1),
  GF_MIN     = (1<<2),
  GF_MAX     = (1<<3),
  GF_LINEAR  = (1<<4),
  GF_SECOND  = (1<<5)
};

// Implementation of gaussian numbers.
class Gauss : public Rand
{
public:
  Gauss(void);
  Gauss(const char *value);
  Gauss(NxF32 f); // has no random deviation

  void SetGaussFlag(GaussFlag f)
  {
    mFlags|=f;
  }

  void ClearGaussFlag(GaussFlag f)
  {
    mFlags&=~f;
  }

  bool HasGaussFlag(GaussFlag f) const
  {
    if ( mFlags &f ) return true;
    return false;
  }


  NxF32 Get(Rand &r); // generate *deterministic* gaussian number using a specific random number seed.
  NxF32 Get(void);    // generate a random number
  NxF32 GetCurrent(void) const; // last generated value.

  void GetString(String &str) const; // get string representation

  const char * Set(const char *arg); // set from asciiz string.

  NxF32 RandGauss(Rand *r); // construct and return gaussian number.

  // convert string to gaussian number.  Return code
  // indicates number of arguments found.
  NxI32 strtogmd(const char* spec,
               char** end,
               NxF32& mean,
               NxF32& deviation,
               NxF32& min,
               NxF32& max,
               bool &linear ) const;

  NxF32 GetMean(void)              const { return mMean; };
  NxF32 GetStandardDeviation(void) const { return mStandardDeviation; };
  NxF32 GetMin(void)               const { return mMin; };
  NxF32 GetMax(void)               const { return mMax; };

  void srand(void);

  void Reset(void);

  void SetMin(NxF32 m)
  {
    mMin = m;
    SetGaussFlag(GF_MIN);
  }

  void SetMax(NxF32 m)
  {
    mMax = m;
    SetGaussFlag(GF_MAX);
  }

  void SetMean(NxF32 m)
  {
    mMean = m;
    if ( mMean == 0.0f )
    {
      ClearGaussFlag(GF_MEAN);
    }
    else
    {
      SetGaussFlag(GF_MAX);
    }
  }

  void SetStandardDeviation(NxF32 sd)
  {
    mStandardDeviation = sd;
    if ( sd == 0.0 )
      ClearGaussFlag(GF_STDEV);
    else
      SetGaussFlag(GF_STDEV);
  }

private:
  NxI32   mFlags;
  NxF32 mMean;               // gaussian number has mean and
  NxF32 mStandardDeviation;  // standard deviation, also
  NxF32 mMin;                // min/max clamping values
  NxF32 mMax;

  NxF32 mCurrent;           // last got value.
  NxF32 mGauss1;            // 1st gaussian
  NxF32 mGauss2;            // 2nd gaussian
};

NxF32 ranfloat(void);

#endif
