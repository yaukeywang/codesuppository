#include "rand.h"


static Rand gRand;


HeF32 ranf(void)
{
  return gRand.ranf();
}

HeF32 ranf(HeF32 low,HeF32 high)
{
  return gRand.ranf(low,high);
}

