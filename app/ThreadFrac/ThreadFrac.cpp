//--------------------------------------------------------------------------------------
// File: SampleSoftBodyD3D.cpp
//
// This sample shows a simple example of the Microsoft Direct3D's High-Level
// Shader Language (HLSL) using the Effect interface.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma warning(disable:4995)
#pragma warning(disable:4996)

#define NOMINMAX
#include "common/dxut/dxstdafx.h"
#include "resource.h"
#include "menu.h"
#include "common/dxut/GuiTui.h"
#include "Shlwapi.h"
#include "common/snippets/log.h"
#include "common/snippets/sutil.h"
#include <direct.h>
#include "common/tui/tui.h"
#include "common/binding/binding.h"
#include "pd3d/pd3d.h"
#include "RenderDebug.h"

extern NxI32 gWINDOW_WIDE;
extern NxI32 gWINDOW_TALL;

#include "Tfrac.h"

namespace JOB_SWARM
{
	class JobSwarmContext;
};

JOB_SWARM::JobSwarmContext *gJobSwarmContext=0;


namespace NVSHARE
{
	class RenderDebug;
	RenderDebug *gRenderDebug=0;
}
using namespace NVSHARE;

static NxU32 gScreenWidth=1024;
static NxU32 gScreenHeight=768;

//#define DEBUG_VS   // Uncomment this line to debug vertex shaders
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders

#define USE_HUD 0


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
ID3DXFont*              g_pFont = NULL;         // Font for drawing text
ID3DXSprite*            g_pSprite = NULL;       // Sprite for batching draw text calls
bool                    g_bShowHelp = false;     // If true, it renders the UI control text
NxF32                   gDtime = 0.0f;
CFirstPersonCamera      g_Camera;               // A model viewing camera

CDXUTDialogResourceManager g_DialogResourceManager; // manager for shared resources of dialogs
CD3DSettingsDlg         g_SettingsDlg;          // Device settings dialog
#if USE_HUD
CDXUTDialog             g_HUD;                  // manages the 3D UI
#endif
bool                    g_bEnablePreshader;     // if TRUE, then D3DXSHADER_NO_PRESHADER is used when compiling the shader
D3DXMATRIXA16           g_mCenterWorld;
HWND                    g_Hwnd;

NxI32 gNumFrames=0;
NxI32 gTotalNumFrames=0;
NxF32 gStartTime=0;
NxF32 gFPS=60.0f;

IDirect3DDevice9*    gDevice=0;
HINSTANCE            gInstance=0;
NVSHARE::Pd3d                 *gPd3d=0;
RenderDebug          *gRenderDebug=0;

NxI32 mLastIndex=0;
NxF32 mSpeed=2.0f;
static NxI32 gMovieCapture=0;
static NxI32 gScreenCapture=0;
static NxI32 gMovieFrame=0;

TfracSettings gSettings;
Tfrac *gTfrac=0;

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           3
#define IDC_CHANGEDEVICE        4

#define IDC_ENABLE_PRESHADER    5
#define IDC_NUM_LIGHTS          6
#define IDC_NUM_LIGHTS_STATIC   7
#define IDC_ACTIVE_LIGHT        8
#define IDC_LIGHT_SCALE         9
#define IDC_LIGHT_SCALE_STATIC  10



//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
bool    CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
bool    CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext );
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void    CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, NxF64 fTime, NxF32 fElapsedTime, void* pUserContext );
void    CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, NxF64 fTime, NxF32 fElapsedTime, void* pUserContext );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext );
void    CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void    CALLBACK OnGUIEvent( UINT nEvent, NxI32 nControlID, CDXUTControl* pControl, void* pUserContext );
void    CALLBACK OnLostDevice( void* pUserContext );
void    CALLBACK OnDestroyDevice( void* pUserContext );

void    InitApp();
HRESULT LoadMesh( IDirect3DDevice9* pd3dDevice, WCHAR* strFileName, ID3DXMesh** ppMesh );
void    RenderText( NxF64 fTime );


void myOnDeviceReset(void *device)
{
  if ( gPd3d          ) gPd3d->onDeviceReset(0);
}

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE instance, HINSTANCE, LPSTR, NxI32 )
{

	char vcExePath[1024] = {0};
	char vcExe[1024];

	GetModuleFileNameA(0, vcExePath, sizeof(vcExePath));
	strcpy(vcExe,vcExePath);

	PathRemoveFileSpecA(vcExePath);

	NxI32 len = strlen(vcExePath);
	const char *exeName = &vcExe[len];
	if ( exeName[0] == '\\' || exeName[0] == '/' )
		exeName++;

	if(vcExePath[0])
	{
		chdir(vcExePath);
	}



    // Set the callback functions. These functions allow DXUT to notify
    // the application about device changes, user input, and windows messages.  The
    // callbacks are optional so you need only set callbacks for events you're interested
    // in. However, if you don't handle the device reset/lost callbacks then the sample
    // framework won't be able to reset your device since the application must first
    // release all device resources before resetting.  Likewise, if you don't handle the
    // device created/destroyed callbacks then DXUT won't be able to
    // recreate your device resources.

    DXUTSetCallbackDeviceCreated( OnCreateDevice );
    DXUTSetCallbackDeviceReset( OnResetDevice );
    DXUTSetCallbackDeviceLost( OnLostDevice );
    DXUTSetCallbackDeviceDestroyed( OnDestroyDevice );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackKeyboard( KeyboardProc );
    DXUTSetCallbackFrameRender( OnFrameRender );
    DXUTSetCallbackFrameMove( OnFrameMove );

    // Show the cursor and clip it when in full screen
    DXUTSetCursorSettings( true, true );

		gInstance = instance;

    gPd3d = (NVSHARE::Pd3d *)getBindingInterface("pd3d.dll","pd3d",PD3D_VERSION,0,0);

    if ( gPd3d )
    {

      InitApp();

      // Initialize DXUT and create the desired Win32 window and Direct3D
      // device for the application. Calling each of these functions is optional, but they
      // allow you to set several options which control the behavior of the framework.
      DXUTInit( true, true, true ); // Parse the command line, handle the default hotkeys, and show msgboxes

  		HMENU menu = createMyMenu();

  		wchar_t scratch[512];
			StringCchPrintf(scratch,512,L"ThreadFrac 1.0 written by John W. Ratcliff mailto:jratcliffscarab@gmail.com  September 29, 2008 : Press F1 to toggle UI : codesuppository.blogspot.com" );

      DXUTCreateWindow(scratch, 0, 0, menu );

      DXUTCreateDevice( D3DADAPTER_DEFAULT, true, gWINDOW_WIDE, gWINDOW_TALL+20, IsDeviceAcceptable, ModifyDeviceSettings );

      // Pass control to DXUT for handling the message pump and
      // dispatching render calls. DXUT will call your FrameMove
      // and FrameRender callback when there is idle time between handling window messages.
      DXUTMainLoop();

      myOnDeviceReset(0);


      saveMenuState();

//      tf_release(gTfrac);

  		delete gGuiTui;
  		gGuiTui = 0;

      // Perform any application-level cleanup here. Direct3D device resources are released within the
      // appropriate callback functions and therefore don't require any cleanup code here.
  }

  return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Initialize the app
//--------------------------------------------------------------------------------------
void InitApp()
{
  g_bEnablePreshader = true;
  g_SettingsDlg.Init( &g_DialogResourceManager );
#if USE_HUD
  g_HUD.Init( &g_DialogResourceManager );
  g_HUD.SetCallback( OnGUIEvent ); NxI32 iY = 10;
  g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 );
  g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22 );
  g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22, VK_F2 );
#endif



}


//--------------------------------------------------------------------------------------
// Called during device initialization, this code checks the device for some
// minimum set of capabilities, and rejects those that don't pass by returning E_FAIL.
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat,
                                  D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    // No fallback defined by this app, so reject any device that
    // doesn't support at least ps1.1
    if( pCaps->PixelShaderVersion < D3DPS_VERSION(1,1) )
        return false;

    // Skip backbuffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3DObject();
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                    AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
                    D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
// This callback function is called immediately before a device is created to allow the
// application to modify the device settings. The supplied pDeviceSettings parameter
// contains the settings that the framework has selected for the new device, and the
// application can make any desired changes directly to this structure.  Note however that
// DXUT will not correct invalid device settings so care must be taken
// to return valid device settings, otherwise IDirect3D9::CreateDevice() will fail.
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
    // If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW 
    // then switch to SWVP.
    if( (pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
         pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
    {
        pDeviceSettings->BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

    // Debugging vertex shaders requires either REF or software vertex processing 
    // and debugging pixel shaders requires REF.
#ifdef DEBUG_VS
    if( pDeviceSettings->DeviceType != D3DDEVTYPE_REF )
    {
        pDeviceSettings->BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
        pDeviceSettings->BehaviorFlags &= ~D3DCREATE_PUREDEVICE;                            
        pDeviceSettings->BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
#endif
#ifdef DEBUG_PS
    pDeviceSettings->DeviceType = D3DDEVTYPE_REF;
#endif
    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( pDeviceSettings->DeviceType == D3DDEVTYPE_REF )
            DXUTDisplaySwitchingToREFWarning();
    }

    return true;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been
// created, which will happen during application initialization and windowed/full screen
// toggles. This is the best location to create D3DPOOL_MANAGED resources since these
// resources need to be reloaded whenever the device is destroyed. Resources created
// here should be released in the OnDestroyDevice callback.
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnCreateDevice( pd3dDevice ) );
    V_RETURN( g_SettingsDlg.OnCreateDevice( pd3dDevice ) );
    // Initialize the font
    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                              L"Arial", &g_pFont ) );


    V_RETURN( CDXUTDirectionWidget::StaticOnCreateDevice( pd3dDevice ) );

    // Define DEBUG_VS and/or DEBUG_PS to debug vertex and/or pixel shaders with the
    // shader debugger. Debugging vertex shaders requires either REF or software vertex
    // processing, and debugging pixel shaders requires REF.  The
    // D3DXSHADER_FORCE_*_SOFTWARE_NOOPT flag improves the debug experience in the 
    // shader debugger.  It enables source level debugging, prevents instruction
    // reordering, prevents dead code elimination, and forces the compiler to compile
    // against the next higher available software target, which ensures that the
    // unoptimized shaders do not exceed the shader model limitations.  Setting these
    // flags will cause slower rendering since the shaders will be unoptimized and
    // forced into software.  See the DirectX documentation for more information about
    // using the shader debugger.
    DWORD dwShaderFlags = 0; //D3DXFX_NOT_CLONEABLE;

    #if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DXSHADER_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in
    // the release configuration of this program.
    dwShaderFlags |= D3DXSHADER_DEBUG;
    #endif

    #ifdef DEBUG_VS
        dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
    #endif
    #ifdef DEBUG_PS
        dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
    #endif

    // Preshaders are parts of the shader that the effect system pulls out of the
    // shader and runs on the host CPU. They should be used if you are GPU limited.
    // The D3DXSHADER_NO_PRESHADER flag disables preshaders.
    if( !g_bEnablePreshader )
        dwShaderFlags |= D3DXSHADER_NO_PRESHADER;

    // Setup the camera's view parameters
    D3DXVECTOR3 vecEye(0.0f, 1.5f, -6.0f);
    D3DXVECTOR3 vecAt (0.0f, 1.0f,  0.0f);

    g_Camera.SetViewParams( &vecEye, &vecAt );
    g_Camera.SetScalers(0.01f,2.0f);

		gDevice = pd3dDevice;

		if ( gGuiTui == 0 )
		  gGuiTui = MEMALLOC_NEW(GuiTui)(gDevice,g_Hwnd,gInstance,&g_DialogResourceManager);

    gTui->SetShowGraphics(g_bShowHelp);

    return S_OK;
}

//--------------------------------------------------------------------------------------
// This function loads the mesh and ensures the mesh has normals; it also optimizes the 
// mesh for the graphics card's vertex cache, which improves performance by organizing
// the internal triangle list for less cache misses.
//--------------------------------------------------------------------------------------
HRESULT LoadMesh( IDirect3DDevice9* pd3dDevice, WCHAR* strFileName, ID3DXMesh** ppMesh )
{
    ID3DXMesh* pMesh = NULL;
    WCHAR str[MAX_PATH];
    HRESULT hr;

    // Load the mesh with D3DX and get back a ID3DXMesh*.  For this
    // sample we'll ignore the X file's embedded materials since we know
    // exactly the model we're loading.  See the mesh samples such as
    // "OptimizedMesh" for a more generic mesh loading example.
    V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, strFileName ) );
    V_RETURN( D3DXLoadMeshFromX(str, D3DXMESH_MANAGED, pd3dDevice, NULL, NULL, NULL, NULL, &pMesh) );

    DWORD *rgdwAdjacency = NULL;

    // Make sure there are normals which are required for lighting
    if( !(pMesh->GetFVF() & D3DFVF_NORMAL) )
    {
        ID3DXMesh* pTempMesh;
        V( pMesh->CloneMeshFVF( pMesh->GetOptions(),
                                  pMesh->GetFVF() | D3DFVF_NORMAL,
                                  pd3dDevice, &pTempMesh ) );
        V( D3DXComputeNormals( pTempMesh, NULL ) );

        SAFE_RELEASE( pMesh );
        pMesh = pTempMesh;
    }

    // Optimize the mesh for this graphics card's vertex cache
    // so when rendering the mesh's triangle list the vertices will
    // cache hit more often so it won't have to re-execute the vertex shader
    // on those vertices so it will improve perf.
    rgdwAdjacency = MEMALLOC_NEW(DWORD)[pMesh->GetNumFaces() * 3];
    if( rgdwAdjacency == NULL )
        return E_OUTOFMEMORY;
    V( pMesh->GenerateAdjacency(1e-6f,rgdwAdjacency) );
    V( pMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE, rgdwAdjacency, NULL, NULL, NULL) );
    delete []rgdwAdjacency;

    *ppMesh = pMesh;

    return S_OK;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been
// reset, which will happen after a lost device scenario. This is the best location to
// create D3DPOOL_DEFAULT resources since these resources need to be reloaded whenever
// the device is lost. Resources created here should be released in the OnLostDevice
// callback.
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice,
                                const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    myOnDeviceReset(pd3dDevice);

    V_RETURN( g_DialogResourceManager.OnResetDevice() );
    V_RETURN( g_SettingsDlg.OnResetDevice() );

    if( g_pFont )
        V_RETURN( g_pFont->OnResetDevice() );

    gGuiTui->onResetDevice(pd3dDevice);

    // Create a sprite to help batch calls when drawing many lines of text
    V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pSprite ) );

    // Setup the camera's projection parameters
    NxF32 fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
    g_Camera.SetProjParams( D3DX_PI/3, fAspectRatio, 0.01f, 2000.0f );

#if USE_HUD
    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width-170, pBackBufferSurfaceDesc->Height-170 );
    g_HUD.SetSize( 170, 170 );
#endif


    gScreenWidth  = pBackBufferSurfaceDesc->Width;
    gScreenHeight = pBackBufferSurfaceDesc->Height;

    return S_OK;
}


//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not
// intended to contain actual rendering calls, which should instead be placed in the
// OnFrameRender callback.
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, NxF64 fTime, NxF32 fElapsedTime, void* pUserContext )
{

  g_Camera.SetScalers(0.01f,mSpeed);

	gDtime = fElapsedTime;

	gNumFrames++;
	gTotalNumFrames++;
	gStartTime+=fElapsedTime;

  if ( gTfrac )
  {
    tf_process(gTfrac,fElapsedTime);
  }

	if( gStartTime > 0.2f )
	{
		gFPS = (NxF32)(1.0 / (gStartTime / gNumFrames));
		gNumFrames = 0;
		gStartTime = 0;
	}


  // Update the camera's position based on user input
  bool lshift = false;
  bool rshift = false;

  bool rbutton = false;
  bool lbutton = false;

	if ( GetAsyncKeyState(VK_RBUTTON) )
  	 rbutton = true;

 	if ( GetAsyncKeyState(VK_LBUTTON) )
  	 lbutton = true;

  if ( GetAsyncKeyState(VK_LSHIFT) )
    lshift = true;

  if ( GetAsyncKeyState(VK_RSHIFT) )
    rshift = true;


  POINT p;
	GetCursorPos( &p );
 	ScreenToClient(g_Hwnd, &p );
	bool snarfed = false;
  if ( !snarfed )
  {
    gGuiTui->Update();
    g_Camera.FrameMove( fElapsedTime );
  }

	NxF32 dtime = fElapsedTime;

	if ( gMovieCapture == 2 ) dtime = 1.0f /60.0f;

}

void ScreenGrab(LPDIRECT3DDEVICE9 pDev,const char *_strNamePrefix,NxI32 frameNo,bool jpeg)
{

	HRESULT hr;

  // create the image surface to store the front buffer image
	// note that call to GetFrontBuffer will always convert format to A8R8G8B8
	LPDIRECT3DSURFACE9 surf;
	// Next, this surface is passed to the GetFrontBuffer() method of the device, which will copy the entire screen into our image buffer:
	if ( FAILED(hr=pDev->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&surf) ) )
	{
		surf->Release();
		return;
	}
	char  pszScreenshotName[256];

	if ( !jpeg )
	  sprintf(pszScreenshotName,"%s_%04d.bmp",_strNamePrefix, frameNo);
	else
	  sprintf(pszScreenshotName,"%s_%04d.jpg",_strNamePrefix, frameNo);

	wchar_t temp[512];
	CharToWide(pszScreenshotName,temp,512);

	if ( !jpeg )
	  hr=D3DXSaveSurfaceToFile(temp,D3DXIFF_BMP,surf,NULL,NULL);
	else
	  hr=D3DXSaveSurfaceToFile(temp,D3DXIFF_JPG,surf,NULL,NULL);

	if ( FAILED(hr) )
	{
		// Release the image surface, its our responsibility
		surf->Release();
		return;
	}

	// Release the image surface, its our responsibility
	surf->Release();
}


//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the
// rendering calls for the scene, and it will also be called if the window needs to be
// repainted. After this function has returned, DXUT will call
// IDirect3DDevice9::Present to display the contents of the next buffer in the swap chain
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, NxF64 fTime, NxF32 fElapsedTime, void* pUserContext )
{
  // If the settings dialog is being shown, then
  // render it instead of rendering the app's scene
  if( g_SettingsDlg.IsActive() )
  {
    g_SettingsDlg.OnRender( fElapsedTime );
    return;
  }

  HRESULT hr;
  D3DXMATRIXA16 mWorldViewProjection;
  D3DXMATRIXA16 mWorld;
  D3DXMATRIXA16 mView;
  D3DXMATRIXA16 mProj;

  // Clear the render target and the zbuffer
	NxF32 r = 133.0f / 255.0f;
	NxF32 g = 153.0f / 255.0f;
	NxF32 b = 181.0f / 255.0f;

  V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(r,g,b,0.5f), 1.0f, 0) );

  // Render the scene
  if( SUCCEEDED( pd3dDevice->BeginScene() ) )
  {
    // Get the projection & view matrix from the camera class
    mWorld = g_mCenterWorld * *g_Camera.GetWorldMatrix();
    mProj = *g_Camera.GetProjMatrix();
    mView = *g_Camera.GetViewMatrix();
    mWorldViewProjection = mWorld * mView * mProj;

    gPd3d->setDevice( pd3dDevice );
		gPd3d->preserveRenderState();
    gPd3d->setViewProjectionMatrix( &mView, &mProj );

    if ( gView3d )
    {
//      gRenderDebug->drawGrid(false);
    }

		NVSHARE::Pd3dTexture *texture = 	gPd3d->locateTexture("white.dds");

    if ( gTfrac == 0 )
    {
      gTfrac = tf_create(gScreenWidth,gScreenHeight,gSettings);
		}

  	static bool first = true;

		if ( first )
		{
			first = false;
      MenuSetup();
    }

    tf_render(gTfrac,gView3d,fElapsedTime);

    if ( g_bShowHelp && gScreenCapture == 0 )
    {
			gPd3d->flush();
      gPd3d->renderScreenQuad(texture,0,0,1.0f,gScreenWidth,gScreenHeight,0xA0000000);
      gPd3d->flush();
    }

    //ok..now let's render the debug visualization data.
//    gRenderDebug->Render(fElapsedTime,true,true);   // do the z-buffered pass
//    gRenderDebug->Render(fElapsedTime,true,false);   // do the non-zbuffered pass
		gPd3d->restoreRenderState();

    if ( gScreenCapture == 0 )
    {
      #if USE_HUD
      g_HUD.OnRender( fElapsedTime );
      #endif
  		gGuiTui->Render();
    }

    V( pd3dDevice->EndScene() );

		if ( gScreenCapture )
		{
			gMovieFrame++;
 			ScreenGrab(pd3dDevice,"DFRAC",gMovieFrame,true);
      gScreenCapture = 0;
      gLog->Display("Saved screenshot as 'DFRAC%03d.JPG'.\r\n", gMovieFrame);
    }
  }
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText( NxF64 fTime )
{
}


// adjust the view frustum
void adjustView(const NxF32 *bmin,const NxF32 *bmax)
{
}

//--------------------------------------------------------------------------------------
// Before handling window messages, DXUT passes incoming windows
// messages to the application through this callback function. If the application sets
// *pbNoFurtherProcessing to TRUE, then DXUT will not process this message.
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{
	  g_Hwnd = hWnd;
    // Always allow dialog resource manager calls to handle global messages
    // so GUI state is updated correctly
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }


    // Give the dialogs a chance to handle the message first
#if USE_HUD
    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
#endif
    bool snarfed = gGuiTui->MsgProc(hWnd,uMsg,wParam,lParam);

    bool fsnarf = false;
    if ( !gView3d || GetAsyncKeyState(VK_SHIFT) )
    {
      if ( gTfrac )
        fsnarf = true;
    }

    if ( fsnarf )
    {
      FractalAction action = FA_LAST;

      bool astate = false;

  		NxI32 iMouseX = (NxI16)LOWORD(lParam);
  		NxI32 iMouseY = (NxI16)HIWORD(lParam);
      static NxI32 gLastMouseX = 512;
      static NxI32 gLastMouseY = 378;
      static NxI32 gLastCenterX = 512;
      static NxI32 gLastCenterY = 376;

      static NxI32 mDragX = 0;
      static NxI32 mDragY = 0;
      static bool mDragging=false;

			NxI32 zoomFactor = 1;

      if ( !g_bShowHelp )
      {

    		switch( uMsg )
    		{
  				case WM_MOUSEWHEEL:
			    {
				    NxI32 iDelta        = ((NxI16)HIWORD(wParam));

						if ( iDelta < 0 )
						{
							action = FA_MOUSE_ZOOM_OUT;
  						iDelta/=4;
							zoomFactor = -iDelta;
						}
						else if( iDelta > 0 )
						{
							action = FA_MOUSE_ZOOM_IN;
  						iDelta/=8;
							zoomFactor = iDelta;
						}

            iMouseX = gWINDOW_WIDE/2;
            iMouseY = gWINDOW_TALL/2;

			    }
			    break;

  				case WM_LBUTTONDOWN:
            mDragging = true;
            mDragX = iMouseX;
            mDragY = iMouseY;
    				break;
   				case WM_LBUTTONDBLCLK:
            {
              action = FA_MOUSE_CENTER;
              POINT p;
              GetCursorPos(&p);

              NxI32 dx = p.x - iMouseX;
              NxI32 dy = p.y - iMouseY;

              SetCursorPos((gWINDOW_WIDE/2)+dx,(gWINDOW_TALL/2)+dy);

              gLastCenterX = gWINDOW_WIDE/2;
              gLastCenterY = gWINDOW_TALL/2;
            }
    				break;
  				case WM_LBUTTONUP:
            mDragging = false;
            break;
   				case WM_RBUTTONDOWN:
            iMouseX = gWINDOW_WIDE/2;
            iMouseY = gWINDOW_TALL/2;
            action = FA_MOUSE_CENTER;
            break;
   				case WM_RBUTTONDBLCLK:
   				case WM_MBUTTONDOWN:
   				case WM_MBUTTONDBLCLK:
            break;
   				case WM_RBUTTONUP:
            action = FA_MOUSE_ZOOM_OUT;
            break;
   				case WM_MBUTTONUP:
            break;
  				case WM_MOUSEMOVE:
            gLastMouseX = iMouseX;
            gLastMouseY = iMouseY;
            if ( mDragging && tf_actionReady(gTfrac) )
            {
							NxI32 mx = iMouseX;
							NxI32 my = iMouseY;

              iMouseX = mDragX -iMouseX;
              iMouseY = mDragY -iMouseY;

							iMouseX = (iMouseX/4)*4;
							iMouseY = (iMouseY/4)*4;

							if ( iMouseX != 0 || iMouseY != 0 )
							{
							  mDragX = mx;
							  mDragY = my;
                action = FA_MOUSE_DRAG;
							}

            }
            break;
    		}
      }


      if ( GetAsyncKeyState(VK_LEFT) )
        action = FA_LEFT;
      else if ( GetAsyncKeyState(VK_RIGHT) )
        action = FA_RIGHT;
      else if ( GetAsyncKeyState(VK_UP) )
        action = FA_UP;
      else if ( GetAsyncKeyState(VK_DOWN) )
        action = FA_DOWN;
      else if ( GetAsyncKeyState(VK_ADD) )
        action = FA_ZOOM_IN;
      else if ( GetAsyncKeyState('+') )
        action = FA_ZOOM_IN;
      else if ( GetAsyncKeyState(VK_SUBTRACT) )
        action = FA_ZOOM_OUT;
      else if ( GetAsyncKeyState('-') )
        action = FA_ZOOM_OUT;

      if ( action != FA_LAST )
      {
        snarfed = true;
        tf_action(gTfrac,action,astate,iMouseX,iMouseY,zoomFactor);
      }

    }

    // Give the dialogs a chance to handle the message first

		if ( !snarfed )
		{
      g_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );
    }

		switch ( uMsg )
		{
			case WM_MOUSEWHEEL:
			{
			}
			break;

		  case WM_COMMAND:
   				{
						NxF32 bmin[3];
						NxF32 bmax[3];
   			    bool resize = processMenu(hWnd, LOWORD(wParam), bmin, bmax );
   					if ( resize )
   					{
   						adjustView(bmin,bmax);
   					}
   				}
	    		break;
		}

    return 0;
}


//--------------------------------------------------------------------------------------
// As a convenience, DXUT inspects the incoming windows messages for
// keystroke messages and decodes the message parameters to pass relevant keyboard
// messages to the application.  The framework does not remove the underlying keystroke
// messages, which are still passed to the application's MsgProc callback.
//--------------------------------------------------------------------------------------
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
  if( bKeyDown )
  {
    switch( nChar )
    {
/****
    	case 'M':
    		if ( gMovieCapture == 2 )
    		{
    			gMovieCapture = 0;
    			gLog->Display("Movie Capture stopped\r\n");
    		}
    		else if ( gMovieCapture == 0 )
    		{
    			gMovieCapture = 1;
    			gLog->Display("Press 'M' a second time to begin movie capture.\r\n");
    		}
    		else if ( gMovieCapture == 1 )
    		{
    			gMovieCapture = 2;
    			gLog->Display("Movie Capture started.  Press 'M' to stop.\r\n");
    		}
    		break;
**/
      case 'P':
        gScreenCapture = 1;
        break;
    	case VK_SPACE:
        if ( gTfrac )
        {
          bool previewOnly = tf_togglePreviewOnly(gTfrac);
          CPARSER.Parse("/PreviewOnly %s", previewOnly ? "true" : "false" );
        }
    		break;
    	case '0':
    	case '1':
    	case '2':
    	case '3':
    	case '4':
    	case '5':
    	case '6':
    	case '7':
    	case '8':
    	case '9':
    	  {
    	  	NxI32 index = (nChar-'0')+1;
    	  	if ( index == mLastIndex )
    	  	{
    	  		if ( index == 1 )
    	  			mSpeed*=0.5f;
    	  		else
      	  		mSpeed+=((NxF32)index);
    	  	}
    	  	else
    	  	{
    	  		mSpeed = (NxF32)index;
    	  	}
    	  	mLastIndex = index;
    	  	gLog->Display("Camera speed %0.2f meters per second\r\n", mSpeed );
    	  }
    		break;
      case VK_HOME:
				break;
      case VK_F1:
        g_bShowHelp = !g_bShowHelp; 
        gTui->SetShowGraphics(g_bShowHelp);
        break;
      case 'M':
        if ( gView3d )
        {
          CPARSER.Parse("/View3d false");
        }
        else
        {
          CPARSER.Parse("/View3d true");
        }
        break;
      case 'R':
//        gRenderDebug->Reset();
				{
					TfracSettings settings;
          tf_setFractalCoordinates(gTfrac,settings.mXleft,settings.mXright,settings.mYtop);
          CPARSER.Parse("/ColorPalette %s", settings.mPalette );
				}
        break;
      case 'I':
      	{
          const D3DXVECTOR3 *eye  = g_Camera.GetEyePt();
					const D3DXVECTOR3 *look = g_Camera.GetLookAtPt();
					gLog->Display("EYE: %0.9f,%0.9f,%0.9f  LOOKAT: %0.9f,%0.9f,%0.9f\r\n", eye->x, eye->y, eye->z, look->x, look->y, look->z );
      	}
      	break;
			case 'B':
				break;
    }
  }
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, NxI32 nControlID, CDXUTControl* pControl, void* pUserContext )
{
  switch( nControlID )
  {
    case IDC_TOGGLEFULLSCREEN: DXUTToggleFullScreen(); break;
    case IDC_TOGGLEREF:        DXUTToggleREF(); break;
    case IDC_CHANGEDEVICE:     g_SettingsDlg.SetActive( !g_SettingsDlg.IsActive() ); break;
  }
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has
// entered a lost state and before IDirect3DDevice9::Reset is called. Resources created
// in the OnResetDevice callback should be released here, which generally includes all
// D3DPOOL_DEFAULT resources. See the "Lost Devices" section of the documentation for
// information about lost devices.
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext )
{
  if ( gTfrac )
  {
    tf_getSettings(gTfrac,gSettings);
    tf_release(gTfrac);
    gTfrac = 0;
  }


  myOnDeviceReset(0);
  g_DialogResourceManager.OnLostDevice();
  g_SettingsDlg.OnLostDevice();
  CDXUTDirectionWidget::StaticOnLostDevice();
  if( g_pFont )
      g_pFont->OnLostDevice();
  SAFE_RELEASE(g_pSprite);
  gGuiTui->onLostDevice();


}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has
// been destroyed, which generally happens as a result of application termination or
// windowed/full screen toggles. Resources created in the OnCreateDevice callback
// should be released here, which generally includes all D3DPOOL_MANAGED resources.
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{
    g_DialogResourceManager.OnDestroyDevice();
    g_SettingsDlg.OnDestroyDevice();
    CDXUTDirectionWidget::StaticOnDestroyDevice();
    SAFE_RELEASE(g_pFont);
}


void lookAt(const NxF32 *eye,const NxF32 *look)
{
  g_Camera.SetViewParams((D3DXVECTOR3 *)eye,(D3DXVECTOR3 *)look );
}


void getLookAt(NxF32 *_eye,NxF32 *_look)
{
  const D3DXVECTOR3 *eye  = g_Camera.GetEyePt();
	const D3DXVECTOR3 *look = g_Camera.GetLookAtPt();

  _eye[0] = eye->x;
  _eye[1] = eye->y;
  _eye[2] = eye->z;

  _look[0] = look->x;
  _look[1] = look->y;
  _look[2] = look->z;

}

