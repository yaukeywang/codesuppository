// ===============================================================================
//						  AGEIA PHYSX SDK SAMPLE PROGRAMS
//					        PARTICLE FLUID
//
//		            Written by Simon Schirm 01-12-06
// ===============================================================================

#ifndef MYFLUID_H
#define MYFLUID_H

#ifdef PHYSX

#include "NxPhysics.h"

/**
Stores elements which can be received from the SDK.
Note, not all data here is used in all samples, some just make use of the position.
*/
struct ParticleSDK
{
	NxVec3	position;
	NxVec3  velocity;
	NxReal	density;
	NxReal  lifetime;
	NxU32	  id;
	NxVec3	collisionNormal;
};

/**
Stores user specific data.
Note, not all samples make use of user specific data.
*/
struct MyParticle
{
  float mSoil;
};

class MyFluid
{
public:

	MyFluid(NxScene* scene, NxFluidDesc &desc, bool trackUserData, bool provideCollisionNormals);
	~MyFluid();

	NxFluid*				getNxFluid(void) const { return mFluid; }

	const ParticleSDK*		getParticles(void) const { return mParticleBuffer; }
	unsigned				getParticlesNum(void) const { return mParticleBufferNum; }

	MyParticle*			getMyParticles(void) const { return mMyParticleBuffer; }

	const unsigned*	getCreatedIds(void) const { return mCreatedParticleIds; }
	unsigned				getCreatedIdsNum() { return mCreatedParticleIdsNum; }

	const unsigned*	getDeletedIds(void) const { return mDeletedParticleIds; }
	unsigned				getDeletedIdsNum() { return mDeletedParticleIdsNum; }

  NxU32           getMaxParticles(void) const { return mMaxParticles; };

	void draw(void);

private:

	unsigned			  mParticleBufferNum;
	ParticleSDK*		mParticleBuffer;

	NxFluid*			  mFluid;
	NxU32			  	  mMaxParticles;

	/**
	These fields are only relevant for tracking user partilce data (MyParticle)
	*/
	bool				    mTrackUserData;
	MyParticle*			mMyParticleBuffer;
	NxU32				    mCreatedParticleIdsNum;
	NxU32*			  	mCreatedParticleIds;
	NxU32				    mDeletedParticleIdsNum;
	NxU32*				  mDeletedParticleIds;

};

#endif

#endif
