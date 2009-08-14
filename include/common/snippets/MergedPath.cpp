#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <vector>

#define DEBUG_PATH 0

#include "MergedPath.h"
#include "FloatMath.h"
#include "UserMemAlloc.h"

#if DEBUG_PATH
#include "timelog.h"

static TimeLog gLog;

#endif

namespace MERGED_PATH
{

typedef USER_STL::vector< NxF32 > HeF32Vector;


class MyMergedPath : public MergedPath
{
public:

  void addPoint(const NxF32 *p)
  {
    mResults.push_back( p[0] );
    mResults.push_back( p[1] );
    mResults.push_back( p[2] );
  }

  NxF32 * createMergedPath(const NxF32 *pos,const NxF32 *_pdir,NxU32 pcount,const NxF32 * points,NxU32 pstride,NxU32 &pout,NxF32 maxDist)
  {
    NxF32 *ret = 0;
    pout = 0;

    mResults.clear();

    NxF32 pdist = fm_distanceSquared(pos,points);
    if ( pdist < (0.002f*0.002f) )
    {
      const NxF32 *p = points;
      for (NxU32 i=0; i<pcount; i++)
      {
        addPoint(p);
        p+=3;
      }
      pout = pcount;
      ret = &mResults[0];
    }
    else
    {

      NxF32 pdir[3] = { _pdir[0], _pdir[1], _pdir[2] };
      fm_normalize(pdir);


      NxF32 nearDot = 0;
      NxF32 nearDist = 100;
      NxF32 nearPos[3];
      NxU32 nearIndex = 0;

      const NxF32 *p1   = points;
      const NxU8  *scan = (const NxU8 *)points;
      NxF32 maxDist2    = maxDist*maxDist;

      for (NxU32 i=0; i<(pcount-1); i++)
      {
        scan+=pstride;

        bool match = false;

        const NxF32 *p2 = (const NxF32 *)scan;

        NxF32 distance = fm_distance(p1,p2); // real-distance

        NxU32 steps = (NxU32)(distance*20.0f+0.5f);
        NxF32 recip = 1.0f / (NxF32)steps;

        for (NxU32 j=0; j<steps; j++)
        {
          NxF32 lerp = (NxF32)j*recip;
          NxF32 lpos[3];

          fm_lerp(p1,p2,lpos,lerp);

          NxF32 distance = fm_distanceSquared(pos,lpos);

          if ( distance < maxDist2 )
          {
            NxF32 dir[3];


            dir[0] = lpos[0] - pos[0];
            dir[1] = lpos[1] - pos[1];
            dir[2] = lpos[2] - pos[2];

            NxF32 pdist = dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2];

            NxF32 dot = 1;
            if ( distance > 0.001f )
            {
              fm_normalize(dir);
              dot =  fm_dot(pdir,dir);
            }

            bool closer = false;

            if ( dot > 0.1f && dot > nearDot )
            {
              nearDot = dot;
              closer = true;
            }
            else if ( nearDot == 0 )
            {
              if ( pdist < nearDist )
              {
                nearDist = pdist;
                closer = true;
              }
            }
            
            if ( closer )
            {
              nearPos[0] = lpos[0];
              nearPos[1] = lpos[1];
              nearPos[2] = lpos[2];

              nearIndex = i;
              match = true;
            }

          }
        }

        bool prevSolution = true;
        if ( nearDot == 0 || nearDist == 100 ) prevSolution = false;

        if ( !match && prevSolution )  // if we didn't match but have a previous solution take the best one so far...
        {
          break;
        }


        p1 = p2;
      }

      if ( nearDot != -2 )
      {
        addPoint(pos);
        addPoint(nearPos);
        nearIndex++;

        const NxU8 *scan = (const NxU8 *)points;
        scan+=(nearIndex*pstride);
        for (NxU32 i=nearIndex; i<pcount; i++)
        {
          const NxF32 *pos = (const NxF32 *)scan;
          addPoint(pos);
          scan+=pstride;
        }
        if ( !mResults.empty() )
        {
          pout = static_cast<NxU32>(mResults.size())/3;
          ret = &mResults[0];
        }
      }

  #if DEBUG_PATH
      if ( ret )
        gLog.logTime("MERGED_PATH SUCCESSFULL");
      else
        gLog.logTime("MERGED_PATH FAILED");

      gLog.log("Position:  (%0.9f,%0.9f,%0.9f)\r\n", pos[0], pos[1], pos[2] );
      gLog.log("Direction: (%0.9f,%0.9f,%0.9f)\r\n", pdir[0], pdir[1], pdir[2] );
      gLog.log("PCOUNT: %d MAXDIST: %0.9f PSTRIDE: %d\r\n", pcount, maxDist, pstride );
      if ( 1 )
      {
        const char *scan = (const char *) points;
        for (NxU32 i=0; i<pcount; i++)
        {
          const NxF32 *p = (const NxF32 *) scan;
          gLog.log("  Point%d = (%0.9f,%0.9f,%0.9f)\r\n", i+1, p[0], p[1], p[2] );
          scan+=pstride;
        }
        gLog.log("\r\n");
      }
  #endif
    }

    return ret;
  }

private:
  HeF32Vector mResults;
};


MergedPath * createMergedPath(void)
{
  MyMergedPath *ret = MEMALLOC_NEW(MyMergedPath);
  return static_cast< MergedPath *>(ret);
}

void        releaseMergedPath(MergedPath *mp)
{
  MyMergedPath *mmp = static_cast< MyMergedPath *>(mp);
  delete mmp;
}

}; // end of namespace
