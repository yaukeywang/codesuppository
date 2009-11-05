#ifndef ERRORH_H

#define ERRORH_H

#include "common/snippets/UserMemAlloc.h"

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

  NxF32 Get(NxI32 x,NxI32 y) const; // get error from this heightfield location

private:

   NxF32 GetError(NxI32 x,NxI32 y,const RtinObj &rtin,TopoRtin &topo) const;

   NxI32  mWidth;
   NxI32  mHeight;
   NxF32 *mError;

};

#endif
