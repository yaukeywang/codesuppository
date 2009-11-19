#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#pragma warning(disable:4995)

#define NOMINMAX
#include "common/dxut/dxstdafx.h"
#include "resource.h"
#include "menu.h"
#include "common/snippets/cparser.h"
#include "common/snippets/filesystem.h"
#include "common/snippets/log.h"
#include "common/tui/tui.h"
#include "common/snippets/sutil.h"
#include "ThreadFrac.h"
#include "Tfrac.h"
#include "common/snippets/wildcard.h"
#include "common/snippets/ffind.h"
#include "mappal.h"
#include "Pd3d/pd3d.h"
#include "RenderDebug.h"
#include "terrain.h"
#include "common/snippets/MyMessageBox.h"

extern Tfrac *gTfrac;

bool gView3d=false;

enum MenuOptions
{
  MO_RUN_SCRIPT = 10000,
	MO_EXIT,
};

enum MyCommand
{
	MC_CREATE_DYNAMICS = 11000,
	MC_PSSCRIPT,
	MC_PSLOOKAT,
  MC_PREVIEW_ONLY,
  MC_ITERATION_COUNT,
  MC_COLOR_PALETTE,
	MC_RECTANGLE_SUBDIVISION,
	MC_USE_THREADING,
	MC_CLOCK_CYCLES,
  MC_SMOOTH_COLOR,
  MC_FRACTAL_COORDINATES,
  MC_VIEW3D,
  MC_WIREFRAME_OVERLAY,
  MC_CLAMP_LOW,
  MC_CLAMP_HIGH,
  MC_CLAMP_SCALE,
  MC_DEFAULT_MANDELBROT,
  MC_FILTER_FRACTAL,
	MC_OPTIMIZE_MESH,
  MC_ENVIRONMENT_TEXTURE,
  MC_SHOW_NORMALS,
  MC_ROTATION_SPEED,
  MC_FLOATING_POINT_RESOLUTION,
  MC_MEMORY_REPORT,
};

class MyMenu : public FileSystem, public CommandParserInterface, public TuiSaveCallback, public SendTextMessage
{
public:
  MyMenu(void)
  {
    gSendTextMessage = this;
    mMainMenu = CreateMenu();

  	HMENU m = CreatePopupMenu();
		mFileMenu = m;

  	AppendMenu( mMainMenu, MF_POPUP, (UINT_PTR)m, L"&File" );
  	  AppendMenu( m, MF_STRING, MO_RUN_SCRIPT,  L"Run Demo Script");
  	  AppendMenu( m, MF_STRING, MO_EXIT, L"E&xit");

	  // ok, now initialize the scripted menu interface.
	  gFileSystem       = this;

	  gLog              = MEMALLOC_NEW(Log)("ThreadFrac.txt",true);
    gLog->Display("ThreadFrac initialized.\r\n");

	  gTheCommandParser = MEMALLOC_NEW(TheCommandParser);
		gTui              = MEMALLOC_NEW(TextUserInterface)("ThreadFrac.tui");
    gTui->setTuiSaveCallback(this);


  }

  void tuiSaveCallback(FILE *fph)
  {
    if ( gTfrac )
    {
      BigFloat xleft,xright,ytop;
      tf_getFractalCoordinates(gTfrac,xleft,xright,ytop);
      char str1[2048];
      char str2[2048];
      char str3[2048];
      toString(xleft,str1);
      toString(xright,str2);
      toString(ytop,str3);

      fprintf(fph,"FractalCoordinates %s %s %s\r\n", str1, str2, str3 );

      NxF32 eye[3];
      NxF32 look[3];
      getLookAt(eye,look);
      fprintf(fph,"PsLookAt %0.9f %0.9f %0.9f  %0.9f %0.9f %0.9f\r\n", eye[0], eye[1], eye[2], look[0], look[1], look[2] );
    }
  }

  void menuSetup(void)
  {
    mEnvironmentTexture = SGET("cubemap.dds");
		mStartup = true;

    AddToken("FractalCoordinates",MC_FRACTAL_COORDINATES);

    createButton("Default Mandelbrot", MC_DEFAULT_MANDELBROT, "DefaultMandelbrot");
    createButton("Filter Fractal", MC_FILTER_FRACTAL, "FilterFractal");
		createButton("Optimize Mesh", MC_OPTIMIZE_MESH, "OptimizeMesh");

    createCheckbox("Preview Only", MC_PREVIEW_ONLY, "PreviewOnly", false );
		createCheckbox("Use Rectangle Subdivision", MC_RECTANGLE_SUBDIVISION,"RectangleSubdivision",true);
		createCheckbox("Use Multi-Threading", MC_USE_THREADING,"UseThreading",false);
    createCheckbox("View3D", MC_VIEW3D,"View3d",false);
    createCheckbox("Wireframe Overlay", MC_WIREFRAME_OVERLAY,"WireframeOverlay",false);
    createCheckbox("Show Normals", MC_SHOW_NORMALS,"ShowNormals",false);

    createSlider("Color Scale", MC_SMOOTH_COLOR, "SmoothColor", 1, 32, 16, true );
    createSlider("Iteration Count", MC_ITERATION_COUNT, "IterationCount", 40, 100000, 1024, true );
		createSlider("Clock Cycles Per Frame Mhz", MC_CLOCK_CYCLES, "ClockCycles", 20, 1000, 100, true );

    createSlider("Clamp Low",       MC_CLAMP_LOW,   "ClampLow",  0.0f, 1, 0.0f, false );
    createSlider("Clamp High",      MC_CLAMP_HIGH,  "ClampHigh",  0.001f, 0.3f, 0.3f, false );
    createSlider("Clamp Scale",     MC_CLAMP_SCALE, "ClampScale", 0.1f, 8, 1.0f, false );
    createSlider("Rotation Speed",  MC_ROTATION_SPEED, "RotationSpeed", -10, 10, 0.0f, false );

    CPARSER.Parse("TuiComboBegin ColorPalette");
    CPARSER.Parse("TuiName \"Color Palette\"");

    {
      WildCard *wc = createWildCard("*.pal");
      FileFind ff(wc,0);
      StringVector list;
      ff.GetFiles(list);
      StringVector::iterator i;
      for (i=list.begin(); i!=list.end(); i++)
      {
        CPARSER.Parse("TuiChoice \"%s\"", (*i).c_str() );
      }
      releaseWildCard(wc);
    }

    CPARSER.Parse("TuiArg default.pal");
    CPARSER.Parse("TuiScript ColorPalette %%1");
    CPARSER.Parse("TuiEnd");

    AddToken("ColorPalette", MC_COLOR_PALETTE );

    CPARSER.Parse("TuiComboBegin FloatingPointResolution");
    CPARSER.Parse("TuiName \"Float Resolution\"");

    CPARSER.Parse("TuiChoices SMALL MEDIUM BIGFLOAT FIXED32");

    CPARSER.Parse("TuiArg SMALL");
    CPARSER.Parse("TuiScript FloatingPointResolution %%1");
    CPARSER.Parse("TuiEnd");

    AddToken("FloatingPointResolution", MC_FLOATING_POINT_RESOLUTION );

    CPARSER.Parse("TuiComboBegin EnvironmentTexture");
    CPARSER.Parse("TuiName \"Environment Texture\"");

    {
      WildCard *wc = createWildCard("cubemap*.dds");
      FileFind ff(wc,"..\\..\\media\\CodeSuppository");
      StringVector list;
      ff.GetFiles(list);
      StringVector::iterator i;
      for (i=list.begin(); i!=list.end(); i++)
      {
        CPARSER.Parse("TuiChoice \"%s\"", (*i).c_str() );
      }
      releaseWildCard(wc);
    }

    CPARSER.Parse("TuiArg %s", mEnvironmentTexture.Get());
    CPARSER.Parse("TuiScript EnvironmentTexture %%1");
    CPARSER.Parse("TuiEnd");

    AddToken("EnvironmentTexture", MC_ENVIRONMENT_TEXTURE );

    createButton("Memory Report", MC_MEMORY_REPORT, "MemoryReport");


    CPARSER.Parse("TuiPageBegin MainMenu");
    CPARSER.Parse("TuiElement DefaultMandelbrot");
    CPARSER.Parse("TuiElement PreviewOnly");
		CPARSER.Parse("TuiElement RectangleSubdivision");
		CPARSER.Parse("TuiElement FloatingPointResolution");
		CPARSER.Parse("TuiElement UseThreading");
    CPARSER.Parse("TuiElement View3d");
    CPARSER.Parse("TuiElement WireframeOverlay");
    CPARSER.Parse("TuiElement ShowNormals");
		CPARSER.Parse("TuiElement FilterFractal");
		CPARSER.Parse("TuiElement OptimizeMesh");
    CPARSER.Parse("TuiElement SmoothColor");
    CPARSER.Parse("TuiElement IterationCount");
		CPARSER.Parse("TuiElement ClockCycles");
		CPARSER.Parse("TuiElement ClampLow");
		CPARSER.Parse("TuiElement ClampHigh");
		CPARSER.Parse("TuiElement ClampScale");
    CPARSER.Parse("TuiElement RotationSpeed");
		CPARSER.Parse("TuiElement ColorPalette");
    CPARSER.Parse("TuiElement EnvironmentTexture");
    CPARSER.Parse("TuiElement FractalCoordinates");
    CPARSER.Parse("TuiElement MemoryReport");
    CPARSER.Parse("TuiPageEnd");

    CPARSER.Parse("TuiPage MainMenu");


		//
		AddToken("CreateDynamics", MC_CREATE_DYNAMICS );
		AddToken("PsScript", MC_PSSCRIPT);
		AddToken("PsLookAt", MC_PSLOOKAT);

    CPARSER.Parse("TuiLoad ThreadFrac.psc");
    CPARSER.Parse("TuiPageExecute MainMenu");



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

  bool processMenu(HWND hwnd,NxU32 cmd,NxF32 *bmin,NxF32 *bmax)
  {

  	bool ret = false;

  	switch ( cmd )
  	{
  		case MO_RUN_SCRIPT:
 				CPARSER.Parse("PsScript");
  			break;
    	case MO_EXIT:
    		SendMessage(hwnd,WM_CLOSE,0,0);
  		  break;
  	}

  	return ret;
  }

  NxI32 CommandCallback(NxI32 token,NxI32 count,const char **arglist)
  {
  	NxI32 ret = 0;

    saveMenuState();

		switch ( token )
		{
      case MC_MEMORY_REPORT:
        break;

      case MC_FLOATING_POINT_RESOLUTION:
        if ( count == 2 )
        {
          MultiFloatType type = MFT_MEDIUM;
          if ( stricmp(arglist[1],"SMALL") == 0 )
            type = MFT_SMALL;
          else if ( stricmp(arglist[1],"MEDIUM") == 0 )
            type = MFT_MEDIUM;
          else if ( stricmp(arglist[1],"BIGFLOAT") == 0 )
            type = MFT_BIG;
          else if ( stricmp(arglist[1],"FIXED32") == 0 )
            type = MFT_FIXED32;
          tf_setFloatingPointResolution(gTfrac,type);
          gLog->Display("Setting Floating Point Resolution to: %s\r\n", arglist[1] );
        }
        break;
      case MC_SHOW_NORMALS:
        if ( count == 2 )
        {
          setShowNormals( getBool(arglist[1]) );
        }
        break;
      case MC_ENVIRONMENT_TEXTURE:
        if ( count == 2 )
        {
          const char *t = arglist[1];
          if ( gFileSystem ) t = gFileSystem->FileOpenString(t,true);
          gPd3d->setEnvironmentTexture(t);
        }
        break;
      case MC_ROTATION_SPEED:
        if ( count == 2 )
        {
          NxF32 rspeed = (NxF32)atof( arglist[1] );
          setRotationSpeed(rspeed);
        }
        break;
		  case MC_OPTIMIZE_MESH:
				if ( !mStartup )
				{
					tf_state(gTfrac,TS_OPTIMIZE_MESH);
				}
				break;
      case MC_FILTER_FRACTAL:
        if ( !mStartup )
        {
          tf_state(gTfrac,TS_FILTER_FRACTAL);
        }
        break;
      case MC_DEFAULT_MANDELBROT:
				if ( !mStartup )
				{
          BigFloat xleft;
          BigFloat xright;
          BigFloat ytop;
          xleft.FromDouble(-2.5);
          xright.FromDouble(0.75);
          ytop.FromDouble(-1.5);
          tf_setFractalCoordinates(gTfrac,xleft,xright,ytop);
          tf_action(gTfrac,FA_MOUSE_CENTER,false,1024/2,768/2);
				}
        break;
      case MC_CLAMP_LOW:
        if ( count == 2 && gTfrac )
        {
          NxF32 c = (NxF32) atof( arglist[1] );
          tf_state(gTfrac,TS_CLAMP_LOW,false,0,c);
        }
        break;
      case MC_CLAMP_HIGH:
        if ( count == 2 && gTfrac )
        {
          NxF32 c = (NxF32) atof( arglist[1] );
          tf_state(gTfrac,TS_CLAMP_HIGH,false,0,c);
        }
        break;
      case MC_CLAMP_SCALE:
        if ( count == 2 && gTfrac )
        {
          NxF32 c = (NxF32) atof( arglist[1] );
          tf_state(gTfrac,TS_CLAMP_SCALE,false,0,c);
        }
        break;
      case MC_WIREFRAME_OVERLAY:
        if ( count == 2 && gTfrac )
        {
          bool state = getBool(arglist[1]);
          tf_state(gTfrac,TS_WIREFAME_OVERLAY,state);
        }
        break;
      case MC_VIEW3D:
        if ( count == 2 )
        {
					gView3d = getBool(arglist[1]);
          if ( gView3d )
          {
            NxF32 eye[3];
            NxF32 look[3];
            look[0] = 0;
            look[1] = 0;
            look[2] = 0;
            eye[0] = 200;
            eye[1] = 250;
            eye[2] = 200;
            lookAt(eye,look);
          }
        }
        break;
      case MC_FRACTAL_COORDINATES:
        if ( count == 4 && gTfrac )
        {
          BigFloat xleft =  arglist[1];
          BigFloat xright = arglist[2];
          BigFloat ytop   = arglist[3];
          tf_setFractalCoordinates(gTfrac,xleft,xright,ytop);
        }
        break;
      case MC_COLOR_PALETTE:
        if ( count == 2 && gTfrac )
        {
          tf_setPal(gTfrac,arglist[1]);
        }
        break;
		  case MC_ITERATION_COUNT:
				if ( count == 2 && gTfrac )
				{
					NxU32 icount = (NxU32)atoi(arglist[1]);
					tf_state(gTfrac,TS_ITERATION_COUNT,false,icount);
				}
				break;
		  case MC_CLOCK_CYCLES:
				if ( count == 2 && gTfrac )
				{
					NxU32 icount = (NxU32)atoi(arglist[1]);
					tf_state(gTfrac,TS_CLOCK_CYCLES,false,icount);
				}
				break;
			case MC_USE_THREADING:
				if ( count == 2 && gTfrac )
				{
					bool state = getBool(arglist[1]);
  			  tf_state(gTfrac,TS_THREADING,state);
  			}
				break;

			case MC_SMOOTH_COLOR:
				if ( count == 2 && gTfrac )
				{
					NxI32 cscale  = atoi(arglist[1]);
					tf_state(gTfrac,TS_SMOOTH_COLOR,false,cscale);
				}
				break;

      case MC_PREVIEW_ONLY:
        if ( count == 2 && gTfrac )
        {
          bool state = getBool(arglist[1]);
          tf_state(gTfrac,TS_PREVIEW_ONLY,state);
        }
        break;
			case MC_RECTANGLE_SUBDIVISION:
        if ( count == 2 && gTfrac )
        {
          bool state = getBool(arglist[1]);
          tf_state(gTfrac,TS_RECTANGLE_SUBDIVISION,state);
        }
        break;
			case MC_PSLOOKAT:
				//            0      1      2      3       4      5       6
				// Usage: PsLookAt <eyex> <eyey> <eyez> <lookx> <looky> <lookz>
				if ( count == 7 )
				{
					NxF32 eye[3];
					NxF32 look[3];

					eye[0] = (NxF32) atof( arglist[1] );
					eye[1] = (NxF32) atof( arglist[2] );
					eye[2] = (NxF32) atof( arglist[3] );

					look[0] = (NxF32) atof(arglist[4] );
					look[1] = (NxF32) atof(arglist[5] );
					look[2] = (NxF32) atof(arglist[6] );

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


  void createSlider(const char *title,NxI32 token,const char *cmd,NxF32 smin,NxF32 smax,NxF32 sdefault,bool isint)
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

  void createCheckbox(const char *title,NxI32 token,const char *cmd,bool state)
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

  void createButton(const char *title,NxI32 token,const char *cmd)
  {
    char scratch[512];
    strcpy(scratch,cmd);
    char *space = strchr(scratch,' ');
    if ( space ) *space = 0;

  	AddToken(scratch,token);
  	CPARSER.Parse("TuiButtonBegin %s", scratch);
  	CPARSER.Parse("TuiName \"%s\"",title);
  	CPARSER.Parse("TuiScript %s",cmd);
    CPARSER.Parse("TuiExecuteOk false");
  	CPARSER.Parse("TuiCheckboxEnd");
  }

  void   sendTextMessage(NxU32 priority,const char * fmt,...)
  {
    char wbuff[8192];
    wbuff[8191] = 0;
    _vsnprintf(wbuff,8191, fmt, (char *)(&fmt+1));
    gLog->Display("%s",wbuff);
  }


	bool mStartup;
  HMENU	mMainMenu;
  HMENU mFileMenu;
  StringRef       mEnvironmentTexture;
};

static class MyMenu *gMyMenu=0;

HMENU createMyMenu(void)
{
	gMyMenu = MEMALLOC_NEW(MyMenu);
	return gMyMenu->mMainMenu;
}



bool  processMenu(HWND hwnd,NxU32 cmd,NxF32 *bmin,NxF32 *bmax)
{
	bool ret = false;

  if ( gMyMenu )
  {
  	ret = gMyMenu->processMenu(hwnd,cmd,bmin,bmax);
  }
	return ret;

}


//==================================================================================
void CALLBACK gOnUIPageEvent( UINT nEvent, NxI32 nControlID, CDXUTControl* pControl )
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
								//DXUTListBoxItem *pItem = ((CDXUTListBox *)pControl)->GetItem( ((CDXUTListBox *)pControl)->GetSelectedIndex( -1 ) );
								break;
							}
							case EVENT_LISTBOX_SELECTION:
							{
								CDXUTListBox *pListBox = (CDXUTListBox *)pControl;
								TuiChoiceVector &choices = ret->GetChoices();
								NxI32 count = (NxI32)choices.size();
								for (NxI32 i=0; i<count; i++)
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

											page->ExecuteElement( nControlID-1, 2, args, tui, false );
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
						NxI32 v = slider->GetValue();
						NxF32 fv = ret->GetSliderValue(v);
						char scratch[512];
  						sprintf(scratch,"%0.3f", fv );

						const char *args[1];
						args[0] = scratch;
						page->ExecuteElement( nControlID-1, 1, args, tui, false );
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
						page->ExecuteElement( nControlID-1, 1, args,tui, false );
					
 						// now, do we need to hide any items (or show them)?
 						page->OnComboBoxChange( scratch, nControlID-1 );
					}
					break;
				case TT_BUTTON:
				case TT_PAGE:
					page->ExecuteElement( nControlID-1, 0, 0, tui, false );
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
						page->ExecuteElement( nControlID-1, 1, args, tui, false );

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
  CPARSER.Parse("TuiSave ThreadFrac.psc");
}


void MenuSetup(void)
{
  if ( gMyMenu )
  {
    gMyMenu->menuSetup();
  }
}
