#pragma warning(disable:4996)
#include "systemcall.h"
#include <wchar.h>
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



#ifdef WIN32
#include <windows.h>
#endif

#include "UserMemAlloc.h"

NxI32 systemCall(const char* strFunct,const char* strstrParams,bool hideWindow)
{
  NxI32 rc = 1;

#ifdef WIN32
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ProcessInfo;
	char Args[4096];
	char *pEnvCMD = NULL;
	char *pDefaultCMD = "CMD.EXE";

	memset(&StartupInfo, 0, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
  if ( hideWindow )
  {
  	StartupInfo.wShowWindow = SW_HIDE;
  }
  else
  {
  	StartupInfo.wShowWindow = SW_SHOWNORMAL;
  }

	Args[0] = 0;

	pEnvCMD = getenv("COMSPEC");

	if(pEnvCMD){

		strcpy(Args, pEnvCMD);
	}
	else{
		strcpy(Args, pDefaultCMD);
	}

	// "/c" option - Do the command then terminate the command window
	strcat(Args, " /c ");
	//the application you would like to run from the command window
	strcat(Args, strFunct);
	strcat(Args, " ");
	//the parameters passed to the application being run from the command window.
	strcat(Args, strstrParams);

  wchar_t wArgs[512];
 	mbstowcs(wArgs,Args,512 );

	if (!CreateProcess( NULL, wArgs, NULL, NULL, FALSE,	CREATE_NEW_CONSOLE,	NULL,	NULL,	&StartupInfo,	&ProcessInfo))
	{
		rc = GetLastError();
	}
  else
  {
  	WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
  	if(!GetExitCodeProcess(ProcessInfo.hProcess, (LPDWORD)&rc))
	  	rc = 0;

	  CloseHandle(ProcessInfo.hThread);
	  CloseHandle(ProcessInfo.hProcess);
  }
#endif

	return rc;

}

