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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "octree.h"


OcTree::OcTree(HeF32 x1,HeF32 y1,HeF32 z1,HeF32 x2,HeF32 y2,HeF32 z2)
{
  mLastFrame = 0;
  mMin[0] = x1;
  mMin[1] = y1;
  mMin[2] = z1;

  mMax[0] = x2;
  mMax[1] = y2;
  mMax[2] = z2;

  HeF32 dx = mMax[0] - mMin[0];
  HeF32 dy = mMax[1] - mMin[1];
  HeF32 dz = mMax[2] - mMin[2];

  HeF32 recip = 1.0f / (HeF32) ODEPTH;

  mSides[0] = dx * recip;
  mSides[1] = dy * recip;
  mSides[2] = dz * recip;

  mRecip[0] = 1.0f / mSides[0];
  mRecip[1] = 1.0f / mSides[1];
  mRecip[2] = 1.0f / mSides[2];

  HeI32 ncount = ODEPTH*ODEPTH*ODEPTH;

  mNodes = MEMALLOC_NEW_ARRAY(OctTreeNode,ncount+1)[ncount+1];

  mNodes[0].setMaxSize(60000);

  mNodeIndices = 0;

}

OcTree::~OcTree(void)
{
	delete []mNodes;
  delete mNodeIndices;
}


void OcTree::Traverse(Frustum *f,HeF32 *dest,HeU32 stride,HeU32 &count,const HeF32 *base)
{
	count = 0;
  Traverse(0,0,0,ODEPTH,f,VT_PARTIAL,dest,stride,count,base);
}

void OcTree::Traverse(HeI32 fromX,HeI32 fromY,HeI32 fromZ,HeI32 length,Frustum *f,ViewTest state,HeF32 *dest,HeU32 stride,HeU32 &count,const HeF32 *base)
{
  if ( state & VT_PARTIAL ) // we have a partial solution still, so we need to test further...
  {
    HeF32 bound[6];
		HeF32 flength = (HeF32) length;

    bound[0] = (HeF32) (fromX * mSides[0])+mMin[0];
    bound[1] = (HeF32) (fromY * mSides[1])+mMin[1];
    bound[2] = (HeF32) (fromZ * mSides[2])+mMin[2];

    bound[3] = bound[0] + mSides[0]*flength;
    bound[4] = bound[1] + mSides[1]*flength;
    bound[5] = bound[2] + mSides[2]*flength;

    state = f->ViewTestAABB(bound, state );

#if 0
		RenderDebug *rd = gGlobals.GetRenderDebug();
	  HeU32 color = 0xFFFFFF;
	  if ( state & VT_PARTIAL )
	  	color = 0xFFFF00;
	  if ( state & VT_OUTSIDE )
	  	color = 0xFF0000;

		rd->DebugBound(bound,&bound[3],color);
#endif

    if ( state & VT_OUTSIDE )
      return;

  }
  if ( length == 1 )
  {
    HeI32 index = (fromZ*ODEPTH*ODEPTH)+(fromY*ODEPTH)+fromX+1;
    if ( mNodes[index].mPool )
      mNodes[index].getPool(dest,stride,count,base);
  }
  else
  {

    length/=2;

    Traverse(fromX,        fromY,        fromZ,        length,f,state,dest,stride,count,base);
    Traverse(fromX,        fromY,        fromZ+length, length,f,state,dest,stride,count,base);
    Traverse(fromX,        fromY+length, fromZ,        length,f,state,dest,stride,count,base);
    Traverse(fromX,        fromY+length, fromZ+length, length,f,state,dest,stride,count,base);
    Traverse(fromX+length, fromY,        fromZ,        length,f,state,dest,stride,count,base);
    Traverse(fromX+length, fromY,        fromZ+length, length,f,state,dest,stride,count,base);
    Traverse(fromX+length, fromY+length, fromZ,        length,f,state,dest,stride,count,base);
    Traverse(fromX+length, fromY+length, fromZ+length, length,f,state,dest,stride,count,base);

  }
}


void OcTree::setItems(const HeF32 *pos,HeU32 stride,HeU32 count)
{
  assert( mNodeIndices == 0 );

  mNodeIndices = MEMALLOC_NEW_ARRAY(OctTreeItem *,count)[count];

  mCount       = count;

  const char *p = (const char *) pos;

	HeU32 fstride = stride/sizeof(HeF32);

  for (HeU32 i=0; i<count; i++)
  {
    const HeF32 *floc = (const HeF32 *)p;

    HeI32 index = getIndex(floc);

    mNodeIndices[i] = mNodes[index].add(i*fstride, index );

    p+=stride;
  }

}

#define PER_FRAME 4000 // only update 2,000 objects each frame.

void OcTree::update(const HeF32 *poses) // update everybodys positions
{
	if( mCount )
	{
		HeU32 ii = mLastFrame;

		for (HeU32 j=0; j<PER_FRAME; j++)
		{
			OctTreeItem *item = mNodeIndices[ii];

			assert( item );

			HeU32 findex = item->mPos;

			const HeF32 *fpos = poses+findex;

			HeI32 index = getIndex( fpos );

			if ( index != (HeI32)item->mIndex )
			{
				item->mParent->remove( item );
				mNodeIndices[ii] = mNodes[index].add(findex,index);
			}

			ii++;
			if ( ii >= mCount )
			{
				ii = 0;
				break;
			}
		}
		mLastFrame = ii;
	}
}


void OcTree::reset(void)
{
  mLastFrame = 0;
  for (HeU32 i=0; i<ODEPTH*ODEPTH*ODEPTH; i++)
  {
    mNodes[i].reset();
  }
  mCount = 0;
  delete mNodeIndices;
  mNodeIndices = 0;
}
