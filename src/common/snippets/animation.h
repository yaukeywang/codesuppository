#ifndef ANIMATION_H

#define ANIMATION_H

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



#include <stdio.h>
#include <string.h>
#include <float.h>

#include "common/snippets/UserMemAlloc.h"

#pragma warning(push)
#pragma warning(disable:4996)

class AnimPose
{
public:

	void SetPose(const HeF32 *pos,const HeF32 *quat)
	{
		mPos[0] = pos[0];
		mPos[1] = pos[1];
		mPos[2] = pos[2];
		mQuat[0] = quat[0];
		mQuat[1] = quat[1];
		mQuat[2] = quat[2];
		mQuat[3] = quat[3];
	};

	void Sample(HeF32 *pos,HeF32 *quat) const
	{
		pos[0] = mPos[0];
		pos[1] = mPos[1];
		pos[2] = mPos[2];
		quat[0] = mQuat[0];
		quat[1] = mQuat[1];
		quat[2] = mQuat[2];
		quat[3] = mQuat[3];
	}

	void Save(FILE *fph) const
	{
		fwrite(mPos,  sizeof(HeF32)*3, 1, fph );
		fwrite(mQuat, sizeof(HeF32)*4, 1, fph );
	}

	void Load(FILE *fph)
	{
		fread(mPos,  sizeof(HeF32)*3, 1, fph );
		fread(mQuat, sizeof(HeF32)*4, 1, fph );
	}

	HeF32 mPos[3];
	HeF32 mQuat[4];
};

class AnimTrack
{
public:

	AnimTrack(HeI32 framecount,
						HeF32 duration,
						HeF32 dtime)
	{
		mName[0] = 0;
		mFrameCount = framecount;
		mPose = MEMALLOC_NEW_ARRAY(AnimPose,mFrameCount)[mFrameCount];
		mDuration   = duration;
		mDtime      = dtime;
	}

	AnimTrack(const AnimTrack &c)
	{
		strcpy(mName, c.mName );
		mFrameCount = c.mFrameCount;
		mDuration   = c.mDuration;
		mDtime      = c.mDtime;
		mPose = MEMALLOC_NEW_ARRAY(AnimPose,mFrameCount)[mFrameCount];
		for (HeI32 i=0; i<mFrameCount; i++)
		{
			mPose[i] = c.mPose[i];
		}
	}

	AnimTrack(FILE *fph)
	{
		fread(mName, sizeof(mName), 1, fph );
		fread(&mFrameCount, sizeof(HeI32), 1, fph );
		fread(&mDuration, sizeof(HeF32), 1, fph );
		fread(&mDtime, sizeof(HeF32), 1, fph );
		mPose = MEMALLOC_NEW_ARRAY(AnimPose,mFrameCount)[mFrameCount];
		for (HeI32 i=0; i<mFrameCount; i++)
			mPose[i].Load(fph);
	}

	~AnimTrack(void)
	{
		delete []mPose;
	}

	void SetName(const char *name)
	{
    if ( name )
  		strncpy(mName,name,256);
    else
      mName[0] = 0;
	}

	void SetPose(HeI32 frame,const HeF32 *pos,const HeF32 *quat)
	{
		if ( frame >= 0 && frame < mFrameCount )
			mPose[frame].SetPose(pos,quat);
	}

	const char * GetName(void) const { return mName; };

	void SampleAnimation(HeI32 frame,HeF32 *pos,HeF32 *quat) const
	{
		mPose[frame].Sample(pos,quat);
	}

	void Save(FILE *fph) const
	{
		fwrite(mName, sizeof(mName), 1, fph );
		fwrite(&mFrameCount, sizeof(HeI32), 1, fph );
		fwrite(&mDuration, sizeof(HeF32), 1, fph );
		fwrite(&mDtime, sizeof(HeF32), 1, fph );
		for (HeI32 i=0; i<mFrameCount; i++)
			mPose[i].Save(fph);
	}

	HeI32 GetFrameCount(void) const { return mFrameCount; };

	AnimPose * GetPose(HeI32 index) { return &mPose[index]; };

private:
	char      mName[256]; // name of the track.
	HeI32       mFrameCount;
	HeF32     mDuration;
	HeF32     mDtime;
	AnimPose *mPose;
};

class Animation
{
public:
	Animation(const char *name,HeI32 trackcount,HeI32 framecount,HeF32 duration,HeF32 dtime)
	{
		strncpy(mName,name,256);
		mTrackCount = trackcount;
		mFrameCount = framecount;
		mTracks = (AnimTrack **) MEMALLOC_MALLOC(sizeof(AnimTrack*)*mTrackCount);\
		mDuration  = duration;
		mDtime     = dtime;
		for (HeI32 i=0; i<trackcount; i++)
		{
			mTracks[i] = MEMALLOC_NEW(AnimTrack)(framecount,duration,dtime);
		}
	}

	Animation(const Animation &c) // construct animation by copying an existing one
	{
		strcpy(mName, c.mName );
		mTrackCount = c.mTrackCount;
		mFrameCount = c.mFrameCount;
		mDuration   = c.mDuration;
		mDtime      = c.mDtime;
		mTracks     = (AnimTrack **) MEMALLOC_MALLOC(sizeof(AnimTrack*)*mTrackCount);
		for (HeI32 i=0; i<mTrackCount; i++)
		{
			mTracks[i] = MEMALLOC_NEW(AnimTrack)( *c.mTracks[i] );
		}
	}

	Animation(FILE *fph)
	{
		// need to implement..
		fread(mName,sizeof(mName),1,fph);
		fread(&mTrackCount, sizeof(HeI32), 1, fph );
		fread(&mFrameCount, sizeof(HeI32), 1, fph );
		fread(&mDuration, sizeof(HeF32), 1, fph );
		fread(&mDtime, sizeof(HeF32), 1, fph );
		if (mTrackCount )
		{
			mTracks = (AnimTrack **) MEMALLOC_MALLOC(sizeof(AnimTrack *)*mTrackCount);
			for (HeI32 i=0; i<mTrackCount; i++)
			{
				AnimTrack *t = MEMALLOC_NEW(AnimTrack)(fph);
				mTracks[i] = t;
			}
		}
		else
		{
			mTracks = 0;
		}
	}


	~Animation(void)
	{
		for (HeI32 i=0; i<mTrackCount; i++)
		{
			AnimTrack *at = mTracks[i];
			delete at;
		}
		MEMALLOC_FREE(mTracks);
	}

	void SetName(const char *name)
	{
		strcpy(mName,name);
	}

	void SetTrackName(HeI32 track,const char *name)
	{
		mTracks[track]->SetName(name);
	}

	void SetTrackPose(HeI32 track,HeI32 frame,const HeF32 *pos,const HeF32 *quat)
	{
		mTracks[track]->SetPose(frame,pos,quat);
	}

	const char * GetName(void) const { return mName; };

	const AnimTrack * LocateTrack(const char *name) const
	{
		const AnimTrack *ret = 0;
		for (HeI32 i=0; i<mTrackCount; i++)
		{
			const AnimTrack *t = mTracks[i];
			if ( stricmp(t->GetName(),name) == 0 )
			{
				ret = t;
				break;
			}
		}
		return ret;
	}

	HeI32 GetFrameIndex(HeF32 t) const
	{
		t = fmodf( t, mDuration );
		HeI32 index = HeI32(t / mDtime);
		return index;
	}

	void Save(FILE *fph) const
	{
		fwrite(mName,sizeof(mName),1,fph);
		fwrite(&mTrackCount, sizeof(HeI32), 1, fph );
		fwrite(&mFrameCount, sizeof(HeI32), 1, fph );
		fwrite(&mDuration, sizeof(HeF32), 1, fph );
		fwrite(&mDtime, sizeof(HeF32), 1, fph );
		for (HeI32 i=0; i<mTrackCount; i++)
		{
			mTracks[i]->Save(fph);
		}
	}

	HeI32 GetTrackCount(void) const { return mTrackCount; };
	HeF32 GetDuration(void) const { return mDuration; };

	AnimTrack * GetTrack(HeI32 index)
	{
		AnimTrack *ret = 0;
		if ( index >= 0 && index < mTrackCount )
		{
			ret = mTracks[index];
		}
		return ret;
	};

	HeI32 GetFrameCount(void) const { return mFrameCount; };
	HeF32 GetDtime(void) const { return mDtime; };

private:
	char        mName[256];
	HeI32         mTrackCount;
	HeI32         mFrameCount;
	HeF32       mDuration;
	HeF32       mDtime;
	AnimTrack **mTracks;
};

#pragma warning(pop)

#endif
