#ifndef REMOVE_TJUNCTIONS_H

#define REMOVE_TJUNCTIONS_H

#include <NxSimpleTypes.h>

class RemoveTjunctionsDesc
{
public:
  RemoveTjunctionsDesc(void)
  {
    mVcount = 0;
    mVertices = 0;
    mTcount = 0;
    mIndices = 0;
    mIds = 0;
    mTcountOut = 0;
    mIndicesOut = 0;
    mIdsOut = 0;
	mEpsilon = 0.00000001f;
  }

// input
  NxF32        mEpsilon;
  NxF32        mDistanceEpsilon;
  NxU32        mVcount;  // input vertice count.
  const NxF32 *mVertices; // input vertices as NxF32s or...
  NxU32        mTcount;    // number of input triangles.
  const NxU32 *mIndices;   // triangle indices.
  const NxU32 *mIds;       // optional triangle Id numbers.
// output..
  NxU32        mTcountOut;  // number of output triangles.
  const NxU32 *mIndicesOut; // output triangle indices
  const NxU32 *mIdsOut;     // output retained id numbers.
};

// Removes t-junctions from an input mesh.  Does not generate any new data points, but may possible produce additional triangles and new indices.
class RemoveTjunctions
{
public:

   virtual NxU32 removeTjunctions(RemoveTjunctionsDesc &desc) =0; // returns number of triangles output and the descriptor is filled with the appropriate results.


};

RemoveTjunctions * createRemoveTjunctions(void);
void               releaseRemoveTjunctions(RemoveTjunctions *tj);

#endif
