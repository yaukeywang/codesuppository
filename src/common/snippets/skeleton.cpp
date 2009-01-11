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

#include "skeleton.h"

#include <float.h>
#include <math.h>


void Skeleton::FindFourClosest(const HeF32 *pos,HeF32 *closest,HeI32 *bones,HeF32 &fifthbone) const
{
	closest[0] = FLT_MAX;
	closest[1] = FLT_MAX;
	closest[2] = FLT_MAX;
	closest[3] = FLT_MAX;

	fifthbone  = FLT_MAX;

	bones[0]   = 0;
	bones[1]   = 0;
	bones[2]   = 0;
	bones[3]   = 0;

	Bone *b = mBones;
	for (HeI32 i=0; i<mBoneCount; i++,b++)
	{
		HeF32 bpos[3];

		b->GetPos(bpos);

		HeF32 dx = bpos[0] - pos[0];
		HeF32 dy = bpos[1] - pos[1];
		HeF32 dz = bpos[2] - pos[2];

		HeF32 distance = dx*dx+dy*dy+dz*dz;

		if ( distance < closest[0] )
		{
			fifthbone  = closest[3];
			closest[3] = closest[2];
			bones[3]   = bones[2];

			closest[2] = closest[1];
			bones[2]   = bones[1];

			closest[1] = closest[0];
			bones[1]   = bones[0];

			closest[0] = distance;
			bones[0]   = i;

		}
		else if ( distance < closest[1] )
		{
			fifthbone  = closest[3];
			closest[3] = closest[2];
			bones[3]   = bones[2];

			closest[2] = closest[1];
			bones[2]   = bones[1];

			closest[1] = distance;
			bones[1]   = i;
		}
		else if ( distance < closest[2] )
		{
			fifthbone  = closest[3];
			closest[3] = closest[2];
			bones[3]   = bones[2];

			closest[2]  = distance;
			bones[2]    = i;
		}
		else if ( distance < closest[3] )
		{
			fifthbone  = closest[3];
			closest[3] = distance;
			bones[3]   = i;
		}
	}

	closest[0] = sqrtf( closest[0] );
	closest[1] = sqrtf( closest[1] );
	closest[2] = sqrtf( closest[2] );
	closest[3] = sqrtf( closest[3] );

	fifthbone  = sqrtf(fifthbone);
}

void Skeleton::ComputeDefaultWeighting(const HeF32 *pos,HeF32 *weight,HeU16 &b1,HeU16 &b2,HeU16 &b3,HeU16 &b4) const
{

	HeF32 closest[4];
	HeI32   bones[4];
	HeF32 furthest;

	FindFourClosest(pos,closest,bones,furthest);

	HeF32 recip = 1.0f / furthest;

	weight[0] = (furthest-closest[0]) * recip;
	weight[1] = (furthest-closest[1]) * recip;
	weight[2] = (furthest-closest[2]) * recip;
	weight[3] = (furthest-closest[3]) * recip;

	HeF32 total = weight[0] + weight[1] + weight[2] + weight[3];

	recip = 1.0f / total;

	weight[0] = weight[0]*recip;
	weight[1] = weight[1]*recip;
	weight[2] = weight[2]*recip;
	weight[3] = weight[3]*recip;

	total = weight[0] + weight[1] + weight[2] + weight[3];

	b1 = (HeU16)bones[0];
	b2 = (HeU16)bones[1];
	b3 = (HeU16)bones[2];
	b4 = (HeU16)bones[3];

}


void Skeleton::DebugReport(void) const
{
	for (HeI32 i=0; i<mBoneCount; i++)
	{
		const Bone &b = mBones[i];
		const char *foo = "no parent";
		if ( b.GetParentIndex() >= 0 )
		{
			foo = mBones[ b.GetParentIndex() ].GetName();
		}
		printf("Bone%d : %s  ParentIndex(%d) Parent(%s)\n", i, b.GetName(), b.GetParentIndex(), foo );
	}
}

Skeleton::Skeleton(FILE *fph)
{
	fread(mName, MAXSTRLEN, 1, fph);
	fread(&mBoneCount, sizeof(HeI32), 1, fph );
	mBones = MEMALLOC_NEW_ARRAY(Bone,mBoneCount)[mBoneCount];
	fread(mBones, sizeof(Bone)*mBoneCount, 1, fph );
}

void Skeleton::Save(FILE *fph) const
{
	fwrite(mName,MAXSTRLEN,1,fph);
	fwrite(&mBoneCount, sizeof(HeI32),1,fph);
	fwrite(mBones, sizeof(Bone)*mBoneCount, 1, fph );
}


