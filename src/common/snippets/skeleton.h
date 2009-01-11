#ifndef SKELETON_H

#define SKELETON_H


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

#define MAXSTRLEN 256

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "common/snippets/UserMemAlloc.h"

#pragma warning(push)
#pragma warning(disable:4996)
class Bone
{
public:
	Bone(void)
	{
		mParentIndex = -1;
		strcpy(mName,"");
		Identity();
	}

	void Identity(void)
	{
		mPosition[0] = 0;
		mPosition[1] = 0;
		mPosition[2] = 0;

		mOrientation[0] = 0;
		mOrientation[1] = 0;
		mOrientation[2] = 0;
		mOrientation[3] = 1;
		IdentityTransform();
	}

	void IdentityTransform(void)
	{
		mElement[0][0] = 1;
		mElement[1][1] = 1;
		mElement[2][2] = 1;
		mElement[3][3] = 1;

		mElement[1][0] = 0;
		mElement[2][0] = 0;
		mElement[3][0] = 0;

		mElement[0][1] = 0;
		mElement[2][1] = 0;
		mElement[3][1] = 0;

		mElement[0][2] = 0;
		mElement[1][2] = 0;
		mElement[3][2] = 0;

		mElement[0][3] = 0;
		mElement[1][3] = 0;
		mElement[2][3] = 0;
	}

	void SetName(const char *name)
	{
		strncpy(mName,name,MAXSTRLEN);
	}

	void Set(const char *name,HeI32 parent,const HeF32 *transform)
	{
		strncpy(mName,name,MAXSTRLEN);
		mParentIndex = parent;
		memcpy(mElement,transform,sizeof(HeF32)*16);

		ExtractOrientation(mOrientation);

		mPosition[0] = mElement[3][0];
		mPosition[1] = mElement[3][1];
		mPosition[2] = mElement[3][2];

	}

	void Set(const char *name,HeI32 parent,const HeF32 *pos,const HeF32 *rot)
	{
		strncpy(mName,name,MAXSTRLEN);

		mParentIndex = parent;

		mPosition[0] = pos[0];
		mPosition[1] = pos[1];
		mPosition[2] = pos[2];

		mOrientation[0] = rot[0];
		mOrientation[1] = rot[1];
		mOrientation[2] = rot[2];
		mOrientation[3] = rot[3];

		ComposeTransform();

	}

	void GetPos(HeF32 *pos) const
	{
		pos[0] = mElement[3][0];
		pos[1] = mElement[3][1];
		pos[2] = mElement[3][2];
	}

	const char * GetName(void) const { return mName; };

	const HeF32 * GetTransform(void) const { return &mElement[0][0]; };

	void ExtractOrientation(HeF32 *rot)
	{
		HeF32 tr = mElement[0][0] + mElement[1][1] + mElement[2][2];
		if (tr > 0.0f )
		{
			HeF32 s = (HeF32) sqrtf( tr + 1.0f);
			rot[3] = s * 0.5f;
			s = 0.5f / s;
			rot[0] = (mElement[1][2] - mElement[2][1]) * s;
			rot[1] = (mElement[2][0] - mElement[0][2]) * s;
			rot[2] = (mElement[0][1] - mElement[1][0]) * s;
		}
		else
		{
			// diagonal is negative
			HeI32 nxt[3] = {1, 2, 0};
			HeF32  qa[4];

			HeI32 i = 0;

			if (mElement[1][1] > mElement[0][0]) i = 1;
			if (mElement[2][2] > mElement[i][i]) i = 2;

			HeI32 j = nxt[i];
			HeI32 k = nxt[j];

			HeF32 s = sqrtf ( ((mElement[i][i] - (mElement[j][j] + mElement[k][k])) + 1.0f) );

			qa[i] = s * 0.5f;

			if (s != 0.0f ) s = 0.5f / s;

			qa[3] = (mElement[j][k] - mElement[k][j]) * s;
			qa[j] = (mElement[i][j] + mElement[j][i]) * s;
			qa[k] = (mElement[i][k] + mElement[k][i]) * s;

			rot[0] = qa[0];
			rot[1] = qa[1];
			rot[2] = qa[2];
			rot[3] = qa[3];
		}
	}

	void ComposeTransform(void)
	{
		IdentityTransform();
		mElement[3][0] = mPosition[0];
		mElement[3][1] = mPosition[1];
		mElement[3][2] = mPosition[2];
		ComposeOrientation();
	}

	void ComposeOrientation(void)
	{
		HeF32 w = mOrientation[3];

		HeF32 xx = mOrientation[0]*mOrientation[0];
		HeF32 yy = mOrientation[1]*mOrientation[1];
		HeF32 zz = mOrientation[2]*mOrientation[2];
		HeF32 xy = mOrientation[0]*mOrientation[1];
		HeF32 xz = mOrientation[0]*mOrientation[2];
		HeF32 yz = mOrientation[1]*mOrientation[2];
		HeF32 wx = w*mOrientation[0];
		HeF32 wy = w*mOrientation[1];
		HeF32 wz = w*mOrientation[2];

		mElement[0][0] = 1 - 2 * ( yy + zz );
		mElement[1][0] =     2 * ( xy - wz );
		mElement[2][0] =     2 * ( xz + wy );

		mElement[0][1] =     2 * ( xy + wz );
		mElement[1][1] = 1 - 2 * ( xx + zz );
		mElement[2][1] =     2 * ( yz - wx );

		mElement[0][2] =     2 * ( xz - wy );
		mElement[1][2] =     2 * ( yz + wx );
		mElement[2][2] = 1 - 2 * ( xx + yy );

	}

	HeI32 GetParentIndex(void) const { return mParentIndex; };
	const HeF32 * GetPosition(void) const { return mPosition; };
	const HeF32 * GetOrientation(void) const { return mOrientation; };

	char          mName[MAXSTRLEN];
	HeI32           mParentIndex;          // array index of parent bone
	HeF32         mPosition[3];
	HeF32         mOrientation[4];
	HeF32         mElement[4][4];
};

class MeshEntry
{
public:
private:
	char      mName[MAXSTRLEN];
	HeI32       mBone;         // bone this mesh is associcated
};

class Skeleton
{
public:
	Skeleton(const char *name)
	{
		strcpy(mName,name);
		mBoneCount = 0;
		mBones = 0;
	}

	Skeleton(const char *name,HeI32 bonecount)
	{
		strcpy(mName,name);
		mBoneCount = bonecount;
    mBones = MEMALLOC_NEW_ARRAY(Bone,bonecount)[bonecount];
	}

	Skeleton(FILE *fph);

	Skeleton(const Skeleton &sk)
	{
		strcpy(mName, sk.mName );
		mBoneCount = sk.mBoneCount;
		mBones = 0;
		if ( mBoneCount )
		{
			mBones = MEMALLOC_NEW_ARRAY(Bone,mBoneCount)[mBoneCount];
			memcpy(mBones,sk.mBones,sizeof(Bone)*mBoneCount);
		}
	}

	~Skeleton(void)
	{
		delete []mBones;
	}

	void SetName(const char *name)
	{
		strcpy(mName,name);
	}

	void SetBones(HeI32 bcount,Bone *bones) // memory ownership changes hands here!!!!!!!!!!1
	{
		mBoneCount = bcount;
		mBones     = bones;
	}

	void ComputeDefaultWeighting(const HeF32 *pos,HeF32 *weight,HeU16 &b1,HeU16 &b2,HeU16 &b3,HeU16 &b4) const;

	void FindFourClosest(const HeF32 *pos,HeF32 *closest,HeI32 *bones,HeF32 &fifthbone) const;

	HeI32 GetBoneCount(void) const { return mBoneCount; };

	const Bone& GetBone(HeI32 index) const { return mBones[index]; };

	Bone * GetBonePtr(HeI32 index) const { return &mBones[index]; };

	void SetBone(HeI32 index,const Bone &b) { mBones[index] = b; };

	const char * GetName(void) const { return mName; };

	void DebugReport(void) const;

	void Save(FILE *fph) const; // save a binary copy to disk.

private:
	char                   mName[MAXSTRLEN];
	HeI32                    mBoneCount;
	Bone                  *mBones;
};

#pragma warning(pop)

#endif

