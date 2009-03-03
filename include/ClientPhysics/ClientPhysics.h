#ifndef CLIENT_PHYSICS_H

#define CLIENT_PHYSICS_H

#include "common/snippets/HeSimpleTypes.h"

class NxPhysicsSDK;
class NxScene;
class NxCookingInterface;


namespace MESHIMPORT
{
  class MeshImport;
};


namespace MESHIMPORT
{
  class MeshSystem;
};

namespace RENDER_DEBUG
{
  class BaseRenderDebug;
};

namespace AUTO_GEOMETRY
{
  class AutoGeometry;
};


namespace CLIENT_PHYSICS
{

enum VertexFormatFlag
{
  VFF_POSITION    = (1<<0),
  VFF_NORMAL      = (1<<1),
  VFF_TANGENT     = (1<<2),
  VFF_BINORMAL    = (1<<3),
  VFF_COLOR       = (1<<4),
  VFF_TEXCOORD0   = (1<<5),
  VFF_TEXCOORD1   = (1<<6),
  VFF_TEXCOORD2   = (1<<7),
  VFF_TEXCOORD3   = (1<<8),
  VFF_BONE_INDEX  = (1<<9),
  VFF_BONE_WEIGHT = (1<<10)
};

class ApexVertexBuffer
{
public:
  ApexVertexBuffer(void)
  {
    mRefCount = 0;
  }
  virtual void *lock(HeU32 firstElement,HeU32 numElements) = 0;
  virtual void  unlock(void) = 0;
  virtual void  release(void) = 0;
  virtual int   incrementReferenceCount(void) { mRefCount++; return mRefCount; };
  virtual int   decrementReferenceCount(void) { mRefCount--; return mRefCount; };
  virtual HeU32 getVertexFormat(void) const = 0;
  virtual HeU32 getVertexCount(void) const = 0;
  virtual HeU32 getVertexStride(void) const = 0;

  int mRefCount;
};

class ApexIndexBuffer
{
public:
  virtual void *lock(HeU32 firstElement,HeU32 numElements) = 0;
  virtual void  unlock(void) = 0;
  virtual void  release(void) = 0;
};

class ApexMaterial
{
public:
  virtual void release(void) = 0;
};

class ApexRenderDesc
{
public:
  ApexRenderDesc(void)
  {
    mUserData = 0;
    mMaterial = 0;
    mBoneBuffer = 0;
    mVertexBuffers = 0;
    mIndexBuffer = 0;
    mFirstVertex = 0;
    mNumVerts = 0;
    mFirstIndex = 0;
    mNumIndices = 0;
    mFirstBone = 0;
    mNumBones = 0;
  }
  ApexMaterial      *mMaterial;
  const HeF32       *mBoneBuffer;      // bone matrices
  HeU32              mVertexBufferCount;
  ApexVertexBuffer **mVertexBuffers;
  ApexIndexBuffer   *mIndexBuffer;
  HeU32              mFirstVertex;
  HeU32              mNumVerts;
  HeU32              mFirstIndex;
  HeU32              mNumIndices;
  HeU32              mFirstBone;
  HeU32              mNumBones;
  void              *mUserData;
};

class ApexCloth;

class ApexRenderInterface
{
public:
  virtual ApexMaterial     * createApexMaterial(const char *matName,void *userData) = 0;

  virtual ApexVertexBuffer * createApexVertexBuffer(HeU32 vertexFormat,
                                                    HeU32 vertexSize,
                                                    HeU32 vcount,
                                                    const unsigned char *vertices) = 0;

  virtual ApexIndexBuffer  * createApexIndexBuffer(HeU32 icount,const HeU32 *indices) = 0;
  virtual ApexIndexBuffer  * createApexIndexBuffer(HeU32 icount,const HeU16 *indices) = 0;

  virtual void               apexRender(const ApexRenderDesc &desc) = 0;

  virtual void               apexProcess(NxScene *scene,float dtime) = 0; // do game processing.
  virtual void               syncClothPhysics(NxScene *scene,ApexCloth *c) = 0;

};

class ApexCloth
{
public:
  virtual void setCompositeTransforms(unsigned int bone_count,const float *matrices,bool continuous) = 0;
  virtual void render(void) = 0;
  virtual void setWind(const float *vec) = 0;
  virtual void * getUserData(void) const = 0;
  virtual void setPriority(float priority) = 0;
  virtual void setParallelMeshMeshSkinning(bool state) = 0;
  virtual void setParallelPhysXMeshSkinning(bool state) = 0;

};


class ApexScene
{
public:
  virtual void         simulate(float dtime,bool async) = 0;
  virtual void         fetchResults(void) = 0;

  virtual ApexCloth *  createApexCloth(void *userData,
                                       unsigned int bone_count,
                                       const float *matrices,
                                       const char *fqn) = 0;

  virtual void         releaseApexCloth(ApexCloth *c) = 0;

  virtual void debugRender(RENDER_DEBUG::BaseRenderDebug *renderDebug) = 0;

};

class Apex
{
public:
  virtual ApexScene *createApexScene(NxScene *scene) = 0;
  virtual void releaseApexScene(ApexScene *scene) = 0;
  virtual void setApexRenderInterface(ApexRenderInterface *iface) = 0;

  virtual void setDebugVisualize(HeF32 distance) = 0;
  virtual void setDebugState(const char *str,HeF32 value) = 0;
  virtual HeU32 getNxParameterCount(void) const = 0;
  virtual const char * getNxParameterString(HeU32 index) const = 0;

};

class CharacterBoneInterface
{
public:
  virtual const float * getBoneWorldTransform(int bone_index) = 0;
};

class CharacterCollision
{
public:
  virtual AUTO_GEOMETRY::AutoGeometry * createAutoGeometry(void) = 0;
  virtual void createCollisionVolumes(void) = 0;
};

class ClientPhysics
{
public:

  virtual void setRenderDebug(RENDER_DEBUG::BaseRenderDebug *rd) = 0;

  virtual CharacterCollision * createCharacterCollision(HeU64 guid,CharacterBoneInterface *iface) = 0;
  virtual void                 releaseCharacterCollision(CharacterCollision *c) = 0;

  virtual bool pump(NxPhysicsSDK *sdk,
                    NxScene      *scene,
                    NxCookingInterface *cooking) = 0;

  virtual bool                 initSDK(NxPhysicsSDK *sdk,NxCookingInterface *iface) = 0;

  virtual NxPhysicsSDK *       getNxPhysicsSDK(void) = 0;
  virtual NxCookingInterface * getNxCookingInterface(void) = 0;
  virtual NxScene *            getNxScene(void) = 0;

  virtual Apex * createApex(void) = 0;
  virtual void   releaseApex(Apex *apex) = 0;
  virtual Apex * getApex(void) = 0;

  virtual void setGameScale(float gameScale) = 0;

};

}; // end of namespace


extern CLIENT_PHYSICS::ClientPhysics *gClientPhysics;

#define CLIENT_PHYSICS_VERSION 1

#endif
