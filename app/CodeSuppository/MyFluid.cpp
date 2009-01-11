#include "MyFluid.h"

#ifdef PHYSX

#include "RenderDebug/RenderDebug.h"
#include "common/snippets/SendTextMessage.h"

/**
Note: There is no error handling code for simplicity. If any SDK code fails, an assert is hit.
*/
MyFluid::MyFluid(NxScene* scene, NxFluidDesc &desc, bool trackUserData, bool provideCollisionNormals)
:	mParticleBufferNum(0),
	mParticleBuffer(NULL),
	mFluid(NULL),
	mTrackUserData(trackUserData),
	mMyParticleBuffer(NULL),
	mCreatedParticleIdsNum(0),
	mCreatedParticleIds(NULL),
	mDeletedParticleIdsNum(0),
	mDeletedParticleIds(NULL)
{
	mMaxParticles = desc.maxParticles;
	mParticleBuffer = new ParticleSDK[mMaxParticles];
	desc.userData = this;

	//Setup particle write data.
	NxParticleData particleData;
	particleData.numParticlesPtr = &mParticleBufferNum;
	particleData.bufferPos = &mParticleBuffer[0].position.x;
	particleData.bufferPosByteStride = sizeof(ParticleSDK);
	particleData.bufferVel = &mParticleBuffer[0].velocity.x;
	particleData.bufferVelByteStride = sizeof(ParticleSDK);
	particleData.bufferDensity = &mParticleBuffer[0].density;
	particleData.bufferDensityByteStride = sizeof(ParticleSDK);
	particleData.bufferLife = &mParticleBuffer[0].lifetime;
	particleData.bufferLifeByteStride = sizeof(ParticleSDK);
	particleData.bufferId = &mParticleBuffer[0].id;
	particleData.bufferIdByteStride = sizeof(ParticleSDK);
	particleData.bufferDensity = &mParticleBuffer[0].density;
	particleData.bufferDensityByteStride = sizeof(ParticleSDK);

	if (provideCollisionNormals)
	{
		particleData.bufferCollisionNormal = &mParticleBuffer[0].collisionNormal.x;
		particleData.bufferCollisionNormalByteStride = sizeof(ParticleSDK);
	}

	desc.particlesWriteData = particleData;

	//User data buffers
	if (mTrackUserData)
	{
		mMyParticleBuffer = new MyParticle[mMaxParticles];
        mCreatedParticleIds = new NxU32[mMaxParticles];
        mDeletedParticleIds = new NxU32[mMaxParticles];

		//Setup id write data.
		NxParticleIdData idData;
		
		//Creation
		idData.numIdsPtr = &mCreatedParticleIdsNum;
		idData.bufferId = mCreatedParticleIds;
		idData.bufferIdByteStride = sizeof(NxU32);
		desc.particleCreationIdWriteData = idData;
		
		//Deletion
		idData.numIdsPtr = &mDeletedParticleIdsNum;
		idData.bufferId = mDeletedParticleIds;
		idData.bufferIdByteStride = sizeof(NxU32);
		desc.particleDeletionIdWriteData = idData;
	}

	assert(scene);
	mFluid = scene->createFluid(desc);
	assert(mFluid);
}

MyFluid::~MyFluid()
{
	delete[] mParticleBuffer;
	mParticleBuffer = NULL;
	mFluid->getScene().releaseFluid(*mFluid);

	if (mTrackUserData)
	{
		delete[] mMyParticleBuffer;
		mMyParticleBuffer = NULL;
		delete[] mCreatedParticleIds;
		mCreatedParticleIds = NULL;
		delete[] mDeletedParticleIds;
		mDeletedParticleIds = NULL;
	}

}

void MyFluid::draw()
{
#if 0
  static unsigned int maxp = 0;


  if ( mParticleBufferNum > maxp )
  {
    maxp = mParticleBufferNum;
    SEND_TEXT_MESSAGE(0,"MaxParticles: %d\r\n", maxp );
  }
  for (unsigned i=0; i<mParticleBufferNum; i++)
  {
    const ParticleSDK& particle = mParticleBuffer[i];

    NxVec3 p2 = particle.position + particle.velocity;

    NxVec3 p3 = particle.position + particle.collisionNormal;

    gRenderDebug->DebugLine(&particle.position.x,&p2.x,0xFFFFFF);
    gRenderDebug->DebugLine(&particle.position.x,&p3.x,0xFF0000);

//    gRenderDebug->DebugPoint(&particle.position.x,0.05f,0xFFFFFF);
  }
#endif
}

#endif
