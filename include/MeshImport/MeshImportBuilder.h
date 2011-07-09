#ifndef MESH_IMPORT_BUILDER_H

#define MESH_IMPORT_BUILDER_H

#include "MeshImport.h"
#include "PsShare.h"

namespace physx
{

class MeshBuilder : public MeshSystem, public MeshImportInterface
{
public:
  virtual void gather(void) = 0;
  virtual void scale(PxF32 s) = 0;
  virtual void rotate(PxF32 rotX,PxF32 rotY,PxF32 rotZ) = 0;


};

MeshBuilder * createMeshBuilder(const char *meshName,
                                const void *data,
                                PxU32 dlen,
                                MeshImporter *mi,
                                const char *options,
                                MeshImportApplicationResource *appResource);


MeshBuilder * createMeshBuilder(MeshImportApplicationResource *appResource);
void          releaseMeshBuilder(MeshBuilder *m);

MeshSystem * getMeshSystem(MeshBuilder *mb);

}; // end of namespace

#endif
