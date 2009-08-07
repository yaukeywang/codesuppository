#include "rand.h"


static Rand gRand;


NxF32 ranf(void)
{
  return gRand.ranf();
}

NxF32 ranf(NxF32 low,NxF32 high)
{
  return gRand.ranf(low,high);
}

