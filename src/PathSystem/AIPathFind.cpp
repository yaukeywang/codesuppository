#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include <vector>

#include "common/snippets/UserMemAlloc.h"
#include "common/snippets/KdTree.h"
#include "common/snippets/rand.h"
#include "common/snippets/FloatMath.h"
#include "common/FileInterface/FileInterface.h"
#include "RenderDebug/RenderDebug.h"
#include "HbPhysics/HbPhysics.h"
#include "HeroWorld/HeStats.h"
#include "common/snippets/KeyValueSpec.h"
#include "AIPathFind.h"
#include "common/snippets/HeAstar.h"
#include "common/snippets/timelog.h"
#if defined(LINUX)
#include "linux_compat.h"
#endif

#define INFLATION 0.05f

extern float gInverseGameScale;

#define USE_TIME_LOG 0

#if USE_TIME_LOG
static TimeLog *gTimeLog=0;
#endif

namespace HBPATHSYSTEM_AIWISDOM
{


extern bool gWeightDir;

class AINode;

typedef USER_STL::vector< AINode > AINodeVector;
typedef USER_STL::vector< HBPATHSYSTEM::PathNode > AINodeResultVector;

#define NO_EDGE_LIMIT (0xFFFFFFFF-8192)

class AINode : public AI_Node
{
public:
  AINode(void)
  {
    mKey = 0x4A4F484E;

    mBmin[0] = 0;
    mBmin[1] = 0;
    mBmin[2] = 0;

    mBmax[0] = 0;
    mBmax[1] = 0;
    mBmax[2] = 0;

    mCenterY = 0;
    mCount = 0;
    mEdgeIndex = NO_EDGE_LIMIT;

    mFlags = 0;

    mConnections = 0;

  }

  ~AINode(void)
  {
    validate();
    mKey = 0;
  }

  bool validate(void) const
  {
    HE_ASSERT( mKey == 0x4A4F484E );
    return (mKey == 0x4A4F484E) ? true : false;
  }

  void set(const HeBounds3 &b)
  {
    mBmin[0] = b.min.x;
    mBmin[1] = b.min.y;
    mBmin[2] = b.min.z;

    mBmax[0] = b.max.x;
    mBmax[1] = b.max.y;
    mBmax[2] = b.max.z;

    mCenterY = (mBmin[1]+mBmax[1])*0.5f;
  }

  void getBounds(HeBounds3 &b)
  {
    b.min.x = mBmin[0];
    b.min.y = mBmin[1];
    b.min.z = mBmin[2];

    b.max.x = mBmax[0];
    b.max.y = mBmax[1];
    b.max.z = mBmax[2];
  }

  AINode ** read(FILE_INTERFACE *fph,AINode *nodes,AINode **connections,HeU32 edgeIndex,HeU32 version_number,HeF32 longEdge)
  {
    mEdgeIndex = edgeIndex;

    if ( version_number == 107 )
    {
      fi_fread(&mFlags, sizeof(mFlags),1,fph);
    }

    fi_fread(mBmin,sizeof(HeF32)*3,1,fph);
    fi_fread(mBmax,sizeof(HeF32)*3,1,fph);
    mBmax[0]+=longEdge;
    mBmax[2]+=longEdge;
    fi_fread(&mCenterY,sizeof(HeF32),1,fph);

    fi_fread(&mCount,sizeof(HeU32),1,fph);

    if ( connections )
    {
      mConnections = connections;
      for (HeU32 i=0; i<mCount; i++)
      {
        HeU32 c;
        fi_fread(&c,sizeof(HeU32),1,fph);
        AINode *n = &nodes[c];
        *connections++ = n;
      }
    }
    else
    {
      mCount = 0;
      mConnections = 0;
    }

#if USE_TIME_LOG
    if ( gTimeLog == 0 )
    {
      gTimeLog = new TimeLog("AIPathFind");
    }

    gTimeLog->log("EdgeIndex: %d Bmin(%0.9f,%0.9f,%0.9f) Bmax(%0.9f,%0.9f,%0.9f) CenterY(%0.9f) longEdge(%0.9f) count(%d)\r\n",
      edgeIndex,mBmin[0],mBmin[1],mBmin[2],
      mBmax[0],mBmax[1],mBmax[2],
      mCenterY,
      longEdge,
      mCount);
#endif

    return connections;
  }

  void getCenter(HeF32 *c) const
  {
    c[0] = (mBmin[0]+mBmax[0])*0.5f;
    c[1] = (mBmin[1]+mBmax[1])*0.5f; //mCenterY;
    c[2] = (mBmin[2]+mBmax[2])*0.5f;
  }

  HeF32 getRadius(void) const
  {

    HeF32 dx = mBmax[0]-mBmin[0];
    HeF32 dy = mBmax[1]-mBmin[1];
    HeF32 dz = mBmax[2]-mBmin[2];

    HeF32 d  = sqrtf( dx*dx + dy*dy + dz*dz );

    return d*0.5f;
  }

  HeF32 computeCenterY(void) const
  {
    return mCenterY;
  }

  void debugRender(unsigned int color,float duration)
  {
    if ( gBaseRenderDebug )
    {
      gBaseRenderDebug->DebugBound(mBmin,mBmax,color,duration,true,true);
      gBaseRenderDebug->DebugBound(mBmin,mBmax,0xFFFFFF,duration,true,false);
    }
  }

//#pragma optimize("",off)
  void _cdecl render(bool showBound,bool showConnections,bool show_solid,PathInstance *pinst,const AINode *nodes,HeF32 duration=0.0001f) const
  {
    if ( !gBaseRenderDebug )
      return;

    HE_ASSERT(pinst);
    HeF32 cost = 1;

    bool region_update = false;

    if ( pinst->isRegionUpdate() )
    {
      region_update = fm_intersectAABB(mBmin,mBmax, &pinst->mRegion.min.x, &pinst->mRegion.max.x );
    }

    {
      HeU32 index = this-nodes;
      if ( pinst->mNodeCosts[index] == 0 )
      {
        return;
      }
      cost = pinst->mNodeCosts[index];
    }

    HeF32 center[3];

    getCenter(center);

    center[1] = this->mCenterY+0.01f;


    HeU32 color = 0xFFFFFF;

    if ( showBound )
    {

      if ( nodes == 0 )
      {
        color = 0xFFFF00;
      }
      if ( mFlags & NF_IS_WATER )
      {
        color = 0x8080FF;
      }
      else if ( mFlags & NF_IS_TERRAIN )
      {
        color = 0x80FF80;
      }
      else if ( mFlags & NF_IS_UNDERWATER )
      {
        color = 0xFFFF20;
      }
      else if ( mFlags & ( NF_AVOID_1 | NF_AVOID_2 | NF_AVOID_3 | NF_AVOID_4 ) )
      {
        color = 0xFFD0D0;
      }
      else if ( mFlags & (NF_PREFER_1 | NF_PREFER_2 | NF_PREFER_3 |NF_PREFER_4))
      {
        color = 0xD0FFD0;
      }
      if ( region_update )
      {
        color = 0xFF0000;
      }

      if ( mCenterY < mBmin[1] || mCenterY > mBmax[1] )
      {
        color = 0xFF0000;
      }

      if ( show_solid )
      {
        gBaseRenderDebug->DebugBound(mBmin,mBmax,color,duration,true,true);
        gBaseRenderDebug->DebugBound(mBmin,mBmax,0xFFFFFF,duration,true,false);
      }
      else
      {
        gBaseRenderDebug->DebugBound(mBmin,mBmax,color,duration,true,false);
//        gBaseRenderDebug->DebugSphere(center,0.10f,0xFFFF00,duration,true,show_solid);
      }
    }

    if ( showConnections && (!region_update) && color != 0xFF0000 )
    {

      gBaseRenderDebug->DebugSphere(center,0.1f,0xFFFF00,duration,true,show_solid);

      for (HeU32 i=0; i<mCount; i++)
      {
        AINode &n = *mConnections[i];

        HeF32 c[3];
        n.getCenter(c);

        c[1] = n.mCenterY+0.01f;

        {
          PathEdge &edge = pinst->mEdges[i+mEdgeIndex];

          if ( edge.hasPathEdgeFlag(PEF_ACTIVE) )
          {
            if ( n.contains(this,pinst,nodes) )
            {
              HeU32 color = 0x00FF00;
              if ( cost > 1 )
                color = 0x004000;
              else if ( cost < 1 )
                color = 0x40FFFF;

              if (show_solid )
                gBaseRenderDebug->DebugCylinder(center,c,0.01f,color,duration,true,true);
              else
                gBaseRenderDebug->DebugLine(center,c,color,duration);
            }
            else
            {
              if ( show_solid)
                gBaseRenderDebug->DebugThickRay(center,c,0.01f,0.05f,0xFF0000,0xFF0000,duration);
              else
                gBaseRenderDebug->DebugRay(center,c,0.05f,0xFF0000,0xFF0000,duration);
            }
          }
          else
          {
            if ( show_solid )
              gBaseRenderDebug->DebugCylinder(center,c,0.01f,0x404040,duration,true,true);
            else
              gBaseRenderDebug->DebugLine(center,c,0x404040,duration);
          }
        }
      }
    }
  }
//#pragma optimize("",on)

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

  bool contains(const AINode *c) const
  {
    bool ret = false;

    for (HeU32 i=0; i<mCount; i++)
    {
      if ( c == mConnections[i] )
      {
        ret = true;
        break;
      }
    }
    return ret;
  }

  bool contains(const AINode *c,PathInstance * /*pinst*/,const AINode * /*nodes*/) const
  {
    bool ret = false;

    for (HeU32 i=0; i<mCount; i++)
    {
      if ( c == mConnections[i] )
      {
        ret = true;
        break;
      }
    }
    return ret;
  }

  void getResult(HBPATHSYSTEM::PathNode &r,const AINode *root) const
  {
    r.mBiDirectional = true;
    r.mBmin[0]   = mBmin[0];
    r.mBmin[1]   = mBmin[1];
    r.mBmin[2]   = mBmin[2];

    r.mBmax[0]   = mBmax[0];
    r.mBmax[1]   = mBmax[1];
    r.mBmax[2]   = mBmax[2];

    r.mCenter[0] = (mBmin[0]+mBmax[0])*0.5f;
    r.mCenter[1] = mCenterY;
    r.mCenter[2] = (mBmin[2]+mBmax[2])*0.5f;
    r.mIndex     = this-root;
  }

  HeF32        getDistance(const AI_Node *node,void *userData);

  HeF32        getCost(const AI_Node *node,void *userData);


  void setNodeFlag(NodeFlag flag)
  {
    mFlags|=flag;
  }

  void clearNodeFlag(NodeFlag flag)
  {
    mFlags&=~flag;
  }

  bool hasNodeFlag(NodeFlag flag) const
  {
    bool ret = false;
    if ( flag & mFlags )
      ret = true;

    return ret;
  }

  bool propertiesMatch(const HBPATHSYSTEM::PathProperties &p)
  {
    bool ret = true;

    switch ( mFlags )
    {
      case 0:
        ret = p.mNonTerrain;
        break;
      case NF_IS_WATER:
        ret = p.mWater;
        break;
      case NF_IS_UNDERWATER:
        ret = p.mUnderwater;
        break;
      case NF_IS_TERRAIN:
        ret = p.mTerrain;
        break;
      case NF_AVOID_1:
        ret = p.mPathAvoid1;
        break;
      case NF_AVOID_2:
        ret = p.mPathAvoid2;
        break;
      case NF_AVOID_3:
        ret = p.mPathAvoid3;
        break;
      case NF_AVOID_4:
        ret = p.mPathAvoid4;
        break;
      case NF_PREFER_1:
        ret = p.mPathPrefer1;
        break;
      case NF_PREFER_2:
        ret = p.mPathPrefer2;
        break;
      case NF_PREFER_3:
        ret = p.mPathPrefer3;
        break;
      case NF_PREFER_4:
        ret = p.mPathPrefer4;
        break;
    }


    return ret;
  }

  HeU32 getEdgeCount(void *userData) const;
  AI_Node *    getEdge(HeI32 index,void *userData) const;

  HeU32          mKey;
  HeU32          mFlags;
  HeF32          mBmin[3];
  HeF32          mBmax[3];
  HeF32          mCenterY;
  HeU32          mEdgeIndex;
  HeU32          mCount;
  AINode       **mConnections;
};

typedef USER_STL::vector< AINode > AINodeVector;

class EdgeReport : public KdTreeInterface
{
public:
  EdgeReport(PathEdgeInterface *callback)
  {
    mCallback = callback;
    mCount    = 0;
  }

  HeU32 intersect(KdTree &tree,const HeF32 *bmin,const HeF32 *bmax,bool exact,AINode *nodes)
  {
    mCount = 0;
    mNodes   = nodes;
    mExact   = exact;

    mBmin[0] = bmin[0];
    mBmin[1] = bmin[1];
    mBmin[2] = bmin[2];

    mBmax[0] = bmax[0];
    mBmax[1] = bmax[1];
    mBmax[2] = bmax[2];

    HeF32 center[3];
    center[0] = (mBmax[0]+mBmin[0])*0.5f;
    center[1] = (mBmax[1]+mBmin[1])*0.5f;
    center[2] = (mBmax[2]+mBmin[2])*0.5f;

    HeF32 dx = mBmax[0]-mBmin[0];
    HeF32 dy = mBmax[1]-mBmin[1];
    HeF32 dz = mBmax[2]-mBmin[2];

    HeF32 radius = sqrtf( dx*dx + dy*dy + dz*dz ) * 0.5f;

    tree.search(center,radius,this);

    return mCount;
  }

  void foundNode(KdTreeNode *node,HeF32 /*distanceSquared*/)
  {
    AINode *n = (AINode *) node->getUserData();

    if ( fm_intersectAABB(n->mBmin,n->mBmax, mBmin, mBmax ) )
    {
      HeU32 index = n - mNodes;
      HeF32 center[3];
      n->getCenter(center);
      mCallback->nodeIntersect(index,center);
      mCount++;
    }

    for (HeU32 i=0; i<n->mCount; i++)
    {
      AINode *to = n->mConnections[i];
      HeF32 c1[3];
      HeF32 c2[3];

      n->getCenter(c1);
      to->getCenter(c2);

      HeU32 i1 = n - mNodes;
      HeU32 i2 = to - mNodes;

      if ( mExact )
      {
        HeF32 time;
        if ( fm_lineTestAABBXZ( c1, c2,mBmin,mBmax, time ) )
        {

          mCallback->edgeIntersect( n->mEdgeIndex+i, c1, c2, i1, i2 );
          mCount++;
        }
      }
      else
      {
        mCallback->edgeIntersect( n->mEdgeIndex+i, c1, c2, i1, i2 );
        mCount++;
      }
    }
  }

private:
  HeU32       mCount;
  AINode            *mNodes;
  bool               mExact;
  HeF32              mBmin[3];
  HeF32              mBmax[3];
  PathEdgeInterface *mCallback;
};

class AIPathFindFilter : public KdTreeFilter
{
public:
  AIPathFindFilter(PathInstance *pinst,AINode *nodes,HBPATHSYSTEM::PathProperties *properties)
  {
    mPathInstance = pinst;
    mNodes        = nodes;
    mProperties   = properties;
  }


  bool filterNode(KdTreeNode *node)
  {
    bool ok = true;

    AINode *connect = (AINode *) node->getUserData();
    if ( !connect->propertiesMatch(*mProperties) )
    {
      ok = false;
    }
    else
    {
      HeU32 index = connect - mNodes;

      if ( mPathInstance->mNodeCosts && mPathInstance->mNodeCosts[index] == 0 )
      {
        ok = false;
      }
    }
    return ok;
  }


private:
  AINode        *mNodes;
  PathInstance  *mPathInstance;
  HBPATHSYSTEM::PathProperties  *mProperties;
};

typedef USER_STL::vector< HBPATHSYSTEM::PathNode > PathNodeVector;

class NodesInRange : public KdTreeInterface
{
public:
  NodesInRange(void)
  {
    mProperties = 0;
    mRoot       = 0;
  }


  void foundNode(KdTreeNode *node,HeF32 /*distanceSquared*/)
  {
    AINode *n = (AINode *) node->getUserData();

    if ( n->propertiesMatch(*mProperties) )
    {
      bool ok = true;

      HBPATHSYSTEM::PathNode p;
      n->getResult(p,mRoot);

      if ( mBounds )
      {
        HeVec3 v(p.mCenter);
        ok = mBounds->contain(v);
      }
      if ( ok )
      {
#if 0 // just for debugging...
        if ( gBaseRenderDebug )
        {
          HeF32 s = gBaseRenderDebug->getRenderScale();
          gBaseRenderDebug->setRenderScale(0.1f);
          n->render(true,true,mPathInstance,mRoot,30.0f);
          gBaseRenderDebug->setRenderScale(s);
        }
#endif
        mResults.push_back(p);
      }
    }
  }

  HBPATHSYSTEM::PathNode * getNodesInRange(KdTree *tree,const HeF32 *point,HeF32 radius,HeU32 &count,HBPATHSYSTEM::PathProperties *props,const AINode *root,PathInstance *pinstance,const HeBounds3 *bounds)
  {
    HBPATHSYSTEM::PathNode *ret = 0;

    count = 0;
    mRoot = root;
    mPathInstance = pinstance;
    mBounds = bounds;

    if ( root )
    {
      mResults.clear();
      mProperties = props;

      tree->search(point,radius,this);

      if ( !mResults.empty() )
      {
        count = mResults.size();
        ret = &mResults[0];
      }
    }
    return ret;
  }



  const AINode                 *mRoot;
  HBPATHSYSTEM::PathProperties *mProperties;
  PathNodeVector                mResults;
  PathInstance                 *mPathInstance;
  const HeBounds3              *mBounds;
};

typedef USER_STL::vector< HBPATHSYSTEM::PathNode > PathNodeVector;

class MyAIPathFind : public KdTreeInterface, public AIPathFind
{
public:
  MyAIPathFind(const AIPathFind &/*c*/)
  {
    HE_ASSERT(0); // not implemented yet
  }

  bool validVersionNumber(HeU32 version_number)
  {
    bool ret = false;

    if ( version_number == 106 || version_number == 107 )
      ret = true;

    return ret;
  }

  MyAIPathFind(const void *mem,HeU32 len,HeU32 &ncount,HeU32 &ccount)
  {

    mKey[0] = 'P';
    mKey[1] = 'A';
    mKey[2] = 'T';
    mKey[3] = 'H';
    mKey[4] = 'F';
    mKey[5] = 'I';
    mKey[6] = 'N';
    mKey[7] = 'D';

    mShowBound = true;
    mShowSolid = false;
    mShowConnections = true;
    ncount = 0;
    ccount = 0;
    mNodes = 0;
    mConnections = 0;
    mNodeCount = 0;
    mConnectionCount = 0;
    mEdges = 0;

    FILE_INTERFACE *fph = fi_fopen("connections","rb",const_cast<void *>(mem),len);
    if ( fph )
    {
      fi_fread(&mHeader,sizeof(AIPathHeader),1,fph);
      char temp[9];
      memcpy(temp,mHeader.mPath,8);
      temp[8] = 0;

      if ( strcmp(temp,"AIPATHID") == 0 && mHeader.mNodeCount && validVersionNumber(mHeader.mVersionNumber) )
      {

#define HACK_ME 0
#if HACK_ME
        static bool first = true;
        if ( first )
        {
          mHeader.mNodeCount = 1;
          mHeader.mConnectionCount = 0;
          first = false;
        }

#endif

        ncount = mHeader.mNodeCount;
        ccount = mHeader.mConnectionCount;

        SEND_TEXT_MESSAGE(0,"PathData contains: %d nodes with %d connections.\r\n", ncount, ccount );

        mNodeCount = ncount;
        mConnectionCount = ccount;

        mNodes = MEMALLOC_NEW_ARRAY(AINode,mHeader.mNodeCount)[mHeader.mNodeCount];

        mConnections = 0;
        if ( mHeader.mConnectionCount )
          mConnections = (AINode **)MEMALLOC_MALLOC_TYPE( sizeof(AINode *)*mHeader.mConnectionCount, "AINode *",__FILE__,__LINE__);

        AINode *n = mNodes;

        AINode **connections = mConnections;

        for (HeU32 i=0; i<ncount; i++)
        {
          HeU32 edgeIndex = connections - mConnections;
          connections = n->read(fph,mNodes,connections,edgeIndex,mHeader.mVersionNumber,mHeader.mLongEdge);
          n++;
        }
      }

      fi_fclose(fph);

      // ok..now we have to add them to the KdTree
      if ( ncount )
      {
        RandPool r(ncount,ncount);
        for (HeU32 i=0; i<ncount; i++)
        {
          bool shuffled;
          HeU32 select = r.get(shuffled);
          AINode *n = &mNodes[select];

          HeF32 center[3];
          n->getCenter(center);
          HeF32 radius = n->getRadius();

          if ( n->mCount > 0 )
          {
            mTree.add(center[0],center[1],center[2],radius,n);
          }
        }
      }
    }

  }

  ~MyAIPathFind(void)
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
    HE_ASSERT( mKey[4] == 'F' );
    HE_ASSERT( mKey[5] == 'I' );
    HE_ASSERT( mKey[6] == 'N' );
    HE_ASSERT( mKey[7] == 'D' );

    if ( mKey[0] == 'P' &&
         mKey[1] == 'A' &&
         mKey[2] == 'T' &&
         mKey[3] == 'H' &&
         mKey[4] == 'F' &&
         mKey[5] == 'I' &&
         mKey[6] == 'N' &&
         mKey[7] == 'D' )
    {
      ret = true;
    }
    return ret;
  }

  AIPathSolver  * startFindPath(const HeF32 *p1,const HeF32 *p2,PathInstance *edges,HBPATHSYSTEM::PathProperties *properties,const HBPATHSYSTEM::PathBuildProps &props);

  void release(void)
  {
    delete []mNodes;
    mNodes = 0;
    MEMALLOC_FREE(mConnections);
    mConnections = 0;
    mHeader.mNodeCount = 0;
    mHeader.mConnectionCount = 0;
    mNodeCount = 0;
  }


  void foundNode(KdTreeNode *node,HeF32 /*distanceSquared*/)
  {
    AINode *n = (AINode *) node->getUserData();
    n->render(mShowBound,mShowConnections,mShowSolid,mEdges,mNodes);
  }


  HeU32 render(const HeF32 *eyePos,HeF32 range,bool showBounds,bool showConnections,bool showSolid,bool showPathCacheValid,bool showPathCacheInvalid,PathInstance *edges)
  {
    HeU32 ret = 0;

    mEdges     = edges;
    mShowBound = showBounds;
    mShowSolid = showSolid;
    mShowConnections = showConnections;
    mShowPathCacheValid = showPathCacheValid;
    mShowPathCacheInvalid = showPathCacheInvalid;


    if ( gBaseRenderDebug && eyePos )
    {
      if ( mShowConnections || mShowBound )
      {
        ret = mTree.search(eyePos,range,this);
      }

      if ( (showPathCacheValid || showPathCacheInvalid) && edges && edges->mStringPull )
      {
        edges->mStringPull->render(this,eyePos,range);
      }

    }


    return ret;
  }

  bool qstartup(const HeF32 *p1,const HeF32 *p2,AIPathSolver *solver,PathInstance *pinst);

  AINode * getNearestNode(const HeF32 *point,PathInstance *pinst,HBPATHSYSTEM::PathProperties *properties,const HeF32 *dir)
  {
    AINode *fnode = 0;

    HeStatsProbe _p("getNearestNode","Global Physics Update");

    // Search the Root NavMesh.
    if ( mNodes )
    {
      HBPATHSYSTEM::PathProperties p(*properties);
      AIPathFindFilter filter(pinst,mNodes,&p);
      float d = 4;

      while ( d <= 32 )
      {

        KdTreeFindNode found[16];
        HeU32 count = mTree.search( point, d, 16, &found[0], &filter);

        if ( count )
        {
          fnode = (AINode *) found[0].mNode->getUserData();
          AINode *nearest = 0;
          HeF32 nearDot = 0;

          for (HeU32 i=0; i<count; i++)
          {
            AINode *check = (AINode *) found[i].mNode->getUserData();

            if ( check->contains(point,INFLATION) )
            {
              fnode = check;
              break;
            }
            else
            {
              if ( dir )
              {
                HeF32 pdir[3];
                HeF32 center[3];
                check->getCenter(center);

                pdir[0] = center[0] - point[0];
                pdir[1] = center[1] - point[1];
                pdir[2] = center[2] - point[2];

                fm_normalize(pdir);

                HeF32 dot = fm_dot(dir,pdir);
                if ( !nearest || dot > nearDot )
                {
                  nearest = check;
                  nearDot = dot;
                }
              }
            }
          }

          if ( nearest )
          {
            fnode = nearest;
          }
        }

        if ( fnode )
          break;

        d*=2;
      }
    }

    return fnode;
  }

  AINode * getNearestValidNode(const HeF32 *point,PathInstance *pinst,HBPATHSYSTEM::PathProperties *properties,const HeF32 *dir)
  {
    AINode *nearest = 0;


    HeStatsProbe _p("getNearestValidNode","Global Physics Update");

    // Search the Root NavMesh.
    if ( mNodes )
    {
      HBPATHSYSTEM::PathProperties p(*properties);
      AIPathFindFilter filter(pinst,mNodes,&p);

      float d = 64;
      float nearDist = d*d;

      KdTreeFindNode found[16];
      HeU32 count = mTree.search( point, d, 16, &found[0], &filter);

      bool exact_match = false;

      if ( count )
      {
        HeF32 nearDot = 0;
        for (HeU32 i=0; i<count; i++)
        {
          AINode *check = (AINode *) found[i].mNode->getUserData();
          if ( check->contains(point,INFLATION) )
          {
            exact_match = true;
            nearest = check;
            break;
          }
          else
          {

            HeF32 center[3];
            check->getCenter(center);

            HeF32 sect[3];
            bool hit = fm_intersectLineSegmentAABB(check->mBmin, check->mBmax, point, center, sect );
            HE_ASSERT(hit);

            if ( hit )
            {
              float dist = fm_distanceSquared(center,point);
              if ( dist < 4.0f )
              {

                if ( dir )
                {
                  HeF32 pdir[3];

                  pdir[0] = center[0] - point[0];
                  pdir[1] = center[1] - point[1];
                  pdir[2] = center[2] - point[2];

                  fm_normalize(pdir);

                  HeF32 dot = fm_dot(dir,pdir);
                  if ( !nearest || dot > nearDot )
                  {
                    nearest = check;
                    nearDot = dot;
                  }
                }
                else
                {
                  if ( !nearest || dist < nearDist )
                  {
                    nearDist = dist;
                    nearest = check;
                  }
                }
              }
            }
          }
        }
      }
    }

    return nearest;
  }


  HBPATHSYSTEM::PathNode *   getNearestPointConnections(HeF32 *point,PathInstance *pinst,HBPATHSYSTEM::PathProperties *properties,HeU32 &pcount)
  {
    HBPATHSYSTEM::PathNode *ret = 0;

    mResults.clear();

    AINode *node = getNearestNode(point,pinst,properties,0);
    if ( node )
    {
      HBPATHSYSTEM::PathNode p;
      node->getResult(p, mNodes );
      p.mCenter[0]*=gInverseGameScale;
      p.mCenter[1]*=gInverseGameScale;
      p.mCenter[2]*=gInverseGameScale;
      p.mBmin[0]*=gInverseGameScale;
      p.mBmin[1]*=gInverseGameScale;
      p.mBmin[2]*=gInverseGameScale;
      p.mBmax[0]*=gInverseGameScale;
      p.mBmax[1]*=gInverseGameScale;
      p.mBmax[2]*=gInverseGameScale;
      mResults.push_back(p);


      HeU32 ecount = node->getEdgeCount(0);
      for (HeU32 i=0; i<ecount; i++)
      {
        AI_Node *n = node->getEdge(i,0);
        if ( n )
        {
          AINode *an = static_cast< AINode *>(n);
          HBPATHSYSTEM::PathNode p;
          an->getResult(p, mNodes );
          p.mCenter[0]*=gInverseGameScale;
          p.mCenter[1]*=gInverseGameScale;
          p.mCenter[2]*=gInverseGameScale;
          p.mBmin[0]*=gInverseGameScale;
          p.mBmin[1]*=gInverseGameScale;
          p.mBmin[2]*=gInverseGameScale;
          p.mBmax[0]*=gInverseGameScale;
          p.mBmax[1]*=gInverseGameScale;
          p.mBmax[2]*=gInverseGameScale;
          p.mBiDirectional = an->contains(node,pinst,mNodes);
          mResults.push_back(p);
        }
      }

      pcount = mResults.size();
      ret = &mResults[0];
    }

    return ret;
  }

  bool getNearestPoint(HeF32 *point,PathInstance *pinst,HBPATHSYSTEM::PathProperties *properties,bool validate)
  {
    bool ret = false;

    AINode *node = 0;

    if ( validate )
    {
      node = getNearestValidNode(point,pinst,properties,0);
    }
    else
    {
      node = getNearestNode(point,pinst,properties,0);
    }
    if ( node )
    {
      ret = true;
      node->getCenter(point);
    }

    return ret;
  }

  bool getRandomPoint(HeF32 *point,const HBPATHSYSTEM::PathProperties *properties)
  {
    bool ret = false;

    if ( mNodes && properties )
    {
      HeU32 select = mRand.get()%mNodeCount;
      for (HeU32 i=0; i<mNodeCount; i++)
      {
        AINode &node = mNodes[select];
        if ( node.propertiesMatch(*properties))
        {
          node.getCenter(point);
          ret = true;
          break;
        }
        else
        {
          select++;
          if ( select == mNodeCount )
          {
            select = 0;
          }
        }
      }
    }

    return ret;
  }


  HeU32  intersectEdges(PathEdgeInterface *callback,const HeF32 *bmin,const HeF32 *bmax,bool exact)
  {
    HeU32 ret = 0;

     EdgeReport er(callback);

     ret = er.intersect(mTree,bmin,bmax,exact,mNodes);

     return ret;
  }

  HeU32 getNodeCount(HeU32 &ecount) const
  {
    ecount = mConnectionCount;
    return mNodeCount;
  }

  AINode * getNodes(void) const { return mNodes; };

  AINode * getBounds(HeU32 index,HeBounds3 &b)
  {
    AINode *ret = 0;

    if ( index < mNodeCount )
    {
      ret = &mNodes[index];
      ret->getBounds(b);
    }
    return ret;
  }


  void enumerateNodes(AIPathFindInterface *iface)
  {
    AINode *scan = mNodes;
    for (HeU32 i=0; i<mNodeCount; i++)
    {
      HeF32 center[3];
      scan->getCenter(center);

      HeF32 bmin[3];
      HeF32 bmax[3];
      fm_copy3(scan->mBmin,bmin);
      fm_copy3(scan->mBmax,bmax);

      bmax[0]-=mHeader.mLongEdge;
      bmax[2]-=mHeader.mLongEdge;

      iface->reportNode(i,center,bmin,bmax,scan->mFlags);

      scan++;
    }
  }

  void enumerateConnections(AIPathFindInterface *iface)
  {
    AINode *scan = mNodes;
    for (HeU32 i=0; i<mNodeCount; i++)
    {
      for (HeU32 j=0; j<scan->mCount; j++)
      {
        AINode *n = scan->mConnections[j];
        HeU32 index = n - mNodes;
        iface->reportConnection(i,index);
      }
      scan++;
    }
  }

  bool getPathDataVersion(HeU32 &build_version,HeU32 &asset_version,HeU64 &timeStamp)
  {
    bool ret = true;

    build_version = mHeader.mVersionNumber;
    asset_version = mHeader.mSourceVersionNumber;
    timeStamp     = mHeader.mAssetTimeStamp;

    return ret;
  }

  void debugString(const HeF32 *bmin,const HeF32 *bmax,HeU32 i1,HeU32 i2,CanWalkState state)
  {
    HE_ASSERT( i1 < mNodeCount );
    HE_ASSERT( i2 < mNodeCount );

    bool ok;

    if ( state == CWS_CAN_WALK )
    {
      ok = mShowPathCacheValid;
    }
    else
    {
      ok = mShowPathCacheInvalid;
    }

    if ( ok && i1 < mNodeCount && i2 < mNodeCount )
    {
      HeF32 c1[3];
      HeF32 c2[3];

      mNodes[i1].getCenter(c1);
      mNodes[i2].getCenter(c2);

      HeF32 time;
      if ( fm_lineTestAABB(c1,c2,bmin,bmax,time) )
      {
        HeU32 color = 0;
        switch ( state )
        {
          case CWS_CAN_WALK:
            color = 0xC0C0FF;
            break;
          case CWS_CANNOT_WALK:
            color = 0xFF8080;
            break;
          case CWS_PATH_FAILURE:
            color = 0xFF20FF;
            break;
        }
        if ( color )
        {
          c1[1]+=0.011f;
          c2[1]+=0.011f;
          gBaseRenderDebug->DebugLine(c1,c2,color);
        }
      }
    }
  }


  HBPATHSYSTEM::PathNode * getNodesInRange(const HeF32 * point,HeF32 radius,HeU32 &count,HBPATHSYSTEM::PathProperties *props,const HeBounds3 *b)
  {
    return mNodesInRange.getNodesInRange(&mTree,point,radius,count,props,mNodes,mEdges,b);
  }


  bool getRandomPointInVolume(const HeBounds3 &b,HeVec3 &point,HBPATHSYSTEM::PathProperties * props)
  {
    bool ret = false;

    HE_ASSERT(props);

    HeVec3 center;
    b.getCenter(center);

    HeF32 radius = b.getDiagonalDistance()*0.5f;
    HeU32 count;
    HBPATHSYSTEM::PathNode *nodes = getNodesInRange(&center.x,radius,count,props,&b);
    if ( nodes )
    {
      HeU32 index = rand()%count;
      HBPATHSYSTEM::PathNode &n = nodes[index];
      point.set( n.mCenter );
      ret = true;
    }

    return ret;
  }

  bool  getRandomPointInSphere(const HeVec3 &center,HeF32 radius,HeVec3 &point,HBPATHSYSTEM::PathProperties * props)
  {
    bool ret = false;

    HE_ASSERT(props);

    HeU32 count;
    HBPATHSYSTEM::PathNode *nodes = getNodesInRange(&center.x,radius,count,props,0);
    if ( nodes )
    {
      HeU32 index = rand()%count;
      HBPATHSYSTEM::PathNode &n = nodes[index];
      point.set( n.mCenter );
      ret = true;
    }

    return ret;
  }

  bool getValidIndex(AINode *node,HeU32 &index) const
  {
    bool ret = false;

    int i = node - mNodes;
    HE_ASSERT( i >= 0 && i < (int) mNodeCount );
    if ( i >= 0 && i < (int) mNodeCount )
    {
      index = (HeU32)i;
      ret = true;
    }
    return ret;
  }

private:
  char                 mKey[8];
  KdTree               mTree;
  HeU32                mNodeCount;
  HeU32                mConnectionCount;
  AINode              *mNodes;
  AINode             **mConnections;
  AIPathHeader         mHeader;
  bool                 mShowConnections:1;
  bool                 mShowBound:1;
  bool                 mShowPathCacheValid:1;
  bool                 mShowPathCacheInvalid:1;
  bool                 mShowSolid:1;
  Rand                 mRand;
  PathInstance        *mEdges;
  NodesInRange         mNodesInRange;
  PathNodeVector       mResults;
};


AIPathFind *doLoadAI(const void *mem,HeU32 len,HeU32 &ncount,HeU32 &ccount)
{
  AIPathFind *ret = 0;

  ncount = 0;
  ccount = 0;

  ret = MEMALLOC_NEW(MyAIPathFind)(mem,len,ncount,ccount);

  return static_cast< AIPathFind *>(ret);
}


bool        doRelease(AIPathFind *pfind)
{
  bool ret = false;

  if ( pfind )
  {
    MyAIPathFind *mpf = static_cast< MyAIPathFind *>(pfind);
    ret = true;
    delete mpf;
  }

  return ret;
}


class AIPathSolver
{
public:
  AIPathSolver(AIPathFind *pfind,const HeF32 *p1,const HeF32 *p2,PathInstance *edges,HBPATHSYSTEM::PathProperties *properties,const HBPATHSYSTEM::PathBuildProps &props)
  {

    mKey[0] = 'A';
    mKey[1] = 'I';
    mKey[2] = 'S';
    mKey[3] = 'O';
    mKey[4] = 'L';
    mKey[5] = 'V';
    mKey[6] = 'E';
    mKey[7] =  0;

    HE_ASSERT(properties);
    mPathProperties = *properties;
    mBuildProps = props;
    mPathFind = pfind;
    mFrom[0] = p1[0];
    mFrom[1] = p1[1];
    mFrom[2] = p1[2];
    mTo[0] = p2[0];
    mTo[1] = p2[1];
    mTo[2] = p2[2];
    mAstar = 0;
    MyAIPathFind *mpf = static_cast< MyAIPathFind *>(mPathFind);
    mRoot = mpf->getNodes();
    mEdges = edges;
    mOk = mpf->qstartup(mFrom,mTo,this,edges);
    mDir[0] = mTo[0] - mFrom[0];
    mDir[1] = mTo[1] - mFrom[1];
    mDir[2] = mTo[2] - mFrom[2];
    fm_normalize(mDir);


  }

  ~AIPathSolver(void)
  {
    if ( mAstar )
      releaseHeAstar(mAstar);
    validate();
    memset(mKey,0,sizeof(mKey));
  }

  bool validate(void) const
  {
    bool ret = false;

    HE_ASSERT( mKey[0] == 'A' );
    HE_ASSERT( mKey[1] == 'I' );
    HE_ASSERT( mKey[2] == 'S' );
    HE_ASSERT( mKey[3] == 'O' );
    HE_ASSERT( mKey[4] == 'L' );
    HE_ASSERT( mKey[5] == 'V' );
    HE_ASSERT( mKey[6] == 'E' );
    HE_ASSERT( mKey[7] == 0   );

    if (  mKey[0] == 'A' &&
          mKey[1] == 'I' &&
          mKey[2] == 'S' &&
          mKey[3] == 'O' &&
          mKey[4] == 'L' &&
          mKey[5] == 'V' &&
          mKey[6] == 'E' &&
          mKey[7] == 0   )
    {
      ret = true;
    }


    return ret;
  }


  HBPATHSYSTEM::PathNode *findPath(HeU32 &count,HeU32 solverCount,bool &ok,HeU32 &totalSolverCount,HeF32 globalTime)
  {
    HBPATHSYSTEM::PathNode *ret = 0;

    //HeStatsProbe _p("findPath","doFindPath");

    ok = mOk;

    if ( mOk )
    {
      bool solved = false;

      totalSolverCount = 0;

      if ( mAstar )
      {

        AI_Node *from = mAstar->astarGetFrom();
        AI_Node *to   = mAstar->astarGetTo();

        if ( from == to )
        {
          HBPATHSYSTEM::PathNode p1;
          HBPATHSYSTEM::PathNode p2;

          p1.simpleCenterBox(mFrom,1);
          p2.simpleCenterBox(mTo,1);

          mPath.push_back(p1);
          mPath.push_back(p2);

          count = mPath.size();

          ret = &mPath[0];

        }
        else
        {
          for (HeU32 i=0; i<solverCount; i++)
          {
            HeU32 searchCount;
            solved = mAstar->astarSearchStep(searchCount);
            totalSolverCount+=searchCount;
            if ( solved ) break;
            if ( totalSolverCount > solverCount )
              break;
          }

          if ( solved )
          {
            AI_Node **solution = mAstar->getSolution(count);
            if ( solution )
            {
              HBPATHSYSTEM::PathNode p1;
              HBPATHSYSTEM::PathNode p2;

              p1.simpleCenterBox(mFrom,1);
              p2.simpleCenterBox(mTo,1);

              for (HeU32 i=0; i<count; i++)
              {
                AI_Node *n = solution[i];

                AINode *node = static_cast< AINode *>(n);

                HBPATHSYSTEM::PathNode r;
                node->getResult(r,mRoot);

                if ( i == 0 )
                {
                  if ( r.contains(mFrom,INFLATION) )
                  {
                    mPath.push_back(p1);
                  }
                  else
                  {
                    mPath.push_back(p1);
                    mPath.push_back(r);
                  }
                }
                else if ( i == (count-1) )
                {
                  if ( r.contains(mTo,INFLATION) )
                  {
                    mPath.push_back(p2);
                  }
                  else
                  {
                    mPath.push_back(r);
                    mPath.push_back(p2);
                  }
                }
                else
                {
                  mPath.push_back(r);
                }
              }

              count = mPath.size();

              ret = &mPath[0];
  //            SEND_TEXT_MESSAGE(0,"AstarSearch(%d)\r\n", count );
            }
            else
            {
              if ( mEdges )
              {
                AI_Node *from = mAstar->astarGetFrom();
                AI_Node *to   = mAstar->astarGetTo();
                AINode *_from = static_cast< AINode *>(from);
                AINode *_to   = static_cast< AINode *>(to);
                HeU32 i1 = _from - mRoot;
                HeU32 i2 = _to   - mRoot;

  //              SEND_TEXT_MESSAGE(0,"Setting PathFailure(%d)-(%d)\r\n", i1, i2 );
                mEdges->mStringPull->addTracker(i1,i2,CWS_PATH_FAILURE,mPathProperties.mInternalGuid,globalTime);

              }
              ok = false; //
            }
          }
        }
      }
    }

    return ret;
  }

  bool isOk(void) const { return mOk; };

  PathInstance * getPathInstance(void) const { return mEdges; };


  const HBPATHSYSTEM::PathProperties * getPathProperties(void) const { return &mPathProperties; };

public:
  char                                mKey[8];
  bool                                mOk;
  HeF32                               mFrom[3];
  HeF32                               mTo[3];
  HeF32                               mDir[3];
  HBPATHSYSTEM::PathBuildProps        mBuildProps;
  const AINode                       *mRoot;
  AIPathFind                         *mPathFind;
  HeAstar                            *mAstar;
  AINodeResultVector                  mPath;
  PathInstance                       *mEdges;
  HBPATHSYSTEM::PathProperties        mPathProperties;
};


bool MyAIPathFind::qstartup(const HeF32 *p1,const HeF32 *p2,AIPathSolver *solver,PathInstance *pinst)
{
  bool ret = false;


  //HeStatsProbe _p("getDistance","doStartFindPath");

  if ( solver && pinst && pinst->mParent )
  {
    float dir[3];

    dir[0] = p2[0] - p1[0];
    dir[1] = p2[1] - p1[1];
    dir[2] = p2[2] - p1[2];

    fm_normalize(dir);

    AINode *node1 = getNearestValidNode(p1,solver->getPathInstance(),&solver->mPathProperties,dir);

    dir[0]*=-1;
    dir[1]*=-1;
    dir[2]*=-1;

    AINode *node2 = getNearestValidNode(p2,solver->getPathInstance(),&solver->mPathProperties,dir);

    if ( node1 && node2 )
    {
      ret = true;

      solver->mAstar = createHeAstar();
      solver->mAstar->astarStartSearch(node1,node2,solver);

      if ( pinst && pinst->mStringPull )
      {
        MyAIPathFind *mpf = static_cast< MyAIPathFind *>(pinst->mParent);

        HeU32 i1 = node1 - mpf->getNodes();
        HeU32 i2 = node2 - mpf->getNodes();

        CanWalkState state = pinst->mStringPull->canWalk(i1,i2,solver->mPathProperties.mInternalGuid);

        if ( state == CWS_CAN_WALK )
        {
          //  SEND_TEXT_MESSAGE(0,"Found Direct Solution!\r\n");
          if ( solver->mPathProperties.mStringPulling )
          {
            solver->mAstar->setDirectSolution();
          }
        }
        else if ( state == CWS_PATH_FAILURE )
        {
          ret = false; // immediately return a failure!
        }
      }

    }
  }

  return ret;
}


HBPATHSYSTEM::PathNode      *doFindPath(AIPathSolver *solver,HeU32 &count,HeU32 solverCount,bool &ok,HeU32 &totalSolverCount,HeF32 globalTime)
{
  HBPATHSYSTEM::PathNode      *ret = 0;

  if ( solver )
  {
    ret = solver->findPath(count,solverCount,ok,totalSolverCount,globalTime);
  }

  return ret;
}

void           doReleaseFindPath(AIPathSolver *solver)
{
  if ( solver )
  {
    delete solver;
  }
}


#define MAX_COST 10

#pragma warning(disable:4100)

HeF32        AINode::getDistance(const AI_Node *_node,void *userData)
{
  HeF32 ret = 9999;
  if ( validate() )
  {
    const AINode *n = static_cast< const AINode * >(_node);
    if ( n->validate() )
    {

      HeF32 c1[3];
      HeF32 c2[3];

      getCenter(c1);
      n->getCenter(c2);

      HeF32 dir[3];

      dir[0] = c1[0] - c2[0];
      dir[1] = c1[1] - c2[1];
      dir[2] = c1[2] - c2[2];

      ret = sqrtf(dir[0]*dir[0]+dir[1]*dir[1]+dir[2]*dir[2]);
    }
  }


  return ret;
}


HeF32        AINode::getCost(const AI_Node * _node,void * userData)
{
  HeF32 dot_cost = 1;
  HeF32 cost = 1;

  if ( validate() )
  {
    const AINode *n = static_cast< const AINode * >(_node);
    if ( n->validate() )
    {

      HeF32 c1[3];
      HeF32 c2[3];

      getCenter(c1);
      n->getCenter(c2);

      HeF32 dir[3];

      dir[0] = c2[0] - c1[0];
      dir[1] = c2[1] - c1[1];
      dir[2] = c2[2] - c1[2];

      if ( userData )
      {
        AIPathSolver *solver = (AIPathSolver *)userData;

        if ( solver->validate() )
        {

          if ( gWeightDir )
          {
            HeF32 sdir[3];

            sdir[0] = solver->mTo[0] - c1[0];
            sdir[1] = solver->mTo[1] - c1[1];
            sdir[2] = solver->mTo[2] - c1[2];

            HeF32 slen = sqrtf(sdir[0]*sdir[0]+sdir[1]*sdir[1]+sdir[2]*sdir[2]);
            if ( slen < 0.01f )
            {
              sdir[0] = solver->mDir[0];
              sdir[1] = solver->mDir[1];
              sdir[2] = solver->mDir[2];
            }
            else
            {
              HeF32 recip = 1.0f / slen;
              sdir[0]*=recip;
              sdir[1]*=recip;
              sdir[2]*=recip;
            }

            fm_normalize(dir);

            HeF32 dot = fm_dot(sdir,dir);

            dot_cost = 9 - (4*(dot+1));
          }

          PathInstance *pi = solver->getPathInstance();

          if ( pi == 0 )
          {
            SEND_TEXT_MESSAGE(0,"Solver has no PathInstance pointer! %s : %d\r\n", __FILE__, __LINE__ );
          }
          else if ( !pi->validate() )
          {
            SEND_TEXT_MESSAGE(0,"Solver reports an invalid path instance pointer! %s : %d\r\n", __FILE__, __LINE__);
          }
          else if ( !pi->mParent )
          {
            SEND_TEXT_MESSAGE(0,"Solver reports that the path instance pointer does not have a parent! %s : %d\r\n", __FILE__, __LINE__);
          }
          else
          {
            MyAIPathFind *mpf = static_cast< MyAIPathFind *>(pi->mParent);
            if ( mpf->validate() )
            {
              HeU32 index;
              if ( mpf->getValidIndex(this,index) )
              {
                dot_cost*= pi->mNodeCosts[index];
              }
              else
              {
                SEND_TEXT_MESSAGE(0,"Invalid index computed; %s : %d\r\n", __FILE__, __LINE__);
              }
            }
          }
        }


        const AINode *node   = static_cast< const AINode *>(this);

        if ( solver )
        {
          if ( solver->validate() )
          {

            const HBPATHSYSTEM::PathProperties *p = solver->getPathProperties();

            if ( p )
            {

              if ( p->validate() )
              {
                if ( node->hasNodeFlag(NF_IS_TERRAIN) )
                {
                  if ( p->mTerrain )
                  {
                    cost = p->mTerrainCost;
                  }
                  else
                  {
                    cost = MAX_COST;
                  }
                }
                else if ( node->hasNodeFlag(NF_IS_WATER) )
                {
                  if ( p->mWater )
                  {
                    cost = p->mWaterCost;
                  }
                  else
                  {
                    cost = MAX_COST;
                  }
                }
                else if ( node->hasNodeFlag(NF_IS_UNDERWATER ) )
                {
                  if ( p->mUnderwater )
                  {
                    cost = p->mUnderwaterCost;
                  }
                  else
                  {
                    cost = MAX_COST;
                  }
                }
                else if ( node->hasNodeFlag(NF_AVOID_1) )
                {
                  if ( p->mPathAvoid1 )
                  {
                    cost = p->mPathAvoidCost[0];
                  }
                  else
                  {
                    cost = MAX_COST;
                  }
                }
                else if ( node->hasNodeFlag(NF_AVOID_2) )
                {
                  if ( p->mPathAvoid2 )
                  {
                    cost = p->mPathAvoidCost[1];
                  }
                  else
                  {
                    cost = MAX_COST;
                  }
                }
                else if ( node->hasNodeFlag(NF_AVOID_3) )
                {
                  if ( p->mPathAvoid3 )
                  {
                    cost = p->mPathAvoidCost[2];
                  }
                  else
                  {
                    cost = MAX_COST;
                  }
                }
                else if ( node->hasNodeFlag(NF_AVOID_4) )
                {
                  if ( p->mPathAvoid4 )
                  {
                    cost = p->mPathAvoidCost[3];
                  }
                  else
                  {
                    cost = MAX_COST;
                  }
                }
                else if ( node->hasNodeFlag(NF_PREFER_1) )
                {
                  if ( p->mPathPrefer1 )
                  {
                    cost = p->mPathPreferCost[0];
                  }
                  else
                  {
                    cost = MAX_COST;
                  }
                }
                else if ( node->hasNodeFlag(NF_PREFER_2) )
                {
                  if ( p->mPathPrefer2 )
                  {
                    cost = p->mPathPreferCost[1];
                  }
                  else
                  {
                    cost = MAX_COST;
                  }
                }
                else if ( node->hasNodeFlag(NF_PREFER_3) )
                {
                  if ( p->mPathPrefer3 )
                  {
                    cost = p->mPathPreferCost[2];
                  }
                  else
                  {
                    cost = MAX_COST;
                  }
                }
                else if ( node->hasNodeFlag(NF_PREFER_4) )
                {
                  if ( p->mPathPrefer4 )
                  {
                    cost = p->mPathPreferCost[3];
                  }
                  else
                  {
                    cost = MAX_COST;
                  }
                }
                else
                {
                  if ( p->mNonTerrain )
                  {
                    cost = p->mNonTerrainCost;
                  }
                  else
                  {
                    cost = MAX_COST;
                  }
                }
              }
              else
              {
                SEND_TEXT_MESSAGE(0,"Invalid PathProperties pointer. %s : %d\r\n", __FILE__, __LINE__);
              }
            }
          }
          else
          {
            SEND_TEXT_MESSAGE(0,"Invalid PathSolver pointer! %s : %d\r\n", __FILE__, __LINE__);
          }
        }
        else
        {
          SEND_TEXT_MESSAGE(0,"UserData does not point to a valid 'PathSolver' data structure. %s : %d\r\n", __FILE__,__LINE__);
        }
      }

      if ( contains(n) ) // if this is a node we are connected to...
      {
        if ( !n->contains(this) ) // but the connection is uni-directional...
        {
          dot_cost = 4;
          cost*=8;
        }
      }
    }
    else
    {
      SEND_TEXT_MESSAGE(0,"Invalid Node Pointer in getCost %s : %d\r\n", __FILE__, __LINE__ );
    }
  }
  else
  {
    SEND_TEXT_MESSAGE(0,"getCost called with bad 'this' pointer. %s : %d\r\n", __FILE__, __LINE__ );
  }

  return cost*dot_cost;
}

HeU32 AINode::getEdgeCount(void * userData) const
{
  HeU32 ret = 0;

  if ( validate() )
  {
    if ( userData )
    {
      AIPathSolver *solver = (AIPathSolver *)userData;
      if ( !solver->validate() )
      {
        SEND_TEXT_MESSAGE(0,"AINode::getEdgeCount base path solver pointer! %s : %d\r\n", __FILE__, __LINE__);
      }
      else
      {
        ret = mCount;
        if ( mCenterY < mBmin[1] || mCenterY > mBmax[1] )
        {
          ret = 0;
        }
      }

    }
    else
    {
      ret = mCount;
      SEND_TEXT_MESSAGE(0,"AINode::getEdgeCount missing userData pointer! %s : %d\r\n", __FILE__, __LINE__);
    }
  }
  else
  {
    SEND_TEXT_MESSAGE(0,"AINode::getEdgeCount bad pointer. %s : %d\r\n", __FILE__, __LINE__ );
  }



  return ret;
}

AI_Node *    AINode::getEdge(HeI32 index,void *userData) const
{
  AI_Node *ret = 0;


  if ( validate() && index >= 0 && index < (int)mCount )
  {
    if ( userData )
    {
      AIPathSolver *solver = (AIPathSolver *)userData;
      if ( !solver->validate() )
      {
        SEND_TEXT_MESSAGE(0,"AINode::getEdge base path solver pointer! %s : %d\r\n", __FILE__, __LINE__);
      }
      else
      {
        ret = mConnections[index];

        AINode *node = static_cast< AINode *>(ret);

        AIPathSolver *solver = (AIPathSolver *)userData;
        if ( solver && ret )
        {

          const HBPATHSYSTEM::PathProperties *p = solver->getPathProperties();

          if ( p )
          {
            bool ok = true;
            switch ( node->mFlags )
            {
              case NF_IS_WATER:
                ok = p->mWater;
                break;
              case NF_IS_TERRAIN:
                ok = p->mTerrain;
                break;
              case NF_IS_UNDERWATER:
                ok = p->mUnderwater;
                break;
              case NF_AVOID_1:
                ok = p->mPathAvoid1;
                break;
              case NF_AVOID_2:
                ok = p->mPathAvoid2;
                break;
              case NF_AVOID_3:
                ok = p->mPathAvoid3;
                break;
              case NF_AVOID_4:
                ok = p->mPathAvoid4;
                break;
              case NF_PREFER_1:
                ok = p->mPathPrefer1;
                break;
              case NF_PREFER_2:
                ok = p->mPathPrefer1;
                break;
              case NF_PREFER_3:
                ok = p->mPathPrefer1;
                break;
              case NF_PREFER_4:
                ok = p->mPathPrefer1;
                break;
              default:
                ok = p->mNonTerrain;
                break;
            }
            if ( !ok )
            {
              ret = 0;
            }
          }
        }

        if ( ret )
        {
          AINode *n = static_cast< AINode *>(ret);
          if ( n->mCenterY < n->mBmin[1] || n->mCenterY > n->mBmax[1] )
          {
            ret = 0;
          }
        }
      }
    }
    else
    {
      SEND_TEXT_MESSAGE(0,"AINode::getEdge missing userData pointer! %s : %d\r\n", __FILE__, __LINE__);
      HE_ASSERT( index >= 0 && index < (int)mCount );
      ret = mConnections[index];
    }

  }
  else
  {
    if ( index >= 0 && index < (int)mCount )
    {
      SEND_TEXT_MESSAGE(0,"AINode::getEdge bad pointer. %s : %d\r\n", __FILE__, __LINE__ );
    }
    else
    {
      SEND_TEXT_MESSAGE(0,"AINode::getEdge bad array index of %d when our count is only %d.\r\n", index, mCount );
    }
  }

  return ret;
}


AIPathSolver  *MyAIPathFind::startFindPath(const HeF32 *p1,const HeF32 *p2,PathInstance *edges,HBPATHSYSTEM::PathProperties *properties,const HBPATHSYSTEM::PathBuildProps &props)
{
  AIPathSolver *ret = 0;

  ret = MEMALLOC_NEW(AIPathSolver)(this,p1,p2,edges,properties,props);
  if ( !ret->isOk() )
  {
    doReleaseFindPath(ret);
    ret = 0;
  }

  return ret;
}

void  stepSolution(void)
{
}

};  // END OF NAMESPACE


