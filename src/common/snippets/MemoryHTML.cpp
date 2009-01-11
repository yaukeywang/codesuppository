#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>

#pragma warning(disable:4996)

#include "MemoryHTML.h"
#include "common/snippets/inparser.h"
#include "bitmap.h"

#include <vector>

#define BITMAP_SIZE 1000
#define BITMAP_PIXELS (BITMAP_SIZE*BITMAP_SIZE)

static HeI32 BLOCK_SIZE=5;
static HeI32 BLOCK_WIDTH=(BITMAP_SIZE/5);
static HeI32 BLOCK_PIXELS (5*5);
static HeI32 BLOCK_COUNT (BITMAP_PIXELS/(5*5));

#define MAXNUMERIC 32  // JWR  support up to 16 32 character long numeric formated strings
#define MAXFNUM    16

static	char  gFormat[MAXNUMERIC*MAXFNUM];
static HeI32    gIndex=0;


static const char * formatNumber(HeI32 number) // JWR  format this integer into a fancy comma delimited string
{
	char * dest = &gFormat[gIndex*MAXNUMERIC];
	gIndex++;
	if ( gIndex == MAXFNUM ) gIndex = 0;

	char scratch[512];

#if defined (LINUX_GENERIC) || defined(LINUX)
	snprintf(scratch, 10, "%d", number);
#else
	itoa(number,scratch,10);
#endif

	char *str = dest;
	HeU32 len = (HeU32)strlen(scratch);
	for (HeU32 i=0; i<len; i++)
	{
		HeI32 place = (len-1)-i;
		*str++ = scratch[i];
		if ( place && (place%3) == 0 ) *str++ = ',';
	}
	*str = 0;

	return dest;
}


static inline char ToLower(char c)
{
  if ( c >= 'A' && c <= 'F' ) c+=32;
  return c;
}

static inline HeU32 GetHex(HeU8 c)
{
	HeU32 v = 0;
	c = ToLower(c);
	if ( c >= '0' && c <= '9' )
		v = c-'0';
	else
	{
		if ( c >= 'a' && c <= 'f' )
		{
			v = 10 + c-'a';
		}
	}
	return v;
}


static inline HeU32 GetHEX4(const char *foo,const char **endptr)
{
	HeU32 ret = 0;

	for (HeI32 i=0; i<8; i++)
	{
		ret = (ret<<4) | GetHex(foo[i]);
	}

	if ( endptr )
	{
		*endptr = foo+8;
	}

	return ret;
}

static inline HeU32 getHex(const char *c)
{
  HeU32 ret = 0;

  if ( c[0] == '0' && c[1] == 'x' ) c+=2;

  ret = GetHEX4(c,0);

  return ret;
}

static HeI32 inline getInt(const char *c)
{
  HeI32 ret = 0;

  char num[512];
  char *dest = num;
  while ( *c )
  {
    if ( c[0] != ',' )
      *dest++ = c[0];
    c++;
  }
  *dest = 0;

  ret = atoi(num);

  return ret;
}

class MemoryItem
{
public:

  MemoryItem(const char **argv)
  {
    //   0   1 2  3 4 5 6   7 8 9 10 11 12
    // adress,size,crc,flags,tag,file,lineno
    mAddress = getHex(argv[0]);
    mSize    = getInt(argv[2]);
    mCRC     = argv[4];
    mFlags   = argv[6];
    mTag     = argv[8];
    mFile    = argv[10];
    mLineNo  = argv[12];
  }

  HeU32 computeIndex(HeU32 m,HeU32 minAddress,HeU32 maxAddress)
  {
    HE_ASSERT( m >= minAddress && m <= maxAddress );
    m-=minAddress;
//    HeU32 diff = (maxAddress-minAddress);
    HeU64 a = m;
    HeU64 b = (maxAddress-minAddress);
    HeU64 c = (a*BLOCK_COUNT)/b;
    return (HeU32) c;
  }

  void graph(BitMap256 &b,HeU32 minAddress,HeU32 maxAddress,HeU32 mlow,HeU32 mhigh)
  {
    HeU32 from = computeIndex(mAddress,minAddress,maxAddress);
    HeU32 to   = computeIndex(mAddress+mSize,minAddress,maxAddress);

    for (HeU32 i=from; i<=to; i++)
    {
      HeI32 x = i%BLOCK_WIDTH;
      HeI32 y = i/BLOCK_WIDTH;
      plotBlock(b,x,y,i==from,i==to,mlow,mhigh);
    }
  }

  void graph(MemoryRender &b,HeU32 minAddress,HeU32 maxAddress,HeU32 mlow,HeU32 mhigh,const char *section)
  {
    HeU32 from = computeIndex(mAddress,minAddress,maxAddress);
    HeU32 to   = computeIndex(mAddress+mSize,minAddress,maxAddress);

    for (HeU32 i=from; i<=to; i++)
    {
      HeI32 x = i%BLOCK_WIDTH;
      HeI32 y = i/BLOCK_WIDTH;
      plotBlock(b,x,y,i==from,i==to,mlow,mhigh,section);
    }
  }

  void plotBlock(BitMap256 &b,HeI32 x,HeI32 y,bool openBlock,bool closeBlock,HeU32 /*mlow*/,HeU32 /*mhigh*/)
  {
    HeU8 color1 = b.getNearestColor(0xFFFFFF);
    HeU8 color2 = b.getNearestColor(0x808080);

    HeI32 x1 = x*BLOCK_SIZE;
    HeI32 y1 = y*BLOCK_SIZE;

    HeI32 x2 = x1+BLOCK_SIZE;
    HeI32 y2 = y1+BLOCK_SIZE;

    for (HeI32 y=y1; y<=y2; y++)
    {
      for (HeI32 x=x1; x<=x2; x++)
      {
        b.plotPixelSafe(x,y,color2);
      }
    }

    for (HeI32 x=x1; x<=x2; x++)
    {
      b.plotPixelSafe(x,y1,color1);
      b.plotPixelSafe(x,y2,color1);
    }

    if ( openBlock )
    {
      for (HeI32 y=y1; y<=y2; y++)
      {
        b.plotPixelSafe(x1,y,color1);
      }
    }

    if ( closeBlock )
    {
      for (HeI32 y=y1; y<=y2; y++)
      {
        b.plotPixelSafe(x2,y,color1);
      }
    }


  }

  void plotBlock(MemoryRender &b,HeI32 x,HeI32 y,bool openBlock,bool closeBlock,HeU32 mlow,HeU32 mhigh,const char *section)
  {
    HeU32 color1 = 0xFFFFFF;
    HeU32 color2 = 0x808080;

    if ( mSize <= 64 )
    {
      color2 = 0xFF0000;
    }


    HeI32 x1 = x*BLOCK_SIZE;
    HeI32 y1 = y*BLOCK_SIZE;

    if ( mSize < mlow || mSize >= mhigh )
    {
      color2 = 0x00;
    }
    else
    {
      b.plotBlock(mAddress,x1,y1,BLOCK_SIZE,BLOCK_SIZE,openBlock,section,mTag,mFile,mLineNo,mSize);
    }

    HeI32 x2 = x1+BLOCK_SIZE;
    HeI32 y2 = y1+BLOCK_SIZE;

    for (HeI32 y=y1; y<=y2; y++)
    {
      for (HeI32 x=x1; x<=x2; x++)
      {
        b.plotPixel(x,y,color2);
      }
    }

    for (HeI32 x=x1; x<=x2; x++)
    {
      b.plotPixel(x,y1,color1);
      b.plotPixel(x,y2,color1);
    }

    if ( openBlock )
    {
      for (HeI32 y=y1; y<=y2; y++)
      {
        b.plotPixel(x1,y,color1);
      }
    }

    if ( closeBlock )
    {
      for (HeI32 y=y1; y<=y2; y++)
      {
        b.plotPixel(x2,y,color1);
      }
    }


  }

  HeU32 mAddress;
  HeU32 mSize;
  const char  *mFlags;
  const char  *mCRC;
  const char  *mTag;
  const char  *mFile;
  const char  *mLineNo;
};

typedef USER_STL::vector< MemoryItem > MemoryItemVector;

typedef USER_STL::vector< HeU32 > UintVector;

class Power2
{
public:

  void init(HeU32 low,HeU32 high)
  {
    mTotal = 0;
    mLow = low;
    mHigh = high;
  }

  bool add(HeU32 v)
  {
    bool ret = false;
    if ( v >= mLow && v < mHigh )
    {
      ret = true;
      mTotal+=v;
      mSamples.push_back(v);
    }
    return ret;
  }

  void report(MemoryRender *callback)
  {
    if ( !mSamples.empty() )
    {
      HeF32 count = (HeF32)mSamples.size();

      HeF32 mean = (HeF32)mTotal / count;
      HeF32 diff = 0;
      for (HeU32 i=0; i<mSamples.size(); i++)
      {
        HeF32 v = (HeF32)mSamples[i];
        HeF32 d = mean-v;
        d = d*d;
        diff+=d;
      }
      HeF32 stdev = 0;

      if ( count >= 2 )
      {
        diff = diff / (count-1);
        stdev = sqrtf(diff);
      }
      callback->memoryMessage("Range: %12s - %12s  Count: %8s Total: %12s Mean: %10s STDEV: %0.4f\r\n", formatNumber(mLow),formatNumber(mHigh-1),formatNumber(mSamples.size()),formatNumber(mTotal),formatNumber( (HeI32)mean), stdev );
    }
  }

  HeU32 mTotal;
  HeU32 mLow;
  HeU32 mHigh;
  UintVector   mSamples;
};

#define MAX_POWER2 30


class MemorySection
{
public:
  MemorySection(const char *section)
  {
    if ( section[0] == '[' )
      section++;
    char *scan = (char *)section;
    while ( *scan )
    {
      if ( scan[0] == ']' )
      {
        scan[0] = 0;
        break;
      }
      else
        scan++;
    }
    mSection = section;
    mMinAddress = 0;
    mMaxAddress = 0;
    mSectionSize = 0;
    mMemCount = 0;
    mTotalMemory = 0;
    mMean = 0;
    mStandardDeviation = 0;
  }

  void addItem(HeI32 argc,const char **argv,HeU32 &itemCount,HeU32 &itemSize)
  {
    if( argc == 13 )
    {

      const char *adr = argv[0];

      if ( adr[0] == '0' && adr[1] == 'x' )
      {

        MemoryItem m(argv);

        mMemCount++;
        mTotalMemory+=m.mSize;

        if ( mItems.empty() )
        {
          mMinAddress = m.mAddress;
          mMaxAddress = m.mAddress+m.mSize;
        }
        else
        {
          if ( m.mAddress < mMinAddress ) mMinAddress = m.mAddress;
          if ( (m.mAddress+m.mSize) > mMaxAddress ) mMaxAddress = (m.mAddress+m.mSize);
        }

        itemCount++;
        mSectionSize+=m.mSize;
        itemSize+=m.mSize;

        mItems.push_back(m);
      }
    }
  }

  void generateHTML(void)
  {
    printf("Section: %s has an address range of: %08X to %08X covering %s bytes.  Contains %s items totallying %s bytes.\r\n", mSection, mMinAddress, mMaxAddress, formatNumber(mMaxAddress-mMinAddress), formatNumber(mItems.size()), formatNumber(mSectionSize) );
  }

  void generateGraph(BitMap256 &bm,HeU32 minAddress,HeU32 maxAddress,HeU32 mlow,HeU32 mhigh)
  {
    MemoryItemVector::iterator i;
    for (i=mItems.begin(); i!=mItems.end(); ++i)
    {
      (*i).graph(bm,minAddress,maxAddress,mlow,mhigh);
    }
  }

  void generateGraph(MemoryRender &bm,HeU32 minAddress,HeU32 maxAddress,HeU32 mlow,HeU32 mhigh)
  {
    MemoryItemVector::iterator i;
    for (i=mItems.begin(); i!=mItems.end(); ++i)
    {
      (*i).graph(bm,minAddress,maxAddress,mlow,mhigh,mSection);
    }
  }

  void enumerate(MemoryRender *callback)
  {

    mMean = mTotalMemory / mMemCount;

    HeF32 fmean = (HeF32)mMean;

    HeF32 dtotal = 0;
    for (HeU32 i=0; i<mItems.size(); i++)
    {
      MemoryItem &mi = mItems[i];
      HeF32 diff = (HeF32)mi.mSize - fmean;
      dtotal+=(diff*diff);
    }

    HeF32 stdev = 0;
    HeI32 tcount = mItems.size();
    if ( tcount >= 2 )
    {
      stdev = dtotal /(HeF32)(tcount-1);
    }

    mStandardDeviation = sqrtf( stdev );


    callback->memorySection(mSection,mMinAddress,mMaxAddress,mMemCount,mTotalMemory,mMean,mStandardDeviation);
  }


  void buildPower2(Power2 *p)
  {
    MemoryItemVector::iterator i;
    for (i=mItems.begin(); i!=mItems.end(); ++i)
    {
      MemoryItem &mi = (*i);
      for (HeU32 i=0; i<MAX_POWER2; i++)
      {
        if ( p[i].add( mi.mSize ) )
        {
          break;
        }
      }
    }
  }


  HeU32          mMemCount;
  HeU32          mTotalMemory;
  HeU32          mMean;
  HeF32                 mStandardDeviation;
  HeU32          mMinAddress;
  HeU32          mMaxAddress;
  HeU32          mSectionSize;
  const char           *mSection;
  MemoryItemVector     mItems;
};

typedef USER_STL::vector< MemorySection * > MemorySectionVector;

class MemoryHTML : public InPlaceParserInterface
{
public:

  MemoryHTML(const char *fname)
  {
    mCurrent = 0;
    mFirst = true;
    mItemCount = 0;
    mItemSize  = 0;

    mMinAddress = 0;
    mMaxAddress = 0;
    mParser.SetFile(fname,0);
    mParser.SetHardSeparator(',');
    mParser.Parse(this);
    updateMinMax();

  }

  ~MemoryHTML(void)
  {
    MemorySectionVector::iterator i;
    for (i=mSections.begin(); i!=mSections.end(); ++i)
    {
      MemorySection *ms = (*i);
      delete ms;
    }

  }

  void updateMinMax(void)
  {
    if ( mCurrent && !mCurrent->mItems.empty() )
    {
      if ( mFirst )
      {
        mMinAddress = mCurrent->mMinAddress;
        mMaxAddress = mCurrent->mMaxAddress;
        mFirst = false;
      }
      else
      {
        if ( mCurrent->mMinAddress < mMinAddress )
          mMinAddress = mCurrent->mMinAddress;
        if ( mCurrent->mMaxAddress > mMaxAddress )
          mMaxAddress = mCurrent->mMaxAddress;
      }
    }
  }

	HeI32 ParseLine(HeI32 /*lineno*/,HeI32 argc,const char **argv)
  {
    HeI32 ret = 0;

    if ( argc )
    {
      const char *c = argv[0];

      if ( c[0] == '[' )
      {
        updateMinMax();
        mCurrent = MEMALLOC_NEW(MemorySection)(argv[0]);
        mSections.push_back(mCurrent);
      }
      else if ( mCurrent )
      {
        mCurrent->addItem(argc,argv,mItemCount,mItemSize);
      }
    }

    return ret;
  }

  bool generateHTML(const char * /*htmlName*/)
  {
    bool ret = false;

    printf("Address range: %08X to %08X covering %s bytes.  Contains: %s sections with %s items for a total of %s bytes.\r\n", mMinAddress, mMaxAddress, formatNumber(mMaxAddress-mMinAddress), formatNumber(mSections.size()), formatNumber(mItemCount), formatNumber(mItemSize) );


    BitMap256 bm(BITMAP_SIZE,BITMAP_SIZE);

//    HeU8 black      = 0;
    HeU8 white      = bm.getNearestColor(0xFFFFFF);
//    HeU8 light_gray = bm.getNearestColor(0xA0A0A0);
//    HeU8 gray       = bm.getNearestColor(0x808080);
    HeU8 dark_gray  = bm.getNearestColor(0x404040);

    for (HeI32 y=0; y<1000; y++)
    {
      for (HeI32 x=0; x<1000; x++)
      {
        HeU8 color = dark_gray;
        if ( x == 0 || y == 0 || x == 999 || y == 999 ) color = white;
        bm.plotPixel(x,y,color);
      }
    }

    MemorySectionVector::iterator i;
    for (i=mSections.begin(); i!=mSections.end(); i++)
    {
      MemorySection *ms = (*i);
      ms->generateGraph(bm,mMinAddress,mMaxAddress,0,0x7FFFFFFF);
    }


    bm.saveGIF("coredump.gif");


    return ret;
  }

  bool render(MemoryRender *callback,const char *section,HeU32 mlow,HeU32 mhigh)
  {
    bool ret = false;

//    printf("Address range: %08X to %08X covering %s bytes.  Contains: %s sections with %s items for a total of %s bytes.\r\n", mMinAddress, mMaxAddress, formatNumber(mMaxAddress-mMinAddress), formatNumber(mSections.size()), formatNumber(mItemCount), formatNumber(mItemSize) );


    for (HeI32 y=0; y<1000; y++)
    {
      for (HeI32 x=0; x<1000; x++)
      {
        HeU32 color = 0x404040;
        if ( x == 0 || y == 0 || x == 999 || y == 999 ) color = 0xFFFFFF;
        callback->plotPixel(x,y,color);
      }
    }

    if ( section )
    {
      MemorySectionVector::iterator i;
      for (i=mSections.begin(); i!=mSections.end(); i++)
      {
        MemorySection *ms = (*i);
        if ( strcmp(ms->mSection,section) == 0 )
        {
          ms->generateGraph(*callback,ms->mMinAddress,ms->mMaxAddress,mlow,mhigh);
          break;
        }
      }
    }
    else
    {
      MemorySectionVector::iterator i;
      for (i=mSections.begin(); i!=mSections.end(); i++)
      {
        MemorySection *ms = (*i);
        ms->generateGraph(*callback,mMinAddress,mMaxAddress,mlow,mhigh);
      }
    }


    return ret;
  }

  HeU32 enumerateMemorySections(MemoryRender *callback)
  {
    HeU32 ret = mSections.size();
    for (HeU32 i=0; i<mSections.size(); i++)
    {
      MemorySection &ms = *mSections[i];
      ms.enumerate(callback);
    }
    return ret;
  }

  void power2(const char *section,MemoryRender *callback)
  {

    Power2 p[MAX_POWER2];

    p[0].init(0,1);
    for (HeI32 i=1; i<MAX_POWER2; i++)
    {
      HeU32 low = 1<<i;
      HeU32 high = 1<<(i+1);
      p[i].init(low,high);
    }

    if ( section )
    {
      MemorySectionVector::iterator i;
      for (i=mSections.begin(); i!=mSections.end(); i++)
      {
        MemorySection *ms = (*i);
        if ( strcmp(ms->mSection,section) == 0 )
        {
          ms->buildPower2(p);
          break;
        }
      }
    }
    else
    {
      MemorySectionVector::iterator i;
      for (i=mSections.begin(); i!=mSections.end(); i++)
      {
        MemorySection *ms = (*i);
        ms->buildPower2(p);
      }
    }

    for (HeU32 i=0; i<MAX_POWER2; i++)
    {
      p[i].report(callback);
    }

  }

private:
  InPlaceParser         mParser;
  MemorySection        *mCurrent;

  MemorySectionVector   mSections;

  bool                  mFirst;
  HeU32          mItemCount;
  HeU32          mItemSize;

  HeU32          mMinAddress;
  HeU32          mMaxAddress;

};


MemoryHTML * createMemoryHTML(const char *fname)
{
  MemoryHTML *ret = MEMALLOC_NEW(MemoryHTML)(fname);
  return ret;
}

void releaseMemoryHTML(MemoryHTML *m)
{
  delete m;
}

bool generateHTML(MemoryHTML *m,const char *htmlName)
{
  return m->generateHTML(htmlName);
}

bool         memoryRender(MemoryHTML *m,MemoryRender *callback,const char *section,HeU32 mlow,HeU32 mhigh)
{
  return m->render(callback,section,mlow,mhigh);
}


HeU32 enumerateMemorySections(MemoryHTML *m,MemoryRender *callback)
{
  return m->enumerateMemorySections(callback);
}


void         memoryPower2(MemoryHTML *m,const char *section,MemoryRender *callback)
{
  m->power2(section,callback);
}

void setBlockSize(HeU32 blockSize)
{
  BLOCK_SIZE = blockSize;
  BLOCK_WIDTH = (BITMAP_SIZE/BLOCK_SIZE);
  BLOCK_PIXELS = (BLOCK_SIZE*BLOCK_SIZE);
  BLOCK_COUNT = (BITMAP_PIXELS/BLOCK_PIXELS);
}
