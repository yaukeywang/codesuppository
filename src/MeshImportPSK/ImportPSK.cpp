#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "FloatMath.h"
#include "NxSimpleTypes.h"
#include "MeshImport.h"
#include "UserMemAlloc.h"

#pragma warning(disable:4100 4189)

namespace MESHIMPORT
{

struct Header
{
    char  mChunkName[20];
    NxI32 mType;
    NxI32 mLen;
    NxI32 mCount;
};

struct Vector
{
    NxF32 x;
    NxF32 y;
    NxF32 z;
};

struct Vertex
{
    NxU16 mIndex;
    NxF32 mTexel[2];
    NxU8  mMaterialIndex;
    NxU8  mUnused;
};

struct Triangle
{
    NxU16 mWedgeIndex[3];
    NxU8  mMaterialIndex;
    NxU8  mAuxMaterialIndex;
    NxU32 mSmoothingGroups;
};

struct Material
{
    char mMaterialName[64];
    NxI32 mTextureIndex;
    NxU32 mPolyFlags;
    NxU32 mAuxMaterial;
    NxI32 mLodBias;
    NxI32 mLodStyle;
};

struct Bone
{
    char  mName[64];
    NxU32 mFlags;
    NxI32 mNumChildren;
    NxI32 mParentIndex;
    NxF32 mOrientation[4];
    NxF32 mPosition[3];
    NxF32 mLength;
    NxF32 mXSize;
    NxF32 mYSize;
    NxF32 mZSize;
};

struct BoneInfluence
{
    NxF32 mWeight;
    NxI32 mVertexIndex;
    NxI32 mBoneIndex;
};

class DeformVector
{
public:
    DeformVector(void)
    {
        mCount = 0;
        mWeight[0] = mWeight[1] = mWeight[2] = mWeight[3] = 0;
        mBone[0] = mBone[1] = mBone[2] = mBone[3] = 0;
    }

    NxU32  mCount;
    NxF32  mWeight[4];
    NxU16  mBone[4];

};


// [Dummy Header]
// [Vector Header]
// Array of Fvectors.
// [Vertex]
// Array of Vertices

class MeshImporterPSK : public MeshImporter
{
public:
  MeshImporterPSK(void)
  {
  }

  ~MeshImporterPSK(void)
  {
  }

  virtual int              getExtensionCount(void) { return 1; }; // most importers support just one file name extension.
  virtual const char *     getExtension(int index) { return ".psk"; }; // report the default file name extension for this mesh type.
  virtual const char *     getDescription(int index)
  {
	  return "PSK Skeletal Meshes";
  }

  void getVertex(MeshVertex &dest,const Vector &p,const Vertex &v,const DeformVector &dv)
  {

    dest.mPos[0] = p.x;
    dest.mPos[1] = p.y;
    dest.mPos[2] = p.z;

    dest.mTexel1[0] = v.mTexel[0];
    dest.mTexel2[1] = v.mTexel[1];

    dest.mWeight[0] = dv.mWeight[0];
    dest.mWeight[1] = dv.mWeight[1];
    dest.mWeight[2] = dv.mWeight[2];
    dest.mWeight[3] = dv.mWeight[3];

    dest.mBone[0]   = dv.mBone[0];
    dest.mBone[1]   = dv.mBone[1];
    dest.mBone[2]   = dv.mBone[2];
    dest.mBone[3]   = dv.mBone[3];

  }

  // warning, this code will not work on big endian processors.
  virtual bool             importMesh(const char *meshName,const void *data,unsigned int dlen,MeshImportInterface *callback,const char *options,MeshImportApplicationResource *appResource)
  {
	  bool ret = false;

      const char *scan = (const char *)data;
      const char *end  = scan+dlen;

      scan+=sizeof(Header); // skip first dummy header

      const Header *h = 0;

      const Header *positionsHeader = h = (Header *)scan;
      scan+=sizeof(Header);
      const Vector *positions = (const Vector *)scan;
      scan+=h->mLen*h->mCount;

      const Header *verticesHeader = h = (const Header *)scan;
      scan+=sizeof(Header);
      const Vertex *vertices = (const Vertex *)scan;
      scan+=h->mLen*h->mCount;

      const Header *trianglesHeader = h = (const Header *)scan;
      scan+=sizeof(Header);
      const Triangle *triangles = (const Triangle *)scan;
      scan+=h->mLen*h->mCount;

      const Header *materialsHeader = h = (const Header *)scan;
      scan+=sizeof(Header);
      const Material *materials= (const Material *)scan;
      scan+=h->mLen*h->mCount;

      const Header *bonesHeader = h = (const Header *)scan;
      scan+=sizeof(Header);
      const Bone *bones= (const Bone *)scan;
      scan+=h->mLen*h->mCount;

      const Header *boneInfluencesHeader =  h = (const Header *)scan;
      scan+=sizeof(Header);
      const BoneInfluence *boneInfluences = (const BoneInfluence *)scan;
      scan+=h->mLen*h->mCount;

      assert( scan == end );

      DeformVector *dvertices = MEMALLOC_NEW_ARRAY(DeformVector,positionsHeader->mCount)[positionsHeader->mCount];

      for (NxI32 i=0; i<boneInfluencesHeader->mCount; i++)
      {
        const BoneInfluence &b = boneInfluences[i];
        DeformVector &d = dvertices[b.mVertexIndex];
        if ( d.mCount < 4 )
        {
            d.mWeight[d.mCount] = b.mWeight;
            d.mBone[d.mCount] = (NxU16)b.mBoneIndex;
            d.mCount++;
        }
      }

      for (NxI32 i=0; i<trianglesHeader->mCount; i++)
      {
        const Triangle &t = triangles[i];
        MeshVertex mv1,mv2,mv3;

        const Vertex &v1 = vertices[t.mWedgeIndex[0]];
        const Vertex &v2 = vertices[t.mWedgeIndex[1]];
        const Vertex &v3 = vertices[t.mWedgeIndex[2]];

        const Vector &p1 = positions[v1.mIndex];
        const Vector &p2 = positions[v2.mIndex];
        const Vector &p3 = positions[v3.mIndex];

        const DeformVector &dv1 = dvertices[v1.mIndex];
        const DeformVector &dv2 = dvertices[v2.mIndex];
        const DeformVector &dv3 = dvertices[v3.mIndex];

        getVertex(mv1,p1,v1,dv1);
        getVertex(mv2,p2,v2,dv2);
        getVertex(mv3,p3,v3,dv3);

        callback->importTriangle("dummy","null", MIVF_ALL, mv3, mv2, mv1 );

      }


      MEMALLOC_DELETE_ARRAY(DeformVector,dvertices);


	  return ret;
  }


};

MeshImporter * createMeshImportPSK(void)
{
    MeshImporterPSK *m = MEMALLOC_NEW(MeshImporterPSK);
    return static_cast< MeshImporter *>(m);
}

void           releaseMeshImportPSK(MeshImporter *iface)
{
    MeshImporterPSK *p = static_cast< MeshImporterPSK *>(iface);
    MEMALLOC_DELETE(MeshImporterPSK,p);
}


};  // end of namespace
