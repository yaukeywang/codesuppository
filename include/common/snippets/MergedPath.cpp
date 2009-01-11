#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <assert.h>

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

typedef USER_STL::vector< HeF32 > HeF32Vector;


class MyMergedPath : public MergedPath
{
public:

  void addPoint(const HeF32 *p)
  {
    mResults.push_back( p[0] );
    mResults.push_back( p[1] );
    mResults.push_back( p[2] );
  }

  HeF32 * createMergedPath(const HeF32 *pos,const HeF32 *_pdir,HeU32 pcount,const HeF32 * points,HeU32 pstride,HeU32 &pout,HeF32 maxDist)
  {
    HeF32 *ret = 0;
    pout = 0;

    mResults.clear();

    HeF32 pdist = fm_distanceSquared(pos,points);
    if ( pdist < (0.002f*0.002f) )
    {
      const HeF32 *p = points;
      for (HeU32 i=0; i<pcount; i++)
      {
        addPoint(p);
        p+=3;
      }
      pout = pcount;
      ret = &mResults[0];
    }
    else
    {

      HeF32 pdir[3] = { _pdir[0], _pdir[1], _pdir[2] };
      fm_normalize(pdir);


      HeF32 nearDot = 0;
      HeF32 nearDist = 100;
      HeF32 nearPos[3];
      HeU32 nearIndex = 0;

      const HeF32 *p1   = points;
      const HeU8  *scan = (const HeU8 *)points;
      HeF32 maxDist2    = maxDist*maxDist;

      for (HeU32 i=0; i<(pcount-1); i++)
      {
        scan+=pstride;

        bool match = false;

        const HeF32 *p2 = (const HeF32 *)scan;

        HeF32 distance = fm_distance(p1,p2); // real-distance

        HeU32 steps = (HeU32)(distance*20.0f+0.5f);
        HeF32 recip = 1.0f / (HeF32)steps;

        for (HeU32 j=0; j<steps; j++)
        {
          HeF32 lerp = (float)j*recip;
          HeF32 lpos[3];

          fm_lerp(p1,p2,lpos,lerp);

          HeF32 distance = fm_distanceSquared(pos,lpos);

          if ( distance < maxDist2 )
          {
            HeF32 dir[3];


            dir[0] = lpos[0] - pos[0];
            dir[1] = lpos[1] - pos[1];
            dir[2] = lpos[2] - pos[2];

            HeF32 pdist = dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2];

            HeF32 dot = 1;
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

        const HeU8 *scan = (const HeU8 *)points;
        scan+=(nearIndex*pstride);
        for (HeU32 i=nearIndex; i<pcount; i++)
        {
          const HeF32 *pos = (const HeF32 *)scan;
          addPoint(pos);
          scan+=pstride;
        }
        if ( !mResults.empty() )
        {
          pout = static_cast<HeU32>(mResults.size())/3;
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
        for (HeU32 i=0; i<pcount; i++)
        {
          const HeF32 *p = (const HeF32 *) scan;
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
