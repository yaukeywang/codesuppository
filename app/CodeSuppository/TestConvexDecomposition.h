#ifndef TEST_CONVEX_DECOMPOSITION_H
#define TEST_CONVEX_DECOMPOSITION_H

#include "UserMemAlloc.h"

class MeshSystemHelper;

void testConvexDecomposition(MeshSystemHelper *ms,
                             NxU32 depth,
                             NxF32 mergePercentage,
                             NxF32 concavityPercentage,
                             NxF32 volumePercentage,
                             NxU32 maxVertices,
                             NxF32 skinWidth,
                             bool fitObb,
                             bool removeTjunctions,
                             bool initialIslandGeneration,
                             bool islandGeneration);

#endif
