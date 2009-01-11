#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOMINMAX
#ifdef _WIN32
#include <windows.h>
#else
#include "linux_compat.h"
#endif
#include <vector>

#pragma comment(lib,"winmm.lib")
#pragma warning(disable:4100)

#include "common/snippets/UserMemAlloc.h"
#include "./HBPathSystem_AIWISDOM/HBPathSystem_AIWISDOM.h"
#include "./RenderDebug/RenderDebug.h"
#include "./HbPhysics/HbPhysics.h"
#include "./HeroWorld/CollisionGroupFlag.h"
#include "./common/snippets/streamer.h"
#include "./HeroWorld/HeroWorldCommandStream.h"
#include "./HeroWorld/HeroWorld.h"
#include "./PhysicsAsset/PhysicsAsset.h"
#include "StringPull.h"
#include "common/ResourceInterface/ResourceInterface.h"
#include "common/snippets/keyvalue.h"
#include "common/snippets/filesystem.h"
#include "common/snippets/cparser.h"
#include "common/snippets/KeyValueSpec.h"
#include "common/comlayer/comlayer.h"
#include "common/snippets/stringdict.h"
#include "AIPathFind.h"
#include "AIPathMaker.h"
#include "common/snippets/sutil.h"
#include "common/snippets/cycle.h"
#include "common/snippets/FloatMath.h"
#include "common/snippets/SmoothPath.h"
#include "common/snippets/clock.h"
#include "HeroWorld/HeStats.h"
#include "../HeroWorld/PathPlanning.h"
#include "common/snippets/AsciiPath.h"
#include "common/snippets/timelog.h"

#include <NxBounds3.h>
#include <NxSphere.h>
#include <NxShape.h>
#include <NxBox.h>
#include <NxBoxShape.h>
#include <NxActor.h>
#include <NxScene.h>

#include <map>
#include <vector>

#pragma warning(disable:4100)

#define UPDATE_SLOP 4
#define SOLVER_COUNT_PER 50

extern HeF32 gGameScale;

#ifndef PLUGINS_EMBEDDED
RESOURCE_INTERFACE::ResourceInterface *gResourceInterface=0;
RENDER_DEBUG::BaseRenderDebug *gBaseRenderDebug=0;
HB_PHYSICS::HBPhysics *gHBPhysics=0;
HeF32 gGameScale=1;
HeStats *gHeStats=0;
#endif

HBPATHSYSTEM::PathSystem *gParent=0;

#ifdef WIN32
#ifdef PATHSYSTEM_AIWISDOM_EXPORTS
#define PATHSYSTEM_AIWISDOM_API __declspec(dllexport)
#else
#define PATHSYSTEM_AIWISDOM_API __declspec(dllimport)
#endif
#else
#define PATHSYSTEM_AIWISDOM_API
#endif

#define TIME_LOG 0

#define MAX_PATH_REQUESTS 8000 // maximum total path requests at once.



HeF32 gRenderScale=1;
HeF32 gInverseGameScale=1;
HeU32                                      gMinNodeSize;
HeU32                                      gMaxNodeSize;
HeU32 gFrameCount=0;

TimeLog *gTimeLog=0;



HeU32 gMaxSolveTime = 6;
HeU32 gMaxBuildTime = 6;

HeU32 gActiveRegionCount=0;

bool doShutdown(void);

extern "C"
{
PATHSYSTEM_AIWISDOM_API PATHSYSTEM_AIWISDOM::PathSystem_AIWISDOM * getInterface(HeI32 version_number);
};

namespace PATHSYSTEM_AIWISDOM
{

void setGroup(NxActor *actor,HeU32 group)
{
  NxU32 scount = actor->getNbShapes();
  NxShape * const * shapes = actor->getShapes();
  for (NxU32 i=0; i<scount; i++)
  {
    shapes[i]->setGroup( (NxCollisionGroup)group );
  }
}

void groupSetup(NxScene *scene)
{
  if ( scene )
  {
    gHBPhysics->fetchResults(scene);
    NxU32 acount = scene->getNbActors();
    if ( acount )
    {
      NxActor **alist = scene->getActors();
      for (NxU32 i=0; i<acount; i++)
      {
        NxActor *actor = alist[i];
        const char *properties = actor->getName();
        if ( properties )
        {
          HeU32 count;
          const char **kv = getKeyValues(properties,count);
          for (HeU32 i=0; i<count; i++)
          {
            const char *key   = kv[0];
            const char *value = kv[1];
            if ( stricmp(key,"pathing_mode") == 0 )
            {
              if ( stricmp(value,"BLOCK_ONLY") == 0 )
              {
                setGroup(actor,CG_BLOCK_ONLY);
              }
              else if ( stricmp(value,"IGNORE") == 0  )
              {
                setGroup(actor,CG_IGNORE_FOR_PATH_FINDING);
              }
            }
            kv+=2;
          }
        }
      }
    }
  }
}

class PathSolver;
class PathSystem;

typedef USER_STL::hash_map< PATHSYSTEM::PATHGUID, PathSolver * > PathSolverMap;
typedef USER_STL::hash_map< PATHSYSTEM::PATHSYSTEMGUID, PathSystem * > PathSystemMap;
typedef USER_STL::hash_map< HeU32, HeU32 > IntIntMap;


bool gWeightDir=true;

enum PluginCommand
{
  PC_HELP,
  PC_HELLO_WORLD,
  PC_SNAP_TO_GROUND,
  PC_STRING_PULLING,
  PC_SMOOTH_PATH,
};



class PreserveSendTextMessage
{
public:
  PreserveSendTextMessage(SendTextMessage *stm)
  {
    mSave = gSendTextMessage;
    gSendTextMessage = stm;
  }
  ~PreserveSendTextMessage(void)
  {
    gSendTextMessage = mSave;
  }

  SendTextMessage *mSave;
};

class PathData : public RESOURCE_INTERFACE::ResourceInterfaceCallback
{
public:

  PathData(void *mem,HeU32 len,const StringRef &name)
  {
    mRebuildCount   = 0;
    mEyePos[0]      = 0;
    mEyePos[1]      = 0;
    mEyePos[2]      = 0;
    mDataMem        = mem;
    mDataLen        = len;
    mShowDistance   = 10;
    mScene          = 0;
    mPathMaker      = 0;
    mWaiting        = false;
    mPathFind       = 0;
    mPathFindCount  = 0;
    mNodeCount      = 0;
    mEdgeCount      = 0;
    mReferenceCount = 0;
    mRebuildCount++;
    mPathFind       = doLoadAI(mem,len,mNodeCount,mEdgeCount);
    mPathFindCount++;
    mReloadTime     = 0;
    mName           = name;

    HE_ASSERT( strstr(mName.Get(),".ai") );

    if ( mNodeCount == 0 )
    {
      doRelease(mPathFind);
      mPathFind = 0;
    }

  }

  PathData(const StringRef &name,bool loadOk)
  {
    mScene          = 0;
    mRebuildCount   = 0;
    mReloadTime     = 0;
    mEyePos[0]      = 0;
    mEyePos[1]      = 0;
    mEyePos[2]      = 0;
    mDataMem        = 0;
    mDataLen        = 0;
    mShowDistance   = 100;
    mPathMaker      = 0;
    mName           = name;
    HE_ASSERT( strstr(mName,".ai"));
    mWaiting        = false;
    mPathFind       = 0;
    mPathFindCount  = 0;
    mNodeCount      = 0;
    mEdgeCount      = 0;
    mReferenceCount = 0;
    mTimeStamp      = 0;
    SEND_TEXT_MESSAGE(0,"PathData(%s) constructed.\r\n", mName.Get() );
    if ( loadOk )
    {
      reload();
    }
  }

  PathData(AIPathFind *pfind,HeU32 ncount,HeU32 ccount)
  {
    mReloadTime = 0;
    mWaiting = false;
    mPathFind = pfind;
    mNodeCount = ncount;
    mEdgeCount = ccount;
    mPathFindCount = 1;
  }

  ~PathData(void)
  {
    release();
  }

  void reload(void)
  {
    if ( mWaiting == false ) // if reload not already pending..
    {
      release();
      mWaiting = true;
      gResourceInterface->getResource(mName.Get(),(PathData *)this,(RESOURCE_INTERFACE::ResourceInterfaceCallback *)this,RESOURCE_INTERFACE::RIF_NONE);
    }
  }


  HeI32 getRebuildCount(void) const { return mRebuildCount; };

  const StringRef& getName(void) const { return mName; };

  bool getPathDataVersion(HeU32 &build_version,HeU32 &asset_version,HeU64 &timeStamp)
  {
    bool ret = false;

    if ( mPathFind )
    {
      ret = mPathFind->getPathDataVersion(build_version,asset_version,timeStamp);
    }

    return ret;
  }

  bool rebuildPathData(NxScene *scene,const HeF32 *eyePos,HeF32 radius,const PATHSYSTEM::PathBuildProperties *properties)
  {
    bool ret = false;

    ret = forceRebuild(scene,eyePos,radius,0,0,properties);

    return ret;
  }

  bool savePathData(HeU32 asset_version,HeU64 timeStamp)
  {
    bool ret = false;

    ret = saveSolution(asset_version,timeStamp);

    return ret;
  }

  bool isRebuildComplete(void)
  {
    bool ret = true;

    if ( mPathMaker )
    {
      ret = false;
    }

    return ret;
  }

  bool forceRebuild(NxScene *scene,const HeF32 *eyePos,HeF32 showDistance,HeI32 version_number,HeU64 timeStamp,const PATHSYSTEM::PathBuildProperties *properties)
  {
    bool ret = false;

    release();

    mScene        = scene;
    mEyePos[0]    = eyePos[0];
    mEyePos[1]    = eyePos[1];
    mEyePos[2]    = eyePos[2];
    mShowDistance = showDistance;
    mPathMaker = beginPathMaker(mScene,mEyePos,mShowDistance,gMinNodeSize,gMaxNodeSize,version_number,timeStamp,properties);
    if ( mPathMaker )
    {
      ret = true;
    }
    return ret;
  }

  void release(void)
  {
    if ( mPathMaker )
    {
      releasePathMaker(mPathMaker);
      mPathMaker = 0;
    }

    if ( mWaiting )
    {
      gResourceInterface->releaseResourceHandles(this);
      mWaiting = false;
    }

    if ( mPathFind )
    {
      doRelease(mPathFind);
      mPathFind = 0;
      mPathFindCount++;
    }

    if ( mDataMem )
    {
      MEMALLOC_FREE(mDataMem);
      mDataMem = 0;
      mDataLen = 0;
    }

    mNodeCount = 0;
    mEdgeCount = 0;
  }

  RESOURCE_INTERFACE::ResourceUpdate notifyResource(RESOURCE_INTERFACE::RESOURCE_HANDLE /*handle*/,const void *mem,HeU32 len,void * userData,const char *source_options,const char *resource_options)
  {
    RESOURCE_INTERFACE::ResourceUpdate ret = RESOURCE_INTERFACE::RU_RELEASE_NO_UPDATE;

    mWaiting = false;

    SEND_TEXT_MESSAGE(0,"AI_DATA_SIZE(%d)\r\n", len );

#if 0 // temporary hack for testing, pretend no master graph exists
    mem = 0;
    len = 0;
#endif

    if ( mem && len )
    {
      mRebuildCount++;
      mPathFind = doLoadAI(mem,len,mNodeCount,mEdgeCount);
      mPathFindCount++;
      if ( mNodeCount == 0 )
      {
        doRelease(mPathFind);
        mPathFind = 0;
      }
      if ( mPathFind )
      {
        SEND_TEXT_MESSAGE(0,"PathData(%s) loaded with %s nodes with %s connections.\r\n", mName.Get(), formatNumber(mNodeCount), formatNumber(mEdgeCount) );
      }
      else
      {
        SEND_TEXT_MESSAGE(0,"PathData(%s) loaded but no valid AI data found.  Version mismatch!??\r\n", mName.Get() );
      }
    }
    else
    {
      SEND_TEXT_MESSAGE(0,"PathData(%s) not found.\r\n", mName.Get() );
    }

    return ret;
  };

  bool hasNoData(void) const
  {
    bool ret = false;

    // If we are not waiting for an asset
    // And the path-find is empty
    // and we are not re-generating path data..
    // then 'hasNoData' is true
    if ( !mWaiting && mPathFind == 0 && mPathMaker == 0)
    {
      ret = true;
    }

    return ret;
  }

  bool isWaiting(void) const { return mWaiting; };


  HeU32     render(const HeF32 *eyePos,HeF32 showDistance,bool showBounds,bool showConnections,bool showSolid,bool showPathCacheValid,bool showPathCacheInvalid,PathInstance *edges)
  {
    HeU32 ret = 0;

    HeStatsProbe _p("HbPathSystem->render","Global Physics Update");

    if ( !mWaiting && mPathFind && gBaseRenderDebug )
    {
      gBaseRenderDebug->setRenderScale(gRenderScale);
      ret = mPathFind->render(eyePos,showDistance,showBounds,showConnections,showSolid,showPathCacheValid,showPathCacheInvalid,edges);
      gBaseRenderDebug->setRenderScale(1);
    }

    return ret;
  }

  AIPathFind * getPathFind(void) const { return mPathFind; };
  HeU32 getPathFindCount(void) const { return mPathFindCount; };


  bool process(void)
  {
    bool ret = false;

    if ( mPathMaker )
    {
      ret = true;
      bool connectionPhase;
      bool  complete = processPathMaker(mPathMaker,false,connectionPhase,true);
      if ( complete )
      {
        MEMALLOC_FREE(mDataMem);
        mDataMem = 0;
        HeU32 len;
        void *mem = doGetPathData(mPathMaker,len);
        releasePathMaker(mPathMaker);
        mPathMaker = 0;
        if ( mem )
        {
          mRebuildCount++;
          mPathFind = doLoadAI(mem,len,mNodeCount,mEdgeCount);
          mDataMem = mem;
          mDataLen = len;
          mPathFindCount++;
        }
      }
    }

    return ret;
  }


  bool saveSolution(HeU32 assetVersion,HeU64 timeStamp)
  {
    bool ret = false;

    if ( gResourceInterface && mDataMem )
    {
      AIPathHeader *header = (AIPathHeader *) mDataMem;
      header->mSourceVersionNumber = assetVersion;
      header->mAssetTimeStamp = timeStamp;
      SEND_TEXT_MESSAGE(0,"Putting NavMesh(%s) with %s nodes and %s connections, totally %s bytes of data.\r\n", mName.Get(), formatNumber(mNodeCount),formatNumber(mEdgeCount),formatNumber(mDataLen));
      gResourceInterface->putResource(mName.Get(),mDataMem,mDataLen,0,0,RESOURCE_INTERFACE::RIF_NONE);
      ret = true;
    }

    return ret;
  }

  bool rebuildCancel(void)
  {
    bool ret = false;

    if ( mPathMaker )
    {
      MEMALLOC_FREE(mDataMem);
      mDataMem = 0;
      releasePathMaker(mPathMaker);
      mPathMaker = 0;
      ret = true;
    }

    return ret;
  }

  bool rebuildComplete(void)
  {
    bool ret = true;
    if ( mPathMaker )
    {
      ret = false;
    }
    return ret;
  }

  bool getRandomPoint(HeF32 *point,PATHSYSTEM::PathProperties *properties)
  {
    bool ret = false;

    if ( mPathFind )
    {
      ret = mPathFind->getRandomPoint(point,properties);
    }

    return ret;
  }

  PATHSYSTEM::PathNode * getNodesInRange(const HeF32 * point,HeF32 radius,HeU32 &count,PATHSYSTEM::PathProperties *props)
  {
    PATHSYSTEM::PathNode *ret = 0;

    if ( mPathFind )
    {
      ret = mPathFind->getNodesInRange(point,radius,count,props,0);
    }
    return ret;
  }

  bool getRandomPointInVolume(const HeBounds3 &b,HeVec3 &point,PATHSYSTEM::PathProperties * props)
  {
    bool ret = false;

    if ( mPathFind )
    {
      ret = mPathFind->getRandomPointInVolume(b,point,props);
    }

    return ret;
  }

  bool getRandomPointInSphere(const HeVec3 &center,HeF32 radius,HeVec3 &point,PATHSYSTEM::PathProperties * props)
  {
    bool ret = false;

    if ( mPathFind )
    {
      ret = mPathFind->getRandomPointInSphere(center,radius,point,props);
    }

    return ret;
  }

  bool getNearestPoint(HeF32 *point,PathInstance *pinst,PATHSYSTEM::PathProperties *properties,bool validate)
  {
    bool ret = false;

    if ( mPathFind )
    {
      ret = mPathFind->getNearestPoint(point,pinst,properties,validate);
    }
    return ret;
  }

  PATHSYSTEM::PathNode * getNearestPointConnections(HeF32 *point,PathInstance *pinst,PATHSYSTEM::PathProperties *properties,HeU32 &pcount)
  {
    PATHSYSTEM::PathNode *ret = 0;

    if ( mPathFind )
    {
      ret = mPathFind->getNearestPointConnections(point,pinst,properties,pcount);
    }

    return ret;
  }

  HeI32 incrementReferenceCount(void)
  {
    mReferenceCount++;
    return mReferenceCount;
  }

  HeI32 decrementReferenceCount(void)
  {
    mReferenceCount--;
    return mReferenceCount;
  }

  HeU32 getEdgeCount(void) const { return mEdgeCount; };

  void getStatus(std::string &status,const char *label)
  {
    if ( mPathMaker )
    {
      const char *s = doGetStatus(mPathMaker);
      if ( s == 0 ) s = "";
      addStatus(status,"%s: %s", label, s );
    }
    else
    {
      addStatus(status,"%s: has %d nodes and %d edges.", label, mName.Get(), mNodeCount, mEdgeCount );
    }
  }

  HeI32 getReferenceCount(void) const
  {
    return mReferenceCount;
  };

//private:

  HeI32                 mReferenceCount;
  HeI32                 mRebuildCount;
  bool                  mWaiting;
  StringRef             mName;
  HeU64                 mTimeStamp;
  HeU32                 mPathFindCount;
  AIPathFind           *mPathFind;
  HeU32                 mNodeCount;
  HeU32                 mEdgeCount;
  NxScene              *mScene;
  AIPathMaker          *mPathMaker;
  HeF32                 mEyePos[3];
  HeF32                 mShowDistance;
  HeU32                 mDataLen;
  void                 *mDataMem;
  HeF32                 mReloadTime;
};

class PathSolver
{
public:

  PathSolver(PATHSYSTEM::PATHGUID guid,const PATHSYSTEM::PathRequestDesc &desc,PathData *pathData,NxScene *scene,const PATHSYSTEM::PathBuildProps &props)
  {

#if TIME_LOG

    if ( gTimeLog == 0 )
    {
      gTimeLog = MEMALLOC_NEW(TimeLog);
      gTimeLog->beginLog("PATHSYSTEM");
    }

    gTimeLog->log("PathSolver(%0.9f,%0.9f,%0.9f, %0.9f,%0.9f,%0.9f) Area(%s)\r\n",desc.mFrom[0],desc.mFrom[1],desc.mFrom[2], desc.mTo[0], desc.mTo[1], desc.mTo[2], pathData->getName().Get() );


#endif

    mKey[0] = 'P';
    mKey[1] = 'S';
    mKey[2] = 'O';
    mKey[3] = 'L';
    mKey[4] = 'V';
    mKey[5] = 'E';
    mKey[6] = 0;

    mStartFrame = gFrameCount;
    mProperties = desc.mProperties;
    mScene    = scene;
    mGuid     = guid;
    mDesc     = desc;
    mPathData = pathData;
    mPathFind = 0;
    mTime     = 0;
    mPathSolver = 0;

    mDesc.mFrom[0]*=gGameScale;
    mDesc.mFrom[1]*=gGameScale;
    mDesc.mFrom[2]*=gGameScale;

    mDesc.mTo[0]*=gGameScale;
    mDesc.mTo[1]*=gGameScale;
    mDesc.mTo[2]*=gGameScale;

    HE_ASSERT(mDesc.mCallback);

    gHBPhysics->getGrounding(mScene,mDesc.mFrom,2.0f,props.mCharacterWalkSlope,mDesc.mFrom[1],0,props.mCharacterWidth,props.mCharacterHeight,false,false,CGF_PATH_COLLISION);
    gHBPhysics->getGrounding(mScene,mDesc.mTo,2.0f,props.mCharacterWalkSlope,mDesc.mTo[1],0,props.mCharacterWidth,props.mCharacterHeight,false,false,CGF_PATH_COLLISION);

    mTotalSolverCount = 0;
  }


  bool validate(void) const
  {
    bool ret = false;

    HE_ASSERT( mKey[0] == 'P' );
    HE_ASSERT( mKey[1] == 'S' );
    HE_ASSERT( mKey[2] == 'O' );
    HE_ASSERT( mKey[3] == 'L' );
    HE_ASSERT( mKey[4] == 'V' );
    HE_ASSERT( mKey[5] == 'E' );
    HE_ASSERT( mKey[6] == 0 );

    if ( mKey[0] == 'P' &&
         mKey[1] == 'S' &&
         mKey[2] == 'O' &&
         mKey[3] == 'L' &&
         mKey[4] == 'V' &&
         mKey[5] == 'E' &&
         mKey[6] == 0  )
    {
      ret = true;
    }
    return ret;
  }

  ~PathSolver(void)
  {
    validate();
    if ( mPathSolver )
    {
      doReleaseFindPath(mPathSolver);
    }
    mKey[0] = 0;
    mKey[1] = 0;
    mKey[2] = 0;
    mKey[3] = 0;
    mKey[4] = 0;
    mKey[5] = 0;
    mKey[6] = 0;
  }

  void restart(PathData *pathData)
  {
    validate();

    if ( mPathSolver )
    {
      doReleaseFindPath(mPathSolver);
      mPathSolver = 0;
    }

    mPathFind  = 0;
    mPathData = pathData;
    mTime     = 0;
    mTotalSolverCount = 0;
  }

  bool pumpLife(PATHSYSTEM::PATHSYSTEMGUID parentGuid,HeF32 dtime)
  {
    bool ret = false;

    validate();

    if ( mDesc.mCallback )
    {

      mTime+=dtime;

      HeF32 timeOut = 4;
      if ( mProperties )
      {
        timeOut = mProperties->mTimeOut;
      }

      if ( mTime < timeOut )
      {
        ret = true;
      }
      else
      {
        ret = false;

        HE_ASSERT( mDesc.mCallback );
        if ( mDesc.mCallback )
        {
          mDesc.mCallback->receivePathSolution(parentGuid,mGuid,mDesc.mUserData,mDesc.mUserId1,mDesc.mUserId2,0,0,mTotalSolverCount,mTime);
          mDesc.mCallback = 0;
        }

      }

    }

    return ret;
  }

  void cancel(PATHSYSTEM::PATHSYSTEMGUID parentGuid)
  {
    validate();
    HE_ASSERT( mDesc.mCallback );
    if ( mDesc.mCallback )
    {
      mDesc.mCallback->receivePathSolution(parentGuid,mGuid,mDesc.mUserData,mDesc.mUserId1,mDesc.mUserId2,0,0,mTotalSolverCount,mTime);
      mDesc.mCallback = 0;
    }
  }

  bool pump(PATHSYSTEM::PATHSYSTEMGUID parentGuid,HeF32 dtime,HeU32 solverCount,PathInstance *edges,HeF32 globalTime,const PATHSYSTEM::PathBuildProps &props)  // true if still alive, false if time to kill it.
  {
    bool ret = false;

    validate();
    if ( mDesc.mCallback )
    {

      mTime+=dtime;
      HeF32 timeOut = 4;
      if ( mProperties )
      {
        timeOut = mProperties->mTimeOut;
      }

      if ( mTime < timeOut )
      {

        ret = true;

        if ( mPathFind == 0 ) // if we don't have a pathing data set
        {
          HE_ASSERT(mPathSolver==0);

          mPathFind = mPathData->getPathFind();  // see if it is available yet

          if ( mPathFind ) // if it is now available, startup of a path solver tasks
          {
            HeF32 sdist = fm_distanceSquaredXZ( mDesc.mFrom, mDesc.mTo );
            if ( sdist < (4*4) )
            {
              PATHSYSTEM::PathNode nodes[2];

              HB_PHYSICS::GroundingState g1 = gHBPhysics->getGrounding(mScene,mDesc.mFrom,2.0f,props.mCharacterWalkSlope,mDesc.mFrom[1],0,props.mCharacterWidth,props.mCharacterHeight,false,false,CGF_PATH_COLLISION);
              HB_PHYSICS::GroundingState g2 = gHBPhysics->getGrounding(mScene,mDesc.mTo,2.0f,props.mCharacterWalkSlope,mDesc.mTo[1],0,props.mCharacterWidth,props.mCharacterHeight,false,false,CGF_PATH_COLLISION);

              HeU32 ncount = 0;
              PATHSYSTEM::PathNode *nptr = 0;

              if ( g1 == HB_PHYSICS::GS_OK && g2 == HB_PHYSICS::GS_OK )
              {
                HeF32 epos[3];

                bool canWalk = gHBPhysics->canWalk(mScene,mDesc.mFrom,mDesc.mTo, props.mCharacterWidth,props.mCharacterHeight,props.mCharacterStepHeight,props.mCharacterWalkSlope, epos, true, false, CGF_PATH_COLLISION,false );

                if ( canWalk )
                {
                  ncount = 2;
                  nodes[0].simpleCenterBox(mDesc.mFrom,gInverseGameScale);
                  nodes[1].simpleCenterBox(epos,gInverseGameScale);
                  nptr   = nodes;
                }

              }
              HE_ASSERT(mDesc.mCallback);
              if ( mDesc.mCallback )
              {
                mDesc.mCallback->receivePathSolution(parentGuid,mGuid,mDesc.mUserData,mDesc.mUserId1,mDesc.mUserId2,ncount,nptr,mTotalSolverCount,mTime);
                mDesc.mCallback = 0;
              }
            }
            else
            {
              mPathSolver = mPathFind->startFindPath(mDesc.mFrom,mDesc.mTo,edges,mProperties,props);
              if ( mPathSolver == 0 )
              {
                HE_ASSERT(mDesc.mCallback);
                if ( mDesc.mCallback )
                {
                  mDesc.mCallback->receivePathSolution(parentGuid,mGuid,mDesc.mUserData,mDesc.mUserId1,mDesc.mUserId2,0,0,mTotalSolverCount,mTime);
                  mDesc.mCallback = 0;
                }
                ret = false;
              }
            }
          }
        }

        if ( mPathSolver )
        {
          bool ok;
          HeU32 ncount;
          HeU32 totalSolverSteps;

          PATHSYSTEM::PathNode *result =  doFindPath(mPathSolver,ncount,solverCount,ok,totalSolverSteps,globalTime); // see if we have a solution yet.

          mTotalSolverCount+=totalSolverSteps;


          if ( result || !ok)
          {
            if ( ncount >= 2 )
            {
              processPathSolution(parentGuid,ncount,result,edges,globalTime,props);
            }
            else
            {
              if ( ncount == 1 )
              {
                SEND_TEXT_MESSAGE(0,"Produced One Node as a result? (%0.9f,%0.9f,%0.9f) to (%0.9f,%0.9f,%0.9f)\r\n", mDesc.mFrom[0], mDesc.mFrom[1], mDesc.mFrom[2], mDesc.mTo[0], mDesc.mTo[1], mDesc.mTo[2] );
              }
              HE_ASSERT(mDesc.mCallback);
              if ( mDesc.mCallback )
              {
                mDesc.mCallback->receivePathSolution(parentGuid,mGuid,mDesc.mUserData,mDesc.mUserId1,mDesc.mUserId2,0,0,mTotalSolverCount,mTime);
                mDesc.mCallback = 0;
              }
            }

            doReleaseFindPath(mPathSolver);

            mPathSolver = 0;
            ret = false;
          }
        }

      }
      else
      {
        HE_ASSERT( mDesc.mCallback );
        if  (mDesc.mCallback )
        {
          mDesc.mCallback->receivePathSolution(parentGuid,mGuid,mDesc.mUserData,mDesc.mUserId1,mDesc.mUserId2,0,0,mTotalSolverCount,mTime);
          mDesc.mCallback = 0;
        }
      }
    }

    return ret;
  }

  PATHSYSTEM::PATHGUID getGuid(void) const
  {
    return mGuid;
  };

  PATHSYSTEM::PathNodeInterface * getCallback(void) const
  {
    HE_ASSERT(mDesc.mCallback);
    return mDesc.mCallback;
  };

  void processPathSolution(PATHSYSTEM::PATHSYSTEMGUID parentGuid,HeU32 ncount,PATHSYSTEM::PathNode *result,PathInstance *pinstance,HeF32 globalTime,const PATHSYSTEM::PathBuildProps &props);

  bool match(AIPathFind *pfind) const
  {
    return (pfind==mPathFind) ? true : false;
  }

  char                                    mKey[8];
  HeF32                                   mTime;
  HeU32                                   mStartFrame;
  PATHSYSTEM::PATHGUID                  mGuid;
  PATHSYSTEM::PathRequestDesc           mDesc;
  PathData                               *mPathData;
  AIPathSolver                           *mPathSolver;
  AIPathFind                             *mPathFind;
  HeU32                                   mSolverBudget;
  HeI32                                   mTotalSolverCount;
  NxScene                                *mScene;
  PATHSYSTEM::PathProperties           *mProperties;
};



typedef USER_STL::map< StringRef , PathData * >    PathDataMap;
typedef USER_STL::vector< PATHSYSTEM::PATHGUID > PathGuidVector;

class MergePathData : public AIPathFindInterface
{
public:
  MergePathData(AIPathMaker *merge,AIPathFind *master)
  {
    mMerge = merge;
    mMaster = master;
  }

  void addNodes(void)
  {
    if ( mMaster )
      mMaster->enumerateNodes(this);
  }

  void addConnections(void)
  {
    if ( mMaster )
      mMaster->enumerateConnections(this);
  }

  void reportNode(HeU32 index,const HeF32 *center,const HeF32 *bmin,const HeF32 *bmax,HeU32 flags)
  {
    HeU32 new_index = doAddNode(mMerge,center,bmin,bmax,flags);
    if ( new_index != 0xFFFFFFFF )
    {
      mIndices[index] = new_index;
    }
  }

  void reportConnection(HeU32 from,HeU32 to)
  {
    IntIntMap::iterator found1;
    IntIntMap::iterator found2;
    found1 = mIndices.find(from);
    found2 = mIndices.find(to);
    if ( found1 != mIndices.end() && found2 != mIndices.end() )
    {
      HeU32 c1 = (*found1).second;
      HeU32 c2 = (*found2).second;
      doAddConnection(mMerge,c1,c2);
    }
  }

private:
  AIPathFind         *mMaster;
  AIPathMaker        *mMerge;
  IntIntMap           mIndices;
};

class PathSystem : public PathEdgeInterface
{
public:
  PathSystem(PathData *data,NxScene *scene,PATHSYSTEM::PATHSYSTEMGUID guid,PathPlanning *pp,const StringRef &fqn)
  {
    mFQN                   = fqn;
    mPathPlanning          = pp;
    mTime                  = 0;
    mMergePathData         = 0;
    mMessageInterface      = 0;
    mUserData              = 0;
    mUserId1               = 0;
    mUserId2               = 0;
    mPathData              = data;
    mPathData->incrementReferenceCount();
    mInstancePathData      = 0;
    mScene                 = scene;
    mGuid                  = guid;
    mShowBounds            = true;
    mShowSolid             = false;
    mShowConnections       = true;
    mShowPathCacheValid    = false;
    mShowPathCacheInvalid  = false;
    mPathFindCount         = 0;
    mRebuildEdgeCount      = 1;
    mRebuildCount          = 0;
    mPathMaker             = 0;
    mSceneCopy             = 0;
    mRegionUpdateScene     = 0;
    mSendTextMessage       = 0;
    mRememberGuid          = 0;
    groupSetup(scene);
    rebuildEdges();
  }

  ~PathSystem(void);

  bool setPathSystemSendTextMessage(SendTextMessage *stm)
  {
    mSendTextMessage = stm;
    return true;
  }

  void refreshClient(void)
  {
    if ( mMessageInterface )
    {
      Streamer s((void *)0,0);
      s.write( (HeU32)HWCS_PATH_REFRESH);
      HeU32 wlen;
      void *mem = s.getWriteBuffer(wlen);
      mMessageInterface->sendHeroWorldMessage(mem,wlen,mUserData,mUserId1,mUserId2);
    }

  }

  void releasePathData(void);

  PathData * getPathData(void) const
  {
    PathData *ret = 0;
    if ( mInstancePathData )
    {
      HE_ASSERT(!mPathData);
      ret = mInstancePathData;
    }
    else
    {
      HE_ASSERT(mPathData);
      ret = mPathData;
    }
    return ret;
  }

  HeU32     render(const HeF32 *eyePos,HeF32 showDistance,
                                  void *userData,
                                  HEROWORLD::HeroWorldMessageInterface *iface,
                                  HeU64 userId1,
                                  HeU64 userId2)

  {
    HeU32 ret = 0;

    mMessageInterface = iface;
    mUserData         = userData;
    mUserId1          = userId1;
    mUserId2          = userId2;


    getEdges();
    ret = getPathData()->render(eyePos,showDistance,mShowBounds,mShowConnections,mShowSolid,mShowPathCacheValid,mShowPathCacheInvalid,&mInstance);

    return ret;
  }

  void getEdges(void)
  {
    HeU32 pcount = getPathData()->getPathFindCount();
    if ( pcount != mPathFindCount )
    {
      mPathFindCount = pcount;
      rebuildEdges();
    }
  }

  void restartPathSolvers(void)
  {
    PathSolverMap::iterator i;
    for (i=mPathSolutions.begin(); i!=mPathSolutions.end(); ++i)
    {
      PathSolver *ps = (*i).second;
      HE_ASSERT( ps->getGuid() == (*i).first );
      ps->restart(getPathData());
      HE_ASSERT( ps->getGuid() == (*i).first );
    }
  }

  void rebuildEdges(void)
  {
    mRebuildEdgeCount++;
    mInstance.set( getPathData()->getPathFind());
    getPathFindingNodes();
    refreshClient();
    restartPathSolvers();
  }


  void getPathFindingNodes(void)
  {
    HB_PHYSICS::OverlapRequest request;

    request.mType              = HB_PHYSICS::ORT_SPHERE;
    request.mGroupFlags        = CGF_PATHFINDING_NODE;
    request.mSphere.mCenter[0] = 0;
    request.mSphere.mCenter[1] = 0;
    request.mSphere.mCenter[2] = 0;
    request.mSphere.mRadius    = 100000.0f;

    HeU32 count = gHBPhysics->overlapImmediate(mScene,request);

    if ( count )
    {

      for (HeU32 i=0; i<count; i++)
      {
        HB_PHYSICS::ShapeInfo &info = request.mShapeInfo[i];
        NxShape *shape = info.mShape;
        NxBoxShape *boxShape = shape->isBox();

        HE_ASSERT( boxShape );

        if ( boxShape )
        {

          bool weighted=true;
          bool portalToHeightmap=true;
          bool active=true;
          HeF32 pathingCost=100;

          const char *properties = shape->getActor().getName();

          HeU32 count;
          const char **kv = getKeyValues(properties,count);
          for (HeU32 i=0; i<count; i++)
          {
            const char *key = kv[0];
            const char *value = kv[1];
            if ( strcmp(key,"active") == 0 )
            {
              active = getBool(value);
            }
            else if ( strcmp(key,"pathing_cost") == 0 )
            {
              pathingCost = (HeF32) atof(value);
            }
            else if ( strcmp(key,"weighted") == 0 )
            {
              weighted = getBool(value);
            }
            else if ( strcmp(key,"portal_to_heightmap") == 0 )
            {
              portalToHeightmap = getBool(value);
            }
            kv+=2;
          }

          if ( active )
          {
            weighted = true;
          }

          if ( count )
          {
            mActive    = active;
            mWeighted = weighted;

            mPathingCost = pathingCost/100.0f;

            shape->getWorldBounds(mWorldBounds);
            boxShape->getWorldOBB(mOBB);

            mOBB.rot.getInverse(mInverse);

            mBmin = -mOBB.extents;
            mBmax =  mOBB.extents;

            AIPathFind *pfind = 0;

            pfind = getPathData()->getPathFind();

            if ( pfind )
            {
              pfind->intersectEdges( this, &mWorldBounds.min.x, &mWorldBounds.max.x, true );
            }

            mEdgeTesting.clear();

          }
        }
      }

    }

  }

  bool pumpBuild(void)
  {
    bool ret = false;

    PreserveSendTextMessage stm(mSendTextMessage);

    if ( mInstance.mParent == 0)
    {
      AIPathFind * parent = getPathData()->getPathFind();
      if ( parent )
      {
        mInstance.set(parent);
      }
    }

    if ( mInstancePathData )
    {
      HE_ASSERT(!mPathData);
      HeI32 rc = mInstancePathData->getRebuildCount();
      if ( rc != mRebuildCount )
      {
        mRebuildCount = rc;
        refreshClient();
      }
    }
    else
    {
      HE_ASSERT(mPathData);
      if ( mPathData->hasNoData() && mPathMaker == 0 )
      {
        // if the parent has no data, then we have to generate the data locally!
        SEND_TEXT_MESSAGE(0,"No pre-existing master graph data, so we are rebuilding it locally.\r\n");
        HeF32 ESIZE=10000;
        HeF32 region[6] = { -ESIZE, -ESIZE, -ESIZE, ESIZE, ESIZE, ESIZE };
        regionUpdate(region);
      }
    }


    if ( mPathMaker )
    {
      ret = true;

      bool connectionPhase=false;
      bool finished = processPathMaker(mPathMaker,true,connectionPhase,true);

      if ( connectionPhase )
      {
        SEND_TEXT_MESSAGE(0,"Merging Region Update with Main graph.\r\n");
        AIPathFind *merge = getPathData()->getPathFind();

        if ( NULL != merge )
        {
          mMergePathData = MEMALLOC_NEW(MergePathData)(mPathMaker,merge);
          mMergePathData->addNodes();
          doRebuildTree(mPathMaker); // rebuild the KdTree with the new nodes added.
        }
      }

      if ( finished )
      {
        if ( mMergePathData == 0 )
        {
          AIPathFind *merge = getPathData()->getPathFind();
          mMergePathData = MEMALLOC_NEW(MergePathData)(mPathMaker,merge);
          mMergePathData->addNodes();
          doRebuildTree(mPathMaker); // rebuild the KdTree with the new nodes added.
        }

        mMergePathData->addConnections();

        delete mMergePathData;
        mMergePathData = 0;

        HeU32 len;
        void *mem = doGetPathData(mPathMaker,len);
        gActiveRegionCount--;
        releasePathMaker(mPathMaker);
        mInstance.clearRegionUpdate();
        mPathMaker = 0;

        if ( mSceneCopy )
        {
          PHYSICSASSET::PhysicsAssetPlugin *pa = gHBPhysics->getPhysicsAssetPlugin();
          pa->releaseSceneCopy(mSceneCopy);
          mSceneCopy = 0;
          mRegionUpdateScene = 0;
        }

        delete mInstancePathData;
        mInstancePathData = 0;
        mInstancePathData = MEMALLOC_NEW(PathData)(mem,len,mFQN);
        mPathFindCount = 0;
        releasePathData();
        rebuildEdges();

        SEND_TEXT_MESSAGE(0,"@PM:REGION UPDATE COMPLETE!");
      }

    }


    return ret;
  }

  bool pump(HeF32 dtime,HeU32 stime,bool &haveWork)
  {
    HeStatsProbe _p("PathSystem->pump","PathSolving");

    bool ret = true;

    mTime+=dtime;

    if ( mInstance.mStringPull )
    {
      mInstance.mStringPull->bringOutYourDead(mTime);
    }

    if ( !mPathSolutions.empty() )
    {

      PATHSYSTEM::PathBuildProps props;

      const PATHSYSTEM::PathBuildProperties *properties=0;
      HE_ASSERT(mPathPlanning);

      if ( mPathPlanning )
      {
        properties = mPathPlanning->getProperties();
        HE_ASSERT(properties);
        if ( properties )
        {
          props = properties->mDefault;
        }
      }

      PreserveSendTextMessage stm(mSendTextMessage);

      {


        PathSolverMap::iterator k = mPathSolutions.find(mRememberGuid);
        if ( k == mPathSolutions.end() )
        {
          k = mPathSolutions.begin();
          mRememberGuid = 0;
        }


        while ( k != mPathSolutions.end() )
        {
          PathSolver *ps = (*k).second;

          HE_ASSERT( ps->getGuid() == (*k).first );

          HeI32 priority = 1;

          if ( ps->mProperties )
          {
            priority = ps->mProperties->mPriority;
          }

          HeU32 count = SOLVER_COUNT_PER*priority;

          bool alive = ps->pump(mGuid,dtime,count,&mInstance,mTime,props);

          if ( !alive )
          {
            HE_ASSERT( ps->getGuid() == (*k).first );
            k = mPathSolutions.erase(k);
            delete ps;
          }
          else
          {
            k++;
          }

          HeU32 etime = timeGetTime();
          HeU32 diff = etime-stime;

          // if we have exceeded our solution time budget....
          if ( diff > gMaxSolveTime )
          {
            ret = false;

            if ( dtime > 0 )
            {


              if ( k != mPathSolutions.end() )
              {
                mRememberGuid = (*k).first;
              }

              while ( k != mPathSolutions.end()  )
              {
                ps = (*k).second;

                HE_ASSERT( ps->getGuid() == (*k).first );

                bool alive = ps->pumpLife(mGuid,dtime);
                if ( !alive )
                {
                  HE_ASSERT( ps->getGuid() == (*k).first );
                  delete ps;
                  k = mPathSolutions.erase(k);
                }
                else
                {
                  k++;
                }
              }

            }
            break;
          }
        }
      }

    }

    haveWork = !mPathSolutions.empty();

    return ret;
  }

  void solvePath(PATHSYSTEM::PATHSYSTEMGUID guid,const PATHSYSTEM::PathRequestDesc &_desc)
  {
    PATHSYSTEM::PathBuildProps props;
    const PATHSYSTEM::PathBuildProperties *_properties=0;
    HE_ASSERT(mPathPlanning);
    if ( mPathPlanning )
    {
      _properties = mPathPlanning->getProperties();
      HE_ASSERT(_properties);
      if ( _properties )
      {
        props = _properties->mDefault;
      }
    }

    PATHSYSTEM::PathRequestDesc desc = _desc;
    PATHSYSTEM::PathProperties *properties = 0;
    if ( gParent )
    {
      properties = gParent->getPathProperties(desc.mDataId);
    }

    desc.mProperties = properties;

    PathSolver *ps= MEMALLOC_NEW(PathSolver)(guid,desc,getPathData(),this->mScene,props);
    mPathSolutions[guid] = ps;

  }

  bool cancelPath(PATHSYSTEM::PATHSYSTEMGUID guid)
  {
    bool ret = false;

    PathSolverMap::iterator found;
    found = mPathSolutions.find(guid);
    if ( found != mPathSolutions.end() )
    {
      PathSolver *ps = (*found).second;
      HE_ASSERT( ps->getGuid() == (*found).first );
      delete ps;
      mPathSolutions.erase(found);
      ret = true;
    }

    if ( guid == mRememberGuid )
    {
      mRememberGuid = 0;
    }

    return ret;
  }

  HeU32 cancelPaths(PATHSYSTEM::PathNodeInterface *callback)
  {
    HeU32 ret = 0;

    PathSolverMap::iterator k = mPathSolutions.begin();
    while ( k!=mPathSolutions.end() )
    {
      PathSolver *ps = (*k).second;
      HE_ASSERT( ps->getGuid() == (*k).first );
      if ( ps->getCallback() == callback )
      {
        ret++;
        delete ps;
        k = mPathSolutions.erase(k);
      }
      else
      {
        k++;
      }
    }

    mRememberGuid = 0;

    return ret;
  }

  bool forceRebuildInstance(const HeF32 *eyePos,HeF32 showDistance)
  {
    bool ret = false;

    if ( showDistance >= 90 ) showDistance = 10000; // rebuild entire map!

    rebuildCancel(); // cancel any previous rebuild!

    HeF32 region[6];
    region[0] = eyePos[0]-showDistance;
    region[1] = eyePos[1]-showDistance;
    region[2] = eyePos[2]-showDistance;

    region[3] = eyePos[0]+showDistance;
    region[4] = eyePos[1]+showDistance;
    region[5] = eyePos[2]+showDistance;

    regionUpdate(region);

    return ret;
  }


  bool forceRebuild(const HeF32 *eyePos,HeF32 showDistance)
  {
    bool ret = false;

    const PATHSYSTEM::PathBuildProperties *properties=0;
    HE_ASSERT(mPathPlanning);
    if ( mPathPlanning )
      properties = mPathPlanning->getProperties();

    if ( mScene )
    {
      ret = getPathData()->forceRebuild(mScene,eyePos,showDistance*2,-1,0,properties);
    }

    return ret;
  }

  bool rebuildCancel(void)
  {
    bool ret = false;


    restartPathSolvers();

    getPathData()->rebuildCancel();

    if ( mSceneCopy )
    {
      PHYSICSASSET::PhysicsAssetPlugin *pa = gHBPhysics->getPhysicsAssetPlugin();
      pa->releaseSceneCopy(mSceneCopy);
      mSceneCopy = 0;
      mRegionUpdateScene = 0;
    }

    if ( mPathMaker )
    {
      SEND_TEXT_MESSAGE(0,"Canceling Region Rebuild that was in progress.\r\n");
      releasePathMaker(mPathMaker);
      mInstance.clearRegionUpdate();
      refreshClient();
      mPathMaker = 0;
      gActiveRegionCount--;
    }

    if ( mMergePathData )
    {
      delete mMergePathData;
      mMergePathData = 0;
    }

    return ret;
  }

  bool rebuildComplete(void)
  {
    bool ret = true;

    ret = getPathData()->rebuildComplete();

    return ret;
  }

  void regionUpdate(const HeF32 *region)
  {
    HE_ASSERT(mPathMaker==0);
    SEND_TEXT_MESSAGE(0,"@PM:Starting RegionUpdate(%0.2f,%0.2f,%0.2f)(%0.2f,%0.2f,%0.2f)", region[0],region[1],region[2],region[3],region[4],region[5]);

    if ( mSceneCopy )
    {
      PHYSICSASSET::PhysicsAssetPlugin *pa = gHBPhysics->getPhysicsAssetPlugin();
      pa->releaseSceneCopy(mSceneCopy);
      mSceneCopy = 0;
      mRegionUpdateScene = 0;
    }

    PHYSICSASSET::PhysicsAssetPlugin *pa = gHBPhysics->getPhysicsAssetPlugin();

    if ( pa )
    {
      const HeF32 REGION_SLOP = 40; // go ahead and copy a big border around the region to ensure valid connectivity later..

      HeF32 _region[6];

      _region[0] = region[0]-REGION_SLOP;
      _region[1] = region[1]-REGION_SLOP;
      _region[2] = region[2]-REGION_SLOP;

      _region[3] = region[3]+REGION_SLOP;
      _region[4] = region[4]+REGION_SLOP;
      _region[5] = region[5]+REGION_SLOP;

      mSceneCopy = pa->createSceneCopy(mScene,&_region[0],&_region[3],mRegionUpdateScene);

      // ok...now let's fix up the 'ignore' path properties.
      groupSetup(mRegionUpdateScene);

      const PATHSYSTEM::PathBuildProperties *properties=0;
      HE_ASSERT(mPathPlanning);
      if ( mPathPlanning )
      {
        properties = mPathPlanning->getProperties();
      }

      mPathMaker = beginPathMaker(mRegionUpdateScene,region,gMinNodeSize,gMaxNodeSize,properties);

      HeBounds3 b;
      b.min.set(region);
      b.max.set(&region[3]);
      mInstance.setRegionUpdate(b);
      refreshClient();
      gActiveRegionCount++;
    }
  }

  bool             setPathState(PATHSYSTEM::PathState state,bool bstate,HeU32 /*v*/,const HeF32 *fv,PATHSYSTEM::PathStateDesc * /*desc*/,HeU64 /*bigv*/)
  {
    bool ret = false;

    PreserveSendTextMessage stm(mSendTextMessage);

    switch ( state )
    {
      case PATHSYSTEM::PS_UPDATE_REGION:
        {
          HE_ASSERT(mPathMaker==0);
          HeF32 region[6];
          HE_ASSERT(fv);
          if ( fv  )
          {
            region[0] = fv[0]*gGameScale;
            region[1] = fv[1]*gGameScale;
            region[2] = fv[2]*gGameScale;

            region[3] = fv[3]*gGameScale;
            region[4] = fv[4]*gGameScale;
            region[5] = fv[5]*gGameScale;

            region[0]-=UPDATE_SLOP;
            region[1]-=UPDATE_SLOP;
            region[2]-=UPDATE_SLOP;

            region[3]+=UPDATE_SLOP;
            region[4]+=UPDATE_SLOP;
            region[5]+=UPDATE_SLOP;

            regionUpdate(region);

          }
        }
        break;
      case PATHSYSTEM::PS_RELOAD_FROM_MASTER_GRAPH:
        getPathData()->reload();
        mInstance.set(0);
        restartPathSolvers();
        break;
      case PATHSYSTEM::PS_REBUILD_EDGES:
        rebuildEdges();
        break;
      case PATHSYSTEM::PS_CANCEL_PATH_REBUILD:
        rebuildCancel();
        break;
      case PATHSYSTEM::PS_SET_SHOW_BOUNDS:
        mShowBounds = bstate;
        ret = true;
        break;
      case PATHSYSTEM::PS_SET_SHOW_SOLID:
        mShowSolid = bstate;
        ret = true;
        break;

      case PATHSYSTEM::PS_SET_SHOW_CONNECTIONS:
        mShowConnections = bstate;
        ret = true;
        break;
      case PATHSYSTEM::PS_SET_SHOW_PATH_CACHE_VALID:
        mShowPathCacheValid = bstate;
        ret = true;
        break;
      case PATHSYSTEM::PS_SET_SHOW_PATH_CACHE_INVALID:
        mShowPathCacheInvalid = bstate;
        ret = true;
        break;
    }
    return ret;
  }

  HeU32 getPathSolutionsSize(void) const
  {
    return mPathSolutions.size();
  }

  bool checkReload(PathData *data)
  {
    bool ret = false;

    if ( mPathData && mPathData == data )
    {
      mPathData->reload();
      mInstance.set(0);
      restartPathSolvers();
      ret = true;
    }
    return ret;
  }

  bool             getPathState(PATHSYSTEM::PathState state,bool * /*bstate*/,HeU32 *v,HeF32 * /*fv*/,PATHSYSTEM::PathStateDesc * /*desc*/,HeU64 * /*bigv*/)
  {
    bool ret = false;

    PreserveSendTextMessage stm(mSendTextMessage);

    switch ( state )
    {
      case PATHSYSTEM::PS_GET_SOLVER_COUNT:
        if ( v )
        {
          v[0] = mPathSolutions.size();
          ret = true;
        }
        break;
      case PATHSYSTEM::PS_GET_UPDATE_OK:
        if ( mPathMaker )
        {
          ret = false;
        }
        else
        {
          ret = true;
        }
        break;
      case PATHSYSTEM::PS_REBUILD_COUNT:
        if ( v )
        {
          ret = true;
          v[0] = mRebuildEdgeCount;
        }
        break;
    }

    return ret;
  }

  void nodeIntersect(HeU32 nodeIndex,const HeF32 *center)
  {
    NxVec3 v1(center);
    NxVec3 o1;
    v1-=mOBB.center;
    mInverse.multiply(v1,o1);
    HeU32 oc1 = fm_clipTestPoint(&mBmin.x, &mBmax.x, &o1.x );
    if ( oc1 == 0 )
    {
      mInstance.mNodeCosts[nodeIndex] = 0;
    }
  }

  void edgeIntersect(HeU32 edgeIndex,const HeF32 *p1,const HeF32 *p2,HeU32 index1,HeU32 index2)
  {

    NxVec3 v1(p1);
    NxVec3 v2(p2);

    NxVec3 o1,o2;

    v1-=mOBB.center;
    v2-=mOBB.center;

    mInverse.multiply(v1,o1);
    mInverse.multiply(v2,o2);

    HeU32 oc1 = fm_clipTestPoint(&mBmin.x, &mBmax.x, &o1.x );
    HeU32 oc2 = fm_clipTestPoint(&mBmin.x, &mBmax.x, &o2.x );

    HeU32 acode = oc1&oc2;

    if ( acode == 0 )
    {
      if ( mActive )
      {
        if ( mWeighted )
        {
          mInstance.mEdges[edgeIndex].setPathEdgeFlag(PEF_ACTIVE);
          mInstance.mNodeCosts[index1] = mPathingCost;
          mInstance.mNodeCosts[index2] = mPathingCost;
        }
        else
        {
          mInstance.mEdges[edgeIndex].clearPathEdgeFlag(PEF_ACTIVE);
          if ( oc1 || oc2 )
          {
            mInstance.mEdges[edgeIndex].setPathEdgeFlag(PEF_INTERSECTS);
          }
        }
      }
      else
      {
        mInstance.mEdges[edgeIndex].clearPathEdgeFlag(PEF_ACTIVE);
      }
    }
  }

  const char * getStatus(void)
  {
    mStatus.clear();

    if ( mInstancePathData )
    {
      HE_ASSERT(!mPathData);
      mInstancePathData->getStatus(mStatus,"INSTANCED_PATHDATA");
    }
    else
    {
      HE_ASSERT(mPathData);
      mPathData->getStatus(mStatus,"BASE_PATHDATA");
    }

    if ( mMergePathData )
    {
      mStatus+=" : REGION UPDATE IN PROGRESS";
    }

    return mStatus.c_str();
  }

  void notifyPathDataChange(PathData *pdata)
  {
    if ( pdata == mPathData && mInstancePathData == 0 )
    {
      PreserveSendTextMessage stm(mSendTextMessage);
      SEND_TEXT_MESSAGE(0,"Notified that the base path data changed.  Cancelling outstanding PathSolutions.\r\n");
      rebuildEdges();
    }
  }


  bool             getRandomPointInVolume(HeU32 dataId,const HeBounds3 &_b,HeVec3 &point)
  {
    bool ret = false;

    PATHSYSTEM::PathProperties *properties = 0;
    if ( gParent )
    {
      properties = gParent->getPathProperties(dataId);
    }

    HeBounds3 b = _b;
    b.min*=gGameScale;
    b.max*=gGameScale;

    ret = getPathData()->getRandomPointInVolume(b,point,properties);

    if ( ret )
    {
      point*=gInverseGameScale;
    }

    return ret;
  }

  bool             getRandomPointInSphere(HeU32 dataId,const HeVec3 &_center,HeF32 radius,HeVec3 &point)
  {
    bool ret = false;

    PATHSYSTEM::PathProperties *properties = 0;
    if ( gParent )
    {
      properties = gParent->getPathProperties(dataId);
    }

    HeVec3 center = _center*gGameScale;
    radius*=gGameScale;

    ret = getPathData()->getRandomPointInSphere(center,radius,point,properties);

    if ( ret )
    {
      point*=gInverseGameScale;
    }

    return ret;
  }

  bool             getNearestPoint(HeU32 dataId,const HeF32 *point,HeF32 *result,bool validate)
  {
    bool ret = false;

    HE_ASSERT( result );
    HE_ASSERT( point );

    PATHSYSTEM::PathProperties *properties = 0;
    if ( gParent )
    {
      properties = gParent->getPathProperties(dataId);
    }

    if ( result && point )
    {
      result[0] = point[0]*gGameScale;
      result[1] = point[1]*gGameScale;
      result[2] = point[2]*gGameScale;

      ret = getPathData()->getNearestPoint(result,&mInstance,properties,validate);

      result[0]*=gInverseGameScale;
      result[1]*=gInverseGameScale;
      result[2]*=gInverseGameScale;

      if ( ret )
      {
        //SEND_TEXT_MESSAGE(0,"Got Nearest Point(%0.4f,%0.4f,%0.4f)\r\n", result[0], result[1], result[2] );
      }
      else
      {
        //SEND_TEXT_MESSAGE(0,"Failed to get nearest point.\r\n");
      }
    }

    return ret;
  }

  PATHSYSTEM::PathNode *  getNearestPointConnections(HeU32 dataId,const HeF32 *point,HeU32 &pcount)
  {
    PATHSYSTEM::PathNode *ret = 0;

    HE_ASSERT( point );

    HeF32 result[3];

    if ( point )
    {
      result[0] = point[0]*gGameScale;
      result[1] = point[1]*gGameScale;
      result[2] = point[2]*gGameScale;

      PATHSYSTEM::PathProperties *properties = 0;
      if ( gParent )
        properties = gParent->getPathProperties(dataId);

      ret = getPathData()->getNearestPointConnections(result,&mInstance,properties,pcount);

    }

    return ret;
  }

  bool             getRandomPoint(HeU32 dataId,HeF32 *fv)
  {
    bool ret = false;

    PATHSYSTEM::PathProperties *properties = 0;
    if ( gParent )
    {
      properties = gParent->getPathProperties(dataId);
    }

    ret = getPathData()->getRandomPoint(fv,properties);
    if ( ret )
    {
      fv[0]*=gInverseGameScale;
      fv[1]*=gInverseGameScale;
      fv[2]*=gInverseGameScale;
    }

    if ( ret )
    {
      //SEND_TEXT_MESSAGE(0,"GotRandomPoint(%0.4f,%0.4f,%0.4f)\r\n", fv[0], fv[1], fv[2] );
    }
    else
    {
      //SEND_TEXT_MESSAGE(0,"Failed to get a random point!\r\n");
    }

    return ret;
  }


  PATHSYSTEM::PathNode * getNodesInRange(HeU32 dataId,const HeF32 * _point,HeF32 _radius,HeU32 &count)
  {
    PATHSYSTEM::PathNode *ret = 0;

    HeF32 point[3] = { _point[0]*gGameScale, _point[1]*gGameScale, _point[2]*gGameScale };
    HeF32 radius   = _radius*gGameScale;

    PATHSYSTEM::PathProperties *props=0;
    if ( gParent )
      props = gParent->getPathProperties(dataId);

    ret = getPathData()->getNodesInRange(point,radius,count,props);

    if ( ret )
    {
      HeF32 border = 0.1f*gInverseGameScale;

      PATHSYSTEM::PathNode *scan = ret;
      for (HeU32 i=0; i<count; i++)
      {
        scan->mCenter[0]*=gInverseGameScale;
        scan->mCenter[1]*=gInverseGameScale;
        scan->mCenter[2]*=gInverseGameScale;

        scan->mBmin[0]*=gInverseGameScale;
        scan->mBmin[1] = scan->mCenter[1]-border;
        scan->mBmin[2]*=gInverseGameScale;

        scan->mBmax[0]*=gInverseGameScale;
        scan->mBmax[1] = scan->mCenter[1]+border;
        scan->mBmax[2]*=gInverseGameScale;

        scan++;

      }
    }

    return ret;
  }

  HeI32 getSolverPendingCount(void) const
  {
    return (HeI32)mPathSolutions.size();
  }

  bool getRandomPointInNode(HeU32 dataId,HeU64 pathFindingNode,HeVec3 &point,bool regionNode)
  {
    bool ret = false;

    HE_ASSERT(mScene);
    if ( mScene )
    {
      HB_PHYSICS::HBACTORGUID actor = gHBPhysics->locateActor(mScene,pathFindingNode);
      if ( actor )
      {
        NxActor *act = gHBPhysics->getNxActor(actor);
        // cool! we have the actual NxActor here!
        if ( act )
        {
          HeU32 scount = act->getNbShapes();
          if ( scount == 1 )
          {
            NxShape *const *slist = act->getShapes();
            NxShape *shape = slist[0];
            if ( shape->getType() == NX_SHAPE_BOX )
            {
              NxBounds3 b;
              shape->getWorldBounds(b);
              PATHSYSTEM::PathProperties *props=0;
              if ( gParent )
                props = gParent->getPathProperties(dataId);

              AIPathFind *pfind = getPathData()->getPathFind();

              if ( pfind )
              {
                HeBounds3 bounds;
                bounds.min.set( &b.min.x );
                bounds.max.set( &b.max.x );
                HeVec3 center;
                bounds.getCenter(center);
                HeF32 radius = bounds.getDiagonalDistance()*0.5f;
                HeU32 count;
                PATHSYSTEM::PathNode *nodes = pfind->getNodesInRange(&center.x,radius,count,props,&bounds);
                if ( nodes )
                {
                  if ( shape->getGroup() == CG_PATHFINDING_NODE && !regionNode )
                  {
                    NxMat34 pose = shape->getGlobalPose();
                    NxBoxShape *box = shape->isBox();
                    NxVec3 sides = box->getDimensions();
                    NxBounds3 b;
                    b.min.set(-sides.x,-sides.y,-sides.z);
                    b.max = sides;

//                    HeF32 s = gBaseRenderDebug->getRenderScale();
//                    gBaseRenderDebug->setRenderScale(0.1f);

                    HeU32 index = 0;
                    for (HeU32 i=0; i<count; i++)
                    {
                      NxVec3 p( nodes[i].mCenter );
                      NxVec3 t;
                      pose.multiplyByInverseRT(p,t);
                      if ( b.contain(t) )
                      {
//                        gBaseRenderDebug->DebugBound(nodes[i].mBmin,nodes[i].mBmax,0xFFFF00,30.0f);
                        nodes[index] = nodes[i];
                        index++;
                      }
                    }
                    if ( index )
                    {
                      index = rand()%index;
                      point.set( nodes[index].mCenter );
                      point*=gInverseGameScale;
                      ret = true;
                    }
//                    gBaseRenderDebug->setRenderScale(s);
                  }
                  else if ( shape->getGroup() == CG_REGION_NODE && regionNode )
                  {
                    NxActor &a = shape->getActor();

                    const char *name = a.getName();
                    if ( name )
                    {
                      AsciiPath *ap = createAsciiPath();

                      HeU32 pcount;
                      const char **kv = getKeyValues(name,pcount);
                      for (HeU32 i=0; i<pcount; i++)
                      {
                        const char *key   = kv[0];
                        const char *value = kv[1];
                        if ( strcmp(key,"region_node") == 0 )
                        {
                          ap->setData(value);
                        }
                        kv+=2;
                      }

                      pcount = ap->getPointCount();

                      if ( pcount )
                      {
                        NxArray< NxVec3 > polygon;
                        NxVec3 p(0,0,0);
                        for (HeU32 i=0; i<pcount; i++)
                        {
                          ap->getPoint(i,p.x,p.z);
                          p*=gGameScale;
                          polygon.push_back(p);
                        }

                        NxMat34 pose = shape->getGlobalPose();
                        NxBoxShape *box = shape->isBox();
                        NxVec3 sides = box->getDimensions();
                        NxBounds3 b;
                        b.min.set(-sides.x,-sides.y,-sides.z);
                        b.max = sides;

//                        HeF32 s = gBaseRenderDebug->getRenderScale();
//                        gBaseRenderDebug->setRenderScale(0.1f);

                        HeU32 index = 0;
                        for (HeU32 i=0; i<count; i++)
                        {
                          NxVec3 p( nodes[i].mCenter );
                          NxVec3 t;
                          pose.multiplyByInverseRT(p,t);
                          if ( b.contain(t) && fm_pointInsidePolygon2d(pcount,&polygon[0].x,sizeof(NxVec3),&t.x,0,2) )
                          {
//                            gBaseRenderDebug->DebugBound(nodes[i].mBmin,nodes[i].mBmax,0xFFFF00,30.0f);
                            nodes[index] = nodes[i];
                            index++;
                          }
                        }
                        if ( index )
                        {
                          index = rand()%index;
                          point.set( nodes[index].mCenter );
                          point*=gInverseGameScale;
                          ret = true;
                        }
//                        gBaseRenderDebug->setRenderScale(s);
                      }
                      releaseAsciiPath(ap);
                    }
                  }
                }
              }
              else
              {
                SEND_TEXT_MESSAGE(0,"PathSystem::getRandomWalkPointInNode->No pathing data available.\r\n");
              }
            }
            else
            {
              SEND_TEXT_MESSAGE(0,"PathSystem::getRandomWalkPointInNode->Unexpected shape type (%d) expecting NX_BOX_SHAPE\r\n", shape->getType() );
            }
          }
          else
          {
            SEND_TEXT_MESSAGE(0,"PathSystem::getRandomWalkPointInNode->Unexpected number of shapes %d for actor %lld\r\n", scount, pathFindingNode );
          }
        }
        else
        {
          SEND_TEXT_MESSAGE(0,"PathSystem::getRandomWalkPointInNode->Failed to resolve the NxActor pointer for actor %lld\r\n", pathFindingNode );
        }
      }
      else
      {
        SEND_TEXT_MESSAGE(0,"PathSystem::getRandomWalkPointInNode->Failed to locate actor %lld in the scene?\r\n", pathFindingNode );
      }
    }
    else
    {
      SEND_TEXT_MESSAGE(0,"PathSystem::getRandomWalkPointInNode->No valid scene for path system?\r\n", mScene );
    }

    return ret;
  }


  const HeU64 *    getNodesContainingPoint(const HeVec3 &point,HeU32 &count,bool regionNode)
  {
    const HeU64 *ret = 0;
    count = 0;

    mNodeIds.clear();

    HE_ASSERT(mScene);
    if ( mScene )
    {
      NxSphere sphere;
      sphere.center.set(&point.x);
      sphere.center*=gGameScale;
      sphere.radius = 0.01f;

      unsigned int flags = regionNode ? CGF_REGION_NODE : CGF_PATHFINDING_NODE;

      #define MAX_SHAPES 256
      NxShape *slist[MAX_SHAPES];
      NxU32 scount = mScene->overlapSphereShapes(sphere,NX_STATIC_SHAPES,MAX_SHAPES,slist,0,flags,0,0);
      if ( scount )
      {
        for (HeU32 i=0; i<scount; i++)
        {
          NxShape *shape =slist[i];
          NxActor &a = shape->getActor();
          NxMat34 pose = shape->getGlobalPose();
          NxBoxShape *box = shape->isBox();
          NxVec3 sides = box->getDimensions();
          NxBounds3 b;
          b.min.set(-sides.x,-sides.y,-sides.z);
          b.max = sides;
          NxVec3 t;
          pose.multiplyByInverseRT(sphere.center,t);
          if ( b.contain(t) )
          {
            bool ok = true;
            if ( regionNode )
            {
              ok = false;
              const char *name = a.getName();
              if ( name )
              {
                AsciiPath *ap = createAsciiPath();
                HeU32 pcount;
                const char **kv = getKeyValues(name,pcount);
                for (HeU32 i=0; i<pcount; i++)
                {
                  const char *key   = kv[0];
                  const char *value = kv[1];
                  if ( strcmp(key,"region_node") == 0 )
                  {
                    ap->setData(value);
                  }
                  kv+=2;
                }

                pcount = ap->getPointCount();

                if ( pcount )
                {
                  NxArray< NxVec3 > polygon;
                  NxVec3 p(0,0,0);
                  for (HeU32 i=0; i<pcount; i++)
                  {
                    ap->getPoint(i,p.x,p.z);
                    p*=gGameScale;
                    polygon.push_back(p);
                  }
                  if ( fm_pointInsidePolygon2d(pcount,&polygon[0].x,sizeof(NxVec3),&t.x,0,2) )
                  {
                    ok = true;
                  }
                }
                releaseAsciiPath(ap);
              }
            }
            if ( ok )
            {
              HeU64 aguid;
              HB_PHYSICS::HBACTORGUID guid = gHBPhysics->getActorGuids(&a,aguid);
              if ( guid )
              {
                mNodeIds.push_back(aguid);
              }
            }
          }
        }
        if ( !mNodeIds.empty() )
        {
          count = mNodeIds.size();
          ret = &mNodeIds[0];
        }
      }
    }

    return ret;
  }

  StringRef                                    mFQN;
  PATHSYSTEM::PATHGUID                       mRememberGuid;
  PHYSICSASSET::SceneCopy                     *mSceneCopy;
  NxScene                                     *mRegionUpdateScene;
  HEROWORLD::HeroWorldMessageInterface        *mMessageInterface;
  void                                        *mUserData;
  HeU64                                        mUserId1;
  HeU64                                        mUserId2;
  PATHSYSTEM::PATHSYSTEMGUID                 mGuid;
  PathData                                    *mPathData;
  PathData                                    *mInstancePathData;
  NxScene                                     *mScene;
  PathSolverMap                                mPathSolutions;
  NxArray< HeU64 >                             mNodeIds;

  bool                                         mErasePathData:1;
  bool                                         mShowBounds:1;
  bool                                         mShowSolid:1;
  bool                                         mShowConnections:1;
  bool                                         mShowPathCacheValid:1;
  bool                                         mShowPathCacheInvalid:1;
  bool                                         mActive:1;
  bool                                         mWeighted:1;

  HeU32                                        mPathFindCount;     // tracks version number of pathing data.
  PathInstance                                 mInstance;
  NxMat33                                      mInverse;
  NxBox                                        mOBB;
  NxVec3                                       mBmin;
  NxVec3                                       mBmax;
  HeF32                                        mPathingCost;
  NxBounds3                                    mWorldBounds;
  SimpleVector< HeU32 >                        mEdgeTesting;
  HeU32                                        mRebuildEdgeCount;
  HeI32                                        mRebuildCount;
  AIPathMaker                                 *mPathMaker;
  MergePathData                               *mMergePathData;
  std::string                                  mStatus;
  SendTextMessage                             *mSendTextMessage;
  HeF32                                        mTime;
  PathPlanning                                *mPathPlanning;
};

void PathSolver::processPathSolution(PATHSYSTEM::PATHSYSTEMGUID parentGuid,HeU32 ncount,PATHSYSTEM::PathNode *result,PathInstance *pinstance,HeF32 globalTime,const PATHSYSTEM::PathBuildProps &props)
{

  //HeStatsProbe _p("processPathSolution", "PathSolver->pump");

  // Perform string pulling
  USER_STL::vector< PATHSYSTEM::PathNode > nodes;
  USER_STL::vector< PATHSYSTEM::PathNode > temp;

  for (HeU32 i=0; i<ncount; i++)
  {
    gHBPhysics->getGrounding(mScene,mDesc.mFrom,2.0f,props.mCharacterWalkSlope,mDesc.mFrom[1],0,props.mCharacterWidth,props.mCharacterHeight,false,false,CGF_PATH_COLLISION);
  }

  bool stringPulling=true;
  if ( mProperties )
  {
    stringPulling = mProperties->mStringPulling;
  }

  if ( ncount >= 3 && stringPulling )
  {
    if ( pinstance )
    {
      ncount = pinstance->mStringPull->stringPull(mScene,ncount,result,mProperties->mInternalGuid,globalTime,props);
    }
  }

  PATHSYSTEM::PathNode *scan = result;

  HeF32 border = 0.1f*gInverseGameScale;

  for (HeU32 i=0; i<ncount; i++)
  {
    scan->mCenter[0]*=gInverseGameScale;
    scan->mCenter[1]*=gInverseGameScale;
    scan->mCenter[2]*=gInverseGameScale;

    scan->mBmin[0]*=gInverseGameScale;
    scan->mBmin[1] = scan->mCenter[1]-border;
    scan->mBmin[2]*=gInverseGameScale;

    scan->mBmax[0]*=gInverseGameScale;
    scan->mBmax[1] = scan->mCenter[1]+border;
    scan->mBmax[2]*=gInverseGameScale;

    scan++;
  }

  {
    //HeStatsProbe _p("receivePathSolution","PathSolver->pump");
    HE_ASSERT(mDesc.mCallback);
    if ( mDesc.mCallback )
    {
      mDesc.mCallback->receivePathSolution(parentGuid,mGuid,mDesc.mUserData,mDesc.mUserId1,mDesc.mUserId2,ncount,result,mTotalSolverCount,mTime);  // send the results back to the user.
      mDesc.mCallback = 0;
    }
  }

}



class MyPathSystem_AIWISDOM : public PathSystem_AIWISDOM, public ComLayer
{
public:
  MyPathSystem_AIWISDOM(void)
  {
    setDescription("PathSystem_AIWISDOM");
    mGuid = 0;
    mPathGuid = 0;
    mSolverBudget = 4000.0f;
    mShowDistance = 20.0f;
    mEyePos[0] = 0;
    mEyePos[1] = 0;
    mEyePos[2] = 0;
    mFlags = 0;

    gMinNodeSize =  3;
    gMaxNodeSize =  10;

//    gMinNodeSize =  10;
//    gMaxNodeSize =  15;


    mDtime = 0;
    AddToken("pc_snap_to_ground", PC_SNAP_TO_GROUND);
    AddToken("pc_string_pulling", PC_STRING_PULLING);
    AddToken("pc_smooth_path", PC_SMOOTH_PATH );
  }

  ~MyPathSystem_AIWISDOM(void)
  {
  }

  void test(void)
  {
  }

  bool shutdown(void)
  {
    return doShutdown();
  }


	HeI32 CommandCallback(HeI32 token,HeI32 count,const char **arglist)
  {
    HeI32 ret = 0;

    ComLayer::CommandCallback(token,count,arglist);

    switch ( token )
    {
      case PC_HELP:
        mProcessed = false;
        SEND_TEXT_MESSAGE(mClient,"PathSystem_AIWISDOM Plugin Help:\r\n");
        SEND_TEXT_MESSAGE(mClient,"==============================\r\n");
        SEND_TEXT_MESSAGE(mClient," hello         : Send a hello world acknowledgement.\r\n");
        break;
      case PC_HELLO_WORLD:
        mProcessed = true;
        SEND_TEXT_MESSAGE(mClient,"Hello dude, you got to the Path Planning plugin!\r\n");
        break;
    }

    return ret;
  }

  HeU32 getHeapSize(HeU32 &unused)
  {
    HeU32 ret = ComLayer::getHeapSize(unused);
    return ret;
  }

  StringRef getName(const char *fqn)
  {
    char scratch[512];
    strcpy(scratch,fqn);
    char *dot = stristr(scratch,".");
    if ( dot )
    {
      *dot = 0;
      strcat(scratch,".ai");
    }
    strlwr(scratch);
    StringRef name(scratch);
    return name;
  }

  PathData * locatePathData(const char *fqn,bool loadOk)
  {
    PathData   * ret = 0;

    StringRef name = getName(fqn);

    PathDataMap::iterator found;
    found = mPaths.find(name);
    if ( found == mPaths.end() )
    {
      ret = MEMALLOC_NEW(PathData)(name,loadOk);
      mPaths[name] = ret;
    }
    else
    {
      ret = (*found).second;
    }

    return ret;
  }

  PATHSYSTEM::PATHSYSTEMGUID  createPathSystem(const char *fqn,NxScene *scene,PathPlanning *pp)
  {
    PATHSYSTEM::PATHSYSTEMGUID ret = 0;

    StringRef ref = getName(fqn);

    PathData *pd = locatePathData(fqn,true);

    if ( pd )
    {
      mGuid++;
      PathSystem *ps = MEMALLOC_NEW(PathSystem)(pd,scene,mGuid,pp,ref);
      mPathSystems[mGuid] = ps;
      ret = mGuid;
    }

    return ret;
  }

  PathSystem *get(PATHSYSTEM::PATHSYSTEMGUID guid)
  {
    PathSystem *ret = 0;

    PathSystemMap::iterator found = mPathSystems.find(guid);
    if ( found != mPathSystems.end() )
    {
      ret = (*found).second;
    }
    return ret;
  }


  bool             releasePathSystem(PATHSYSTEM::PATHSYSTEMGUID guid)
  {
    bool ret = false;

    PathSystem *ps = get(guid);

    if ( ps )
    {
      delete ps;
      mPathSystems.erase(guid);
      ret = true;
    }

    return ret;
  }

  PATHSYSTEM::PATHGUID         solvePath(PATHSYSTEM::PATHSYSTEMGUID guid,const PATHSYSTEM::PathRequestDesc &desc)
  {
    PATHSYSTEM::PATHGUID ret = 0;


    PathSystem *ps = get(guid);
    if ( ps )
    {
      mPathGuid++;
      ret = mPathGuid;
      ps->solvePath(ret,desc);
    }

    return ret;
  }

  bool             cancelPath(PATHSYSTEM::PATHSYSTEMGUID guid,PATHSYSTEM::PATHGUID path)
  {
    bool ret = false;


    PathSystem *ps = get(guid);

    if ( ps )
    {
      ret = ps->cancelPath(path);
    }


    return ret;
  }

  HeU32     cancelPaths(PATHSYSTEM::PATHSYSTEMGUID guid,PATHSYSTEM::PathNodeInterface *callback)
  {
    HeU32 ret = 0;

    PathSystem *ps = get(guid);

    if ( ps )
    {
      ret = ps->cancelPaths(callback);
    }


    return ret;
  }


  bool             setPathState(PATHSYSTEM::PATHSYSTEMGUID guid,PATHSYSTEM::PathState state,bool bstate,HeU32 v,const HeF32 *fv,PATHSYSTEM::PathStateDesc *desc,HeU64 bigv)
  {
    bool ret = false;

    PathSystem *ps = get(guid);

    if ( state == PATHSYSTEM::PS_STEP_SOLUTION )
    {
      stepSolution();
    }

    if ( ps )
    {
      if ( state == PATHSYSTEM::PS_REBUILD_OF_INSTANCE_GRAPH )
      {
        ps->forceRebuildInstance(mEyePos,mShowDistance);
      }
      else
      {
        ret = ps->setPathState(state,bstate,v,fv,desc,bigv);
      }
    }

    switch ( state )
    {
      case PATHSYSTEM::PS_WEIGHT_DIR:
        gWeightDir = bstate;
        break;
      case PATHSYSTEM::PS_SET_PATH_BUILD_TIME:
        if ( v >= 1 && v <= 16 )
        {
          gMaxBuildTime = v;
        }
        else
        {
          SEND_TEXT_MESSAGE(0,"Invalid path build time %d valid range is 1 to 16 milliseconds.\r\n", v );
        }
        break;
      case PATHSYSTEM::PS_SET_PATH_SOLVE_TIME:
        if ( v >= 1 && v <= 16 )
        {
          gMaxSolveTime = v;
        }
        else
        {
          SEND_TEXT_MESSAGE(0,"Invalid path solve time %d, valid range is 1 to 16 milliseconds.\r\n", v );
        }
        break;
      case PATHSYSTEM::PS_MIN_NODE_SIZE:
        gMinNodeSize = (HeU32 )v;
        break;
      case PATHSYSTEM::PS_MAX_NODE_SIZE:
        if ( fv )
        {
          gMaxNodeSize = (HeU32 )fv[0];
        }
        else
        {
          gMaxNodeSize = (HeU32 )v;
        }
        break;
      case PATHSYSTEM::PS_SET_GAME_SCALE:
        gGameScale = fv[0];
        gInverseGameScale = 1.0f / gGameScale;
        break;
      case PATHSYSTEM::PS_SET_RENDER_SCALE:
        gRenderScale = fv[0];
        break;
      case PATHSYSTEM::PS_SET_SOLVER_BUDGET:
        mSolverBudget = (HeF32) v;
        SEND_TEXT_MESSAGE(0,"PathSystem solver budget set to %d iterations.\r\n", v );
        break;
      case PATHSYSTEM::PS_SET_EYE_POS:
        if ( fv )
        {
          mEyePos[0] = fv[0]*gGameScale;
          mEyePos[1] = fv[1]*gGameScale;
          mEyePos[2] = fv[2]*gGameScale;
        }
        break;
      case PATHSYSTEM::PS_SET_SHOW_DISTANCE:
        if ( fv )
        {
          mShowDistance = *fv;
        }
        break;
    }

    return ret;
  }

  bool             getPathState(PATHSYSTEM::PATHSYSTEMGUID guid,PATHSYSTEM::PathState state,bool *bstate,HeU32 *v,HeF32 *fv,PATHSYSTEM::PathStateDesc *desc,HeU64 *bigv)
  {
    bool ret = false;

    if ( guid )
    {
      PathSystem *ps = get(guid);
      if ( ps )
      {
        ret = ps->getPathState(state,bstate,v,fv,desc,bigv);
      }
    }
    else if ( state == PATHSYSTEM::PS_GET_BUILD_VERSION )
    {
      HE_ASSERT(v);
      if ( v )
      {
        v[0] = AIPATH_VERSION_NUMBER;
        ret = true;
      }
    }
    else if ( state == PATHSYSTEM::PS_GET_ACTIVE_REGION_COUNT )
    {
      HE_ASSERT(v);
      if ( v )
      {
        v[0] = gActiveRegionCount;
        ret = true;
      }
    }

    return ret;
  }

  void setBaseRenderDebug(RENDER_DEBUG::BaseRenderDebug *r)
  {
    gBaseRenderDebug = r;
  }

  void setResourceInterface(RESOURCE_INTERFACE::ResourceInterface *iface)
  {
    gResourceInterface = iface;
  }

  HeU32     render(PATHSYSTEM::PATHSYSTEMGUID guid,const HeF32 *_eyePos,HeF32 showDistance,
    void *userData,
    HEROWORLD::HeroWorldMessageInterface *iface,
    HeU64 userId1,
    HeU64 userId2)

  {
    HeU32 ret = 0;


    PathSystem *ps = get(guid);
    if ( ps )
    {
      HeF32 eyePos[3];
      eyePos[0] = _eyePos[0]*gGameScale;
      eyePos[1] = _eyePos[1]*gGameScale;
      eyePos[2] = _eyePos[2]*gGameScale;
      ret = ps->render(eyePos,showDistance,userData,iface,userId1,userId2);
    }

    return ret;
  }

  void pumpFrame(HeF32 dtime)
  {
#if 0
    gMaxBuildTime = 64;
    gMaxSolveTime = 64;
#endif
    mDtime+=dtime;

    if ( mDtime >= (1.0f/60.0f ) )
    {
      gFrameCount++;

      dtime = mDtime;
      mDtime = 0;

      static int tickCount = 0;

      tickCount++;

      if ( tickCount == 60 )
      {
        tickCount = 0;
        HeI32 solverPending = 0;
        PathSystemMap::iterator k = mPathSystems.begin();
        while ( k != mPathSystems.end() )
        {
          PathSystem *ps = (*k).second;
          solverPending+=ps->getSolverPendingCount();
          k++;
        }

        static HeI32 lastSolverPending = 0;
        if ( solverPending != lastSolverPending )
        {
          lastSolverPending = solverPending;
//          SEND_TEXT_MESSAGE(0,"SolverPending(%d)\r\n", solverPending );
        }
      }


      HeU32 stime = timeGetTime();

      if ( !mPathSystems.empty() )
      {
        HeStatsProbe _p("PathSolving","HeroWorld->pump");
        static bool _state = true;

        while ( _state )
        {
          HeU32 work_count = 0;

          PathSystemMap::iterator k = mPathSystems.begin();
          while ( k != mPathSystems.end() )
          {
            PathSystem *ps = (*k).second;

            bool haveWork;
            bool ok = ps->pump(dtime,stime,haveWork);
            if ( !ok )
            {
              work_count = 0;       // used up too much time!
              break;
            }
            else if ( haveWork )
            {
              work_count++;
            }
            k++;
          }

          dtime = 0;

          if ( work_count == 0  ) // if no work to do, exit
          {
            break;
          }

        }

      }

      {

        HeU32 build_count;

        do
        {
          build_count = 0;

          bool exit_time = false;

          {
            HeStatsProbe _p("PathBuilding","HeroWorld->pump");
            PathDataMap::iterator i;
            for (i=mPaths.begin(); i!=mPaths.end(); ++i)
            {
              PathData *pd = (*i).second;
              if ( pd->process() )
              {
                build_count++;
              }
              HeU32 etime = timeGetTime();
              HeU32 diff = etime - stime;
              if ( diff > (gMaxSolveTime+gMaxBuildTime) )
              {
                build_count = 0;
                exit_time = true;
                break;
              }
            }
          }

          if (!exit_time)
          {
            HeStatsProbe _p("PathEditBuilding","HeroWorld->pump");
            PathSystemMap::iterator k = mPathSystems.begin();
            while ( k!= mPathSystems.end() )
            {
              PathSystem *ps = (*k).second;

              if ( ps->pumpBuild() )
              {
                build_count++;
              }
              k++;

              HeU32 etime = timeGetTime();
              HeU32 diff = etime - stime;
              if ( diff > (gMaxSolveTime+gMaxBuildTime) )
              {
                build_count = 0;
                break;
              }
            }
          }
        } while ( build_count != 0 );

      }
    }
  }

  void setHBPhysics(HB_PHYSICS::HBPhysics *hb)
  {
    gHBPhysics = hb;
  }

  bool             rebuildPathSystem(PATHSYSTEM::PATHSYSTEMGUID guid,const HeF32 *_center,HeF32 radius)
  {
    bool ret = false;

    PathSystem *ps = get(guid);

    if ( ps )
    {
      HeF32 center[3] = { 0, 0, 0 };
      if ( _center )
      {
        center[0] = _center[0]*gGameScale;
        center[1] = _center[1]*gGameScale;
        center[2] = _center[2]*gGameScale;
      }
      ret = ps->forceRebuild(center,radius);
    }

    return ret;
  }

  bool isRebuildComplete(PATHSYSTEM::PATHSYSTEMGUID guid)
  {
    bool ret = false;

    PathSystem *ps = get(guid);
    if ( ps )
    {
      ret = ps->rebuildComplete();
    }

    return ret;
  }

  const char * getStatus(PATHSYSTEM::PATHSYSTEMGUID guid)
  {
    const char *ret = 0;

    PathSystem *ps = get(guid);
    if ( ps )
    {
      ret = ps->getStatus();
    }
    return ret;
  }

  bool             setPathSystemSendTextMessage(PATHSYSTEM::PATHSYSTEMGUID guid,SendTextMessage *stm) // set the 'sendTextMessage' interface relative to a specific PATHSYSTEM
  {
    bool ret = false;

    PathSystem *ps = get(guid);
    if ( ps )
    {
      ret = ps->setPathSystemSendTextMessage(stm);
    }


    return ret;
  }

  bool             rebuildCancel(PATHSYSTEM::PATHSYSTEMGUID guid)
  {
    bool ret = false;

    PathSystem *ps = get(guid);
    if ( ps )
    {
      ret = ps->rebuildCancel();
    }

    return ret;
  }

  HeU32 executeCommand(SendTextMessage *tmessage,const char *cmd)
  {
    HeU32 ret = 0;

    SendTextMessage *save = gSendTextMessage;

    if ( tmessage ) gSendTextMessage = tmessage;

    mClient = 0;
    ret = (HeU32) CPARSER.Parse("%s",cmd);

    gSendTextMessage = save;

    return ret;
  }

  void frameBegin(SendTextMessage *stm)
  {
    MEMALLOC_FRAME_BEGIN(stm);
  }

  void frameEnd(SendTextMessage *stm,const char *header)
  {
    MEMALLOC_FRAME_END(stm,header);
  }


  bool      releasePathData(const char *fqn) // release the path data from memory, so long as there are no references to it.
  {
    bool ret = false;


    StringRef name = getName(fqn);

    PathDataMap::iterator found;
    found = mPaths.find(name);
    if ( found != mPaths.end() )
    {
      PathData *p = (*found).second;
      HeI32 ref = p->getReferenceCount();
      HE_ASSERT(ref == 0 );
      if ( ref == 0 )
      {
        delete p;
        mPaths.erase(found);
      }
    }

    return ret;
  }

  PATHSYSTEM::PATHDATAGUID     getPathData(const char *fqn,bool loadOk)
  {
    PATHSYSTEM::PATHDATAGUID ret = 0;

    StringRef name = getName(fqn);

    ret = locatePathData(name.Get(),loadOk);

    return ret;
  }

  bool             getPathDataVersion(PATHSYSTEM::PATHDATAGUID guid,HeU32 &build_version,HeU32 &asset_version,HeU64 &timeStamp)
  {
    bool ret = false;

    PathData *pd = (PathData *)guid;
    if ( pd )
    {
      ret = pd->getPathDataVersion(build_version,asset_version,timeStamp);
    }

    return ret;
  }


  bool             rebuildPathData(PATHSYSTEM::PATHDATAGUID guid,NxScene *scene,const HeF32 *eyePos,HeF32 radius,PathPlanning *pp)
  {
    bool ret = false;

    PathData *pd = (PathData *)guid;

    if ( pd )
    {
//      HE_ASSERT(pp);
      const PATHSYSTEM::PathBuildProperties *properties=0;
      if ( pp ) 
        properties = pp->getProperties();

      ret = pd->rebuildPathData(scene,eyePos,radius,properties);
    }


    return ret;
  }

  const char *     getStatus(PATHSYSTEM::PATHDATAGUID guid)
  {
    const char *ret = 0;

    PathData *pd = (PathData *)guid;
    if ( pd )
    {
      mStatusString.clear();
      pd->getStatus(mStatusString,"status");
      ret = mStatusString.c_str();
    }

    return ret;
  }

  bool             savePathData(PATHSYSTEM::PATHDATAGUID guid,HeU32 asset_version,HeU64 timeStamp)
  {
    bool ret = false;

    PathData *pd = (PathData *)guid;
    if ( pd )
    {
      ret = pd->savePathData(asset_version,timeStamp);
    }

    return ret;
  }

  bool             releasePathData(PATHSYSTEM::PATHDATAGUID guid)
  {
    bool ret = false;


    PathData *pd = (PathData *)guid;
    if ( pd )
    {
      PathDataMap::iterator found;
      found = mPaths.find( pd->mName );
      if ( found != mPaths.end() )
      {
        HE_ASSERT( pd == (*found).second );
        delete pd;
        mPaths.erase(found);
        ret = true;
      }
      else
      {
        HE_ASSERT(0);
      }
    }

    return ret;
  }

  bool             rebuildCancel(PATHSYSTEM::PATHDATAGUID guid)
  {
    bool ret = false;

    PathData *pd = (PathData *)guid;
    if ( pd )
    {
      ret = pd->rebuildCancel();
    }

    return ret;
  }

  bool             isRebuildComplete(PATHSYSTEM::PATHDATAGUID guid)
  {
    bool ret = false;

    PathData *pd = (PathData *)guid;
    if ( pd )
    {
      ret = pd->isRebuildComplete();
    }

    return ret;
  }

  bool notifyAssetChanged(const char *_fqn)
  {
    bool ret = false;

    if ( _fqn )
    {
      StringRef name = getName(_fqn);

      PathDataMap::iterator i;
      for (i=mPaths.begin(); i!=mPaths.end(); ++i)
      {
        if ( fqnMatch( (*i).first.Get(), name.Get() ) )
        {
          PathData *pdata = (*i).second;
          SEND_TEXT_MESSAGE(0,"Reloading Path Data for '%s'\r\n", name.Get() );
          PathSystemMap::iterator j;
          for (j=mPathSystems.begin(); j!=mPathSystems.end(); j++)
          {
            PathSystem *ps = (*j).second;
            ps->checkReload(pdata);
          }
          break;
        }
      }
    }

    return ret;
  }

  bool             getNearestPoint(PATHSYSTEM::PATHSYSTEMGUID guid,HeU32 dataId,const HeF32 *point,HeF32 *result,bool validate)
  {
    bool ret = false;

    PathSystem *ps = get(guid);

    if ( ps )
    {
      ret = ps->getNearestPoint(dataId,point,result,validate);
    }

    return ret;
  }

  bool             getRandomPoint(PATHSYSTEM::PATHSYSTEMGUID guid,HeU32 dataId,HeF32 *result)
  {
    bool ret = false;

    PathSystem *ps = get(guid);

    if ( ps )
    {
      ret = ps->getRandomPoint(dataId,result);
    }

    return ret;
  }

  PATHSYSTEM::PathNode *       getNodesInRange(PATHSYSTEM::PATHSYSTEMGUID guid,HeU32 dataId,const HeF32 * point,HeF32 radius,HeU32 &count)
  {
    PATHSYSTEM::PathNode * ret = 0;

    count = 0;

    PathSystem *ps = get(guid);

    if ( ps )
    {
      ret = ps->getNodesInRange(dataId,point,radius,count);
    }

    return ret;
  }


  void setHeStats(HeStats *stats)
  {
    gHeStats = stats;
  }

  PATHSYSTEM::PathNode *       getNearestPointConnections(PATHSYSTEM::PATHSYSTEMGUID /*guid*/,const char * /*properties*/,const HeF32 * /*point*/,HeU32 &/*pcount*/)
  {
    PATHSYSTEM::PathNode *ret = 0;

    HE_ASSERT(0); // not implemented

    return ret;
  }

  PATHSYSTEM::PathNode *       getNearestPointConnections(PATHSYSTEM::PATHSYSTEMGUID guid,HeU32 dataId,const HeF32 * point,HeU32 &pcount)
  {
    PATHSYSTEM::PathNode *ret = 0;


    PathSystem *ps = get(guid);

    if ( ps )
    {
      ret = ps->getNearestPointConnections(dataId,point,pcount);
    }


    return ret;
  }



  bool createKeyValueSpecData(PATHSYSTEM::PATHSYSTEMGUID /*guid*/,const char * /*specName*/,HeU32 /*dataId*/,const char * /*spec*/,const char * /*data*/)
  {
    return false;
  }

  void setPathSystem(PATHSYSTEM::PathSystem *ps)
  {
    gParent = ps;
  }

  bool             getRandomPointInNode(PATHSYSTEM::PATHSYSTEMGUID guid,HeU32 dataId,HeU64 pathFindingNode,HeVec3 & point,bool regionNode)
  {
    bool ret = false;

    PathSystem *ps = get(guid);

    if ( ps )
    {
      ret = ps->getRandomPointInNode(dataId,pathFindingNode,point,regionNode);
    }

    return ret;
  }

  const HeU64 *    getNodesContainingPoint(PATHSYSTEM::PATHSYSTEMGUID guid,const HeVec3 &point,HeU32 &count,bool regionNode)
  {
    const HeU64 *ret = 0;
    count = 0;

    PathSystem *ps = get(guid);

    if ( ps )
    {
      ret = ps->getNodesContainingPoint(point,count,regionNode);
    }

    return ret;
  }

  bool             getRandomPointInSphere(PATHSYSTEM::PATHSYSTEMGUID guid,HeU32 dataId,const HeVec3 &center,HeF32 radius,HeVec3 &point)
  {
    bool ret = false;

    PathSystem *ps = get(guid);

    if ( ps )
    {
      ret = ps->getRandomPointInSphere(dataId,center,radius,point);
    }




    return ret;
  }

  bool             getRandomPointInVolume(PATHSYSTEM::PATHSYSTEMGUID guid,HeU32 dataId,const HeBounds3 &b,HeVec3 &point)
  {
    bool ret = false;

    PathSystem *ps = get(guid);

    if ( ps )
    {
      ret = ps->getRandomPointInVolume(dataId,b,point);
    }


    return ret;
  }

  bool releasePathData(PathData *pd)
  {
    bool ret = false;

    PathDataMap::iterator i;
    for (i=mPaths.begin(); i!=mPaths.end(); ++i)
    {
      PathData *_pd = (*i).second;
      if ( _pd == pd )
      {
        delete pd;
        mPaths.erase(i);
        ret = true;
        break;
      }
    }

    HE_ASSERT(ret);

    return ret;
  }

  const char *     getPathDataName(const char *fqn)
  {
    StringRef name = getName(fqn);
    return name.Get();
  }

private:
  HeF32                                             mDtime;
  HeU32                                             mFlags;
  HeF32                                             mEyePos[3];
  HeF32                                             mShowDistance;
  HeF32                                             mSolverBudget;
  PATHSYSTEM::PATHSYSTEMGUID                      mGuid;
  PATHSYSTEM::PATHGUID                            mPathGuid;
  std::string                                       mStatusString;

  PathSystemMap                                     mPathSystems;

  PathDataMap                                       mPaths;


};


static MyPathSystem_AIWISDOM *gInterface=0;

PathSystem::~PathSystem(void)
{
  PathSolverMap::iterator i;
  for (i=mPathSolutions.begin(); i!=mPathSolutions.end(); ++i)
  {
    PathSolver *ps = (*i).second;
    HE_ASSERT( ps->getGuid() == (*i).first );
    delete ps;
  }

  mPathSolutions.clear();

  if ( mPathMaker )
  {
    gActiveRegionCount--;
    releasePathMaker(mPathMaker);
  }

  if ( mSceneCopy )
  {
    PHYSICSASSET::PhysicsAssetPlugin *pa = gHBPhysics->getPhysicsAssetPlugin();
    pa->releaseSceneCopy(mSceneCopy);
    mSceneCopy = 0;
  }

  if ( mInstancePathData )
  {
    delete mInstancePathData;
  }

  delete mMergePathData;

  releasePathData();

  if ( mPathData )
  {
    HeI32 count = mPathData->decrementReferenceCount();
    if ( count == 0 )
    {
      gInterface->releasePathData(mPathData);
    }
  }
}

void PathSystem::releasePathData(void)
{
  if ( mPathData )
  {
    HeI32 count = mPathData->decrementReferenceCount();
    if ( count == 0 )
    {
      gInterface->releasePathData(mPathData);
    }
    mPathData = 0;
  }
}    


};  // End of Namespace


using namespace PATHSYSTEM_AIWISDOM;

#define MEGABYTE (1024*1024)



extern "C"
{
#ifdef PLUGINS_EMBEDDED
PathSystem_AIWISDOM * getInterfacePathSystem_AIWISDOM(HeI32 version_number)
#else
PATHSYSTEM_AIWISDOM_API PathSystem_AIWISDOM * getInterface(HeI32 version_number)
#endif
{
  HE_ASSERT( gInterface == 0 );
  if ( gInterface == 0 && version_number == PATHSYSTEM_AIWISDOM_VERSION )
  {
#if HE_USE_MEMORY_TRACKING
    MEMALLOC::setComponentName("PathSystem_AIWISDOM.dll");
#endif
    gInterface = MEMALLOC_NEW(MyPathSystem_AIWISDOM);
  }


  return static_cast<PathSystem_AIWISDOM *>(gInterface);
};

};  // End of namespace PATHPLANNING

using namespace PATHSYSTEM_AIWISDOM;

#ifndef PLUGINS_EMBEDDED
bool doShutdown(void)
{
  bool ret = false;
  if ( gInterface )
  {
    ret = true;
    delete gInterface;
    gInterface = 0;
  }
  return ret;
}
#endif

using namespace PATHSYSTEM_AIWISDOM;

#ifdef WIN32

#ifndef PLUGINS_EMBEDDED

#include <windows.h>

BOOL APIENTRY DllMain( HANDLE /*hModule*/,
                       DWORD  ul_reason_for_call,
                       LPVOID /*lpReserved*/)
{
  HeI32 ret = 0;

  switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
      ret = 1;
			break;
		case DLL_THREAD_ATTACH:
      ret = 2;
			break;
		case DLL_THREAD_DETACH:
      ret = 3;
			break;
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
#endif

#endif
