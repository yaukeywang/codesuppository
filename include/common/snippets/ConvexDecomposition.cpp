#pragma warning(disable:4702)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <math.h>
#include <algorithm>

#include "UserMemAlloc.h"
#include "ConvexDecomposition.h"
#include "MeshIslandGeneration.h"
#include "cd_hull.h"
#include "FloatMath.h"
#include "SplitMesh.h"
#include "RemoveTjunctions.h"


#pragma warning(disable:4996)
#pragma warning(disable:4267)

#define SHOW_DEBUG 0

#if SHOW_DEBUG
#include "RenderDebug/RenderDebug.h"
#endif

static const NxF64 EPSILON=0.0001;


using namespace ConvexDecomposition;

typedef std::vector< NxU32 > UintVector;


namespace ConvexDecomposition
{

class Cdesc
{
public:
  ConvexDecompInterface *mCallback;
  NxF64                 mMasterVolume;
  bool                   mUseIslandGeneration;
  NxF64                 mMasterMeshVolume;
  NxU32           mMaxDepth;
  NxF64                 mConcavePercent;
  NxF64                 mMergePercent;
  NxF64                 mMeshVolumePercent;
};

template <class Type> class Vector3d
{
public:
	Vector3d(void) { };  // null constructor, does not inialize point.

	Vector3d(const Vector3d &a) // constructor copies existing vector.
	{
		x = a.x;
		y = a.y;
		z = a.z;
	};

	Vector3d(Type a,Type b,Type c) // construct with initial point.
	{
		x = a;
		y = b;
		z = c;
	};

	Vector3d(const NxF64 *t)
	{
		x = t[0];
		y = t[1];
		z = t[2];
	};

	Vector3d(const NxI32 *t)
	{
		x = t[0];
		y = t[1];
		z = t[2];
	};

  void Set(const NxF32 *p)
  {
    x = (Type)p[0];
    y = (Type)p[1];
    z = (Type)p[2];
  }

  void Set(const NxF64 *p)
  {
    x = (Type)p[0];
    y = (Type)p[1];
    z = (Type)p[2];
  }



	const Type* Ptr() const { return &x; }
	Type* Ptr() { return &x; }


	Type x;
	Type y;
	Type z;
};



#define WSCALE 4
#define CONCAVE_THRESH 0.05f


NxU32 getDebugColor(void)
{
	static NxU32 colors[8] =
	{
		0xFF0000,
	  0x00FF00,
		0x0000FF,
		0xFFFF00,
		0x00FFFF,
		0xFF00FF,
		0xFFFFFF,
		0xFF8040
	};

	static NxI32 count = 0;

	count++;

	if ( count == 8 ) count = 0;

	assert( count >= 0 && count < 8 );

	NxU32 color = colors[count];

  return color;

}

class Wpoint
{
public:
  Wpoint(const Vector3d<NxF64> &p,NxF64 w)
  {
    mPoint = p;
    mWeight = w;
  }

  Vector3d<NxF64> mPoint;
  NxF64           mWeight;
};

typedef std::vector< Wpoint > WpointVector;


class CTri
{
public:
	CTri(void) { };

  CTri(const NxF64 *p1,const NxF64 *p2,const NxF64 *p3,NxU32 i1,NxU32 i2,NxU32 i3)
  {
    mProcessed = 0;
    mI1 = i1;
    mI2 = i2;
    mI3 = i3;

  	mP1.Set(p1);
  	mP2.Set(p2);
  	mP3.Set(p3);
  	mPlaneD = fm_computePlane(mP1.Ptr(),mP2.Ptr(),mP3.Ptr(),mNormal.Ptr());
	}

  NxF64 Facing(const CTri &t)
  {
		NxF64 d = fm_dot(mNormal.Ptr(),t.mNormal.Ptr());
		return d;
  }

  // clip this line segment against this triangle.
  bool clip(const Vector3d<NxF64> &start,Vector3d<NxF64> &end) const
  {
    Vector3d<NxF64> sect;

    bool hit = fm_lineIntersectsTriangle(start.Ptr(), end.Ptr(), mP1.Ptr(), mP2.Ptr(), mP3.Ptr(), sect.Ptr() );

    if ( hit )
    {
      end = sect;
    }
    return hit;
  }

	bool Concave(const Vector3d<NxF64> &p,NxF64 &distance,Vector3d<NxF64> &n) const
	{
    fm_nearestPointInTriangle(p.Ptr(),mP1.Ptr(),mP2.Ptr(),mP3.Ptr(),n.Ptr());
    distance = fm_distance(p.Ptr(),n.Ptr());
		return true;
	}

	void addTri(NxU32 *indices,NxU32 i1,NxU32 i2,NxU32 i3,NxU32 &tcount) const
	{
		indices[tcount*3+0] = i1;
		indices[tcount*3+1] = i2;
		indices[tcount*3+2] = i3;
		tcount++;
	}

	NxF64 getVolume(ConvexDecompInterface *callback) const
	{
		NxU32 indices[8*3];


    NxU32 tcount = 0;

    addTri(indices,0,1,2,tcount);
    addTri(indices,3,4,5,tcount);

    addTri(indices,0,3,4,tcount);
    addTri(indices,0,4,1,tcount);

    addTri(indices,1,4,5,tcount);
    addTri(indices,1,5,2,tcount);

    addTri(indices,0,3,5,tcount);
    addTri(indices,0,5,2,tcount);

    const NxF64 *vertices = mP1.Ptr();

		if ( callback )
		{
			NxU32 color = getDebugColor();

			for (NxU32 i=0; i<tcount; i++)
			{
				NxU32 i1 = indices[i*3+0];
				NxU32 i2 = indices[i*3+1];
				NxU32 i3 = indices[i*3+2];

				const NxF64 *p1 = &vertices[ i1*3 ];
				const NxF64 *p2 = &vertices[ i2*3 ];
				const NxF64 *p3 = &vertices[ i3*3 ];

				callback->ConvexDebugTri(p1,p2,p3,color);

			}
		}

		NxF64 v = fm_computeMeshVolume(mP1.Ptr(), tcount, indices );

		return v;

	}

	NxF64 raySect(const Vector3d<NxF64> &p,const Vector3d<NxF64> &dir,Vector3d<NxF64> &sect) const
	{
		NxF64 plane[4];

    plane[0] = mNormal.x;
    plane[1] = mNormal.y;
    plane[2] = mNormal.z;
    plane[3] = mPlaneD;

		Vector3d<NxF64> dest;

    dest.x = p.x+dir.x*10000;
    dest.y = p.y+dir.y*10000;
    dest.z = p.z+dir.z*10000;


    fm_intersectPointPlane( p.Ptr(), dest.Ptr(), sect.Ptr(), plane );

    return fm_distance(sect.Ptr(),p.Ptr()); // return the intersection distance.

	}

  NxF64 planeDistance(const Vector3d<NxF64> &p) const
  {
		NxF64 plane[4];

    plane[0] = mNormal.x;
    plane[1] = mNormal.y;
    plane[2] = mNormal.z;
    plane[3] = mPlaneD;

		return fm_distToPlane(plane,p.Ptr());

  }

	bool samePlane(const CTri &t) const
	{
		const NxF64 THRESH = 0.001f;
    NxF64 dd = fabs( t.mPlaneD - mPlaneD );
    if ( dd > THRESH ) return false;
    dd = fabs( t.mNormal.x - mNormal.x );
    if ( dd > THRESH ) return false;
    dd = fabs( t.mNormal.y - mNormal.y );
    if ( dd > THRESH ) return false;
    dd = fabs( t.mNormal.z - mNormal.z );
    if ( dd > THRESH ) return false;
    return true;
	}

	bool hasIndex(NxU32 i) const
	{
		if ( i == mI1 || i == mI2 || i == mI3 ) return true;
		return false;
	}

  bool sharesEdge(const CTri &t) const
  {
    bool ret = false;
    NxU32 count = 0;

		if ( t.hasIndex(mI1) ) count++;
	  if ( t.hasIndex(mI2) ) count++;
		if ( t.hasIndex(mI3) ) count++;

    if ( count >= 2 ) ret = true;

    return ret;
  }

  void debug(NxU32 color,ConvexDecompInterface *callback)
  {
    callback->ConvexDebugTri( mP1.Ptr(), mP2.Ptr(), mP3.Ptr(), color );
    callback->ConvexDebugTri( mP1.Ptr(), mP1.Ptr(), mNear1.Ptr(), 0xFF0000 );
    callback->ConvexDebugTri( mP2.Ptr(), mP2.Ptr(), mNear2.Ptr(), 0xFF0000 );
    callback->ConvexDebugTri( mP2.Ptr(), mP3.Ptr(), mNear3.Ptr(), 0xFF0000 );
    callback->ConvexDebugPoint( mNear1.Ptr(), 0.01f, 0xFF0000 );
    callback->ConvexDebugPoint( mNear2.Ptr(), 0.01f, 0xFF0000 );
    callback->ConvexDebugPoint( mNear3.Ptr(), 0.01f, 0xFF0000 );
  }

  NxF64 area(void)
  {
		NxF64 a = mConcavity * fm_areaTriangle(mP1.Ptr(),mP2.Ptr(),mP3.Ptr());
    return a;
  }

  void addWeighted(WpointVector &list,ConvexDecompInterface * /* callback */)
  {

    Wpoint p1(mP1,mC1);
    Wpoint p2(mP2,mC2);
    Wpoint p3(mP3,mC3);

    Vector3d<NxF64> d1,d2,d3;

    fm_subtract(mNear1.Ptr(),mP1.Ptr(),d1.Ptr());
    fm_subtract(mNear2.Ptr(),mP2.Ptr(),d2.Ptr());
    fm_subtract(mNear3.Ptr(),mP3.Ptr(),d3.Ptr());

    fm_multiply(d1.Ptr(),WSCALE);
    fm_multiply(d2.Ptr(),WSCALE);
    fm_multiply(d3.Ptr(),WSCALE);

    fm_add(d1.Ptr(), mP1.Ptr(), d1.Ptr());
    fm_add(d2.Ptr(), mP2.Ptr(), d2.Ptr());
    fm_add(d3.Ptr(), mP3.Ptr(), d3.Ptr());

    Wpoint p4(d1,mC1);
    Wpoint p5(d2,mC2);
    Wpoint p6(d3,mC3);

    list.push_back(p1);
    list.push_back(p2);
    list.push_back(p3);

    list.push_back(p4);
    list.push_back(p5);
    list.push_back(p6);

  }

  Vector3d<NxF64>	mP1;
  Vector3d<NxF64>	mP2;
  Vector3d<NxF64>	mP3;
  Vector3d<NxF64> mNear1;
  Vector3d<NxF64> mNear2;
  Vector3d<NxF64> mNear3;
  Vector3d<NxF64> mNormal;
  NxF64           mPlaneD;
  NxF64           mConcavity;
  NxF64           mC1;
  NxF64           mC2;
  NxF64           mC3;
  NxU32    mI1;
  NxU32    mI2;
  NxU32    mI3;
  NxI32             mProcessed; // already been added...
};

typedef std::vector< CTri > CTriVector;

bool featureMatch(CTri &m,const CTriVector &tris,ConvexDecompInterface * /* callback */,const CTriVector & /* input_mesh */)
{

  bool ret = false;

  NxF64 neardot = 0.707f;

  m.mConcavity = 0;

	//gLog->Display("*********** FEATURE MATCH *************\r\n");
	//gLog->Display("Plane: %0.4f,%0.4f,%0.4f   %0.4f\r\n", m.mNormal.x, m.mNormal.y, m.mNormal.z, m.mPlaneD );
	//gLog->Display("*********************************************\r\n");

	CTriVector::const_iterator i;

	CTri nearest;

	for (i=tris.begin(); i!=tris.end(); ++i)
	{
		const CTri &t = (*i);


  	//gLog->Display("   HullPlane: %0.4f,%0.4f,%0.4f   %0.4f\r\n", t.mNormal.x, t.mNormal.y, t.mNormal.z, t.mPlaneD );

		if ( t.samePlane(m) )
		{
			//gLog->Display("*** PLANE MATCH!!!\r\n");
			ret = false;
			break;
		}

	  NxF64 dot = fm_dot(t.mNormal.Ptr(),m.mNormal.Ptr());

	  if ( dot > neardot )
	  {

      NxF64 d1 = t.planeDistance( m.mP1 );
      NxF64 d2 = t.planeDistance( m.mP2 );
      NxF64 d3 = t.planeDistance( m.mP3 );

      if ( d1 > 0.001f || d2 > 0.001f || d3 > 0.001f ) // can't be near coplaner!
      {

  	  	neardot = dot;

        Vector3d<NxF64> n1,n2,n3;

        t.raySect( m.mP1, m.mNormal, m.mNear1 );
        t.raySect( m.mP2, m.mNormal, m.mNear2 );
        t.raySect( m.mP3, m.mNormal, m.mNear3 );

				nearest = t;

	  		ret = true;
		  }

	  }
	}

	if ( ret )
	{
	  //gLog->Display("*********************************************\r\n");
  	//gLog->Display("   HullPlaneNearest: %0.4f,%0.4f,%0.4f   %0.4f\r\n", nearest.mNormal.x, nearest.mNormal.y, nearest.mNormal.z, nearest.mPlaneD );

		m.mC1 = fm_distance(m.mP1.Ptr(), m.mNear1.Ptr() );
		m.mC2 = fm_distance(m.mP2.Ptr(), m.mNear2.Ptr() );
		m.mC3 = fm_distance(m.mP3.Ptr(), m.mNear3.Ptr() );

		m.mConcavity = m.mC1;

		if ( m.mC2 > m.mConcavity ) m.mConcavity = m.mC2;
		if ( m.mC3 > m.mConcavity ) m.mConcavity = m.mC3;

    #if 0
		callback->ConvexDebugTri( m.mP1.Ptr(), m.mP2.Ptr(), m.mP3.Ptr(), 0x00FF00 );
		callback->ConvexDebugTri( m.mNear1.Ptr(), m.mNear2.Ptr(), m.mNear3.Ptr(), 0xFF0000 );

		callback->ConvexDebugTri( m.mP1.Ptr(), m.mP1.Ptr(), m.mNear1.Ptr(), 0xFFFF00 );
		callback->ConvexDebugTri( m.mP2.Ptr(), m.mP2.Ptr(), m.mNear2.Ptr(), 0xFFFF00 );
		callback->ConvexDebugTri( m.mP3.Ptr(), m.mP3.Ptr(), m.mNear3.Ptr(), 0xFFFF00 );
    #endif

	}
	else
	{
		//gLog->Display("No match\r\n");
	}

	//gLog->Display("*********************************************\r\n");
	return ret;
}

bool isFeatureTri(CTri &t,CTriVector &flist,NxF64 fc,ConvexDecompInterface * /* callback */,NxU32 /* color */)
{
  bool ret = false;

  if ( t.mProcessed == 0 ) // if not already processed
  {

    NxF64 c = t.mConcavity / fc; // must be within 80% of the concavity of the parent.

    if ( c > 0.85f )
    {
      // see if this triangle is a 'feature' triangle.  Meaning it shares an
      // edge with any existing feature triangle and is within roughly the same
      // concavity of the parent.
			if ( flist.size() )
			{
			  CTriVector::iterator i;
			  for (i=flist.begin(); i!=flist.end(); ++i)
			  {
				  CTri &ftri = (*i);
				  if ( ftri.sharesEdge(t) )
				  {
					  t.mProcessed = 2; // it is now part of a feature.
					  flist.push_back(t); // add it to the feature list.
//					  callback->ConvexDebugTri( t.mP1.Ptr(), t.mP2.Ptr(),t.mP3.Ptr(), color );
					  ret = true;
					  break;
          }
				}
			}
			else
			{
				t.mProcessed = 2;
				flist.push_back(t); // add it to the feature list.
//				callback->ConvexDebugTri( t.mP1.Ptr(), t.mP2.Ptr(),t.mP3.Ptr(), color );
				ret = true;
			}
    }
    else
    {
      t.mProcessed = 1; // eliminated for this feature, but might be valid for the next one..
    }

  }
  return ret;
}

NxF64 computeConcavity(NxU32 vcount,
                       const NxF64 *vertices,
                       NxU32 tcount,
                       const NxU32 *indices,
                       ConvexDecompInterface *callback,
                       NxF64 *plane,      // plane equation to split on
                       NxF64 &volume)
{


	NxF64 cret = 0;
	volume = 1;

	HullResult  result;
  HullLibrary hl;
  HullDesc    desc;

	desc.mMaxVertices = 256;
	desc.SetHullFlag(QF_TRIANGLES);


  desc.mVcount       = vcount;
  desc.mVertices     = vertices;
  desc.mVertexStride = sizeof(NxF64)*3;

  HullError ret = hl.CreateConvexHull(desc,result);

  if ( ret == QE_OK )
  {

		NxF64 bmin[3];
		NxF64 bmax[3];

    fm_computeBestFitAABB( result.mNumOutputVertices, result.mOutputVertices, sizeof(NxF64)*3, bmin, bmax );

		NxF64 dx = bmax[0] - bmin[0];
		NxF64 dy = bmax[1] - bmin[1];
		NxF64 dz = bmax[2] - bmin[2];

		Vector3d<NxF64> center;

		center.x = bmin[0] + dx*0.5f;
		center.y = bmin[1] + dy*0.5f;
		center.z = bmin[2] + dz*0.5f;

		volume = fm_computeMeshVolume( result.mOutputVertices, result.mNumFaces, result.mIndices );

#if 1
		// ok..now..for each triangle on the original mesh..
		// we extrude the points to the nearest point on the hull.
		const NxU32 *source = result.mIndices;

		CTriVector tris;

    for (NxU32 i=0; i<result.mNumFaces; i++)
    {
    	NxU32 i1 = *source++;
    	NxU32 i2 = *source++;
    	NxU32 i3 = *source++;

    	const NxF64 *p1 = &result.mOutputVertices[i1*3];
    	const NxF64 *p2 = &result.mOutputVertices[i2*3];
    	const NxF64 *p3 = &result.mOutputVertices[i3*3];

//			callback->ConvexDebugTri(p1,p2,p3,0xFFFFFF);

			CTri t(p1,p2,p3,i1,i2,i3); //
			tris.push_back(t);
		}

    // we have not pre-computed the plane equation for each triangle in the convex hull..

		NxF64 totalVolume = 0;

		CTriVector ftris; // 'feature' triangles.

		const NxU32 *src = indices;


    NxF64 maxc=0;


		{
      CTriVector input_mesh;
      {
		    const NxU32 *src = indices;
  			for (NxU32 i=0; i<tcount; i++)
  			{

      		NxU32 i1 = *src++;
      		NxU32 i2 = *src++;
      		NxU32 i3 = *src++;

      		const NxF64 *p1 = &vertices[i1*3];
      		const NxF64 *p2 = &vertices[i2*3];
      		const NxF64 *p3 = &vertices[i3*3];

   				CTri t(p1,p2,p3,i1,i2,i3);
          input_mesh.push_back(t);
        }
      }

      CTri  maxctri;

			for (NxU32 i=0; i<tcount; i++)
			{

    		NxU32 i1 = *src++;
    		NxU32 i2 = *src++;
    		NxU32 i3 = *src++;

    		const NxF64 *p1 = &vertices[i1*3];
    		const NxF64 *p2 = &vertices[i2*3];
    		const NxF64 *p3 = &vertices[i3*3];

 				CTri t(p1,p2,p3,i1,i2,i3);

				featureMatch(t, tris, callback, input_mesh );

				if ( t.mConcavity > CONCAVE_THRESH )
				{

          if ( t.mConcavity > maxc )
          {
            maxc = t.mConcavity;
            maxctri = t;
          }

  				NxF64 v = t.getVolume(0);
  				totalVolume+=v;
   				ftris.push_back(t);
   			}

			}
		}
   	fm_computeSplitPlane( vcount, vertices, tcount, indices, plane );
#endif

		cret = totalVolume;

	  hl.ReleaseResult(result);
  }


	return cret;
}





class FaceTri
{
public:
	FaceTri(void) { };

  FaceTri(const NxF64 *vertices,NxU32 i1,NxU32 i2,NxU32 i3)
  {
  	fm_copy3(&vertices[i1*3],mP1 );
  	fm_copy3(&vertices[i2*3],mP2 );
  	fm_copy3(&vertices[i3*3],mP3 );
  }

  NxF64	mP1[3];
  NxF64	mP2[3];
  NxF64 	mP3[3];
  NxF64  mNormal[3];

};





class CHull
{
public:
  CHull(const ConvexResult &result)
  {
    mResult = new ConvexResult(result);
    mVolume = fm_computeMeshVolume( result.mHullVertices, result.mHullTcount, result.mHullIndices );

    mDiagonal = fm_computeBestFitAABB( result.mHullVcount, result.mHullVertices, sizeof(NxF64)*3, mMin, mMax );

    NxF64 dx = mMax[0] - mMin[0];
    NxF64 dy = mMax[1] - mMin[1];
    NxF64 dz = mMax[2] - mMin[2];

    dx*=0.1f; // inflate 1/10th on each edge
    dy*=0.1f; // inflate 1/10th on each edge
    dz*=0.1f; // inflate 1/10th on each edge

    mMin[0]-=dx;
    mMin[1]-=dy;
    mMin[2]-=dz;

    mMax[0]+=dx;
    mMax[1]+=dy;
    mMax[2]+=dz;


  }

  ~CHull(void)
  {
    delete mResult;
  }

  bool overlap(const CHull &h) const
  {
    return fm_intersectAABB(mMin,mMax, h.mMin, h.mMax );
  }

  NxF64          mMin[3];
  NxF64          mMax[3];
	NxF64          mVolume;
  NxF64          mDiagonal; // long edge..
  ConvexResult  *mResult;
};

// Usage: std::sort( list.begin(), list.end(), StringSortRef() );
class CHullSort
{
	public:

	 bool operator()(const CHull *a,const CHull *b) const
	 {
		 return a->mVolume < b->mVolume;
	 }
};


typedef std::vector< CHull * > CHullVector;


class ConvexBuilder : public ConvexDecompInterface
{
public:
  ConvexBuilder(ConvexDecompInterface *callback)
  {
    mCallback = callback;
  };

  ~ConvexBuilder(void)
  {
    CHullVector::iterator i;
    for (i=mChulls.begin(); i!=mChulls.end(); ++i)
    {
      CHull *cr = (*i);
      delete cr;
    }
  }

	bool isDuplicate(NxU32 i1,NxU32 i2,NxU32 i3,
		               NxU32 ci1,NxU32 ci2,NxU32 ci3)
	{
		NxU32 dcount = 0;

		assert( i1 != i2 && i1 != i3 && i2 != i3 );
		assert( ci1 != ci2 && ci1 != ci3 && ci2 != ci3 );

		if ( i1 == ci1 || i1 == ci2 || i1 == ci3 ) dcount++;
		if ( i2 == ci1 || i2 == ci2 || i2 == ci3 ) dcount++;
		if ( i3 == ci1 || i3 == ci2 || i3 == ci3 ) dcount++;

		return dcount == 3;
	}

	void getMesh(const ConvexResult &cr,fm_VertexIndex *vc)
	{
		NxU32 *src = cr.mHullIndices;

		for (NxU32 i=0; i<cr.mHullTcount; i++)
		{
			size_t i1 = *src++;
			size_t i2 = *src++;
			size_t i3 = *src++;

			const NxF64 *p1 = &cr.mHullVertices[i1*3];
			const NxF64 *p2 = &cr.mHullVertices[i2*3];
			const NxF64 *p3 = &cr.mHullVertices[i3*3];
      bool newPos;
			i1 = vc->getIndex(p1,newPos);
			i2 = vc->getIndex(p2,newPos);
			i3 = vc->getIndex(p3,newPos);


		}
	}
#pragma warning(disable:4100)
	CHull * canMerge(CHull *a,CHull *b)
	{
#if 0
    return 0;
#else
    if ( !a->overlap(*b) ) return 0; // if their AABB's (with a little slop) don't overlap, then return.

    if ( mMergePercent < 0 ) return 0;

		assert( a->mVolume > 0 );
		assert( b->mVolume > 0 );

		CHull *ret = 0;

		// ok..we are going to combine both meshes into a single mesh
		// and then we are going to compute the concavity...

    fm_VertexIndex *vc = fm_createVertexIndex((NxF64)EPSILON,false);

    getMesh( *a->mResult, vc);
    getMesh( *b->mResult, vc);

		size_t vcount = vc->getVcount();
		const NxF64 *vertices = vc->getVerticesDouble();

    HullResult hresult;
    HullLibrary hl;
    HullDesc   desc;

  	desc.SetHullFlag(QF_TRIANGLES);

    desc.mVcount       = (NxU32)vcount;
    desc.mVertices     = vertices;
    desc.mVertexStride = sizeof(NxF64)*3;

    HullError hret = hl.CreateConvexHull(desc,hresult);

    if ( hret == QE_OK )
    {

      NxF64 combineVolume  = fm_computeMeshVolume( hresult.mOutputVertices, hresult.mNumFaces, hresult.mIndices );
			NxF64 sumVolume      = a->mVolume + b->mVolume;

      NxF64 percent = (sumVolume*100) / combineVolume;

      if ( percent >= (100.0f-mMergePercent)  )
      {
  			ConvexResult cr(hresult.mNumOutputVertices, hresult.mOutputVertices, hresult.mNumFaces, hresult.mIndices);
    		ret = new CHull(cr);
    	}
		}


		fm_releaseVertexIndex(vc);

		return ret;
#endif
	}

  bool combineHulls(void)
  {

  	bool combine = false;

		sortChulls(mChulls); // sort the convex hulls, largest volume to least...

		CHullVector output; // the output hulls...


    CHullVector::iterator i;

    for (i=mChulls.begin(); i!=mChulls.end() && !combine; ++i)
    {
      CHull *cr = (*i);

      CHullVector::iterator j;
      for (j=mChulls.begin(); j!=mChulls.end(); ++j)
      {
        CHull *match = (*j);

        if ( cr != match ) // don't try to merge a hull with itself, that be stoopid
        {

					CHull *merge = canMerge(cr,match); // if we can merge these two....

					if ( merge )
					{

						output.push_back(merge);


						++i;
						while ( i != mChulls.end() )
						{
							CHull *cr = (*i);
							if ( cr != match )
							{
  							output.push_back(cr);
  						}
							i++;
						}

						delete cr;
						delete match;
						combine = true;
						break;
					}
        }
      }

      if ( combine )
      {
      	break;
      }
      else
      {
      	output.push_back(cr);
      }

    }

		if ( combine )
		{
			mChulls.clear();
			mChulls = output;
			output.clear();
		}


    return combine;
  }

  NxU32 process(const DecompDesc &desc)
  {

  	NxU32 ret = 0;


    Cdesc cdesc;
		cdesc.mMaxDepth         = desc.mDepth;
		cdesc.mConcavePercent   = desc.mCpercent;
    cdesc.mMeshVolumePercent = desc.mVpercent;
		mMergePercent = cdesc.mMergePercent     = desc.mPpercent;
    cdesc.mCallback         = this;
    cdesc.mUseIslandGeneration = desc.mUseIslandGeneration;


    HullResult result;
    HullLibrary hl;
    HullDesc   hdesc;
  	hdesc.SetHullFlag(QF_TRIANGLES);
    hdesc.mVcount       = desc.mVcount;
    hdesc.mVertices     = desc.mVertices;
    hdesc.mVertexStride = sizeof(NxF64)*3;
    hdesc.mMaxVertices  = desc.mMaxVertices; // maximum number of vertices allowed in the output
    HullError eret = hl.CreateConvexHull(hdesc,result);

    if ( eret == QE_OK )
    {
	  	cdesc.mMasterVolume = fm_computeMeshVolume( result.mOutputVertices, result.mNumFaces, result.mIndices ); // the volume of the hull.
      cdesc.mMasterMeshVolume = fm_computeMeshVolume( desc.mVertices, desc.mTcount, desc.mIndices );


      const NxU32 *indices = desc.mIndices;
      size_t tcount               = desc.mTcount;

      RemoveTjunctions *rt = 0;
#if 0
      if ( desc.mRemoveTjunctions )
      {
        rt = createRemoveTjunctions();
        RemoveTjunctionsDesc rdesc;
        rdesc.mVcount = desc.mVcount;
        rdesc.mVerticesD = desc.mVertices;
        rdesc.mTcount = desc.mTcount;
        rdesc.mIndices = desc.mIndices;
        rt->removeTjunctions(rdesc);
        indices = rdesc.mIndicesOut;
        tcount = rdesc.mTcountOut;
      }
#endif

      if ( desc.mInitialIslandGeneration )
      {
        MeshIslandGeneration *mig = createMeshIslandGeneration();

        size_t icount = mig->islandGenerate(tcount, indices, desc.mVertices );

        NxF64 *scratch_vertices = new NxF64[desc.mVcount*3];

        for (size_t i=0; i<icount; i++)
        {
          size_t otcount;
          size_t *oindices = mig->getIsland(i,otcount);
          assert(oindices);
          if ( oindices )
          {
            size_t vcount = fm_copyUniqueVertices(desc.mVcount,desc.mVertices,scratch_vertices, otcount, oindices, oindices);
            doConvexDecomposition( vcount, scratch_vertices, otcount, oindices,cdesc,0);
          }
        }
        delete []scratch_vertices;
        releaseMeshIslandGeneration(mig);
      }
      else
      {
        doConvexDecomposition(desc.mVcount, desc.mVertices, tcount, indices, cdesc, 0);
      }

      if ( rt )
      {
        releaseRemoveTjunctions(rt);
      }


  		while ( combineHulls() ); // keep combinging hulls until I can't combine any more...

      CHullVector::iterator i;
      for (i=mChulls.begin(); i!=mChulls.end(); ++i)
      {
        CHull *cr = (*i);

  			// before we hand it back to the application, we need to regenerate the hull based on the
  			// limits given by the user.

  			const ConvexResult &c = *cr->mResult; // the high resolution hull...

        HullResult result;
        HullLibrary hl;
        HullDesc   hdesc;

      	hdesc.SetHullFlag(QF_TRIANGLES);

        hdesc.mVcount       = c.mHullVcount;
        hdesc.mVertices     = c.mHullVertices;
        hdesc.mVertexStride = sizeof(NxF64)*3;
        hdesc.mMaxVertices  = desc.mMaxVertices; // maximum number of vertices allowed in the output

        if ( desc.mSkinWidth > 0 )
        {
        	hdesc.mSkinWidth = desc.mSkinWidth;
        	hdesc.SetHullFlag(QF_SKIN_WIDTH); // do skin width computation.
        }

        HullError ret = hl.CreateConvexHull(hdesc,result);

        if ( ret == QE_OK )
        {
    			ConvexResult r(result.mNumOutputVertices, result.mOutputVertices, result.mNumFaces, result.mIndices);

  				r.mHullVolume = fm_computeMeshVolume( result.mOutputVertices, result.mNumFaces, result.mIndices ); // the volume of the hull.
          if ( mCallback )
          {
            mCallback->ConvexDecompResult(r);
          }
#if SHOW_DEBUG
          else
          {
            static NxF32 offset = 0;

            NxU32 colors[8] =
            {
              0xFF0000,
              0x00FF00,
              0x0000FF,
              0xFFFF00,
              0x00FFFF,
              0xFF00FF,
              0xFFFFFF,
              0x808080
            };

            static NxU32 cindex = 0;

            NxU32 color = colors[cindex];
            cindex++;
            if ( cindex == 8 ) cindex = 0;


            for (size_t i=0; i<r.mHullTcount; i++)
            {
              NxU32 i1 = r.mHullIndices[i*3+0];
              NxU32 i2 = r.mHullIndices[i*3+1];
              NxU32 i3 = r.mHullIndices[i*3+2];
              const NxF64 *_p1 = &r.mHullVertices[i1*3];
              const NxF64 *_p2 = &r.mHullVertices[i2*3];
              const NxF64 *_p3 = &r.mHullVertices[i3*3];
              NxF32 p1[3];
              NxF32 p2[3];
              NxF32 p3[3];
              fm_doubleToFloat3(_p1,p1);
              fm_doubleToFloat3(_p2,p2);
              fm_doubleToFloat3(_p3,p3);
              p1[0]+=offset;
              p2[0]+=offset;
              p3[0]+=offset;
              gRenderDebug->DebugSolidTri(p1,p2,p3,color,600000.0f);
              gRenderDebug->DebugTri(p1,p2,p3,0xFFFFFF,600000.0f);
            }

  //          offset+=2;


          }
#endif
        }


        delete cr;
      }

  		ret = mChulls.size();

      mChulls.clear();
    }

    return ret;
  }


	virtual void ConvexDebugTri(const NxF64 *p1,const NxF64 *p2,const NxF64 *p3,NxU32 color)
  {
    mCallback->ConvexDebugTri(p1,p2,p3,color);
  }

  virtual void ConvexDebugOBB(const NxF64 *sides, const NxF64 *matrix,NxU32 color)
  {
    mCallback->ConvexDebugOBB(sides,matrix,color);
  }
	virtual void ConvexDebugPoint(const NxF64 *p,NxF64 dist,NxU32 color)
  {
    mCallback->ConvexDebugPoint(p,dist,color);
  }

  virtual void ConvexDebugBound(const NxF64 *bmin,const NxF64 *bmax,NxU32 color)
  {
    mCallback->ConvexDebugBound(bmin,bmax,color);
  }

  virtual void ConvexDecompResult(ConvexResult &result)
  {
    CHull *ch = new CHull(result);
    if ( ch->mVolume > 0.00001f )
    {
		  mChulls.push_back(ch);
    }
    else
    {
      delete ch;
    }
  }

	void sortChulls(CHullVector &hulls)
	{
		std::sort( hulls.begin(), hulls.end(), CHullSort() );
	}

  bool addTri(fm_VertexIndex *vl,
              UintVector &list,
              const NxF64 *p1,
              const NxF64 *p2,
              const NxF64 *p3)
  {
    bool ret = false;

    bool newPos;
    NxU32 i1 = vl->getIndex(p1,newPos );
    NxU32 i2 = vl->getIndex(p2,newPos );
    NxU32 i3 = vl->getIndex(p3,newPos );

    // do *not* process degenerate triangles!

    if ( i1 != i2 && i1 != i3 && i2 != i3 )
    {

      list.push_back(i1);
      list.push_back(i2);
      list.push_back(i3);
      ret = true;
    }
    return ret;
  }

#if SHOW_DEBUG
  void debugMesh(const SPLIT_MESH::SimpleMeshDouble &mesh,NxU32 color,NxF64 offset,const NxF64 *plane,NxF64 poffset)
  {
    for (size_t i=0; i<mesh.mTcount; i++)
    {
      size_t i1 = mesh.mIndices[i*3+0];
      size_t i2 = mesh.mIndices[i*3+1];
      size_t i3 = mesh.mIndices[i*3+2];
      const NxF64 *p1 = &mesh.mVertices[i1*3];
      const NxF64 *p2 = &mesh.mVertices[i2*3];
      const NxF64 *p3 = &mesh.mVertices[i3*3];
      NxF32 fp1[3];
      NxF32 fp2[3];
      NxF32 fp3[3];
      fm_doubleToFloat3(p1,fp1);
      fm_doubleToFloat3(p2,fp2);
      fm_doubleToFloat3(p3,fp3);

      fp1[0]+=(NxF32)offset;
      fp2[0]+=(NxF32)offset;
      fp3[0]+=(NxF32)offset;

      fp1[0]+=(NxF32)(plane[0]*poffset);
      fp1[1]+=(NxF32)(plane[1]*poffset);
      fp1[2]+=(NxF32)(plane[2]*poffset);

      fp2[0]+=(NxF32)(plane[0]*poffset);
      fp2[1]+=(NxF32)(plane[1]*poffset);
      fp2[2]+=(NxF32)(plane[2]*poffset);

      fp3[0]+=(NxF32)(plane[0]*poffset);
      fp3[1]+=(NxF32)(plane[1]*poffset);
      fp3[2]+=(NxF32)(plane[2]*poffset);

      gRenderDebug->DebugSolidTri(fp1,fp2,fp3,color,30.0f);
      gRenderDebug->DebugTri(fp1,fp2,fp3,0xFFFFFF,30.0f);
    }
  }
#endif

  #pragma warning(disable:4702)



  void doConvexDecomposition(NxU32           vcount,
                             const NxF64           *vertices,
                             NxU32           tcount,
                             const NxU32    *indices,
                             const Cdesc            &cdesc,
                             NxU32           depth)

  {

    NxF64 plane[4];

    bool split = false;

    bool isCoplanar = fm_isMeshCoplanar(tcount,indices,vertices,true);

    if ( isCoplanar ) // we can't do convex decomposition on co-planar meshes!
    {
      // skipping co-planar mesh here...
    }
    else
    {
      if ( depth < cdesc.mMaxDepth )
      {
        if ( cdesc.mConcavePercent >= 0 )
        {
      		NxF64 volume;
      		NxF64 c = computeConcavity( vcount, vertices, tcount, indices, cdesc.mCallback, plane, volume );
      		NxF64 percent = (c*100.0f)/cdesc.mMasterVolume;
      		if ( percent > cdesc.mConcavePercent ) // if great than 5% of the total volume is concave, go ahead and keep splitting.
      		{
            split = true;
          }
        }


        NxF64 mvolume = fm_computeMeshVolume(vertices, tcount, indices );
        NxF64 mpercent = (mvolume*100.0f)/cdesc.mMasterMeshVolume;
        if ( mpercent < cdesc.mMeshVolumePercent )
        {
          split = false; // it's too tiny to bother with!
        }

        if ( split )
        {
          split = fm_computeSplitPlane(vcount,vertices,tcount,indices,plane);
        }

      }

      if ( depth >= cdesc.mMaxDepth || !split )
      {

        HullResult result;
        HullLibrary hl;
        HullDesc   desc;

      	desc.SetHullFlag(QF_TRIANGLES);

        desc.mVcount       = vcount;
        desc.mVertices     = vertices;
        desc.mVertexStride = sizeof(NxF64)*3;

        HullError ret = hl.CreateConvexHull(desc,result);

        if ( ret == QE_OK )
        {
    			ConvexResult r(result.mNumOutputVertices, result.mOutputVertices, result.mNumFaces, result.mIndices);
#if SHOW_DEBUG

          static NxF32 offset = 2;

          for (size_t i=0; i<tcount; i++)
          {
            NxU32 i1 = indices[i*3+0];
            NxU32 i2 = indices[i*3+1];
            NxU32 i3 = indices[i*3+2];
            const NxF64 *_p1 = &vertices[i1*3];
            const NxF64 *_p2 = &vertices[i2*3];
            const NxF64 *_p3 = &vertices[i3*3];
            NxF32 p1[3];
            NxF32 p2[3];
            NxF32 p3[3];
            fm_doubleToFloat3(_p1,p1);
            fm_doubleToFloat3(_p2,p2);
            fm_doubleToFloat3(_p3,p3);
            p1[0]+=offset;
            p2[0]+=offset;
            p3[0]+=offset;
            gRenderDebug->DebugSolidTri(p1,p2,p3,0xFF00FF,60.0f);
          }


          for (size_t i=0; i<r.mHullTcount; i++)
          {
            NxU32 i1 = r.mHullIndices[i*3+0];
            NxU32 i2 = r.mHullIndices[i*3+1];
            NxU32 i3 = r.mHullIndices[i*3+2];
            const NxF64 *_p1 = &r.mHullVertices[i1*3];
            const NxF64 *_p2 = &r.mHullVertices[i2*3];
            const NxF64 *_p3 = &r.mHullVertices[i3*3];
            NxF32 p1[3];
            NxF32 p2[3];
            NxF32 p3[3];
            fm_doubleToFloat3(_p1,p1);
            fm_doubleToFloat3(_p2,p2);
            fm_doubleToFloat3(_p3,p3);
            p1[0]+=offset;
            p2[0]+=offset;
            p3[0]+=offset;

            gRenderDebug->DebugTri(p1,p2,p3,0xFFFFFF,60.0f);
          }

          offset+=2;
  #endif
          cdesc.mCallback->ConvexDecompResult(r);

        }

        return;
      }

      SPLIT_MESH::SimpleMeshDouble mesh(vcount, tcount, vertices, (const size_t *)indices,0);

#if SHOW_DEBUG
  //    debugMesh(mesh,0xFFFFFF);

  //    NxF32 p[4];
  //    p[0] = (NxF32)plane[0];
  //    p[1] = (NxF32)plane[1];
  //    p[2] = (NxF32)plane[2];
  //    p[3] = (NxF32)plane[3];

  //    gRenderDebug->DebugPlane(p,2.0f,2.0f,0xFFFF00,30.0f);
#endif

      SPLIT_MESH::SimpleMeshDouble leftMesh;
      SPLIT_MESH::SimpleMeshDouble rightMesh;

      SPLIT_MESH::splitMesh(mesh,leftMesh,rightMesh,plane,false,false,true,false);

      leftMesh.mVcount  = fm_copyUniqueVertices( leftMesh.mVcount, leftMesh.mVertices, leftMesh.mVertices, leftMesh.mTcount, leftMesh.mIndices, leftMesh.mIndices );
      rightMesh.mVcount = fm_copyUniqueVertices( rightMesh.mVcount, rightMesh.mVertices, rightMesh.mVertices, rightMesh.mTcount, rightMesh.mIndices, rightMesh.mIndices );

#if SHOW_DEBUG
  //    static NxF64 offset=0;
  //    debugMesh(leftMesh,0xFFFF00,offset,plane,0.1);
  //    debugMesh(rightMesh,0x00FF00,offset,plane,-0.1);
  //    offset+=4;
#endif


      if ( leftMesh.mTcount )
      {
        if ( cdesc.mUseIslandGeneration )
        {
          MeshIslandGeneration *mig = createMeshIslandGeneration();
          size_t icount = mig->islandGenerate(leftMesh.mTcount, leftMesh.mIndices, leftMesh.mVertices );
          if ( icount == 1 )
          {
            doConvexDecomposition(leftMesh.mVcount, leftMesh.mVertices, leftMesh.mTcount,leftMesh.mIndices, cdesc, depth+1 );
          }
          else
          {

            NxF64 *scratch_vertices = new NxF64[leftMesh.mVcount*3];

            for (size_t i=0; i<icount; i++)
            {
              size_t otcount;
              size_t *oindices = mig->getIsland(i,otcount);
              assert(oindices);
              if ( oindices )
              {
                size_t vcount = fm_copyUniqueVertices( leftMesh.mVcount, leftMesh.mVertices, scratch_vertices, otcount, oindices, oindices );
                doConvexDecomposition( vcount, scratch_vertices, otcount, oindices, cdesc, depth+1);

              }
            }
            delete []scratch_vertices;
          }
          releaseMeshIslandGeneration(mig);

        }
        else
        {
          doConvexDecomposition(leftMesh.mVcount, leftMesh.mVertices, leftMesh.mTcount,leftMesh.mIndices,cdesc,depth+1);
        }
      }

      if ( rightMesh.mTcount )
      {
        if ( cdesc.mUseIslandGeneration )
        {
          MeshIslandGeneration *mig = createMeshIslandGeneration();
          size_t icount = mig->islandGenerate(rightMesh.mTcount, rightMesh.mIndices, rightMesh.mVertices );
          if ( icount == 1 )
          {
            doConvexDecomposition(rightMesh.mVcount, rightMesh.mVertices, rightMesh.mTcount,rightMesh.mIndices, cdesc, depth+1);
          }
          else
          {
            NxF64 *scratch_vertices = new NxF64[rightMesh.mVcount*3];
            for (size_t i=0; i<icount; i++)
            {
              size_t otcount;
              size_t *oindices = mig->getIsland(i,otcount);
              assert(oindices);
              if ( oindices )
              {
                size_t vcount = fm_copyUniqueVertices( rightMesh.mVcount, rightMesh.mVertices, scratch_vertices, otcount, oindices, oindices );

                doConvexDecomposition( vcount, scratch_vertices, otcount, oindices, cdesc, depth+1 );

              }
            }
            delete []scratch_vertices;
          }
          releaseMeshIslandGeneration(mig);
        }
        else
        {
          doConvexDecomposition(rightMesh.mVcount, rightMesh.mVertices, rightMesh.mTcount,rightMesh.mIndices, cdesc, depth+1);
        }
      }
    }
  }

NxF64          mMergePercent;
CHullVector     mChulls;
ConvexDecompInterface *mCallback;

};

NxU32 performConvexDecomposition(const DecompDesc &_desc)
{
	NxU32 ret = 0;


  // we cannot assume that the input mesh has been properly indexed
  // so we must re-index it here!

  fm_VertexIndex *vindex = fm_createVertexIndex( (NxF64) SPLIT_EPSILON, false );
  typedef std::vector< size_t > size_tVector;

  size_tVector indices;


  for (size_t i=0; i<_desc.mTcount; i++)
  {
    NxU32 i1 = _desc.mIndices[i*3+0];
    NxU32 i2 = _desc.mIndices[i*3+1];
    NxU32 i3 = _desc.mIndices[i*3+2];

    const NxF64 *p1 = &_desc.mVertices[i1*3];
    const NxF64 *p2 = &_desc.mVertices[i2*3];
    const NxF64 *p3 = &_desc.mVertices[i3*3];

    bool newp;
    i1 = vindex->getIndex(p1,newp);
    i2 = vindex->getIndex(p2,newp);
    i3 = vindex->getIndex(p3,newp);

    indices.push_back(i1);
    indices.push_back(i2);
    indices.push_back(i3);
  }

  DecompDesc desc = _desc;
  desc.mIndices = &indices[0];
  desc.mVertices = vindex->getVerticesDouble();
  desc.mVcount   = vindex->getVcount();

#if 0
  static size_t count = 0;
  count++;
  char scratch[512];
  sprintf(scratch,"decomp%02d.obj", count);
  FILE *fph = fopen(scratch,"wb");
  if ( fph )
  {
    for (size_t i=0; i<desc.mVcount; i++)
    {
      const NxF64 *p = &desc.mVertices[i*3];
      fprintf(fph,"v %0.9f %0.9f %0.9f\r\n", (NxF32)p[0], (NxF32)p[1], (NxF32)p[2]);
    }
    for (size_t i=0; i<desc.mTcount; i++)
    {
      size_t i1 = desc.mIndices[i*3+0];
      size_t i2 = desc.mIndices[i*3+1];
      size_t i3 = desc.mIndices[i*3+2];
      fprintf(fph,"f %d %d %d\r\n", i1+1, i2+1, i3+1);
    }
    fclose(fph);
  }
#endif

  ConvexBuilder cb(desc.mCallback);
  ret = cb.process(desc);

  fm_releaseVertexIndex(vindex);

  return ret;
}



};
