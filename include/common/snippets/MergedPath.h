#ifndef MERGED_PATH_H

#define MERGED_PATH_H

#include "HeSimpleTypes.h"

namespace MERGED_PATH
{

class MergedPath
{
public:

  virtual HeF32 * createMergedPath(const HeF32 *pos,
                                   const HeF32 *pdir,
                                   HeU32 pcount,
                                   const HeF32 * points,
                                   HeU32 pstride,
                                   HeU32 &pout,
                                   HeF32 maxDist)=0;
};


MergedPath * createMergedPath(void);
void         releaseMergedPath(MergedPath *mp);

}; // end of namespace

#endif
