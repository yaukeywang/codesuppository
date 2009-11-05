#ifndef THREAD_HULL_H

#define THREAD_HULL_H

#include "UserMemAlloc.h"
#include "StanHull.h"

using namespace NVSHARE;

namespace JOB_SWARM
{
class JobSwarmContext;
};

namespace STAN_HULL
{

class ThreadHull
{
public:

   virtual HullError getResult(HullResult &result) = 0; // return true if we have a result

};


ThreadHull * createThreadHull(const HullDesc &desc,JOB_SWARM::JobSwarmContext *context);
void         releaseThreadHull(ThreadHull *th);

};

#endif
