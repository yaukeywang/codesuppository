#ifndef ERRORH_H

#define ERRORH_H

#include "common/snippets/HeSimpleTypes.h"

/** @file errorh.h
 *  @brief Used to compute a localized error heuristic on a heightfield, effectively samples the frequency at that location.
 *
 *  I found that when applying one error value to an entire heightfield, I would either get too much data in some spots or
 *  not enough definition in others, because the frequency of the source data was not uniform.  This class calculates a local
 *  frequency term, so that when tessalating the surface we do not lose detail where we need it, and don't retain too much
 *  detail where we don't.
 *
 *  @author John W. Ratcliff
*/

/** @file errorh.cpp
 *  @brief Used to compute a localized error heuristic on a heightfield, effectively samples the frequency at that location.
 *
 *  I found that when applying one error value to an entire heightfield, I would either get too much data in some spots or
 *  not enough definition in others, because the frequency of the source data was not uniform.  This class calculates a local
 *  frequency term, so that when tessalating the surface we do not lose detail where we need it, and don't retain too much
 *  detail where we don't.
 *
 *  @author John W. Ratcliff
*/


// computes error heuristic for a heightfield
#define SAMPLESIZE 16 // 32x32 cells for error heuristic.
#define SAMPLELEN (SAMPLESIZE+1)

class RtinObj;
class TopoRtin;

class ErrorHeuristic
{
public:

  ErrorHeuristic(const RtinObj &rtin,TopoRtin &topo);

  ~ErrorHeuristic(void);

  HeF32 Get(HeI32 x,HeI32 y) const; // get error from this heightfield location

private:

   HeF32 GetError(HeI32 x,HeI32 y,const RtinObj &rtin,TopoRtin &topo) const;

   HeI32  mWidth;
   HeI32  mHeight;
   HeF32 *mError;

};

#endif
