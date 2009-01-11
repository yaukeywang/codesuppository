#ifndef AI_PATH_MAKER_H

#define AI_PATH_MAKER_H

#include <string>

class NxScene;

namespace HBPATHSYSTEM
{
class PathBuildProperties;
};

namespace HBPATHSYSTEM_AIWISDOM
{

class AIPathMaker;

AIPathMaker * beginPathMaker(NxScene *scene,const HeF32 *eyePos,HeF32 showDistance,HeU32 minNodeSize,HeU32 maxNodeSize,HeI32 version_number,HeU64 timeStamp,const HBPATHSYSTEM::PathBuildProperties *properties);

AIPathMaker * beginPathMaker(NxScene *scene,const HeF32 *region,HeU32 minNodeSize,HeU32 maxNodeSize,const HBPATHSYSTEM::PathBuildProperties *properties);

bool          processPathMaker(AIPathMaker *aip,bool showbox,bool &connectionPhase,bool echo);

HeU32  doRebuildTree(AIPathMaker *aip);

void          releasePathMaker(AIPathMaker *aip);
void *        doGetPathData(AIPathMaker *aip,HeU32 &len);
bool          isConnections(AIPathMaker *aip);

HeU32  doAddNode(AIPathMaker *pmaker,const HeF32 *center,const HeF32 *bmin,const HeF32 *bmax,HeU32 flags);
HeU32  doAddConnection(AIPathMaker *pmaker,HeU32 c1,HeU32 c2);

const char *  doGetStatus(AIPathMaker *pmaker);

void addStatus(std::string &status,const char *fmt,...);

};

#endif
