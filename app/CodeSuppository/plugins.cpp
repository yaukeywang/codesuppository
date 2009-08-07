#include "plugins.h"

#include "common/binding/binding.h"
#include "JobSwarm.h"
#include "RenderDebug/RenderDebug.h"
#include "MeshImport/MeshImport.h"
#include "ResourceInterface.h"
#include "SystemServices.h"
#include "pd3d/pd3d.h"
#include "JobSwarm.h"
#include "CodeSuppository.h"

PD3D::Pd3d                 *gPd3d=0;
RENDER_DEBUG::RenderDebug  *gRenderDebug=0;

bool loadPlugins(void)
{
  bool ok = false;

  gJobSwarmContext = JOB_SWARM::createJobSwarmContext(4); // create a jobswarm context

  gResourceInterface = createDefaultResourceInterface();

#if HE_USE_MEMORY_TRACKING
  MEMALLOC::setMaxFixedMemorySize(0);
#endif

  gPd3d        = (PD3D::Pd3d *)getBindingInterface("pd3d.dll","pd3d",PD3D_VERSION,SYSTEM_SERVICES::gSystemServices,0);
	gRenderDebug = (RENDER_DEBUG::RenderDebug *)getBindingInterface("RenderDebugPd3d.dll","RenderDebugPd3d",RENDER_DEBUG_VERSION,SYSTEM_SERVICES::gSystemServices,0);
  if ( gPd3d && gRenderDebug )
    ok = true;
  gMeshImport            = loadMeshImporters(0,SYSTEM_SERVICES::gSystemServices);

  gPd3d->setResourceInterface(gResourceInterface);

  return ok;
}
