#ifndef MULTI_FLOAT_H

#define MULTI_FLOAT_H

#include <float.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "common/ttmath/ttmath.h"
#include "fixedpoint.h"

typedef NxF32                        SmallFloat;
typedef HeF64                       MediumFloat;
typedef ttmath::Big<1,3>             BigFloat;


enum MultiFloatType
{
  MFT_SMALL,
  MFT_MEDIUM,
  MFT_BIG,
  MFT_FIXED32,
  MFT_LAST,
  MFT_DEFAULT = MFT_MEDIUM,
};

inline void toString(const BigFloat &f,char *dest)
{
  std::string num;
  f.ToString(num);
  strcpy(dest,num.c_str());
}

inline HeF64 getDouble(const BigFloat &v)
{
  HeF64 ret;
  v.ToDouble(ret);
  return ret;
}

#endif
