#ifndef MERGED_PATH_H

#define MERGED_PATH_H

#include "NxSimpleTypes.h"

namespace MERGED_PATH
{

class MergedPath
{
public:

  virtual NxF32 * createMergedPath(const NxF32 *pos,
                                   const NxF32 *pdir,
                                   NxU32 pcount,
                                   const NxF32 * points,
                                   NxU32 pstride,
                                   NxU32 &pout,
                                   NxF32 maxDist)=0;
};


MergedPath * createMergedPath(void);
void         releaseMergedPath(MergedPath *mp);

}; // end of namespace

#endif
