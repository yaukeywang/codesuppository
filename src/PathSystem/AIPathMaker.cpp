#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <vector>

#include "common/snippets/UserMemAlloc.h"
#include "AIPathMaker.h"
#include "HbPhysics/HbPhysics.h"
#include "AIPathFind.h"
#include "NxScene.h"
#include "NxBounds3.h"
#include "NxShape.h"
#include "NxActor.h"
#include "common/snippets/pool.h"
#include "common/snippets/FloatMath.h"
#include "NxTriangleMeshShape.h"
#include "NxHeightField.h"
#include "NxHeightFieldDesc.h"
#include "NxHeightFieldShape.h"
#include "NxHeightFieldShapeDesc.h"
#include "NxConvexShapeDesc.h"
#include "NxConvexShape.h"
#include "NxConvexMeshDesc.h"
#include "NxTriangle.h"
#include "common/snippets/rand.h"
#include "NxActor.h"
#include "NxActorDesc.h"
#include "NxTriangleMesh.h"
#include "NxTriangleMeshDesc.h"
#include "NxTriangleMeshShapeDesc.h"
#include "NxShapeDesc.h"
#include "common/snippets/KdTree.h"
#include "HeroWorld/CollisionGroupFlag.h"
#include "NxUtilLib.h"
#include "PhysXLoader.h"
#include "common/snippets/keyvalue.h"
#include "common/snippets/sutil.h"
#include "common/snippets/simplevector.h"
#include "common/snippets/PhysXHeightfield.h"
#include "RenderDebug/RenderDebug.h"
#include "common/FileInterface/FileInterface.h"

#ifdef LINUX
#include "linux_compat.h"
#endif

#pragma warning(disable:4189)
#define DEBUG_TEST 0

#define DO_LOG 0

#define BUILD_GRID_SIZE 64
#define SLOP 2

#define MAX_SEARCH 64

#define SEARCH_SLOP 0.2f

#define MAX_GRID_WIDTH 128

#define MAX_GRID_SIZE (MAX_GRID_WIDTH*MAX_GRID_WIDTH)

extern HeF32 gInverseGameScale;


namespace HBPATHSYSTEM_AIWISDOM
{

class BuildAIPointHash;
typedef USER_STL::hash_map< HeU32, BuildAIPointHash * > IntPtr;

BuildAIPointHash * get(const IntPtr &list,HeU32 guid)
{
  BuildAIPointHash *ret = 0;

  IntPtr::const_iterator found = list.find(guid);
  if ( found != list.end() )
  {
    ret = (*found).second;
  }
  return ret;
}

class MyNxTriangle : public NxTriangle
{
public:
  HeU32 mFlags;
};

enum PathMakerState
{
  PMS_INVALID,
  PMS_PROCESS,
  PMS_GET_TRIANGLES,
  PMS_TRIANGULATE,
  PMS_VALIDATE_POINTS,
  PMS_GROW,
  PMS_DEATH_BLOSSOM,
  PMS_CONNECTIONS,
  PMS_BACKLINK_CONNECTIONS,
  PMS_COMPLETE,
};

static inline HeU32 computeHash(HeI32 x,HeI32 z)
{
  x+=32768;
  z+=32768;
  HE_ASSERT( x >= 0 && x < 65536 );
  HE_ASSERT( z >= 0 && z < 65536 );

  HeU32 hash = (HeU32)z<<16|(HeU32)x;

  return hash;
}

class BuildAIPoint
{
public:
  BuildAIPoint(void)
  {
    mGuid = 0;
    mPos[0] = 0;
    mPos[1] = 0;
    mPos[2] = 0;
    mTotalY = 0;
    mX = 0;
    mZ = 0;
    mCount = 0;
    mFlags = 0;
  }

  BuildAIPoint(const HeF32 *p,HeI32 ix,HeI32 iz,HeU32 flags)
  {
    mGuid   = 0;
    mPos[0] = p[0];
    mPos[1] = p[1];
    mPos[2] = p[2];
    mTotalY = p[1];
    mX      = ix;
    mZ      = iz;
    mCount  = 1;
    mFlags  = flags;
  }

  HeU32 getHash(void) const
  {
    return computeHash(mX,mZ);
  }

  bool samePoint(const BuildAIPoint &p,HeF32 deltaY)
  {
    bool same = false;

    if ( p.mFlags == mFlags )
    {
      HeF32 dy = fabsf(mPos[1]-p.mPos[1]);
      if ( dy < deltaY )
      {
        if ( p.mPos[1] >mPos[1] )
        {
          mPos[1] = p.mPos[1];
        }
        mTotalY+=p.mPos[1];
        mCount++;
        same = true;
      }
    }

    return same;
  }

  bool inside(const HeF32 *bmin,const HeF32 *bmax) const
  {
    return fm_insideAABB(mPos,bmin,bmax);
  }

  HeF32 getMeanY(void) const
  {
    return mTotalY / (HeF32)mCount;
  }

  bool verticalMatch(HeF32 y2,HeF32 vthresh) const
  {
    bool ret = false;
    HeF32 y1 = getMeanY();
    HeF32 diff = fabsf(y1-y2);
    if ( diff < vthresh )
      ret = true;
    return ret;
  }

  bool validate(NxScene *scene,HeF32 longEdge,HeF32 charWidth,HeF32 charHeight,HeF32 stepHeight,HeF32 walkSlope)
  {
#if DEBUG_TEST
    return true;
#else
    bool valid = gHBPhysics->isValidPoint(scene,mPos,longEdge,charWidth,charHeight,stepHeight,walkSlope,false,false,CGF_PATH_COLLISION);
#if 0
    if ( gBaseRenderDebug )
    {
      unsigned int color = 0xFF0000;
      if ( valid )
        color = 0x00FF00;
      gBaseRenderDebug->setRenderScale(gInverseGameScale);
      gBaseRenderDebug->DebugPoint(mPos,0.05f,color,600.0f);
    }
#endif

    return valid;
#endif
  }


  HeF32 mPos[3];
  HeF32 mTotalY;
  HeI32   mCount;         // number of points that map to this exact hash location..
  HeI32   mX;
  HeI32   mZ;
  HeI32   mGuid;         // a unique identifier
  HeU32   mFlags;
};


typedef SimpleVector< BuildAIPoint > BuildAIPointVector;

class BuildAIPointHash
{
public:

  BuildAIPointHash(void)
  {
    mPoints.setTag("BuildAIPointHash::BuildAIPoint");
  }

  BuildAIPointHash(const BuildAIPoint &ap)
  {
    mPoints.setTag("BuildAIPointHash::BuildAIPoint");
    mPoints.push_back(ap);
  }

  bool getMatch(HeF32 y2,BuildAIPoint &match,HeI32 /*x*/,HeI32 /*z*/,const char *consumed,HeF32 vthresh,HeU32 flags)
  {
    bool ret = false;

    BuildAIPointVector::iterator i;
    for (i=mPoints.begin(); i!=mPoints.end(); ++i)
    {
      BuildAIPoint &ap = (*i);
      if ( !consumed[ap.mGuid] && ap.verticalMatch(y2,vthresh) && flags == ap.mFlags )
      {
        match = ap;
        ret = true;
        break;
      }
    }
    return ret;
  }

  void quickAdd(const BuildAIPoint &ap)
  {
    mPoints.push_back(ap);
  }

  bool addPoint(const BuildAIPoint &ap,HeF32 dy)
  {
    bool ret = true;

    BuildAIPointVector::iterator i;
    for (i=mPoints.begin(); i!=mPoints.end(); ++i)
    {
      if ( (*i).samePoint(ap,dy) )
      {
        ret = false;
        break;
      }
    }

    if ( ret )
    {
      BuildAIPoint p = ap;
      mPoints.push_back(p);
    }
    return ret;
  }

  void getPointList(const HeF32 *bmin,const HeF32 *bmax,BuildAIPointVector &points) const
  {
    BuildAIPointVector::const_iterator i;
    for (i=mPoints.begin(); i!=mPoints.end(); i++)
    {
      const BuildAIPoint &p = (*i);
      if ( p.inside(bmin,bmax) )
      {
        points.push_back(p);
      }
    }
  }

  void validate(NxScene *scene,HeF32 longEdge,HeF32 charWidth,HeF32 charHeight,HeF32 stepHeight,HeF32 walkSlope,BuildAIPointVector &points)
  {
    BuildAIPointVector::iterator i;
    for (i=mPoints.begin(); i!=mPoints.end(); ++i)
    {
      bool valid = (*i).validate(scene,longEdge,charWidth,charHeight,stepHeight,walkSlope);
      if ( valid )
      {
        points.push_back( (*i) );
      }
    }
    mPoints.clear();  // clear them out..
  }

  BuildAIPointVector mPoints;
};

enum AIScan
{
  AI_LEFT   = (1<<0),
  AI_RIGHT  = (1<<1),
  AI_TOP    = (1<<2),
  AI_BOTTOM = (1<<3),
};

class BuildAINode;
class PathTask;

class BuildAINode
{
public:
  BuildAINode(void)
  {
    mConnections.setTag("BuildAINode::mConnections");
    mConsidered.setTag("BuildAINode::mConsidered");
  }

  void getCenter(HeF32 *center) const
  {
    center[0] = ((mBmax[0]-mBmin[0])*0.5f)+mBmin[0];
    center[1] = ((mBmax[1]-mBmin[1])*0.5f)+mBmin[1];
    center[2] = ((mBmax[2]-mBmin[2])*0.5f)+mBmin[2];
  }

  HeF32 getRadius(void) const
  {
    HeF32 ret = fm_distance(mBmin,mBmax)*0.5f;
    return ret;
  }

  bool match(const BuildAINode &n) const
  {
    bool ret = false;

    HeF32 c1[3];
    HeF32 c2[3];

    getCenter(c1);
    c1[1] = mCenterY;
    n.getCenter(c2);
    c2[1] = n.mCenterY;

    HeF32 d = fm_distanceSquared(c1,c2);
    if ( d < (0.2f*0.2f) )
    {
      ret = true;
    }
    return ret;
  }

  bool contains(HeU32 index) const
  {
    bool ret = false;
    for (HeU32 i=0; i<mConnections.size(); i++)
    {
      if ( mConnections[i] == index )
      {
        ret = true;
        break;
      }
    }
    return ret;
  }

  bool considered(HeU32 index) const
  {
    bool ret = false;
    for (HeU32 i=0; i<mConsidered.size(); i++)
    {
      if ( mConsidered[i] == index )
      {
        ret = true;
        break;
      }
    }
    return ret;
  }

  void clear(void)
  {
    mConnections.clear();
    mConsidered.clear();
  }

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

  bool isWater(void) const
  {
    bool ret =  false;
    if ( hasNodeFlag(NF_IS_UNDERWATER) || hasNodeFlag(NF_IS_WATER) ) 
      ret = true;
    return ret;
  }

  HeU32              mFlags;
  HeF32              mBmin[3];
  HeF32              mBmax[3];
  HeF32              mCenterY;

  SimpleVector< HeU32 > mConnections;        // connections we have established
  SimpleVector< HeU32 > mConsidered;         // connections we considred but rejected.

};

typedef SimpleVector< BuildAINode > BuildAINodeVector;

class AIBuildNode
{
public:
  AIBuildNode(const BuildAIPoint &p)
  {

    mGuid = p.mGuid;

    mX  = p.mX;
    mY = p.mPos[1];
    mZ  = p.mZ;
    mFlags = p.mFlags;

  }

  ~AIBuildNode(void)
  {
  }

  void getNode(BuildAINode &n,HeF32 stepHeight,HeF32 walkSlope,NxScene *scene)
  {

    n.mFlags   = mFlags;

    n.mBmin[0] = mBmin[0];
    n.mBmin[1] = mBmin[1];
    n.mBmin[2] = mBmin[2];

    n.mBmax[0] = mBmax[0];
    n.mBmax[1] = mBmax[1];
    n.mBmax[2] = mBmax[2];

    n.mBmin[1]-=stepHeight;
    n.mBmax[1]+=stepHeight;

    n.mCenterY = (n.mBmax[1]-n.mBmin[1])*0.5f+n.mBmin[1];

    HeF32 center[3];

    center[0] = (n.mBmax[0]-n.mBmin[0])*0.5f+n.mBmin[0];
    center[1] = n.mCenterY = (n.mBmax[1]-n.mBmin[1])*0.5f+n.mBmin[1];
    center[2] = (n.mBmax[2]-n.mBmin[2])*0.5f+n.mBmin[2];

    if ( gHBPhysics )
    {
      HeF32 pos[3];

      pos[0] = center[0];
      pos[1] = n.mCenterY;
      pos[2] = center[2];

      HeF32 groundy;
      gHBPhysics->getGrounding(scene,pos,2.0f,walkSlope,groundy,0,0.3f,1.6f,false,false,(CollisionGroupFlag)(CGF_PATH_COLLISION | CGF_WATER));
      if ( groundy > n.mBmin[1] && groundy < n.mBmax[1] )
      {
        n.mCenterY = groundy;
      }
    }
  }

  bool grow(PathTask *points,HeF32 scale,HeF32 stepHeight,char *consumed,HeI32 minDepth,HeI32 maxDepth,HeU32 &ecount);

  inline HeF32 getPoint(HeI32 x,HeI32  z,HeF32 *altitude)
  {
    HeF32 ret = mY;

    HeI32 x1 = mX-(MAX_GRID_WIDTH/2);
    HeI32 z1 = mZ-(MAX_GRID_WIDTH/2);

    x-=x1;
    z-=z1;

    HE_ASSERT(x>=0 && x < MAX_GRID_WIDTH );
    HE_ASSERT(z>=0 && z < MAX_GRID_WIDTH );

    HeI32 index = z*MAX_GRID_WIDTH+x;

    if ( altitude[index] != FLT_MAX )
    {
      ret = altitude[index];
    }

    return ret;
  }

  bool setAltitude(HeF32 *altitude,const BuildAIPoint &match)
  {
    bool ret = false;

    HeI32 x1 = mX;
    HeI32 z1 = mZ;

    HeI32 x = match.mX;
    HeI32 z = match.mZ;

    x1-=(MAX_GRID_WIDTH/2);
    z1-=(MAX_GRID_WIDTH/2);

    x-=x1;
    z-=z1;

    HE_ASSERT(x>=0 && x < MAX_GRID_WIDTH );
    HE_ASSERT(z>=0 && z < MAX_GRID_WIDTH );

    HeI32 index = z*MAX_GRID_WIDTH+x;

    if ( altitude[index] == FLT_MAX )
    {
      altitude[index] = match.getMeanY();
      ret = true;
    }

    return ret;
  }


  HeU32   mFlags;
  HeI32   mGuid;
  HeI32   mX;
  HeI32   mZ;
  HeF32 mY;
  HeF32 mBmin[3];
  HeF32 mBmax[3];
};



typedef SimpleVector< AIBuildNode > AIBuildNodeVector;


class ChunkHeightfield : public PhysXHeightFieldInterface
{
public:
  ChunkHeightfield(int wid,
                   int hit,
                   const unsigned int *heightField,
                   float thickness,
                   float columnScale,
                   float rowScale,
                   float heightScale,
                   const NxMat34 &pose,
                   const NxBounds3 &bounds,
                   NxArray< MyNxTriangle > *triangles,
                   float slopeLimit,
                   HeU32 flags)
  {
    mSlopeLimit = slopeLimit;
    mFlags      = flags;
    mPose       = pose;
    mBounds     = bounds;
    mTriangles  = triangles;
    physXHeightFieldToMesh(wid,hit,heightField,thickness,columnScale,rowScale,heightScale,1,this);
  }

  void receiveHeightFieldMesh(unsigned int /*vcount*/,float *vertices,unsigned int tcount,unsigned int *indices)
  {
    for (HeU32 i=0; i<tcount; i++)
    {
      unsigned int i1 = indices[0];
      unsigned int i2 = indices[1];
      unsigned int i3 = indices[2];

      NxVec3 t1(&vertices[i1*3]);
      NxVec3 t2(&vertices[i2*3]);
      NxVec3 t3(&vertices[i3*3]);

      NxVec3 p1,p2,p3;

      mPose.multiply(t1,p1);
      mPose.multiply(t2,p2);
      mPose.multiply(t3,p3);

      MyNxTriangle t;
      t.verts[0].set(p1);
      t.verts[1].set(p2);
      t.verts[2].set(p3);
      t.mFlags = mFlags;
      NxVec3 normal;
      t.normal(normal);
      if ( normal.y > mSlopeLimit )
      {
        HeU32 acode = 0;
        fm_clipTestAABB(&mBounds.min.x,&mBounds.max.x,&p1.x,&p2.x,&p3.x,acode);
        if ( acode == 0 )
        {
          mTriangles->push_back(t);
        }
      }
      indices+=3;
    }
  }

  HeU32        mFlags;
  float        mSlopeLimit;
  NxMat34      mPose;
  NxBounds3    mBounds;
  NxArray< MyNxTriangle > *mTriangles;
};



class PathTask : public NxUserEntityReport< HeU32 >
{
public:
  PathTask(void)
  {
    mBuildNodes.setTag("PathTask:AIBuildNode");
    mPoints.setTag("PathTask:BuildAIPoint");
  };

  PathTask(const NxBounds3 &b,HeF32 slopeLimit,HeF32 longEdge,HeF32 charWidth,HeF32 charHeight,HeF32 stepHeight,HeF32 walkSlope,HeU32 minDepth,HeU32 maxDepth,HBPATHSYSTEM::PathBuildProperties *properties)
  {
    mProperties = properties;
    mBounds     = b;
    mBuildNodes.setTag("PathTask:AIBuildNode");
    mPoints.setTag("PathTask:BuildAIPoint");
    mLastState  = PMS_INVALID;
    mState      = PMS_GET_TRIANGLES;
    mSlopeLimit = slopeLimit;
    mLongEdge   = longEdge;
    mScale      = 1.0f / mLongEdge;
    mCharWidth  = charWidth;
    mCharHeight = charHeight;
    mStepHeight = stepHeight;
    mWalkSlope  = walkSlope;
    mMinDepth   = minDepth;
    mMaxDepth   = maxDepth;
    mPointCount = 0;
    mRandPool   = 0;
    mConsumed   = 0;
    mTriangleIndex = 0;

    mVerticalChange = mLongEdge*mSlopeLimit*2.0f;
    if ( mVerticalChange < mStepHeight )
      mVerticalChange = mStepHeight;
  }

  ~PathTask(void)
  {
    mBuildNodes.clear();
    mPoints.clear();

    delete mRandPool;
    MEMALLOC_FREE(mConsumed);

    IntPtr::iterator k;
    for (k=mHashTable.begin(); k != mHashTable.end(); ++k)
    {
      BuildAIPointHash *ah = (*k).second;
      delete ah;
    }
  }

  bool grow(NxScene * /*scene*/)
  {
    bool ret = true;

    HeU32 growCount = 0;

    bool shuffled;

    HeI32 select = mRandPool->get(shuffled);

    if ( shuffled )
    {
      if ( mBuildNodes.empty() )
      {
        ret = true;
      }
      else
      {
        ret = false;
        mBuildNodeIndex = 0;
        mState = PMS_DEATH_BLOSSOM;
      }
    }
    else
    {
      ret = false;
      if ( !mConsumed[select] )
      {
        BuildAIPoint &p = mPoints[select];
        AIBuildNode n(p);
        HeU32 ecount;
        bool valid = n.grow(this,mScale,mStepHeight,mConsumed,mMinDepth,mMaxDepth,ecount);
        growCount++;
        if ( valid )
        {
          mBuildNodes.push_back(n);
/***********
          for (HeU32 i=0; i<mPoints.size(); i++)
          {
            if ( mConsumed[i] == 1 )
            {
              mConsumed[i] = 2;
              BuildAIPoint &p = mPoints[i];
              gBaseRenderDebug->DebugPoint(p.mPos,0.05f,0x00FF00,60.0f);
            }
          }
************/
        }
      }
    }


    return ret;
  }

  bool deathBlossom(NxScene *scene,BuildAINodeVector &nodes)
  {
    bool ret = true;

    HeU32 buildCount = mBuildNodes.size();

    if ( mBuildNodeIndex < buildCount )
    {
      AIBuildNode &n = mBuildNodes[mBuildNodeIndex];

      BuildAINode node;

      n.getNode(node,mStepHeight,mWalkSlope,scene);

      HeF32 center[3];
      node.getCenter(center);
      center[1] = node.mCenterY;
      center[1]+=mStepHeight; //*1.5f; // need a slight fudge factor

#if DEBUG_TEST
      bool ok = true;
#else
      bool ok = gHBPhysics->deathBlossom(scene,center,20.0f,(CollisionGroupFlag)(CGF_PATH_COLLISION & ~CGF_WATER),45.0f,false,2);
#endif

      if ( ok )
      {
        nodes.push_back(node);
      }

      ret = false;
      mBuildNodeIndex++;
    }
    else
    {
      ret = true;
    }

    return ret;
  }

  bool process(NxScene *scene,BuildAINodeVector &nodes,bool echo) // return 'true' to indicate that the task is complete or 'false' to continue executing the task.
  {
    bool ret = true;

    HeU32 color = 0xFFFFFF;

    bool inform=false;

    if ( mState != mLastState )
    {
      inform = echo;
    }

    mLastState = mState;


    switch ( mState )
    {
      case PMS_GET_TRIANGLES:
        color = 0xFF0000;
        ret = getTriangles(scene);
        if ( inform ) SEND_TEXT_MESSAGE(0,"@PM:Getting triangles.");
        break;
      case PMS_TRIANGULATE:
        color = 0xFF00FF;
        ret = tesselateTriangles(scene);
        if ( inform ) SEND_TEXT_MESSAGE(0,"@PM:Tesselating triangles.");
        break;
      case PMS_VALIDATE_POINTS:
        color = 0x00FF00;
        ret = validatePoints(scene);
        if ( inform ) SEND_TEXT_MESSAGE(0,"@PM:Validating points.");
        break;
      case PMS_GROW:
        color = 0xFFFF00;
        ret = grow(scene);
        if ( inform ) SEND_TEXT_MESSAGE(0,"@PM:Growing nodes from seed points.");
        break;
      case PMS_DEATH_BLOSSOM:
        color = 0xFFFFFF;
        ret = deathBlossom(scene,nodes);
        if ( inform ) SEND_TEXT_MESSAGE(0,"@PM:Performing death blossom.");
        break;
    }

    if ( gBaseRenderDebug )
      gBaseRenderDebug->DebugBound(&mBounds.min.x,&mBounds.max.x,color);

    return ret;
  }

  bool validatePoints(NxScene *scene)
  {
    bool ret = true;

    {
      if ( mPointIterator != mHashTable.end() )
      {
        BuildAIPointHash *hash = (*mPointIterator).second;
        ret = false;
        hash->validate(scene,mLongEdge,mCharWidth,mCharHeight,mStepHeight,mWalkSlope,mPoints);
        mPointIterator++;
      }
      else
      {
        if ( mPoints.empty() )
        {
          ret = true;
        }
        else
        {
          mState = PMS_GROW;
          ret = false;

          mPointCount = mPoints.size();
          mConsumed   = (char *)MEMALLOC_MALLOC(sizeof(char)*(mPointCount));
          mRandPool = MEMALLOC_NEW(RandPool)(mPointCount,mPointCount);
          memset(mConsumed,0,mPointCount);

          // transfer to the 'clean points' hash table.
          for (HeU32 i=0; i<mPointCount; i++)
          {
            BuildAIPoint &ap = mPoints[i];
            ap.mGuid = i;
            BuildAIPointHash *ph = get(mHashTable,ap.getHash());
            if ( ph )
            {
              ph->quickAdd(ap);
            }
            else
            {
              ph = MEMALLOC_NEW(BuildAIPointHash)(ap);
              mHashTable[ ap.getHash() ] = ph;
            }

          }
        }
      }
    }

    return ret;
  }

  bool onEvent(HeU32 nbEntities,HeU32 *triangle)
  {
    bool ret = true;

    for (HeU32 i=0; i<nbEntities; i++)
    {
      HeU32 tindex = triangle[i];
      NxTriangle t;
      if ( mTriangleMesh )
        mTriangleMesh->getTriangle(t,0,0,tindex,true,true);
      else
        mHeightfieldMesh->getTriangle(t,0,0,tindex,true,true);
      NxVec3 normal;
      t.normal(normal);
      if ( normal.y > mSlopeLimit )
      {
        MyNxTriangle tri;
        tri.mFlags = mFlags;
        NxTriangle *dest = static_cast< NxTriangle * >(&tri);
        *dest = t;
        mTriangles.push_back(tri);
      }
    }

    return ret;
  }


  bool intersects(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3)
  {
    bool ret = true;
    HeU32 andCode;
    fm_clipTestAABB(&mBounds.min.x,&mBounds.max.x,p1,p2,p3,andCode);
    if ( andCode )
    {
      ret = false;
    }
    return ret;
  }

  #define MAXDEPTH 32

  void triangulate(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,HeF32 longEdge,HeI32 recurse,HeU32 flags,NxScene *scene)
  {

    if ( !intersects(p1,p2,p3) ) return;


  	bool split = false;
  	HeF32 l1,l2,l3;

    l1 = l2 = l3 = 0;

  	if ( recurse < MAXDEPTH )
  	{
  	  l1 = fm_distanceSquared(p1,p2);
    	l2 = fm_distanceSquared(p2,p3);
    	l3 = fm_distanceSquared(p3,p1);

      HeF32 long2 = longEdge*longEdge;

  	  if (  l1 > long2 || l2 > long2 || l3 > long2 )
  	  	split = true;

    }

    if ( split )
  	{
  		HeI32 edge;

  		if ( l1 >= l2 && l1 >= l3 )
  			edge = 0;
  		else if ( l2 >= l1 && l2 >= l3 )
  			edge = 1;
  		else
  			edge = 2;

			HeF32 split[3];

  		switch ( edge )
  		{
  			case 0:
  				{
            fm_lerp(p1,p2,split,0.5f);
            triangulate(p1,split,p3, longEdge, recurse+1, flags,scene );
            triangulate(split,p2,p3, longEdge, recurse+1, flags,scene );
  				}
  				break;
  			case 1:
  				{
            fm_lerp(p2,p3,split,0.5f);
            triangulate(p1,p2,split, longEdge, recurse+1, flags,scene );
            triangulate(p1,split,p3, longEdge, recurse+1, flags,scene );
  				}
  				break;
  			case 2:
  				{
  					fm_lerp(p3,p1,split,0.5f);
            triangulate(p1,p2,split, longEdge, recurse+1, flags,scene );
            triangulate(split,p2,p3, longEdge, recurse+1, flags,scene );
  				}
  				break;
  		}
  	}
  	else
  	{
      HeF32 pos[3];

      const HeF32 INV3=1.0f/3.0f;

      pos[0] = (p1[0]+p2[0]+p3[0])*INV3;
      pos[1] = (p1[1]+p2[1]+p3[1])*INV3;
      pos[2] = (p1[2]+p2[2]+p3[2])*INV3;

      // Snap X/Z to the integer grid location.

      HeI32 ix = (HeI32)((pos[0]*mScale)+0.5f);
      HeI32 iz = (HeI32)((pos[2]*mScale)+0.5f);

      pos[0] = (HeF32)ix*mLongEdge;
      pos[2] = (HeF32)iz*mLongEdge;

      if ( fm_insideAABB(pos,&mBounds.min.x,&mBounds.max.x) )
      {
        // once the point has been 'snapped' to a grid, it may no longer be a valid point!
#if 1
        HB_PHYSICS::RaycastRequest ray;
        ray.mRayStart[0] = pos[0];
        ray.mRayStart[1] = pos[1]+0.2f;
        ray.mRayStart[2] = pos[2];
        ray.mRayDir[0] = 0;
        ray.mRayDir[1] = -1;
        ray.mRayDir[2] = 0;
        ray.mRayDist = 0.4f*gInverseGameScale;
        ray.mRayStart[0]*=gInverseGameScale;
        ray.mRayStart[1]*=gInverseGameScale;
        ray.mRayStart[2]*=gInverseGameScale;
        ray.mGroupFlags = CGF_PATH_COLLISION;
        bool hit = gHBPhysics->raycastImmediate(scene, ray);
        if ( hit )
#else
        if ( 1 )
#endif
        {
          HeU32 _flags = flags;

          if ( flags != NF_IS_WATER )
          {
            HB_PHYSICS::RaycastRequest ray;
            ray.mRayStart[0] = pos[0];
            ray.mRayStart[1] = pos[1]+0.5f;
            ray.mRayStart[2] = pos[2];
            ray.mRayDir[0] = 0;
            ray.mRayDir[1] = 1;
            ray.mRayDir[2] = 0;
            ray.mRayDist = 1000.0f;
            ray.mRayStart[0]*=gInverseGameScale;
            ray.mRayStart[1]*=gInverseGameScale;
            ray.mRayStart[2]*=gInverseGameScale;
            ray.mGroupFlags = CGF_WATER;
            bool hit = gHBPhysics->raycastImmediate(scene, ray);
            if ( hit )
            {
              _flags = NF_IS_UNDERWATER;
            }
          }

          if ( (_flags == NF_IS_UNDERWATER) && mProperties->mDefault.mUnderwater == false )
          {
          }
          else
          {

            BuildAIPoint ap(pos,ix,iz,_flags);
            BuildAIPointHash *ah = get(mHashTable, ap.getHash() );

            if ( ah )
            {
              ah->addPoint(ap,mVerticalChange);
            }
            else
            {
              BuildAIPointHash *ah = MEMALLOC_NEW(BuildAIPointHash)(ap);
              mHashTable[ ap.getHash() ] = ah;
            }
          }
        }
      }
    }
  }

  bool getTriangles(NxScene *scene)
  {
    bool ret = true;

//***    gHBPhysics->coreDump(scene,"getTriangles.xml",true);

    mTriangles.clear();
    mTriangleIndex = 0;

    #define MAX_REQUEST 8192
    NxShape *found[MAX_REQUEST];

    HeU32 scount = scene->overlapAABBShapes(mBounds,NX_STATIC_SHAPES,MAX_REQUEST,found,0,CGF_PATH_COLLISION |CGF_WATER ,NULL,false);

    if ( scount )
    {
      for (HeU32 i=0; i<scount; i++)
      {

        NxShape *shape = found[i];

        mFlags = 0;

        NxTriangleMeshShape *tmesh = shape->isTriangleMesh();
        NxConvexShape       *cmesh = shape->isConvexMesh();
        NxHeightFieldShape  *hmesh = shape->isHeightField();

        NxActor &a = shape->getActor();
        const char *properties = a.getName();

        if ( properties )
        {
          HeU32 count;
          const char **kv = getKeyValues(properties,count);
          for (HeU32 i=0; i<count; i++)
          {
            const char *key = kv[0];
            const char *value = kv[1];
            if ( stricmp(key,"pathing_mode") == 0 )
            {
              if ( stricmp(value,"BLOCK_ONLY") == 0 )
              {
                tmesh = 0;
                cmesh = 0;
              }
              else if ( stricmp(value,"IGNORE") == 0  )
              {
                tmesh = 0;  // if we only 'block' then we just don't feed these points in to begin with!
                cmesh = 0;
              }
              else if ( stricmp(value,"AVOID1") == 0 )
              {
                mFlags = NF_AVOID_1;
              }
              else if ( stricmp(value,"AVOID2") == 0 )
              {
                mFlags = NF_AVOID_2;
              }
              else if ( stricmp(value,"AVOID3") == 0 )
              {
                mFlags = NF_AVOID_3;
              }
              else if ( stricmp(value,"AVOID4") == 0 )
              {
                mFlags = NF_AVOID_4;
              }
              else if ( stricmp(value,"PREFER1")==0)
              {
                mFlags = NF_PREFER_1;
              }
              else if ( stricmp(value,"PREFER2")==0)
              {
                mFlags = NF_PREFER_2;
              }
              else if ( stricmp(value,"PREFER3")==0)
              {
                mFlags = NF_PREFER_3;
              }
              else if ( stricmp(value,"PREFER4")==0)
              {
                mFlags = NF_PREFER_4;
              }
            }
            kv+=2;
          }
        }


        NxCollisionGroup group = shape->getGroup();

        if ( group == CG_NO_COLLIDE ||
             group == CG_IGNORE_FOR_PLAYER ||
             group == CG_IGNORE_FOR_NPC ||
             group == CG_RAYCAST_ONLY ||
             group == CG_IGNORE_FOR_PATH_FINDING ||
             group == CG_BLOCK_ONLY )
        {
          tmesh = 0;
          cmesh = 0;
          hmesh = 0;
        }


        if ( mFlags == 0 )
        {
          if ( group == CG_WATER )
            mFlags = NF_IS_WATER;
          else if ( group == CG_HEIGHTFIELD )
            mFlags = NF_IS_TERRAIN;
        }

        if ( tmesh || cmesh || hmesh )
        {

          bool ok = false;

          switch ( mFlags )
          {
            case 0:
              ok = mProperties->mDefault.mNonTerrain;
              break;
            case NF_IS_WATER:
              ok = mProperties->mDefault.mWater;
              break;
            case NF_IS_TERRAIN:
              ok = mProperties->mDefault.mTerrain;
              break;
            case NF_IS_UNDERWATER:
              ok = mProperties->mDefault.mUnderwater;
              break;
            case NF_AVOID_1:
              ok = mProperties->mDefault.mPathAvoid1;
              break;
            case NF_AVOID_2:
              ok = mProperties->mDefault.mPathAvoid2;
              break;
            case NF_AVOID_3:
              ok = mProperties->mDefault.mPathAvoid3;
              break;
            case NF_AVOID_4:
              ok = mProperties->mDefault.mPathAvoid4;
              break;
            case NF_PREFER_1:
              ok = mProperties->mDefault.mPathPrefer1;
              break;
            case NF_PREFER_2:
              ok = mProperties->mDefault.mPathPrefer2;
              break;
            case NF_PREFER_3:
              ok = mProperties->mDefault.mPathPrefer3;
              break;
            case NF_PREFER_4:
              ok = mProperties->mDefault.mPathPrefer4;
              break;
            default:
              HE_ASSERT(0);
              break;
          }
          if ( !ok )
          {
            tmesh = 0;
            cmesh = 0;
          }
        }


        if ( tmesh )
        {
          mTriangleMesh = tmesh;
          mHeightfieldMesh = 0;
          tmesh->overlapAABBTriangles(mBounds,NX_QUERY_WORLD_SPACE,this);
        }
        else if ( cmesh )
        {
   				NxConvexShapeDesc desc;
   				cmesh->saveToDesc(desc);
   				NxConvexMesh *meshData = desc.meshData;
   				if ( meshData )
   				{
   					NxConvexMeshDesc desc;
   					meshData->saveToDesc(desc);
            NxMat34 pose = shape->getGlobalPose();
            USER_STL::vector< NxVec3 > points;
   					const char *pscan = (const char *) desc.points;
            for (HeU32 i=0; i<desc.numVertices; i++)
            {
              const HeF32 *pos = (const HeF32 *) pscan;
              NxVec3 v(pos);
              NxVec3 t;
              pose.multiply(v,t);
              points.push_back(t);
              pscan+=desc.pointStrideBytes;
            }
   					const char *scan = (const char *) desc.triangles;
   					for (HeU32 i=0; i<desc.numTriangles; i++)
   					{
   						if ( desc.flags & NX_CF_16_BIT_INDICES )
   						{
   							const NxU16 *source = (const NxU16 *) scan;

                const HeF32 *p1 = &points[source[0]].x;
                const HeF32 *p2 = &points[source[1]].x;
                const HeF32 *p3 = &points[source[2]].x;

                MyNxTriangle t;
                t.verts[0].set(p1);
                t.verts[1].set(p2);
                t.verts[2].set(p3);
                t.mFlags = mFlags;
                NxVec3 normal;
                t.normal(normal);
                if ( normal.y > mSlopeLimit )
                {
                  HeU32 acode = 0;
                  fm_clipTestAABB(&mBounds.min.x,&mBounds.max.x,p1,p2,p3,acode);
                  if ( acode == 0 )
                  {
                    mTriangles.push_back(t);
                  }
                }
              }
  						else
   						{
   							const HeU32 *source = (const HeU32 *) scan;

                const HeF32 *p1 = &points[source[0]].x;
                const HeF32 *p2 = &points[source[1]].x;
                const HeF32 *p3 = &points[source[2]].x;

                MyNxTriangle t;
                t.verts[0].set(p1);
                t.verts[1].set(p2);
                t.verts[2].set(p3);
                t.mFlags = mFlags;
                NxVec3 normal;
                t.normal(normal);
                if ( normal.y > mSlopeLimit )
                {
                  HeU32 acode = 0;
                  fm_clipTestAABB(&mBounds.min.x,&mBounds.max.x,p1,p2,p3,acode);
                  if ( acode == 0 )
                  {
                    mTriangles.push_back(t);
                  }
                }
   						}
   						scan+=desc.triangleStrideBytes;
   					}
   				}
        }
        else if ( hmesh )
        {
#if 1
          mHeightfieldMesh = hmesh;
          mTriangleMesh = 0;
          hmesh->overlapAABBTriangles(mBounds,NX_QUERY_WORLD_SPACE,this);
#else
          NxHeightFieldShapeDesc desc;
          hmesh->saveToDesc(desc);
          NxHeightField *heightfield = desc.heightField;

  				NxHeightFieldDesc hdesc;
  				heightfield->saveToDesc(hdesc);

         	HeU32	size = heightfield->getNbRows()	*heightfield->getNbColumns() *heightfield->getSampleStride();

       		if ( size )
       		{
         	  HeU8 *tempSamples	=	MEMALLOC_NEW_ARRAY(HeU8,size)[size];

       		  heightfield->saveCells(tempSamples,	size);

            HeI32 wid    = (HeI32)heightfield->getNbColumns();
            HeI32 hit    = (HeI32)heightfield->getNbRows();

            HeU32 hole   = desc.holeMaterial;
            HeU32 stride = heightfield->getSampleStride();

            const HeU8 *scan = (const HeU8 *) tempSamples;

            NxMat34 pose = shape->getGlobalPose();

            ChunkHeightfield ch(wid,hit,(const unsigned int *)tempSamples,hdesc.thickness,desc.columnScale,desc.rowScale,desc.heightScale,pose,mBounds,&mTriangles,mSlopeLimit,mFlags);

            delete tempSamples;
          }
#endif
        }
      }
    }

    if ( mTriangles.empty() )
    {
      ret = true;
    }
    else
    {
      SEND_TEXT_MESSAGE(0,"@PM:Gathered %d triangles.", mTriangles.size() );
      mState = PMS_TRIANGULATE;
      ret = false;
    }

    return ret;
  }

  void getVertex(HeU32 index,NxVec3 &dest,const void *points,HeU32 stride,const NxMat34 &mat)
  {
    const char *scan = (const char *) points;
    scan+=(index*stride);
    const NxVec3 *source = (const NxVec3 *) scan;
    mat.multiply(*source,dest);
  }

  bool tesselateTriangles(NxScene *scene)
  {
    bool ret = false;

    if ( mTriangleIndex < mTriangles.size() )
    {
      MyNxTriangle &t = mTriangles[mTriangleIndex];
      triangulate(&t.verts[0].x,&t.verts[1].x,&t.verts[2].x,mLongEdge,0,t.mFlags,scene);
      mTriangleIndex++;
    }
    else
    {
      ret = true;

      mTriangles.clear();
#if DO_LOG
      SEND_TEXT_MESSAGE(0,"HashTable: %d in size.\r\n", mHashTable.size() );
#endif

      IntPtr::iterator found = mHashTable.begin();
      if ( found != mHashTable.end() )
      {
        BuildAIPointHash *hash = (*found).second;
        ret = false;
        mState = PMS_VALIDATE_POINTS;
        mPointIterator = mHashTable.begin();
        hash->validate(scene,mLongEdge,mCharWidth,mCharHeight,mStepHeight,mWalkSlope,mPoints);
      }
    }

    return ret;
  }


  bool validRelativePoint(HeI32 x1,HeI32 z1,HeF32 y2,BuildAIPoint &match,const char *consumed,HeU32 flags)
  {
    bool ret = false;

    HeI32 hash1 = computeHash(x1,z1);

    BuildAIPointHash *h1 = get(mHashTable,hash1);

    if ( h1 )
    {
      ret = h1->getMatch(y2,match,x1,z1,consumed,mVerticalChange,flags);
    }


    return ret;
  }

  BuildAIPoint & getPoint(HeI32 index)
  {
    return mPoints[index];
  }


  PathTask * GetNext(void) const { return mNext; };
  PathTask * GetPrevious(void) const { return mPrevious; };
  void       SetNext(PathTask *task) { mNext = task; };
  void       SetPrevious(PathTask *task) { mPrevious = task; };

private:
  PathTask                         *mNext;
  PathTask                         *mPrevious;
  NxTriangleMeshShape              *mTriangleMesh;
  NxHeightFieldShape               *mHeightfieldMesh;
  HBPATHSYSTEM::PathBuildProperties *mProperties;
  PathMakerState                    mState;
  PathMakerState                    mLastState;
  NxBounds3                         mBounds;
  HeU32                             mFlags;
  HeF32                             mSlopeLimit;
  HeF32                             mLongEdge;
  HeF32                             mScale;
  HeF32                             mCharWidth;
  HeF32                             mCharHeight;
  HeF32                             mStepHeight;
  HeF32                             mWalkSlope;
  HeF32                             mVerticalChange;
  IntPtr::iterator                  mPointIterator;
  IntPtr                            mHashTable;
  BuildAIPointVector                mPoints;
  RandPool                         *mRandPool;
  char                             *mConsumed;
  HeU32                             mPointCount;
  AIBuildNodeVector                 mBuildNodes;
  HeI32                             mMinDepth;
  HeI32                             mMaxDepth;
  HeU32                             mBuildNodeIndex;
  HeU32                             mTriangleIndex;
  NxArray< MyNxTriangle >           mTriangles;
};


bool AIBuildNode::grow(PathTask *points,HeF32 /*scale*/,HeF32 stepHeight,char *consumed,HeI32 minDepth,HeI32 maxDepth,HeU32 &ecount)
{

  bool ret = false;

  HeI32 scanFlags = 0;

  ecount = 0;

  HeI32 mX1 = mX;
  HeI32 mX2 = mX;

  HeI32 mZ1 = mZ;
  HeI32 mZ2 = mZ;

  HE_ASSERT( (maxDepth*maxDepth) < MAX_GRID_SIZE );

  HeI32 guids[MAX_GRID_SIZE];
  HeI32 gcount = 0;

  HeF32 altitude[MAX_GRID_SIZE];
  for (HeU32 i=0; i<MAX_GRID_SIZE; i++)
    altitude[i] = FLT_MAX;

  guids[gcount] = mGuid;
  gcount++;


  for (HeI32 i=1; i<maxDepth; i++)
  {
    // let's do the top row...

    if ( !(scanFlags & AI_TOP) )
    {
      HeI32 x1 = mX-i;
      HeI32 x2 = mX+i;

      if ( scanFlags & AI_LEFT )  x1 = mX1;
      if ( scanFlags & AI_RIGHT ) x2 = mX2;

      HeI32 z  = mZ-i;
      // ok.. see if that is valid...
      for (HeI32 x=x1; x<=x2; x++)
      {
        BuildAIPoint match;
        HeF32 y = getPoint(x,z-1,altitude);
        if ( !points->validRelativePoint(x,z,y,match,consumed,mFlags) )
        {
          scanFlags|=AI_TOP;
          break;
        }
        else
        {
          HE_ASSERT( consumed[match.mGuid] == 0);
          if ( setAltitude(altitude,match) )
          {
            guids[gcount++] = match.mGuid;
          }
        }
      }

      if ( !(scanFlags & AI_TOP) )
      {
        mZ1 = z;
      }

    }

    if ( !(scanFlags & AI_RIGHT) )
    {
      HeI32 z1 = mZ-i;
      HeI32 z2 = mZ+i;
      if ( scanFlags & AI_TOP )    z1 = mZ1;
      if ( scanFlags & AI_BOTTOM ) z2 = mZ2;
      HeI32 x = mX+i;
      for (HeI32 z=z1; z<=z2; z++)
      {
        BuildAIPoint match;
        BuildAIPoint p;
        HeF32 y = getPoint(x-1,z,altitude);
        if ( !points->validRelativePoint(x,z,y,match,consumed,mFlags) )
        {
          scanFlags|=AI_RIGHT;
          break;
        }
        else
        {
          HE_ASSERT(consumed[match.mGuid] == 0 );
          if ( setAltitude(altitude,match) )
          {
            guids[gcount++] = match.mGuid;
          }
        }
      }
      if ( !(scanFlags & AI_RIGHT) )
      {
        mX2 = x;
      }
    }

    if ( !(scanFlags & AI_BOTTOM) )
    {
      HeI32 x1 = mX-i;
      HeI32 x2 = mX+i;
      if ( scanFlags & AI_LEFT )  x1 = mX1;
      if ( scanFlags & AI_RIGHT ) x2 = mX2;
      HeI32 z  = mZ+i;
      for (HeI32 x=x1; x<=x2; x++)
      {
        BuildAIPoint match;
        BuildAIPoint p;
        HeF32 y = getPoint(x,z+1,altitude);
        if ( !points->validRelativePoint(x,z,y,match,consumed,mFlags) )
        {
          scanFlags|=AI_BOTTOM;
          break;
        }
        else
        {
          HE_ASSERT(consumed[match.mGuid] == 0 );
          if ( setAltitude(altitude,match) )
          {
            guids[gcount++] = match.mGuid;
          }
        }
      }
      if ( !(scanFlags & AI_BOTTOM) )
      {
        mZ2 = z;
      }
    }

    if ( !(scanFlags & AI_LEFT) )
    {
      HeI32 z1 = mZ-i;
      HeI32 z2 = mZ+i;
      if ( scanFlags & AI_TOP )    z1 = mZ1;
      if ( scanFlags & AI_BOTTOM ) z2 = mZ2;
      HeI32 x  = mX-i;
      for (HeI32 z=z1; z<=z2; z++)
      {
        BuildAIPoint match;
        BuildAIPoint p;
        HeF32 y = getPoint(x+1,z,altitude);
        if ( !points->validRelativePoint(x,z,y,match,consumed,mFlags) )
        {
          scanFlags|=AI_LEFT;
          break;
        }
        else
        {
          HE_ASSERT(consumed[match.mGuid] == 0 );
          if ( setAltitude(altitude,match) )
          {
            guids[gcount++] = match.mGuid;
          }
        }
      }
      if ( !(scanFlags & AI_LEFT) )
      {
        mX1 = x;
      }
    }

    HeI32 dx = (mX2-mX1)+1;
    HeI32 dz = (mZ2-mZ1)+1;

    if ( dx > maxDepth ) break;
    if ( dz > maxDepth ) break;

    HeI32 l1,l2;
    if ( dx > dz )
    {
      l1 = dx;
      l2 = dz;
    }
    else
    {
      l1 = dz;
      l2 = dx;
    }
    HeI32 diff = l1 - l2;
    if ( diff > (l1/4) )
    {
      break;
    }


    if ( scanFlags == (AI_LEFT | AI_RIGHT | AI_TOP | AI_BOTTOM ) )
      break;
  }

  HeI32 dx = (mX2-mX1)+1;
  HeI32 dz = (mZ2-mZ1)+1;

  if ( dx >= minDepth && dz >= minDepth )
  {
    bool first = true;

    for (HeI32 i=0; i<gcount; i++)
    {
      HeI32 guid = guids[i];

      BuildAIPoint &p = points->getPoint(guid);
      HE_ASSERT(p.mGuid == guid );

      HeU32 color = 0x00FF00;

      if ( p.mX < mX1 || p.mX > mX2 || p.mZ < mZ1 || p.mZ > mZ2 )
      {
        color = 0xFF0000;
      }
      else
      {
        ecount++;
        HE_ASSERT(consumed[p.mGuid] == 0 );
        consumed[p.mGuid] = 1;

        if ( first )
        {
          mBmin[0] = p.mPos[0];
          mBmin[1] = p.mPos[1];
          mBmin[2] = p.mPos[2];
          mBmax[0] = p.mPos[0];
          mBmax[1] = p.mPos[1];
          mBmax[2] = p.mPos[2];
          first = false;
        }
        else
        {
          if ( p.mPos[0] < mBmin[0] ) mBmin[0] = p.mPos[0];
          if ( p.mPos[1] < mBmin[1] ) mBmin[1] = p.mPos[1];
          if ( p.mPos[2] < mBmin[2] ) mBmin[2] = p.mPos[2];
          if ( p.mPos[0] > mBmax[0] ) mBmax[0] = p.mPos[0];
          if ( p.mPos[1] > mBmax[1] ) mBmax[1] = p.mPos[1];
          if ( p.mPos[2] > mBmax[2] ) mBmax[2] = p.mPos[2];
        }
      }

    }

    mBmin[1]-=stepHeight;
    mBmax[1]+=stepHeight;

    // ok..now time to compute the plane equation.
    HeF32 center[3];
    fm_getAABBCenter(mBmin,mBmax,center);

#if 0
    if ( gBaseRenderDebug )
    {
      gBaseRenderDebug->DebugBound(mBmin,mBmax,0xFFFFFF,60.0f);
    }
#endif

    ret = true;
  }


  return ret;
}


class AIPathMaker
{
public:

  AIPathMaker(NxScene *scene,const HeF32 *region,HeU32 minNodeSize,HeU32 maxNodeSize,const HBPATHSYSTEM::PathBuildProperties *properties)
  {
    if ( properties )
    {
      mProperties = *properties;
    }
    mFinalNodes.setTag("AIPathMaker::AIBuildNode");
    mSkipConnections = true;
    mVersionNumber = 0;
    init(scene,region,minNodeSize,maxNodeSize);
  }

  AIPathMaker(NxScene *scene,const HeF32 *eyePos,HeF32 showDistance,HeU32 minNodeSize,HeU32 maxNodeSize,HeI32 version_number,HeU64 timeStamp,const HBPATHSYSTEM::PathBuildProperties *properties)
  {
    if ( properties )
    {
      mProperties = *properties;
    }
    mFinalNodes.setTag("AIPathMaker::AIBuildNode");
    mSkipConnections = false;
    mVersionNumber = version_number;
    mTimeStamp = timeStamp;
    HeF32 sdist = showDistance*0.5f;

    HeF32 region[6];
    region[0] = eyePos[0] - sdist;
    region[1] = eyePos[1] - sdist;
    region[2] = eyePos[2] - sdist;
    region[3] = eyePos[0] + sdist;
    region[4] = eyePos[1] + sdist;
    region[5] = eyePos[2] + sdist;
    init(scene,region,minNodeSize,maxNodeSize);
  }

  void init(NxScene *scene,const HeF32 *region,HeU32 minNodeSize,HeU32 maxNodeSize)
  {

    mDetailLevel = 2;
    mMinNodeSize = minNodeSize;
    mMaxNodeSize = maxNodeSize;

    mMinNodeSize = mProperties.mDefault.mNodeMin;
    mMaxNodeSize = mProperties.mDefault.mNodeMax;

    mTasks.Set(100,100,10000,"AIPathMaker->PathTask",__FILE__,__LINE__);

    mScene           = scene;
    mState           = PMS_INVALID;
    mWalkSlope       = mProperties.mDefault.mCharacterWalkSlope;
    mSlopeLimit      = cosf(mWalkSlope*FM_DEG_TO_RAD);
    mCharWidth       = mProperties.mDefault.mCharacterWidth;
    mCharHeight      = mProperties.mDefault.mCharacterHeight;
    mStepHeight      = mProperties.mDefault.mCharacterStepHeight;

    mLongEdge        = mProperties.mDefault.mNodeGranularity;
    mNodeTree        = 0;
    mRenderScale     = 1;

    NxBounds3 cvolume;

    cvolume.min.x = region[0];
    cvolume.min.y = region[1];
    cvolume.min.z = region[2];

    cvolume.max.x = region[3];
    cvolume.max.y = region[4];
    cvolume.max.z = region[5];

    mOriginalBounds = cvolume;


    if ( mScene )
    {

      HeU32 nbActors = mScene->getNbActors();

      if ( nbActors )
      {
        mState = PMS_PROCESS;
        mBounds.setEmpty();
        NxActor **alist = mScene->getActors();
        for (HeU32 i=0; i<nbActors; i++)
        {
          NxActor *actor = alist[i];
          HeU32 c = actor->getNbShapes();
          NxShape *const*shapes = actor->getShapes();
          for (HeU32 j=0; j<c; j++)
          {
            NxShape *shape = shapes[j];
            NxBounds3 b;
            shape->getWorldBounds(b);
            mBounds.combine(b);
          }
        }


        if ( mBounds.min.x < cvolume.min.x ) mBounds.min.x = cvolume.min.x;
        if ( mBounds.min.y < cvolume.min.y ) mBounds.min.y = cvolume.min.y;
        if ( mBounds.min.z < cvolume.min.z ) mBounds.min.z = cvolume.min.z;

        if ( mBounds.max.x > cvolume.max.x ) mBounds.max.x = cvolume.max.x;
        if ( mBounds.max.y > cvolume.max.y ) mBounds.max.y = cvolume.max.y;
        if ( mBounds.max.z > cvolume.max.z ) mBounds.max.z = cvolume.max.z;

        HeF32 dx = mBounds.max.x - mBounds.min.x;
        HeF32 dy = mBounds.max.y - mBounds.min.y;
        HeF32 dz = mBounds.max.z - mBounds.min.z;


        if ( dx < 2 )
        {
          mBounds.min.x-=1;
          mBounds.max.x+=1;
          dx+=2;
        }

        if ( dy < 2 )
        {
          mBounds.min.y-=1;
          mBounds.max.y+=1;
          dy+=2;
        }

        if ( dz < 2 )
        {
          mBounds.min.z=1;
          mBounds.max.z+=1;
          dz+=2;
        }

        HeI32 stepx = (HeI32) dx / BUILD_GRID_SIZE;
        HeI32 stepz = (HeI32) dz / BUILD_GRID_SIZE;

        stepx++;
        stepz++;

        dx = dx / (HeF32) stepx;
        dz = dz / (HeF32) stepz;

#if DO_LOG
        SEND_TEXT_MESSAGE(0,"SceneBounds: (%0.4f,%0.4f,%0.4f) - (%0.4f,%0.4f,%0.4f)\r\n",
          cvolume.min.x,
          cvolume.min.y,
          cvolume.min.z,
          cvolume.max.x,
          cvolume.max.y,
          cvolume.max.z);

        SEND_TEXT_MESSAGE(0,"MapSize(%d,%d) for a total of %d tasks.\r\n", stepx, stepz, stepx*stepz);
#endif

        for (HeI32 z=0; z<stepz; z++)
        {
          for (HeI32 x=0; x<stepx; x++)
          {
            NxBounds3 b;

            b.min.x = mBounds.min.x + (HeF32)x*dx;
            b.min.y = mBounds.min.y;
            b.min.z = mBounds.min.z + (HeF32)z*dz;

            b.max.x = b.min.x+dx;
            b.max.y = mBounds.max.y;
            b.max.z = b.min.z+dz;

            b.min.x-=SLOP;
            b.min.z-=SLOP;

            b.max.x+=SLOP;
            b.max.z+=SLOP;


            if ( b.intersects(cvolume) )
            {
              PathTask *task = mTasks.GetFreeLink();
              new ( task ) PathTask(b,mSlopeLimit,mLongEdge,mCharWidth,mCharHeight,mStepHeight,mWalkSlope,mMinNodeSize,mMaxNodeSize,&mProperties);
            }

          }
        }
        mTaskCount = mTasks.GetUsedCount();
      }
    }

  }

  ~AIPathMaker(void)
  {

    HeI32 count = mTasks.Begin();
    for (HeI32 i=0; i<count; i++)
    {
      PathTask *pt = mTasks.GetNext();
      pt->~PathTask();
    }

    delete mNodeTree;
  }

  bool isConnections(void) const
  {
    bool ret = false;
    if ( mState == PMS_CONNECTIONS || mState == PMS_BACKLINK_CONNECTIONS ) ret = true;
    return false;
  }

  bool process(bool showbox,bool &connectionPhase,bool echo)
  {
    bool ret = true;

    if ( gBaseRenderDebug )
    {
      gBaseRenderDebug->setRenderScale(gInverseGameScale);
    }

    connectionPhase = false;

    if ( !gBaseRenderDebug ) showbox = false;

    HeI32 count = mTasks.Begin();

    if ( count )
    {
      ret = false;
      PathTask *task = mTasks.GetNext();
      bool kill = task->process(mScene,mFinalNodes,echo);
      if ( kill )
      {
        if ( echo )
          SEND_TEXT_MESSAGE(0,"@PM:Processing PathTask %d remaining.", count );
        task->~PathTask();
        mTasks.Release(task);
      }
    }
    else
    {
      switch ( mState )
      {
        case PMS_CONNECTIONS:
        case PMS_BACKLINK_CONNECTIONS:
          if ( showbox ) gBaseRenderDebug->DebugBound(&mBounds.min.x,&mBounds.max.x,0xFFFF00);
          ret = processConnections();
          break;
        case PMS_PROCESS:
          connectionPhase = true;
          if ( !mFinalNodes.empty() )
          {
            mState = PMS_CONNECTIONS;
            mNodeTree = MEMALLOC_NEW(KdTree);
            HeU32 count = mFinalNodes.size();

            RandPool r(count,count);
            for (HeU32 i=0; i<count; i++)
            {
              bool shuffled;
              HeU32 select = r.get(shuffled);
              BuildAINode &n = mFinalNodes[select];
              n.clear();
              HeF32 center[3];
              n.getCenter(center);
              HeF32 radius = n.getRadius();
              mNodeTree->add( center[0], center[1], center[2], radius, (void *) select );
            }
            mBuildConnectionIndex = 0;
            mBuildConnections = count;
            mConnectionCount = 0;
            ret = false;
          }
          break;
      }

    }

    if ( gBaseRenderDebug )
    {
      gBaseRenderDebug->setRenderScale(1);
    }

    return ret;
  }

  HeU32 rebuildTree(void)
  {
    delete mNodeTree;
    mNodeTree = MEMALLOC_NEW(KdTree);
    HeU32 count = mFinalNodes.size();
    RandPool r(count,count);
    for (HeU32 i=0; i<count; i++)
    {
      bool shuffled;
      HeU32 select = r.get(shuffled);
      BuildAINode &n = mFinalNodes[select];
      HeF32 center[3];
      n.getCenter(center);
      HeF32 radius = n.getRadius();
      mNodeTree->add( center[0], center[1], center[2], radius, (void *) select );
    }
    return mFinalNodes.size();
  }

  bool testConnection(BuildAINode &n,BuildAINode &to,HeU32 index,bool isWater)
  {

    bool ret = false;
    HeF32 epos[3];
    HeF32 p1[3];
    HeF32 p2[3];

    n.getCenter(p1);
    to.getCenter(p2);

    p1[1] = n.mCenterY;
    p2[1] = to.mCenterY;

    HeF32 d = fm_distanceSquared(p1,p2);

    bool ok = true;

    if ( isWater )
    {
      ok = gHBPhysics->canSwim(mScene,p1,p2,mCharWidth,mCharHeight,mStepHeight,mWalkSlope,epos,true,false,(CollisionGroupFlag)(CGF_PATH_COLLISION & ~CGF_WATER));
    }
    else if ( d > 0.2f*0.2f )
    {
#if DEBUG_TEST
      ok = true;
#else
      ok = gHBPhysics->canWalk(mScene,p1,p2,mCharWidth,mCharHeight,mStepHeight,mWalkSlope,epos,true,false,CGF_PATH_COLLISION);
#endif
    }

    if ( ok )
    {
      ret = true;
      n.mConnections.push_back(index);
      mConnectionCount++;
      HeF32 center[3];
      n.getCenter(center);
      center[1] = n.mCenterY+mStepHeight;
      HeF32 pos[3];
      to.getCenter(pos);
      pos[1] = to.mCenterY;
      pos[1]+=mStepHeight;

    }
    else
    {
      n.mConsidered.push_back(index); // remember that we at least considered it...
    }

    return ret;
  }

  bool processConnections(void)
  {
#if 0
    return true;
#else
    bool ret = true;

    bool finished;
    HeU32 ccount;
    continueBuildingConnections(ccount,finished);
    if ( !finished )
    {
      ret = false;
    }
    return ret;
#endif
  }

  HeU32    continueBuildingConnections(HeU32 &ccount,bool &finished)
  {

    finished = false;

    if ( mBuildConnections )
    {

      BuildAINode &n = mFinalNodes[mBuildConnectionIndex];

      HeF32 center[3];
      n.getCenter(center);

      if ( n.hasNodeFlag(NF_IS_WATER) || n.hasNodeFlag(NF_IS_UNDERWATER) )
      {
        KdTreeFindNode results[8];
        HeU32 found = mNodeTree->searchXZ(center,4.0f,8,results,0);
        if ( found )
        {
          for (HeU32 i=0; i<found; i++)
          {
            KdTreeNode *node = results[i].mNode;
            HeU32 index = (HeU32) node->getUserData();
            if ( index != mBuildConnectionIndex )
            {
              BuildAINode &to = mFinalNodes[index];
              bool test = true;

              if ( n.hasNodeFlag(NF_IS_WATER) && to.hasNodeFlag(NF_IS_WATER) )
                test = false;
              else if ( n.hasNodeFlag(NF_IS_UNDERWATER) && to.hasNodeFlag(NF_IS_UNDERWATER) )
                test = false;

              if ( test )
              {
                float tc[3];
                to.getCenter(tc);
                float dy = fabsf(center[1]-tc[1]);
                if ( dy > 2.0f )
                {
                  bool ok = testConnection(n,to,index,true);
                  if ( ok )
                    break;
                }
              }
            }
          }
        }
      }


      KdTreeFindNode results[MAX_SEARCH];

      HeU32 maxSearch = mProperties.mDefault.mConnectionSearchCount;
      if ( maxSearch > MAX_SEARCH )
        maxSearch = MAX_SEARCH;

      HeU32 found = mNodeTree->search(center,mProperties.mDefault.mConnectionSearchDistance,maxSearch,results,0);

      if ( found )
      {

        center[1] = n.mCenterY;

        for (HeU32 i=0; i<found; i++)
        {
          KdTreeNode *node = results[i].mNode;
          HeU32 index = (HeU32) node->getUserData();

          if ( index != mBuildConnectionIndex )
          {

            BuildAINode &to = mFinalNodes[index];

            HeF32 pos[3];
            to.getCenter(pos);
            pos[1] = to.mCenterY;

            bool ok = true;
            // ok..first make sure that this node does not intersect any previously found node.
            if ( !n.mConnections.empty() )
            {
              SimpleVector< HeU32 >::iterator i;
              for (i=n.mConnections.begin(); i!=n.mConnections.end(); ++i)
              {

                HeU32 index = (*i);

                BuildAINode &check = mFinalNodes[index];

                HeF32 time;

                HeF32 dx = check.mBmax[0] - check.mBmin[0];
                HeF32 dy = check.mBmax[1] - check.mBmin[1];
                HeF32 dz = check.mBmax[2] - check.mBmin[2];

                HeF32 cx = dx*0.5f + check.mBmin[0];
                HeF32 cy = dy*0.5f + check.mBmin[1];
                HeF32 cz = dz*0.5f + check.mBmin[2];

                dx = (dx*0.5f)*SEARCH_SLOP;
                dy = (dy*0.5f)*SEARCH_SLOP;
                dz = (dz*0.5f)*SEARCH_SLOP;

                HeF32 bmin[3];
                HeF32 bmax[3];

                bmin[0] = cx-dx;
                bmin[1] = cy-dy;
                bmin[2] = cz-dz;

                bmax[0] = cx+dx;
                bmax[1] = cy+dy;
                bmax[2] = cz+dz;

                bool intersects = fm_lineTestAABBXZ(center,pos,bmin,bmax,time);

                if ( intersects )
                {
                  ok = false;
                  break;
                }
              }
            }
            if ( ok )
            {
              testConnection(n,to,index,false);
              if ( n.mConnections.size() > mProperties.mDefault.mConnectionMax )
                break;
            }

          }
        }
      }


      mBuildConnections--;
      mBuildConnectionIndex++;

      if ( (mBuildConnectionIndex%100) == 0 )
      {
        SEND_TEXT_MESSAGE(0,"@PM:Processing Connections for %d of %d final nodes.", mBuildConnectionIndex, mFinalNodes.size() );
      }

      if ( mBuildConnections == 0 )
      {
//        mState = PMS_COMPLETE;
        mState = PMS_BACKLINK_CONNECTIONS;
        mBuildConnectionIndex = 0;
        delete mNodeTree;
        mNodeTree = 0;
      }
    }

    if ( mBuildConnections == 0 )
    {
       // ok...now..let's double check for non-bi-directional connections...
#if 1
      HeU32 i = mBuildConnectionIndex;
      BuildAINode &n = mFinalNodes[i];
      SimpleVector< HeU32 > connections = n.mConnections;
      for (HeU32 j=0; j<connections.size(); j++)
      {
        HeU32 index = connections[j];
        BuildAINode &to = mFinalNodes[index];
        if ( to.contains(i) ) // fine, it's bi-directional.
        {
        }
        else
        {
          if ( to.considered(i) ) // then we checked it already..
          {
          }
          else
          {
            // ok.. if someone has a link to us, we certainly want to determine if we can link back to them!
            bool isWater = false;

            if ( to.isWater() && n.isWater() )
              isWater = true;

            testConnection(to,n,i,isWater);
          }
        }
      }
#endif
      mBuildConnectionIndex++;
      if ( mBuildConnectionIndex == mFinalNodes.size() )
      {
        SEND_TEXT_MESSAGE(0,"@PM:Finished backlinking connections.");
        finished = true;
      }
    }

    ccount = mConnectionCount;

    return mBuildConnections;
  }

  void *        getPathData(HeU32 &len)
  {
    void *ret = 0;

    bool binary = true;

    FILE_INTERFACE *fph = fi_fopen("scratch","wmem",0,0);

    if ( fph )
    {
      if ( binary )
      {

        AIPathHeader header;
        header.mPath[0] = 'A';
        header.mPath[1] = 'I';
        header.mPath[2] = 'P';
        header.mPath[3] = 'A';
        header.mPath[4] = 'T';
        header.mPath[5] = 'H';
        header.mPath[6] = 'I';
        header.mPath[7] = 'D';
        header.mNodeCount = mFinalNodes.size();
        header.mVersionNumber = AIPATH_VERSION_NUMBER;
        header.mConnectionCount = 0;
        header.mSourceVersionNumber = mVersionNumber;
        header.mAssetTimeStamp = mTimeStamp;
        BuildAINodeVector::iterator i;
        for (i=mFinalNodes.begin(); i!=mFinalNodes.end(); i++)
        {
          BuildAINode &n = (*i);
          header.mConnectionCount+=n.mConnections.size();
        }

        header.mLongEdge = mLongEdge;
        header.mDetailLevel = mDetailLevel;
        header.mWalkSlope   = mWalkSlope;
        header.mStepHeight  = mStepHeight;
        header.mMinNodeSize = mMinNodeSize;
        header.mMaxNodeSize = mMaxNodeSize;
        header.mCharacterWidth   = mCharWidth;
        header.mCharacterHeight  = mCharHeight;

        fi_fwrite(&header,sizeof(AIPathHeader),1,fph);

        for (i=mFinalNodes.begin(); i!=mFinalNodes.end(); i++)
        {
          BuildAINode &n = (*i);

          HeF32 bmin[3];
          HeF32 bmax[3];

          bmin[0] = n.mBmin[0]*mRenderScale;
          bmin[1] = n.mBmin[1]*mRenderScale;
          bmin[2] = n.mBmin[2]*mRenderScale;

          bmax[0] = n.mBmax[0]*mRenderScale;
          bmax[1] = n.mBmax[1]*mRenderScale;
          bmax[2] = n.mBmax[2]*mRenderScale;
          HeF32 cy = n.mCenterY*mRenderScale;

          fi_fwrite(&n.mFlags, sizeof(n.mFlags),1,fph);
          fi_fwrite(bmin,sizeof(HeF32)*3,1,fph);
          fi_fwrite(bmax,sizeof(HeF32)*3,1,fph);
          fi_fwrite(&cy,sizeof(HeF32),1,fph);

          HeU32 cc = n.mConnections.size();
          fi_fwrite(&cc, sizeof(HeU32), 1, fph );
          for (HeU32 j=0; j<cc; j++)
          {
            HeU32 v = n.mConnections[j];
            fi_fwrite(&v,sizeof(HeU32),1,fph);
          }
        }
      }
      else
      {
        fi_fprintf(fph,"## Auto-Generated AI Navigation Graph - AI Wisdom 4 - by John W. Ratcliff mailto:jratcliff@infiniplex.net\r\n");
        fi_fprintf(fph,"## Navigation graph contains %d nodes and a total of %d connections.\r\n", mFinalNodes.size(), mConnectionCount );
        BuildAINodeVector::iterator i;
        for (i=mFinalNodes.begin(); i!=mFinalNodes.end(); i++)
        {
          BuildAINode &n = (*i);
          HeU32 ccount = n.mConnections.size();
          fi_fprintf(fph,"node%d bmin(%0.9f,%0.9f,%0.9f) bmax(%0.9f,%0.9f,%0.9f) centery(%0.9f) connect(", i,
             n.mBmin[0]*mRenderScale, n.mBmin[1]*mRenderScale, n.mBmin[2]*mRenderScale,
             n.mBmax[0]*mRenderScale, n.mBmax[1]*mRenderScale, n.mBmax[2]*mRenderScale,
             n.mCenterY*mRenderScale);
          for (HeU32 j=0; j<ccount; j++)
          {
            HeU32 connect = n.mConnections[j];
            fi_fprintf(fph,"%d",connect);
            if ( (j+1) < ccount ) fi_fprintf(fph,",");
          }
          fi_fprintf(fph,")\r\n");
        }
      }

      void *mret = fi_getMemBuffer(fph,&len);
      if ( mret )
      {
        ret = MEMALLOC_MALLOC(len);
        memcpy(ret,mret,len);
      }

      fi_fclose(fph);

    }

    return ret;

  }

  const char * getStatus(void)
  {
    mStatus.clear();

    if ( mTasks.GetUsedCount() )
    {
      addStatus(mStatus,"PathTask %s of %s :", formatNumber(mTaskCount-mTasks.GetUsedCount()), formatNumber(mTaskCount) );
      switch ( mState )
      {
        case PMS_INVALID:
          addStatus(mStatus,"Invalid PathMaker state");
          break;
        case PMS_PROCESS:
          addStatus(mStatus,"Processing");
          break;
        case PMS_GET_TRIANGLES:
          addStatus(mStatus,"Getting Triangles");
          break;
        case PMS_TRIANGULATE:
          addStatus(mStatus,"Performing triangulation");
          break;
        case PMS_VALIDATE_POINTS:
          addStatus(mStatus,"Validating points");
          break;
        case PMS_GROW:
          addStatus(mStatus,"Growing nodes");
          break;
        case PMS_DEATH_BLOSSOM:
          addStatus(mStatus,"Performing death blossom");
          break;
      }
    }
    else
    {
      if ( mState == PMS_BACKLINK_CONNECTIONS )
        addStatus(mStatus,"BackLinking node %d of %d.", mBuildConnectionIndex, mFinalNodes.size() );
      else
        addStatus(mStatus,"Building Connection %d of %d", mBuildConnectionIndex, mFinalNodes.size() );
    }
    return mStatus.c_str();
  }

  bool contains(const NxBounds3 &s,NxBounds3 &b) const
  {
    bool ret = false;
    if ( b.min.x >= s.min.x &&
         b.max.x <= s.max.x &&
         b.min.y >= s.min.y &&
         b.max.y <= s.max.y &&
         b.min.z >= s.min.z &&
         b.max.z <= s.max.x )
         ret = true;
    return ret;
  }

  HeU32 addNode(const HeF32 *center,const HeF32 *bmin,const HeF32 *bmax,HeU32 flags)
  {
    HeU32 ret;

    NxBounds3 b;
    b.min.set(bmin);
    b.max.set(bmax);

//    if ( contains(mOriginalBounds,b) ) // if this node intersects our bounding volume, we do *not* add it, as it is represented by the rebuild.
    if ( b.intersects(mOriginalBounds) ) // if this node intersects our bounding volume, we do *not* add it, as it is represented by the rebuild.
    {
      ret = 0xFFFFFFFF;
    }
    else
    {
      ret = mFinalNodes.size();
      BuildAINode ba;
      ba.mFlags   = flags;
      ba.mBmin[0] = bmin[0];
      ba.mBmin[1] = bmin[1];
      ba.mBmin[2] = bmin[2];
      ba.mBmax[0] = bmax[0];
      ba.mBmax[1] = bmax[1];
      ba.mBmax[2] = bmax[2];
      ba.mCenterY = center[1];
      mFinalNodes.push_back(ba);
    }
    return ret;
  }

  HeU32  addConnection(HeU32 c1,HeU32 c2)
  {
#ifdef _DEBUG
    HeU32 count = mFinalNodes.size();
    HE_ASSERT( c1 >= 0 && c1 < count );
    HE_ASSERT( c2 >= 0 && c2 < count );
#endif
    mFinalNodes[c1].mConnections.push_back(c2);
    mConnectionCount++;
    return mFinalNodes[c1].mConnections.size();
  }

private:
  NxScene                           *mScene;
  HeI32                              mVersionNumber;
  std::string                        mStatus;
  HeU64                              mTimeStamp;
  HeF32                              mWalkSlope;
  HeF32                              mCharWidth;
  HeF32                              mCharHeight;
  HeF32                              mSlopeLimit;      // cos(walkSlope)
  HeF32                              mStepHeight;
  HeF32                              mLongEdge;
  PathMakerState                     mState;
  NxBounds3                          mOriginalBounds;
  NxBounds3                          mBounds;
  BuildAINodeVector                  mFinalNodes;
  KdTree                            *mNodeTree;
  HeI32                              mTaskCount;
  Pool< PathTask >                   mTasks;
  HeU32                              mBuildConnections;
  HeU32                              mBuildConnectionIndex;
  HeU32                              mConnectionCount;
  HeF32                              mRenderScale;
  HeI32                              mMaxNodeSize;
  HeI32                              mMinNodeSize;
  HeI32                              mDetailLevel;
  bool                               mSkipConnections;
  HBPATHSYSTEM::PathBuildProperties  mProperties;

};




AIPathMaker * beginPathMaker(NxScene *scene,const HeF32 *eyePos,HeF32 showDistance,HeU32 minNodeSize,HeU32 maxNodeSize,HeI32 version_number,HeU64 timeStamp,const HBPATHSYSTEM::PathBuildProperties *properties)
{
  AIPathMaker *aip = MEMALLOC_NEW(AIPathMaker)(scene,eyePos,showDistance,minNodeSize,maxNodeSize,version_number,timeStamp,properties);
  return aip;
}

bool          processPathMaker(AIPathMaker *aip,bool showbox,bool &connectionPhase,bool echo)
{
  bool ret = aip->process(showbox,connectionPhase,echo);
  return ret;
}

void          releasePathMaker(AIPathMaker *aip)
{
  delete aip;
}

void *        doGetPathData(AIPathMaker *aip,HeU32 &len)
{
  return aip->getPathData(len);
}

bool          isConnections(AIPathMaker *aip)
{
  return aip->isConnections();
}

AIPathMaker * beginPathMaker(NxScene *scene,const HeF32 *region,HeU32 minNodeSize,HeU32 maxNodeSize,const HBPATHSYSTEM::PathBuildProperties *properties)
{
  AIPathMaker *aip = MEMALLOC_NEW(AIPathMaker)(scene,region,minNodeSize,maxNodeSize,properties);
  return aip;
}


HeU32  doAddNode(AIPathMaker *pmaker,const HeF32 *center,const HeF32 *bmin,const HeF32 *bmax,HeU32 flags)
{
  return pmaker->addNode(center,bmin,bmax,flags);
}


HeU32  doAddConnection(AIPathMaker *pmaker,HeU32 c1,HeU32 c2)
{
  return pmaker->addConnection(c1,c2);
}


HeU32  doRebuildTree(AIPathMaker *aip)
{
  return aip->rebuildTree();
}

void addStatus(std::string &status,const char *fmt,...)
{
	char wbuff[8192];
  wbuff[8191] = 0;
	_vsnprintf(wbuff,8191, fmt, (char *)(&fmt+1));
  std::string tmp = wbuff;
  status+=tmp;
}

const char *  doGetStatus(AIPathMaker *pmaker)
{
  return pmaker->getStatus();
}

};

