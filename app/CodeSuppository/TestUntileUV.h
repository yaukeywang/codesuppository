#ifndef TEST_UNTILE_UV_H
#define TEST_UNTILE_UV_H
#include "UserMemAlloc.h"

class MeshSystemHelper;
namespace NVSHARE
{
class Pd3Material;
};

namespace physx
{
	class MeshSystemContainer;
	class MeshImport;
}



physx::MeshSystemContainer* testUntileUV( physx::MeshImport* mimport, MeshSystemHelper *mhelper );

#endif
