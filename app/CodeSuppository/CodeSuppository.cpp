#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>

#include "RenderPacket.h"
#include "UserMemAlloc.h"
#include "fmem.h"
#include "shared/MeshSystem/MeshSystemHelper.h"
#include "CodeSuppository.h"
#include "MeshImport.h"
#include "NxFoundation.h"
#include "TestBestFitOBB.h"
#include "TestBestFitCapsule.h"
#include "TestBestFitPlane.h"
#include "TestStanHull.h"
#include "TestInPlaceParser.h"
#include "TestClipper.h"
#include "TestFrustum.h"
#include "TestPlaneTri.h"
#include "TestVertexLookup.h"
#include "TestMapPal.h"
#include "TestMeshVolume.h"
#include "TestDfrac.h"
#include "TestLookAt.h"
#include "TestConvexDecomposition.h"
#include "TestWinMsg.h"
#include "TestWildCard.h"
#include "TestGestalt.h"
#include "TestAsc2Bin.h"
#include "TestFileInterface.h"
#include "TestKeyValueIni.h"
#include "TestKdTree.h"
#include "TestMeshCleanup.h"
#include "TestFastAstar.h"
#include "TestSas.h"
#include "TestCompression.h"
#include "TestSplitMesh.h"
#include "TestArrowHead.h"
#include "TestSendMail.h"
#include "TestSendAIM.h"
#include "TestErosion.h"
#include "TestAutoGeometry.h"
#include "TestTjunctions.h"
#include "TestUntileUV.h"
#include "TestTexturePacker.h"
#include "TestMeshConsolidation.h"
#include "TestIslandGeneration.h"
#include "TestVectorFont.h"
#include "TestMarchingCubes.h"
#include "TestConcavity.h"
#include "RenderDebug.h"
#include "SplitMeshApp.h"
#include "JobSwarm.h"
#include "fastipc.h"

using namespace NVSHARE;

CodeSuppository *gCodeSuppository=0;

typedef std::vector< RenderPacket * > RenderPacketVector;

class MyCodeSuppository : public CodeSuppository, public NVSHARE::Memalloc
{
public:
  MyCodeSuppository(void)
  {
    mTestAutoGeometry = 0;
    mMeshSystemHelper = 0;
    mShowSkeleton = true;
    mShowMesh = true;
    mShowWireframe = false;
    mPlayAnimation = false;
    mShowCollision = true;
    mFlipWinding = false;
    mAnimationSpeed = 4;
    mMergePercentage = 3;
    mConcavityPercentage = 1;
    mFitObb = false;
    mDepth = 1;
    mVolumePercentage = 1;
    mMaxVertices = 32;
    mSkinWidth = 0;
    mRemoveTjunctions = false;
    mInitialIslandGeneration = false;
    mIslandGeneration = false;
    iFastIPC::ErrorCode err;
    mCommLayer = createFastIPC(err,iFastIPC::CT_SERVER,"Global\\CodeSuppository",1024*256,1024*256);
  }

  ~MyCodeSuppository(void)
  {
    resetMeshSystem();
    if ( mCommLayer )
    {
    	releaseFastIPC(mCommLayer);
    }

    for (RenderPacketVector::iterator i=mRenderPacketsCurrent.begin(); i!=mRenderPacketsCurrent.end(); ++i)
    {
    	RenderPacket *rp = (*i);
    	delete rp;
    }
  }

  void resetMeshSystem(void)
  {
    if ( mTestAutoGeometry )
    {
      releaseTestAutoGeometry(mTestAutoGeometry);
      mTestAutoGeometry = 0;
    }
    if ( mMeshSystemHelper )
    {
      releaseMeshSystemHelper(mMeshSystemHelper);
      mMeshSystemHelper = 0;
    }
    appSetMeshSystemHelper(0);
  }

  void processCommand(CodeSuppositoryCommand command,bool state,const NxF32 * data)
  {
    switch ( command )
    {
    	case CSC_TEST_MARCHING_CUBES:
    		testMarchingCubes();
    		break;
      case CSC_TEST_VECTOR_FONT:
        testVectorFont();
        break;
      case CSC_SELECT_COLLISION:
        {
          NxI32 s = (NxI32) data[0];
          if ( mMeshSystemHelper )
          {
            mMeshSystemHelper->setSelectCollision(s);
          }
        }
        break;
      case CSC_MERGE_PERCENTAGE:
        mMergePercentage = data[0];
        break;
      case CSC_CONCAVITY_PERCENTAGE:
        mConcavityPercentage = data[0];
        break;
      case CSC_FIT_OBB:
        mFitObb = state;
        break;
      case CSC_DEPTH:
        mDepth = (NxU32)data[0];
        break;
      case CSC_VOLUME_PERCENTAGE:
        mVolumePercentage = data[0];
        break;
      case CSC_MAX_VERTICES:
        mMaxVertices = (NxU32 )data[0];
        break;
      case CSC_SKIN_WIDTH:
        mSkinWidth = data[0];
        break;
      case CSC_REMOVE_TJUNCTIONS:
        mRemoveTjunctions = state;
        break;
      case CSC_INITIAL_ISLAND_GENERATION:
        mInitialIslandGeneration = state;
        break;
      case CSC_ISLAND_GENERATION:
        mIslandGeneration = state;
        break;
      case CSC_ANIMATION_SPEED:
        if ( data )
        {
          mAnimationSpeed = data[0];
        }
        break;
      case CSC_EXPORT_EZM:
        if ( mMeshSystemHelper )
        {
          mMeshSystemHelper->exportEZM();
        }
        break;
      case CSC_TEST_CONCAVITY:
      	testConcavity(mMeshSystemHelper);
      	break;
      case CSC_EXPORT_OBJ:
        if ( mMeshSystemHelper )
        {
          mMeshSystemHelper->exportObj();
        }
        break;
      case CSC_EXPORT_OGRE:
        if ( mMeshSystemHelper )
        {
          mMeshSystemHelper->exportOgre();
        }
        break;
      case CSC_FLIP_WINDING:
        mFlipWinding = state;
        break;
      case CSC_PLAY_ANIMATION:
        mPlayAnimation = state;
        break;
      case CSC_SHOW_COLLISION:
        mShowCollision = state;
        break;
      case CSC_SHOW_WIREFRAME:
        mShowWireframe = state;
        break;
      case CSC_SHOW_SKELETON:
        mShowSkeleton = state;
        break;
      case CSC_SHOW_MESH:
        mShowMesh = state;
        break;
      case CSC_CLEAR_MESH:
        resetMeshSystem();
        break;
      case CSC_SPLIT_MESH:
        testSplitMesh();
        break;
      case CSC_LOOKAT:
        testLookAt();
        break;
      case CSC_EROSION:
        testErosion();
        break;
      case CSC_SEND_MAIL:
        testSendMail();
        break;
      case CSC_SEND_AIM:
        testSendAIM();
        break;
      case CSC_ARROW_HEAD:
        testArrowHead();
        break;
      case CSC_COMPRESSION:
        testCompression();
        break;
      case CSC_SAS:
        testSas();
        break;
      case CSC_FAST_ASTAR:
        testFastAstar();
        break;
      case CSC_KDTREE:
        testKdTree();
        break;
      case CSC_MESH_CLEANUP:
        testMeshCleanup();
        break;
      case CSC_TINY_XML:
        break;
      case CSC_KEY_VALUE_INI:
        testKeyValueIni();
        break;
      case CSC_FILE_INTERFACE:
        testFileInterface();
        break;
      case CSC_ASC2BIN:
        testAsc2Bin();
        break;
      case CSC_GESTALT:
        testGestalt();
        break;
      case CSC_WILDCARD:
        testWildCard();
        break;
      case CSC_WINMSG:
        testWinMsg();
        break;
      case CSC_CONVEX_DECOMPOSITION:
        testConvexDecomposition(mMeshSystemHelper,
                                mDepth,
                                mMergePercentage,
                                mConcavityPercentage,
                                mVolumePercentage,
                                mMaxVertices,
                                mSkinWidth,
                                mFitObb,
                                mRemoveTjunctions,
                                mInitialIslandGeneration,
                                mIslandGeneration);
        break;
      case CSC_DFRAC:
        testDfrac();
        break;
      case CSC_MESH_VOLUME:
        testMeshVolume();
        break;
      case CSC_MAP_PAL:
        testMapPal();
        break;
      case CSC_VERTEX_LOOKUP:
        testVertexLookup();
        break;
      case CSC_PLANE_TRI:
        testPlaneTri();
        break;
      case CSC_FRUSTUM:
        testFrustum();
        break;
      case CSC_CLIPPER:
        testClipper();
        break;
      case CSC_INPARSER:
        testInPlaceParser();
        break;
      case CSC_STAN_HULL:
        testStanHull(mMeshSystemHelper);
        break;
      case CSC_BEST_FIT_PLANE:
        testBestFitPlane(mMeshSystemHelper);
        break;
      case CSC_BEST_FIT_OBB:
        testBestFitOBB(mMeshSystemHelper);
        break;
      case CSC_TEST_MESH_CONSOLIDATION:
        testMeshConsolidation(mMeshSystemHelper);
        break;
	  case CSC_TEST_REMOVE_TJUNCTIONS:
		  testTjunctions(mMeshSystemHelper);
		  break;
	  case CSC_TEST_UNTILE_UV:
		  {
			  NVSHARE::MeshSystemContainer *msc = testUntileUV(gMeshImport, mMeshSystemHelper);
			  if (msc)
			  {
				  NVSHARE::MeshSystem *msexp = gMeshImport->getMeshSystem(msc); // get the mesh system data.
				  NVSHARE::MeshSerialize data(NVSHARE::MSF_EZMESH);
				  bool ok = gMeshImport->serializeMeshSystem(msexp,data); // serialize it in EZ-MESH
				  if ( ok && data.mBaseData )
				  {
					  FILE *fph = fopen("untiled.ezm", "wb");
					  if ( fph )
					  {
						  fwrite(data.mBaseData, data.mBaseLen, 1, fph );
						  fclose(fph);
					  }
				  }
				  gMeshImport->releaseSerializeMemory(data);
				  gMeshImport->releaseMeshSystemContainer(msc);
			  }
		  }
		  break;
	  case CSC_TEST_TEXTURE_PACKER:
		  {
			  NVSHARE::MeshSystemContainer *msc = testTexturePacker(gMeshImport, mMeshSystemHelper);
			  if (msc)
			  {
				  NVSHARE::MeshSystem *msexp = gMeshImport->getMeshSystem(msc); // get the mesh system data.
				  NVSHARE::MeshSerialize data(NVSHARE::MSF_EZMESH);
				  bool ok = gMeshImport->serializeMeshSystem(msexp,data); // serialize it in EZ-MESH
				  if ( ok && data.mBaseData )
				  {
					  FILE *fph = fopen("packed.ezm", "wb");
					  if ( fph )
					  {
						  fwrite(data.mBaseData, data.mBaseLen, 1, fph );
						  fclose(fph);
					  }
				  }
				  gMeshImport->releaseSerializeMemory(data);
				  gMeshImport->releaseMeshSystemContainer(msc);
			  }
		  }
		  break;
	  case CSC_TEST_ISLAND_GENERATION:
		  testIslandGeneration(mMeshSystemHelper);
		  break;
      case CSC_BEST_FIT_CAPSULE:
        testBestFitCapsule(mMeshSystemHelper);
        break;
      case CSC_AUTO_GEOMETRY:
        if ( mMeshSystemHelper && mTestAutoGeometry == 0 )
        {
          mTestAutoGeometry = createTestAutoGeometry(mMeshSystemHelper);
        }
        break;
    }
  }

  void render(NxF32 dtime)
  {

    if ( mCommLayer )
    {
    	mCommLayer->pumpPendingSends();
    	mCommLayer->pumpPendingReceives();
        const char *msg = mCommLayer->receiveMessage();
        if ( msg )
        {
            if ( strcmp(msg,"NewFrame") == 0 )
            {
            	for (RenderPacketVector::iterator i=mRenderPacketsCurrent.begin(); i!=mRenderPacketsCurrent.end(); ++i)
            	{
            		RenderPacket *rp = (*i);
            		delete rp;
            	}
            	mRenderPacketsCurrent = mRenderPacketsLast;
            	mRenderPacketsLast.clear();
			}
			mCommLayer->receiveAcknowledge();
        }
		const void *data;
		NxU32 dlen;
		iFastIPC::MessageType mt = mCommLayer->receiveData(data,dlen);
		if ( mt != iFastIPC::MT_NONE )
		{
			if ( mt == iFastIPC::MT_APP )
			{
				const NxU32 *type = (const NxU32 *)data;
				switch ( *type )
				{
					case RPT_INDEXED_TRIANGLE_LIST:
						{
							RenderPacketIndexedTriangleList *rpt = new RenderPacketIndexedTriangleList(data,dlen);
							RenderPacket *rp = static_cast< RenderPacket *>(rpt);
							mRenderPacketsLast.push_back(rp);
						}
						break;
				}
			}
            mCommLayer->receiveAcknowledge();
		}
    }

	if ( !mRenderPacketsCurrent.empty() )
	{
		for (RenderPacketVector::iterator i=mRenderPacketsCurrent.begin(); i!=mRenderPacketsCurrent.end(); ++i)
		{
			RenderPacket *rp = (*i);
    		switch ( rp->mType )
    		{
    			case RPT_INDEXED_TRIANGLE_LIST:
    				{
						gRenderDebug->pushRenderState();
						gRenderDebug->addToCurrentState(DebugRenderState::SolidWireShaded);
						gRenderDebug->setCurrentColor(0x00FFFF,0xFFFFFF);
    					RenderPacketIndexedTriangleList *rpt = static_cast< RenderPacketIndexedTriangleList *>(rp);
    					for (NxU32 i=0; i<rpt->mTriCount; i++)
    					{
    						NxU32 i1 = rpt->mIndices[i*3+0];
    						NxU32 i2 = rpt->mIndices[i*3+1];
    						NxU32 i3 = rpt->mIndices[i*3+2];

    						const NxF32 *p1 = &rpt->mPositions[i1*3];
    						const NxF32 *p2 = &rpt->mPositions[i2*3];
    						const NxF32 *p3 = &rpt->mPositions[i3*3];

							const NxF32 *n1 = &rpt->mNormals[i1*3];
							const NxF32 *n2 = &rpt->mNormals[i2*3];
							const NxF32 *n3 = &rpt->mNormals[i3*3];

    						gRenderDebug->DebugTri(p1,p2,p3,n1,n2,n3);

    					}
						gRenderDebug->popRenderState();
    				}
    				break;
   			}
		}
	}

    {
      if ( mMeshSystemHelper )
      {
        if ( mPlayAnimation )
        {
          mMeshSystemHelper->advanceAnimation(dtime,mAnimationSpeed);
        }
        mMeshSystemHelper->debugRender(mShowMesh,mShowSkeleton,mShowWireframe,mPlayAnimation,mShowCollision,mFlipWinding);
      }
    }
  }

  virtual void importMesh(const char *fname)
  {
    resetMeshSystem();
    if ( mMeshSystemHelper == 0 )
    {
      mMeshSystemHelper = createMeshSystemHelper();
    }
    mMeshSystemHelper->importMesh(fname);
    appSetMeshSystemHelper(mMeshSystemHelper);
  }


  virtual void process(NxF32 /*dtime*/)
  {
    if ( mTestAutoGeometry )
    {
      bool tcontinue = mTestAutoGeometry->pump();
      if ( !tcontinue )
      {
        releaseTestAutoGeometry(mTestAutoGeometry);
        mTestAutoGeometry = 0;
      }
    }
    if ( gJobSwarmContext )
    {
      gJobSwarmContext->processSwarmJobs();
    }
  }

private:
  bool               mShowSkeleton;
  bool               mShowMesh;
  bool               mShowWireframe;
  bool               mPlayAnimation;
  bool               mFlipWinding;
  bool               mShowCollision;
  NxF32              mAnimationSpeed;
  MeshSystemHelper  *mMeshSystemHelper;
  NxF32 mMergePercentage;
  NxF32 mConcavityPercentage;
  bool  mFitObb;
  NxU32 mDepth;
  NxF32 mVolumePercentage;
  NxU32 mMaxVertices;
  NxF32 mSkinWidth;
  bool mRemoveTjunctions;
  bool mInitialIslandGeneration;
  bool mIslandGeneration;
  TestAutoGeometry *mTestAutoGeometry;
  iFastIPC				*mCommLayer;
  RenderPacketVector 	mRenderPacketsCurrent;
  RenderPacketVector	mRenderPacketsLast;
};

CodeSuppository * createCodeSuppository(void)
{
  MyCodeSuppository *mcs = MEMALLOC_NEW(MyCodeSuppository);
  return static_cast< CodeSuppository *>(mcs);
}

void              releaseCodeSuppository(CodeSuppository *c)
{
  MyCodeSuppository *mcs = static_cast< MyCodeSuppository *>(c);
  delete mcs;
}

