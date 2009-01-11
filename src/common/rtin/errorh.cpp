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

  HeI32 hwid = rtin.GetWidth();
  HeI32 hhit = rtin.GetHeight();

  mWidth  = hwid/SAMPLESIZE;
  mHeight = hhit/SAMPLESIZE;

  mError  = MEMALLOC_NEW_ARRAY(float,mWidth*mHeight)[mWidth*mHeight];

  for (HeI32 y=0; y<mHeight; y++)
  {
    for (HeI32 x=0; x<mWidth; x++)
    {

      HeF32 err = GetError(x,y,rtin,topo);


      mError[ y*mWidth+x ] = err;
    }
  }

  printf("ErrorHeuristic reference table is %d,%d pixels.\n",mWidth,mHeight);
}

ErrorHeuristic::~ErrorHeuristic(void)
{
  delete mError;
}



HeF32 ErrorHeuristic::Get(HeI32 x,HeI32 y) const  // get error from this heightfield location
{
  HeF32 e = 1;

  x = x/SAMPLESIZE;
  y = y/SAMPLESIZE;

  if ( x >= 0 && x < mWidth &&
       y >= 0 && y < mHeight )
  {
    e*=mError[y*mWidth+x];
  }

  return e; // default value.
}

HeF32 ErrorHeuristic::GetError(HeI32 x,HeI32 y,const RtinObj & /*rtin*/,TopoRtin &topo) const
{
  HeI32 wx = x*SAMPLESIZE;
  HeI32 wy = y*SAMPLESIZE;

  HeI32 icount = SAMPLELEN*SAMPLELEN*2*3;

  HeU16 *work = MEMALLOC_NEW_ARRAY(unsigned short,icount)[icount];

  HeU16 *result = topo.BuildIndices(work,wx,wy);

  #define BASELINE 50
  HeF32 tcount = BASELINE;

  if ( result )
  {
    HeI32 count = (result-work)/3; // total number of triangles created.
//    printf("(%d,%d) = %d triangles.\n",x,y,count);
    tcount = HeF32(count);
  }

  delete work;

  HeF32 err = tcount/BASELINE;

  if ( err < 0.1f ) err = 0.1f;

  return err;
}
