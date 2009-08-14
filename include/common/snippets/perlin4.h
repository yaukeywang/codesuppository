#ifndef PERLIN4_H

#define PERLIN4_H

#include "UserMemAlloc.h"

class Perlin4
{
public:

  virtual NxF32 get(NxF32 x,NxF32 y) = 0;

};


// envelope
//  NULL,      // 0 do nothing, leave it as is.
//  SQUARED1,  // 1
//  SQUARED2,  // 2
//  SQUARED3,  // 3
//  ATAN1,     // 4
//  ATAN2,     // 5
//  ATAN3,     // 6
//  ACOS1,     // 7
//  ACOS2,     // 8
//  ACOS3,     // 9
//  ACOS4,     // 10
//  COS1,      // 11
//  COS2,      // 12
//  EXP1,      // 13
//  EXP2,      // 14
//  EXP3,      // 15
//  EXP4,      // 16
//  EXP5,      // 17
//  LOG1,      // 18
//  LOG2,      // 19
//  LOG3,      // 20
//  TANH,      // 21
//  SQRT,      // 22
// octaves
// frequency
// amplitude
// seed
Perlin4 * createPerlin4(const char *options="envelope1=tanh,octaves1=6,frequency1=0.017,amplitude1=1,seed1=0,envelope2=log1,octaves2=6,frequency2=0.021,amplitude=1,seed=0,envelope3=squared3,octaves3=6,frequency3=0.023,amplitude=1,seed=0,envelope4=null");
void      releasePerlin4(Perlin4 *p);

#endif
