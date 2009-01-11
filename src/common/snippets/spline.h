#ifndef SPLINE_H

#define SPLINE_H

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



/** @file spline.h
 *  @brief A utility class to manage 3d spline curves.
 *
 *  This is used heavily by the terrain terraforming tools for roads, lakes, and flatten operations.
 *
 *  @author John W. Ratcliff
*/

/** @file spline.cpp
 *  @brief A utility class to manage 3d spline curves.
 *
 *  This is used heavily by the terrain terraforming tools for roads, lakes, and flatten operations.
 *
 *  @author John W. Ratcliff
*/


#include "common/snippets/UserMemAlloc.h"
#include "vector.h"

class SplineNode
{
public:
  HeF32 GetY(void) { return y; };
  HeF32 x;              // time/distance x-axis component.
  HeF32 y;              // y component.
  HeF32 u;
  HeF32 p;
};

typedef USER_STL::vector< SplineNode > SplineNodeVector;

class Spline
{
public:
  void Reserve(HeI32 size)
  {
    mNodes.reserve(size);
  };
  void AddNode(HeF32 x,HeF32 y);
  void ComputeSpline(void);
  HeF32 Evaluate(HeF32 x); // evaluate Y component based on X
  HeI32 GetSize(void) { return mNodes.size(); };
  HeF32 GetEntry(HeI32 i) { return mNodes[i].GetY(); };
  void Clear(void)
  {
    mNodes.clear();
  };
private:
  SplineNodeVector mNodes; // nodes.
};

class SplineCurve
{
public:
  void Reserve(HeI32 size)
  {
    mXaxis.Reserve(size);
    mYaxis.Reserve(size);
    mZaxis.Reserve(size);
  };

  void AddControlPoint(const Vector3d<HeF32>& p); // add control point.
  void AddControlPoint(const Vector3d<HeF32>& p,HeF32 t); // add control point.

	void GetPointOnSpline(HeF32 t,Vector3d<HeF32> &pos)
	{
		HeF32 d = t*mTime;
		pos = Evaluate(d);
	}

  Vector3d<HeF32> Evaluate(HeF32 dist);

  HeF32 GetLength(void) { return mTime; }; //total length of spline

  HeI32 GetSize(void) { return mXaxis.GetSize(); };

  Vector3d<HeF32> GetEntry(HeI32 i);

  void ComputeSpline(void); // compute spline.

  void Clear(void)
  {
    mXaxis.Clear();
    mYaxis.Clear();
    mZaxis.Clear();
    mTime = 0;
  };

  HeF32 Set(const Vector3dVector &vlist)
  {
    Clear();
    HeI32 count = vlist.size();
    Reserve(count);
    Vector3dVector::const_iterator i;
    for (i=vlist.begin(); i!=vlist.end(); ++i) AddControlPoint( (*i) );
    ComputeSpline();
    return mTime;
  };

  static void ResampleControlPoints(const Vector3dVector &inputpoints,
                                 Vector3dVector &outputpoints,
                                 HeF32 dtime)
  {
    SplineCurve curve;

    HeF32 length = curve.Set(inputpoints);
    for (HeF32 l=0; l<=length; l+=dtime)
    {
      Vector3d<HeF32> pos = curve.Evaluate(l);
      outputpoints.push_back(pos);
    }
  };

private:
  HeF32  mTime; // time/distance travelled.
  Spline mXaxis;
  Spline mYaxis;
  Spline mZaxis;
};

#endif
