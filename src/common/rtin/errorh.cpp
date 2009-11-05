#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "errorh.h"
#include "rtinobj.h"
#include "toportin.h"


ErrorHeuristic::ErrorHeuristic(const RtinObj &rtin,TopoRtin &topo)
{

  topo.SetBlockSize(SAMPLESIZE);
  topo.SetLod(false);
  topo.SetErrorThreshold(20); // baseline
  topo.SetIgnoreSealevel(false);
  topo.SetErrorHeuristic(0);
  topo.SetDistanceThreshold(8);
  topo.SetEye(0,0);

  NxI32 hwid = rtin.GetWidth();
  NxI32 hhit = rtin.GetHeight();

  mWidth  = hwid/SAMPLESIZE;
  mHeight = hhit/SAMPLESIZE;

  mError  = MEMALLOC_NEW_ARRAY(float,mWidth*mHeight)[mWidth*mHeight];

  for (NxI32 y=0; y<mHeight; y++)
  {
    for (NxI32 x=0; x<mWidth; x++)
    {

      NxF32 err = GetError(x,y,rtin,topo);


      mError[ y*mWidth+x ] = err;
    }
  }

  printf("ErrorHeuristic reference table is %d,%d pixels.\n",mWidth,mHeight);
}

ErrorHeuristic::~ErrorHeuristic(void)
{
  delete mError;
}



NxF32 ErrorHeuristic::Get(NxI32 x,NxI32 y) const  // get error from this heightfield location
{
  NxF32 e = 1;

  x = x/SAMPLESIZE;
  y = y/SAMPLESIZE;

  if ( x >= 0 && x < mWidth &&
       y >= 0 && y < mHeight )
  {
    e*=mError[y*mWidth+x];
  }

  return e; // default value.
}

NxF32 ErrorHeuristic::GetError(NxI32 x,NxI32 y,const RtinObj & /*rtin*/,TopoRtin &topo) const
{
  NxI32 wx = x*SAMPLESIZE;
  NxI32 wy = y*SAMPLESIZE;

  NxI32 icount = SAMPLELEN*SAMPLELEN*2*3;

  NxU16 *work = MEMALLOC_NEW_ARRAY(unsigned short,icount)[icount];

  NxU16 *result = topo.BuildIndices(work,wx,wy);

  #define BASELINE 50
  NxF32 tcount = BASELINE;

  if ( result )
  {
    NxI32 count = (result-work)/3; // total number of triangles created.
//    printf("(%d,%d) = %d triangles.\n",x,y,count);
    tcount = NxF32(count);
  }

  delete work;

  NxF32 err = tcount/BASELINE;

  if ( err < 0.1f ) err = 0.1f;

  return err;
}
