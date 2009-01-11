#ifndef WSTRINGDICT_H
#define WSTRINGDICT_H

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




#pragma warning(disable:4786)

#include <vector>
#include <map>
#include <set>

#include "common/snippets/UserMemAlloc.h"
#include "wstable.h"
#include "wstring.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <wchar.h>



extern const wchar_t *wemptystring;

class WStringRef
{
public:
	WStringRef(void)
	{
		mString = L"";
	}

	inline WStringRef(const wchar_t *str);
	inline WStringRef(const WStringRef &str);

	operator const wchar_t *() const
	{
		return mString;
	}

	const wchar_t * Get(void) const { return mString; };

	void Set(const wchar_t *str)
	{
		mString = str;
	}

	const WStringRef &operator= (const WStringRef& rhs )
	{
		mString = rhs.Get();
		return *this;
	}

	bool operator== ( const WStringRef& rhs ) const
	{
		return rhs.mString == mString;
	}

	bool operator< ( const WStringRef& rhs ) const
	{
		return rhs.mString < mString;
	}

	bool operator!= ( const WStringRef& rhs ) const
	{
		return rhs.mString != mString;
	}

	bool operator> ( const WStringRef& rhs ) const
	{
		return rhs.mString > mString;
	}

	bool operator<= ( const WStringRef& rhs ) const
	{
		return rhs.mString < mString;
	}

	bool operator>= ( const WStringRef& rhs ) const
	{
		return rhs.mString >= mString;
	}

	bool SamePrefix(const wchar_t *prefix) const
	{
		HeU32 len = (HeU32)wstrlen(prefix);
		if ( len && wstrncmp(mString,prefix,len) == 0 ) return true;
		return false;
	}

	bool SameSuffix(const WStringRef &suf) const
	{
		const wchar_t *source = mString;
		const wchar_t *suffix = suf.mString;
		HeU32 len1 = (HeU32)wstrlen(source);
		HeU32 len2 = (HeU32)wstrlen(suffix);
		if ( len1 < len2 ) return false;
		const wchar_t *compare = &source[(len1-len2)];
		if ( wstrcmp(compare,suffix) == 0 ) return true;
		return false;
	}

private:
	const wchar_t *mString; // the actual char ptr
};


class WStringDict
{
public:

	WStringDict(void)
	{
	}

	~WStringDict(void)
	{
	}

  WStringRef Get(const char *text)
  {
    wchar_t *temp = 0;
    if ( text )
    {
      size_t len = strlen(text);
      temp = MEMALLOC_NEW_ARRAY(wchar_t,len+1)[len+1];
      mbstowcs( temp, text, len+1 );
    }
    WStringRef ret = Get(temp);
    delete []temp;
    return ret;
  }

	WStringRef Get(const wchar_t *text)
	{
		WStringRef ref;
		if ( text )
		{
			if ( wstrcmp(text,wemptystring) == 0 )
			{
				ref.Set(wemptystring);
			}
			else
			{
				if ( wstrcmp(text,wemptystring) == 0 )
				{
					ref.Set(wemptystring);
				}
				else
				{
					const wchar_t *foo = mStringTable.Get(text);
					ref.Set(foo);
				}
			}
		}
		return ref;
	}

	WStringRef Get(const wchar_t *text,bool &first)
	{
		HE_ASSERT(text); // no null string support for this version!
		WStringRef ref;
		const wchar_t *foo = mStringTable.Get(text,first);
		ref.Set(foo);
		return ref;
	}

private:
	WStringTable mStringTable;
};

extern WStringDict *gWStringDict;

typedef USER_STL::vector< WStringRef  >    WStringRefVector;
typedef USER_STL::set< WStringRef, USER_STL::GlobalMemoryPool >        WStringRefSet;

inline WStringRef WSGET(const wchar_t *foo)
{
	if ( !gWStringDict )
	{
		gWStringDict = MEMALLOC_NEW(WStringDict);
	}
	return gWStringDict->Get(foo);
}

inline WStringRef WSGET(const char *foo)
{
	if ( !gWStringDict )
	{
		gWStringDict = MEMALLOC_NEW(WStringDict);
	}
	return gWStringDict->Get(foo);
}

inline WStringRef::WStringRef(const wchar_t *str)
{
	WStringRef ref = WSGET(str);
	mString = ref.mString;
}

inline WStringRef::WStringRef(const WStringRef &str)
{
	mString = str.Get();
}

// This is a helper class so you can easily do an alphabetical sort on an STL vector of StringRefs.
// Usage: std::sort( list.begin(), list.end(), StringSortRef() );
class WStringSortRef
{
	public:

	 bool operator()(const WStringRef &a,const WStringRef &b) const
	 {
		 const wchar_t *str1 = a.Get();
		 const wchar_t *str2 = b.Get();
		 HeI32 r = wstrcmp(str1,str2);
		 return r < 0;
	 }
};

#endif
