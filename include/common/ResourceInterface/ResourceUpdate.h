#ifndef RESOURCE_UPDATE_H

#define RESOURCE_UPDATE_H

namespace FIRE_STORM_RESOURCE
{

class FireStormResourceInterface;

void registerResourceUpdate(const char *fqn,FireStormResourceInterface *iface);
void registerResourceCancel(FIRE_STORM_RESOURCE::FireStormResourceInterface *iface);
void processResourceUpdate(const char *fqn);

}; // end of namespace

#endif
