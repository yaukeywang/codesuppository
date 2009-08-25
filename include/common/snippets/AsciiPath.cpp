#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "AsciiPath.h"
#include "UserMemAlloc.h"


typedef USER_STL::vector< NxF32 > HeF32Vector;

class MyAsciiPath : public AsciiPath
{
public:
  MyAsciiPath(void)
  {
    mCount = 0;
  }

  ~MyAsciiPath(void)
  {
    release();
  }

  void release(void)
  {
    mCount = 0;
    mPoints.clear();
  }


  const char * nextData(const char *data) const
  {
    while ( *data && *data != ',' ) data++;
    if ( *data == ',' ) data++;
    if ( *data == 0 ) 
      data = 0;
    return data;
  }

  NxU32 setData(const char *data)
  {
    NxU32 ret = 0;

    release();

    if ( data )
    {
      NxU32 count = atoi(data);
      NxU32 i;
      for (i=0; i<count; i++)
      {
        data = nextData(data);
        if ( data )
        {
          NxF32 x = (NxF32)atof(data);
          data = nextData(data);
          if ( data )
          {
            NxF32 y = (NxF32)atof(data);
            mPoints.push_back(x);
            mPoints.push_back(y);
          }
          else
          {
            break;
          }
        }
        else
        {
          break;
        }
      }
      if ( i == count )
      {
        ret = count;
        mCount = count;
      }
      else
      {
        mPoints.clear();
      }
    }

    return ret;
  }

  bool  getPoint(NxU32 index,NxF32 &x,NxF32 &y) const
  {
    bool ret = false;

    if ( !mPoints.empty() )
    {
      assert( index >= 0 && index < mCount );
      if ( index >= 0 && index < mCount )
      {
        x = mPoints[index*2+0];
        y = mPoints[index*2+1];
        ret = true;
      }
    }

    return ret;
  }

  NxU32 getPointCount(void) const
  {
    return mCount;
  }

private:
  NxU32       mCount;
  HeF32Vector mPoints;
};

AsciiPath * createAsciiPath(void)
{
  MyAsciiPath *ma = MEMALLOC_NEW(MyAsciiPath);
  return static_cast< AsciiPath *>(ma);
}

void        releaseAsciiPath(AsciiPath *ap)
{
  MyAsciiPath *ma = static_cast< MyAsciiPath *>(ap);
  delete ma;
}

