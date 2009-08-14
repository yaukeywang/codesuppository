#ifndef STAN_HULL_H

#define STAN_HULL_H

#include "UserMemAlloc.h"

/*----------------------------------------------------------------------
		Copyright (c) 2004 Open Dynamics Framework Group
					www.physicstools.org
		All rights reserved.

		Redistribution and use in source and binary forms, with or without modification, are permitted provided
		that the following conditions are met:

		Redistributions of source code must retain the above copyright notice, this list of conditions
		and the following disclaimer.

		Redistributions in binary form must reproduce the above copyright notice,
		this list of conditions and the following disclaimer in the documentation
		and/or other materials provided with the distribution.

		Neither the name of the Open Dynamics Framework Group nor the names of its contributors may
		be used to endorse or promote products derived from this software without specific prior written permission.

		THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES,
		INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
		DISCLAIMED. IN NO EVENT SHALL THE INTEL OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
		EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
		LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
		IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
		THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------*/

namespace STAN_HULL
{

class HullResult
{
public:
	HullResult(void)
	{
		mPolygons = true;
		mNumOutputVertices = 0;
		mOutputVertices = 0;
		mNumFaces = 0;
		mNumIndices = 0;
		mIndices = 0;
	}
	bool                    mPolygons;                  // true if indices represents polygons, false indices are triangles
	NxU32            mNumOutputVertices;         // number of vertices in the output hull
	NxF32                  *mOutputVertices;            // array of vertices, 3 floats each x,y,z
	NxU32            mNumFaces;                  // the number of faces produced
	NxU32            mNumIndices;                // the total number of indices
	NxU32           *mIndices;                   // pointer to indices.

// If triangles, then indices are array indexes into the vertex list.
// If polygons, indices are in the form (number of points in face) (p1, p2, p3, ..) etc..
};

enum HullFlag
{
	QF_TRIANGLES         = (1<<0),             // report results as triangles, not polygons.
	QF_REVERSE_ORDER     = (1<<1),             // reverse order of the triangle indices.
	QF_SKIN_WIDTH        = (1<<2),             // extrude hull based on this skin width
	QF_DEFAULT           = 0
};


class HullDesc
{
public:
	HullDesc(void)
	{
		mFlags          = QF_DEFAULT;
		mVcount         = 0;
		mVertices       = 0;
		mVertexStride   = 0;
		mNormalEpsilon  = 0.001f;
		mMaxVertices = 4096; // maximum number of points to be considered for a convex hull.
		mSkinWidth = 0.01f; // default is one centimeter
	};

	HullDesc(HullFlag flag,
						 NxU32 vcount,
						 const NxF32 *vertices,
						 NxU32 stride)
	{
		mFlags          = flag;
		mVcount         = vcount;
		mVertices       = vertices;
		mVertexStride   = stride;
		mNormalEpsilon  = 0.001f;
		mMaxVertices    = 4096;
		mSkinWidth = 0.01f; // default is one centimeter
	}

	bool HasHullFlag(HullFlag flag) const
	{
		if ( mFlags & flag ) return true;
		return false;
	}

	void SetHullFlag(HullFlag flag)
	{
		mFlags|=flag;
	}

	void ClearHullFlag(HullFlag flag)
	{
		mFlags&=~flag;
	}

	NxU32      mFlags;           // flags to use when generating the convex hull.
	NxU32      mVcount;          // number of vertices in the input point cloud
	const NxF32      *mVertices;        // the array of vertices.
	NxU32      mVertexStride;    // the stride of each vertex, in bytes.
	NxF32             mNormalEpsilon;   // the epsilon for removing duplicates.  This is a normalized value, if normalized bit is on.
	NxF32             mSkinWidth;
	NxU32      mMaxVertices;               // maximum number of vertices to be considered for the hull!
};

enum HullError
{
	QE_OK,            // success!
	QE_FAIL,           // failed.
	QE_NOT_READY,
};

// This class is used when converting a convex hull into a triangle mesh.
class ConvexHullVertex
{
public:
	NxF32         mPos[3];
	NxF32         mNormal[3];
	NxF32         mTexel[2];
};

// A virtual interface to receive the triangles from the convex hull.
class ConvexHullTriangleInterface
{
public:
	virtual void ConvexHullTriangle(const ConvexHullVertex &v1,const ConvexHullVertex &v2,const ConvexHullVertex &v3) = 0;
};


class HullLibrary
{
public:

	HullError CreateConvexHull(const HullDesc       &desc,           // describes the input request
															HullResult           &result);        // contains the resulst

	HullError ReleaseResult(HullResult &result); // release memory allocated for this result, we are done with it.

	HullError CreateTriangleMesh(HullResult &answer,ConvexHullTriangleInterface *iface);
private:
	NxF32 ComputeNormal(NxF32 *n,const NxF32 *A,const NxF32 *B,const NxF32 *C);
	void AddConvexTriangle(ConvexHullTriangleInterface *callback,const NxF32 *p1,const NxF32 *p2,const NxF32 *p3);

	void BringOutYourDead(const NxF32 *verts,NxU32 vcount, NxF32 *overts,NxU32 &ocount,NxU32 *indices,NxU32 indexcount);

	bool    CleanupVertices(NxU32 svcount,
													const NxF32 *svertices,
													NxU32 stride,
													NxU32 &vcount,       // output number of vertices
													NxF32 *vertices,                 // location to store the results.
													NxF32  normalepsilon,
													NxF32 *scale);
};

}; // end of STAN_HULL namespace

#endif
