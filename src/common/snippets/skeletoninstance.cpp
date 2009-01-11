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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "skeletoninstance.h"
#include "skeleton.h"
#include "animation.h"
#include "bonebinding.h"

#include "vector.h"
#include "quat.h"
#include "matrix.h"


#include "common/snippets/log.h"

class BoneBinding;
class Bone;

const HeI32 NO_PARENT=-1;

enum BoneTransformFlag
{
	BTF_HAS_SCALE  = (1<<0),
	BTF_HAS_PARENT = (1<<1),
};

enum SkeletonInstanceFlag
{
	SIF_HAS_HIERARCHY,           // has a hierachy
};

class BoneBinding;

class BonePose
{
public:

	void Set(const Bone &b)
	{
		mPosition.Set( b.GetPosition() );
		mOrientation.Set( b.GetOrientation() );
		memcpy(mTransform.Ptr(), b.GetTransform(), sizeof(HeF32)*16);
	}

	const MyMatrix & GetTransform(void) const { return mTransform; };

	void SetTransform(const MyMatrix &t)
	{
		mTransform = t;
	}

	void RebuildTransform(void)
	{
		mOrientation.QuatToMatrix(mTransform);
		mTransform.SetTranslation(mPosition);
	}

//private:
	Vector3d<HeF32> mPosition;
	Quat            mOrientation;
	MyMatrix        mTransform;
};

class BoneInstance
{
public:

	void DebugBone(const MyMatrix * /* bone */,const MyMatrix * /* parent */)
	{
#if USE_RENDER_DEBUG
		RenderDebug *rd = gGlobals.GetRenderDebug();
		Vector3d<HeF32> p(0,0,0);

		Vector3d<HeF32> p2(0.01f,0,0);

		Vector3d<HeF32> t1,t2,t3;

		bone->Transform(p,t1);

		bone->Transform(p2,t3);

		rd->DebugLine(t1.Ptr(), t3.Ptr(), 0xFFFF0000, 1200.0f );

		rd->DebugSphere( t1.Ptr(), 0.001f, 0xFFFFFFFF, 1200.0f );

		if ( parent )
		{
			parent->Transform(p,t2);
			rd->DebugLine( t1.Ptr(), t2.Ptr(), 0xFF00FF00, 1200.0f );
		}
#endif
	}

	void Set(const Bone &b,BoneBindingInterface *iface,BoneInstance *bones)
	{
		mName = SGET( b.GetName() );
		mParentIndex = b.GetParentIndex();
		mTrack = 0;


		if ( mParentIndex == NO_PARENT )
			mFlags = 0;
		else
			mFlags = BTF_HAS_PARENT;

		mRestPose.Set(b);

		mCurrentPose = mRestPose;

//    gLog->Display("Bone(%s) Parent(%d)\n", mName.Get(), mParentIndex );
//    mRestPose.GetTransform().Report("BoneTransform");

		if ( mParentIndex == NO_PARENT )
		{
			mRestWorld = mRestPose.GetTransform();

			//DebugBone( &mRestWorld, 0 );

		}
		else
		{
			mRestWorld.Multiply( mRestPose.GetTransform(), bones[mParentIndex].GetRestWorld()  );
			//DebugBone( &mRestWorld, &bones[mParentIndex].GetRestWorld() );
		}

		mRestWorld.Invert(mRestInverse);

//    if ( strcmp(mName,"Bip01") == 0 )
//      mBinding = 0;
//    else
		{
			if ( iface )
				mBinding = iface->GetBoneBinding( mName );
			else
				mBinding = 0;
		}

	}

	void Compose(MyMatrix &composite,BoneInstance *bones)
	{

		if ( mParentIndex == NO_PARENT )
		{

			if ( mBinding && !mTrack )
			{
				mWorld.Set( mBinding->GetTransform() );
			}
			else
			{
				mWorld = mCurrentPose.GetTransform();
			}


		}
		else
		{
			if ( mBinding && !mTrack )
			{

				const MyMatrix *m = (const MyMatrix *) mBinding->GetTransform();
#if 1
				mWorld = *m;
#else
			 MyMatrix invert;
			 invert.InvertUnscaled(bones[mParentIndex].GetWorld());
			 mCurrentPose.mTransform.Multiply( *m, invert );
			 mWorld.Multiply(mCurrentPose.mTransform, bones[mParentIndex].GetWorld() );
#endif


			}
			else
			{
				mWorld.Multiply(mCurrentPose.mTransform, bones[mParentIndex].GetWorld() );
			}

			if ( mBinding && mTrack )
			{
				mBinding->SetTransform( mWorld.Ptr() );
			}

		}

		composite.Multiply( mRestInverse, mWorld );

	}

	void SetWorld(const MyMatrix &world)
	{
		mCurrentPose.SetTransform(world);
	}

	void SetMultiplyWorld(const MyMatrix &world)
	{
		mCurrentPose.mTransform.Multiply( mRestPose.mTransform, world );
	}

	const MyMatrix& GetWorld(void) const { return mWorld; };
	const MyMatrix& GetRestWorld(void) const { return mRestWorld; };
	const MyMatrix& GetRestInverse(void) const { return mRestInverse; };

#if USE_RENDER_DEBUG
	void DebugRender(RenderDebug *rd,BoneInstance *bones)
	{
		Vector3d<HeF32> p(0,0,0);
		Vector3d<HeF32> t;

		mWorld.Transform(p,t);

		const char *select = gGlobals.GetJointSelect();

		if ( select && stricmp(select,mName) == 0 )
		{
			rd->DebugSphere( t.Ptr(), 0.020f, 0xFFFFFFFF );
		}

		if ( mParentIndex == NO_PARENT )
		{
			rd->DebugText( mName.Get(), t.Ptr(), 0xFFFFFFFF );
			rd->DebugSphere( t.Ptr(), 0.005f, 0xFFFFFFFF );
		}
		else
		{
			HeU32 color1 = 0xFFFFFF00;
			HeU32 color2 = 0xFF00FF00;
			rd->DebugText( mName.Get(), t.Ptr(), color1 );
			rd->DebugSphere( t.Ptr(), 0.004f, color1 );

			const MyMatrix &w = bones[mParentIndex].GetWorld();
			Vector3d<HeF32> t2;
			w.Transform(p,t2);
			rd->DebugLine( t.Ptr(), t2.Ptr(), color2);
		}
	}
#endif

	const StringRef& GetName(void) const { return mName; };

	StringRef GetBoneParentName(const BoneInstance *bones)
	{
		StringRef ret;
		if ( mParentIndex != NO_PARENT )
		{
			ret = bones[mParentIndex].GetName();
		}
		return ret;
	}

	void SetVelocity(const Vector3d<HeF32> &v)
	{
		if ( mBinding )
		{
			mBinding->SetVelocity( v.Ptr() );
		}
	}

	void StopAnimation(void)
	{
		if ( mTrack )
		{
			if ( mBinding ) mBinding->SetKinematic(false);
			mTrack = 0;
//      mBinding = 0;
		}
	}

	void PlayAnimation(const Animation *a)
	{
		{
			mTrack = a->LocateTrack(mName);

			bool kinematic = true;

			if ( strcmp(mName,"Bip01") == 0 )
			{
				kinematic = false;
				mBinding = 0;
			}

//			if ( stristr(mName,"tail") != 0 ) kinematic = false;
//      if ( strstr(mName,"neck") != 0 || strstr(mName,"Neck") != 0 ) kinematic = false;
//      if ( strstr(mName,"head") != 0 || strstr(mName,"Head") != 0 ) kinematic = false;
//      if ( strstr(mName,"jaw") != 0 || strstr(mName,"Jaw") != 0 ) kinematic = false;


			if (  kinematic )
			{
				if ( mBinding && mTrack )
				{
					mBinding->SetKinematic(true);
				}
			}
			else
			{
				mTrack = 0;
			}
		}
	}

	void SampleAnimation(HeI32 frame)
	{
		if ( mTrack )
		{
			mTrack->SampleAnimation(frame, mCurrentPose.mPosition.Ptr(), mCurrentPose.mOrientation.Ptr() );
			mCurrentPose.RebuildTransform();
		}
	}

private:
	StringRef       mName;                   // name of the bone
	HeI32             mFlags;
	HeI32             mParentIndex;            // parent bone index
	BonePose        mRestPose;               // the resting pose for the bone (in object space if parent relative)
	MyMatrix        mRestWorld;              // resting world space
	MyMatrix        mRestInverse;            // the inverse of resting pose in world space.
	MyMatrix        mWorld;                  // the world matrix
	BonePose        mCurrentPose;            // the current pose
	BoneBinding    *mBinding;
	const AnimTrack *mTrack;
};



SkeletonInstance::SkeletonInstance(const Skeleton *sk,BoneBindingInterface *iface)
{
	mInterface = iface;
  if ( sk )
  {
  	mBoneCount = sk->GetBoneCount();
  	mName      = SGET( sk->GetName() );
  }
  else
  {
    mBoneCount = 1;
    mName = "root";
  }

	mFlags     = 0;
	mBones = MEMALLOC_NEW_ARRAY(BoneInstance,mBoneCount)[mBoneCount];
	mComposite = MEMALLOC_NEW_ARRAY(MyMatrix,mBoneCount)[mBoneCount];
	for (HeI32 i=0; i<mBoneCount; i++)
	{
    if ( sk )
    {
  		const Bone& b = sk->GetBone(i);
	  	mBones[i].Set(b,iface,mBones);
    }
    else
    {
      Bone b;
	  	mBones[i].Set(b,iface,mBones);
    }
	}

	mAnimation = 0;
	mTime = 0;

//  sk->DebugReport();

}


SkeletonInstance::~SkeletonInstance(void)
{
	delete []mBones;
	delete []mComposite;
}

const MyMatrix * SkeletonInstance::ReCompose(const MyMatrix *root,HeF32 ftime)
{
	mTime = ftime;
	return Compose(root,0);
}

const MyMatrix * SkeletonInstance::Compose(const MyMatrix *root,HeF32 dtime)
{
	if ( root ) //&& !mInterface )
	{
		mBones[0].SetMultiplyWorld(*root);
	}

	if ( mAnimation )
	{
		mTime+=dtime;
		HeI32 frame = mAnimation->GetFrameIndex(mTime);
		for (HeI32 i=0; i<mBoneCount; i++)
		{
			mBones[i].SampleAnimation(frame); // sample the animation at this frame
		}
	}

	for (HeI32 i=0; i<mBoneCount; i++)
	{
		mBones[i].Compose( mComposite[i], mBones );
	}


	return mComposite;
}


void SkeletonInstance::DebugRender(void) // debug render the skeleton
{
#if USE_RENDER_DEBUG
	RenderDebug *rd = gGlobals.GetRenderDebug();

	for (HeI32 i=0; i<mBoneCount; i++)
	{
		mBones[i].DebugRender(rd,mBones);
	}
#endif
}



const StringRef & SkeletonInstance::GetBoneName(HeI32 index) const
{
	return mBones[index].GetName();
}

const MyMatrix & SkeletonInstance::GetBoneTransform(HeI32 index) const
{
	return mBones[index].GetRestWorld();
}

const MyMatrix & SkeletonInstance::GetBoneInverseTransform(HeI32 index) const
{
	return mBones[index].GetRestInverse();
}

StringRef SkeletonInstance::GetBoneParentName(HeI32 index) const
{
	return mBones[index].GetBoneParentName(mBones);
}


void SkeletonInstance::PlayAnimation(const Animation *a)
{
	mTime = 0;
	mAnimation = a;

	for (HeI32 i=0; i<mBoneCount; i++)
	{
		mBones[i].PlayAnimation(a);
	}

}
void SkeletonInstance::StopAnimation(void)
{
	mAnimation = 0;
	for (HeI32 i=0; i<mBoneCount; i++)
	{
		mBones[i].StopAnimation();
	}

}

void SkeletonInstance::SetVelocity(const Vector3d<HeF32> &v)
{
	for (HeI32 i=0; i<mBoneCount; i++)
	{
		mBones[i].SetVelocity(v);
	}
}


