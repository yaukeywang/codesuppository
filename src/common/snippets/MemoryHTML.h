#ifndef MEMORY_HTML_H

#define MEMORY_HTML_H

// reads the results of a memory 'core dump' file and converts it to a nice
// HTML output.
#include "common/snippets/UserMemAlloc.h"

class MemoryHTML;

class MemoryRender
{
public:

  virtual void plotBlock(HeU32 /*baseAddress*/,
                         HeI32 /*screenx*/,HeI32 /*screeny*/,HeI32 /*widx*/,HeI32 /*widy*/,bool /*firstBlock*/,
                         const char * /*section*/,
                         const char * /*tag*/,
                         const char * /*fname*/,
                         const char * /*lineno*/,
                         HeU32 /*size*/) { };

  virtual void plotPixel(HeI32 /*x*/,HeI32 /*y*/,HeU32 /*color*/) {};
  virtual void memorySection(const char * /*section*/,HeU32 /*lowAddress*/,HeU32 /*highAddress*/,HeU32 /*mcount*/,HeU32 /*tmem*/,HeU32 /*mean*/,HeF32 /*stdev*/) {};

  virtual void memoryMessage(const char * /*fmt*/,...) { };

};

MemoryHTML * createMemoryHTML(const char *fname);
void         releaseMemoryHTML(MemoryHTML *m);
bool         generateHTML(MemoryHTML *m,const char *htmlName);

bool         memoryRender(MemoryHTML *m,MemoryRender *callback,const char *section,HeU32 mlow,HeU32 mhigh);

void         memoryPower2(MemoryHTML *m,const char *section,MemoryRender *callback);

HeU32 enumerateMemorySections(MemoryHTML *m,MemoryRender *callback);

void setBlockSize(HeU32 blockSize);


#endif
