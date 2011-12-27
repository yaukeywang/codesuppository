#include "RaycastMesh.h"
#include "NxBounds3.h"
#include "NxMat34.h"
#include <vector>

#pragma warning(disable:4100)

namespace RAYCAST_MESH
{

typedef std::vector< RmUint > TriVector;

static RmReal computePlane(const RmReal *A,const RmReal *B,const RmReal *C,RmReal *n) // returns D
{
	RmReal vx = (B[0] - C[0]);
	RmReal vy = (B[1] - C[1]);
	RmReal vz = (B[2] - C[2]);

	RmReal wx = (A[0] - B[0]);
	RmReal wy = (A[1] - B[1]);
	RmReal wz = (A[2] - B[2]);

	RmReal vw_x = vy * wz - vz * wy;
	RmReal vw_y = vz * wx - vx * wz;
	RmReal vw_z = vx * wy - vy * wx;

	RmReal mag = sqrt((vw_x * vw_x) + (vw_y * vw_y) + (vw_z * vw_z));

	if ( mag < 0.000001f )
	{
		mag = 0;
	}
	else
	{
		mag = 1.0f/mag;
	}

	RmReal x = vw_x * mag;
	RmReal y = vw_y * mag;
	RmReal z = vw_z * mag;


	RmReal D = 0.0f - ((x*A[0])+(y*A[1])+(z*A[2]));

	n[0] = x;
	n[1] = y;
	n[2] = z;

	return D;
}


#define MAX_CLIP 64

// This is a general purpose CohenSutherland polygon clipper written in
// C++ and using STL.  This clipper is not inherently slow, but then again
// it is not necessarily blazingly fast.  It uses STL vectors to build
// the clipped polygon and return the result.
//
// This routine is presented to educate clearly the cohen sutherland
// clipping algorithm.  It clips against an arbitrary
// 3d axis aligned bounding region which you specify.
//
// It is fairly straightforward to change the Vector3d class to some
// other vertex format and easily use this routine to clip any kind
// of vertex data with any number of interpolants.
//
// This was written by John W. Ratcliff (jratcliff@verant.com) on
// August 10, 2000 and is relased into the public domain as part of
// the Code Snippet library on FlipCode.com

class Vec3d
{
public:
	Vec3d(void) { };
	Vec3d(const RmReal *p) { x = p[0]; y = p[1]; z = p[2]; };
	Vec3d(RmReal _x,RmReal _y,RmReal _z) { x = _x; y = _y; z = _z; };

	void set(RmReal _x,RmReal _y,RmReal _z) { x = _x; y = _y; z = _z; };
	void set(const RmReal *p) { x = p[0]; y = p[1]; z = p[2]; };


	RmReal x;
	RmReal y;
	RmReal z;
};



// Enumeration defining the 6 planes of the view frustum.
enum ClipPlane
{
	CP_TOP = 0,
	CP_BOTTOM,
	CP_LEFT,
	CP_RIGHT,
	CP_NEAR,
	CP_FAR,
	CP_LAST
};

enum ClipResult
{
	CR_INSIDE, // completely inside the frustum.
	CR_OUTSIDE, //completely outside the frustum.
	CR_PARTIAL, // was clipped.
	CR_LAST
};

// An intermediate vertex format which contains the cohen sutherland
// clipping codes.
class ClipVertex
{
public:
	ClipVertex(void) { };

	ClipVertex(const Vec3d &pos,int code)
	{
		mPos = pos;
		mClipCode = code;
	};


	// clip vertex between v1 and v2 on this plane..
	ClipVertex(const ClipVertex &v1,
		const ClipVertex &v2,
		ClipPlane p,
		RmReal edge); // the clipping boundary..


	void Set(const Vec3d &pos,int code)
	{
		mPos = pos;
		mClipCode = code;
	};

	const Vec3d& GetPos(void) const { return mPos; };


	int GetClipCode(void) const { return mClipCode; };
	void SetClipCode(int code) { mClipCode = code; };

	RmReal GetX(void) const { return mPos.x; };
	RmReal GetY(void) const { return mPos.y; };
	RmReal GetZ(void) const { return mPos.z; };

	int             mClipCode;
	Vec3d mPos;
};


class FrustumClipper
{
public:

	FrustumClipper(const RmReal *fmin,const RmReal *fmax)
	{
		SetFrustum(fmin,fmax);
	};

	FrustumClipper(void)
	{
		Vec3d minbound(-0.5f,-0.5f,-0.5f);
		Vec3d maxbound(+0.5f,+0.5f,+0.5f);
		SetFrustum(&minbound.x,&maxbound.x);
	};

	void SetFrustum(const RmReal *fmin,const RmReal *fmax)
	{
		mEdges[CP_LEFT]   = fmin[0];
		mEdges[CP_RIGHT]  = fmax[0];
		mEdges[CP_TOP]    = fmin[1];
		mEdges[CP_BOTTOM] = fmax[1];
		mEdges[CP_NEAR]   = fmin[2];
		mEdges[CP_FAR]    = fmax[2];
	};

	// compute the cohen sutherland clipping bits for this 3d position
	// against the view frustum.
	inline int ClipCode(const Vec3d &pos) const;

	// compute the cohen sutherland clipping codes, and *also* accumulate
	// the or bits and the and bits for a series of point tests.
	inline int ClipCode(const Vec3d &pos,int &ocode,int &acode) const;

	// clips input polygon against the frustum.  Places output polygon
	// in 'output'.
	inline ClipResult Clip(const Vec3d *input, // input vertices.
		RmUint           vcount,   // input vertex count.
		Vec3d       *output,
		RmUint          &ocount) const;

	inline ClipResult ClipRay(const Vec3d &r1a,
		const Vec3d &r2a,
		Vec3d &r1b,
		Vec3d &r2b);

private:
	RmReal mEdges[CP_LAST]; // define the clipping planes of the view frustum

};

int FrustumClipper::ClipCode(const Vec3d &pos) const
{
	int code = 0;

	// build cohen sutherland clip codes.
	if ( pos.x < mEdges[CP_LEFT]   ) code|=(1<<CP_LEFT);
	if ( pos.x > mEdges[CP_RIGHT]  ) code|=(1<<CP_RIGHT);
	if ( pos.y < mEdges[CP_TOP]    ) code|=(1<<CP_TOP);
	if ( pos.y > mEdges[CP_BOTTOM] ) code|=(1<<CP_BOTTOM);
	if ( pos.z < mEdges[CP_NEAR]   ) code|=(1<<CP_NEAR);
	if ( pos.z > mEdges[CP_FAR]    ) code|=(1<<CP_FAR);

	return code;
}

int FrustumClipper::ClipCode(const Vec3d &pos,int &ocode,int &acode) const
{
	int code = ClipCode(pos);

	ocode|=code; // build or mask
	acode&=code; // build and mask

	return code;
}


ClipResult FrustumClipper::Clip(const Vec3d *polygon,RmUint in_count,Vec3d *dest,RmUint &ocount) const
{

	ocount    = 0;
	int ocode = 0;
	int acode = 0xFFFF;

	ClipVertex	list1[MAX_CLIP];
	ClipVertex  list2[MAX_CLIP];

	ClipVertex *input  = list1;
	ClipVertex *output = list2;

	for (RmUint i=0; i<in_count; i++)
	{
		input[i].Set( polygon[i], ClipCode(polygon[i], ocode, acode) );
	}

	if ( acode ) return CR_OUTSIDE; // points lie completely outside the frustum, no intersection of any kind

	if ( !ocode )
	{
		dest[0] = polygon[0];
		dest[1] = polygon[1];
		dest[1] = polygon[2];
		ocount = 3;
		return CR_INSIDE; // completely inside!
	}

	// ok..need to clip it!!

	RmUint l;
	l = CP_LAST;

	for (RmUint i=0; i<l; i++)
	{
		RmUint mask = (1<<i); // this is the clip mask.
		if ( ocode & mask ) // if any vertices are clipped against this plane
		{
			ocount    = 0;
			int new_ocode = 0;
			int new_acode = 0xFFFF;
			for (RmUint j=0; j<in_count; j++)
			{
				RmUint k = j+1;
				if ( k == in_count ) k = 0;
				ClipVertex &v1 = input[j];
				ClipVertex &v2 = input[k];
				// if this vertice is coming into or exiting out from this plane
				if ( (v1.GetClipCode() ^ v2.GetClipCode() ) & mask )
				{
					ClipVertex v0(v1,v2,(ClipPlane)i,mEdges[i]);
					int code = ClipCode(v0.GetPos(),new_ocode,new_acode);
					v0.SetClipCode(code);
					output[ocount] = v0;
					ocount++;
				}
				if ( ! (v2.GetClipCode() & mask ) )
				{
					output[ocount] = v2;
					ocount++;
					new_acode&=v2.GetClipCode();
				}
			}

			// Result of clipping produced no valid polygon *or* clipped result
			// is completely outside the frustum.
			if ( ocount < 3 || new_acode )
			{
				ocount = 0;
				return CR_OUTSIDE;
			}

			ClipVertex *temp = input;
			input = output;
			output = temp;
			in_count = ocount;
		}
	}

	for (RmUint i=0; i<in_count; i++)
	{
		dest[i] = input[i].mPos;
	}

	ocount = in_count;

	return CR_PARTIAL;
}


// clip vertex between v1 and v2 on this plane..
ClipVertex::ClipVertex(const ClipVertex &v1,
					   const ClipVertex &v2,
					   ClipPlane p,
					   RmReal edge)  // the clipping boundary..
{
	RmReal slope;

	switch ( p )
	{
	case CP_LEFT:
	case CP_RIGHT:
		slope   = (edge - v1.GetX() ) / (v2.GetX() - v1.GetX() );
		mPos.x = edge;
		mPos.y = v1.GetY() + ((v2.GetY() - v1.GetY()) * slope);
		mPos.z = v1.GetZ() + ((v2.GetZ() - v1.GetZ()) * slope);
		break;
	case CP_TOP:
	case CP_BOTTOM:
		slope   = (edge - v1.GetY() ) / (v2.GetY() - v1.GetY() );
		mPos.y = edge;
		mPos.x = v1.GetX() + ((v2.GetX() - v1.GetX()) * slope);
		mPos.z = v1.GetZ() + ((v2.GetZ() - v1.GetZ()) * slope);
		break;
	case CP_NEAR:
	case CP_FAR:
		slope   = (edge - v1.GetZ() ) / (v2.GetZ() - v1.GetZ() );
		mPos.z = edge;
		mPos.x = v1.GetX() + ((v2.GetX() - v1.GetX()) * slope);
		mPos.y = v1.GetY() + ((v2.GetY() - v1.GetY()) * slope);
		break;
	case CP_LAST:
	default:
		// Do nothing.
		break;
	}
}


ClipResult FrustumClipper::ClipRay(const Vec3d &r1a,
								   const Vec3d &r2a,
								   Vec3d &r1b,
								   Vec3d &r2b)
{
	int ocode = 0;
	int acode = 0xFFFF;

	ClipVertex ray1( r1a, ClipCode( r1a, ocode, acode ) );
	ClipVertex ray2( r2a, ClipCode( r2a, ocode, acode ) );

	if ( acode ) return CR_OUTSIDE; // points lie completely outside the frustum, no intersection of any kind

	if ( !ocode )
	{
		r1b = r1a;
		r2b = r2a;
		return CR_INSIDE; // completely inside!
	}


	int l;
	l = CP_LAST;

	for (int i=0; i<l; i++)
	{

		int mask = (1<<i); // this is the clip mask.

		if ( ocode & mask ) // if any vertices are clipped against this plane
		{
			// if this RAY is coming into or exiting out from this plane
			int new_acode = 0xFFFF;
			int new_ocode = 0;

			if ( (ray1.GetClipCode() ^ ray2.GetClipCode() ) & mask )
			{

				ClipVertex v0(ray1,ray2,(ClipPlane)i,mEdges[i]);
				int code = ClipCode(v0.GetPos(),new_ocode,new_acode);
				v0.SetClipCode(code);

				if ( ray1.GetClipCode() & mask )
				{
					ray1 = v0;
					new_acode&=ray2.GetClipCode();
				}
				else
				{
					ray2 = v0;
					new_acode&=ray1.GetClipCode();
				}

				if ( new_acode ) return CR_OUTSIDE;

			}
		}
	}


	r1b = ray1.GetPos();
	r2b = ray2.GetPos();

	return CR_PARTIAL;


}



#define TRI_EOF 0xFFFFFFFF

enum AxisAABB
{
	AABB_XAXIS,
	AABB_YAXIS,
	AABB_ZAXIS
};

enum ClipCode
{
	OLEFT	=	(1<<0),
	ORIGHT  =	(1<<1),
	OTOP	=	(1<<2),
	OBOTTOM	=	(1<<3),
	OFRONT	=	(1<<4),
	OBACK	=	(1<<5),
};

class BoundsAABB
{
public:


	void setMin(const RmReal *v)
	{
		mMin[0] = v[0];
		mMin[1] = v[1];
		mMin[2] = v[2];
	}

	void setMax(const RmReal *v)
	{
		mMax[0] = v[0];
		mMax[1] = v[1];
		mMax[2] = v[2];
	}

	void setMin(RmReal x,RmReal y,RmReal z)
	{
		mMin[0] = x;
		mMin[1] = y;
		mMin[2] = z;
	}

	void setMax(RmReal x,RmReal y,RmReal z)
	{
		mMax[0] = x;
		mMax[1] = y;
		mMax[2] = z;
	}

	RmUint clipTestXYZ(const RmReal *p) const
	{
		RmUint ocode = 0;
		if ( p[0] < mMin[0] ) ocode|=OLEFT;
		if ( p[0] > mMax[0] ) ocode|=ORIGHT;

		if ( p[1] < mMin[1] ) ocode|=OTOP;
		if ( p[1] > mMax[1] ) ocode|=OBOTTOM;

		if ( p[2] < mMin[2] ) ocode|=OFRONT;
		if ( p[2] > mMax[2] ) ocode|=OBACK;

		return ocode;
	};


	void include(const RmReal *v)
	{
		if ( v[0] < mMin[0] ) mMin[0] = v[0];
		if ( v[1] < mMin[1] ) mMin[1] = v[1];
		if ( v[2] < mMin[2] ) mMin[2] = v[2];

		if ( v[0] > mMax[0] ) mMax[0] = v[0];
		if ( v[1] > mMax[1] ) mMax[1] = v[1];
		if ( v[2] > mMax[2] ) mMax[2] = v[2];
	}

	void getCenter(RmReal *center) const
	{
		center[0] = (mMin[0]+mMax[0])*0.5f;
		center[1] = (mMin[1]+mMax[1])*0.5f;
		center[2] = (mMin[2]+mMax[2])*0.5f;
	}

	bool containsTriangle(const RmReal *p1,const RmReal *p2,const RmReal *p3) const
	{
		FrustumClipper clipper;
		clipper.SetFrustum(mMin,mMax);
		Vec3d verts[3];
		Vec3d outVerts[MAX_CLIP];

		verts[0] = Vec3d(p1);
		verts[1] = Vec3d(p2);
		verts[2] = Vec3d(p3);

		RmUint ocount;
		ClipResult result = clipper.Clip(verts,3,outVerts,ocount);
		return result != CR_OUTSIDE;
	}


	bool containsLineSegment(const RmReal *p1,const RmReal *p2,RmUint &acode) const
	{
		FrustumClipper clipper;
		clipper.SetFrustum(mMin,mMax);
		acode = 0;
		RmUint ocode1 = clipTestXYZ(p1);
		if ( !ocode1 ) return true;
		RmUint ocode2 = clipTestXYZ(p2);
		if ( !ocode2 ) return true;
		acode = ocode1 & ocode2;
		if ( acode ) return false;
		acode = acode;
		return true;
	}

	void clamp(const BoundsAABB &aabb)
	{
		if ( mMin[0] < aabb.mMin[0] ) mMin[0] = aabb.mMin[0];
		if ( mMin[1] < aabb.mMin[1] ) mMin[1] = aabb.mMin[1];
		if ( mMin[2] < aabb.mMin[2] ) mMin[2] = aabb.mMin[2];
		if ( mMax[0] > aabb.mMax[0] ) mMax[0] = aabb.mMax[0];
		if ( mMax[1] > aabb.mMax[1] ) mMax[1] = aabb.mMax[1];
		if ( mMax[2] > aabb.mMax[2] ) mMax[2] = aabb.mMax[2];
	}



	RmReal		mMin[3];
	RmReal		mMax[3];
};


class NodeAABB;

class NodeInterface
{
public:
	virtual NodeAABB * getNode(void) = 0;
	virtual void getFaceNormal(RmUint tri,float *faceNormal) = 0;
};





	class NodeAABB
	{
	public:
		NodeAABB(void)
		{
			mLeft = NULL;
			mRight = NULL;
			mTriIndices= NULL;
		}

		NodeAABB(RmUint vcount,const RmReal *vertices,RmUint tcount,RmUint *indices,
			RmUint maxDepth,	// Maximum recursion depth for the triangle mesh.
			RmUint minLeafSize,	// minimum triangles to treat as a 'leaf' node.
			RmReal	minAxisSize,
			NodeInterface *callback)	// once a particular axis is less than this size, stop sub-dividing.

		{
			mLeft = NULL;
			mRight = NULL;
			mTriIndices = NULL;
			TriVector triangles;
			triangles.reserve(tcount);
			for (RmUint i=0; i<tcount; i++)
			{
				triangles.push_back(i);
			}
			mBounds.setMin( vertices );
			mBounds.setMax( vertices );
			const RmReal *vtx = vertices+3;
			for (RmUint i=1; i<vcount; i++)
			{
				mBounds.include( vtx );
				vtx+=3;
			}
			split(triangles,vcount,vertices,tcount,indices,0,maxDepth,minLeafSize,minAxisSize,callback);
		}

		NodeAABB(const BoundsAABB &aabb)
		{
			mBounds = aabb;
			mLeft = NULL;
			mRight = NULL;
			mTriIndices = NULL;
		}

		~NodeAABB(void)
		{
			if ( mTriIndices )
			{
				::free(mTriIndices);
			}
		}

		// here is where we split the mesh..
		void split(const TriVector &triangles,
			RmUint vcount,
			const RmReal *vertices,
			RmUint tcount,
			const RmUint *indices,
			RmUint depth,
			RmUint maxDepth,	// Maximum recursion depth for the triangle mesh.
			RmUint minLeafSize,	// minimum triangles to treat as a 'leaf' node.
			RmReal	minAxisSize,
			NodeInterface *callback)	// once a particular axis is less than this size, stop sub-dividing.

		{
			// Find the longest axis
			RmReal dx = mBounds.mMax[0] - mBounds.mMin[0];
			RmReal dy = mBounds.mMax[1] - mBounds.mMin[1];
			RmReal dz = mBounds.mMax[2] - mBounds.mMin[2];

			AxisAABB axis = AABB_XAXIS;
			RmReal laxis = dx;
			if ( dy > dx )
			{
				axis = AABB_YAXIS;
				laxis = dy;
			}
			if ( dz > dx && dz > dy )
			{
				axis = AABB_ZAXIS;
				laxis = dz;
			}

			RmUint count = triangles.size();

			if ( count < minLeafSize || depth >= maxDepth || laxis < minAxisSize )
			{ 
				mTriIndices = (RmUint *)::malloc(sizeof(RmUint)*(count+1));
				memcpy(mTriIndices,&triangles[0],count*sizeof(RmUint));
				mTriIndices[count] = TRI_EOF;
			}
			else
			{
				RmReal center[3];
				mBounds.getCenter(center);
				BoundsAABB b1,b2;
				splitRect(axis,mBounds,b1,b2,center);

				BoundsAABB leftBounds,rightBounds;

				TriVector leftTriangles;
				TriVector rightTriangles;


				for (TriVector::const_iterator i=triangles.begin(); i!=triangles.end(); ++i)
				{
					RmUint tri = (*i); 

					{
						RmUint i1 = indices[tri*3+0];
						RmUint i2 = indices[tri*3+1];
						RmUint i3 = indices[tri*3+2];

						const RmReal *p1 = &vertices[i1*3];
						const RmReal *p2 = &vertices[i2*3];
						const RmReal *p3 = &vertices[i3*3];

						if ( b1.containsTriangle(p1,p2,p3))
						{
							if ( leftTriangles.empty() )
							{
								leftBounds.setMin(p1);
								leftBounds.setMax(p1);
							}
							leftBounds.include(p1);
							leftBounds.include(p2);
							leftBounds.include(p3);
							leftTriangles.push_back(tri);
						}

						if ( b2.containsTriangle(p1,p2,p3))
						{
							if ( rightTriangles.empty() )
							{
								rightBounds.setMin(p1);
								rightBounds.setMax(p1);
							}
							rightBounds.include(p1);
							rightBounds.include(p2);
							rightBounds.include(p3);
							rightTriangles.push_back(tri);
						}
					}
				}

				if ( !leftTriangles.empty() )
				{
					leftBounds.clamp(b1);
					mLeft = callback->getNode();
					new ( mLeft ) NodeAABB(leftBounds);
					mLeft->split(leftTriangles,vcount,vertices,tcount,indices,depth+1,maxDepth,minLeafSize,minAxisSize,callback);
				}

				if ( !rightTriangles.empty() )
				{
					rightBounds.clamp(b2);
					mRight = callback->getNode();
					new ( mRight ) NodeAABB(rightBounds);
					mRight->split(rightTriangles,vcount,vertices,tcount,indices,depth+1,maxDepth,minLeafSize,minAxisSize,callback);
				}

			}
		}

		void splitRect(AxisAABB axis,const BoundsAABB &source,BoundsAABB &b1,BoundsAABB &b2,const RmReal *midpoint)
		{
			switch ( axis )
			{
				case AABB_XAXIS:
					{
						b1.setMin( source.mMin );
						b1.setMax( midpoint[0], source.mMax[1], source.mMax[2] );

						b2.setMin( midpoint[0], source.mMin[1], source.mMin[2] );
						b2.setMax(source.mMax);
					}
					break;
				case AABB_YAXIS:
					{
						b1.setMin(source.mMin);
						b1.setMax(source.mMax[0], midpoint[1], source.mMax[2]);

						b2.setMin(source.mMin[0], midpoint[1], source.mMin[2]);
						b2.setMax(source.mMax);
					}
					break;
				case AABB_ZAXIS:
					{
						b1.setMin(source.mMin);
						b1.setMax(source.mMax[0], source.mMax[1], midpoint[2]);

						b2.setMin(source.mMin[0], source.mMin[1], midpoint[2]);
						b2.setMax(source.mMax);
					}
					break;
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		*	A method to compute a ray-AABB intersection.
		*	Original code by Andrew Woo, from "Graphics Gems", Academic Press, 1990
		*	Optimized code by Pierre Terdiman, 2000 (~20-30% faster on my Celeron 500)
		*	Epsilon value added by Klaus Hartmann. (discarding it saves a few cycles only)
		*
		*	Hence this version is faster as well as more robust than the original one.
		*
		*	Should work provided:
		*	1) the integer representation of 0.0f is 0x00000000
		*	2) the sign bit of the RmReal is the most significant one
		*
		*	Report bugs: p.terdiman@codercorner.com
		*
		*	\param		aabb		[in] the axis-aligned bounding box
		*	\param		origin		[in] ray origin
		*	\param		dir			[in] ray direction
		*	\param		coord		[out] impact coordinates
		*	\return		true if ray intersects AABB
		*/
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		#define RAYAABB_EPSILON 0.00001f
		//! Integer representation of a RmRealing-point value.
		#define IR(x)	((RmUint&)x)

		bool intersectRayAABB(const RmReal MinB[3],const RmReal MaxB[3],const RmReal origin[3],const RmReal dir[3],RmReal coord[3])
		{
			bool Inside = true;
			RmReal MaxT[3];
			MaxT[0]=MaxT[1]=MaxT[2]=-1.0f;

			// Find candidate planes.
			for(RmUint i=0;i<3;i++)
			{
				if(origin[i] < MinB[i])
				{
					coord[i]	= MinB[i];
					Inside		= false;

					// Calculate T distances to candidate planes
					if(IR(dir[i]))	MaxT[i] = (MinB[i] - origin[i]) / dir[i];
				}
				else if(origin[i] > MaxB[i])
				{
					coord[i]	= MaxB[i];
					Inside		= false;

					// Calculate T distances to candidate planes
					if(IR(dir[i]))	MaxT[i] = (MaxB[i] - origin[i]) / dir[i];
				}
			}

			// Ray origin inside bounding box
			if(Inside)
			{
				coord[0] = origin[0];
				coord[1] = origin[1];
				coord[2] = origin[2];
				return true;
			}

			// Get largest of the maxT's for final choice of intersection
			RmUint WhichPlane = 0;
			if(MaxT[1] > MaxT[WhichPlane])	WhichPlane = 1;
			if(MaxT[2] > MaxT[WhichPlane])	WhichPlane = 2;

			// Check final candidate actually inside box
			if(IR(MaxT[WhichPlane])&0x80000000) return false;

			for(RmUint i=0;i<3;i++)
			{
				if(i!=WhichPlane)
				{
					coord[i] = origin[i] + MaxT[WhichPlane] * dir[i];
					#ifdef RAYAABB_EPSILON
					if(coord[i] < MinB[i] - RAYAABB_EPSILON || coord[i] > MaxB[i] + RAYAABB_EPSILON)	return false;
					#else
					if(coord[i] < MinB[i] || coord[i] > MaxB[i])	return false;
					#endif
				}
			}
			return true;	// ray hits box
		}




		bool intersectLineSegmentAABB(const RmReal bmin[3],const RmReal bmax[3],const RmReal p1[3],const RmReal dir[3],RmReal &dist,RmReal intersect[3])
		{
			bool ret = false;

			if ( dist > RAYAABB_EPSILON )
			{
				ret = intersectRayAABB(bmin,bmax,p1,dir,intersect);
				if ( ret )
				{
					RmReal dx = p1[0]-intersect[0];
					RmReal dy = p1[1]-intersect[1];
					RmReal dz = p1[2]-intersect[2];
					RmReal d = dx*dx+dy*dy+dz*dz;
					if ( d < dist*dist )
					{
						dist = sqrtf(d);
					}
					else
					{
						ret = false;
					}
				}
			}
			return ret;
		}


		/* a = b - c */
#define vector(a,b,c) \
	(a)[0] = (b)[0] - (c)[0];	\
	(a)[1] = (b)[1] - (c)[1];	\
	(a)[2] = (b)[2] - (c)[2];



#define innerProduct(v,q) \
	((v)[0] * (q)[0] + \
	(v)[1] * (q)[1] + \
	(v)[2] * (q)[2])

#define crossProduct(a,b,c) \
	(a)[0] = (b)[1] * (c)[2] - (c)[1] * (b)[2]; \
	(a)[1] = (b)[2] * (c)[0] - (c)[2] * (b)[0]; \
	(a)[2] = (b)[0] * (c)[1] - (c)[0] * (b)[1];


		bool rayIntersectsTriangle(const RmReal *p,const RmReal *d,const RmReal *v0,const RmReal *v1,const RmReal *v2,RmReal &t)
		{
			RmReal e1[3],e2[3],h[3],s[3],q[3];
			RmReal a,f,u,v;

			vector(e1,v1,v0);
			vector(e2,v2,v0);
			crossProduct(h,d,e2);
			a = innerProduct(e1,h);

			if (a > -0.00001 && a < 0.00001)
				return(false);

			f = 1/a;
			vector(s,p,v0);
			u = f * (innerProduct(s,h));

			if (u < 0.0 || u > 1.0)
				return(false);

			crossProduct(q,s,e1);
			v = f * innerProduct(d,q);
			if (v < 0.0 || u + v > 1.0)
				return(false);
			// at this stage we can compute t to find out where
			// the intersection point is on the line
			t = f * innerProduct(e2,q);
			if (t > 0) // ray intersection
				return(true);
			else // this means that there is a line intersection
				// but not a ray intersection
				return (false);
		}



		virtual void raycast(bool &hit,
							const RmReal *from,
							const RmReal *to,
							const RmReal *dir,
							RmReal *hitLocation,
							RmReal *hitNormal,
							RmReal *hitDistance,
							const RmReal *vertices,
							const RmUint *indices,
							RmReal &nearestDistance,
							NodeInterface *callback)
		{
			unsigned acode;
			if ( !mBounds.containsLineSegment(from,to,acode))
			{
				return;
			}

			if ( acode )
			{
				RmReal sect[3];
				RmReal nd = nearestDistance;
				if ( !intersectLineSegmentAABB(mBounds.mMin,mBounds.mMax,from,dir,nd,sect) )
				{
					return;	
				}
			}

			if ( mTriIndices )
			{
				const RmUint *scan = mTriIndices;
				while ( *scan != TRI_EOF )
				{
					RmUint tri = *scan++;
					RmUint i1 = indices[tri*3+0];
					RmUint i2 = indices[tri*3+1];
					RmUint i3 = indices[tri*3+2];

					const RmReal *p1 = &vertices[i1*3];
					const RmReal *p2 = &vertices[i2*3];
					const RmReal *p3 = &vertices[i3*3];

					RmReal t;
					if ( rayIntersectsTriangle(from,dir,p1,p2,p3,t))
					{
						if ( t < nearestDistance )
						{
							nearestDistance = t;
							if ( hitLocation )
							{
								hitLocation[0] = from[0]+dir[0]*t;
								hitLocation[1] = from[1]+dir[1]*t;
								hitLocation[2] = from[2]+dir[2]*t;
								if ( hitNormal )
								{
									callback->getFaceNormal(tri,hitNormal);
								}
								if ( hitDistance )
								{
									*hitDistance = t;
								}
								hit = true;
							}
						}
					}
				}
			}
			else
			{
				if ( mLeft )
				{
					mLeft->raycast(hit,from,to,dir,hitLocation,hitNormal,hitDistance,vertices,indices,nearestDistance,callback);
				}
				if ( mRight )
				{
					mRight->raycast(hit,from,to,dir,hitLocation,hitNormal,hitDistance,vertices,indices,nearestDistance,callback);
				}
			}
		}

		NodeAABB		*mLeft;			// left node
		NodeAABB		*mRight;		// right node
		BoundsAABB		mBounds;		// bounding volume of node
		RmUint	*mTriIndices;	// if it is a leaf node; then these are the triangle indices.
	};

class MyRaycastMesh : public RaycastMesh, public NodeInterface
{
public:

	MyRaycastMesh(RmUint vcount,const RmReal *vertices,RmUint tcount,const RmUint *indices,RmUint maxDepth,RmUint minLeafSize,RmReal minAxisSize)
	{
		if ( maxDepth < 2 )
		{
			maxDepth = 2;
		}
		if ( maxDepth > 15 )
		{
			maxDepth = 15;
		}
		RmUint pow2Table[16] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 65536 };
		mMaxNodeCount = 0;
		for (RmUint i=0; i<=maxDepth; i++)
		{
			mMaxNodeCount+=pow2Table[i];
		}
		mFaceNormals = NULL;
		mNodes = new NodeAABB[mMaxNodeCount];
		mNodeCount = 0;
		mVcount = vcount;
		mVertices = (RmReal *)::malloc(sizeof(RmReal)*3*vcount);
		memcpy(mVertices,vertices,sizeof(RmReal)*3*vcount);
		mTcount = tcount;
		mIndices = (RmUint *)::malloc(sizeof(RmUint)*tcount*3);
		memcpy(mIndices,indices,sizeof(RmUint)*tcount*3);
		mRoot = getNode();
		new ( mRoot ) NodeAABB(mVcount,mVertices,mTcount,mIndices,maxDepth,minLeafSize,minAxisSize,this);
	}

	~MyRaycastMesh(void)
	{
		delete []mNodes;
		::free(mVertices);
		::free(mIndices);
		::free(mFaceNormals);
	}

	virtual bool raycast(const RmReal *from,const RmReal *to,RmReal *hitLocation,RmReal *hitNormal,RmReal *hitDistance)
	{
		bool ret = false;
		RmReal dir[3];
		dir[0] = to[0] - from[0];
		dir[1] = to[1] - from[1];
		dir[2] = to[2] - from[2];
		RmReal distance = sqrtf( dir[0]*dir[0] + dir[1]*dir[1]+dir[2]*dir[2] );
		if ( distance < 0.0000000001f ) return false;
		RmReal recipDistance = 1.0f / distance;
		dir[0]*=recipDistance;
		dir[1]*=recipDistance;
		dir[2]*=recipDistance;
		mRoot->raycast(ret,from,to,dir,hitLocation,hitNormal,hitDistance,mVertices,mIndices,distance,this);
		return ret;
	}

	virtual void release(void)
	{
		delete this;
	}

	virtual const RmReal * getBoundMin(void) const // return the minimum bounding box
	{
		return mRoot->mBounds.mMin;
	}
	virtual const RmReal * getBoundMax(void) const // return the maximum bounding box.
	{
		return mRoot->mBounds.mMax;
	}

	virtual NodeAABB * getNode(void) 
	{
		assert( mNodeCount < mMaxNodeCount );
		NodeAABB *ret = &mNodes[mNodeCount];
		mNodeCount++;
		return ret;
	}

	virtual void getFaceNormal(RmUint tri,float *faceNormal)
	{
		if ( mFaceNormals == NULL )
		{
			mFaceNormals = (RmReal *)::malloc(sizeof(RmReal)*3*mTcount);
			for (RmUint i=0; i<mTcount; i++)
			{
				RmUint i1 = mIndices[i*3+0];
				RmUint i2 = mIndices[i*3+1];
				RmUint i3 = mIndices[i*3+2];
				const RmReal*p1 = &mVertices[i1*3];
				const RmReal*p2 = &mVertices[i2*3];
				const RmReal*p3 = &mVertices[i3*3];
				computePlane(p3,p2,p1,&mFaceNormals[i*3]);
			}
		}
		const float *src = &mFaceNormals[tri*3];
		faceNormal[0] = src[0];
		faceNormal[1] = src[1];
		faceNormal[2] = src[2];
	}

	RmUint	mVcount;
	RmReal			*mVertices;
	RmReal			*mFaceNormals;
	RmUint	mTcount;
	RmUint	*mIndices;
	NodeAABB		*mRoot;
	RmUint	mNodeCount;
	RmUint	mMaxNodeCount;
	NodeAABB		*mNodes;

};

};



using namespace RAYCAST_MESH;


RaycastMesh * createRaycastMesh(RmUint vcount,		// The number of vertices in the source triangle mesh
								const RmReal *vertices,		// The array of vertex positions in the format x1,y1,z1..x2,y2,z2.. etc.
								RmUint tcount,		// The number of triangles in the source triangle mesh
								const RmUint *indices, // The triangle indices in the format of i1,i2,i3 ... i4,i5,i6, ...
								RmUint maxDepth,	// Maximum recursion depth for the triangle mesh.
								RmUint minLeafSize,	// minimum triangles to treat as a 'leaf' node.
								RmReal	minAxisSize	// once a particular axis is less than this size, stop sub-dividing.
								)
{
	MyRaycastMesh *m = new MyRaycastMesh(vcount,vertices,tcount,indices,maxDepth,minLeafSize,minAxisSize);
	return static_cast< RaycastMesh * >(m);
}



