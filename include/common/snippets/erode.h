#ifndef ERODE_H

#define ERODE_H

#include "NxSimpleTypes.h"

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

  NxU32        mFillBasin;
  NxU32        mFillBasinPer;
  NxU32        mErodeIterations;
  NxU32        mErodeCount;

  NxF32        mErodeRate;
  NxF32        mErodePower;
  NxF32        mSmoothRate;
  NxF32        mErodeThreshold;
  NxF32        mErodeSedimentation;

  NxI32        mWidth;
  NxI32        mDepth;
  NxF32       *mData;

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
