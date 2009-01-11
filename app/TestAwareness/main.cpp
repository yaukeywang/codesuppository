#include <windows.h>
#include <assert.h>
#include <stdio.h>

#include "main.h"
#include "entity.h"
#include "unit_test.h"
#include "clock.h"
#include "../../include/common/snippets/UserMemAlloc.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#pragma warning(disable:4996)

HDC				backbuffer = 0;
HBITMAP		backbufferbitmap = 0;
char			textbuffer[1024];

static HeI32 gDisplay=0;

//======================================================================================
//======================================================================================
HeI32 WINAPI WinMain(HINSTANCE appInstance, HINSTANCE prevInstance, LPSTR cmdLine, HeI32 showCmd)
{
	MSG			msg;

  HeI32 count = atoi( cmdLine );
  if ( count == 0 ) count = 1000;


//  assert( unit_test() );

	Application* App = MEMALLOC_NEW(Application)(count);

	if (App->Startup(appInstance))
	{
		delete (App);
		exit(-1);
	}

	while (!App->ExitApp())
	{
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		App->Process();
	}

	App->Shutdown();

	delete(App);

	return 0;
}


//======================================================================================
//======================================================================================
Application::Application(HeI32 simcount)
{
  createEntityFactory(simcount);
  mSimulationCount  = simcount;
	mExitApplication	= false;
	mAppInstance			= NULL;
	mWindow						= NULL;
	mFrameRate				= 0;
	mUpdateFrameRate	= timeGetTime();
	mStep							= false;
	mPause						= false;
}


//======================================================================================
Application::~Application(void)
{
  releaseEntityFactory();
}


//======================================================================================
HeI32 Application::Startup(HINSTANCE hinstance)
{
	srand(timeGetTime());

	mAppInstance									= hinstance;

	mWindowClass.cbSize						= sizeof(WNDCLASSEX);
	mWindowClass.cbClsExtra				= 0;
	mWindowClass.cbWndExtra				= 0;
	mWindowClass.hbrBackground		= (HBRUSH)GetStockObject(COLOR_BACKGROUND);
	mWindowClass.hCursor					= LoadCursor(NULL, IDC_ARROW);
	mWindowClass.hIcon						= NULL;
	mWindowClass.hIconSm					= NULL;
	mWindowClass.hInstance				= hinstance;
	mWindowClass.style						= NULL;
	mWindowClass.lpfnWndProc			= Application::WindowProc;
	mWindowClass.lpszMenuName			= NULL;
	mWindowClass.lpszClassName		= "spheretest_window_class";

	ATOM atom = RegisterClassEx(&mWindowClass);

	mWindow = CreateWindow(	"spheretest_window_class",
													"AWARENESS TEST APPLICATION : by John W. Ratcliff.",
													WS_OVERLAPPEDWINDOW, 30, 30, SCREEN_WIDTH+8, SCREEN_HEIGHT+28,
													NULL, NULL, mAppInstance, NULL);

	SetWindowLong(mWindow, GWL_USERDATA, (HeI32)this);
	ShowWindow(mWindow, SW_SHOW);


	HDC hdc = GetDC(mWindow);
	backbuffer = CreateCompatibleDC(hdc);
	HBITMAP bitmap = CreateCompatibleBitmap(hdc, SCREEN_WIDTH, SCREEN_HEIGHT);
	SelectObject(backbuffer, bitmap);
	ReleaseDC(mWindow, hdc);

	SetTextColor(backbuffer, 0x00ffff00);
	SetBkMode(backbuffer, TRANSPARENT);

	return 0;
}


//======================================================================================
HeI32 Application::Shutdown(void)
{
	DeleteObject(backbufferbitmap);
	DeleteDC(backbuffer);
	backbuffer = 0;

	return 0;
}


//======================================================================================
HeI32 Application::Process(void)
{
	HDC hdc = GetDC(mWindow);
	Render(hdc);
	ReleaseDC(mWindow, hdc);

	return 0;
}


//======================================================================================
HeI32 Application::Render(HDC hdc)
{
	BitBlt(backbuffer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, backbuffer, 0, 0, BLACKNESS);

	mFrameRate++;
	if (timeGetTime() > mUpdateFrameRate)
	{
		mUpdateFrameRate += 1000;
		itoa(mFrameRate, mBuffer, 10);
		mFrameRate = 0;
	}

	TextOut(backbuffer, 2, 2, mBuffer, strlen(mBuffer));
  PrintText(2,15,0xFFFFFF,"Press 'R' to toggle rendering");
  PrintText(2,30,0xFFFFFF,"Press 'M' to generate memory usage report");
  PrintText(2,45,0xFFFFFF,"Press 'ESC' to exit");

  HeF32 dtime = CLOCK::doClockFrame();
  if ( dtime > (1.0f/60.0f ) )
    dtime = 1.0f/60.0f;

	//  Here is where we put in the code to render the spheres
	if (!mPause)
	{
		processEntityFactory(dtime);
	}
	else if (mStep)
	{
		mPause = true;
		mStep = false;
    processEntityFactory(dtime);
	}

  if ( gDisplay )
  {
    renderEntityFactory();
  }



	BitBlt(hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, backbuffer, 0, 0, SRCCOPY);

	return 0;
}

void DrawPoint(HeI32 locx,HeI32 locy,HeI32 color)
{
  DrawLine(locx-1,locy,locx+2,locy,color);
  DrawLine(locx,locy-1,locx,locy+2,color);
}

//======================================================================================
HeI32 DrawCircle(HeI32 locx, HeI32 locy, HeI32 radius, HeI32 color)
{
	if (backbuffer)
	{
		HPEN pen = CreatePen(PS_SOLID, 0, color);
		HGDIOBJ oldpen = SelectObject(backbuffer, pen);

		HeI32 left, top, right, bottom;
		HeI32	startx, starty, endx, endy;

		left		= locx - radius;
		top			= locy - radius;
		right		= locx + radius;
		bottom	= locy + radius;
		startx	= locx;
		starty  = locy - radius;
		endx	= locx;
		endy  = locy - radius;

		Arc(backbuffer, left, top, right, bottom, startx, starty, endx, endy);

		SelectObject(backbuffer, oldpen);
		DeleteObject(pen);
	}

	return 0;
}


//======================================================================================
HeI32 PrintText(HeI32 x, HeI32 y, HeI32 color, char* output, ...)
{
	vsprintf(textbuffer, output, (char *)(&output+1));
	HeI32 oldcolor = SetTextColor(backbuffer, color);
	TextOut(backbuffer, x, y, textbuffer, strlen(textbuffer));
	SetTextColor(backbuffer, oldcolor);
	return 0;
}


//======================================================================================
HeI32 DrawLine(HeI32 x1, HeI32 y1, HeI32 x2, HeI32 y2, HeI32 color)
{
	HPEN pen = CreatePen(PS_SOLID, 0, color);
	HGDIOBJ oldpen = SelectObject(backbuffer, pen);

	MoveToEx(backbuffer, x1, y1, NULL);
	LineTo(backbuffer, x2, y2);

	SelectObject(backbuffer, oldpen);
	DeleteObject(pen);

	return 0;
}


//======================================================================================
LRESULT CALLBACK Application::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Application* app = (Application*)GetWindowLong(hwnd, GWL_USERDATA);

	switch (msg)
	{
		case WM_CLOSE:
		{
			app->CloseApp(true);
			return 0;
		}

		case WM_CHAR:
		{
		  switch (wparam)
			{
        case VK_ESCAPE:
          app->CloseApp(true);
          break;
        case 'm':
        case 'M':
          if ( 1 )
          {
          }
          break;
        case 'a':
        case 'A':
          break;
	  	  case 't':
    		case 'T':
          break;
        case 'f':
        case 'F':
          break;
        case 'r':
        case 'R':
          gDisplay = 1-gDisplay;
          break;
				case VK_SPACE:
					app->PauseApp();
					return 0;
				case 13:
					app->RunApp();
					return 0;
			}
		}
	} // end switch(msg)

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

