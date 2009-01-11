#ifndef OCTREE_H

#define OCTREE_H

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



// voxelizes a space into a 64x64x64 grid.
// The depth is hard-coded on purpose so that we can
// gain performance improvements by avoiding multiplies.
// If you want a larger (or smaller) vovel space, then you
// would have to change the define
#include <string.h>

#include "common/snippets/UserMemAlloc.h"
#include "common/snippets/frustum.h"
#include "common/snippets/pool.h"

#define ODEPTH 32
#define MAXPER 2048 // maximum number of objects that can be located in each node


class OctTreeNode;

class OctTreeItem
{
public:

  OctTreeItem(void)
  {
    mNext = 0;
    mPrevious = 0;
    mPos = 0;
    mIndex = 0;
    mParent = 0;
  }

  OctTreeItem * GetNext(void)     const { return mNext; };
  OctTreeItem * GetPrevious(void) const { return mPrevious; };
  void          SetNext(OctTreeItem *next) { mNext = next; };
  void          SetPrevious(OctTreeItem *previous) { mPrevious = previous; };

  OctTreeItem *mNext;
  OctTreeItem *mPrevious;
  OctTreeNode *mParent;
  HeU32 mPos;           // the position of the item.
  HeU32 mIndex;         // the index for the item.
};

class OctTreeNode
{
public:
  OctTreeNode(void)
  {
    mPool = 0;
    mMaxSize = MAXPER;
  }

  ~OctTreeNode(void)
  {
    delete mPool;
  }

  OctTreeItem * add(HeU32 pos,HeU32 index)
  {

    if ( mPool == 0 )
    {
      mPool = MEMALLOC_NEW(Pool< OctTreeItem >);
      mPool->Set(mMaxSize,100,0x7FFFFFFF,"OcTreeItem",__FILE__,__LINE__);
    }

    OctTreeItem *item = mPool->GetFreeLink();
    assert(item);

    if ( item )
    {
      item->mPos = pos;
      item->mIndex = index;
      item->mParent = this;
    }
    return item;
  }

  void remove(OctTreeItem *p)
  {
    mPool->Release(p);
    if ( mPool->GetUsedCount() == 0 )
    {
      delete mPool;
      mPool = 0;
    }
  }

  void getPool(HeF32 *dest,HeU32 /*stride*/,HeU32 &count,const HeF32 *base)
  {
    if ( mPool )
    {
      HeI32 pc = mPool->Begin();

      OctTreeItem *item = mPool->GetNext();

			dest = dest+(count*8);

			for (HeI32 i=0; i<pc; i++)
      {

				const HeF32 *source = base+item->mPos;

#if 0
				memcpy(dest,source,sizeof(HeF32)*8);
#else
				dest[0] = source[0];
				dest[1] = source[1];
				dest[2] = source[2];
				dest[3] = source[3];
				dest[4] = source[4];
				dest[5] = source[5];
				dest[6] = source[6];
				dest[7] = source[7];
#endif

        dest+=8;

				item = item->GetNext();

      }
			count+=pc;
    }
  }

  void reset(void)
  {
    delete mPool;
    mPool = 0;
  }

  void setMaxSize(HeU32 maxs) { mMaxSize = maxs; };

  HeU32 mMaxSize;
  Pool< OctTreeItem > *mPool; // the pool of items at this node
};

class OcTree
{
public:
  OcTree(HeF32 x1,HeF32 y1,HeF32 z1,HeF32 x2,HeF32 y2,HeF32 z2);
  ~OcTree(void);


  void Traverse(Frustum *f,HeF32 *dest,HeU32 stride,HeU32 &count,const HeF32 *base);

  void Traverse(HeI32 fromX,HeI32 fromY,HeI32 fromZ,HeI32 length,Frustum *f,ViewTest state,HeF32 *dest,HeU32 stride,HeU32 &count,const HeF32 *base);

  void setItems(const HeF32 *pos,HeU32 stride,HeU32 count);

  HeI32 getIndex(const HeF32 *p)
  {
    HeI32 index = 0;

    HeI32 iy = (HeI32)((p[1]-mMin[1])*mRecip[1]);
    if ( iy >= 0 && iy < ODEPTH )
    {
      HeI32 ix = (HeI32)((p[0]-mMin[0])*mRecip[0]);
      if ( ix >= 0 && ix < ODEPTH )
      {
        HeI32 iz = (HeI32)((p[2]-mMin[2])*mRecip[2]);
        if ( iz >= 0 && iz < ODEPTH )
        {
          index = (iz*(ODEPTH*ODEPTH)) + (iy*ODEPTH) + ix+ 1;
        }
      }
    }

    return index;
  }

  void update(const HeF32 *poses); // update everybodys positions

  bool isEmpty(void) const
  {
    if ( mNodeIndices ) return false;
    return true;
  }

  void reset(void);

public:
  HeF32          mMin[3];
  HeF32          mMax[3];
  HeF32          mSides[3];
  HeF32          mRecip[3];

  HeU32   mDepth;
  OctTreeNode   *mNodes;


  OctTreeItem  **mNodeIndices;  // the node in which each one *currently* resides.

  HeU32   mCount;
  HeU32   mLastFrame;

};

#endif
