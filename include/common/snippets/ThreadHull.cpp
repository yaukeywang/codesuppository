#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "StanHull.h"
#include "ThreadHull.h"
#include "JobSwarm.h"
#include "UserMemAlloc.h"

#pragma warning(disable:4100)

namespace STAN_HULL
{

class MyThreadHull : public ThreadHull, public JOB_SWARM::JobSwarmInterface
{
public:

  MyThreadHull(const HullDesc &desc,JOB_SWARM::JobSwarmContext *context)
  {
    //
    mRet = QE_NOT_READY;
    mDesc.mFlags         = desc.mFlags;
    mDesc.mVcount        = desc.mVcount;
    mDesc.mVertices      = MEMALLOC_NEW(NxF32)[mDesc.mVcount*3];
    mDesc.mVertexStride  = sizeof(NxF32)*3;
    if ( mDesc.mVertexStride == desc.mVertexStride )
    {
      memcpy((void *)mDesc.mVertices,desc.mVertices,mDesc.mVertexStride*mDesc.mVcount);
    }
    else
    {
      const NxU8 *source = (const NxU8 *)desc.mVertices;
      NxF32 *dest = (NxF32 *)mDesc.mVertices;
      for (NxU32 i=0; i<mDesc.mVcount; i++)
      {
        const NxF32 *temp = (const NxF32 *)source;
        dest[0] = temp[0];
        dest[1] = temp[1];
        dest[2] = temp[2];
        dest+=3;
        source+=desc.mVertexStride;
      }
    }
    mDesc.mNormalEpsilon = desc.mNormalEpsilon;
    mDesc.mSkinWidth     = desc.mSkinWidth;
    mDesc.mMaxVertices   = desc.mMaxVertices;
    mSwarmJob = context->createSwarmJob(this,0,0);
  }

  ~MyThreadHull(void)
  {
    assert( mSwarmJob == 0 );
  }

  virtual HullError getResult(HullResult &result) // return true if we have a result
  {
    if ( mRet != QE_NOT_READY )
    {
      result = mResult;
    }
    return mRet;
  }

  virtual void job_process(void *userData,NxI32 userId)    // RUNS IN ANOTHER THREAD!! MUST BE THREAD SAFE!
  {
    mRet = mHull.CreateConvexHull(mDesc,mResult);
    delete []mDesc.mVertices;
    mDesc.mVertices = 0;
  }

  virtual void job_onFinish(void *userData,NxI32 userId)  // runs in primary thread of the context
  {
  }

  virtual void job_onCancel(void *userData,NxI32 userId)  // runs in primary thread of the context
  {
    mRet = QE_FAIL;
  }

  void releaseResult(void)
  {
    mHull.ReleaseResult(mResult);
  }


  JOB_SWARM::SwarmJob *mSwarmJob;
  HullLibrary mHull;
  HullError   mRet;
  HullDesc    mDesc;
  HullResult  mResult;
};


ThreadHull * createThreadHull(const HullDesc &desc,JOB_SWARM::JobSwarmContext *context)
{
  MyThreadHull *mt = MEMALLOC_NEW(MyThreadHull)(desc,context);
  return static_cast< ThreadHull *>(mt);
}

void         releaseThreadHull(ThreadHull *th)
{
  MyThreadHull *m = static_cast< MyThreadHull *>(th);
  delete m;
}

};
