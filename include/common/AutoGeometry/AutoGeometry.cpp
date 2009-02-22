#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <vector>

#ifdef APEX_TOOLS
#include "../include/AutoGeometry.h"
#include "../include/UserMemAlloc.h"
#include "../include/StanHull.h"
#else
#include "AutoGeometry.h"
#include "../snippets/UserMemAlloc.h"
#include "../snippets/StanHull.h"
#endif

#pragma warning(disable:4100)

namespace AUTO_GEOMETRY
{
void  fm_transform(const float matrix[16],const float v[3],float t[3]) // rotate and translate this point
{
  if ( matrix )
  {
    float tx = (matrix[0*4+0] * v[0]) +  (matrix[1*4+0] * v[1]) + (matrix[2*4+0] * v[2]) + matrix[3*4+0];
    float ty = (matrix[0*4+1] * v[0]) +  (matrix[1*4+1] * v[1]) + (matrix[2*4+1] * v[2]) + matrix[3*4+1];
    float tz = (matrix[0*4+2] * v[0]) +  (matrix[1*4+2] * v[1]) + (matrix[2*4+2] * v[2]) + matrix[3*4+2];
    t[0] = tx;
    t[1] = ty;
    t[2] = tz;
  }
  else
  {
    t[0] = v[0];
    t[1] = v[1];
    t[2] = v[2];
  }
}
inline float det(const float *p1,const float *p2,const float *p3)
{
  return  p1[0]*p2[1]*p3[2] + p2[0]*p3[1]*p1[2] + p3[0]*p1[1]*p2[2] -p1[0]*p3[1]*p2[2] - p2[0]*p1[1]*p3[2] - p3[0]*p2[1]*p1[2];
}


float  fm_computeMeshVolume(const float *vertices,size_t tcount,const unsigned int *indices)
{
	float volume = 0;

	for (unsigned int i=0; i<tcount; i++,indices+=3)
	{
  	const float *p1 = &vertices[ indices[0]*3 ];
		const float *p2 = &vertices[ indices[1]*3 ];
		const float *p3 = &vertices[ indices[2]*3 ];
		volume+=det(p1,p2,p3); // compute the volume of the tetrahedran relative to the origin.
	}

	volume*=(1.0f/6.0f);
	if ( volume < 0 )
		volume*=-1;
	return volume;
}



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

  bool        mValidHull;
  Vec3Vector  mPoints;
};

class MyAutoGeometry : public AutoGeometry
{
public:
  MyAutoGeometry(void)
  {
    mHulls = 0;
    mSimpleHulls = 0;
  }

  ~MyAutoGeometry(void)
  {
    release();
  }

  void release(void)
  {
    MEMALLOC_DELETE_ARRAY(MyHull,mHulls);
    mHulls = 0;
    MEMALLOC_DELETE_ARRAY(SimpleHull *,mSimpleHulls);
    mSimpleHulls = 0;
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

    for (unsigned int i=0; i<mesh->mVertexCount; i++)
    {
      const SimpleSkinnedVertex &vtx = mesh->mVertices[i];
      addPos( vtx.mPos, vtx.mWeight[0], vtx.mBone[0], bones );
      addPos( vtx.mPos, vtx.mWeight[1], vtx.mBone[1], bones );
      addPos( vtx.mPos, vtx.mWeight[2], vtx.mBone[2], bones );
      addPos( vtx.mPos, vtx.mWeight[3], vtx.mBone[3], bones );
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
          mSimpleHulls[index] = h;
          index++;
        }
      }
    }

    return mSimpleHulls;
  }

  void addPos(const float *p,float w,int bone,const SimpleBone *bones)
  {
    if ( w > 0.1f ) // must be weighted at least 10% or higher to this vertex
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
            bone = bones[bone].mParentIndex;
            if ( bone > 0 )
            {
              mHulls[bone].addPos(p); // collapse into the parent
            }
          }
          break;
      }
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

  virtual void addSimpleSkinnedVertex(const SimpleSkinnedVertex &vtx)
  {
    mVertices.push_back(vtx);
  }

  virtual void addSimpleBone(const SimpleBone &b)
  {
    mBones.push_back(b);
  }

private:
  typedef USER_STL::vector< SimpleBone > SimpleBoneVector;
  typedef USER_STL::vector< SimpleSkinnedVertex > SimpleSkinnedVertexVector;
  SimpleBoneVector mBones;
  SimpleSkinnedVertexVector mVertices;

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
