#ifndef IMPORT_HEIGHTMAP_H

#define IMPORT_HEIGHTMAP_H

#include "NxSimpleTypes.h"

class ImportHeightMap
{
public:

  virtual bool  importHeightMap(const char *fname) = 0;
  virtual NxU32 getWidth(void) const = 0;
  virtual NxU32 getDepth(void) const = 0;
  virtual NxF32 * getData(void) const = 0;
  virtual NxF32 getPoint(NxU32 x,NxU32 y) = 0;
  virtual bool exportHeightMap(const char *fname) = 0;
  virtual NxF32 getHeightRange(NxF32 &vlow,NxF32 &vhigh) = 0;

};

ImportHeightMap * createImportHeightMap(void);
void              releaseImportHeightMap(ImportHeightMap *hm);

#endif
