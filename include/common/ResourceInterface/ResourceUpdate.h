#ifndef RESOURCE_UPDATE_H

#define RESOURCE_UPDATE_H

namespace RESOURCE_INTERFACE
{

class ResourceInterfaceCallback;

void registerResourceUpdate(const char *fqn,ResourceInterfaceCallback *iface);
void registerResourceCancel(RESOURCE_INTERFACE::ResourceInterfaceCallback *iface);
void processResourceUpdate(const char *fqn);

}; // end of namespace

#endif
