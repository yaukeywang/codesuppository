#ifndef MAIN_H
#define MAIN_H


#include "../../include/common/snippets/UserMemAlloc.h"

/***********************************************************************/
/** MAIN        : The windows test application demonstrating the       */
/**               SphereTree system for Game Programming Gems 2        */
/**                                                                    */
/**               Written by John W. Ratcliff jratcliff@att.net        */
/***********************************************************************/



//======================================================================================
class Application;




//======================================================================================
class Application
{
public:
													Application(HeI32 simcount);
													~Application(void);

	HeI32											Startup(HINSTANCE hinstnace);
	HeI32											Shutdown(void);
	HeI32											Process(void);
	HeI32											Render(HDC hdc);

	void										PauseApp(void)	{ mStep = true; mPause = true; };
	void										RunApp(void)		{ mStep = false; mPause = false; };

	bool										ExitApp(void) { return mExitApplication; };
	void										CloseApp(bool close) { mExitApplication = close; };

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	HINSTANCE								mAppInstance;
	WNDCLASSEX							mWindowClass;
	HWND										mWindow;

	HeI32											mFrameRate;
	HeU32						mUpdateFrameRate;
	char										mBuffer[32];
	char										mTextBuffer[1024];

	bool										mExitApplication;
	bool										mPause;
	bool										mStep;
  HeI32                     mSimulationCount;
};

#endif MAIN_H
