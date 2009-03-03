#include "plugins.h"

#include "common/binding/binding.h"
#include "common/MemoryServices/ApplicationSystemServices.h"
#include "common/snippets/JobSwarm.h"
#include "RenderDebug/RenderDebug.h"
#include "MeshImport/MeshImport.h"
#include "ClientPhysics/ClientPhysics.h"
#include "ApexRenderInterface.h"

#ifndef OPEN_SOURCE
#include "MeshImportGranny/MeshImportGranny.h"
#include "MeshImportSpeedTree/MeshImportSpeedTree.h"
#include "MeshImportHeTerrain/MeshImportHeTerrain.h"
#include "MeshImportHeWater/MeshImportHeWater.h"
#include "HeGrDriver/HeGrDriver.h"
#include "HeGrDriver/HeGrShader.h"
#include "HeSGScene/HeSGScene.h"
#include "HeTextureManager/HeTextureManager.h"
#include "HeTextureSubstance/HeTextureSubstance.h"
#include "HeMaterialSystem/HeMaterialSystem.h"
#include "HeRenderGraph/HeRenderGraph.h"
#else
#include "pd3d/pd3d.h"
#endif

#include "common/snippets/JobSwarm.h"
#include "CodeSuppository.h"

SYSTEM_SERVICES::ApplicationSystemServices gAppServices;

PD3D::Pd3d                 *gPd3d=0;
RENDER_DEBUG::RenderDebug  *gRenderDebug=0;
CLIENT_PHYSICS::ClientPhysics *gClientPhysics=0;

bool loadPlugins(void)
{
  bool ok = false;

  gJobSwarmContext = JOB_SWARM::createJobSwarmContext(4); // create a jobswarm context

  gResourceInterface = createDefaultResourceInterface();

#if HE_USE_MEMORY_TRACKING
  MEMALLOC::setMaxFixedMemorySize(0);
#endif

#ifdef OPEN_SOURCE
  gPd3d        = (PD3D::Pd3d *)getBindingInterface("pd3d.dll","pd3d",PD3D_VERSION,SYSTEM_SERVICES::gSystemServices,0);
	gRenderDebug = (RENDER_DEBUG::RenderDebug *)getBindingInterface("RenderDebugPd3d.dll","RenderDebugPd3d",RENDER_DEBUG_VERSION,SYSTEM_SERVICES::gSystemServices,0);
  if ( gPd3d && gRenderDebug )
    ok = true;
#else

  

	gRenderDebug       = (RENDER_DEBUG::RenderDebug *)       getBindingInterface("renderdebug.dll","renderdebug",RENDER_DEBUG_VERSION,SYSTEM_SERVICES::gSystemServices,0);
	gHeGrDriver        = (HEGRDRIVER::HeGrDriver *)       getBindingInterface("HeGrDriverD3D9.dll","HeGrDriverD3D9",HEGRDRIVER_VERSION,SYSTEM_SERVICES::gSystemServices,0);
  gHeTextureManager = (HETEXTUREMANAGER::HeTextureManager *)getBindingInterface("HeTextureManager.dll","HeTextureManager", HETEXTUREMANAGER_VERSION,SYSTEM_SERVICES::gSystemServices,0);
  gHeTextureSubstance = (HETEXTURESUBSTANCE::HeTextureSubstance *)getBindingInterface("HeTextureSubstance.dll", "HeTextureSubstance",HETEXTURESUBSTANCE_VERSION,SYSTEM_SERVICES::gSystemServices,0);

  if ( gHeGrDriver && gRenderDebug )
  {
    ok = true;
  }
  if ( gHeTextureManager )
  {
    gHeTextureManager->setSendTextMessage(gSendTextMessage);
    gHeTextureManager->setHeGrDriver(gHeGrDriver);
    gHeTextureManager->setResourceInterface(gResourceInterface);
  }

  if ( gRenderDebug ) gRenderDebug->setSendTextMessage(gSendTextMessage);


  gHeMaterialSystem   = (HEMATERIALSYSTEM::HeMaterialSystem *)getBindingInterface("HeMaterialSystem.dll",HEMATERIALSYSTEM_VERSION,SYSTEM_SERVICES::gSystemServices);
  gHeRenderGraph      = (HERENDERGRAPH::HeRenderGraph *)getBindingInterface("HeRenderGraph.dll",HERENDERGRAPH_VERSION,SYSTEM_SERVICES::gSystemServices);
  gHeSGScene          = (HESGSCENE::HeSGScene *)getBindingInterface("HeSGScene.dll",HESGSCENE_VERSION,SYSTEM_SERVICES::gSystemServices);

  if ( gHeSGScene )
  {
    gHeSGScene->setSendTextMessage(gSendTextMessage);
    gHeSGScene->setRenderDebug(gRenderDebug);
  }

  if ( gHeMaterialSystem )
  {
    gHeMaterialSystem->setSendTextMessage(gSendTextMessage);
    gHeMaterialSystem->setResourceInterface(gResourceInterface);
    gHeMaterialSystem->setHeGrDriver(gHeGrDriver);
    gHeMaterialSystem->setHeTextureManager(gHeTextureManager);
    gHeMaterialSystem->setHeRenderGraph(gHeRenderGraph);
  }


  if ( gHeRenderGraph )
  {
    gHeRenderGraph->setResourceInterface(gResourceInterface);
    gHeRenderGraph->setHeGrDriver(gHeGrDriver);
    gHeRenderGraph->setHeTextureManager(gHeTextureManager);
    gHeRenderGraph->setHeMaterialSystem(gHeMaterialSystem);
  }

#endif
  gClientPhysics     = (CLIENT_PHYSICS::ClientPhysics *) getBindingInterface("ClientPhysics.dll","ClientPhysics",CLIENT_PHYSICS_VERSION,SYSTEM_SERVICES::gSystemServices,0);
  gMeshImport            = loadMeshImporters(0,SYSTEM_SERVICES::gSystemServices);

  if ( gClientPhysics )
  {
    gClientPhysics->setRenderDebug(gRenderDebug);
    gClientPhysics->initSDK(0,0);
    gApexRenderInterface = createApexRenderInterface();
    gApex = gClientPhysics->createApex();
    gApex->setApexRenderInterface(gApexRenderInterface);
    NxScene *scene = gClientPhysics->getNxScene();
    gApexScene = gApex->createApexScene(scene);
  }
  gPd3d->setResourceInterface(gResourceInterface);

  return ok;
}
