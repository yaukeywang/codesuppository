#include "UserMemAlloc.h"

#ifdef WIN32
#include "IncludeWindows.h"
#include "utf.h"

namespace UTF_CONVERT
{

/*
** Convert a UTF-8 string to microsoft unicode (UTF-16?). 
*/
int utf8ToUnicode(WCHAR *zFilenameOut, size_t zFilenameOutLen, const char *zFilename){
  if ( zFilenameOut==0 ){
    return MultiByteToWideChar(CP_UTF8, 0, zFilename, -1, NULL, 0);
  }
  else {
    return MultiByteToWideChar(CP_UTF8, 0, zFilename, -1, zFilenameOut, zFilenameOutLen);
  }
}
WCHAR *utf8ToUnicode(const char *zFilename){
  int nChar;
  WCHAR *zWideFilename;

  nChar = MultiByteToWideChar(CP_UTF8, 0, zFilename, -1, NULL, 0);
  zWideFilename = (WCHAR *)MEMALLOC_MALLOC( nChar*sizeof(zWideFilename[0]) );
  if( zWideFilename==0 ){
    return 0;
  }
  nChar = MultiByteToWideChar(CP_UTF8, 0, zFilename, -1, zWideFilename, nChar);
  if( nChar==0 ){
    MEMALLOC_FREE(zWideFilename);
    zWideFilename = 0;
  }
  return zWideFilename;
}

/*
** Convert microsoft unicode to UTF-8.
*/
int unicodeToUtf8(char *zFilenameOut, size_t zFilenameOutLen, const WCHAR *zWideFilename){
  if ( zFilenameOut==0 ){
    return WideCharToMultiByte(CP_UTF8, 0, zWideFilename, -1, 0, 0, 0, 0);
  }
  else {
    return WideCharToMultiByte(CP_UTF8, 0, zWideFilename, -1, zFilenameOut, zFilenameOutLen,
                              0, 0);
  }
}
char *unicodeToUtf8(const WCHAR *zWideFilename){
  int nByte;
  char *zFilename;

  nByte = WideCharToMultiByte(CP_UTF8, 0, zWideFilename, -1, 0, 0, 0, 0);
  zFilename = (char *)MEMALLOC_MALLOC( nByte );
  if( zFilename==0 ){
    return 0;
  }
  nByte = WideCharToMultiByte(CP_UTF8, 0, zWideFilename, -1, zFilename, nByte,
                              0, 0);
  if( nByte == 0 ){
    MEMALLOC_FREE(zFilename);
    zFilename = 0;
  }
  return zFilename;
}

/*
** Convert an ansi string to microsoft unicode, based on the
** current codepage settings for file apis.
*/
int mbcsToUnicode(WCHAR *zFilenameOut, size_t zFilenameOutLen, const char *zFilename){
  int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;

  if ( zFilenameOut==0 ){
    return MultiByteToWideChar(codepage, 0, zFilename, -1, NULL,0)*sizeof(WCHAR);
  }
  else {
    return MultiByteToWideChar(codepage, 0, zFilename, -1, zFilenameOut, zFilenameOutLen);
  }
}
WCHAR *mbcsToUnicode(const char *zFilename){
  int nByte;
  WCHAR *zMbcsFilename;
  int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;

  nByte = MultiByteToWideChar(codepage, 0, zFilename, -1, NULL,0)*sizeof(WCHAR);
  zMbcsFilename = (WCHAR *)MEMALLOC_MALLOC( nByte*sizeof(zMbcsFilename[0]) );
  if( zMbcsFilename==0 ){
    return 0;
  }
  nByte = MultiByteToWideChar(codepage, 0, zFilename, -1, zMbcsFilename, nByte);
  if( nByte==0 ){
    MEMALLOC_FREE(zMbcsFilename);
    zMbcsFilename = 0;
  }
  return zMbcsFilename;
}

/*
** Convert microsoft unicode to multibyte character string, based on the
** user's Ansi codepage.
*/
int unicodeToMbcs(char *zFilenameOut, size_t zFilenameOutLen, const WCHAR *zWideFilename){
  int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;

  if ( zFilenameOut==0 ){
    return WideCharToMultiByte(codepage, 0, zWideFilename, -1, 0, 0, 0, 0);
  }
  else {
    return WideCharToMultiByte(codepage, 0, zWideFilename, -1, zFilenameOut, zFilenameOutLen,
                              0, 0);
  }
}
char *unicodeToMbcs(const WCHAR *zWideFilename){
  int nByte;
  char *zFilename;
  int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;

  nByte = WideCharToMultiByte(codepage, 0, zWideFilename, -1, 0, 0, 0, 0);
  zFilename = (char *)MEMALLOC_MALLOC( nByte );
  if( zFilename==0 ){
    return 0;
  }
  nByte = WideCharToMultiByte(codepage, 0, zWideFilename, -1, zFilename, nByte,
                              0, 0);
  if( nByte == 0 ){
    MEMALLOC_FREE(zFilename);
    zFilename = 0;
  }
  return zFilename;
}

}
#endif