#ifndef IMPORT_HEIGHTMAP_H

#define IMPORT_HEIGHTMAP_H

#include "He.h"

class ImportHeightMap
{
public:

  virtual bool  importHeightMap(const char *fname) = 0;
  virtual HeU32 getWidth(void) const = 0;
  virtual HeU32 getDepth(void) const = 0;
  virtual HeF32 * getData(void) const = 0;
  virtual HeF32 getPoint(HeU32 x,HeU32 y) = 0;
  virtual bool exportHeightMap(const char *fname) = 0;
  virtual HeF32 getHeightRange(HeF32 &vlow,HeF32 &vhigh) = 0;

};

ImportHeightMap * createImportHeightMap(void);
void              releaseImportHeightMap(ImportHeightMap *hm);

#endif
