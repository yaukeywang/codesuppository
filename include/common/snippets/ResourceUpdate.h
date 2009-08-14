#ifndef RESOURCE_UPDATE_H

#define RESOURCE_UPDATE_H

#include "UserMemAlloc.h"

namespace RESOURCE_INTERFACE
{

class ResourceInterfaceCallback;

void registerResourceUpdate(const char *fqn,ResourceInterfaceCallback *iface);
void registerResourceCancel(RESOURCE_INTERFACE::ResourceInterfaceCallback *iface);
void processResourceUpdate(const char *fqn);

}; // end of namespace

#endif
