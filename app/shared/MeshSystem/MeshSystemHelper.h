#ifndef MESH_SYSTEM_HELPER

#define MESH_SYSTEM_HELPER

namespace MESHIMPORT
{
class MeshSystem;
class MeshSystemContainer;
};

class MeshSystemRaw
{
public:
  MeshSystemRaw(void)
  {
    mVcount = 0;
    mTcount = 0;
    mVertices = 0;
    mIndices = 0;
  }
  unsigned int mVcount;
  unsigned int mTcount;
  float *mVertices;
  unsigned int *mIndices;
};

class MeshSystemHelper
{
public:
  virtual void debugRender(bool showMesh,
                           bool showSkeleton,
                           bool showWireframe,
                           bool showAnimation,
                           bool showCollision,
                           bool flipWinding) = 0;

  virtual MESHIMPORT::MeshSystem * getMeshSystem(void) const = 0;
  virtual MESHIMPORT::MeshSystemContainer * getMeshSystemContainer(void) = 0;
  virtual bool importMesh(const char *fname) = 0;
  virtual void advanceAnimation(float dtime,float rate) = 0;
  virtual bool exportEZM(void) = 0;
  virtual bool exportOgre(void) = 0;
  virtual bool exportObj(void) = 0;
  virtual const float * getCompositeTransforms(unsigned int &bone_count) = 0;

  virtual MeshSystemRaw * getMeshSystemRaw(void) = 0;
  virtual void            releaseMeshSystemRaw(MeshSystemRaw *mr) = 0;
  virtual void setSelectCollision(int sc) = 0;
};

MeshSystemHelper * createMeshSystemHelper(void);
void               releaseMeshSystemHelper(MeshSystemHelper *m);

#endif
