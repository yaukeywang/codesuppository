#ifndef MESH_IMPORT_BUILDER_H

#define MESH_IMPORT_BUILDER_H

#include "MeshImport.h"

class KeyValueIni;

namespace MESHIMPORT
{

class MeshBuilder : public MeshSystem, public MeshImportInterface
{
public:
  virtual void gather(void) = 0;
  virtual void scale(float s) = 0;
  virtual void rotate(float rotX,float rotY,float rotZ) = 0;


};


MeshBuilder * createMeshBuilder(KeyValueIni *ini,
                                const char *meshName,
                                const void *data,
                                unsigned int dlen,
                                MeshImporter *mi,
                                const char *options,
                                MeshImportApplicationResource *appResource);

MeshBuilder * createMeshBuilder(MeshImportApplicationResource *appResource);
void          releaseMeshBuilder(MeshBuilder *m);

};

#endif
