#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <vector>

#ifdef APEX_TOOLS
#include "AutoGeometry.h"
#include "UserMemAlloc.h"
#include "StanHull.h"
#else
#include "AutoGeometry.h"
#include "../snippets/UserMemAlloc.h"
#include "../snippets/StanHull.h"
#include "../snippets/JobSwarm.h"
#include "../snippets/FloatMath.h"
#endif

#pragma warning(disable:4100 4996)

namespace AUTO_GEOMETRY
{

class Vec3
{
public:
  Vec3(const float *pos)
  {
    x = pos[0];
    y = pos[1];
    z = pos[2];
  }
  float x;
  float y;
  float z;
};

typedef USER_STL::vector< Vec3 > Vec3Vector;

class MyHull : public SimpleHull
{
public:
  MyHull(void)
  {
    mValidHull = false;
  }

  ~MyHull(void)
  {
    release();
  }

  void release(void)
  {
    MEMALLOC_DELETE_ARRAY(unsigned int,mIndices);
    MEMALLOC_DELETE_ARRAY(float,mVertices);
    mIndices = 0;
    mVertices = 0;
    mTriCount = 0;
    mVertexCount = 0;
  }

  void addPos(const float *p)
  {
    Vec3 v(p);
    mPoints.push_back(v);
  }

  float generateHull(void)
  {
    release();
    fm_identity(mLocalTransform);
    if ( mPoints.size() >= 3 ) // must have at least 3 vertices to create a hull.
    {
      // now generate the convex hull.
      STAN_HULL::HullDesc desc((STAN_HULL::HullFlag)(STAN_HULL::QF_TRIANGLES | STAN_HULL::QF_SKIN_WIDTH),mPoints.size(),&mPoints[0].x,sizeof(float)*3);
      desc.mMaxVertices = 32;
      desc.mSkinWidth = 0.001f;

      STAN_HULL::HullLibrary h;
      STAN_HULL::HullResult result;
      STAN_HULL::HullError e = h.CreateConvexHull(desc,result);
      if ( e == STAN_HULL::QE_OK )
      {
        mTriCount = result.mNumFaces;
        mIndices  = MEMALLOC_NEW_ARRAY(unsigned int,mTriCount*3)[mTriCount*3];
        memcpy(mIndices,result.mIndices,sizeof(unsigned int)*mTriCount*3);
        mVertexCount = result.mNumOutputVertices;
        mVertices = MEMALLOC_NEW_ARRAY(float,mVertexCount*3)[mVertexCount*3];
        memcpy(mVertices,result.mOutputVertices,sizeof(float)*mVertexCount*3);
        mValidHull = true;
        mMeshVolume = fm_computeMeshVolume( mVertices, mTriCount, mIndices ); // compute the volume of this mesh.
        h.ReleaseResult(result);
      }
    }
    return mMeshVolume;
  }

  void inherit(MyHull &c)
  {
    Vec3Vector::iterator i;
    for (i=c.mPoints.begin(); i!=c.mPoints.end(); ++i)
    {
      mPoints.push_back( (*i) );
    }
    c.mPoints.clear();
    c.mValidHull = false;
    generateHull();
  }

  void setTransform(const SimpleBone &b,int bone_index)
  {
    mBoneName    = b.mBoneName;
    mBoneIndex   = bone_index;
    mParentIndex = b.mParentIndex;
    memcpy(mTransform,b.mTransform,sizeof(float)*16);
    if ( mVertexCount )
    {
      for (unsigned int i=0; i<mVertexCount; i++)
      {
        float *vtx = &mVertices[i*3];
        fm_transform(b.mInverseTransform,vtx,vtx); // inverse transform the point into bone relative object space
      }
    }
  }

  void computeOBB(void)
  {
    fm_computeBestFitOBB( (size_t)mVertexCount, (const float *)mVertices, sizeof(float)*3,mSides, mLocalTransform, true);
    mOBBVolume = mSides[0]*mSides[1]*mSides[2];
  }

  void computeSphere(void)
  {
    mRadius = fm_computeBestFitSphere( (size_t)mVertexCount, (const float *)mVertices, sizeof(float)*3, mCenter );
    mSphereVolume = fm_sphereVolume(mRadius);
  }


  bool        mValidHull;
  Vec3Vector  mPoints;
};

class MyAutoGeometry : public AutoGeometry, public JOB_SWARM::JobSwarmInterface
{
public:
  MyAutoGeometry(void)
  {
    mContext = 0;
    mSwarmJob = 0;
    mHulls = 0;
    mSimpleHulls = 0;
  }

  ~MyAutoGeometry(void)
  {
    assert( mSwarmJob == 0 );
    release();
  }

  void release(void)
  {
    MEMALLOC_DELETE_ARRAY(MyHull,mHulls);
    mHulls = 0;
    MEMALLOC_DELETE_ARRAY(SimpleHull *,mSimpleHulls);
    mSimpleHulls = 0;
  }

  #define MAX_BONE_COUNT 8

  void addBone(unsigned int bone,unsigned int *bones,unsigned int &bcount)
  {
    if ( bcount < MAX_BONE_COUNT )
    {
      bool found = false;

      for (unsigned int i=0; i<bcount; i++)
      {
        if ( bones[i] == bone )
        {
          found = true;
          break;
        }
      }
      if ( !found )
      {
        bones[bcount] = bone;
        bcount++;
      }
    }
  }

  #define MIN_WEIGHT 0.1f
  void addBones(const SimpleSkinnedVertex &v,unsigned int *bones,unsigned int &bcount)
  {
    if ( v.mWeight[0] >= MIN_WEIGHT ) addBone(v.mBone[0],bones,bcount);
    if ( v.mWeight[1] >= MIN_WEIGHT ) addBone(v.mBone[1],bones,bcount);
    if ( v.mWeight[2] >= MIN_WEIGHT ) addBone(v.mBone[2],bones,bcount);
    if ( v.mWeight[3] >= MIN_WEIGHT ) addBone(v.mBone[3],bones,bcount);
  }

  void addTri(const SimpleSkinnedVertex &v1,const SimpleSkinnedVertex &v2,const SimpleSkinnedVertex &v3,const SimpleBone *sbones)
  {
    unsigned int bcount = 0;
    unsigned int bones[MAX_BONE_COUNT];
    addBones(v1,bones,bcount);
    addBones(v2,bones,bcount);
    addBones(v3,bones,bcount);
    for (unsigned int i=0; i<bcount; i++)
    {
      addPos(v1.mPos, bones[i], sbones );
      addPos(v2.mPos, bones[i], sbones );
      addPos(v3.mPos, bones[i], sbones );
    }
  }

  virtual SimpleHull ** createCollisionVolumes(float collapse_percentage,
                                               unsigned int bone_count,
                                               const SimpleBone *bones,
                                               const SimpleSkinnedMesh *mesh,
                                               unsigned int &geom_count)
  {
    release();
    geom_count = 0;

    mHulls = MEMALLOC_NEW_ARRAY(MyHull,bone_count)[bone_count];

    for (unsigned int i=0; i<bone_count; i++)
    {
      const SimpleBone &b = bones[i];
      mHulls[i].setTransform(b,i);
    }

    unsigned int tcount = mesh->mVertexCount/3;

    for (unsigned int i=0; i<tcount; i++)
    {
      const SimpleSkinnedVertex &v1 = mesh->mVertices[i*3+0];
      const SimpleSkinnedVertex &v2 = mesh->mVertices[i*3+0];
      const SimpleSkinnedVertex &v3 = mesh->mVertices[i*3+0];
      addTri(v1,v2,v3,bones);
    }

    float totalVolume = 0;
    for (unsigned int i=0; i<bone_count; i++)
    {
      totalVolume+=mHulls[i].generateHull();
    }

    // ok.. now do auto-collapse of hulls...
    if ( collapse_percentage > 0 )
    {
      float ratio = collapse_percentage / 100.0f;
      for (int i=(int)(bone_count-1); i>=0; i--)
      {
        MyHull &h = mHulls[i];
        const SimpleBone &b = bones[i];
        if ( b.mParentIndex >= 0 )
        {
          MyHull &parent_hull = mHulls[b.mParentIndex];
          if ( h.mValidHull && parent_hull.mValidHull )
          {
            if ( h.mMeshVolume < (parent_hull.mMeshVolume*ratio) ) // if we are less than 1/3 the volume of our parent, copy our vertices to the parent..
            {
              parent_hull.inherit(h);
            }
          }
        }
      }
    }
    for (int i=0; i<(int)bone_count; i++)
    {
      MyHull &h = mHulls[i];
      if ( h.mValidHull )
        geom_count++;
    }

    if ( geom_count )
    {
      mSimpleHulls = MEMALLOC_NEW_ARRAY(SimpleHull *,geom_count)[geom_count];
      int index = 0;
      for (int i=0; i<(int)bone_count; i++)
      {
        MyHull *h = &mHulls[i];
        if ( h->mValidHull )
        {
          const SimpleBone &b = bones[i];
          h->setTransform(b,i);
          h->computeOBB();
          h->computeSphere();
          mSimpleHulls[index] = h;
          index++;
        }
      }
    }

    return mSimpleHulls;
  }

  void addPos(const float *p,int bone,const SimpleBone *bones)
  {
    switch ( bones[bone].mOption )
    {
      case BO_INCLUDE:
        mHulls[bone].addPos(p);
        break;
      case BO_EXCLUDE:
        break;
      case BO_COLLAPSE:
        {
          while ( bone >= 0 )
          {
            bone = bones[bone].mParentIndex;
            if ( bones[bone].mOption == BO_INCLUDE )
              break;
            else if ( bones[bone].mOption == BO_EXCLUDE )
            {
              bone = -1;
              break;
            }
          }
          if ( bone >= 0 )
          {
            mHulls[bone].addPos(p); // collapse into the parent
          }
        }
        break;
    }
  }


  virtual SimpleHull ** createCollisionVolumes(float collapse_percentage,unsigned int &geom_count)
  {
    SimpleHull **ret = 0;

    if ( !mVertices.empty() && !mBones.empty() )
    {
      SimpleSkinnedMesh mesh;
      mesh.mVertexCount = mVertices.size();
      mesh.mVertices    = &mVertices[0];
      ret = createCollisionVolumes(collapse_percentage,mBones.size(),&mBones[0],&mesh,geom_count);
      mVertices.clear();
      mBones.clear();
    }

    return ret;
  }

  virtual void addSimpleSkinnedTriangle(const SimpleSkinnedVertex &v1,const SimpleSkinnedVertex &v2,const SimpleSkinnedVertex &v3)
  {
    mVertices.push_back(v1);
    mVertices.push_back(v2);
    mVertices.push_back(v3);
  }

  virtual void addSimpleBone(const SimpleBone &_b)
  {
    SimpleBone b = _b;
    mBones.push_back(b);
  }

  virtual const char * stristr(const char *str,const char *key)  // case insensitive ststr
  {
  	assert( strlen(str) < 2048 );
  	assert( strlen(key) < 2048 );

  	char istr[2048];
  	char ikey[2048];

  	strncpy(istr,str,2048);
  	strncpy(ikey,key,2048);
  	strlwr(istr);
  	strlwr(ikey);

  	char *foo = strstr(istr,ikey);
  	if ( foo )
  	{
  		unsigned int loc = (unsigned int)(foo - istr);
  		foo = (char *)str+loc;
  	}

  	return foo;
  }

  virtual bool createCollisionVolumes(float collapse_percentage,JOB_SWARM::JobSwarmContext *context)
  {
    bool ret = true;

    mReady = false;
    mFinished = false;
    mContext = context;
    mCollapsePercentage = collapse_percentage;
    mSwarmJob = mContext->createSwarmJob(this,0,0);

    return ret;
  }

  virtual SimpleHull ** getResults(unsigned int &geom_count,bool &ready)
  {
    SimpleHull **ret = 0;
    geom_count = 0;
    ready = false;
    if  ( mReady && mFinished )
    {
      ret = mSimpleHulls;
      geom_count = mHullCount;
      ready = true;
    }
    return ret;
  }

  virtual void job_process(void *userData,int userId)    // RUNS IN ANOTHER THREAD!! MUST BE THREAD SAFE!
  {
    if ( !mVertices.empty() && !mBones.empty() )
    {
      mHullCount = 0;
      SimpleSkinnedMesh mesh;
      mesh.mVertexCount = mVertices.size();
      mesh.mVertices    = &mVertices[0];
      createCollisionVolumes(mCollapsePercentage,mBones.size(),&mBones[0],&mesh,mHullCount);
      mVertices.clear();
      mBones.clear();
    }
    mReady = true;
  }

  virtual void job_onFinish(void *userData,int userId)   // runs in primary thread of the context
  {
    mFinished = true;
    mSwarmJob = 0;
  }

  virtual void job_onCancel(void *userData,int userId)  // runs in primary thread of the context
  {
    mSwarmJob = 0;
    mFinished = true;
  }

  virtual bool cancel(void)
  {
    bool ret = true;
    if ( mSwarmJob )
    {
      mContext->cancel(mSwarmJob);
      ret = false;
    }
    return ret;
  }

  virtual bool isFinish(void) const 
  {
    return mFinished;
  }

private:
  typedef USER_STL::vector< SimpleBone > SimpleBoneVector;
  typedef USER_STL::vector< SimpleSkinnedVertex > SimpleSkinnedVertexVector;
  SimpleBoneVector mBones;
  SimpleSkinnedVertexVector mVertices;
  float                      mCollapsePercentage;
  bool                       mReady;
  bool                       mFinished;

  JOB_SWARM::SwarmJob        *mSwarmJob;
  JOB_SWARM::JobSwarmContext *mContext;

  unsigned int mHullCount;
  MyHull      *mHulls;
  SimpleHull **mSimpleHulls;

};

AutoGeometry * createAutoGeometry(void)
{
  MyAutoGeometry *g = MEMALLOC_NEW(MyAutoGeometry);
  return static_cast< AutoGeometry *>(g);
}

void           releaseAutoGeometry(AutoGeometry *g)
{
  MyAutoGeometry * m = static_cast<MyAutoGeometry *>(g);
  MEMALLOC_DELETE(MyAutoGeometry,m);
}


}; // end of namespace
