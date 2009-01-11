#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "common/snippets/UserMemAlloc.h"

#include "AIPathFind.h"
#include "StringPull.h"
#include "HbPhysics/HbPhysics.h"
#include "HeroWorld/CollisionGroupFlag.h"
#include "common/snippets/SendTextMessage.h"
#include "common/snippets/UserMemAlloc.h"
#include "RenderDebug/BaseRenderDebug.h"

extern HeF32 gInverseGameScale;
extern HeF32 gGameScale;


namespace HBPATHSYSTEM_AIWISDOM
{

#define MAX_PATH_COUNT 256

#define MAX_CACHE_SIZE 8192 // only cache 8k worth of line segments
#define AGE_TIME 10.0f      // age out cache lines if over 60 seconds old.

class Segment
{
public:
  DEFINE_MEMORYPOOL_IN_CLASS(Segment);
  Segment(void)
  {

  }

  Segment(CanWalkState state,HeU32 properties,HeF32 time)
  {
    mState      = (HeU8)state;
    mProperties = properties;
    mTime       = time;
  }

  HeU8        mState;
  HeU32       mProperties;
  HeF32       mTime;
};

IMPLEMENT_MEMORYPOOL_IN_CLASS(Segment);

typedef USER_STL::hash_map< HeU64, Segment, Segment > SegmentHash;

namespace
{
  union Hasher
  {
    HeU32 parts[2];
    HeU64 total;
  };
}

class MyStringPull : public StringPull
{
public:
  MyStringPull(void)
  {
    mLastTime = 0;
  }

  void setProps(HBPATHSYSTEM::PathBuildProps &props)
  {
    mProps = props;
  }

  ~MyStringPull(void)
  {
  }

  inline HeU64 computeHash(HeU32 i1,HeU32 i2)
  {
    Hasher hasher;
    hasher.parts[0]=i1;
    hasher.parts[1]=i2;
    return hasher.total;
  }

  CanWalkState cachedCanWalk(HeU32 i1,HeU32 i2)
  {
    CanWalkState ret = CWS_NONE;

    HeU64 v = computeHash(i1,i2);
    SegmentHash::iterator found;
    found = mTracker.find(v);
    if ( found != mTracker.end() )
    {
      ret = (CanWalkState) (*found).second.mState;
    }

    return ret;

  }

  void addTracker(HeU32 i1,HeU32 i2,CanWalkState state,HeU32 properties,HeF32 time)
  {
    HeU64 v = computeHash(i1,i2);
    Segment s(state,properties,time);
    mTracker[v] = s;

    HeU32 size = mTracker.size();
    if ( (size%1000) == 0 )
    {
      SEND_TEXT_MESSAGE(0,"@PM:Tracking(%d) connections.", size );
    }

  }

  bool internalCanWalk(NxScene *scene,HeU32 left,HeU32 right,HBPATHSYSTEM::PathNode *nodes,HeU32 properties,HeF32 time,const HBPATHSYSTEM::PathBuildProps &props)
  {
    bool ret = true;

    HeU32 diff = right-left;

    if ( diff >= 2 )
    {
      ret = false;

      HBPATHSYSTEM::PathNode &node = nodes[left];
      HBPATHSYSTEM::PathNode &next = nodes[right];

      CanWalkState state = cachedCanWalk(node.mIndex,next.mIndex);
      switch ( state )
      {
        case CWS_CAN_WALK:
          ret = true;
          //SEND_TEXT_MESSAGE(0,"Found CanWalk(%d,%d) Size(%d)\r\n", node.mIndex, next.mIndex, getSize() );
          break;
        case CWS_CANNOT_WALK:
          ret = false;
          //SEND_TEXT_MESSAGE(0,"Found CannotWalk(%d,%d) Size(%d)\r\n", node.mIndex, next.mIndex, getSize() );
          break;
        case CWS_PATH_FAILURE:
          ret = false;
          //SEND_TEXT_MESSAGE(0,"Found PathFailure(%d,%d) Size(%d)\r\n", node.mIndex, next.mIndex, getSize() );
          break;
        default:
          {
            HeF32 from[3] = { node.mCenter[0]*gInverseGameScale, node.mCenter[1]*gInverseGameScale, node.mCenter[2]*gInverseGameScale };
            HeF32 to[3]   = { next.mCenter[0]*gInverseGameScale, next.mCenter[1]*gInverseGameScale, next.mCenter[2]*gInverseGameScale };

            HB_PHYSICS::RaycastRequest r;

            r.setFromTo(from,to);
            r.mGroupFlags = CGF_PATHFINDING_NODE; // only see if we hit a path finding node!

            bool hitPathFindingNode = gHBPhysics->raycastImmediate(scene,r);
            if ( !hitPathFindingNode )
            {
              HeF32 epos[3];
              bool canWalk = gHBPhysics->canWalk(scene,node.mCenter,next.mCenter, props.mCharacterWidth,props.mCharacterHeight,props.mCharacterStepHeight,props.mCharacterWalkSlope, epos, true, false, CGF_PATH_COLLISION,false );
              if ( canWalk )
              {
                addTracker(node.mIndex,next.mIndex,CWS_CAN_WALK,properties,time);
                ret = true;
              }
              else
              {
                addTracker(node.mIndex,next.mIndex,CWS_CANNOT_WALK,properties,time);
              }

            }
            else
            {
              addTracker(node.mIndex,next.mIndex,CWS_CANNOT_WALK,properties,time);
            }
            break;
        }
      }
    }

    return ret;
  }

  CanWalkState  canWalk(HeU32 i1,HeU32 i2,HeU32 properties)
  {
    CanWalkState ret = CWS_NONE;


    HeU64 v = computeHash(i1,i2);
    SegmentHash::iterator found;
    found = mTracker.find(v);

    if ( found != mTracker.end() )
    {
      Segment &s = (*found).second;
      if ( s.mProperties == properties ) // has to match the same properties it was saved as!
      {
        ret = (CanWalkState)s.mState;
      }
    }

    return ret;
  }

  void recursiveStringPull(NxScene *scene,HeU32 left,HeU32 right,HBPATHSYSTEM::PathNode *nodes,HeU8 *path,HeU32 properties,HeF32 time,const HBPATHSYSTEM::PathBuildProps &props)
  {
    if ( internalCanWalk(scene,left,right,nodes,properties,time,props) )
    {
      path[left] = 1;
      path[right] = 1;
    }
    else
    {
      HeU32 center = (left+right+1)/2;
      recursiveStringPull(scene,left,center,nodes,path,properties,time,props);
      recursiveStringPull(scene,center+1,right,nodes,path,properties,time,props);
    }
  }


  HeU32 stringPull(NxScene *scene,HeU32 ncount,HBPATHSYSTEM::PathNode *nodes,HeU32 properties,HeF32 time,const HBPATHSYSTEM::PathBuildProps &props)
  {
    HeU32 ret = ncount;


    if ( ncount < MAX_PATH_COUNT )
    {

      HeU32 end = ncount-1;

      bool skipFront = false;
      bool skipBack  = false;

      if ( nodes[0].mIndex == 0xFFFFFFFF )
      {
        skipFront = true;
      }

      if ( nodes[end].mIndex == 0xFFFFFFFF )
      {
        skipBack = true;
      }

      HBPATHSYSTEM::PathNode *_nodes = nodes;
      if ( skipFront )
      {
        nodes++;
        ncount--;
      }

      if ( skipBack )
      {
        ncount--;
      }

      HeU8 path[MAX_PATH_COUNT];

      memset(path,0,sizeof(HeU8)*ncount);

      recursiveStringPull(scene,0,ncount-1,nodes,path,properties,time,mProps);

      ret = 0;

      for (HeU32 i=0; i<ncount; i++)
      {
        if ( path[i] )
        {
          if ( ret != i )
          {
            nodes[ret] = nodes[i];
          }
          ret++;
        }
      }

      ncount = ret;

      // ok..now the cleanup pass..
      if ( ncount > 2 )
      {
        HeU32 ocount = 0;
        HBPATHSYSTEM::PathNode dest[MAX_PATH_COUNT];
        HeU32 i = 0;

        while ( i < (ncount-2) )
        {
          if ( internalCanWalk(scene,i,i+2,nodes,properties,time,props) )
          {
            dest[ocount+0] = nodes[i+0];
            dest[ocount+1] = nodes[i+2];
            ocount+=2;
            i+=3;
          }
          else
          {
            dest[ocount+0] = nodes[i+0];
            dest[ocount+1] = nodes[i+1];
            ocount+=2;
            i+=2;
          }
        }

        while ( i < ncount )
        {
          dest[ocount] = nodes[i];
          ocount++;
          i++;
        }
        if ( ocount != ncount )
        {
          memcpy(nodes,dest,sizeof(HBPATHSYSTEM::PathNode)*ocount);
          ret = ocount;
        }
      }


      if ( skipBack )
      {
        nodes[ret] = _nodes[end];
        ret++;
      }

      if ( skipFront )
      {
        ret++;
      }

    }

    return ret;
  }

  HeU32 getSize(void) const { return mTracker.size(); };


  void bringOutYourDead(HeF32 time)
  {
    HeU32 count = mTracker.size();



    if ( count > MAX_CACHE_SIZE )
    {
      HeF32 diff = time - mLastTime;
      if ( diff > AGE_TIME )
      {
        mLastTime = time;
        SegmentHash::iterator i = mTracker.begin();
        HeU32 kcount = count;
        while ( i != mTracker.end() )
        {
          Segment &s = (*i).second;
          HeF32 diff = time = s.mTime;
          if ( diff > AGE_TIME )
          {
            i = mTracker.erase(i);
            kcount--;
            if ( kcount < (MAX_CACHE_SIZE/2) )
              break;
          }
          else
          {
            i++;
          }
        }
        SEND_TEXT_MESSAGE(0,"StringPull->BringOutYourDead(%d,%d)\r\n", count, mTracker.size() );
      }
    }
  }


  void render(AIPathFind *pfind,const HeF32 *eyePos,HeF32 range)
  {
    HeF32 bmin[3] = { eyePos[0]-range, eyePos[1]-range, eyePos[2]-range };
    HeF32 bmax[3] = { eyePos[0]+range, eyePos[1]+range, eyePos[2]+range };

//    gBaseRenderDebug->DebugBound(bmin,bmax,0xFFFF00);

    SegmentHash::iterator i;
    for (i=mTracker.begin(); i!=mTracker.end(); ++i)
    {
      HeU64 hash = (*i).first;
      HeU32 *h = (HeU32 *)&hash;
      HeU32 i1   = h[0];
      HeU32 i2   = h[1];
      CanWalkState state = (CanWalkState)(*i).second.mState;
      pfind->debugString(bmin,bmax,i1,i2,state);
    }

  }

private:
  HeF32                 mLastTime;
  SegmentHash           mTracker;
  HBPATHSYSTEM::PathBuildProps        mProps;
};


StringPull * createStringPull(void)
{
  MyStringPull *ret = MEMALLOC_NEW(MyStringPull);
  return ret;
}

void         releaseStringPull(StringPull *stringPull)
{
  MyStringPull *msp = static_cast< MyStringPull *>(stringPull);
  delete msp;
}


};
