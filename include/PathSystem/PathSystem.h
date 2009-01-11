#ifndef PATHSYSTEM_H
#define PATHSYSTEM_H

#include "common/MemoryServices/MemoryServices.h"
#include "common/HeMath/HeFoundation.h"
#include <assert.h>
#include <string.h>

#pragma warning(push)
#pragma warning(disable:4996)

// If INT64 and UINT64 were not previously defined elsewhere, then define them here.

class NxScene;
class HeStats;
class PathPlanning;
class KeyValueData;


namespace HB_PHYSICS
{
  class HBPhysics;
};

namespace RENDER_DEBUG
{
  class BaseRenderDebug;
}

namespace RESOURCE_INTERFACE
{
  class ResourceInterface;
};

namespace HEROWORLD
{
  class HeroWorldMessageInterface;
};


namespace PATHSYSTEM
{

class PathProperties
{
public:
  PathProperties(void)
  {
    mKey[0] = 'P';
    mKey[1] = 'P';
    mKey[2] = 'R';
    mKey[3] = 'O';
    mKey[4] = 'P';
    mKey[5] = 0;

    mStringPulling    = true;
    mTerrain          = true;
    mNonTerrain       = true;
    mWater            = false;
    mUnderwater       = false;

    mPathAvoid1       = true;
    mPathAvoid2       = true;
    mPathAvoid3       = true;
    mPathAvoid4       = true;

    mPathPrefer1      = true;
    mPathPrefer2      = true;
    mPathPrefer3      = true;
    mPathPrefer4      = true;

    mDiveDown         = false;
    mSwimUp           = false;

    mDiveDownCost     = 10;
    mSwimUpCost       = 10;
    mTerrainCost      = 1;
    mNonTerrainCost   = 1;
    mWaterCost        = 10;
    mUnderwaterCost   = 10;

    mPathAvoidCost[0] = 1.5f;
    mPathAvoidCost[1] = 2.0f;
    mPathAvoidCost[2] = 4.0f;
    mPathAvoidCost[3] =10.0f;

    mPathPreferCost[0] = 0.7f;
    mPathPreferCost[1] = 0.5f;
    mPathPreferCost[2] = 0.3f;
    mPathPreferCost[3] = 0.1f;

    mTimeOut           = 4.0f;
    mPriority          = 1;

    mInternalGuid      = 0;
  }

  ~PathProperties(void)
  {
    validate();
    memset(mKey,0,sizeof(mKey));
  }

  bool validate(void) const
  {
    bool ret = false;

    HE_ASSERT( mKey[0] == 'P' );
    HE_ASSERT( mKey[1] == 'P' );
    HE_ASSERT( mKey[2] == 'R' );
    HE_ASSERT( mKey[3] == 'O' );
    HE_ASSERT( mKey[4] == 'P' );
    HE_ASSERT( mKey[5] == 0 );

    if ( mKey[0] == 'P' &&
         mKey[1] == 'P' &&
         mKey[2] == 'R' &&
         mKey[3] == 'O' &&
         mKey[4] == 'P' &&
         mKey[5] == 0 )
    {
      ret = true;
    }
    return ret;
  }

  char                          mKey[6];

  bool                          mStringPulling;
  bool                          mTerrain;
  bool                          mNonTerrain;
  bool                          mWater;
  bool                          mUnderwater;

  bool                          mPathAvoid1;
  bool                          mPathAvoid2;
  bool                          mPathAvoid3;
  bool                          mPathAvoid4;

  bool                          mPathPrefer1;
  bool                          mPathPrefer2;
  bool                          mPathPrefer3;
  bool                          mPathPrefer4;

  bool                          mDiveDown;
  bool                          mSwimUp;

  HeF32                         mDiveDownCost;
  HeF32                         mSwimUpCost;
  HeF32                         mTerrainCost;
  HeF32                         mNonTerrainCost;
  HeF32                         mWaterCost;
  HeF32                         mUnderwaterCost;
  HeF32                         mPathAvoidCost[4];
  HeF32                         mPathPreferCost[4];

  HeF32                         mTimeOut;
  HeI32                         mPriority;   // Priority value 1-10 higher the number, higher the priority.
  HeU32                         mInternalGuid;

};


typedef HeU64 PATHSYSTEMGUID;
typedef void * PATHDATAGUID;
typedef HeU64 PATHGUID;


class PathNode
{
public:

  void simpleCenterBox(const HeF32 *center,HeF32 scale)
  {
    mCenter[0] = center[0]*scale;
    mCenter[1] = center[1]*scale;
    mCenter[2] = center[2]*scale;

    HeF32 dv = 0.1f*scale;

    mBmin[0] = center[0]-dv;
    mBmin[1] = center[1]-dv;
    mBmin[2] = center[2]-dv;

    mBmax[0] = center[0]+dv;
    mBmax[1] = center[1]+dv;
    mBmax[2] = center[2]+dv;
    mIndex = 0xFFFFFFFF;
  }


  bool contains(const HeF32 *p,HeF32 inflation) const
  {
    bool ret = false;
    if ( p[0] >= (mBmin[0]-inflation) &&
         p[1] >= (mBmin[1]-inflation) &&
         p[2] >= (mBmin[2]-inflation) &&
         p[0] <= (mBmax[0]+inflation) &&
         p[1] <= (mBmax[1]+inflation) &&
         p[2] <= (mBmax[2]+inflation) )
    {
      ret = true;
    }
    return ret;
  }
  HeF32 mBmin[3];
  HeF32 mBmax[3];
  HeF32 mCenter[3];
  HeU32 mIndex;         // the node-index
  bool  mBiDirectional; // true if the connectivity is bi-directional; used when querying for connections
};


class PathNodeInterface
{
public:
  virtual void receivePathSolution(PATHSYSTEM::PATHSYSTEMGUID guid,PATHSYSTEM::PATHGUID pathid,void *userData,HeU64 userGuid1,HeU64 userGuid2,HeU32 ncount,const PATHSYSTEM::PathNode *nodes,HeI32 totalSolverCount,HeF32 solveTime) = 0;
};


class PathRequestDesc
{
public:

  PathRequestDesc(void)
  {
    mFrom[0]   = 0;
    mFrom[1]   = 0;
    mFrom[2]   = 0;
    mTo[0]     = 0;
    mTo[1]     = 0;
    mTo[2]     = 0;
    mCallback  = 0;
    mUserData  = 0;
    mUserId1   = 0;
    mUserId2   = 0;
    mDataId    = 0;
    mProperties = 0;
  }

  HeF32              mFrom[3];
  HeF32              mTo[3];
  PathNodeInterface *mCallback;
  void              *mUserData;
  HeU64              mUserId1;
  HeU64              mUserId2;
  HeU32              mDataId;
  PathProperties    *mProperties;
};

class PathBuildProps
{
public:
  PathBuildProps(void)
  {
    strcpy(mName,"default");
    mBuildVersion             = 1;
    mTerrain                  = true;
    mNonTerrain               = true;
    mWater                    = true;
    mUnderwater               = true;
    mDiveDown                 = true;
    mSwimUp                   = true;
    mPathAvoid1               = true;
    mPathAvoid2               = true;
    mPathAvoid3               = true;
    mPathAvoid4               = true;
    mPathPrefer1              = true;
    mPathPrefer2              = true;
    mPathPrefer3              = true;
    mPathPrefer4              = true;
    mCharacterCapsule         = true;
    mCharacterWalkSlope       = 45;
    mCharacterWidth           = 0.3f;
    mCharacterHeight          = 1.9f;
    mCharacterStepHeight      = 0.4f;
    mNodeGranularity          = 0.3f;
    mNodeMin                  = 3;
    mNodeMax                  = 10;
    mConnectionSearchDistance = 20;
    mConnectionSearchCount    = 20;
    mConnectionMax            = 10;

  }

  char            mName[256];

  HeU32           mBuildVersion;

  bool            mTerrain;          // true/false whether or not to build nodes on terrain.
  bool            mNonTerrain;       // true/false whether or not to build nodes on non-terrain
  bool            mWater;
  bool            mUnderwater;

  bool            mPathAvoid1;
  bool            mPathAvoid2;
  bool            mPathAvoid3;
  bool            mPathAvoid4;

  bool            mPathPrefer1;
  bool            mPathPrefer2;
  bool            mPathPrefer3;
  bool            mPathPrefer4;

  bool            mDiveDown;
  bool            mSwimUp;

  bool            mCharacterCapsule;
  HeF32           mCharacterWalkSlope;            // in degrees.
  HeF32           mCharacterWidth;                //
  HeF32           mCharacterHeight;
  HeF32           mCharacterStepHeight;

  HeF32           mNodeGranularity;
  HeU32           mNodeMin;
  HeU32           mNodeMax;

  HeF32           mConnectionSearchDistance;
  HeU32           mConnectionSearchCount;
  HeU32           mConnectionMax;

};

class PathBuildProperties
{
public:
  PathBuildProperties(void)
  {
    mCustomCount = 0;
    mCustomProperties = 0;
  }

  PathBuildProperties(const PathBuildProperties &p)  // implement the deep copy
  {
    mDefault = p.mDefault;
    mCustomCount = p.mCustomCount;
    if ( mCustomCount )
    {
      mCustomProperties = MEMALLOC_NEW_ARRAY(PathBuildProps,mCustomCount)[mCustomCount];
      memcpy(mCustomProperties,p.mCustomProperties,sizeof(PathBuildProps)*mCustomCount);
    }
    else
    {
      mCustomProperties = 0;
    }
  }

  ~PathBuildProperties(void)
  {
    if ( mCustomProperties )
      delete []mCustomProperties;
  }




  PathBuildProps   mDefault;
  HeU32            mCustomCount;
  PathBuildProps  *mCustomProperties;
};

enum PathState
{
  PS_EMPTY,
  PS_EXCLUDE,
  PS_INCLUDE,
  PS_SET_SHOW_BOUNDS,
  PS_SET_SHOW_CONNECTIONS,
  PS_SET_SHOW_PATH_CACHE_VALID,
  PS_SET_SHOW_PATH_CACHE_INVALID,
  PS_SET_SOLVER_BUDGET,
  PS_REBUILD,
  PS_SET_SHOW_DISTANCE,
  PS_SET_EYE_POS,
  PS_UPDATE_REGION,
  PS_SET_RENDER_SCALE,
  PS_SET_GAME_SCALE,
  PS_SET_PATH_BUILD_TIME,  // in milliseconds.
  PS_SET_PATH_SOLVE_TIME, // in milliseconds
  PS_CANCEL_PATH_REBUILD,

  PS_GET_SOLVER_COUNT,

// Message specific to AIWISDOM
  PS_MIN_NODE_SIZE,
  PS_MAX_NODE_SIZE,

  PS_REBUILD_EDGES,

  PS_REBUILD_COUNT, // indicator of how many times the path data has been re-generated.

  PS_GET_UPDATE_OK,

  PS_RELOAD_FROM_MASTER_GRAPH,
  PS_REBUILD_OF_INSTANCE_GRAPH,
  PS_SHOW_SOLID_NODES,

  PS_VISUALIZE_HIERARCHY,
  PS_VISUALIZE_HIERARCHY_RELATION,

  PS_SET_AIWISDOM,
  PS_SET_AISEEK,
  PS_USE_AIWISDOM,
  PS_USE_AISEEK,

  PS_GET_BUILD_VERSION,

  PS_GET_ACTIVE_REGION_COUNT, // report the number of regions currently being rebuilt.

  PS_GET_AIWISDOM,
  PS_GET_AISEEK,

  PS_WEIGHT_DIR,

  PS_STEP_SOLUTION,

  PS_SET_SHOW_SOLID,

  PS_LAST
};

class PathStateDesc
{
public:
};

class PathSystem;

/*!
	* rief
	* The HBPathSystem plugin is used for...
	*
	* The services provided are:
	*
	*
emarks
	* The HBPathSystem plug-in can be used in the following way...
	*
	* \see
	* Separate items with the '|' character.
	*/
class HBPathSystem : public MemoryServices
{
public:
  virtual void setBaseRenderDebug(RENDER_DEBUG::BaseRenderDebug *r) = 0;
  virtual void setResourceInterface(RESOURCE_INTERFACE::ResourceInterface *iface) =0;
  virtual void setHBPhysics(HB_PHYSICS::HBPhysics *hb) = 0;
  virtual void setHeStats(HeStats *stats) = 0; // set the interface for statistics accumulation about pathing
  virtual void setHBPathSystem(HBPathSystem *parent) = 0;

  virtual const char *     getPathDataName(const char *fqn) = 0;

  // Operations on an instance of PathData known as a 'PathSystem'
  virtual PATHSYSTEMGUID   createPathSystem(const char *fqn,NxScene *scene,PathPlanning *pp) = 0;

  virtual bool             releasePathSystem(PATHSYSTEMGUID guid) = 0;
  virtual bool             rebuildPathSystem(PATHSYSTEMGUID guid,const HeF32 *center,HeF32 radius) = 0;
  virtual bool             rebuildCancel(PATHSYSTEMGUID guid) = 0;
  virtual const char *     getStatus(PATHSYSTEMGUID guid) = 0;
  virtual bool             isRebuildComplete(PATHSYSTEMGUID guid) = 0;

  virtual bool             setPathSystemSendTextMessage(PATHSYSTEMGUID guid,SendTextMessage *stm) = 0; // set the 'sendTextMessage' interface relative to a specific PATHSYSTEM


  virtual HeU32     render(PATHSYSTEMGUID guid,
                                  const HeF32 *eyePos,
                                  HeF32 showDistance,
                                  void *userData,
                                  HEROWORLD::HeroWorldMessageInterface *iface,
                                  HeU64 userId1,
                                  HeU64 userId2) = 0;

  virtual void             pumpFrame(HeF32 dtime) = 0;

  virtual PATHGUID         solvePath(PATHSYSTEMGUID guid,const PathRequestDesc &desc) = 0;

  virtual bool             cancelPath(PATHSYSTEMGUID guid,PATHGUID path) = 0;
  virtual HeU32            cancelPaths(PATHSYSTEMGUID guid,PathNodeInterface *callback) = 0;


  virtual bool             setPathState(PATHSYSTEMGUID guid,PathState state,bool bstate=true,HeU32 v=0,const HeF32 *fv=0,PathStateDesc *desc=0,HeU64 bigv=0) = 0;
  virtual bool             getPathState(PATHSYSTEMGUID guid,PathState state,bool *bstate=0,HeU32 *v=0,HeF32 *fv=0,PathStateDesc *desc=0,HeU64 *bigv=0) = 0;

  virtual HBPathSystem *   getHBPathSystem(PathState /* state */) { return 0; };

  // Operations directly on the base PathData
  virtual PATHDATAGUID     getPathData(const char *fqn,bool loadOk) = 0;
  virtual bool             releasePathData(const char *fqn) = 0;

  virtual bool             getPathDataVersion(PATHDATAGUID guid,HeU32 &build_version,HeU32 &asset_version,HeU64 &timeStamp) = 0;
  virtual bool             rebuildPathData(PATHDATAGUID guid,NxScene *scene,const HeF32 *eyePos,HeF32 radius,PathPlanning *pp) = 0;
  virtual const char *     getStatus(PATHDATAGUID guid) = 0;
  virtual bool             savePathData(PATHDATAGUID guid,HeU32 asset_version,HeU64 timeStamp) = 0;
  virtual bool             releasePathData(PATHDATAGUID guid) = 0;
  virtual bool             rebuildCancel(PATHDATAGUID guid) = 0;
  virtual bool             isRebuildComplete(PATHDATAGUID guid) = 0;

  virtual bool             notifyAssetChanged(const char *fqn) = 0; // this path asset has changed. see if we need to reload it.

  virtual bool             getNearestPoint(PATHSYSTEMGUID guid,
                                           HeU32 dataId,
                                           const HeF32 * point,
                                           HeF32 * result,
                                           bool validate) = 0;

  virtual PathNode *       getNodesInRange(PATHSYSTEMGUID guid,HeU32 dataId,const HeF32 * point,HeF32 radius,HeU32 &count) = 0;
  virtual bool             getRandomPoint(PATHSYSTEMGUID guid,HeU32 dataId,HeF32 * result) = 0;

  virtual PathNode *       getNearestPointConnections(PATHSYSTEMGUID guid,HeU32 /* dataId */,const HeF32 * point,HeU32 &pcount) = 0;

// routines to support keyvaluespec data on a per-area basis..
  virtual bool createKeyValueSpecData(PATHSYSTEMGUID guid,const char *specName,HeU32 dataId,const char *spec,const char *data) = 0;

  virtual PathProperties * getPathProperties(HeU32 /* dataId */) { return 0; };

//*************
  virtual bool             getRandomPointInNode(PATHSYSTEMGUID guid,HeU32 dataId,HeU64 node,HeVec3 &point,bool regionNode) = 0;
  virtual const HeU64 *    getNodesContainingPoint(PATHSYSTEMGUID guid,const HeVec3 &point,HeU32 &count,bool regionNode) = 0;
  virtual bool             getRandomPointInSphere(PATHSYSTEMGUID guid,HeU32 dataId,const HeVec3 &center,HeF32 radius,HeVec3 &point) = 0;
  virtual bool             getRandomPointInVolume(PATHSYSTEMGUID guid,HeU32 dataId,const HeBounds3 &b,HeVec3 &point) = 0;



};

}; // End of namespace for PATHSYSTEM

#define PATHSYSTEM_VERSION (14+MEMORY_SERVICES_VERSION) // version 0.01  increase this version number whenever an interface change occurs.

extern PATHSYSTEM::HBPathSystem *gHBPathSystem; // This is an optional global variable that can be used by the application.  If the application uses it, it should define it somewhere in its codespace.

#pragma warning(pop)

#endif
