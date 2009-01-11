#ifndef WSTABLE_H

#define WSTABLE_H

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
#pragma warning(disable:4995)
#pragma warning(disable:4996)


#include <assert.h>
#include <string.h>
#include <string>
#include <set>
#include <wchar.h>

#include "common/snippets/UserMemAlloc.h"
#include "wstring.h"


class WCharPtrLess
{
public:
	bool operator()(const wchar_t *v1,const wchar_t *v2) const
	{
		HeI32 v = wstrcmp(v1,v2);
		if ( v < 0 ) return true;
		return false;
	};
};

#if HE_USE_MEMORY_TRACKING
typedef USER_STL::set< const wchar_t *, USER_STL::GlobalMemoryPool, WCharPtrLess > WCharPtrSet;
#else
typedef USER_STL::set< const wchar_t *, WCharPtrLess > WCharPtrSet;
#endif

class WStringTable
{
public:
	WStringTable(void)
	{
	};

	~WStringTable(void)
	{
		WCharPtrSet::iterator i;
		for (i=mStrings.begin(); i!=mStrings.end(); i++)
		{
			wchar_t *str = (wchar_t *)(*i);
			MEMALLOC_FREE(str);
		}
	}

	const wchar_t * Get(const wchar_t *str)
	{
		WCharPtrSet::iterator found;
		found = mStrings.find( str );
		if ( found != mStrings.end() ) return (*found);
		HeU32 l = (HeU32)wstrlen(str);
		wchar_t *mem = (wchar_t *) MEMALLOC_MALLOC(sizeof(wchar_t)*(l+1));
		wstrcpy(mem,str);
		mStrings.insert( mem );
		return mem;
	};

	const wchar_t * Get(const wchar_t *str,bool &first)
	{
		WCharPtrSet::iterator found;
		found = mStrings.find( str );
		if ( found != mStrings.end() )
		{
			first = false;
			return (*found);
		}
		first = true;
		HeU32 l = (HeU32)wstrlen(str);
		wchar_t *mem = (wchar_t *) MEMALLOC_MALLOC(sizeof(wchar_t)*(l+1));
		wstrcpy(mem,str);
		mStrings.insert( mem );
		return mem;
	};

	WCharPtrSet& GetSet(void) { return mStrings; };

private:
	WCharPtrSet mStrings;
};


class WCharPtrInt
{
public:
	const wchar_t *mString;
	HeI32         mId;
};

class WCharPtrIntLess1
{
public:
	bool operator()(const WCharPtrInt &v1,const WCharPtrInt &v2) const
	{
		HeI32 v = wstrcmp(v1.mString,v2.mString);
		if ( v < 0 ) return true;
		return false;
	};
};

class WCharPtrIntLess2
{
public:
	bool operator()(const WCharPtrInt &v1,const WCharPtrInt &v2) const
	{
		HeI32 v = wstricmp(v1.mString,v2.mString);
		if ( v < 0 ) return true;
		return false;
	};
};

#if HE_USE_MEMORY_TRACKING
typedef USER_STL::set< WCharPtrInt, USER_STL::GlobalMemoryPool, WCharPtrIntLess1 > WCharPtrIntSet1;
typedef USER_STL::set< WCharPtrInt, USER_STL::GlobalMemoryPool, WCharPtrIntLess2 > WCharPtrIntSet2;
#else
typedef USER_STL::set< WCharPtrInt, WCharPtrIntLess1 > WCharPtrIntSet1;
typedef USER_STL::set< WCharPtrInt, WCharPtrIntLess2 > WCharPtrIntSet2;

#endif

class WStringTableInt
{
public:
	WStringTableInt(void)
	{
		mCase = true;
	}

	HeI32 Get(const wchar_t *str) const
	{
		HeI32 ret = 0;
		if ( mCase )
		{
			WCharPtrIntSet1::const_iterator found;
			WCharPtrInt cpi;
			cpi.mString = str;
			cpi.mId     = 0;
			found = mStrings1.find( cpi );
			if ( found != mStrings1.end() )
				ret = (*found).mId;
		}
		else
		{
			WCharPtrIntSet2::const_iterator found;
			WCharPtrInt cpi;
			cpi.mString = str;
			cpi.mId     = 0;
			found = mStrings2.find( cpi );
			if ( found != mStrings2.end() )
				ret = (*found).mId;
		}
		return ret;
	};

	void Add(const wchar_t *foo,HeI32 id)
	{
		HE_ASSERT( id > 0 );
		WCharPtrInt cpi;
		cpi.mString = foo;
		cpi.mId     = id;
		if ( mCase )
			mStrings1.insert(cpi);
		else
			mStrings2.insert(cpi);
	}

	const wchar_t * Get(HeI32 id) const
	{
		const wchar_t *ret = 0;
		if ( mCase )
		{
			WCharPtrIntSet1::const_iterator i;
			for (i=mStrings1.begin(); i!=mStrings1.end(); ++i)
			{
				if ( (*i).mId == id )
				{
					ret = (*i).mString;
					break;
				}
			}
		}
		else
		{
			WCharPtrIntSet2::const_iterator i;
			for (i=mStrings2.begin(); i!=mStrings2.end(); ++i)
			{
				if ( (*i).mId == id )
				{
					ret = (*i).mString;
					break;
				}
			}
		}
		return ret;
	}

	void SetCaseSensitive(bool s)
	{
		mCase = s;
	}

private:
	bool        mCase;
	WCharPtrIntSet1 mStrings1;     // case sensitive
	WCharPtrIntSet2 mStrings2;     // case insensitive
};


#endif
