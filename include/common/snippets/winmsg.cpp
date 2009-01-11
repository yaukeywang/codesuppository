#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#pragma warning(disable:4996)

#ifdef WIN32
#include <windows.h>

#include "winmsg.h"

/*!  
** 
** Copyright (c) 2007 by John W. Ratcliff mailto:jratcliff@infiniplex.net
**
** Portions of this source has been released with the PhysXViewer application, as well as 
** Rocket, CreateDynamics, ODF, and as a number of sample code snippets.
**
** If you find this code useful or you are feeling particularily generous I would
** ask that you please go to http://www.amillionpixels.us and make a donation
** to Troy DeMolay.
**
** DeMolay is a youth group for young men between the ages of 12 and 21.  
** It teaches strong moral principles, as well as leadership skills and 
** public speaking.  The donations page uses the 'pay for pixels' paradigm
** where, in this case, a pixel is only a single penny.  Donations can be
** made for as small as $4 or as high as a $100 block.  Each person who donates
** will get a link to their own site as well as acknowledgement on the
** donations blog located here http://www.amillionpixels.blogspot.com/
**
** If you wish to contact me you can use the following methods:
**
** Skype Phone: 636-486-4040 (let it ring a long time while it goes through switches)
** Skype ID: jratcliff63367
** Yahoo: jratcliff63367
** AOL: jratcliff1961
** email: jratcliff@infiniplex.net
** Personal website: http://jratcliffscarab.blogspot.com
** Coding Website:   http://codesuppository.blogspot.com
** FundRaising Blog: http://amillionpixels.blogspot.com
** Fundraising site: http://www.amillionpixels.us
** New Temple Site:  http://newtemple.blogspot.com
**
**
** The MIT license:
**
** Permission is hereby granted, MEMALLOC_FREE of charge, to any person obtaining a copy 
** of this software and associated documentation files (the "Software"), to deal 
** in the Software without restriction, including without limitation the rights 
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
** copies of the Software, and to permit persons to whom the Software is furnished 
** to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all 
** copies or substantial portions of the Software.

** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
** WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/




static INT_PTR CALLBACK _MsgWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  WinMsgReceive *winmsg = (WinMsgReceive*)LongToPtr(GetWindowLongPtr(hwnd, GWLP_USERDATA));

  if(winmsg)
  {
    switch(msg)
    {
      case WM_COPYDATA:
      {
        COPYDATASTRUCT *data = (COPYDATASTRUCT*) lparam;
        if(data && data->lpData)
        {
					winmsg->receiveMessage( (const char *)data->lpData, data->cbData );
        }
        return 0;
        break;
      }
    }
  }

  return DefWindowProc(hwnd, msg, wparam, lparam);
}

WinMsgReceive::WinMsgReceive(void)
{
	mHwnd = 0;
}

WinMsgReceive::~WinMsgReceive(void)
{
	if ( mHwnd )
	{
  	HWND msgWindow = (HWND) mHwnd;
  	::DestroyWindow(msgWindow);
  }
}

bool WinMsgReceive::initMsg(const char *app) // initialize
{

	bool ret = false;

	if ( mHwnd == 0 )
	{
		HWND msgWindow = 0;

    WNDCLASSEX wcex;
    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style          = 0;
    wcex.lpfnWndProc    = (WNDPROC)_MsgWindowProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = sizeof(void*);
    wcex.hInstance      = 0;
    wcex.hIcon          = 0;
    wcex.hCursor        = 0;
    wcex.hbrBackground  = 0;
    wcex.lpszMenuName   = 0;
#ifdef UNICODE
    wcex.lpszClassName  = L"WinMsg";
#else
    wcex.lpszClassName  = "WinMsg";
#endif
    wcex.hIconSm        = 0;
    RegisterClassEx(&wcex);

  	wchar_t wapp[512];
	  mbstowcs(wapp,app,512);
#ifdef UNICODE
    msgWindow = ::CreateWindow(L"WinMsg",wapp, 0, 0, 0, 0, 0, 0, 0, 0, this);
#else
    msgWindow = ::CreateWindow("WinMsg",app, 0, 0, 0, 0, 0, 0, 0, 0, this);
#endif
    if(msgWindow)
    {
      SetWindowLongPtr(msgWindow, GWLP_USERDATA, PtrToLong(this));
      mHwnd = (void *) msgWindow;
    }
  }

  if ( mHwnd )
    ret = true;

	return ret;
}

bool sendWinMsg(const char *app,const char *fmt,...)
{
	bool ret = false;

#ifdef UNICODE
	wchar_t wapp[512];
	mbstowcs(wapp,app,512);
	HWND targetwindow = ::FindWindow(L"WinMsg", wapp );
#else
	HWND targetwindow = ::FindWindow("WinMsg", app );
#endif

	if ( targetwindow )
	{
		ret = true;
		char buffer[1024];
  	_vsnprintf(buffer, 1023, fmt, (char *)(&fmt+1));
		HeU32 len = (HeU32)strlen(buffer);

    COPYDATASTRUCT copydata;

    copydata.dwData = 0;
    copydata.lpData = buffer;
    copydata.cbData = len+1;

    ::SendMessage(targetwindow, WM_COPYDATA, (WPARAM)0, (LPARAM)&copydata);
	}

  return ret;
}

bool hasWindow(const char *app) // returns true if this application window exists on the system.
{
	bool ret = false;

#ifdef UNICODE
	wchar_t wapp[512];
	mbstowcs(wapp,app,512);
	HWND targetwindow = ::FindWindow(L"WinMsg", wapp );
#else
	HWND targetwindow = ::FindWindow("WinMsg", app );
#endif

	if ( targetwindow )
	{
		ret = true;
  }
  return ret;
}

bool sendWinMsgBinary(const char *app,const void *mem,HeU32 len)
{
	bool ret = false;

#ifdef UNICODE
	wchar_t wapp[512];
	mbstowcs(wapp,app,512);
	HWND targetwindow = ::FindWindow(L"WinMsg", wapp );
#else
	HWND targetwindow = ::FindWindow("WinMsg", app );
#endif

	if ( targetwindow )
	{
		ret = true;

    COPYDATASTRUCT copydata;

    copydata.dwData = 0;
    copydata.lpData = (PVOID)mem;
    copydata.cbData = len;

    ::SendMessage(targetwindow, WM_COPYDATA, (WPARAM)0, (LPARAM)&copydata);
	}

  return ret;
}


void WinMsgReceive::checkWinMsg(void)
{
  if(mHwnd )
  {
  	HWND msgWindow = (HWND) mHwnd;
    MSG	msg;
    while (PeekMessage(&msg, msgWindow, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
}

#else

// no unix version

WinMsgReceive::WinMsgReceive(void)
{
  mHwnd = 0;
}

WinMsgReceive::~WinMsgReceive(void)
{
}


bool WinMsgReceive::initMsg(const char *app)
{
  bool ret = false;
  return ret;
}

void WinMsgReeive::checkWinMsg(void)
{
}



#endif
