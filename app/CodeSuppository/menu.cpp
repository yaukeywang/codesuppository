#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#pragma warning(disable:4995)

#define NOMINMAX
#include "common/dxut/dxstdafx.h"
#include "resource.h"
#include "menu.h"
#include "MeshImport.h"
#include "cparser.h"
#include "filesystem.h"
#include "log.h"
#include "common/tui/tui.h"
#include "sutil.h"
#include "wildcard.h"
#include "ffind.h"
#include "erode.h"
#include "SplitMeshMain.h"
#include "SplitMeshApp.h"
#include "ImportHeightMap.h"
#include "NxVec3.h"
#include "RenderDebug.h"
#include "Pd3d/Pd3d.h"
#include "CodeSuppository.h"
#include "SendTextMessage.h"
#include <direct.h>

enum MenuOptions
{
  MO_RUN_SCRIPT = 10000,
  MO_IMPORT_MESH,
  MO_EXPORT_OBJ,
  MO_EXPORT_EZM,
  MO_EXPORT_OGRE,
  MO_VISUALIZE_NONE,
	MO_EXIT,
};

enum MyCommand
{
	MC_CREATE_DYNAMICS = 11000,
	MC_PSSCRIPT,
	MC_PSLOOKAT,
  MC_PLANE,
  MC_TRIANGULATE_TYPE,
  MC_MEMORY_REPORT,
};

class MenuItem
{
public:
    void set(NxU32 p,const char *name)
    {
      mParameter = p;
      mName = name;
      mState = false;
    }
  NxU32 mParameter;
  const char *mName;
  bool  mState;
};

typedef USER_STL::vector< std::string > StringVector;

class MyMenu : public FileSystem, public CommandParserInterface
{
public:
  MyMenu(void)
  {
  	mMainMenu = CreateMenu();
    mCommandOk = true;
    mStartup = true;

    gCodeSuppository = createCodeSuppository();

  	HMENU m = CreatePopupMenu();
		mFileMenu = m;

  	AppendMenu( mMainMenu, MF_POPUP, (UINT_PTR)m, L"&File" );
      AppendMenu( m, MF_STRING, MO_IMPORT_MESH, L"Import Mesh Data");
      AppendMenu( m, MF_STRING, MO_EXPORT_EZM,  L"Export as EZ-Mesh");
      AppendMenu( m, MF_STRING, MO_EXPORT_OBJ,  L"Export as Wavefront OBJ");
      AppendMenu( m, MF_STRING, MO_EXPORT_OGRE, L"Export as Ogre3D XML");
  	  AppendMenu( m, MF_STRING, MO_EXIT, L"E&xit");

    mVisualizationMenu = 0;


	  // ok, now initialize the scripted menu interface.
	  gFileSystem       = this;
	  gLog              = MEMALLOC_NEW(Log)("CodeSuppository.txt");
	  gTheCommandParser = MEMALLOC_NEW(TheCommandParser);
		gTui              = MEMALLOC_NEW(TextUserInterface)("CodeSuppository.tui");


   createButton("MEMORY_REPORT", MC_MEMORY_REPORT, "MC_MEMORY_REPORT");
   createCheckbox("Show Skeleton", CSC_SHOW_SKELETON, "ShowSkeleton", true );
   createCheckbox("Show Mesh", CSC_SHOW_MESH, "ShowMesh", true );
   createCheckbox("Show Collision", CSC_SHOW_COLLISION, "ShowCollision", true );
   createCheckbox("Show Wireframe", CSC_SHOW_WIREFRAME, "ShowWireframe", false );
   createCheckbox("Flip Winding Order", CSC_FLIP_WINDING,"FlipWinding", false );
   createCheckbox("Play Animation", CSC_PLAY_ANIMATION, "PlayAnimation", false );
   createButton("Create Apex Cloth", CSC_APEX_CLOTH, "CreateApexCloth" );

   createSlider("Animation Speed",CSC_ANIMATION_SPEED,"AnimationSpeed",0,100,4,false);
   createSlider("Select Collision Hull",CSC_SELECT_COLLISION,"SelectCollision",0,100,0,true);

   createButton("Clear Mesh", CSC_CLEAR_MESH, "ClearMesh");
   createButton("AutoGenerate Collision Mesh", CSC_AUTO_GEOMETRY,"AutoGeometry");

   createButton("BEST_FIT_OBB",  CSC_BEST_FIT_OBB,  "CSC_BEST_FIT_OBB");
   createButton("BEST_FIT_CAPSULE",  CSC_BEST_FIT_CAPSULE,  "CSC_BEST_FIT_CAPSULE");
   createButton("BEST_FIT_PLANE",  CSC_BEST_FIT_PLANE,  "CSC_BEST_FIT_PLANE");
   createButton("REMOVE_TJUNCTIONS",  CSC_TEST_REMOVE_TJUNCTIONS,  "CSC_TEST_REMOVE_TJUNCTIONS");
   createButton("ISLAND_GENERATION",  CSC_TEST_ISLAND_GENERATION,  "CSC_TEST_ISLAND_GENERATION");
   createButton("MESH_CONSOLIDATION",  CSC_TEST_MESH_CONSOLIDATION,  "CSC_TEST_MESH_CONSOLIDATION");
   createButton("VECTOR FONT",  CSC_TEST_VECTOR_FONT,  "CSC_TEST_VECTOR_FONT");

   createButton("STAN_HULL",  CSC_STAN_HULL,  "CSC_STAN_HULL");
   createButton("CONVEX_DECOMPOSITION",  CSC_CONVEX_DECOMPOSITION,  "CSC_CONVEX_DECOMPOSITION");

   createButton("*INPARSER",  CSC_INPARSER,  "CSC_INPARSER");
   createButton("*CLIPPER",  CSC_CLIPPER,  "CSC_CLIPPER");
   createButton("*FRUSTUM",  CSC_FRUSTUM,  "CSC_FRUSTUM");
   createButton("*PLANE_TRI",  CSC_PLANE_TRI,  "CSC_PLANE_TRI");
   createButton("*VERTEX_LOOKUP",  CSC_VERTEX_LOOKUP,  "CSC_VERTEX_LOOKUP");
   createButton("*MAP_PAL",  CSC_MAP_PAL,  "CSC_MAP_PAL");
   createButton("*MESH_VOLUME",  CSC_MESH_VOLUME,  "CSC_MESH_VOLUME");
   createButton("*DFRAC",  CSC_DFRAC,  "CSC_DFRAC");
   createButton("*LOOKAT",  CSC_LOOKAT,  "CSC_LOOKAT");
   createButton("*WINMSG",  CSC_WINMSG,  "CSC_WINMSG");
   createButton("*WILDCARD",  CSC_WILDCARD,  "CSC_WILDCARD");
   createButton("*GESTALT",  CSC_GESTALT,  "CSC_GESTALT");
   createButton("*ASC2BIN",  CSC_ASC2BIN,  "CSC_ASC2BIN");
   createButton("*FILE_INTERFACE",  CSC_FILE_INTERFACE,  "CSC_FILE_INTERFACE");
   createButton("*KEY_VALUE_INI",  CSC_KEY_VALUE_INI,  "CSC_KEY_VALUE_INI");
   createButton("*TINY_XML",  CSC_TINY_XML,  "CSC_TINY_XML");
   createButton("*KDTREE",  CSC_KDTREE,  "CSC_KDTREE");
   createButton("*MESH_CLEANUP",  CSC_MESH_CLEANUP,  "CSC_MESH_CLEANUP");
   createButton("*FAST_ASTAR",  CSC_FAST_ASTAR,  "CSC_FAST_ASTAR");
   createButton("*SAS",  CSC_SAS,  "CSC_SAS");
   createButton("*COMPRESSION",  CSC_COMPRESSION,  "CSC_COMPRESSION");
   createButton("*SPLIT_MESH",  CSC_SPLIT_MESH,  "CSC_SPLIT_MESH");
   createButton("*ARROW_HEAD",  CSC_ARROW_HEAD,  "CSC_ARROW_HEAD");
   createButton("*SEND_MAIL",  CSC_SEND_MAIL,  "CSC_SEND_MAIL");
   createButton("*SEND_AIM",  CSC_SEND_AIM,  "CSC_SEND_AIM");
   createButton("*EROSION",  CSC_EROSION,  "CSC_EROSION");

    createCheckbox("Debug View", SMC_DEBUG_VIEW, "DebugView", false );
    createCheckbox("Remove T-Junctions", SMC_REMOVE_TJUNCTIONS, "RemoveTjunctions", false );
    createCheckbox("Tesselate", SMC_TESSELATE, "Tesselate", false );
    createCheckbox("Noise Break", SMC_NOISE, "Noise", false);
    createCheckbox("Wireframe", SMC_WIREFRAME, "WireFrame", true);
    createCheckbox("Solid",     SMC_SOLID, "Solid", true);
    createCheckbox("Show Left", SMC_SHOW_LEFT, "ShowLeft", true );
    createCheckbox("Show Right", SMC_SHOW_RIGHT, "ShowRight", true );
    createCheckbox("Show Rings", SMC_SHOW_RINGS, "ShowRings", true );
    createCheckbox("Show Closure", SMC_SHOW_CLOSURE, "ShowClosure", true );
    createCheckbox("Show Split Plane",SMC_SHOW_SPLIT_PLANE,"ShowSplitPlane",true);
    createCheckbox("Collapse Colinear",SMC_COLLAPSE_COLINEAR,"CollapseColinear",false);
    createCheckbox("Edge Intersect", SMC_EDGE_INTERSECT,"EdgeIntersect",true);
    createButton("Test Intersection", SMC_TEST_INTERSECTION,"TestIntersection");
    createButton("Save Left Edges", SMC_SAVE_LEFT_EDGES,"SaveLeftEdges");
    createButton("Save Edge Data", SMC_SAVE_RESULTS,"SaveResults");
    createButton("Test Edge Walk" ,SMC_TEST_EDGE_WALK,"TestEdgeWalk");
    createButton("Save OBJ", SMC_SAVE_OBJ, "SaveObj");
    createButton("Consolidate Mesh", SMC_CONSOLIDATE_MESH,"ConsolidateMesh");
    createButton("Convex Decomposition", SMC_CONVEX_DECOMPOSITION,"ConvexDecomposition");



    CPARSER.Parse("TuiComboBegin Plane");
    CPARSER.Parse("TuiName \"Cut Plane\"");
    CPARSER.Parse("TuiChoices YUP XUP ZUP");
    CPARSER.Parse("TuiArg YUP");
    CPARSER.Parse("TuiScript plane %%1");
    CPARSER.Parse("TuiEnd");
    CPARSER.Parse("");
    AddToken("plane", MC_PLANE );

    createSlider("Plane Offset",SMC_PLANE_D,"PlaneD",-2,2,0,false);
    createSlider("Explode Distance",SMC_EXPLODE,"Explode",0,2,0.2f,false);

    createSlider("Rotate X",SMC_ROTATE_X,"RotateX",0,360,0,false);
    createSlider("Rotate Y",SMC_ROTATE_Y,"RotateY",0,360,0,false);
    createSlider("Rotate Z",SMC_ROTATE_Z,"RotateZ",0,360,0,false);


    CPARSER.Parse("TuiComboBegin TriangulateType");
    CPARSER.Parse("TuiName \"Triangulate Type\"");
    CPARSER.Parse("TuiChoices CONVEX EAR_SPLITTING GAME_SWF");
    CPARSER.Parse("TuiArg EAR_SPLITTING");
    CPARSER.Parse("TuiScript triangulateType %%1");
    CPARSER.Parse("TuiEnd");
    CPARSER.Parse("");
    AddToken("triangulateType", MC_TRIANGULATE_TYPE );


    CPARSER.Parse("TuiPageBegin CodeSuppository");
    CPARSER.Parse("TuiElement MC_MEMORY_REPORT");
    CPARSER.Parse("TuiElement ShowSkeleton");
    CPARSER.Parse("TuiElement ShowMesh");
    CPARSER.Parse("TuiElement ShowCollision");
    CPARSER.Parse("TuiElement SelectCollision");
    CPARSER.Parse("TuiElement PlayAnimation");
    CPARSER.Parse("TuiElement AnimationSpeed");
    CPARSER.Parse("TuiElement FlipWinding");
    CPARSER.Parse("TuiElement ShowWireframe");
    CPARSER.Parse("TuiElement ClearMesh");
    CPARSER.Parse("TuiElement AutoGeometry");
    CPARSER.Parse("TuiElement CSC_BEST_FIT_OBB");
    CPARSER.Parse("TuiElement CSC_BEST_FIT_CAPSULE");
    CPARSER.Parse("TuiElement CSC_BEST_FIT_PLANE");
    CPARSER.Parse("TuiElement CSC_STAN_HULL");
    CPARSER.Parse("TuiElement ConvexDecomposition");
	CPARSER.Parse("TuiElement CSC_TEST_REMOVE_TJUNCTIONS");
	CPARSER.Parse("TuiElement CSC_TEST_ISLAND_GENERATION");
	CPARSER.Parse("TuiElement CSC_TEST_MESH_CONSOLIDATION");
	CPARSER.Parse("TuiElement CSC_TEST_VECTOR_FONT");
#if 0
    CPARSER.Parse("TuiElement CSC_INPARSER");
    CPARSER.Parse("TuiElement CSC_CLIPPER");
    CPARSER.Parse("TuiElement CSC_FRUSTUM");
    CPARSER.Parse("TuiElement CSC_PLANE_TRI");
    CPARSER.Parse("TuiElement CSC_VERTEX_LOOKUP");
    CPARSER.Parse("TuiElement CSC_MAP_PAL");
    CPARSER.Parse("TuiElement CSC_MESH_VOLUME");
    CPARSER.Parse("TuiElement CSC_DFRAC");
    CPARSER.Parse("TuiElement CSC_LOOKAT");
    CPARSER.Parse("TuiElement CSC_WINMSG");
    CPARSER.Parse("TuiElement CSC_WILDCARD");
    CPARSER.Parse("TuiElement CSC_GESTALT");
    CPARSER.Parse("TuiElement CSC_ASC2BIN");
    CPARSER.Parse("TuiElement CSC_FILE_INTERFACE");
    CPARSER.Parse("TuiElement CSC_KEY_VALUE_INI");
    CPARSER.Parse("TuiElement CSC_TINY_XML");
    CPARSER.Parse("TuiElement CSC_KDTREE");
    CPARSER.Parse("TuiElement CSC_MESH_CLEANUP");
    CPARSER.Parse("TuiElement CSC_FAST_ASTAR");
    CPARSER.Parse("TuiElement CSC_SAS");
    CPARSER.Parse("TuiElement CSC_COMPRESSION");
    
    CPARSER.Parse("TuiElement CSC_ARROW_HEAD");
    CPARSER.Parse("TuiElement CSC_SEND_MAIL");
    CPARSER.Parse("TuiElement CSC_SEND_AIM");
    CPARSER.Parse("TuiElement CSC_EROSION");
#endif
    CPARSER.Parse("TuiElement SplitMesh");
    CPARSER.Parse("TuiPageEnd");

    CPARSER.Parse("TuiPageBegin SplitMesh");
    CPARSER.Parse("TuiElement CodeSuppository");
    CPARSER.Parse("TuiElement DebugView");
    CPARSER.Parse("TuiElement RemoveTjunctions");
    CPARSER.Parse("TuiElement Tesselate");
    CPARSER.Parse("TuiElement Noise");
    CPARSER.Parse("TuiElement WireFrame");
    CPARSER.Parse("TuiElement Solid");
    CPARSER.Parse("TuiElement ShowLeft");
    CPARSER.Parse("TuiElement ShowRight");
    CPARSER.Parse("TuiElement ShowRings");
    CPARSER.Parse("TuiElement ShowClosure");
    CPARSER.Parse("TuiElement ShowSplitPlane");
    CPARSER.Parse("TuiElement CollapseColinear");
    CPARSER.Parse("TuiElement EdgeIntersect");
    CPARSER.Parse("TuiElement TestIntersection");
    CPARSER.Parse("TuiElement SaveLeftEdges");
    CPARSER.Parse("TuiElement TestEdgeWalk");
    CPARSER.Parse("TuiElement ConvexDecomposition");
    CPARSER.Parse("TuiElement SaveResults");
    CPARSER.Parse("TuiElement SaveObj");
    CPARSER.Parse("TuiElement Explode");
    CPARSER.Parse("TUiElement Plane");
    CPARSER.Parse("TuiElement PlaneD");
    CPARSER.Parse("TuiElement RotateX");
    CPARSER.Parse("TuiElement RotateY");
    CPARSER.Parse("TuiElement TriangulateType");
    CPARSER.Parse("TuiElement ConsolidateMesh");

    CPARSER.Parse("TuiPageEnd");

// create UI for erosion parameters
    createSlider("Fill Basin",SMC_FILL_BASIN,"FillBasin",0,4000,300,true);
    createSlider("Fill Basin Per", SMC_FILL_BASIN_PER,"FillBasinPer",0,4000,10,true);
    createSlider("Erode Iterations", SMC_ERODE_ITERATIONS,"ErodeIterations",0,5000,100,true);
    createSlider("Erode Rate",SMC_ERODE_RATE,"ErodeRate",0.000001f,0.01f,0.00002f,false);
    createSlider("Erode Power", SMC_ERODE_POWER, "ErodePower", 0, 8, 1, false );
    createSlider("Smooth Rate", SMC_SMOOTH_RATE, "SmoothRate", 0, 1, 0.05f, false );
    createSlider("Erode Threshold", SMC_ERODE_THRESHOLD, "ErodeThreshold", 0, 4, 1, false );
    createSlider("Erode Sedimentation", SMC_ERODE_SEDIMENTATION, "ErodeSedimentation", 0, 8, 1.6f, false );


    CPARSER.Parse("TuiPageBegin Erosion");
    CPARSER.Parse("TuiElement CodeSuppository");
    CPARSER.Parse("TuiElement FillBasin");
    CPARSER.Parse("TuiElement FillBasinPer");
    CPARSER.Parse("TuiElement ErodeIterations");
    CPARSER.Parse("TuiElement ErodeRate");
    CPARSER.Parse("TuiElement ErodePower");
    CPARSER.Parse("TuiElement SmoothRate");
    CPARSER.Parse("TuiElement ErodeThreshold");
    CPARSER.Parse("TuiElement ErodeSedimentation");
    CPARSER.Parse("TuiPageEnd");


    createSlider("Decompose Depth",CSC_DEPTH,"DecomposeDepth",1,40,1,true);
    createSlider("Max Vertices", CSC_MAX_VERTICES,"MaxVertices",8,512,32,true);
    createSlider("Merge Percentage",CSC_MERGE_PERCENTAGE,"MergePercentage",0,100,3,false);
    createSlider("Concavity Percentage",CSC_CONCAVITY_PERCENTAGE,"ConcavityPercentage",0,100,1,false);
    createSlider("Volume Percentage",CSC_VOLUME_PERCENTAGE,"VolumePercentage",0,100,1,false);
    createSlider("Skin Width", CSC_SKIN_WIDTH,"SkinWidth", 0,1,0,false);

    createCheckbox("Fit OBB", CSC_FIT_OBB, "FitObb", false );
    createCheckbox("Remove Tjunctions", CSC_REMOVE_TJUNCTIONS, "CsRemoveTjunctions", false );
    createCheckbox("Initial Island Generation", CSC_INITIAL_ISLAND_GENERATION, "InitialIslandGeneration", false );
    createCheckbox("Island Generation", CSC_ISLAND_GENERATION, "IslandGeneration", false );

    createButton("Perform Decomposition", CSC_CONVEX_DECOMPOSITION, "PerformDecomposition" );

    CPARSER.Parse("TuiPageBegin ConvexDecomposition");
    CPARSER.Parse("TuiElement CodeSuppository");
    CPARSER.Parse("TuiElement DecomposeDepth");
    CPARSER.Parse("TuiElement MaxVertices");
    CPARSER.Parse("TuiElement MergePercentage");
    CPARSER.Parse("TuiElement ConcavityPercentage");
    CPARSER.Parse("TuiElement VolumePercentage");
    CPARSER.Parse("TuiElement SkinWidth");
    CPARSER.Parse("TuiElement CsRemoveTjunctions");
    CPARSER.Parse("TuiElement InitialIslandGeneration");
    CPARSER.Parse("TuiElement IslandGeneration");
    CPARSER.Parse("TuiElement FitObb");
    CPARSER.Parse("TuiElement PerformDecomposition");


    CPARSER.Parse("TuiPage CodeSuppository");


		//
		AddToken("CreateDynamics", MC_CREATE_DYNAMICS );
		AddToken("PsScript", MC_PSSCRIPT);
		AddToken("PsLookAt", MC_PSLOOKAT);

//    CPARSER.Parse("TuiLoad PhysXViewer.psc");

		gLog->Display("Menu System Initialized\r\n");

    mStartup = false;

  }


	bool isExplicit(const char *name)
	{
		bool ret = false;

		if ( strchr(name,':') ||
			   strchr(name,'\\') ||
				 strchr(name,'/') ) ret = true;

		return ret;
	}

  const char * cdi_getFileName(const char *fname,bool search) // convert file name into full path.
  {
  	return FileOpenString(fname,search);
  }

  void         cdi_output(const char *str)
  {
  	gLog->Display("%s", str );
  }


	const char * FileOpenString(const char *fname,bool search) // return the full path name for this file, set 'search' to true to try to 'find' the file.
	{
		const char *ret = fname;
		if ( !isExplicit(fname) )
		{
  	  static char scratch[512];
  	  sprintf(scratch,"%s\\%s", MEDIA_PATH, fname );
			ret = scratch;
		}
  	return ret;


	}


  void add(wchar_t *dest,const wchar_t *src,unsigned int &index)
  {
    if ( src )
    {
      size_t len;
      StringCchLength(src,512,&len);
      for (unsigned int i=0; i<len; i++)
      {
        wchar_t c = *src++;
        dest[index++] = c;
      }
    }
    else
    {
      wchar_t c = 0;
      dest[index++] = c;
    }
    assert(index<512);
  }


  const char * getFileName(const char *fileSpec,const char *title,const char *initial,bool saveMode) // allows the application the opportunity to present a file save dialog box.
  {
  	const char *ret = initial;

  	static int sWhichFileType = 1;
  	char curdir[512];
  	getcwd(curdir,512);
  	wchar_t buffer[257] = L"";

  	OPENFILENAME f;

  	memset (&f, 0, sizeof(OPENFILENAME));
  	f.lStructSize	= sizeof(OPENFILENAME);
  	f.hwndOwner		= GetActiveWindow();
  	f.lpstrFile		= buffer;
  	f.nMaxFile		= 256;
  	f.nFilterIndex	= sWhichFileType;

    char exportName[512] = { 0 };
    if ( initial )
      strcpy(exportName,initial);

//    assert(extension);

    wchar_t _filter[4096];

    CharToWide(fileSpec,_filter,4096);

    wchar_t _title[512];
    CharToWide(title,_title,512);

    wchar_t *scan = _filter;
    while ( *scan )
    {
      if ( *scan == L'|' )
        *scan = 0;
      scan++;
    }
    scan++;
    *scan = 0;

    f.lpstrFilter = _filter;
    f.lpstrTitle =  _title;

  	f.lpstrInitialDir = NULL;

  	mbstowcs(buffer,exportName,512);

    int ok = 0;
    if ( saveMode )
      ok = GetSaveFileName(&f);
    else
      ok = GetOpenFileName(&f);

  	if ( ok )
  	{
  		// save off which file type they chose for next time
  		sWhichFileType = f.nFilterIndex;

  		// first change dir, and get the name in "normal" char format
  		chdir(curdir);

  		static char tmp[512];
  		wcstombs( tmp, buffer, 512 );

  		bool ok = true;

  		if ( saveMode )
  		{
    		FILE *fph = fopen(tmp,"rb");
    		if ( fph )
    		{
    			fclose(fph);
    			ok = MyMessageBox(true,"Are you sure you want to overwrite the file '%s'?", tmp );
    		}
    	}

  		if ( ok )
  			ret = tmp;
  		else
  			ret = 0;
  	}
  	else
  	{
  		ret = 0;
  	}

  	chdir(curdir);



    return ret;
  }


  bool MyMessageBox(bool yesno,const char *fmt, ...)
  {
  	bool ret = false;

  	char buff[2048];
  	wchar_t wbuff[2048];
  	vsprintf(buff, fmt, (char *)(&fmt+1));
  	mbstowcs(wbuff,buff,2048);
    DWORD mtype;
  	if ( yesno )
  		mtype = MB_YESNO;
  	else
  		mtype = MB_OK | MB_ICONEXCLAMATION;

  	int ok = MessageBox(0,wbuff,L"Error",mtype);
  	if ( ok == IDYES) ret = true;

  	return ret;
  }


  void getPoint(ImportHeightMap *h,NxVec3 &v,NxU32 x,NxU32 z)
  {
    v.y = h->getPoint(x,z)*10;
    v.x = (NxF32)x;
    v.z = (NxF32)z;
  }

  bool processMenu(HWND hwnd,unsigned int cmd,float *bmin,float *bmax)
  {

  	bool ret = false;

  	switch ( cmd )
  	{
      case MO_VISUALIZE_NONE:
        break;
      case MO_EXPORT_EZM:
        gCodeSuppository->processCommand(CSC_EXPORT_EZM);
        break;
      case MO_EXPORT_OBJ:
        gCodeSuppository->processCommand(CSC_EXPORT_OBJ);
        break;
      case MO_EXPORT_OGRE:
        gCodeSuppository->processCommand(CSC_EXPORT_OGRE);
        break;
      case MO_IMPORT_MESH:
        if ( gMeshImport )
        {
          const char *filespec = gMeshImport->getFileRequestDialogString();
          if ( filespec )
          {
            const char * fname = getFileName(filespec,"MeshImport compatible data files", 0, false );
            if ( fname )
            {
              SEND_TEXT_MESSAGE(0,"Processing mesh '%s'\r\n", fname );
              gCodeSuppository->importMesh(fname);
            }
          }
          else
          {
            SEND_TEXT_MESSAGE(0,"No mesh importers found!\r\n");
          }
        }
        else
        {
          SEND_TEXT_MESSAGE(0,"MeshImport plugin not found.\r\n");
        }
        break;
  		case MO_RUN_SCRIPT:
 				CPARSER.Parse("PsScript");
  			break;
    	case MO_EXIT:
    		SendMessage(hwnd,WM_CLOSE,0,0);
  		  break;
      default:
        break;
  	}

  	return ret;
  }

  int CommandCallback(int token,int count,const char **arglist)
  {
  	int ret = 0;

    bool state = true;
    float v=0;

    const float *data = 0;
    data = &v;

    if ( count == 2 )
    {
      state = getBool(arglist[1]);
      v = (float) atof(arglist[1]);
    }

		switch ( token )
		{
      case MC_MEMORY_REPORT:
        break;
      case MC_TRIANGULATE_TYPE:
        if ( count == 2 )
        {
          SplitMeshCommand command = SMC_TT_EAR_SPLITTING;
          if ( stricmp(arglist[1],"convex") == 0 )
          {
            command = SMC_TT_CONVEX;
          }
          else if ( stricmp(arglist[1],"ear_splitting") == 0 )
          {
            command = SMC_TT_EAR_SPLITTING;
          }
          else if ( stricmp(arglist[1],"game_swf") == 0 )
          {
            command = SMC_TT_GAME_SWF;
          }
          appCommand(command);
        }
        break;
      case MC_PLANE:
        if ( count == 2 )
        {
          mCommandOk = false;
          SplitMeshCommand command = SMC_PLANE_YUP;
          if ( strcmp(arglist[1],"YUP") == 0 )
          {
            command = SMC_PLANE_YUP;
            CPARSER.Parse("/RotateX 90");
            CPARSER.Parse("/RotateY 90");
          }
          else if ( strcmp(arglist[1],"XUP") == 0 )
          {
            command = SMC_PLANE_XUP;
            CPARSER.Parse("/RotateX 90");
            CPARSER.Parse("/RotateY 0");
          }
          else if ( strcmp(arglist[1],"ZUP") == 0 )
          {
            command = SMC_PLANE_ZUP;
            CPARSER.Parse("/RotateX 0");
            CPARSER.Parse("/RotateY 0");
          }
          mCommandOk = true;
          appCommand(command);
        }
        break;
      case CSC_BEST_FIT_OBB:
      case CSC_BEST_FIT_CAPSULE:
      case CSC_BEST_FIT_PLANE:
	  case CSC_TEST_REMOVE_TJUNCTIONS:
      case CSC_TEST_ISLAND_GENERATION:
      case CSC_TEST_MESH_CONSOLIDATION:
      case CSC_TEST_VECTOR_FONT:
      case CSC_STAN_HULL:
      case CSC_INPARSER:
      case CSC_CLIPPER:
      case CSC_FRUSTUM:
      case CSC_PLANE_TRI:
      case CSC_VERTEX_LOOKUP:
      case CSC_MAP_PAL:
      case CSC_MESH_VOLUME:
      case CSC_DFRAC:
      case CSC_LOOKAT:
      case CSC_CONVEX_DECOMPOSITION:
      case CSC_WINMSG:
      case CSC_WILDCARD:
      case CSC_GESTALT:
      case CSC_ASC2BIN:
      case CSC_FILE_INTERFACE:
      case CSC_KEY_VALUE_INI:
      case CSC_TINY_XML:
      case CSC_KDTREE:
      case CSC_MESH_CLEANUP:
      case CSC_FAST_ASTAR:
      case CSC_SAS:
      case CSC_COMPRESSION:
      case CSC_ARROW_HEAD:
      case CSC_SEND_MAIL:
      case CSC_SEND_AIM:
      case CSC_EROSION:
      case CSC_SPLIT_MESH:
      case CSC_SHOW_SKELETON:
      case CSC_SHOW_MESH:
      case CSC_CLEAR_MESH:
      case CSC_SHOW_WIREFRAME:
      case CSC_PLAY_ANIMATION:
      case CSC_FLIP_WINDING:
      case CSC_AUTO_GEOMETRY:
      case CSC_SHOW_COLLISION:
      case CSC_APEX_CLOTH:
      case CSC_ANIMATION_SPEED:
      case CSC_MERGE_PERCENTAGE:
      case CSC_CONCAVITY_PERCENTAGE:
      case CSC_FIT_OBB:
      case CSC_DEPTH:
      case CSC_VOLUME_PERCENTAGE:
      case CSC_MAX_VERTICES:
      case CSC_SKIN_WIDTH:
      case CSC_REMOVE_TJUNCTIONS:
      case CSC_INITIAL_ISLAND_GENERATION:
      case CSC_ISLAND_GENERATION:
      case CSC_SELECT_COLLISION:
        gCodeSuppository->processCommand( (CodeSuppositoryCommand)token, state, data );
        break;
      case SMC_WIREFRAME:
      case SMC_SOLID:
      case SMC_PLANE_D:
      case SMC_SHOW_LEFT:
      case SMC_SHOW_RIGHT:
      case SMC_SHOW_RINGS:
      case SMC_SHOW_CLOSURE:
      case SMC_EXPLODE:
      case SMC_SHOW_SPLIT_PLANE:
      case SMC_ROTATE_X:
      case SMC_ROTATE_Y:
      case SMC_ROTATE_Z:
      case SMC_COLLAPSE_COLINEAR:
      case SMC_TEST_INTERSECTION:
      case SMC_EDGE_INTERSECT:
      case SMC_SAVE_LEFT_EDGES:
      case SMC_TEST_OBB:
      case SMC_SAVE_RESULTS:
      case SMC_TEST_EDGE_WALK:
      case SMC_SAVE_OBJ:
      case SMC_CONSOLIDATE_MESH:
      case SMC_CONVEX_DECOMPOSITION:
      case SMC_DEBUG_VIEW:
      case SMC_NOISE:
      case SMC_REMOVE_TJUNCTIONS:
      case SMC_TESSELATE:
      case SMC_FILL_BASIN:
      case SMC_FILL_BASIN_PER:
      case SMC_ERODE_ITERATIONS:
      case SMC_ERODE_RATE:
      case SMC_ERODE_POWER:
      case SMC_SMOOTH_RATE:
      case SMC_ERODE_THRESHOLD:
      case SMC_ERODE_SEDIMENTATION:
        if ( mCommandOk )
        {
          appCommand( (SplitMeshCommand)token, state, data );
        }
        break;

			case MC_PSLOOKAT:
				//            0      1      2      3       4      5       6
				// Usage: PsLookAt <eyex> <eyey> <eyez> <lookx> <looky> <lookz>
				if ( count == 7 )
				{
					float eye[3];
					float look[3];

					eye[0] = (float) atof( arglist[1] );
					eye[1] = (float) atof( arglist[2] );
					eye[2] = (float) atof( arglist[3] );

					look[0] = (float) atof(arglist[4] );
					look[1] = (float) atof(arglist[5] );
					look[2] = (float) atof(arglist[6] );

					lookAt(eye,look);

				}
				break;
			case MC_PSSCRIPT:
				{
					const char *fname = 0;
					if ( count >= 2 )
					{
						fname = arglist[1];
					}
#if TODO
  				SoftFileInterface *sfi = gSoftBodySystem->getSoftFileInterface();
  				if ( sfi )
  				{
  					fname = sfi->getLoadFileName(".psc", "Select a demo script to run.");
  				}
  				if ( fname )
  				{
  					CPARSER.Parse("Run \"%s\"",fname);
  				}
#endif
				}
				break;
		}


    return ret;
  }


  void createSlider(const char *title,int token,const char *cmd,float smin,float smax,float sdefault,bool isint)
  {
  	AddToken(cmd,token);
  	CPARSER.Parse("TuiSliderBegin %s", cmd);
  	CPARSER.Parse("TuiName \"%s\"",title);
  	CPARSER.Parse("TuiSliderRange %f %f",smin,smax);
  	CPARSER.Parse("TuiArg %f",sdefault);
    if ( isint )
    {
      CPARSER.Parse("TuiInt true");
    }
  	CPARSER.Parse("TuiScript %s %%1",cmd);
  	CPARSER.Parse("TuiSliderEnd");
  }

  void createCheckbox(const char *title,int token,const char *cmd,bool state)
  {
  	AddToken(cmd,token);
  	CPARSER.Parse("TuiCheckboxBegin %s", cmd);
  	CPARSER.Parse("TuiName \"%s\"",title);
  	if ( state )
    	CPARSER.Parse("TuiArg true");
    else
    	CPARSER.Parse("TuiArg false");
  	CPARSER.Parse("TuiScript %s %%1",cmd);
  	CPARSER.Parse("TuiCheckboxEnd");
  }

  void createButton(const char *title,int token,const char *cmd)
  {
    char scratch[512];
    strcpy(scratch,cmd);
    char *space = strchr(scratch,' ');
    if ( space ) *space = 0;

  	AddToken(scratch,token);
  	CPARSER.Parse("TuiButtonBegin %s", scratch);
  	CPARSER.Parse("TuiName \"%s\"",title);
  	CPARSER.Parse("TuiScript %s",cmd);
  	CPARSER.Parse("TuiCheckboxEnd");
  }

  bool  mCommandOk:1;
  bool  mStartup:1;

  HMENU	mMainMenu;
  HMENU mFileMenu;
  HMENU mVisualizationMenu;
  NxU32 mMenuCount;
  MenuItem *mMenuItems;
};

static class MyMenu *gMyMenu=0;

HMENU createMyMenu(void)
{
	gMyMenu = MEMALLOC_NEW(MyMenu);
	return gMyMenu->mMainMenu;
}



bool  processMenu(HWND hwnd,unsigned int cmd,float *bmin,float *bmax)
{
	bool ret = false;

  if ( gMyMenu )
  {
  	ret = gMyMenu->processMenu(hwnd,cmd,bmin,bmax);
  }
	return ret;

}


//==================================================================================
void CALLBACK gOnUIPageEvent( UINT nEvent, int nControlID, CDXUTControl* pControl )
{
	CDXUTDialog *dialog = pControl->m_pDialog;

	void *userdata = dialog->GetUserData();
	if ( userdata )
	{
		TuiElement *page = (TuiElement *) userdata;
		TextUserInterface *tui = gTui;

		if ( page )
		{
			TuiElement *ret = page->GetElement( nControlID-1 );

			switch ( ret->GetType() )
			{
				case TT_MULTI_SELECT:
					{
						switch( nEvent )
						{
							case EVENT_LISTBOX_ITEM_DBLCLK:
							{
								break;
							}
							case EVENT_LISTBOX_SELECTION:
							{
								CDXUTListBox *pListBox = (CDXUTListBox *)pControl;
								TuiChoiceVector &choices = ret->GetChoices();
								int count = (int)choices.size();
								for (int i=0; i<count; i++)
								{
									DXUTListBoxItem *item = pListBox->GetItem(i);
									TuiChoice  &choice = choices[i];
									assert(item);
									if ( item )
									{
										if ( choice.GetState() != item->bSelected )
										{
											choice.SetState(item->bSelected);
											const char *args[2];
											args[0] = choice.GetArg().Get();

											if ( choice.GetState() )
												args[1] = "true";
											else
												args[1] = "false";

											page->ExecuteElement( nControlID-1, 2, args, tui );
										}
									}
								}

							}
						}
					}
					break;
				case TT_SLIDER:
					{
						CDXUTSlider *slider = (CDXUTSlider *) pControl;
						int v = slider->GetValue();
						float fv = ret->GetSliderValue(v);
						char scratch[512];
  						sprintf(scratch,"%0.3f", fv );

						const char *args[1];
						args[0] = scratch;
						page->ExecuteElement( nControlID-1, 1, args, tui );
					}
					break;
				case TT_COMBO:
					{
						CDXUTComboBox *combo = (CDXUTComboBox *) pControl;
						DXUTComboBoxItem *pItem = combo->GetSelectedItem();
						wchar_t *string = pItem->strText;
						char scratch[512];
						wcstombs(scratch, string, 512 );
						const char *args[1];
						args[0] = scratch;
						page->ExecuteElement( nControlID-1, 1, args,tui );
					
 						// now, do we need to hide any items (or show them)?
 						page->OnComboBoxChange( scratch, nControlID-1 );
					}
					break;
				case TT_BUTTON:
				case TT_PAGE:
					page->ExecuteElement( nControlID-1, 0, 0, tui );
					break;
				case TT_CHECKBOX:
					{
						CDXUTCheckBox *pCheck = (CDXUTCheckBox *)pControl;
						bool state = pCheck->GetChecked();
						const char *args[1];
						if ( state )
						{
							args[0] = "true";
						}
						else
						{
							args[0] = "false";
						}
						page->ExecuteElement( nControlID-1, 1, args, tui );

 						// now, do we need to hide any items (or show them)?
 						page->OnCheckboxChange( state, nControlID-1 );
					}
					break;
			}
			if ( tui )
			{
				if ( ret && ret->GetType() == TT_PAGE )
				{
					// tell the last page it is now exiting
					TuiElement *exitingPage = tui->GetPage();
					if ( exitingPage )
					{
						exitingPage->OnExit( exitingPage );
					}

					tui->SetPage(ret);

					// tell new page it is now loading
					TuiElement *loadingPage = tui->GetPage();
					if ( loadingPage )
					{
						loadingPage->OnLoad( loadingPage );
					}
				}
			}
		}
	}
	else
	{
		// it's for the console
		CDXUTEditBox *edit = (CDXUTEditBox *) pControl;
		switch ( nEvent )
		{
			case EVENT_EDITBOX_STRING:
				{
					LPCWSTR text = edit->GetText();
					char scratch[1024];
					WideToChar(text,scratch,1024);
					gLog->Display("%s\r\n", scratch);
					CPARSER.Parse("%s",scratch);
					edit->SetText(L"",true);
				}
				break;
			case EVENT_EDITBOX_CHANGE:
				break;
			default:
				break;
		}
	}

}

void saveMenuState(void)
{
  CPARSER.Parse("TuiSave PhysXViewer.psc");
}

