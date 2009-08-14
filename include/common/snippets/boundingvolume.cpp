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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "boundingvolume.h"

static NxU32   MINLEAF=4;     //  leaf nodes down to 2 leaves
static NxU32   MAXDEPTH=12;   //  no more than 12 deep allowed!
static NxF32 MINAXIS=2;     //  2 meters is fine enough

BoundingVolumeSystem::BoundingVolumeSystem(NxU32 minleaf,NxU32 maxdepth,NxF32 minaxis)
{
	mMinLeaf  = minleaf;
	mMaxDepth = maxdepth;
	mMinAxis  = minaxis;
	mRoot     = new BoundingVolumeAABV;
	mBuilt    = false;
	mVolumeFrameNo  = 0;
}

void BoundingVolumeSystem::Build(void)
{
	if ( mBuilt == false )
	{
		mBuilt = true;
		mRoot->Split(this,0);
	}
}

BoundingVolumeSystem::~BoundingVolumeSystem(void)
{
	delete mRoot;
}


BoundingVolumeAABV::~BoundingVolumeAABV(void)
{
	delete mLeft;
	delete mRight;
}

void BoundingVolumeAABV::Split(BoundingVolumeSystem *system,NxU32 depth)
{
	NxU32 count = (NxU32)mList.size();

	NxF32 dx = mBound.mMax[0] - mBound.mMin[0];  // JWR
	NxF32 dy = mBound.mMax[1] - mBound.mMin[1];
	NxF32 dz = mBound.mMax[2] - mBound.mMin[2];

	NxF32 laxis = dx;

	NxU32 axis = 0;

	if ( dy > dx )
	{
		axis = 1;
		laxis = dy;
	}

	if ( dz > dx && dz > dy )
	{
		axis = 2;
		laxis = dz;
	}


	if ( count < MINLEAF || depth >= MAXDEPTH || laxis < MINAXIS )
	{
		SetBoundingVolumeFlag(BV_LEAF);
	}
	else
	{
		NxF32 midpoint[3];
		mBound.GetCenter(midpoint);
		BuildTwo( system,  axis, midpoint );

		if ( mLeft  )
			mLeft->Split(system,depth+1);

		if ( mRight )
			mRight->Split(system,depth+1);

	}


}

void BoundingVolumeAABV::BuildTwo(BoundingVolumeSystem *aabv,NxU32 axis,const NxF32 *midpoint)
{
	BRect3d b1,b2;

	SplitRect(axis,mBound,b1,b2,midpoint);

	mLeft  = aabv->GetBoundingVolumeAABV( b1, mList ); // split the portions of the list that fit into the left side
	mRight = aabv->GetBoundingVolumeAABV( b2, mList ); // split the portions of the list that fit into the right side

	mList.clear();

}

void BoundingVolumeAABV::SplitRect(NxU32 axis,
																	 const BRect3d &source,
																	 BRect3d &b1,
																	 BRect3d &b2,
																	 const NxF32 *midpoint)
{
	switch ( axis )
	{
		case 0:
			{
				SetBoundingVolumeFlag(BV_XAXIS);
				b1.SetMin(source.mMin);
				b1.SetMax( midpoint[0], source.mMax[1], source.mMax[2] );
				b2.SetMin( midpoint[0], source.mMin[1], source.mMin[2] );
				b2.SetMax(source.mMax);
			}
			break;
		case 1:
			{
				SetBoundingVolumeFlag(BV_YAXIS);
				b1.SetMin(source.mMin);
				b1.SetMax( source.mMax[0], midpoint[1], source.mMax[2] );
				b2.SetMin( source.mMin[0], midpoint[1], source.mMin[2] );
				b2.SetMax(source.mMax);
			}
			break;
		case 2:
			{
				SetBoundingVolumeFlag(BV_ZAXIS);
				b1.SetMin(source.mMin);
				b1.SetMax( source.mMax[0], source.mMax[1], midpoint[2] );
				b2.SetMin( source.mMin[0], source.mMin[1], midpoint[2] );
				b2.SetMax(source.mMax);
			}
			break;

	}
}

BoundingVolumeAABV * BoundingVolumeSystem::GetBoundingVolumeAABV(const BRect3d &bound,const BoundingVolumeVector &list)
{
	BRect3d b;

	b.InitMinMax();

	BoundingVolumeAABV *ret = new BoundingVolumeAABV;

	NxU32 count = 0;
	BoundingVolumeVector::const_iterator i;
	for (i=list.begin(); i!=list.end(); ++i)
	{
		BoundingVolume *item = (*i);
		BRect3d ibound;
		item->GetBound(ibound);
		if ( bound.Intersect( ibound ) )
		{
			b.MinMax( ibound );
			count++;
			ret->Add( item );
		}
	}

	if ( count )
	{

		NxU32 wrap = 0;

		if ( b.mMin[0] < bound.mMin[0] ) { b.mMin[0] = bound.mMin[0]; wrap++; };
		if ( b.mMin[1] < bound.mMin[1] ) { b.mMin[1] = bound.mMin[1]; wrap++; };
		if ( b.mMin[2] < bound.mMin[2] ) { b.mMin[2] = bound.mMin[2]; wrap++; };

		if ( b.mMax[0] > bound.mMax[0] ) { b.mMax[0] = bound.mMax[0]; wrap++; };
		if ( b.mMax[1] > bound.mMax[1] ) { b.mMax[1] = bound.mMax[1]; wrap++; };
		if ( b.mMax[2] > bound.mMax[2] ) { b.mMax[2] = bound.mMax[2]; wrap++; };

		ret->SetBound(b);

		return ret;
	}

	delete ret;

	return 0;
}


void BoundingVolumeSystem::Render(FrustumInterface *iface)
{
	if ( !mBuilt )
	{
		Build();
	}
	mVolumeFrameNo++;
	mRoot->Render(VT_PARTIAL,iface,mVolumeFrameNo);
}


void BoundingVolumeAABV::Render(ViewTest state,FrustumInterface *iface,NxU32 frameno)
{
	if ( state & VT_PARTIAL  ) // if still partial
	{
		state = iface->ViewTestAABB( mBound.mMin, state );
		if ( state & VT_OUTSIDE ) return;
	}

	if ( HasBoundingVolumeFlag(BV_LEAF) )
	{
		BoundingVolumeVector::iterator i;
		for (i=mList.begin(); i!=mList.end(); ++i)
		{
			BoundingVolume *item = (*i);
			if ( item->GetVolumeFrameNo() != frameno ) // if not rendered yet on this frame.
			{
				item->SetVolumeFrameNo(frameno);
				item->Render(iface,state);
			}
		}
	}
	else
	{

		if ( mLeft )
			mLeft->Render(state,iface,frameno);

		if ( mRight )
			mRight->Render(state,iface,frameno);
	}


}


void BRect3d::InitMinMax(void)
{
	mMin[0] =  FLT_MAX;
	mMin[1] =  FLT_MAX;
	mMin[2] =  FLT_MAX;

	mMax[0] = -FLT_MAX;
	mMax[1] = -FLT_MAX;
	mMax[2] = -FLT_MAX;
}

bool BRect3d::Intersect(const BRect3d &test) const
{
	if ( test.mMax[2] < mMin[2] ) return false;
	if ( test.mMax[1] < mMin[1] ) return false;
	if ( test.mMax[0] < mMin[0] ) return false;

	if ( test.mMin[2] > mMax[2] ) return false;
	if ( test.mMin[1] > mMax[1] ) return false;
	if ( test.mMin[0] > mMax[0] ) return false;

	return true;
}

void BRect3d::MinMax(const BRect3d &b)
{
	if ( b.mMin[0] < mMin[0] ) mMin[0] = b.mMin[0];
	if ( b.mMin[1] < mMin[1] ) mMin[1] = b.mMin[1];
	if ( b.mMin[2] < mMin[2] ) mMin[2] = b.mMin[2];

	if ( b.mMax[0] > mMax[0] ) mMax[0] = b.mMax[0];
	if ( b.mMax[1] > mMax[1] ) mMax[1] = b.mMax[1];
	if ( b.mMax[2] > mMax[2] ) mMax[2] = b.mMax[2];
}

void BRect3d::GetCenter(NxF32 *center) const
{
	center[0] = (mMax[0] - mMin[0])*0.5f + mMin[0];
	center[1] = (mMax[1] - mMin[1])*0.5f + mMin[1];
	center[2] = (mMax[2] - mMin[2])*0.5f + mMin[2];
}

void BRect3d::SetMin(const NxF32 *m)
{
	mMin[0] = m[0];
	mMin[1] = m[1];
	mMin[2] = m[2];
}

void BRect3d::SetMax(const NxF32 *m)
{
	mMax[0] = m[0];
	mMax[1] = m[1];
	mMax[2] = m[2];
}

void BRect3d::SetMin(NxF32 x,NxF32 y,NxF32 z)
{
	mMin[0] = x;
	mMin[1] = y;
	mMin[2] = z;
}

void BRect3d::SetMax(NxF32 x,NxF32 y,NxF32 z)
{
	mMax[0] = x;
	mMax[1] = y;
	mMax[2] = z;
}


NxF32 BRect3d::GetDistance2(void) const
{
	NxF32 dx = mMax[0] - mMin[0];
	NxF32 dy = mMax[1] - mMin[1];
	NxF32 dz = mMax[2] - mMin[2];
	return dx*dx + dy*dy + dz*dz;
}
