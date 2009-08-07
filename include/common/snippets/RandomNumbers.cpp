/* HeroEngine, Copyright 2002-2008 Simutronics, Corp. All rights reserved. */


#include "RandomNumbers.h"

#pragma warning(disable:4100)


namespace Simutronics
{

// mtrand.h
// C++ include file for MT19937, with initialization improved 2002/1/26.
// Coded by Takuji Nishimura and Makoto Matsumoto.
// Ported to C++ by Jasper Bedaux 2003/1/1 (see http://www.bedaux.net/mtrand/).
// The generators returning floating point numbers are based on
// a version by Isaku Wada, 2002/01/09
//
// Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. The names of its contributors may not be used to endorse or promote
//    products derived from this software without specific prior written
//    permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Any feedback is very welcome.
// http://www.math.keio.ac.jp/matumoto/emt.html
// email: matumoto@math.keio.ac.jp
//
// Feedback about the C++ port should be sent to Jasper Bedaux,
// see http://www.bedaux.net/mtrand/ for e-mail address and info.

class MTRand_int32 { // Mersenne Twister random number generator
public:
// default constructor: uses default seed only if this is the first instance
  MTRand_int32()
  {
   if (!init) seed(5489UL);
    init = true;
  }
// constructor with 32 bit int as seed
  MTRand_int32(unsigned long s)
  {
    seed(s);
    init = true;
  }
// constructor with array of size 32 bit ints as seed
  MTRand_int32(const unsigned long* array, int size) { seed(array, size); init = true; }
// the two seed functions
  void seed(unsigned long); // seed with 32 bit integer
  void seed(const unsigned long*, int size); // seed with array
// overload operator() to make this a generator (functor)
  unsigned long operator()() { return rand_int32(); }
// 2007-02-11: made the destructor virtual; thanks "double more" for pointing this out
  virtual ~MTRand_int32() {} // destructor
protected:
  unsigned long rand_int32(); // generate 32 bit random integer
private:
  static const int n = 624, m = 397; // compile time constants
// the variables below are static (no duplicates can exist)
  static unsigned long state[n]; // state vector array
  static int p; // position in state array
  static bool init; // true if init function is called
// private functions used to generate the pseudo random numbers
  unsigned long twiddle(unsigned long, unsigned long); // used by gen_state()
  void gen_state(); // generate new state
// make copy constructor and assignment operator unavailable, they don't make sense
  MTRand_int32(const MTRand_int32&); // copy constructor not defined
  void operator=(const MTRand_int32&); // assignment operator not defined
};

// inline for speed, must therefore reside in header file
inline unsigned long MTRand_int32::twiddle(unsigned long u, unsigned long v)
{
  return (((u & 0x80000000UL) | (v & 0x7FFFFFFFUL)) >> 1) ^ ((v & 1UL) ? 0x9908B0DFUL : 0x0UL);
}

inline unsigned long MTRand_int32::rand_int32() { // generate 32 bit random int
  if (p == n) gen_state(); // new state vector needed
// gen_state() is split off to be non-inline, because it is only called once
// in every 624 calls and otherwise irand() would become too big to get inlined
  unsigned long x = state[p++];
  x ^= (x >> 11);
  x ^= (x << 7) & 0x9D2C5680UL;
  x ^= (x << 15) & 0xEFC60000UL;
  return x ^ (x >> 18);
}

// generates double floating point numbers in the half-open interval [0, 1)
class MTRand : public MTRand_int32 {
public:
  MTRand() : MTRand_int32() {}
  MTRand(unsigned long seed) : MTRand_int32(seed) {}
  MTRand(const unsigned long* seed, int size) : MTRand_int32(seed, size) {}
  ~MTRand() {}
  double operator()() {
    return static_cast<double>(rand_int32()) * (1. / 4294967296.); } // divided by 2^32
private:
  MTRand(const MTRand&); // copy constructor not defined
  void operator=(const MTRand&); // assignment operator not defined
};

// generates double floating point numbers in the closed interval [0, 1]
class MTRand_closed : public MTRand_int32 {
public:
  MTRand_closed() : MTRand_int32() {}
  MTRand_closed(unsigned long seed) : MTRand_int32(seed) {}
  MTRand_closed(const unsigned long* seed, int size) : MTRand_int32(seed, size) {}
  ~MTRand_closed() {}
  double operator()() {
    return static_cast<double>(rand_int32()) * (1. / 4294967295.); } // divided by 2^32 - 1
private:
  MTRand_closed(const MTRand_closed&); // copy constructor not defined
  void operator=(const MTRand_closed&); // assignment operator not defined
};

// generates double floating point numbers in the open interval (0, 1)
class MTRand_open : public MTRand_int32 {
public:
  MTRand_open() : MTRand_int32() {}
  MTRand_open(unsigned long seed) : MTRand_int32(seed) {}
  MTRand_open(const unsigned long* seed, int size) : MTRand_int32(seed, size) {}
  ~MTRand_open() {}
  double operator()() {
    return (static_cast<double>(rand_int32()) + .5) * (1. / 4294967296.); } // divided by 2^32
private:
  MTRand_open(const MTRand_open&); // copy constructor not defined
  void operator=(const MTRand_open&); // assignment operator not defined
};

// generates 53 bit resolution doubles in the half-open interval [0, 1)
class MTRand53 : public MTRand_int32 {
public:
  MTRand53() : MTRand_int32() {}
  MTRand53(unsigned long seed) : MTRand_int32(seed) {}
  MTRand53(const unsigned long* seed, int size) : MTRand_int32(seed, size) {}
  ~MTRand53() {}
  double operator()() {
    return (static_cast<double>(rand_int32() >> 5) * 67108864. +
      static_cast<double>(rand_int32() >> 6)) * (1. / 9007199254740992.); }
private:
  MTRand53(const MTRand53&); // copy constructor not defined
  void operator=(const MTRand53&); // assignment operator not defined
};

// mtrand.cpp, see include file mtrand.h for information

// non-inline function definitions and static member definitions cannot
// reside in header file because of the risk of multiple declarations

// initialization of static private members
unsigned long MTRand_int32::state[n] = {0x0UL};
int MTRand_int32::p = 0;
bool MTRand_int32::init = false;

void MTRand_int32::gen_state()
{ // generate new state vector
  for (int i = 0; i < (n - m); ++i)
    state[i] = state[i + m] ^ twiddle(state[i], state[i + 1]);
  for (int i = n - m; i < (n - 1); ++i)
    state[i] = state[i + m - n] ^ twiddle(state[i], state[i + 1]);
  state[n - 1] = state[m - 1] ^ twiddle(state[n - 1], state[0]);
  p = 0; // reset position
}

void MTRand_int32::seed(unsigned long s)
{  // init by 32 bit seed
  state[0] = s & 0xFFFFFFFFUL; // for > 32 bit machines
  for (int i = 1; i < n; ++i)
  {
    state[i] = 1812433253UL * (state[i - 1] ^ (state[i - 1] >> 30)) + i;
// see Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier
// in the previous versions, MSBs of the seed affect only MSBs of the array state
// 2002/01/09 modified by Makoto Matsumoto
    state[i] &= 0xFFFFFFFFUL; // for > 32 bit machines
  }
  p = n; // force gen_state() to be called for next random number
}

void MTRand_int32::seed(const unsigned long* array, int size)
{ // init by array
  seed(19650218UL);
  int i = 1, j = 0;
  for (int k = ((n > size) ? n : size); k; --k)
  {
    state[i] = (state[i] ^ ((state[i - 1] ^ (state[i - 1] >> 30)) * 1664525UL)) + array[j] + j; // non linear
    state[i] &= 0xFFFFFFFFUL; // for > 32 bit machines
    ++j; j %= size;
    if ((++i) == n)
    {
      state[0] = state[n - 1]; i = 1;
    }
  }
  for (int k = n - 1; k; --k)
  {
    state[i] = (state[i] ^ ((state[i - 1] ^ (state[i - 1] >> 30)) * 1566083941UL)) - i;
    state[i] &= 0xFFFFFFFFUL; // for > 32 bit machines
    if ((++i) == n) { state[0] = state[n - 1]; i = 1; }
  }
  state[0] = 0x80000000UL; // MSB is 1; assuring non-zero initial array
  p = n; // force gen_state() to be called for next random number
}


static MTRand        frand32;
static MTRand53      frand53;
static MTRand_int32  irand32;

static NxU64 getRTDSC(void)
{
  NxU64 tm = 0;
#if WIN32
  	__asm
	  {
		  push	edx
  		push	eax
	  	_emit 0x0f
  		_emit 0x31
	  	mov		dword ptr [tm],eax
  		mov		dword ptr [tm+4],edx
  		pop eax
  		pop edx
  	}
#endif
  return tm;
}

static NxU32 getSeed(void)
{
  NxU64 seed = getRTDSC();
  const NxU32 *sptr = (const NxU32 *)&seed;
  NxU32 ret = sptr[0]^sptr[1];
  return ret;
}


static void init(void)
{
  static bool first = true;
  if ( first )
  {
    frand32.seed( getSeed() );
    frand53.seed( getSeed() );
    irand32.seed( getSeed() );
    first = false;
  }
}


NxF32 RandomNumbers::get_float(NxF32 hi,NxF32 lo)
{
  init();

  NxF32 ret = (NxF32)frand32();

  ret = ret*(hi-lo)+lo;

  return ret;
}

NxI32 RandomNumbers::get_int(NxI32 hi,NxI32 lo)
{
  init();
  NxI32 ret = lo;
  NxI32 diff = (hi-lo);
  if ( diff > 0 )
  {
    ret = irand32()&NX_MAX_I32;
    ret = (ret%diff)+lo;
  }
  return ret;
}

NxI32 RandomNumbers::get_int_inclusive( NxI32 hi,NxI32 lo )
{
  init();
  NxU32 diff = hi-lo+1;
  NxU32 ret = irand32();
  if ( diff > 0 )
  {
    ret %= diff;
  }
  return ret+lo;
}

NxU32 RandomNumbers::get_uint(NxU32 hi,NxU32 lo)
{
  init();
  NxU32 ret = lo;
  NxU32 diff = hi-lo;
  if ( diff > 0 )
  {
    ret = (irand32()%diff)+lo;
  }
  return ret;
}

NxU32 RandomNumbers::get_uint_inclusive( NxU32 hi,NxU32 lo )
{
  init();
  NxI32 diff = hi-lo+1;
  NxI32 ret = irand32();
  if ( diff > 0 )
  {
    ret %= diff;
  }
  return ret+lo;
}

NxI64 RandomNumbers::get_int64(NxI64 hi,NxI64 lo)
{
  init();
  NxI64 ret = lo;
  NxI64 diff = hi-lo;
  if ( diff > 0 )
  {
    NxI64 v;
    NxU32 i1 = irand32();
    NxU32 i2 = irand32();
    NxU32 *dest = (NxU32 *)&v;
    dest[0] = i1;
    dest[1] = i2;
    if ( v < 0 ) v*=-1;
    ret = (v%diff)+lo;
  }
  return ret;
}

NxI64 RandomNumbers::get_int64_inclusive( NxI64 hi,NxI64 lo )
{
  init();
  NxU64 ret = lo;
  NxU32 *dest = (NxU32 *)&ret;
  dest[0] = irand32();
  dest[1] = irand32();
  NxU64 diff = hi-lo+1;
  if ( diff > 0 )
  {
    ret %= diff;
  }
  return ret+lo;
}

NxU64 RandomNumbers::get_uint64(NxU64 hi,NxU64 lo)
{
  init();
  NxU64 ret = lo;
  NxU64 diff = hi-lo;
  if ( diff > 0 )
  {
    NxU64 v;
    NxU32 i1 = irand32();
    NxU32 i2 = irand32();
    NxU32 *dest = (NxU32 *)&v;
    dest[0] = i1;
    dest[1] = i2;
    ret = (v%diff)+lo;
  }
  return ret;

}

NxU64 RandomNumbers::get_uint64_inclusive( NxU64 hi,NxU64 lo )
{
  init();
  NxU64 ret = lo;
  NxU32 *dest = (NxU32 *)&ret;
  dest[0] = irand32();
  dest[1] = irand32();
  NxU64 diff = hi-lo+1;
  if ( diff > 0 )
  {
    ret %= diff;
  }
  return ret+lo;
}

}; // end of namespace
