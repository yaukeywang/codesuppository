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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "common/snippets/UserMemAlloc.h"
#include "cparser.h"
#include "gauss.h"
#include "filesystem.h"



TheCommandParser *gTheCommandParser=0;

//==================================================================================
//==================================================================================
enum CommandParserCommand
{
	CPC_RUN,
};

//==================================================================================
//==================================================================================
class RootCommands : public CommandParserInterface
{
public:
	RootCommands(void)
	{
		AddToken("Run",CPC_RUN);
	}

	~RootCommands(void)
	{
	}

	virtual HeI32 CommandCallback(HeI32 token,HeI32 count,const char **arglist)
	{
		HeI32 ret = 0;

		switch ( token )
		{
			case CPC_RUN:
				if ( count >= 2 )
				{
					for (HeI32 i=1; i<count; i++)
					{
						HeI32 v = gTheCommandParser->Batch( arglist[1] );
						if ( v ) ret = v;
					}
				}
				break;
		}

		return ret;
	}

};

//==================================================================================
TheCommandParser::TheCommandParser(bool timedEventFactory)
{
	gTheCommandParser = this;

  if ( timedEventFactory )
    mTimedEventFactory = TIMED_EVENT::createTimedEventFactory();
  else
    mTimedEventFactory = false;

	mRoot = MEMALLOC_NEW(RootCommands);
	mLineNo = 0;
}

//==================================================================================
TheCommandParser::~TheCommandParser(void)
{
	delete mRoot;
  if ( mTimedEventFactory )
    TIMED_EVENT::releaseTimedEventFactory(mTimedEventFactory);
}

//==================================================================================
void TheCommandParser::AddToken(const char *key, HeI32 token, CommandParserInterface *service)
{
	StringRef rkey = SGET(key);
	TokenTag ttype(token,service);

	TokenMap::iterator found;
	found = mTokens.find( rkey );
	if ( found != mTokens.end() )
	{
		printf("Attempted to add the same token (%s) twice!!\n",key);
		HE_ASSERT( 0 );
	}
	else
		mTokens[ rkey ] = ttype;
}

//==================================================================================
HeI32 TheCommandParser::GetTokenFromKey( const char *key )
{
	HeI32 ret = -1;

	StringRef rkey = SGET(key);
	TokenMap::iterator found;
	found = mTokens.find( rkey );
	if ( found != mTokens.end() )
	{
		TokenTag ttype = (*found).second;
		ret = ttype.GetLocalToken();
	}

	return ret;
}

//==================================================================================
const char *TheCommandParser::GetKeyFromToken( const char *prefix, HeI32 token )
{
	const char *ret = 0;

	if ( prefix )
	{
		HeI32 prefixLen = (HeI32)strlen( prefix );

		TokenMap::iterator i;
		for ( i = mTokens.begin(); !ret && (i != mTokens.end()); ++i )
		{
			TokenTag ttype = (*i).second;
			if ( ttype.GetLocalToken() == token )
			{
				const char *value = (*i).first;
				if ( value && !strncmp( value, prefix, prefixLen ) )
				{
					ret = value;
				}
			}
		}
	}

	return ret;
}

//==================================================================================
void TheCommandParser::RemoveToken(const char *key)  // JWR  remove a token we don't need anymore.
{
	StringRef rkey = SGET(key);
	TokenMap::iterator found;
	found = mTokens.find( rkey );
	if ( found != mTokens.end() )
	{
		mTokens.erase( found );
	}
	else
	{
		HE_ASSERT( 0 );
		printf("Attempted to remove token %s that didn't exist!\n",key);
	}
}

bool TheCommandParser::preParse(const char *fmt,...)
{
  bool ret = false;

	char buff[8192];
  buff[8191] = 0;
	_vsnprintf(buff, 8191, fmt, (char *)(&fmt+1));
  if ( mTimedEventFactory && strncmp(buff,"/event ",7) == 0 )
  {
    InPlaceParser ipp;
    ipp.DefaultSymbols();
    HeI32 count;
    const char **alist = ipp.GetArglist(buff,count);

    if ( alist && count >= 3 )
    {
      Gauss duetime(alist[1]);

      HeI32 scan = 2;

      Gauss repeat_count(0.0f);
      Gauss repeat_time(0.0f);

      if ( stricmp(alist[2],"repeat") == 0 && count >= 5 )
      {
        repeat_count.Set( alist[3] );
        repeat_time.Set(alist[4]);
        scan = 5;
      }

      char temp[2048];
      temp[0] = 0;
      for (HeI32 i=scan; i<count; i++)
      {
        strcat(temp,alist[i]);
        if ( (i+1) != count )
        {
          strcat(temp," ");
        }
      }
      HeI32 len = strlen(temp);
      if ( len )
      {
        char *cmd = (char *)MEMALLOC_MALLOC(len+1);
        strcpy(cmd,temp);
        TIMED_EVENT::postTimedEvent(mTimedEventFactory,this,duetime,repeat_count,repeat_time,cmd,0); // post this as a timed event command!
      }
      ret = true;
    }
  }
  else if ( strncmp(buff,"@echo",5) == 0 )
  {
    printf("%s\r\n", buff ); // purely for debugging purposes.
    ret = true;
  }
  else
  {
    CommandParserInterfaceVector::iterator i;
    for (i=mFallbacks.begin(); i!=mFallbacks.end(); ++i)
    {
      ret = (*i)->firstChance(buff);
      if ( ret ) break;
    }
  }
  return ret;
}

//==================================================================================
HeI32 TheCommandParser::Parse(const char *fmt, ...)
{

 	HeI32 ret = 0;

	char buff[8192];
  buff[8191] = 0;
	_vsnprintf(buff, 8191, fmt, (char *)(&fmt+1));

  bool processed = preParse("%s",buff);

  if ( !processed )
  {
  	HeU32 len = (HeU32)strlen(buff);
  	if ( len )
  	{
  		InPlaceParser ipp(buff,len);
  		ipp.DefaultSymbols();
  		HeI32 v = ipp.Parse(this);
  		if ( v ) ret = v;
  	}
  }

	return ret;
}

//==================================================================================
HeI32 TheCommandParser::ParseLine(HeI32 lineno,HeI32 argc,const char **argv)
{
	mLineNo = lineno;
	return CommandLine(argc,argv,true);
}

//==================================================================================
void TheCommandParser::DeRegister(CommandParserInterface *cinterface)
{
  static bool state = true;
	while ( state )
	{
		TokenMap::iterator i;
		for (i=mTokens.begin(); i!=mTokens.end(); ++i)
		{
			CommandParserInterface *service = (*i).second.GetService();
			if ( service == cinterface ) break;
		}
		if ( i == mTokens.end() ) break;
		mTokens.erase(i);
	}
}

//==================================================================================
HeI32 TheCommandParser::Batch(const char *fname)
{
	HeI32 ok = -1;

	const char *name = fname;
	InPlaceParser ipp(name,gFileSystem);
	ipp.DefaultSymbols();

	if ( ipp.HasData() )
	{
		ok = ipp.Parse(this);
	}

	return ok;
}

//==================================================================================
HeI32 TheCommandParser::CommandLine(HeI32 argc,const char **argv,bool fallbackok)
{
	HeI32 ret = 0;

	if ( argc )
	{
		TokenMap::iterator found;
		StringRef ref = SGET(argv[0]);
		found = mTokens.find( ref );

		if ( found == mTokens.end() )
		{
			// do case - insenstive search
			for (found=mTokens.begin(); found!=mTokens.end(); ++found)
			{
				if ( stricmp( (*found).first.Get(), ref.Get() ) == 0 ) break;
			}
		}

		if ( found != mTokens.end() )
		{
			TokenTag ttype = (*found).second;

			CommandParserInterface *cmd = ttype.GetService();

			HeI32 v = cmd->CommandCallback( ttype.GetLocalToken(), argc, argv );

			if ( v ) 
				ret = v;
		}
		else
		{
			if ( mFallbacks.empty() || !fallbackok )
			{
			}
			else
			{
				CommandParserInterfaceVector::iterator i;

				for (i=mFallbacks.begin(); i!=mFallbacks.end(); ++i)
				{
					CommandParserInterface *iface = (*i);
					HeI32 v = iface->CommandFallback(argc,argv);
					if ( v ) ret = v;
				}
			}
		}
	}

	return ret;
}

//==================================================================================
bool TheCommandParser::receiveMessage(const char *msg)
{
  Parse(msg);
  return true;
}

//==================================================================================
void TheCommandParser::checkMessages(void)
{
}

//==================================================================================
void CommandParserInterface::AddToken(const char *key,HeI32 token)
{
	gTheCommandParser->AddToken(key,token,this);
}

//==================================================================================
HeI32 CommandParserInterface::GetTokenFromKey(const char *key)
{
	return gTheCommandParser->GetTokenFromKey( key );
}

//==================================================================================
const char *CommandParserInterface::GetKeyFromToken( const char *prefix, HeI32 token )
{
	return gTheCommandParser->GetKeyFromToken( prefix, token );
}

//==================================================================================
CommandParserInterface::CommandParserInterface(void)
{
}

//==================================================================================
CommandParserInterface::~CommandParserInterface(void)
{
//	gTheCommandParser->DeRegister(this);
}

//==================================================================================
CommandCapture::CommandCapture(HeI32 token,HeI32 count,const char **args)
{
	mToken = token;
	mCount = count;
	mArgs = (char **) MEMALLOC_MALLOC(sizeof(char*)*mCount);
	for (HeI32 i=0; i<count; i++)
	{
		const char *foo = args[i];
		HeU32 len = (HeU32)strlen(foo);
		mArgs[i] = (char *) MEMALLOC_MALLOC(len+1);
		strcpy(mArgs[i],foo);
	}
}

//==================================================================================
CommandCapture::CommandCapture(const CommandCapture &a) // copy constructor to do a 'deep' copy.
{
	mToken = a.mToken;
	mCount = a.mCount;
	mArgs = (char **) MEMALLOC_MALLOC(sizeof(char*)*mCount);
	for (HeI32 i=0; i<mCount; i++)
	{
		const char *foo = a.mArgs[i];
		HeU32 len = (HeU32)strlen(foo);
		mArgs[i] = (char *) MEMALLOC_MALLOC(len+1);
		strcpy(mArgs[i],foo);
	}
}

//==================================================================================
CommandCapture::~CommandCapture(void)
{
	if ( mArgs )
	{
		for (HeI32 i=0; i<mCount; i++)
		{
			char *mem = mArgs[i];
			MEMALLOC_FREE(mem);
		}
		MEMALLOC_FREE(mArgs);
	}
}

//==================================================================================
void CommandCapture::Invoke(CommandParserInterface *iface) // spool it as a callback.
{
	iface->CommandCallback(mToken,mCount,(const char **)mArgs);
}


HeI32 TheCommandParser::Batch(const char *data,HeU32 len) // from a block of memory.
{
	HeI32 ok = -1;

  char *temp = (char *) MEMALLOC_MALLOC(len);
  memcpy(temp,data,len);

	InPlaceParser ipp(temp,len);
	ipp.DefaultSymbols();

	if ( ipp.HasData() )
	{
		ok = ipp.Parse(this);
	}

  MEMALLOC_FREE(temp);

	return ok;
}


void TheCommandParser::process(HeF32 dtime) // give up a time slice to the command parser to process outstanding timed events.
{
  TIMED_EVENT::process(mTimedEventFactory,dtime);
}

void TheCommandParser::deleteEventCallback(void *user_data,HeI32 /* user_id */) //true if supposed to continue, false if done.
{
  char *cmd = (char *) user_data;
  MEMALLOC_FREE(cmd);
}

bool TheCommandParser::timedEventCallback(void *user_data,HeI32 /* user_id */,bool /* alive */) //true if supposed to continue, false if done.
{
  bool ret = true;

  char *cmd = (char *) user_data;
  CPARSER.Parse(cmd);

  return ret;
}
