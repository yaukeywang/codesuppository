#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <queue>

#pragma warning(disable:4996 4100 4189)

#define MAXPARSEBUFFER 2048

#ifdef WIN32
#include <windows.h>
#endif

#include "winmsg.h"
#include "UserMemAlloc.h"

/*!
**
** Copyright (c) 2009 by John W. Ratcliff mailto:jratcliffscarab@gmail.com
**
** If you wish to contact me you can use the following methods:
**
** email: jratcliffscarab@gmail.com
** Personal website: http://jratcliffscarab.blogspot.com
** Coding Website:   http://codesuppository.blogspot.com
** FundRaising Blog: http://amillionpixels.blogspot.com
** Fundraising site: http://www.amillionpixels.us
** New Temple Site:  http://newtemple.blogspot.com
**
**
** The MIT license:
**
** Permission is hereby granted, freeof charge, to any person obtaining a copy
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

namespace WINMSG
{

#if defined(__APPLE__) || defined(__CELLOS_LV2__) || defined(LINUX)
#define stricmp(a, b) strcasecmp((a), (b))
#endif

/*******************************************************************/
/******************** InParser.h  ********************************/
/*******************************************************************/
class InPlaceParserInterface
{
public:
	virtual int ParseLine(int lineno,int argc,const char **argv) =0;  // return TRUE to continue parsing, return FALSE to abort parsing process
};

enum SeparatorType
{
	ST_DATA,        // is data
	ST_HARD,        // is a hard separator
	ST_SOFT,        // is a soft separator
	ST_EOS          // is a comment symbol, and everything past this character should be ignored
};

class InPlaceParser
{
public:
	InPlaceParser(void)
	{
		Init();
	}

	InPlaceParser(char *data,int len)
	{
		Init();
		SetSourceData(data,len);
	}

	InPlaceParser(const char *fname)
	{
		Init();
		SetFile(fname);
	}

	~InPlaceParser(void);

	void Init(void)
	{
		mQuoteChar = 34;
		mData = 0;
		mLen  = 0;
		mMyAlloc = false;
		for (int i=0; i<256; i++)
		{
			mHard[i] = ST_DATA;
			mHardString[i*2] = (char)i;
			mHardString[i*2+1] = 0;
		}
		mHard[0]  = ST_EOS;
		mHard[32] = ST_SOFT;
		mHard[9]  = ST_SOFT;
		mHard[13] = ST_SOFT;
		mHard[10] = ST_SOFT;
	}

	void SetFile(const char *fname); // use this file as source data to parse.

	void SetSourceData(char *data,int len)
	{
		mData = data;
		mLen  = len;
		mMyAlloc = false;
	};

	int  Parse(InPlaceParserInterface *callback); // returns true if entire file was parsed, false if it aborted for some reason

	int ProcessLine(int lineno,char *line,InPlaceParserInterface *callback);

	const char ** GetArglist(char *source,int &count); // convert source string into an arg list, this is a destructive parse.

	void SetHardSeparator(char c) // add a hard separator
	{
		mHard[c] = ST_HARD;
	}

	void SetHard(char c) // add a hard separator
	{
		mHard[c] = ST_HARD;
	}


	void SetCommentSymbol(char c) // comment character, treated as 'end of string'
	{
		mHard[c] = ST_EOS;
	}

	void ClearHardSeparator(char c)
	{
		mHard[c] = ST_DATA;
	}


	void DefaultSymbols(void); // set up default symbols for hard seperator and comment symbol of the '#' character.

	bool EOS(char c)
	{
		if ( mHard[c] == ST_EOS )
		{
			return true;
		}
		return false;
	}

	void SetQuoteChar(char c)
	{
		mQuoteChar = c;
	}

private:


	inline char * AddHard(int &argc,const char **argv,char *foo);
	inline bool   IsHard(char c);
	inline char * SkipSpaces(char *foo);
	inline bool   IsWhiteSpace(char c);
	inline bool   IsNonSeparator(char c); // non seperator,neither hard nor soft

	bool   mMyAlloc; // whether or not *I* allocated the buffer and am responsible for deleting it.
	char  *mData;  // ascii data to parse.
	int    mLen;   // length of data
	SeparatorType  mHard[256];
	char   mHardString[256*2];
	char           mQuoteChar;
};

/*******************************************************************/
/******************** InParser.cpp  ********************************/
/*******************************************************************/
void InPlaceParser::SetFile(const char *fname)
{
	if ( mMyAlloc )
	{
		::free(mData);
	}
	mData = 0;
	mLen  = 0;
	mMyAlloc = false;

	FILE *fph = fopen(fname,"rb");
	if ( fph )
	{
		fseek(fph,0L,SEEK_END);
		mLen = ftell(fph);
		fseek(fph,0L,SEEK_SET);
		if ( mLen )
		{
			mData = (char *) ::malloc(sizeof(char)*(mLen+1));
			int ok = (int)fread(mData, mLen, 1, fph);
			if ( !ok )
			{
				::free(mData);
				mData = 0;
			}
			else
			{
				mData[mLen] = 0; // zero byte terminate end of file marker.
				mMyAlloc = true;
			}
		}
		fclose(fph);
	}
}

InPlaceParser::~InPlaceParser(void)
{
	if ( mMyAlloc )
	{
		::free(mData);
	}
}

#define MAXARGS 512

bool InPlaceParser::IsHard(char c)
{
	return mHard[c] == ST_HARD;
}

char * InPlaceParser::AddHard(int &argc,const char **argv,char *foo)
{
	while ( IsHard(*foo) )
	{
		const char *hard = &mHardString[*foo*2];
		if ( argc < MAXARGS )
		{
			argv[argc++] = hard;
		}
		foo++;
	}
	return foo;
}

bool   InPlaceParser::IsWhiteSpace(char c)
{
	return mHard[c] == ST_SOFT;
}

char * InPlaceParser::SkipSpaces(char *foo)
{
	while ( !EOS(*foo) && IsWhiteSpace(*foo) ) foo++;
	return foo;
}

bool InPlaceParser::IsNonSeparator(char c)
{
	if ( !IsHard(c) && !IsWhiteSpace(c) && c != 0 ) return true;
	return false;
}


int InPlaceParser::ProcessLine(int lineno,char *line,InPlaceParserInterface *callback)
{
	int ret = 0;

	const char *argv[MAXARGS];
	int argc = 0;

	char *foo = line;

	while ( !EOS(*foo) && argc < MAXARGS )
	{

		foo = SkipSpaces(foo); // skip any leading spaces

		if ( EOS(*foo) ) break;

		if ( *foo == mQuoteChar ) // if it is an open quote
		{
			foo++;
			if ( argc < MAXARGS )
			{
				argv[argc++] = foo;
			}
			while ( !EOS(*foo) && *foo != mQuoteChar ) foo++;
			if ( !EOS(*foo) )
			{
				*foo = 0; // replace close quote with zero byte EOS
				foo++;
			}
		}
		else
		{

			foo = AddHard(argc,argv,foo); // add any hard separators, skip any spaces

			if ( IsNonSeparator(*foo) )  // add non-hard argument.
			{
				bool quote  = false;
				if ( *foo == mQuoteChar )
				{
					foo++;
					quote = true;
				}

				if ( argc < MAXARGS )
				{
					argv[argc++] = foo;
				}

				if ( quote )
				{
					while (*foo && *foo != mQuoteChar ) foo++;
					if ( *foo ) *foo = 32;
				}

				// continue..until we hit an eos ..
				while ( !EOS(*foo) ) // until we hit EOS
				{
					if ( IsWhiteSpace(*foo) ) // if we hit a space, stomp a zero byte, and exit
					{
						*foo = 0;
						foo++;
						break;
					}
					else if ( IsHard(*foo) ) // if we hit a hard separator, stomp a zero byte and store the hard separator argument
					{
						const char *hard = &mHardString[*foo*2];
						*foo = 0;
						if ( argc < MAXARGS )
						{
							argv[argc++] = hard;
						}
						foo++;
						break;
					}
					foo++;
				} // end of while loop...
			}
		}
	}

	if ( argc )
	{
		ret = callback->ParseLine(lineno, argc, argv );
	}

	return ret;
}

int  InPlaceParser::Parse(InPlaceParserInterface *callback) // returns true if entire file was parsed, false if it aborted for some reason
{
	assert( callback );
	if ( !mData ) return 0;

	int ret = 0;

	int lineno = 0;

	char *foo   = mData;
	char *begin = foo;


	while ( *foo )
	{
		if ( *foo == 10 || *foo == 13 )
		{
			lineno++;
			*foo = 0;

			if ( *begin ) // if there is any data to parse at all...
			{
				int v = ProcessLine(lineno,begin,callback);
				if ( v ) ret = v;
			}

			foo++;
			if ( *foo == 10 ) foo++; // skip line feed, if it is in the carraige-return line-feed format...
			begin = foo;
		}
		else
		{
			foo++;
		}
	}

	lineno++; // lasst line.

	int v = ProcessLine(lineno,begin,callback);
	if ( v ) ret = v;
	return ret;
}


void InPlaceParser::DefaultSymbols(void)
{
	SetHardSeparator(',');
	SetHardSeparator('(');
	SetHardSeparator(')');
	SetHardSeparator('<');
	SetHardSeparator('>');
	SetHardSeparator(':');
	SetHardSeparator('=');
	SetHardSeparator('[');
	SetHardSeparator(']');
	SetHardSeparator('{');
	SetHardSeparator('}');
	SetCommentSymbol('#');
}


const char ** InPlaceParser::GetArglist(char *line,int &count) // convert source string into an arg list, this is a destructive parse.
{
	const char **ret = 0;

	static const char *argv[MAXARGS];
	int argc = 0;

	char *foo = line;

	while ( !EOS(*foo) && argc < MAXARGS )
	{

		foo = SkipSpaces(foo); // skip any leading spaces

		if ( EOS(*foo) ) break;

		if ( *foo == mQuoteChar ) // if it is an open quote
		{
			foo++;
			if ( argc < MAXARGS )
			{
				argv[argc++] = foo;
			}
			while ( !EOS(*foo) && *foo != mQuoteChar ) foo++;
			if ( !EOS(*foo) )
			{
				*foo = 0; // replace close quote with zero byte EOS
				foo++;
			}
		}
		else
		{

			foo = AddHard(argc,argv,foo); // add any hard separators, skip any spaces

			if ( IsNonSeparator(*foo) )  // add non-hard argument.
			{
				bool quote  = false;
				if ( *foo == mQuoteChar )
				{
					foo++;
					quote = true;
				}

				if ( argc < MAXARGS )
				{
					argv[argc++] = foo;
				}

				if ( quote )
				{
					while (*foo && *foo != mQuoteChar ) foo++;
					if ( *foo ) *foo = 32;
				}

				// continue..until we hit an eos ..
				while ( !EOS(*foo) ) // until we hit EOS
				{
					if ( IsWhiteSpace(*foo) ) // if we hit a space, stomp a zero byte, and exit
					{
						*foo = 0;
						foo++;
						break;
					}
					else if ( IsHard(*foo) ) // if we hit a hard separator, stomp a zero byte and store the hard separator argument
					{
						const char *hard = &mHardString[*foo*2];
						*foo = 0;
						if ( argc < MAXARGS )
						{
							argv[argc++] = hard;
						}
						foo++;
						break;
					}
					foo++;
				} // end of while loop...
			}
		}
	}

	count = argc;
	if ( argc )
	{
		ret = argv;
	}

	return ret;
}

static INT_PTR CALLBACK _MsgWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class WindowsMessage
{
public:
  WindowsMessage(void)
  {
    mType = 0;
    mLen = 0;
    mMessage = 0;
  }

  WindowsMessage(const char *message)
  {
    mType = 0;
    mLen  = strlen(message)+1;
    mMessage = (char *)MEMALLOC_MALLOC(mLen);
    memcpy(mMessage,message,mLen);
  }

  WindowsMessage(const char *blobType,const void *mem,HeU32 len)
  {
    HeU32 slen = strlen(blobType)+1;
    mType = (char *)MEMALLOC_MALLOC(slen);
    memcpy(mType,blobType,slen);
    mLen = len;
    mMessage = (char *)MEMALLOC_MALLOC(mLen);
    memcpy(mMessage,mem,mLen);
  }

  WindowsMessage(const WindowsMessage &msg)
  {
    mLen = msg.mLen;
    mMessage = (char *)MEMALLOC_MALLOC(mLen);
    memcpy(mMessage,msg.mMessage,mLen);
    mType = 0;
    if ( msg.mType )
    {
      HeU32 slen = strlen(msg.mType)+1;
      mType = (char *)MEMALLOC_MALLOC(slen);
      memcpy(mType,msg.mType,slen);
    }
  }

  ~WindowsMessage(void)
  {
    if (mType) MEMALLOC_FREE(mType);
    if ( mMessage ) MEMALLOC_FREE(mMessage);
  }

  char    *mType;
  char    *mMessage;
  HeU32    mLen;
};

typedef USER_STL::queue< WindowsMessage > WindowsMessageQueue;

class MyWinMsg : public WinMsg
{
public:

	~MyWinMsg(void)
	{
#ifdef WIN32
		if ( mHwnd )
		{
			HWND msgWindow = (HWND) mHwnd;
			::DestroyWindow(msgWindow);
		}
#endif
	}

    MyWinMsg(const char *app)
    {
#ifdef WIN32
        mHwnd = 0;
        if ( app )
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

            #ifdef UNICODE
         	wchar_t wapp[512];
        	mbstowcs(wapp,app,512);
            msgWindow = ::CreateWindow(L"WinMsg",wapp, 0, 0, 0, 0, 0, 0, 0, 0, this);
            #else
            msgWindow = ::CreateWindow("WinMsg",app, 0, 0, 0, 0, 0, 0, 0, 0, this);
            #endif

            if(msgWindow)
            {
              SetWindowLongPtr(msgWindow, GWLP_USERDATA, PtrToLong(this));
              mHwnd = msgWindow;
            }
      }
#endif
    }



    void * getDataBlob(const char *key,const char *value,const void *mem,HeU32 len,HeU32 &olen)
    {
        HeU32 klen = strlen(key);
        HeU32 vlen = strlen(value);

        olen = klen+vlen+2+sizeof(HeU32)+len;

        char *blob = (char *)MEMALLOC_MALLOC(olen);

        char *dest = blob;
        memcpy(dest,key,klen+1);
        dest+=(klen+1);
        memcpy(dest,value,vlen+1);
        dest+=(vlen+1);
        HeU32 *destlen = (HeU32 *)dest;
        *destlen = len;
        dest+=sizeof(HeU32);
        memcpy(dest,mem,len);

        return blob;
    }

	virtual const char * receiveWindowsMessage(void)
    {
		const char *ret = 0;
        checkWinMsg();

        if ( !mStrings.empty() )
        {
            mCurrent = mStrings.front();
            mStrings.pop();
            ret = mCurrent.mMessage;
        }

		return ret;
    }

	virtual const char * receiveWindowsMessageBlob(const void *&msg,HeU32 &len)
    {
		const char *ret = 0;
        checkWinMsg();
        if ( !mBlobs.empty() )
        {
            mCurrent = mBlobs.front();
            mBlobs.pop();
            ret = mCurrent.mType;
            msg = mCurrent.mMessage;
            len = mCurrent.mLen;
        }
		return ret;
    }

    void checkWinMsg(void)
    {
#ifdef WIN32
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
#endif
    }

    virtual const char ** getArgs(const char *input,unsigned int &argc)  // parse string into a series of arguments.
    {
      strncpy(mParseBuffer,input,MAXPARSEBUFFER);
      mParseBuffer[MAXPARSEBUFFER-1] = 0;
      int ac;
      const char **ret = mParser.GetArglist(mParseBuffer,ac);
      argc = (unsigned int)ac;
      return ret;
    }

    virtual bool sendWinMsg(const char *app,const char *fmt,...)
    {
    	bool ret = false;

#ifdef WIN32
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
    		char buffer[8192];
      	    _vsnprintf(buffer, 8192, fmt, (char *)(&fmt+1));
    		HeU32 len = (HeU32)strlen(buffer);

            HeU32 olen;
            void *mem = getDataBlob("WinMsg","Text",buffer,len+1,olen);

            COPYDATASTRUCT copydata;
            copydata.dwData = 0;
            copydata.lpData = mem;
            copydata.cbData = olen;

            ::SendMessage(targetwindow, WM_COPYDATA, (WPARAM)0, (LPARAM)&copydata);

            MEMALLOC_FREE(mem);
    	}
#endif

      return ret;
    }



    virtual bool sendWinMsgBinary(const char *app,const char *blobType,const void *mem,HeU32 len)
    {
    	bool ret = false;

#ifdef WIN32
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

            HeU32 olen;
            void *omem = getDataBlob("WinMsgBlob",blobType,mem,len,olen);

             COPYDATASTRUCT copydata;

             copydata.dwData = 0;
             copydata.lpData = (PVOID)mem;
             copydata.cbData = len;

             ::SendMessage(targetwindow, WM_COPYDATA, (WPARAM)0, (LPARAM)&copydata);

             MEMALLOC_FREE(omem);
    	}
#endif

      return ret;
    }

    virtual bool hasWindow(const char *app) // returns true if this application window exists on the system.
    {
    	bool ret = false;

#ifdef WIN32

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
#endif

      return ret;
    }

    void queueMessageBlob(const char *blobType,const void *mem,HeU32 len)
    {
        WindowsMessage wm(blobType,mem,len);
        mBlobs.push(wm);
    }

    void queueMessage(const char *msg)
    {
        WindowsMessage wm(msg);
        mStrings.push(wm);
    }

private:

#ifdef WIN32
    HWND mHwnd;
	char mParseBuffer[MAXPARSEBUFFER];
	InPlaceParser mParser;
    WindowsMessage      mCurrent;
    WindowsMessageQueue mStrings;
    WindowsMessageQueue mBlobs;
#endif

};

static INT_PTR CALLBACK _MsgWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  MyWinMsg *winmsg = (MyWinMsg*)LongToPtr(GetWindowLongPtr(hwnd, GWLP_USERDATA));

  if(winmsg)
  {
    switch(msg)
    {
      case WM_COPYDATA:
      {
        COPYDATASTRUCT *data = (COPYDATASTRUCT*) lparam;

        if(data && data->lpData)
        {
            const char *msg = (const char *)data->lpData;
            if ( strncmp(msg,"WinMsg",6) == 0 )
            {
                msg+=7;
                if ( strncmp(msg,"Blob",4) == 0 )
                {
                    msg+=5;
                    const char *blobType = msg;
                    while ( *msg )
                    {
                        msg++;
                    }
                    msg++;
                    const HeU32 *dlen = (const HeU32 *)msg;
                    msg+=sizeof(HeU32);
	                winmsg->queueMessageBlob(blobType,msg,dlen[0]);
                }
                else
                {
                    while ( *msg )
                    {
                        msg++;
                    }
                    msg++;
                    const HeU32 *dlen = (const HeU32 *)msg;
                    msg+=sizeof(HeU32);
	                winmsg->queueMessage(msg);
                }
            }
        }

        return 0;
        break;
      }
    }
  }

  return DefWindowProc(hwnd, msg, wparam, lparam);
}

}; // end of namespace

using namespace WINMSG;

WinMsg * createWinMsg(const char *windowName)  // can be null if this is being used only for send messages.
{
    MyWinMsg *mwm = MEMALLOC_NEW(MyWinMsg)(windowName);
    return static_cast< WinMsg * >(mwm);

}
void     releaseWinMsg(WinMsg *msg)
{
    MyWinMsg *mwm = static_cast< MyWinMsg *>(msg);
    MEMALLOC_DELETE(MyWinMsg,mwm);
}


