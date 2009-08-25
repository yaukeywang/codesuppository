#ifndef RAND_H

#define RAND_H

#include "UserMemAlloc.h"

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
** Personal website: http://jratcliffscarab.blogspot.com
** Coding Website:   http://codesuppository.blogspot.com
** FundRaising Blog: http://amillionpixels.blogspot.com
** Fundraising site: http://www.amillionpixels.us
** New Temple Site:  http://newtemple.blogspot.com
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



//** Random number class and Random number pool class.  This acts as a
//** replacement for the stdlib rand() function.  Why would you want to
//** replace the rand() function?  Because you often want a deteriminstic
//** random number generator that is exactly the same regardless of what
//** machine or compiler you build your code on.  The stdlib rand() function
//** is not guaraenteed to produce the same results on different stdlib
//** implementations.
//**
//** You can also maintain any number of unique random number generators
//** as state machines by instancing this rand class over and over again.
//**
//** The random number pool is a data structure which you allocate if you
//** want to pull items from a data set, randomly, but without any
//** duplications.  A simple example would be a deck of cards.  You would
//** instantiate the random number pool as follows:
//**
//** RandPool deck(52);
//**
//** You would then pull cards from the deck as follows:
//**
//** bool shuffled;
//** NxI32 card = deck.Get(shuffled);
//**
//** This will return a number between 0-51 (representing a card in the deck)
//** without ever reporting the same card twice until the deck has been
//** exhausted.  If the boolean 'shuffled' is true, then the deck was
//** re-shuffled on that call.  This data structure has lots of uses in
//** computer games where you want to randomly select data from a fixed
//** pool size.
//**
//** This code submitted to FlipCode.com on July 23, 2000 by John W. Ratcliff
//** It is released into the public domain on the same date.

#include "UserMemAlloc.h"

class Rand
{
public:

	Rand(NxI32 seed=0)
  {
    mCurrent = seed;
  };

	NxI32 get(void)
  {
    return( (mCurrent = mCurrent * 214013L + 2531011L)  & 0x7fffffff);
  };

  // random number between 0.0 and 1.0
  NxF32 ranf(void)
  {
  	NxI32 v = get()&0x7FFF;
    return (NxF32)v*(1.0f/32767.0f);
  };

  NxF32 ranf(NxF32 low,NxF32 high)
  {
    return (high-low)*ranf()+low;
  }

  void setSeed(NxI32 seed)
  {
    mCurrent = seed;
  };

private:
	NxI32 mCurrent;
};

class RandPool
{
public:
  RandPool(NxI32 size,NxI32 seed)  // size of random number bool.
  {
    mRand.setSeed(seed);       // init random number generator.
    mData = MEMALLOC_NEW(NxI32)[size]; // allocate memory for random number bool.
    mSize = size;
    mTop  = mSize;
    for (NxI32 i=0; i<mSize; i++) mData[i] = i;
  }

  ~RandPool(void)
  {
    delete [] mData;
  };

  // pull a number from the random number pool, will never return the
  // same number twice until the 'deck' (pool) has been exhausted.
  // Will set the shuffled flag to true if the deck/pool was exhausted
  // on this call.
  NxI32 get(bool &shuffled)
  {
    if ( mTop == 0 ) // deck exhausted, shuffle deck.
    {
      shuffled = true;
      mTop = mSize;
    }
    else
      shuffled = false;
    NxI32 entry = mRand.get()%mTop;
    mTop--;
    NxI32 ret      = mData[entry]; // swap top of pool with entry
    mData[entry] = mData[mTop];  // returned
    mData[mTop]  = ret;
    return ret;
  };

	NxF32 ranf(void) { return mRand.ranf(); };

private:
  Rand mRand;  // random number generator.
  NxI32  *mData;  // random number bool.
  NxI32   mSize;  // size of random number pool.
  NxI32   mTop;   // current top of the random number pool.
};


NxF32 ranf(void);
NxF32 ranf(NxF32 low,NxF32 high);

#endif
