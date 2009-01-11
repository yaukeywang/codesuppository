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

#include "vformat.h"

template<> MeshVertex VertexLess<MeshVertex>::mFind = MeshVertex();
template<> USER_STL::vector<MeshVertex > *VertexLess<MeshVertex>::mList=0;

template<>
bool VertexLess<MeshVertex>::operator()(HeI32 v1,HeI32 v2) const
{

	const MeshVertex& a = Get(v1);
	const MeshVertex& b = Get(v2);

	if ( a.GetX() < b.GetX() ) return true;
	if ( a.GetX() > b.GetX() ) return false;

	if ( a.GetY() < b.GetY() ) return true;
	if ( a.GetY() > b.GetY() ) return false;

	if ( a.GetZ() < b.GetZ() ) return true;
	if ( a.GetZ() > b.GetZ() ) return false;

	if ( a.mNormal.x < b.mNormal.x ) return true;
	if ( a.mNormal.x > b.mNormal.x ) return false;

	if ( a.mNormal.y < b.mNormal.y ) return true;
	if ( a.mNormal.y > b.mNormal.y ) return false;

	if ( a.mNormal.z < b.mNormal.z ) return true;
	if ( a.mNormal.z > b.mNormal.z ) return false;


	if ( a.mTexel1.x < b.mTexel1.x ) return true;
	if ( a.mTexel1.x > b.mTexel1.x ) return false;

	if ( a.mTexel1.y < b.mTexel1.y ) return true;
	if ( a.mTexel1.y > b.mTexel1.y ) return false;

	if ( a.mTexel2.x < b.mTexel2.x ) return true;
	if ( a.mTexel2.x > b.mTexel2.x ) return false;

	if ( a.mTexel2.y < b.mTexel2.y ) return true;
	if ( a.mTexel2.y > b.mTexel2.y ) return false;


	return false;
};


template<> MeshDeformVertex VertexLess<MeshDeformVertex>::mFind = MeshDeformVertex();
template<> USER_STL::vector<MeshDeformVertex > *VertexLess<MeshDeformVertex>::mList=0;

template<> 
bool VertexLess<MeshDeformVertex>::operator()(HeI32 v1,HeI32 v2) const
{

	const MeshDeformVertex& a = Get(v1);
	const MeshDeformVertex& b = Get(v2);

	if ( a.GetX() < b.GetX() ) return true;
	if ( a.GetX() > b.GetX() ) return false;

	if ( a.GetY() < b.GetY() ) return true;
	if ( a.GetY() > b.GetY() ) return false;

	if ( a.GetZ() < b.GetZ() ) return true;
	if ( a.GetZ() > b.GetZ() ) return false;

	if ( a.mNormal.x < b.mNormal.x ) return true;
	if ( a.mNormal.x > b.mNormal.x ) return false;

	if ( a.mNormal.y < b.mNormal.y ) return true;
	if ( a.mNormal.y > b.mNormal.y ) return false;

	if ( a.mNormal.z < b.mNormal.z ) return true;
	if ( a.mNormal.z > b.mNormal.z ) return false;


	if ( a.mTexel1.x < b.mTexel1.x ) return true;
	if ( a.mTexel1.x > b.mTexel1.x ) return false;

	if ( a.mTexel1.y < b.mTexel1.y ) return true;
	if ( a.mTexel1.y > b.mTexel1.y ) return false;

	if ( a.mTexel2.x < b.mTexel2.x ) return true;
	if ( a.mTexel2.x > b.mTexel2.x ) return false;

	if ( a.mTexel2.y < b.mTexel2.y ) return true;
	if ( a.mTexel2.y > b.mTexel2.y ) return false;


	return false;
};


template<> PosVertex VertexLess<PosVertex>::mFind = PosVertex();
template<> USER_STL::vector<PosVertex > *VertexLess<PosVertex>::mList=0;

template<>
bool VertexLess<PosVertex>::operator()(HeI32 v1,HeI32 v2) const
{

	const PosVertex& a = Get(v1);
	const PosVertex& b = Get(v2);

	if ( a.GetX() < b.GetX() ) return true;
	if ( a.GetX() > b.GetX() ) return false;

	if ( a.GetY() < b.GetY() ) return true;
	if ( a.GetY() > b.GetY() ) return false;

	if ( a.GetZ() < b.GetZ() ) return true;
	if ( a.GetZ() > b.GetZ() ) return false;

	return false;
};


