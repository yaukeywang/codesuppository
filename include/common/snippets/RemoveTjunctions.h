#ifndef REMOVE_TJUNCTIONS_H

#define REMOVE_TJUNCTIONS_H

#include "UserMemAlloc.h"

class RemoveTjunctionsDesc
{
public:
  RemoveTjunctionsDesc(void)
  {
    mVcount = 0;
    mVerticesF = 0;
    mVerticesD = 0;
    mTcount = 0;
    mIndices = 0;
    mIds = 0;
    mTcountOut = 0;
    mIndicesOut = 0;
    mIdsOut = 0;
  }

// input
  size_t        mVcount;  // input vertice count.
  const NxF32  *mVerticesF; // input vertices as floats or...
  const NxF64 *mVerticesD; // input vertices as doubles
  size_t        mTcount;    // number of input triangles.
  const size_t *mIndices;   // triangle indices.
  const size_t *mIds;       // optional triangle Id numbers.
// output..
  size_t        mTcountOut;  // number of output triangles.
  const size_t *mIndicesOut; // output triangle indices
  const size_t *mIdsOut;     // output retained id numbers.
};

// Removes t-junctions from an input mesh.  Does not generate any new data points, but may possible produce additional triangles and new indices.
class RemoveTjunctions
{
public:

   virtual size_t removeTjunctions(RemoveTjunctionsDesc &desc) =0; // returns number of triangles output and the descriptor is filled with the appropriate results.


};

RemoveTjunctions * createRemoveTjunctions(void);
void               releaseRemoveTjunctions(RemoveTjunctions *tj);

#endif
