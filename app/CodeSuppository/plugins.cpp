#include "plugins.h"

#include "common/binding/binding.h"
#include "JobSwarm.h"
#include "RenderDebug.h"
#include "MeshImport.h"
#include "ResourceInterface.h"
#include "pd3d/pd3d.h"
#include "JobSwarm.h"
#include "CodeSuppository.h"

using namespace NVSHARE;

NVSHARE::Pd3d                 *gPd3d=0;

bool loadPlugins(void)
{
  bool ok = false;

  // disable this jobswarm context for now, since it's unused anywhere...
#if 0
  gJobSwarmContext = JOB_SWARM::createJobSwarmContext(4); // create a jobswarm context
#endif

  gResourceInterface = createDefaultResourceInterface();

#if HE_USE_MEMORY_TRACKING
  MEMALLOC::setMaxFixedMemorySize(0);
#endif

  gPd3d        = (NVSHARE::Pd3d *)getBindingInterface("pd3d.dll","pd3d",PD3D_VERSION,NVSHARE::gSystemServices,0);
	gRenderDebug = createRenderDebug();
  if ( gPd3d && gRenderDebug )
    ok = true;
  gMeshImport            = loadMeshImporters(0);

  gPd3d->setResourceInterface(gResourceInterface);

  return ok;
}
