#ifndef CPARSER_H

#define CPARSER_H

/*!
**
** Copyright (c) 2009 by John W. Ratcliff mailto:jratcliff@infiniplex.net
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

#include "common/snippets/UserMemAlloc.h"
#include "inparser.h"       // include the in-place file parser
#include "stringdict.h"     // include the string dictionary
#include "timedevent.h"

#if defined(LINUX)
#define _vsnprintf vsnprintf
#endif

class TokenTag;

typedef USER_STL::map<StringRef, TokenTag> TokenMap;


//==================================================================================
//==================================================================================
class CommandParserInterface
{
	friend class TheCommandParser;
public:
	CommandParserInterface(void);
	virtual ~CommandParserInterface(void);

	virtual HeI32 CommandCallback(HeI32 token,HeI32 count,const char **arglist) = 0;

	virtual HeI32 CommandFallback(HeI32 /* count */,const char ** /* arglist */)
	{
		HeI32 ret = 0;
		return ret;
	}

  // gives us a chance to trap the command before it gets parsed.
  virtual bool firstChance(const char * /* buffer */)
  {
    bool ret = false;
    return ret;
  }

	void AddToken(const char *key,HeI32 token);
	HeI32 GetTokenFromKey(const char *key);
	const char *GetKeyFromToken( const char *prefix, HeI32 token );

private:
};


typedef USER_STL::vector< CommandParserInterface * > CommandParserInterfaceVector;

//==================================================================================
//==================================================================================
class TokenTag
{
public:
	TokenTag(void) { };
	TokenTag(HeI32 tlocal,CommandParserInterface *service)
	{
		mLocalToken = tlocal;
		mService = service;
	};
	HeI32 GetLocalToken(void) const { return mLocalToken; };
	CommandParserInterface * GetService(void) const { return mService; }
private:
	HeI32 mLocalToken; // JWR  local internal token for this command.
	CommandParserInterface *mService;
};

class RootCommands; // handles root commands.


//==================================================================================
//==================================================================================
class TheCommandParser : public InPlaceParserInterface, public TIMED_EVENT::TimedEventInterface
{
public:
	TheCommandParser(bool timedEventFactory=false);
	~TheCommandParser(void);

	HeI32 Parse(const char *fmt, ...);

	void AddToken(const char *key,HeI32 token,CommandParserInterface *service);
	HeI32 GetTokenFromKey(const char *key);
	const char *GetKeyFromToken( const char *prefix, HeI32 token );

	void RemoveToken(const char *key);

	void DeRegister(CommandParserInterface *service);

	HeI32 Batch(const char *fname);
  HeI32 Batch(const char *data,HeU32 len); // from a block of memory.

	virtual HeI32 ParseLine(HeI32 lineno,HeI32 argc,const char **argv);

	HeI32 CommandLine(HeI32 argc,const char **argv,bool fallbackok=true);

	void AddFallback(CommandParserInterface *iface)
	{
		mFallbacks.push_back(iface);
	}

	HeI32 GetLineNo(void) const { return mLineNo; };

	bool receiveMessage(const char *msg);

	void checkMessages(void);

  void process(HeF32 dtime); // give up a time slice to the command parser to process outstanding timed events.

	bool timedEventCallback(void *user_data,HeI32 user_id,bool alive); //true if supposed to continue, false if done.
  void deleteEventCallback(void *user_data,HeI32 user_id); //true if supposed to continue, false if done.

  bool preParse(const char *fmt,...);

  bool preParseLine(HeI32 /* lineno */,const char *line)
  {
    return preParse("%s",line);
  }


private:
	RootCommands   *mRoot;
	TokenMap        mTokens; // JWR  token id's organized based on ascii name.
	CommandParserInterfaceVector mFallbacks;
	HeI32 mLineNo;
  TIMED_EVENT::TimedEventFactory *mTimedEventFactory; // handles timed event calls
};

//==================================================================================
//==================================================================================
class CommandCapture
{
public:
	CommandCapture(HeI32 token,HeI32 count,const char **args);
	CommandCapture(const CommandCapture &a); // copy constructor to do a 'deep' copy.
	~CommandCapture(void);

	void Invoke(CommandParserInterface *iface); // spool it as a callback.
	const char ** GetArgs(void) const { return (const char **)mArgs; };
	HeU32 GetCount(void) const { return (HeU32) mCount; };

private:
	HeI32    mToken;
	HeI32    mCount;
	char **mArgs;
};

typedef USER_STL::vector< CommandCapture * > CommandCaptureVector;

extern TheCommandParser *gTheCommandParser;

#define PARSE(x) gTheCommandParser->Parse(x)
#define CPARSER (*gTheCommandParser)

#endif
