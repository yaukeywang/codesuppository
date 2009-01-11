#ifndef AI_PATH_FIND_H

#define AI_PATH_FIND_H

#include "common/snippets/UserMemAlloc.h"
#include "HBPathSystem/HBPathSystem.h"
#include "common/snippets/KdTree.h"
#include "common/snippets/simplevector.h"
#include "StringPull.h"

class NxScene;

namespace HBPATHSYSTEM_AIWISDOM
{




#define AIPATH_VERSION_NUMBER 107 // bumped from version #106 to #107 when the flags field was added per node.

enum NodeFlag
{
  NF_IS_WATER      = (1<<0),            // is in water.
  NF_IS_TERRAIN    = (1<<1),            // this node resides on terrain.
  NF_IS_UNDERWATER = (1<<2),           // node is underwater.
  NF_AVOID_1       = (1<<3),
  NF_AVOID_2       = (1<<4),
  NF_AVOID_3       = (1<<5),
  NF_AVOID_4       = (1<<6),
  NF_PREFER_1      = (1<<7),
  NF_PREFER_2      = (1<<8),
  NF_PREFER_3      = (1<<9),
  NF_PREFER_4      = (1<<10),
};

struct AIPathHeader
{
  char         mPath[8];  // should contain the string 'AIPATHID'
  HeU64        mAssetTimeStamp; // a 64 bit unique time stamp.
  HeU32        mVersionNumber;
  HeU32        mSourceVersionNumber;
  HeU32        mNodeCount;
  HeU32        mConnectionCount;
  HeF32        mLongEdge;
  HeU32        mDetailLevel;
  HeF32        mWalkSlope;
  HeF32        mStepHeight;
  HeU32        mMinNodeSize;
  HeU32        mMaxNodeSize;
  HeF32        mCharacterWidth;
  HeF32        mCharacterHeight;
};

class AIPathFindInterface
{
public:
  virtual void reportNode(HeU32 index,const HeF32 *center,const HeF32 *bmin,const HeF32 *bmax,HeU32 flags) = 0;
  virtual void reportConnection(HeU32 from,HeU32 to) = 0;
};

class AIPathFind;
class AIPathSolver;
class PathEdge;
class PathEdgeInterface;
class PathInstance;
class AINode;

AIPathFind   *doLoadAI(const void *mem,HeU32 len,HeU32 &ncount,HeU32 &ccount);
bool          doRelease(AIPathFind *pfind);
HBPATHSYSTEM::PathNode      *doFindPath(AIPathSolver *solver,HeU32 &count,HeU32 solverCount,bool &ok,HeU32 &totalSolverCount,HeF32 globalTime);
void           doReleaseFindPath(AIPathSolver *solver);


class AIPathFind
{
public:
  virtual HBPATHSYSTEM::PathNode * getNodesInRange(const HeF32 * point,HeF32 radius,HeU32 &count,HBPATHSYSTEM::PathProperties *props,const HeBounds3 *bound) = 0;
  virtual bool                     getPathDataVersion(HeU32 &build_version,HeU32 &asset_version,HeU64 &timeStamp) = 0;
  virtual HeU32                    render(const HeF32 *eyePos,HeF32 range,bool showBounds,bool showConnections,bool showSolid,bool showPathCacheValid,bool showPathCacheInvalid,PathInstance *edges) = 0;
  virtual AIPathSolver            *startFindPath(const HeF32 *p1,const HeF32 *p2,PathInstance *edges,HBPATHSYSTEM::PathProperties *properties,const HBPATHSYSTEM::PathBuildProps &props) =0;
  virtual bool                     getRandomPoint(HeF32 *point,const HBPATHSYSTEM::PathProperties *properties) = 0;
  virtual bool                     getNearestPoint(HeF32 *point,PathInstance *pinst,HBPATHSYSTEM::PathProperties *properties,bool validate) = 0;
  virtual HBPATHSYSTEM::PathNode * getNearestPointConnections(HeF32 *point,PathInstance *pinst,HBPATHSYSTEM::PathProperties *properties,HeU32 &pcount) = 0;
  virtual HeU32                    intersectEdges(PathEdgeInterface *callback,const HeF32 *bmin,const HeF32 *bmax,bool exact) = 0;
  virtual HeU32                    getNodeCount(HeU32 &ecount) const = 0;
  virtual void                     enumerateNodes(AIPathFindInterface *iface) = 0;
  virtual void                     enumerateConnections(AIPathFindInterface *iface) = 0;
  virtual AINode *                 getBounds(HeU32 index,HeBounds3 &b) = 0;
  virtual void                     debugString(const HeF32 *bmin,const HeF32 *bmax,HeU32 i1,HeU32 i2,CanWalkState state) = 0;

  virtual bool                     getRandomPointInVolume(const HeBounds3 &b,HeVec3 &point,HBPATHSYSTEM::PathProperties * props) =0;
  virtual bool                     getRandomPointInSphere(const HeVec3 &center,HeF32 radius,HeVec3 &point,HBPATHSYSTEM::PathProperties * props)=0;


};



enum PathEdgeFlag
{
  PEF_ACTIVE     =  (1<<0),
  PEF_INTERSECTS = (1<<1), //this edge intersects the pathfinding node bounding volume.
};

class PathEdge
{
public:
  PathEdge(void)
  {
    mFlags = PEF_ACTIVE;
  }

  void setPathEdgeFlag(PathEdgeFlag flag)
  {
    mFlags|=flag;
  }

  void clearPathEdgeFlag(PathEdgeFlag flag)
  {
    mFlags&=~flag;
  }

  void setPathEdgeFlag(PathEdgeFlag flag,bool state)
  {
    if ( state )
      mFlags|=flag;
    else
      mFlags&=~flag;
  }

  bool hasPathEdgeFlag(PathEdgeFlag flag) const
  {
    bool ret = false;

    if ( mFlags & flag ) ret = true;

    return ret;
  }

  HeU32 mFlags;
};


class PathEdgeInterface
{
public:
  virtual void edgeIntersect(HeU32 edgeIndex,const HeF32 *p1,const HeF32 *p2,HeU32 index1,HeU32 index2) = 0;
  virtual void nodeIntersect(HeU32 nodeIndex,const HeF32 *center) = 0;
};

class PathInstance
{
public:
  PathInstance(void)
  {
    mKey[0]  = 'P';
    mKey[1]  = 'A';
    mKey[2]  = 'T';
    mKey[3]  = 'H';
    mKey[4]  = 'I';
    mKey[5]  = 'N';
    mKey[6]  = 'S';
    mKey[7]  = 'T';
    mKey[8]  = 'A';
    mKey[9]  = 'N';
    mKey[10] = 'C';
    mKey[11] = 'E';
    mKey[12] = 0;

    mParent    = 0;
    mNodeCount = 0;
    mEdgeCount = 0;
    mEdges     = 0;
    mNodeCosts = 0;
    mRegionUpdate = false;
    mStringPull   = createStringPull();
  }

  ~PathInstance(void)
  {
    release();
    validate();
    memset(mKey,0,sizeof(mKey));
  }

  bool validate(void) const
  {
    bool ret = false;

    HE_ASSERT( mKey[0] == 'P' );
    HE_ASSERT( mKey[1] == 'A' );
    HE_ASSERT( mKey[2] == 'T' );
    HE_ASSERT( mKey[3] == 'H' );
    HE_ASSERT( mKey[4] == 'I' );
    HE_ASSERT( mKey[5] == 'N' );
    HE_ASSERT( mKey[6] == 'S' );
    HE_ASSERT( mKey[7] == 'T' );
    HE_ASSERT( mKey[8] == 'A' );
    HE_ASSERT( mKey[9] == 'N' );
    HE_ASSERT( mKey[10] == 'C' );
    HE_ASSERT( mKey[11] == 'E' );
    HE_ASSERT( mKey[12] == 0 );

    if ( mKey[0] == 'P' &&
         mKey[1] == 'A' &&
         mKey[2] == 'T' &&
         mKey[3] == 'H' &&
         mKey[4] == 'I' &&
         mKey[5] == 'N' &&
         mKey[6] == 'S' &&
         mKey[7] == 'T' &&
         mKey[8] == 'A' &&
         mKey[9] == 'N' &&
         mKey[10] == 'C' &&
         mKey[11] == 'E' &&
         mKey[12] == 0 )
    {
      ret = true;
    }

    return ret;
  }

  void release(void)
  {
    delete []mEdges;
    delete []mNodeCosts;
    mEdges = 0;
    mNodeCosts = 0;
    mParent    = 0;
    mNodeCount = 0;
    mEdgeCount = 0;
    releaseStringPull(mStringPull);
    mStringPull = 0;
  }

  void set(AIPathFind *pfind)
  {

    release(); // release all previous transient data.

    if ( pfind )
    {
      mParent = pfind;
      mNodeCount = pfind->getNodeCount(mEdgeCount);
      mEdges = MEMALLOC_NEW_ARRAY(PathEdge,mEdgeCount)[mEdgeCount];
      mNodeCosts = MEMALLOC_NEW_ARRAY(HeF32,mNodeCount)[mNodeCount];
      for (HeU32 i=0; i<mNodeCount; i++)
      {
        mNodeCosts[i] = 1;
      }
      mStringPull   = createStringPull();
    }
  }

  void setRegionUpdate(const HeBounds3 &region)
  {
    mRegionUpdate = true;
    mRegion = region;
  }

  bool isRegionUpdate(void) const
  {
    return mRegionUpdate;
  }

  void clearRegionUpdate(void)
  {
    mRegionUpdate = false;
  }


  char                    mKey[16];
  AIPathFind             *mParent;          // original data
  HeU32                   mNodeCount;
  HeU32                   mEdgeCount;
  PathEdge               *mEdges;           // edge flags of original nodes
  HeF32                  *mNodeCosts;       // costs of original nodes
  bool                    mRegionUpdate;
  HeBounds3               mRegion;
  StringPull             *mStringPull;
};



void                     stepSolution(void);

};

#endif
