#ifndef MESHIMPORT_H
#define MESHIMPORT_H

#include "MeshSystem.h"
#include "MeshImport/MeshImport.h"

#pragma warning(push)
#pragma warning(disable:4996)


// MeshImporters to write:  Wavefront OBJ
//                          EZ-Mesh
//                          Ogre3d
//                          Unreal PSK
//                          Granny
//                          SpeedTree
//                          HeroEngine terrain
//                          HeroEngine water
//                          Leveller heightfields using RTIN
//

namespace MESHIMPORT
{

class MeshImportInterface;

class MeshImporter
{
public:
  virtual int              getExtensionCount(void) { return 1; }; // most importers support just one file name extension.
  virtual const char *     getExtension(int index=0) = 0; // report the default file name extension for this mesh type.
  virtual const char *     getDescription(int index=0) = 0; // report the ascii description of the import type.

  virtual bool             importMesh(const char *meshName,const void *data,unsigned int dlen,MeshImportInterface *callback,const char *options) = 0;
};


class MeshImport
{
public:
  virtual void             addImporter(MeshImporter *importer) = 0; // add an additional importer

  virtual bool             importMesh(const char *meshName,const void *data,unsigned int dlen,MeshImportInterface *callback,const char *options) = 0;
  virtual MeshSystem *     createMeshSystem(const char *meshName,const void *data,unsigned int dlen,const char *options) = 0; // imports and converts to a single MeshSystem data structure

  virtual void             releaseMeshSystem(MeshSystem *mesh) = 0;

  virtual int              getImporterCount(void) = 0;
  virtual MeshImporter    *getImporter(int index) = 0;

  virtual MeshImporter *   locateMeshImporter(const char *fname) = 0; // based on this file name, find a matching mesh importer.
};

}; // End of namespace for MESHIMPORT

#define MESHIMPORT_VERSION 1  // version 0.01  increase this version number whenever an interface change occurs.


extern MESHIMPORT::MeshImport *gMeshImport; // This is an optional global variable that can be used by the application.  If the application uses it, it should define it somewhere in its codespace.

#pragma warning(pop)

#endif
