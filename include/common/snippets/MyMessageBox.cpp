#pragma warning(disable:4996)
#include "MyMessageBox.h"

#if defined(WIN32)
#include <direct.h>
#include <windows.h>
#include <Commdlg.h>
#include <strsafe.h>
#endif

#pragma warning(disable:4995)
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <wchar.h>

#if defined(WIN32)
#include <windows.h>
#endif

static char *stristr(const char *str,const char *key)       // case insensitive str str
{
  char istr[2048];
  char ikey[2048];
  strcpy(istr,str);
  strcpy(ikey,key);
  strlwr(istr);
  strlwr(ikey);

  char *foo = strstr(istr,ikey);
  if ( foo )
  {
    unsigned int loc = (unsigned int)(foo - istr);
    foo = (char *)str+loc;
  }

  return foo;
}



static bool CharToWide(const char *source,void *dest,int maxlen)
{
  bool ret = false;

  ret = true;
  mbstowcs( (wchar_t *) dest, source, maxlen );

  return ret;
}


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
  NxI32 dprintf(const char* format,...);
  dprintf("Error : [%s]\n",buff);
  assert(false);
#else
//ignore on linux
#endif
}


bool myMessageBoxYesNo(const char *fmt, ...)
{
  bool ret = false;

  char buff[2048];
#if defined(WIN32)
#if defined(UNICODE)
  wchar_t wbuff[2048];
  wbuff[2047] = 0;
  _vsnprintf(buff,2047, fmt, (char *)(&fmt+1));
  mbstowcs(wbuff,buff,2048);
  int v = MessageBox(0,wbuff,L"Are you sure?",MB_YESNO);
  if ( v == IDYES )
  {
    ret = true;
  }
#else
  MessageBox(0,buff,"Error",MB_OK | MB_ICONEXCLAMATION);
#endif
#elif defined(_XBOX)
  NxI32 dprintf(const char* format,...);
  dprintf("Error : [%s]\n",buff);
  assert(false);
#else
//ignore on linux
#endif
  return ret;
}


static void add(wchar_t *dest,const wchar_t *src,unsigned int &index)
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
}

const char * getFileName(const char *fileType,const char *initial,const char *description,bool saveMode) // allows the application the opportunity to present a file save dialog box.
{
  const char *ret = initial;

  const char *extension = fileType;

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

  assert(extension);

  wchar_t _filter[512];

  wchar_t _ext[512];
  wchar_t _desc[512];

  CharToWide(description,_desc,512);
  CharToWide(extension,_ext,512);

  unsigned int index = 0;
  _filter[0] = 0;

  add(_filter,_desc,index);
  add(_filter,L" (*",index);
  add(_filter,_ext,index);
  add(_filter,L")",index);
  add(_filter,0,index);
  add(_filter,L"*",index);
  add(_filter,_ext,index);
  add(_filter,0,index);
  add(_filter,0,index);
  add(_filter,0,index);


  f.lpstrFilter = _filter;
  f.lpstrTitle =  _desc;

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

    char *ext = stristr(tmp,extension);
    if ( ext == 0 )
    {
      strcat(tmp,extension);
    }

    bool ok = true;

    if ( saveMode )
    {
      FILE *fph = fopen(tmp,"rb");
      if ( fph )
      {
        fclose(fph);
        ok = myMessageBoxYesNo("Are you sure you want to overwrite the file '%s'?", tmp );
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



