#ifndef MESH_SYSTEM_HELPER

#define MESH_SYSTEM_HELPER

namespace MESHIMPORT
{
class MeshSystem;
};

class MeshSystemHelper
{
public:
  virtual void debugRender(void) = 0;
  virtual MESHIMPORT::MeshSystem * getMeshSystem(void) const = 0;
  virtual bool importMesh(const char *fname) = 0;
};

MeshSystemHelper * createMeshSystemHelper(void);
void               releaseMeshSystemHelper(MeshSystemHelper *m);

#endif
