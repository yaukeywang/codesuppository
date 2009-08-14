#ifndef COMM_LAYER_H

#define COMM_LAYER_H

#include "MeshImport.h"

namespace MESHIMPORT
{

CommLayer *      CreateCommLayerTelent(const char *address="LOCALHOST",unsigned int port=23);
CommLayer *      CreateCommLayerWindowsMessage(const char *appName,const char *destApp);
void             ReleaseCommLayer(CommLayer *t);

};

#endif
