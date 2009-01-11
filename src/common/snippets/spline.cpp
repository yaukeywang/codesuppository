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
#include <math.h>
#include <assert.h>

#include "spline.h"

static HeF32 f(HeF32 x)
{
	return( x*x*x - x);
}


// build spline.
// copied from Sedgwick's Algorithm's in C, page 548.
void Spline::ComputeSpline(void)
{

	HeI32 i;
  HeI32 n = mNodes.size();

	HeF32 *d = MEMALLOC_NEW_ARRAY(float,n)[n];
	HeF32 *w = MEMALLOC_NEW_ARRAY(float,n)[n];


	for (i=1; i<(n-1); i++)
	{
		d[i] = 2.0f * (mNodes[i+1].x - mNodes[i-1].x);
	}

	for (i=0; i<(n-1); i++) mNodes[i].u = mNodes[i+1].x-mNodes[i].x;

	for (i=1; i<(n-1); i++)
		w[i] = 6.0f*((mNodes[i+1].y - mNodes[i].y)   / mNodes[i].u -
				      	 (mNodes[i].y   - mNodes[i-1].y) / mNodes[i-1].u);

	mNodes[0].p   = 0.0f;
	mNodes[n-1].p = 0.0f;

	for (i=1; i<(n-2); i++)
	{
		w[i+1] = w[i+1] - w[i]*mNodes[i].u /d[i];
		d[i+1] = d[i+1] - mNodes[i].u * mNodes[i].u / d[i];
	}

	for (i=n-2; i>0; i--)
		mNodes[i].p = (w[i] - mNodes[i].u * mNodes[i+1].p ) / d[i];

  delete[] d;
  delete[] w;
}

HeF32 Spline::Evaluate(HeF32 v)
{
	HeF32 t;
	HeI32 i=0;
  HeI32 n = mNodes.size();

	while ( v > mNodes[i+1].x && i < (n-1) ) i++;

	t = (v - mNodes[i].x ) / mNodes[i].u;

	return( t*mNodes[i+1].y + (1-t)*mNodes[i].y +
					mNodes[i].u * mNodes[i].u * (f(t)*mNodes[i+1].p +
					f(1-t)*mNodes[i].p )/6.0f );
}


void Spline::AddNode(HeF32 x,HeF32 y)
{
  SplineNode s;
  s.x = x;
  s.y = y;
  mNodes.push_back(s);
}

void SplineCurve::AddControlPoint(const Vector3d<HeF32>& p)  // add control point.
{
  HeI32 size = mXaxis.GetSize();

  if ( size )
  {
    size--;
    Vector3d<HeF32> last;
    last.x = mXaxis.GetEntry(size);
    last.y = mYaxis.GetEntry(size);
    last.z = mZaxis.GetEntry(size);
    HeF32 dist = last.Distance(p);
    mTime+=dist;
  }
  else
    mTime = 0;

  mXaxis.AddNode(mTime,p.x);
  mYaxis.AddNode(mTime,p.y);
  mZaxis.AddNode(mTime,p.z);
}

void SplineCurve::AddControlPoint(const Vector3d<HeF32>& p,HeF32 t)  // add control point.
{

  HeI32 size = mXaxis.GetSize();

  if ( size )
  {
    size--;
    Vector3d<HeF32> last;
    last.x = mXaxis.GetEntry(size);
    last.y = mYaxis.GetEntry(size);
    last.z = mZaxis.GetEntry(size);
  }

	mTime = t;

  mXaxis.AddNode(mTime,p.x);
  mYaxis.AddNode(mTime,p.y);
  mZaxis.AddNode(mTime,p.z);
}

void SplineCurve::ComputeSpline(void)  // compute spline.
{
  mXaxis.ComputeSpline();
  mYaxis.ComputeSpline();
  mZaxis.ComputeSpline();
}

Vector3d<HeF32> SplineCurve::Evaluate(HeF32 dist)
{
  Vector3d<HeF32> p;

  p.x = mXaxis.Evaluate(dist);
  p.y = mYaxis.Evaluate(dist);
  p.z = mZaxis.Evaluate(dist);

  return p;
}

Vector3d<HeF32> SplineCurve::GetEntry(HeI32 i)
{
  HE_ASSERT( i>= 0 && i < GetSize() );
  Vector3d<HeF32> p;
  p.x = mXaxis.GetEntry(i);
  p.y = mYaxis.GetEntry(i);
  p.z = mZaxis.GetEntry(i);
  return p;
}
