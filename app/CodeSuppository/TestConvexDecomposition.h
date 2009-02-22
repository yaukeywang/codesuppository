#ifndef TEST_CONVEX_DECOMPOSITION_H
#define TEST_CONVEX_DECOMPOSITION_H

class MeshSystemHelper;

void testConvexDecomposition(MeshSystemHelper *ms,
                             unsigned int depth,
                             float mergePercentage,
                             float concavityPercentage,
                             float volumePercentage,
                             unsigned int maxVertices,
                             float skinWidth,
                             bool fitObb,
                             bool removeTjunctions,
                             bool initialIslandGeneration,
                             bool islandGeneration);

#endif
