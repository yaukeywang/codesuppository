#ifndef TEST_TEXTURE_PACKER_H
#define TEST_TEXTURE_PACKER_H
#include "UserMemAlloc.h"
class MeshSystemHelper;

namespace physx
{
	class MeshSystemContainer;
	class MeshImport;
}

physx::MeshSystemContainer* testTexturePacker( physx::MeshImport* mimport, MeshSystemHelper *mhelper );

#endif
