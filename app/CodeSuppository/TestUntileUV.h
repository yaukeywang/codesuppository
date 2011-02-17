#ifndef TEST_UNTILE_UV_H
#define TEST_UNTILE_UV_H
#include "UserMemAlloc.h"
class MeshSystemHelper;

namespace NVSHARE
{
	class MeshSystemContainer;
	class MeshImport;
}

NVSHARE::MeshSystemContainer* testUntileUV( NVSHARE::MeshImport* mimport, MeshSystemHelper *mhelper );

#endif
