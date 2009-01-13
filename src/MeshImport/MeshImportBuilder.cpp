#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "MeshImportBuilder.h"
#include "MeshImport/MeshSystem.h"
#include "common/snippets/UserMemAlloc.h"
#include "common/snippets/stringdict.h"

#pragma warning(disable:4100)

namespace MESHIMPORT
{

typedef USER_STL::vector< MeshVertex > MeshVertexVector;
typedef USER_STL::vector< unsigned int > MeshIndexVector;

class MySubMesh : public SubMesh
{
public:
  MySubMesh(const char *mat,unsigned int vertexFlags)
  {
    mMaterialName = mat;
    mVertexFlags  = (MeshVertexFlag)vertexFlags;
  }

  ~MySubMesh(void)
  {
    MEMALLOC_DELETE_ARRAY(MeshVertex,mVertices);
    MEMALLOC_DELETE_ARRAY(unsigned int,mIndices);
  }

  bool isSame(const char *mat,unsigned int vertexFlags) const
  {
    bool ret = false;
    assert(mat);
    assert(mMaterialName);
    if ( strcmp(mat,mMaterialName) == 0 && mVertexFlags == (MeshVertexFlag)vertexFlags ) ret = true;
    return ret;
  }

  void set(unsigned int vcount,
           const MeshVertex *vertices,
           unsigned int tcount,
           const unsigned int *indices)
  {
    for (unsigned int i=0; i<vcount; i++)
    {
      mAABB.include(vertices[i].mPos);
    }
    mVertexCount = vcount;
    mTriCount    = tcount;
    mVertices    = MEMALLOC_NEW_ARRAY(MeshVertex,vcount)[vcount];
    mIndices     = MEMALLOC_NEW_ARRAY(unsigned int,tcount*3)[tcount*3];
  }

  void gather(void)
  {
    if ( !mMyVertices.empty() )
    {
      set( mMyVertices.size(), &mMyVertices[0], mMyIndices.size()/3, &mMyIndices[0] );
      mMyVertices.clear();
      mMyIndices.clear();
    }
  }

  void add(const MeshVertex verts[3])
  {
    unsigned int index = mMyVertices.size();
    mMyVertices.push_back(verts[0]);
    mMyVertices.push_back(verts[1]);
    mMyVertices.push_back(verts[2]);
    mMyIndices.push_back(index);
    mMyIndices.push_back(index+1);
    mMyIndices.push_back(index+2);
  }

  MeshVertexVector  mMyVertices;
  MeshIndexVector   mMyIndices;
};

typedef USER_STL::vector< MySubMesh * > MySubMeshVector;

class MyMesh : public Mesh
{
public:
  MyMesh(const char *meshName,const char *skeletonName)
  {
    mMeshName = meshName;
    mSkeletonName = skeletonName;
    mCurrent = 0;
  }

  ~MyMesh(void)
  {
    release();
  }

  void release(void)
  {
    MEMALLOC_DELETE_ARRAY(SubMesh,mSubMeshes);
    mSubMeshes = 0;
    mSubMeshCount = 0;
    MySubMeshVector::iterator i;
    for (i=mMySubMeshes.begin(); i!=mMySubMeshes.end(); ++i)
    {
      MySubMesh *s = (*i);
      delete s;
    }
    mMySubMeshes.clear();
  }

  bool isSame(const char *meshName) const
  {
    bool ret = false;
    assert(meshName);
    assert(mMeshName);
    if ( strcmp(mMeshName,meshName) == 0 ) ret = true;
    return ret;
  }

  void getCurrent(const char *materialName,unsigned int vertexFlags)
  {
    if ( materialName == 0 ) materialName = "default_material";
    if ( mCurrent == 0 || !mCurrent->isSame(materialName,vertexFlags) )
    {
      mCurrent =0;
      MySubMeshVector::iterator i;
      for (i=mMySubMeshes.begin(); i!=mMySubMeshes.end(); ++i)
      {
        MySubMesh *s = (*i);
        if ( s->isSame(materialName,vertexFlags) )
        {
          mCurrent = s;
          break;
        }
      }
      if ( mCurrent == 0 )
      {
        mCurrent = MEMALLOC_NEW(MySubMesh)(materialName,vertexFlags);
        mMySubMeshes.push_back(mCurrent);
      }
    }
  }

  virtual void        importTriangle(const char *materialName,
                                     unsigned int vertexFlags,
                                     const MeshVertex verts[3])
  {
    mAABB.include( verts[0].mPos );
    mAABB.include( verts[1].mPos );
    mAABB.include( verts[2].mPos );
    getCurrent(materialName,vertexFlags);
    mCurrent->add(verts);
  }

  virtual void        importIndexedTriangleList(const char *materialName,
                                                unsigned int vertexFlags,
                                                unsigned int vcount,
                                                const MeshVertex *vertices,
                                                unsigned int tcount,
                                                const unsigned int *indices)
  {
    for (unsigned int i=0; i<vcount; i++)
    {
      mAABB.include( vertices[i].mPos );
    }
    mCurrent = MEMALLOC_NEW(MySubMesh)(materialName,vertexFlags);
    mCurrent->set(vcount,vertices,tcount,indices);
    mCurrent = 0;
  }

  void gather(void)
  {
    MEMALLOC_DELETE_ARRAY(SubMesh,mSubMeshes);
    if ( !mMySubMeshes.empty() )
    {
      mSubMeshCount = mMySubMeshes.size();
      mSubMeshes = MEMALLOC_NEW_ARRAY(SubMesh,mSubMeshCount)[mSubMeshCount];
      MySubMeshVector::iterator i;
      SubMesh *dst = mSubMeshes;
      for (i=mMySubMeshes.begin(); i!=mMySubMeshes.end(); ++i)
      {
        MySubMesh *src = (*i);      // copy into a linear array, be we still 'own' the memory.
        dst->mMaterialName = src->mMaterialName;
        dst->mMaterial     = src->mMaterial;
        dst->mAABB         = src->mAABB;
        dst->mVertexFlags  = src->mVertexFlags;
        dst->mVertexCount  = src->mVertexCount;
        dst->mVertices     = src->mVertices;
        dst->mTriCount     = src->mTriCount;
        dst->mIndices      = src->mIndices;
        dst++;
      }
    }
  }

  MySubMesh        *mCurrent;
  MySubMeshVector   mMySubMeshes;
};

typedef USER_STL::map< StringRef, StringRef > StringRefMap;
typedef USER_STL::vector< MeshMaterial >      MeshMaterialVector;
typedef USER_STL::vector< MeshInstance >      MeshInstanceVector;
typedef USER_STL::map< StringRef, MyMesh *>   MyMeshMap;


class MyMeshBuilder : public MeshBuilder, public MeshImportInterface
{
public:
  MyMeshBuilder(const char *meshName,const void *data,unsigned int dlen,MeshImporter *mi,const char *options)
  {
    mCurrentMesh = 0;
    mi->importMesh(meshName,data,dlen,this,options);

    gatherMaterials();
    // todo..
    mMaterialCount = mMyMaterials.size();

    if ( mMaterialCount )
      mMaterials     = &mMyMaterials[0];
    mMeshInstanceCount = mMyMeshInstances.size();

    if ( mMeshInstanceCount )
      mMeshInstances = &mMyMeshInstances[0];

    if ( !mMyMeshes.empty() )
    {
      mMeshCount = mMyMeshes.size();
      mMeshes    = MEMALLOC_NEW_ARRAY(Mesh,mMeshCount)[mMeshCount];
      Mesh *dst = mMeshes;

      MyMeshMap::iterator i;
      for (i=mMyMeshes.begin(); i!=mMyMeshes.end(); ++i)
      {
        MyMesh *src = (*i).second;
        src->gather();
        dst->mMeshName = src->mMeshName;
        dst->mSkeletonName = src->mSkeletonName;
        dst->mSkeleton = src->mSkeleton;
        dst->mSubMeshCount = src->mSubMeshCount;
        dst->mSubMeshes = src->mSubMeshes;
        dst++;
      }
    }

  }

  ~MyMeshBuilder(void)
  {
    MyMeshMap::iterator i;
    for (i=mMyMeshes.begin(); i!=mMyMeshes.end(); ++i)
    {
      MyMesh *src = (*i).second;
      delete src;
    }
  }

  void gatherMaterials(void)
  {
    mMyMaterials.clear();
    unsigned int mcount = mMaterialMap.size();
    mMyMaterials.reserve(mcount);
    StringRefMap::iterator i;
    for (i=mMaterialMap.begin(); i!=mMaterialMap.end(); ++i)
    {
      MeshMaterial m;
      m.mName = (*i).first.Get();
      m.mMetaData = (*i).second.Get();
      mMyMaterials.push_back(m);
    }
  }

  virtual void        importUserData(const char *userKey,const char *userValue)       // carry along raw user data as ASCII strings only..
  {
    assert(0); // not yet implemented
  }

  virtual void        importUserBinaryData(const char *name,unsigned int len,const unsigned char *data)
  {
    assert(0); // not yet implemented
  }

  virtual void        importTetraMesh(const char *tetraName,const char *meshName,unsigned int tcount,const float *tetraData)
  {
    assert(0); // not yet implemented
  }

  virtual void importMaterial(const char *matName,const char *metaData)
  {
    StringRef m = mStrings.Get(matName);
    StringRef d = mStrings.Get(metaData);
    mMaterialMap[m] = d;
  }

  virtual void        importAssetName(const char *assetName,const char *info)         // name of the overall asset.
  {
    mAssetName = mStrings.Get(assetName).Get();
    mAssetInfo = mStrings.Get(info).Get();
  }

  virtual void        importMesh(const char *meshName,const char *skeletonName)       // name of a mesh and the skeleton it refers to.
  {
    StringRef m1 = mStrings.Get(meshName);
    StringRef s1 = mStrings.Get(skeletonName);
    MyMeshMap::iterator found = mMyMeshes.find(m1);
    if ( found == mMyMeshes.end() )
    {
      MyMesh *m = MEMALLOC_NEW(MyMesh)(m1.Get(),m1.Get());
      mMyMeshes[m1] = m;
      mCurrentMesh = m;
    }
    else
    {
      mCurrentMesh = (*found).second;
    }
  }

  void getCurrentMesh(const char *meshName)
  {
    if ( meshName == 0 )
    {
      meshName = "default_mesh";
    }
    if ( mCurrentMesh == 0 || !mCurrentMesh->isSame(meshName) )
    {
      importMesh(meshName,0); // make it the new current mesh
    }
  }

  virtual void        importTriangle(const char *meshName,
                                     const char *materialName,
                                     unsigned int vertexFlags,
                                     const MeshVertex verts[3])
  {
    getCurrentMesh(meshName);
    mAABB.include(verts[0].mPos);
    mAABB.include(verts[1].mPos);
    mAABB.include(verts[2].mPos);
    mCurrentMesh->importTriangle(materialName,vertexFlags,verts);
  }

  virtual void        importIndexedTriangleList(const char *meshName,
                                                const char *materialName,
                                                unsigned int vertexFlags,
                                                unsigned int vcount,
                                                const MeshVertex *vertices,
                                                unsigned int tcount,
                                                const unsigned int *indices)
  {
    getCurrentMesh(meshName);
    for (unsigned int i=0; i<vcount; i++)
    {
      mAABB.include( vertices[i].mPos );
    }
    mCurrentMesh->importIndexedTriangleList(materialName,vertexFlags,vcount,vertices,tcount,indices);
  }

  virtual void        importAnimation(const MeshAnimation &animation)
  {
    assert(0); // not yet implemented
  }

  virtual void        importSkeleton(const MeshSkeleton &skeleton)
  {
//    assert(0); // not yet implemented
  }

  virtual void        importRawTexture(const char *textureName,const unsigned char *pixels,unsigned int wid,unsigned int hit)
  {
    assert(0); // not yet implemented
  }

  virtual void        importMeshInstance(const char *meshName,const float pos[3],const float rotation[4],const float scale[3])
  {
    StringRef ref = mStrings.Get(meshName);
    MeshInstance m;
    m.mMeshName = ref.Get();
    m.mPosition[0] = pos[0];
    m.mPosition[1] = pos[1];
    m.mPosition[2] = pos[2];
    m.mRotation[0] = rotation[0];
    m.mRotation[1] = rotation[1];
    m.mRotation[2] = rotation[2];
    m.mRotation[3] = rotation[3];
    m.mScale[0] = scale[0];
    m.mScale[1] = scale[1];
    m.mScale[2] = scale[2];
    mMyMeshInstances.push_back(m);
  }


private:
  StringDict               mStrings;
  StringRefMap             mMaterialMap;
  MeshMaterialVector       mMyMaterials;
  MeshInstanceVector       mMyMeshInstances;
  MyMesh                  *mCurrentMesh;
  MyMeshMap                mMyMeshes;
};

MeshBuilder * createMeshBuilder(const char *meshName,const void *data,unsigned int dlen,MeshImporter *mi,const char *options)
{
  MyMeshBuilder *b = MEMALLOC_NEW(MyMeshBuilder)(meshName,data,dlen,mi,options);
  return static_cast< MeshBuilder *>(b);
}

void          releaseMeshBuilder(MeshBuilder *m)
{
  MyMeshBuilder *b = static_cast< MyMeshBuilder *>(m);
  MEMALLOC_DELETE(MyMsehImportBuilder,b);
}

}; // end of namesapace
