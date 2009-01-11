#include "MyMessageBox.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <wchar.h>

#if defined(WIN32)

#include <windows.h>
#pragma warning(disable:4996)

#endif

void myMessageBox(const char *fmt, ...)
{
  char buff[2048];
#if defined(WIN32)
#if defined(UNICODE)
  wchar_t wbuff[2048];
  wbuff[2047] = 0;
  _vsnprintf(buff,2047, fmt, (char *)(&fmt+1));
  mbstowcs(wbuff,buff,2048);
  MessageBox(0,wbuff,L"Error",MB_OK | MB_ICONEXCLAMATION);
#else
  MessageBox(0,buff,"Error",MB_OK | MB_ICONEXCLAMATION);
#endif
#elif defined(_XBOX)
  HeI32 dprintf(const char* format,...);
  dprintf("Error : [%s]\n",buff);
  assert(false);
#else
  assert(false);
#endif
}


