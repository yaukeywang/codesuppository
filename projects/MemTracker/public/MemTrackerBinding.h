#ifndef MEMTRACKER_BINDING_H

#define MEMTRACKER_BINDING_H

#include "MemTracker.h"

namespace MEM_TRACKER
{

MEM_TRACKER::MemTracker * getMemTracker(uint32_t versionNumber,const char *dllName); // Loads the MemTracker DLL

}; // end of MEM_TRACKER namesapce

extern MEM_TRACKER::MemTracker *gMemTracker;

#endif
