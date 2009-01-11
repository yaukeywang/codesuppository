#ifndef ASCII_PATH_H

#define ASCII_PATH_H

#include "HeSimpleTypes.h"

// Converts a 2d path represented as a CSV data asset into something we can easily index.  Used to parse RegionNode data.  Shared by the path system and hbphysics.
class AsciiPath
{
public:

  virtual HeU32 setData(const char *data) = 0;
  virtual bool  getPoint(HeU32 index,HeF32 &x,HeF32 &y) const = 0;
  virtual HeU32 getPointCount(void) const = 0;

};

AsciiPath * createAsciiPath(void);
void        releaseAsciiPath(AsciiPath *ap);

#endif
