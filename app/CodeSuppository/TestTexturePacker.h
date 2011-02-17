#ifndef TEST_TEXTURE_PACKER_H
#define TEST_TEXTURE_PACKER_H
#include "UserMemAlloc.h"
class MeshSystemHelper;

namespace NVSHARE
{
	class MeshSystemContainer;
	class MeshImport;
}

NVSHARE::MeshSystemContainer* testTexturePacker( NVSHARE::MeshImport* mimport, MeshSystemHelper *mhelper );

#endif
