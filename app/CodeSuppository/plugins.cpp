#include "plugins.h"
#include "common/binding/binding.h"

#include "RenderDebug/RenderDebug.h"
#include "MeshImport/MeshImport.h"

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

#include "MeshImportEzm/MeshImportEzm.h"
#include "MeshImportFbx/MeshImportFbx.h"
#include "MeshImportLeveller/MeshImportLeveller.h"
#include "MeshImportObj/MeshImportObj.h"
#include "MeshImportPsk/MeshImportPsk.h"
#include "MeshImportAssimp/MeshImportAssimp.h"
#include "MeshImportOgre/MeshImportOgre.h"

MESHIMPORT::MeshImport                         *gMeshImport=0;

#ifndef OPEN_SOURCE
MESHIMPORTGRANNY::MeshImportGranny             *gMeshImportGranny=0;
MESHIMPORTSPEEDTREE::MeshImportSpeedTree       *gMeshImportSpeedTree=0;
MESHIMPORTHETERRAIN::MeshImportHeTerrain       *gMeshImportHeTerrain=0;
MESHIMPORTHEWATER::MeshImportHeWater           *gMeshImportHeWater=0;
HEGRDRIVER::HeGrDriver                         *gHeGrDriver=0;
HETEXTUREMANAGER::HeTextureManager             *gHeTextureManager=0;
HETEXTURESUBSTANCE::HeTextureSubstance         *gHeTextureSubstance=0;
HEMATERIALSYSTEM::HeMaterialSystem             *gHeMaterialSystem=0;
HERENDERGRAPH::HeRenderGraph                   *gHeRenderGraph=0;
HESGSCENE::HeSGScene                           *gHeSGScene=0;
#endif

MESHIMPORTEZM::MeshImportEzm                   *gMeshImportEzm=0;
MESHIMPORTFBX::MeshImportFbx                   *gMeshImportFbx=0;
MESHIMPORTLEVELLER::MeshImportLeveller         *gMeshImportLeveller=0;
MESHIMPORTOBJ::MeshImportObj                   *gMeshImportObj=0;
MESHIMPORTPSK::MeshImportPsk                   *gMeshImportPsk=0;
MESHIMPORTASSIMP::MeshImportAssimp             *gMeshImportAssimp=0;
MESHIMPORTOGRE::MeshImportOgre                 *gMeshImportOgre=0;


PD3D::Pd3d                 *gPd3d=0;
RENDER_DEBUG::RenderDebug  *gRenderDebug=0;

bool loadPlugins(void)
{
  bool ok = false;

#ifdef OPEN_SOURCE
  gPd3d        = (PD3D::Pd3d *)getBindingInterface("pd3d.dll",PD3D_VERSION);
	gRenderDebug = (RENDER_DEBUG::RenderDebug *)getBindingInterface("RenderDebugPd3d.dll",RENDER_DEBUG_VERSION);
  if ( gPd3d && gRenderDebug )
    ok = true;
#else

	gRenderDebug       = (RENDER_DEBUG::RenderDebug *)       getBindingInterface("renderdebug.dll",RENDER_DEBUG_VERSION);
	gHeGrDriver        = (HEGRDRIVER::HeGrDriver *)       getBindingInterface("HeGrDriverD3D9.dll",HEGRDRIVER_VERSION);
  gHeTextureManager = (HETEXTUREMANAGER::HeTextureManager *)getBindingInterface("HeTextureManager.dll", HETEXTUREMANAGER_VERSION);
  gHeTextureSubstance = (HETEXTURESUBSTANCE::HeTextureSubstance *)getBindingInterface("HeTextureSubstance.dll", HETEXTURESUBSTANCE_VERSION);

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


  gHeMaterialSystem   = (HEMATERIALSYSTEM::HeMaterialSystem *)getBindingInterface("HeMaterialSystem.dll",HEMATERIALSYSTEM_VERSION);
  gHeRenderGraph      = (HERENDERGRAPH::HeRenderGraph *)getBindingInterface("HeRenderGraph.dll",HERENDERGRAPH_VERSION);
  gHeSGScene          = (HESGSCENE::HeSGScene *)getBindingInterface("HeSGScene.dll",HESGSCENE_VERSION);

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

  gMeshImport            = (MESHIMPORT::MeshImport *)                  getBindingInterface("MeshImport.dll",         MESHIMPORT_VERSION);

#ifndef OPEN_SOURCE
  gMeshImportGranny      = (MESHIMPORTGRANNY::MeshImportGranny *)      getBindingInterface("MeshImportGranny.dll",   MESHIMPORTGRANNY_VERSION);
  gMeshImportSpeedTree   = (MESHIMPORTSPEEDTREE::MeshImportSpeedTree *)getBindingInterface("MeshImportSpeedTree.dll",MESHIMPORTSPEEDTREE_VERSION);
  gMeshImportHeTerrain   = (MESHIMPORTHETERRAIN::MeshImportHeTerrain *)getBindingInterface("MeshImportHeTerrain.dll", MESHIMPORTHETERRAIN_VERSION);
  gMeshImportHeWater     = (MESHIMPORTHEWATER::MeshImportHeWater *)    getBindingInterface("MeshImportHeWater.dll",  MESHIMPORTHEWATER_VERSION);
#endif

  gMeshImportEzm         = (MESHIMPORTEZM::MeshImportEzm *)            getBindingInterface("MeshImportEzm.dll",      MESHIMPORTEZM_VERSION);
  gMeshImportFbx         = (MESHIMPORTFBX::MeshImportFbx *)            getBindingInterface("MeshImportFbx.dll",      MESHIMPORTFBX_VERSION);
  gMeshImportLeveller    = (MESHIMPORTLEVELLER::MeshImportLeveller *)       getBindingInterface("MeshImportLeveller.dll", MESHIMPORTLEVELLER_VERSION);
  gMeshImportObj         = (MESHIMPORTOBJ::MeshImportObj *)            getBindingInterface("MeshImportObj.dll",      MESHIMPORTOBJ_VERSION);
  gMeshImportPsk         = (MESHIMPORTPSK::MeshImportPsk *)            getBindingInterface("MeshImportPsk.dll",      MESHIMPORTPSK_VERSION);
  gMeshImportOgre        = (MESHIMPORTOGRE::MeshImportOgre *)            getBindingInterface("MeshImportOgre.dll",      MESHIMPORTOGRE_VERSION);
  gMeshImportAssimp      = (MESHIMPORTASSIMP::MeshImportAssimp *)      getBindingInterface("MeshImportAssimp.dll",   MESHIMPORTASSIMP_VERSION);
  if ( gMeshImport )
  {
#ifndef OPEN_SOURCE
    gMeshImport->addImporter(gMeshImportGranny);
    gMeshImport->addImporter(gMeshImportSpeedTree);
    gMeshImport->addImporter(gMeshImportHeTerrain);
    gMeshImport->addImporter(gMeshImportHeWater);
#endif
    gMeshImport->addImporter(gMeshImportEzm);
    gMeshImport->addImporter(gMeshImportFbx);
    gMeshImport->addImporter(gMeshImportLeveller);
    gMeshImport->addImporter(gMeshImportObj);
    gMeshImport->addImporter(gMeshImportPsk);
	  gMeshImport->addImporter(gMeshImportOgre);
    gMeshImport->addImporter(gMeshImportAssimp);
  }

  return ok;
}
