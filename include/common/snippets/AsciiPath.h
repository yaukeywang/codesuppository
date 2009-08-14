#ifndef ASCII_PATH_H

#define ASCII_PATH_H

#include "UserMemAlloc.h"

// Converts a 2d path represented as a CSV data asset into something we can easily index.  Used to parse RegionNode data.  Shared by the path system and hbphysics.
class AsciiPath
{
public:

  virtual NxU32 setData(const char *data) = 0;
  virtual bool  getPoint(NxU32 index,NxF32 &x,NxF32 &y) const = 0;
  virtual NxU32 getPointCount(void) const = 0;

};

AsciiPath * createAsciiPath(void);
void        releaseAsciiPath(AsciiPath *ap);

#endif
