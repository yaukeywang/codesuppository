#ifndef SNIPPETS_UTF_H
#define SNIPPETS_UTF_H

namespace UTF_CONVERT
{

/*
** Convert a UTF-8 string to microsoft unicode (UTF-16?). 
**
** Returns the number of characters needed to hold the resulting string.
**
** zFilenameOut may be NULL.
*/
int utf8ToUnicode(WCHAR *zFilenameOut, size_t zFilenameOutLen, const char *zFilename);

/*
** Convert a UTF-8 string to microsoft unicode (UTF-16?). 
**
** Space to hold the returned string is obtained from malloc.
*/
WCHAR *utf8ToUnicode(const char *zFilename);

/*
** Convert microsoft unicode to UTF-8.
**
** Returns the number of characters needed to hold the resulting string.
**
** zFilenameOut may be NULL.
*/
int unicodeToUtf8(char *zFilenameOut, size_t zFilenameOutLen, const WCHAR *zWideFilename);

/*
** Convert microsoft unicode to UTF-8.  Space to hold the returned string is
** obtained from malloc().
*/
char *unicodeToUtf8(const WCHAR *zWideFilename);

/*
** Convert an ansi string to microsoft unicode, based on the
** current codepage settings for file apis.
** 
** Returns the number of characters needed to hold the resulting string.
**
** zFilenameOut may be NULL.
*/
int mbcsToUnicode(WCHAR *zFilenameOut, size_t zFilenameOutLen, const char *zFilename);

/*
** Convert an ansi string to microsoft unicode, based on the
** current codepage settings for file apis.
** 
** Space to hold the returned string is obtained
** from malloc.
*/
WCHAR *mbcsToUnicode(const char *zFilename);

/*
** Convert microsoft unicode to multibyte character string, based on the
** user's Ansi codepage.
**
** Returns the number of characters needed to hold the resulting string.
**
** zFilenameOut may be NULL.
*/
int unicodeToMbcs(char *zFilenameOut, size_t zFilenameOutLen, const WCHAR *zWideFilename);

/*
** Convert microsoft unicode to multibyte character string, based on the
** user's Ansi codepage.
**
** Space to hold the returned string is obtained from
** malloc().
*/
char *unicodeToMbcs(const WCHAR *zWideFilename);

}

#endif
