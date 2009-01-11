#ifndef GEOMETRY_H

#define GEOMETRY_H

/*!
**
** Copyright (c) 2007 by John W. Ratcliff mailto:jratcliff@infiniplex.net
**
** Portions of this source has been released with the PhysXViewer application, as well as
** Rocket, CreateDynamics, ODF, and as a number of sample code snippets.
**
** If you find this code useful or you are feeling particularily generous I would
** ask that you please go to http://www.amillionpixels.us and make a donation
** to Troy DeMolay.
**
** DeMolay is a youth group for young men between the ages of 12 and 21.
** It teaches strong moral principles, as well as leadership skills and
** public speaking.  The donations page uses the 'pay for pixels' paradigm
** where, in this case, a pixel is only a single penny.  Donations can be
** made for as small as $4 or as high as a $100 block.  Each person who donates
** will get a link to their own site as well as acknowledgement on the
** donations blog located here http://www.amillionpixels.blogspot.com/
**
** If you wish to contact me you can use the following methods:
**
** Skype Phone: 636-486-4040 (let it ring a long time while it goes through switches)
** Skype ID: jratcliff63367
** Yahoo: jratcliff63367
** AOL: jratcliff1961
** email: jratcliff@infiniplex.net
**
**
** The MIT license:
**
** Permission is hereby granted, MEMALLOC_FREE of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is furnished
** to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.

** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
** WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/


#include "common/snippets/UserMemAlloc.h"


class Skeleton;
class Animation;

enum GeometryVertexType
{
	GVT_RIGID,          // for rigid bodies, two sets of texture channels and a vertex normal
	GVT_DEFORM,         // for deformed objects, up to 4 bone weightings and 4 bone indices
	GVT_THIN            // for fluids, just a position and normal since the texture coordinates are generated.
};

class GeometryVertexThin
{
public:
	HeF32        mPos[3];
	HeF32        mNormal[3];
};

class GeometryVertex;

class GeometryDeformVertex
{
public:
	GeometryDeformVertex(void) { };
	inline GeometryDeformVertex(const GeometryVertex &v);
	inline void Lerp(const GeometryVertex &a,const GeometryVertex &b,HeF32 d);

	HeF32        mPos[3];
	HeF32        mNormal[3];
	HeF32        mTexel1[2];
	HeF32        mTexel2[2];
	HeF32        mWeight[4];
	HeU16 mBone[4];
};


class GeometryVertex
{
public:
  GeometryVertex(void) { };

	GeometryVertex(const GeometryDeformVertex &v)
	{
		mPos[0] = v.mPos[0];
		mPos[1] = v.mPos[1];
		mPos[2] = v.mPos[2];
		mNormal[0] = v.mNormal[0];
		mNormal[1] = v.mNormal[1];
		mNormal[2] = v.mNormal[2];
		mTexel1[0] = v.mTexel1[0];
		mTexel1[1] = v.mTexel1[1];
		mTexel2[0] = v.mTexel2[0];
		mTexel2[1] = v.mTexel2[1];
	}


	void Lerp(const GeometryVertex &a,const GeometryVertex &b,HeF32 d)
	{
		mPos[0] = ((b.mPos[0] - a.mPos[0]) * d) + a.mPos[0];
		mPos[1] = ((b.mPos[1] - a.mPos[1]) * d) + a.mPos[1];
		mPos[2] = ((b.mPos[2] - a.mPos[2]) * d) + a.mPos[2];

		mNormal[0] = ((b.mNormal[0] - a.mNormal[0])*d) + a.mNormal[0];
		mNormal[1] = ((b.mNormal[1] - a.mNormal[1])*d) + a.mNormal[1];
		mNormal[2] = ((b.mNormal[2] - a.mNormal[2])*d) + a.mNormal[2];

		mTexel1[0] = ((b.mTexel1[0] - a.mTexel1[0])*d) + a.mTexel1[0];
		mTexel1[1] = ((b.mTexel1[1] - a.mTexel1[1])*d) + a.mTexel1[1];
		mTexel1[2] = ((b.mTexel1[2] - a.mTexel1[2])*d) + a.mTexel1[2];

		mTexel2[0] = ((b.mTexel2[0] - a.mTexel2[0])*d) + a.mTexel2[0];
		mTexel2[1] = ((b.mTexel2[1] - a.mTexel2[1])*d) + a.mTexel2[1];
		mTexel2[2] = ((b.mTexel2[2] - a.mTexel2[2])*d) + a.mTexel2[2];

	}

	HeF32        mPos[3];
	HeF32        mNormal[3];
	HeF32        mTexel1[2];
	HeF32        mTexel2[2];
};


#pragma warning(push)
#pragma warning(disable:4100)
class GeometryInterface
{
public:

	virtual void NodeMaterial(const char *name,const char *info)
	{
	}

	virtual void NodeMesh(const char *name,const char *info)
	{
	}

  virtual void NodeTriangleList(HeI32 vcount,const GeometryVertex *vertex,HeI32 icount,const HeU16 *indices)
  {
  }



	virtual void NodeTriangleList(HeI32 vcount,const GeometryVertex *vertex,HeI32 icount,const HeU32 *indices)
	{
	}

	virtual void NodeTriangleList(HeI32 vcount,const GeometryDeformVertex *vertex,HeI32 icount,const HeU32 *indices)
	{
	}


	virtual void NodeTriangle(const GeometryVertex *v1,const GeometryVertex *v2,const GeometryVertex *v3)
	{
	}

	virtual void NodeTriangle(const GeometryDeformVertex *v1,const GeometryDeformVertex *v2,const GeometryDeformVertex *v3)
	{
	}

	// add a skeleton
	virtual void NodeSkeleton(const Skeleton *skeleton)
	{
	}

	virtual void NodeAnimation(const Animation *animation)
	{
	}

	virtual void NodeInstance(const char *name,const HeF32 *transform)
	{
	}

private:
};
#pragma warning(pop)
enum GS_AXIS
{
	GS_XAXIS,
	GS_YAXIS,
	GS_ZAXIS
};

class GeometryShape
{
public:

	HeF32 * CreateBox(const HeF32 *sides,HeI32 &vcount);
	HeF32 * CreateBox(const HeF32 *bmin,const HeF32 *bmax,HeI32 &vcount);

	HeF32 * CreateSphere(HeF32 radius,HeI32 stepsize,HeI32 &vcount);

	HeF32 * CreateCylinder(HeF32 radius,           // radius of cylinder
												 HeF32 height,           // height of cylinder
												 GS_AXIS axis,           // dominant axis of the cylinder
												 HeI32 stepsize,           // step size
												 HeI32 &vcount);           // number of vertices produced

	HeF32 * CreateCapsule(HeF32 radius,           // radius of cylinder
												HeF32 height,           // height of cylinder
												GS_AXIS axis,           // dominant axis of the cylinder
												HeI32 stepsize,           // step size
												HeI32 &vcount);           // number of vertices produced

	// compute the face normal and project the texture coordinates for these three verts.
	static void  TexGenNormal(GeometryVertex &v1,GeometryVertex &v2,GeometryVertex &v3,HeF32 tscale1=(1.0f/4.0f),HeF32 tscale2=(1.0f/16.0f));
	static void  TexGenNormal(GeometryDeformVertex &v1,GeometryDeformVertex &v2,GeometryDeformVertex &v3,HeF32 tscale1=(1.0f/4.0f),HeF32 tscale2=(1.0f/16.0f));
	static HeF32 ComputeNormal(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,HeF32 *normal);

private:
};


class SpriteVertex
{
public:
  HeF32								mPos[3];  // position in world space.
  HeF32               mWeight[2]; // sprite 'weighting' values.  Alpha fade, size, etc. computed in veretex shader.
};


inline GeometryDeformVertex::GeometryDeformVertex(const GeometryVertex &v)
{
	mPos[0] = v.mPos[0];
	mPos[1] = v.mPos[1];
	mPos[2] = v.mPos[2];

	mNormal[0] = v.mNormal[0];
	mNormal[1] = v.mNormal[1];
	mNormal[2] = v.mNormal[2];

	mTexel1[0] = v.mTexel1[0];
	mTexel1[1] = v.mTexel1[1];
	mTexel1[2] = v.mTexel1[2];

	mTexel2[0] = v.mTexel2[0];
	mTexel2[1] = v.mTexel2[1];
	mTexel2[2] = v.mTexel2[2];

	mWeight[0] = 1;
	mWeight[1] = 0;
	mWeight[2] = 0;
	mWeight[3] = 0;

	mBone[0] = 0;
	mBone[0] = 0;
	mBone[0] = 0;
	mBone[0] = 0;
};

inline void GeometryDeformVertex::Lerp(const GeometryVertex &a,const GeometryVertex &b,HeF32 d)
{
	mPos[0] = ((b.mPos[0] - a.mPos[0]) * d) + a.mPos[0];
	mPos[1] = ((b.mPos[1] - a.mPos[1]) * d) + a.mPos[1];
	mPos[2] = ((b.mPos[2] - a.mPos[2]) * d) + a.mPos[2];

	mNormal[0] = ((b.mNormal[0] - a.mNormal[0])*d) + a.mNormal[0];
	mNormal[1] = ((b.mNormal[1] - a.mNormal[1])*d) + a.mNormal[1];
	mNormal[2] = ((b.mNormal[2] - a.mNormal[2])*d) + a.mNormal[2];

	mTexel1[0] = ((b.mTexel1[0] - a.mTexel1[0])*d) + a.mTexel1[0];
	mTexel1[1] = ((b.mTexel1[1] - a.mTexel1[1])*d) + a.mTexel1[1];
	mTexel1[2] = ((b.mTexel1[2] - a.mTexel1[2])*d) + a.mTexel1[2];

	mTexel2[0] = ((b.mTexel2[0] - a.mTexel2[0])*d) + a.mTexel2[0];
	mTexel2[1] = ((b.mTexel2[1] - a.mTexel2[1])*d) + a.mTexel2[1];
	mTexel2[2] = ((b.mTexel2[2] - a.mTexel2[2])*d) + a.mTexel2[2];

}


#endif
