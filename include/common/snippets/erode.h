#ifndef ERODE_H

#define ERODE_H

#include "HeSimpleTypes.h"

namespace ERODE
{

class ErodeDesc
{
public:
  ErodeDesc(void)
  {
    mFillBasin = 300;
    mFillBasinPer = 10;
    mErodeIterations = 100;
    mErodeRate   = 0.00002f;
    mErodePower  = 1;
    mSmoothRate  = 0.05f;
    mErodeThreshold = 2;
    mErodeSedimentation = 1.6f;
    mWidth = 0;
    mDepth = 0;
    mData  = 0;
    mErodeCount = 0;
  }

  HeU32        mFillBasin;
  HeU32        mFillBasinPer;
  HeU32        mErodeIterations;
  HeU32        mErodeCount;

  HeF32        mErodeRate;
  HeF32        mErodePower;
  HeF32        mSmoothRate;
  HeF32        mErodeThreshold;
  HeF32        mErodeSedimentation;

  HeI32        mWidth;
  HeI32        mDepth;
  HeF32       *mData;

};

class Erode
{
public:


  virtual bool erode(void) = 0; // perform a single erosion step
  virtual void getResults(void) = 0; // returns the erosion results back into the original buffer passed in.

};


Erode * createErode(const ErodeDesc &desc);
void    releaseErode(Erode *erode);

}; // end of namespace


#endif
