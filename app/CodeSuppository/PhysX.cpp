#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "PhysX.h"

//#define PHYSX

#ifdef PHYSX

#pragma warning(disable:4189)
#pragma comment( lib, "NxCharacter" )
#pragma comment( lib, "NxCooking" )
#pragma comment( lib, "PhysXLoader" )


#include "NxPhysics.h"
#include "NxCooking.h"
#include "NxActor.h"
#include "NxShape.h"
#include "NxTriangleMesh.h"
#include "NxTriangleMeshShape.h"
#include "fluids/NxFluid.h"
#include "fluids/NxFluidDesc.h"
#include "fluids/NxFluidEmitter.h"
#include "fluids/NxFluidEmitterDesc.h"
#include "MyFluid.h"
#include "terrain.h"
#include "PhysX.h"

#include "common/HeMath/HeFoundation.h"
#include "RenderDebug/RenderDebug.h"

#pragma warning(disable:4100)

#define EMITTER_DX 16
#define EMITTER_DY 16
#define STEP_SIZE   0.1
#define FLOW_RATE 9000
#define MAX_PARTICLES 60000
#define PARTICLE_LIFE 7


PhysX *gPhysX=0;

//*** Utility code
class NxPMap;
class NxTriangleMesh;
class NxUserOutputStream;

bool hasCookingLibrary(); // check to see if the cooking library is available or not!
bool InitCooking(NxUserAllocator* allocator = NULL, NxUserOutputStream* outputStream = NULL);
void CloseCooking();
bool CookConvexMesh(const NxConvexMeshDesc& desc, NxStream& stream);
bool CookClothMesh(const NxClothMeshDesc& desc, NxStream& stream);
bool CookTriangleMesh(const NxTriangleMeshDesc& desc, NxStream& stream);
bool CookSoftBodyMesh(const NxSoftBodyMeshDesc& desc, NxStream& stream);
bool CreatePMap(NxPMap& pmap, const NxTriangleMesh& mesh, NxU32 density, NxUserOutputStream* outputStream = NULL);
bool ReleasePMap(NxPMap& pmap);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "NxPhysicsSDK.h"
#include "NxPMap.h"
#include "PhysXLoader.h"

// if on the Windows, Linux or PS3 platform and 2.5.0 or higher, use the versioned Cooking
// interface via PhysXLoader
#if defined(WIN32) || defined(LINUX) || defined(__CELLOS_LV2__)
#if NX_SDK_VERSION_NUMBER >= 250
#define COOKING_INTERFACE 1
#endif
#endif

#ifdef COOKING_INTERFACE
NxCookingInterface *gCooking=0;
#endif

bool hasCookingLibrary(void) // check to see if the cooking library is available or not!
{
  bool ret = true;

#ifdef COOKING_INTERFACE
  if ( gCooking == 0 )
  {
    gCooking = NxGetCookingLib(NX_PHYSICS_SDK_VERSION);
    if ( gCooking == 0 ) ret = false;
  }
#endif

  return ret;
}


bool CookConvexMesh(const NxConvexMeshDesc& desc, NxStream& stream)
{
#ifdef COOKING_INTERFACE
  hasCookingLibrary();
  if ( !gCooking ) return false;
  return gCooking->NxCookConvexMesh(desc,stream);
#else
  return NxCookConvexMesh(desc,stream);
#endif
}


bool CookClothMesh(const NxClothMeshDesc& desc, NxStream& stream)
{
#ifdef COOKING_INTERFACE
  hasCookingLibrary();
  if ( !gCooking ) return false;
  return gCooking->NxCookClothMesh(desc,stream);
#else
  return NxCookClothMesh(desc,stream);
#endif
}

bool CookTriangleMesh(const NxTriangleMeshDesc& desc, NxStream& stream)
{
#ifdef COOKING_INTERFACE
  hasCookingLibrary();
  if ( !gCooking ) return false;
  return gCooking->NxCookTriangleMesh(desc,stream);
#else
  return NxCookTriangleMesh(desc,stream);
#endif
}

bool CookSoftBodyMesh(const NxSoftBodyMeshDesc& desc, NxStream& stream)
{
#ifdef COOKING_INTERFACE
  hasCookingLibrary();
  if ( !gCooking ) return false;
  return gCooking->NxCookSoftBodyMesh(desc,stream);
#else
  return NxCookSoftBodyMesh(desc,stream);
#endif
}

bool InitCooking(NxUserAllocator* allocator, NxUserOutputStream* outputStream)
{
#ifdef COOKING_INTERFACE
  hasCookingLibrary();
  if ( !gCooking ) return false;
  return gCooking->NxInitCooking(allocator, outputStream);
#else
  return NxInitCooking(allocator, outputStream);
#endif
}

void CloseCooking()
{
#ifdef COOKING_INTERFACE
  if ( !gCooking ) return;
  gCooking->NxCloseCooking();
#else
  return NxCloseCooking();
#endif
}

bool CreatePMap(NxPMap& pmap, const NxTriangleMesh& mesh, NxU32 density, NxUserOutputStream* outputStream)
{
#ifdef COOKING_INTERFACE
  hasCookingLibrary();
  if ( !gCooking ) return false;
  return gCooking->NxCreatePMap(pmap,mesh,density,outputStream);
#else
  return NxCreatePMap(pmap,mesh,density,outputStream);
#endif
}

bool ReleasePMap(NxPMap& pmap)
{
#ifdef COOKING_INTERFACE
  hasCookingLibrary();
  if ( !gCooking ) return false;
  return gCooking->NxReleasePMap(pmap);
#else
  return NxReleasePMap(pmap);
#endif
}

#include "NxStream.h"

class UserStream : public NxStream
{
public:
  UserStream(const char* filename, bool load);
  virtual						~UserStream();

  virtual		NxU8			readByte()								const;
  virtual		NxU16			readWord()								const;
  virtual		NxU32			readDword()								const;
  virtual		NxF32			readFloat()								const;
  virtual		NxF64			readDouble()							const;
  virtual		void			readBuffer(void* buffer, NxU32 size)	const;

  virtual		NxStream&		storeByte(NxU8 b);
  virtual		NxStream&		storeWord(NxU16 w);
  virtual		NxStream&		storeDword(NxU32 d);
  virtual		NxStream&		storeFloat(NxReal f);
  virtual		NxStream&		storeDouble(NxF64 f);
  virtual		NxStream&		storeBuffer(const void* buffer, NxU32 size);

  FILE*			fp;
};

class MemoryWriteBuffer : public NxStream
{
public:
  MemoryWriteBuffer();
  virtual						~MemoryWriteBuffer();
  void			clear();

  virtual		NxU8			readByte()								const	{ NX_ASSERT(0);	return 0;	}
  virtual		NxU16			readWord()								const	{ NX_ASSERT(0);	return 0;	}
  virtual		NxU32			readDword()								const	{ NX_ASSERT(0);	return 0;	}
  virtual		NxF32			readFloat()								const	{ NX_ASSERT(0);	return 0.0f;}
  virtual		NxF64			readDouble()							const	{ NX_ASSERT(0);	return 0.0;	}
  virtual		void			readBuffer(void* buffer, NxU32 size)	const	{ NX_ASSERT(0);				}

  virtual		NxStream&		storeByte(NxU8 b);
  virtual		NxStream&		storeWord(NxU16 w);
  virtual		NxStream&		storeDword(NxU32 d);
  virtual		NxStream&		storeFloat(NxReal f);
  virtual		NxStream&		storeDouble(NxF64 f);
  virtual		NxStream&		storeBuffer(const void* buffer, NxU32 size);

  NxU32			currentSize;
  NxU32			maxSize;
  NxU8*			data;
};

class MemoryReadBuffer : public NxStream
{
public:
  MemoryReadBuffer(const NxU8* data);
  virtual						~MemoryReadBuffer();

  virtual		NxU8			readByte()								const;
  virtual		NxU16			readWord()								const;
  virtual		NxU32			readDword()								const;
  virtual		NxF32			readFloat()								const;
  virtual		NxF64			readDouble()							const;
  virtual		void			readBuffer(void* buffer, NxU32 size)	const;

  virtual		NxStream&		storeByte(NxU8 b)							{ NX_ASSERT(0);	return *this;	}
  virtual		NxStream&		storeWord(NxU16 w)							{ NX_ASSERT(0);	return *this;	}
  virtual		NxStream&		storeDword(NxU32 d)							{ NX_ASSERT(0);	return *this;	}
  virtual		NxStream&		storeFloat(NxReal f)						{ NX_ASSERT(0);	return *this;	}
  virtual		NxStream&		storeDouble(NxF64 f)						{ NX_ASSERT(0);	return *this;	}
  virtual		NxStream&		storeBuffer(const void* buffer, NxU32 size)	{ NX_ASSERT(0);	return *this;	}

  mutable		const NxU8*		buffer;
};

UserStream::UserStream(const char* filename, bool load) : fp(NULL)
{
  fp = fopen(filename, load ? "rb" : "wb");
}

UserStream::~UserStream()
{
  if(fp)	fclose(fp);
}

// Loading API
NxU8 UserStream::readByte() const
{
  NxU8 b;
  size_t r = fread(&b, sizeof(NxU8), 1, fp);
  NX_ASSERT(r);
  return b;
}

NxU16 UserStream::readWord() const
{
  NxU16 w;
  size_t r = fread(&w, sizeof(NxU16), 1, fp);
  NX_ASSERT(r);
  return w;
}

NxU32 UserStream::readDword() const
{
  NxU32 d;
  size_t r = fread(&d, sizeof(NxU32), 1, fp);
  NX_ASSERT(r);
  return d;
}

NxF32 UserStream::readFloat() const
{
  NxReal f;
  size_t r = fread(&f, sizeof(NxReal), 1, fp);
  NX_ASSERT(r);
  return f;
}

NxF64 UserStream::readDouble() const
{
  NxF64 f;
  size_t r = fread(&f, sizeof(NxF64), 1, fp);
  NX_ASSERT(r);
  return f;
}

void UserStream::readBuffer(void* buffer, NxU32 size)	const
{
  size_t w = fread(buffer, size, 1, fp);
  NX_ASSERT(w);
}

// Saving API
NxStream& UserStream::storeByte(NxU8 b)
{
  size_t w = fwrite(&b, sizeof(NxU8), 1, fp);
  NX_ASSERT(w);
  return *this;
}

NxStream& UserStream::storeWord(NxU16 w)
{
  size_t ww = fwrite(&w, sizeof(NxU16), 1, fp);
  NX_ASSERT(ww);
  return *this;
}

NxStream& UserStream::storeDword(NxU32 d)
{
  size_t w = fwrite(&d, sizeof(NxU32), 1, fp);
  NX_ASSERT(w);
  return *this;
}

NxStream& UserStream::storeFloat(NxReal f)
{
  size_t w = fwrite(&f, sizeof(NxReal), 1, fp);
  NX_ASSERT(w);
  return *this;
}

NxStream& UserStream::storeDouble(NxF64 f)
{
  size_t w = fwrite(&f, sizeof(NxF64), 1, fp);
  NX_ASSERT(w);
  return *this;
}

NxStream& UserStream::storeBuffer(const void* buffer, NxU32 size)
{
  size_t w = fwrite(buffer, size, 1, fp);
  NX_ASSERT(w);
  return *this;
}




MemoryWriteBuffer::MemoryWriteBuffer() : currentSize(0), maxSize(0), data(NULL)
{
}

MemoryWriteBuffer::~MemoryWriteBuffer()
{
  NX_DELETE_ARRAY(data);
}

void MemoryWriteBuffer::clear()
{
  currentSize = 0;
}

NxStream& MemoryWriteBuffer::storeByte(NxU8 b)
{
  storeBuffer(&b, sizeof(NxU8));
  return *this;
}
NxStream& MemoryWriteBuffer::storeWord(NxU16 w)
{
  storeBuffer(&w, sizeof(NxU16));
  return *this;
}
NxStream& MemoryWriteBuffer::storeDword(NxU32 d)
{
  storeBuffer(&d, sizeof(NxU32));
  return *this;
}
NxStream& MemoryWriteBuffer::storeFloat(NxReal f)
{
  storeBuffer(&f, sizeof(NxReal));
  return *this;
}
NxStream& MemoryWriteBuffer::storeDouble(NxF64 f)
{
  storeBuffer(&f, sizeof(NxF64));
  return *this;
}
NxStream& MemoryWriteBuffer::storeBuffer(const void* buffer, NxU32 size)
{
  NxU32 expectedSize = currentSize + size;
  if(expectedSize > maxSize)
  {
    maxSize = expectedSize + 4096;

    NxU8* newData = new NxU8[maxSize];
    NX_ASSERT(newData!=NULL);

    if(data)
    {
      memcpy(newData, data, currentSize);
      delete[] data;
    }
    data = newData;
  }
  memcpy(data+currentSize, buffer, size);
  currentSize += size;
  return *this;
}


MemoryReadBuffer::MemoryReadBuffer(const NxU8* data) : buffer(data)
{
}

MemoryReadBuffer::~MemoryReadBuffer()
{
  // We don't own the data => no delete
}

NxU8 MemoryReadBuffer::readByte() const
{
  NxU8 b;
  memcpy(&b, buffer, sizeof(NxU8));
  buffer += sizeof(NxU8);
  return b;
}

NxU16 MemoryReadBuffer::readWord() const
{
  NxU16 w;
  memcpy(&w, buffer, sizeof(NxU16));
  buffer += sizeof(NxU16);
  return w;
}

NxU32 MemoryReadBuffer::readDword() const
{
  NxU32 d;
  memcpy(&d, buffer, sizeof(NxU32));
  buffer += sizeof(NxU32);
  return d;
}

NxF32 MemoryReadBuffer::readFloat() const
{
  NxF32 f;
  memcpy(&f, buffer, sizeof(NxF32));
  buffer += sizeof(NxF32);
  return f;
}

NxF64 MemoryReadBuffer::readDouble() const
{
  NxF64 f;
  memcpy(&f, buffer, sizeof(NxF64));
  buffer += sizeof(NxF64);
  return f;
}

void MemoryReadBuffer::readBuffer(void* dest, NxU32 size) const
{
  memcpy(dest, buffer, size);
  buffer += size;
}

enum EmitterDirection
{
  ED_RIGHT,
  ED_DOWN,
  ED_LEFT
};


class MyPhysX : public PhysX, public TerrainInterface
{
public:
  MyPhysX(void)
  {
    gPhysX = static_cast< PhysX *>(this);
    mSDK = 0;
    mScene = 0;
    mTerrain = 0;
    mFluid = 0;
    mEmitter = 0;
    mActor = 0;
    mTriangleMesh = 0;
    mErodeCount = 0;
  }

  ~MyPhysX(void)
  {
    release();
  }

  void release(void)
  {
    if ( mFluid )
    {
      delete mFluid;
      mFluid = 0;
    }
    if ( mSDK )
    {
      mSDK->release();
    }
    if ( mTerrain )
    {
      releaseTerrain(mTerrain);
      mTerrain = 0;
    }
    mSDK = 0;
    mScene = 0;
    mEmitter = 0;
    mActor = 0;
  }


  void importHeightMap(const char *fname)
  {
    release();
    initSDK();
    mTerrain = createTerrain(fname,this);
  }

  void render(NxF32 dtime)
  {
    if ( mTerrain )
    {
      mTerrain->render();
      if ( mActor )
      {
        gRenderDebug->DebugBound(&mWorldBounds.min.x,&mWorldBounds.max.x,0xFFFFFF);
      }
    }
    if ( mFluid )
    {
      gRenderDebug->DebugDetailedSphere(&mEmitterPos.x, 0.5f, 16, 0xFFFF00, 0.001f, true, true );
      gRenderDebug->DebugDetailedSphere(&mEmitterPos.x, 0.5f, 16, 0xFFFF00, 0.001f, true, false );
      mFluid->draw();
    }
  }

  void simulate(NxF32 dtime)
  {
    if ( mScene )
    {
      mScene->simulate(1.0f/60.0f);
      mScene->flushStream();
      mScene->fetchResults(NX_RIGID_BODY_FINISHED,true);
      performErosion();
      moveRainEmitter();
    }
  }

  void initSDK(void)
  {
    if ( mSDK == 0 )
    {
      NxPhysicsSDKDesc desc;
      NxSDKCreateError errorCode = NXCE_NO_ERROR;
      mSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, NULL, 0, desc, &errorCode);
      if ( mSDK )
      {
        NxSceneDesc sceneDesc;
        sceneDesc.gravity = NxVec3(0,-9.81f,0);
        mScene = mSDK->createScene(sceneDesc);
        if ( mScene )
        {
          NxMaterial *defaultMaterial = mScene->getMaterialFromIndex(0);
          defaultMaterial->setRestitution(0);
          defaultMaterial->setStaticFriction(0.5f);
          defaultMaterial->setDynamicFriction(0.5f);
        }
      }
    }
  }

  void notifyTerrain(NxU32 vcount,const NxF32 *vertices,NxU32 tcount,const NxU32 *indices)
  {
    // create the triangle mesh, and a static actor for it...
    if ( mScene )
    {
      bool first = true;

      if ( mTriangleMesh )
      {
        mScene->releaseActor(*mActor);
        mActor = 0;
        mSDK->releaseTriangleMesh(*mTriangleMesh);
        mTriangleMesh = 0;
        first = false;
      }
      NxTriangleMeshDesc desc;
      desc.numVertices         = vcount;
      desc.numTriangles        = tcount;
      desc.pointStrideBytes    = sizeof(NxVec3);
      desc.triangleStrideBytes = sizeof(NxU32)*3;
      desc.points              = vertices;
      desc.triangles           = indices;
      desc.flags               = 0;
      desc.heightFieldVerticalAxis = NX_Y;
      desc.heightFieldVerticalExtent = -1000.0;
      bool status = InitCooking(0,0);
      if ( status )
      {
        MemoryWriteBuffer buf;
        status = CookTriangleMesh(desc,buf);
        if ( status )
        {
          MemoryReadBuffer rbuf(buf.data);
          mTriangleMesh = mSDK->createTriangleMesh(rbuf);
          if ( mTriangleMesh )
          {
            NxTriangleMeshShapeDesc sdesc;
            sdesc.meshData = mTriangleMesh;
            sdesc.shapeFlags = 0;
            NxActorDesc adesc;
            adesc.shapes.pushBack(&sdesc);
            mActor = mScene->createActor(adesc);

            if ( mActor )
            {
              NxShape * const * shapes = mActor->getShapes();
              NxShape *shape = shapes[0];
              shape->getWorldBounds(mWorldBounds);
            }
            CloseCooking();
            if ( first )
            {
              createRain();
            }
          }
        }
      }
    }
  }

  // ok..now we create the fluid!
  void createRain(void)
  {
    if ( mScene )
    {
      // ...
      //Setup fluid descriptor
      NxFluidDesc fluidDesc;
      fluidDesc.maxParticles                  = MAX_PARTICLES;
      fluidDesc.kernelRadiusMultiplier		= 2.0f;
      fluidDesc.restParticlesPerMeter			= 7.0f;
      fluidDesc.motionLimitMultiplier			= 3.0f;
      fluidDesc.packetSizeMultiplier			= 8;
      fluidDesc.collisionDistanceMultiplier   = 0.1;
      fluidDesc.stiffness						= 50.0f;
      fluidDesc.viscosity						= 40.0f;
      fluidDesc.restDensity					= 1000.0f;
      fluidDesc.damping						= 0.0f;
      fluidDesc.restitutionForStaticShapes	= 0.0f;
      fluidDesc.staticFrictionForStaticShapes = 0.005f;
      fluidDesc.dynamicFrictionForStaticShapes= 0.005f;
      fluidDesc.simulationMethod				= NX_F_SPH;
      fluidDesc.flags &= ~NX_FF_HARDWARE;

      //Create user fluid.
      //- create NxFluid in NxScene
      //- setup the buffers to read from data from the SDK
      //- set NxFluid::userData field to MyFluid instance
      bool trackUserData = true;
      bool provideCollisionNormals = true;
      mFluid = new MyFluid(mScene, fluidDesc, trackUserData, provideCollisionNormals);

      //Create an attached emitter
      NxFluidEmitterDesc emitterDesc;

      emitterDesc.maxParticles = 0;
      emitterDesc.randomAngle = 0.001f;
      emitterDesc.randomPos = NxVec3(0,0,0);
      emitterDesc.dimensionX = EMITTER_DX;
      emitterDesc.dimensionY = EMITTER_DY;
      emitterDesc.type = NX_FE_CONSTANT_FLOW_RATE;
//      emitterDesc.type = NX_FE_CONSTANT_PRESSURE;
      //The rate has no effect with type NX_FE_CONSTANT_PRESSURE
      emitterDesc.rate = FLOW_RATE;
      emitterDesc.fluidVelocityMagnitude = 0.0f;
      emitterDesc.particleLifetime = PARTICLE_LIFE;
      emitterDesc.shape = NX_FE_RECTANGULAR;

      //attach to actor
      emitterDesc.flags |= NX_FEF_ADD_BODY_VELOCITY;
      emitterDesc.repulsionCoefficient = 0.0f;

      emitterDesc.relPose.M.id();
      emitterDesc.relPose.M.rotX(-NxHalfPiF32);
      emitterDesc.relPose.t = NxVec3(64,64,64); //EMITTER_DX,64,EMITTER_DY);

      emitterDesc.frameShape = 0;
      mEmitterDirection = ED_RIGHT;
      mEmitterPos = emitterDesc.relPose.t;

      mEmitter = mFluid->getNxFluid()->createEmitter(emitterDesc);
    }
  }

  void moveRainEmitter(void)
  {
    //
    if ( mEmitter )
    {
      NxF32 edx = EMITTER_DX;
      NxF32 edy = EMITTER_DY;
      switch ( mEmitterDirection )
      {
        case ED_LEFT:
          mEmitterPos.x-=STEP_SIZE;
          if ( mEmitterPos.x <= (mWorldBounds.min.x+edx) )
          {
            mLastDirection = ED_LEFT;
            mEmitterDirection = ED_DOWN;
            mEmitterSteps     = (NxU32)(EMITTER_DY / STEP_SIZE)*2;
          }
          break;
        case ED_RIGHT:
      //    mEmitterPos.x+=STEP_SIZE;
          if ( mEmitterPos.x >= (mWorldBounds.max.x-edx) )
          {
            mLastDirection    = ED_RIGHT;
            mEmitterDirection = ED_DOWN;
            mEmitterSteps     = (NxU32)(EMITTER_DY / STEP_SIZE)*2;
          }
          break;
        case ED_DOWN:
          mEmitterPos.z+=STEP_SIZE;
          if ( mEmitterPos.z >= (mWorldBounds.max.z-edy) )
          {
            mEmitterDirection = ED_RIGHT;
            mEmitterPos.set(edx,64,edy);
          }
          else
          {
            mEmitterSteps--;
            if ( mEmitterSteps == 0 )
            {
              if ( mLastDirection == ED_LEFT )
                mEmitterDirection = ED_RIGHT;
              else
                mEmitterDirection = ED_LEFT;
            }
          }
          break;
      }

      NxF32 x1 = mEmitterPos.x-(EMITTER_DX);
      NxF32 x2 = mEmitterPos.x+(EMITTER_DX);
      NxF32 z1 = mEmitterPos.z-(EMITTER_DY);
      NxF32 z2 = mEmitterPos.z+(EMITTER_DY);

      NxF32 maxV = 0;

      for (NxF32 z=z1; z<=z2; z+=1.0f)
      {
        for (NxF32 x=x1; x<=x2; x+=1.0f)
        {
          NxRay ray;
          ray.orig.set(x,1000,z);
          ray.dir.set(0,-1,0);
          NxRaycastHit hit;
          NxShape *shape = mScene->raycastClosestShape(ray,NX_ALL_SHAPES,hit);
          if ( shape )
          {
            if ( hit.worldImpact.y > maxV )
            {
              maxV = hit.worldImpact.y;
            }
          }
        }
      }

      mEmitterPos.y = maxV+4;

      mEmitter->setGlobalPosition(mEmitterPos);
    }
  }

  void performErosion(void)
  {
    if ( mFluid )
    {

      {
        NxU32 c_count    = mFluid->getCreatedIdsNum();
        const NxU32 *ids = mFluid->getCreatedIds();
        const ParticleSDK *p = mFluid->getParticles();
        MyParticle *mp = mFluid->getMyParticles();
        for (NxU32 i=0; i<c_count; i++)
        {
          NxU32 index = ids[i];
          mp[index].mSoil = 0.0f;
        }
      }

      NxI32 width  = (NxI32) mTerrain->getWidth();
      NxI32 height = (NxI32) mTerrain->getHeight();
      NxF32 *data  = mTerrain->getData();

      const ParticleSDK *p = mFluid->getParticles();
      NxU32 pcount = mFluid->getParticlesNum();
      MyParticle *mp = mFluid->getMyParticles();
      for (NxU32 i=0; i<pcount; i++)
      {
        NxVec3 pp = p->position;
        // ok..first see if it is touching the ground..
        if ( p->collisionNormal.y != 0 )
        {
          NxI32 x = (NxI32) p->position.x;
          NxI32 z = (NxI32) p->position.z;

          if ( x >= 0 && x < width && z >= 0 && z < height )
          {

            #define SOIL_DEPOSIT 0.1f
            #define SOIL_PICKUP (SOIL_DEPOSIT)
            #define VELOCITY_SCALE 0.1f

            NxU32 tindex = z*width+x;
            NxF32 soil = p->collisionNormal.y * SOIL_PICKUP; // * (p->velocity.magnitude()*VELOCITY_SCALE);
            data[tindex]-=soil;
            mp[i].mSoil+=soil;
            if ( mp[i].mSoil> SOIL_DEPOSIT )
            {
              data[tindex]+=SOIL_DEPOSIT;
              mp[i].mSoil-=SOIL_DEPOSIT;
            }
            pp.y = data[tindex];
          }
        }

        gRenderDebug->DebugPoint(&p->position.x,0.1f);

        p++;
      }

      mErodeCount++;
      if ( mErodeCount == 200 )
      {
        mTerrain->rebuildMesh();
        mErodeCount = 0;
      }


/**
      {
        NxU32 d_count    = mFluid->getDeletedIdsNum();
        const NxU32 *ids = mFluid->getDeletedIds();
        MyParticle *mp = mFluid->getMyParticles();
        const ParticleSDK *p = mFluid->getParticles();
        for (NxU32 i=0; i<d_count; i++)
        {
          NxU32 index = ids[i];
          mp[index].mSoil = 0;
          gRenderDebug->DebugPoint(&p[index].position.x,0.01f,0xFF0000,3.0f);
        }
      }
***/
    }
  }

private:
  NxU32           mErodeCount;
  MyFluid        *mFluid;
  Terrain        *mTerrain;
  NxPhysicsSDK   *mSDK;
  NxTriangleMesh *mTriangleMesh;
  NxScene        *mScene;
  NxActor        *mActor;
  NxFluidEmitter *mEmitter;
  NxBounds3       mWorldBounds;
  NxU32            mEmitterSteps;
  NxU32            EmitterCount;
  EmitterDirection mLastDirection;
  EmitterDirection mEmitterDirection;
  NxVec3          mEmitterPos;

//
  NxU32             mParticleCount;
  ParticleSDK      *mParticles;
//

};

static MyPhysX gMyPhysX;

#else

PhysX *gPhysX=0;


#endif
