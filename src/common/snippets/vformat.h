#ifndef VFORMAT_H

#define VFORMAT_H

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
** Personal website: http://jratcliffscarab.blogspot.com
** Coding Website:   http://codesuppository.blogspot.com
** FundRaising Blog: http://amillionpixels.blogspot.com
** Fundraising site: http://www.amillionpixels.us
** New Temple Site:  http://newtemple.blogspot.com
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


#include <set>

#include "common/snippets/UserMemAlloc.h"
#include "vector.h"

class DeformData
{
public:
	HeF32         mWeight[4];          // 4 weightings
	HeU16 mBone1;
	HeU16 mBone2;
	HeU16 mBone3;
	HeU16 mBone4;
};

class MeshVertex
{
public:

	void GetPos(Vector3d<HeF32> &pos) const { pos = mPos; };
	const Vector3d<HeF32>& GetPos(void) const { return mPos; };

	HeF32 GetX(void) const { return mPos.x; };
	HeF32 GetY(void) const { return mPos.y; };
	HeF32 GetZ(void) const { return mPos.z; };

	void Lerp(const MeshVertex &a,const MeshVertex &b,HeF32 p)
	{
		mPos.Lerp(a.mPos,b.mPos,p);
		mTexel1.Lerp(a.mTexel1,b.mTexel1,p);
		mTexel2.Lerp(a.mTexel2,b.mTexel2,p);
		mNormal.Lerp(a.mNormal,b.mNormal,p);
	};

	void Set(HeI32 index,const HeF32 *pos,const HeF32 *normal,const HeF32 *texel1,const HeF32 *texel2)
	{
		const HeF32 * p = &pos[index*3];

		const HeF32 * tv1 = &texel1[index*2];
		const HeF32 * tv2 = &texel2[index*2];

		mPos.x     = p[0];
		mPos.y     = p[1];
		mPos.z     = p[2];

		mNormal.x  = normal[0];
		mNormal.y  = normal[1];
		mNormal.z  = normal[2];

		mTexel1.x  = tv1[0];
		mTexel1.y  = tv1[1];

		mTexel2.x  = tv2[0];
		mTexel2.y  = tv2[1];
	};

	Vector3d<HeF32> mPos;
	Vector3d<HeF32> mNormal;
	Vector2d<HeF32> mTexel1;
	Vector2d<HeF32> mTexel2;
};


class PosVertex
{
public:

	void GetPos(Vector3d<HeF32> &pos) const { pos = mPos; };
	const Vector3d<HeF32>& GetPos(void) const { return mPos; };

	HeF32 GetX(void) const { return mPos.x; };
	HeF32 GetY(void) const { return mPos.y; };
	HeF32 GetZ(void) const { return mPos.z; };

	void Lerp(const MeshVertex &a,const MeshVertex &b,HeF32 p)
	{
		mPos.Lerp(a.mPos,b.mPos,p);
	};

	Vector3d<HeF32> mPos;
};


class MeshDeformVertex
{
public:

	void GetPos(Vector3d<HeF32> &pos) const { pos = mPos; };
	const Vector3d<HeF32>& GetPos(void) const { return mPos; };

	HeF32 GetX(void) const { return mPos.x; };
	HeF32 GetY(void) const { return mPos.y; };
	HeF32 GetZ(void) const { return mPos.z; };

	void Lerp(const MeshVertex &a,const MeshVertex &b,HeF32 p)
	{
		mPos.Lerp(a.mPos,b.mPos,p);
		mTexel1.Lerp(a.mTexel1,b.mTexel1,p);
		mTexel2.Lerp(a.mTexel2,b.mTexel2,p);
		mNormal.Lerp(a.mNormal,b.mNormal,p);
	};

	void Lerp(const MeshDeformVertex &a,const MeshDeformVertex &b,HeF32 p)
	{
		mPos.Lerp(a.mPos,b.mPos,p);
		mTexel1.Lerp(a.mTexel1,b.mTexel1,p);
		mTexel2.Lerp(a.mTexel2,b.mTexel2,p);
		mNormal.Lerp(a.mNormal,b.mNormal,p);
		mWeight[0] = a.mWeight[0];
		mWeight[1] = a.mWeight[1];
		mWeight[2] = a.mWeight[2];
		mWeight[3] = a.mWeight[3];
		mBone[0] = a.mBone[0];
		mBone[1] = a.mBone[1];
		mBone[2] = a.mBone[2];
		mBone[3] = a.mBone[3];

	};

	void Set(HeI32 index,const HeF32 *pos,const HeF32 *normal,const HeF32 *texel1,const HeF32 *texel2)
	{
		const HeF32 * p = &pos[index*3];

		const HeF32 * tv1 = &texel1[index*2];
		const HeF32 * tv2 = &texel2[index*2];

		mPos.x     = p[0];
		mPos.y     = p[1];
		mPos.z     = p[2];

		mNormal.x  = normal[0];
		mNormal.y  = normal[1];
		mNormal.z  = normal[2];

		mTexel1.x  = tv1[0];
		mTexel1.y  = tv1[1];

		mTexel2.x  = tv2[0];
		mTexel2.y  = tv2[1];
	};

	bool HasBone(HeI32 b) const
	{
		if ((b == mBone[0] && mWeight[0] > 0.01f ) ||
				(b == mBone[1] && mWeight[1] > 0.01f ) ||
				(b == mBone[2] && mWeight[2] > 0.01f ) ||
				(b == mBone[3] && mWeight[3] > 0.01f ) ) return true;


		return false;
	}

	void set(const HeF32 *p,const HeF32 *n,const HeF32 *t1,const HeF32 *t2,const HeF32 *w,const HeU16 *b)
	{
		mPos.Set(p);
		mNormal.Set(n);
		mTexel1.Set(t1);
		mTexel2.Set(t2);
		mWeight[0] = w[0];
		mWeight[1] = w[1];
		mWeight[2] = w[2];
		mWeight[3] = w[3];
		mBone[0] = b[0];
		mBone[1] = b[1];
		mBone[2] = b[2];
		mBone[3] = b[3];
	}

	Vector3d<HeF32> mPos;
	Vector3d<HeF32> mNormal;
	Vector2d<HeF32> mTexel1;
	Vector2d<HeF32> mTexel2;
	HeF32           mWeight[4];
	HeU16  mBone[4];
};


template <class Type> class VertexLess
{
public:
	typedef USER_STL::vector< Type > VertexVector;

	bool operator()(HeI32 v1,HeI32 v2) const;

	static void SetSearch(const Type& match,VertexVector *list)
	{
		mFind = match;
		mList = list;
	};

private:
	const Type& Get(HeI32 index) const
	{
		if ( index == -1 ) return mFind;
		VertexVector &vlist = *mList;
		return vlist[index];
	}
	static Type mFind; // vertice to locate.
	static VertexVector  *mList;
};

template <class Type> class VertexPool
{
public:
  typedef USER_STL::set<HeI32, USER_STL::GlobalMemoryPool, VertexLess<Type> > VertexSet;
	typedef USER_STL::vector< Type > VertexVector;

	HeI32 GetVertex(const Type& vtx)
	{
		VertexLess<Type>::SetSearch(vtx,&mVtxs);
		typename VertexSet::iterator found;
		found = mVertSet.find( -1 );
		if ( found != mVertSet.end() )
		{
			return *found;
		}
		HeI32 idx = (HeI32)mVtxs.size();
		mVtxs.push_back( vtx );
		mVertSet.insert( idx );
		return idx;
	};

	void GetPos(HeI32 idx,Vector3d<HeF32> &pos) const
	{
		pos = mVtxs[idx].mPos;
	}

	const Type& Get(HeI32 idx) const
	{
		return mVtxs[idx];
	};

	HeI32 GetSize(void) const
	{
		return (HeI32)mVtxs.size();
	};

	void Clear(HeI32 reservesize)  // clear the vertice pool.
	{
		mVertSet.clear();
		mVtxs.clear();
		mVtxs.reserve(reservesize);
	};

	const VertexVector& GetVertexList(void) const { return mVtxs; };

	void Set(const Type& vtx)
	{
		mVtxs.push_back(vtx);
	}

	HeI32 GetVertexCount(void) const
	{
		return (HeI32)mVtxs.size();
	};

	bool GetVertex(HeI32 i,Vector3d<HeF32> &vect) const
	{
		vect = mVtxs[i].mPos;
		return true;
	};


	Type * GetBuffer(void)
	{
		return &mVtxs[0];
	};
private:
	VertexSet      mVertSet; // ordered list.
	VertexVector   mVtxs;  // set of vertices.
};

#endif
