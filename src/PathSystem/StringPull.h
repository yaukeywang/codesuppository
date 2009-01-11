#ifndef STRING_PULL_H

#define STRING_PULL_H

#include "common/snippets/UserMemAlloc.h"

#include "HBPathSystem/HBPathSystem.h"

class NxScene;

namespace HBPATHSYSTEM_AIWISDOM
{

class AIPathFind;

enum CanWalkState
{
  CWS_NONE,
  CWS_CAN_WALK,
  CWS_CANNOT_WALK,
  CWS_PATH_FAILURE,
};


class StringPull
{
public:
  virtual HeU32 stringPull(NxScene *scene,HeU32 ncount,HBPATHSYSTEM::PathNode *nodes,HeU32 properties,HeF32 time,const HBPATHSYSTEM::PathBuildProps &props) = 0;
  virtual CanWalkState  canWalk(HeU32 i1,HeU32 i2,HeU32 properties) = 0;
  virtual void addTracker(HeU32 i1,HeU32 i2,CanWalkState state,HeU32 properties,HeF32 time) = 0;
  virtual void bringOutYourDead(HeF32 time) = 0;
  virtual void render(AIPathFind *pfind,const HeF32 *eyePos,HeF32 range) = 0;
};


StringPull * createStringPull(void);
void         releaseStringPull(StringPull *stringPull);

};


#endif
