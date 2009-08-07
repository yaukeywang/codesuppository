#ifndef CLIPPER_H

#define CLIPPER_H

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

#include "UserMemAlloc.h"

#define MAX_CLIP 64

// This is a general purpose CohenSutherland polygon clipper written in
// C++ and using STL.  This clipper is not inherently slow, but then again
// it is not necessarily blazingly fast.  It uses STL vectors to build
// the clipped polygon and return the result.
//
// This routine is presented to educate clearly the cohen sutherland
// clipping algorithm.  It clips against an arbitrary
// 3d axis aligned bounding region which you specify.
//
// It is fairly straightforward to change the Vector3d class to some
// other vertex format and easily use this routine to clip any kind
// of vertex data with any number of interpolants.
//
// This was written by John W. Ratcliff (jratcliff@verant.com) on
// August 10, 2000 and is relased into the public domain as part of
// the Code Snippet library on FlipCode.com

class Vec3d
{
public:
  Vec3d(void) { };
  Vec3d(const NxF32 *p) { x = p[0]; y = p[1]; z = p[2]; };
  Vec3d(NxF32 _x,NxF32 _y,NxF32 _z) { x = _x; y = _y; z = _z; };

  void set(NxF32 _x,NxF32 _y,NxF32 _z) { x = _x; y = _y; z = _z; };
  void set(const NxF32 *p) { x = p[0]; y = p[1]; z = p[2]; };


  NxF32 x;
  NxF32 y;
  NxF32 z;
};



// Enumeration defining the 6 planes of the view frustum.
enum ClipPlane
{
  CP_TOP = 0,
  CP_BOTTOM,
  CP_LEFT,
  CP_RIGHT,
  CP_NEAR,
  CP_FAR,
  CP_LAST
};

enum ClipResult
{
  CR_INSIDE, // completely inside the frustum.
  CR_OUTSIDE, //completely outside the frustum.
  CR_PARTIAL, // was clipped.
  CR_LAST
};

// An intermediate vertex format which contains the cohen sutherland
// clipping codes.
class ClipVertex
{
public:
  ClipVertex(void) { };

  ClipVertex(const Vec3d &pos,NxI32 code)
  {
    mPos = pos;
    mClipCode = code;
  };


  // clip vertex between v1 and v2 on this plane..
  ClipVertex(const ClipVertex &v1,
             const ClipVertex &v2,
             ClipPlane p,
             NxF32 edge); // the clipping boundary..


  void Set(const Vec3d &pos,NxI32 code)
  {
    mPos = pos;
    mClipCode = code;
  };

  const Vec3d& GetPos(void) const { return mPos; };


  NxI32 GetClipCode(void) const { return mClipCode; };
  void SetClipCode(NxI32 code) { mClipCode = code; };

  NxF32 GetX(void) const { return mPos.x; };
  NxF32 GetY(void) const { return mPos.y; };
  NxF32 GetZ(void) const { return mPos.z; };

  NxI32             mClipCode;
  Vec3d mPos;
};


class FrustumClipper
{
public:

  FrustumClipper(const NxF32 *fmin,const NxF32 *fmax)
  {
    SetFrustum(fmin,fmax);
  };

  FrustumClipper(void)
  {
    Vec3d minbound(-0.5f,-0.5f,-0.5f);
    Vec3d maxbound(+0.5f,+0.5f,+0.5f);
    SetFrustum(&minbound.x,&maxbound.x);
  };

  void SetFrustum(const NxF32 *fmin,const NxF32 *fmax)
  {
    mEdges[CP_LEFT]   = fmin[0];
    mEdges[CP_RIGHT]  = fmax[0];
    mEdges[CP_TOP]    = fmin[1];
    mEdges[CP_BOTTOM] = fmax[1];
    mEdges[CP_NEAR]   = fmin[2];
    mEdges[CP_FAR]    = fmax[2];
  };

  // compute the cohen sutherland clipping bits for this 3d position
  // against the view frustum.
  NxI32 ClipCode(const Vec3d &pos) const;

  // compute the cohen sutherland clipping codes, and *also* accumulate
  // the or bits and the and bits for a series of point tests.
  NxI32 ClipCode(const Vec3d &pos,NxI32 &ocode,NxI32 &acode) const;

  // clips input polygon against the frustum.  Places output polygon
  // in 'output'.
  ClipResult Clip(const Vec3d *input, // input vertices.
  								NxU32           vcount,   // input vertex count.
                  Vec3d       *output,
                  NxU32          &ocount) const;

  ClipResult ClipRay(const Vec3d &r1a,
                     const Vec3d &r2a,
                     Vec3d &r1b,
                     Vec3d &r2b);

private:
  NxF32 mEdges[CP_LAST]; // define the clipping planes of the view frustum

};

#endif
