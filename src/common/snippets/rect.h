#ifndef RECT_H

#define RECT_H

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


#include "common/snippets/UserMemAlloc.h"
#include "vector.h"
#include "matrix.h"
#include "common/snippets/clipper.h"

#define OLEFT   (1<<0)
#define ORIGHT  (1<<1)
#define OTOP    (1<<2)
#define OBOTTOM (1<<3)
#define OFRONT  (1<<4)
#define OBACK   (1<<5)

template <class Type> class Rect3d
{
public:
  Rect3d(void)
  {
    r1.Set(0,0,0);
    r2.Set(0,0,0);
  };

  Rect3d(const Vector3d<Type> &v1,const Vector3d<Type>& v2)
  {
    r1 = v1;
    r2 = v2;
  };

  Rect3d(Type *v1,Type *v2)
  {
    r1.x = v1[0];
    r1.y = v1[1];
    r1.z = v1[2];
    r2.x = v2[0];
    r2.y = v2[1];
    r2.z = v2[2];
  }

  Rect3d(Type x1,Type y1,Type z1,
         Type x2,Type y2,Type z2)
  {
    r1.Set(x1,y1,z1);
    r2.Set(x2,y2,z2);
  };

  void Set(Type x1,Type y1,Type z1,Type x2,Type y2,Type z2)
  {

    Type temp;

    if ( x1 > x2 )
    {
      temp = x2;
      x2   = x1;
      x1   = temp;
    }

    if ( y1 > y2 )
    {
      temp = y2;
      y2   = y1;
      y1   = temp;
    }

    if ( z1 > z2 )
    {
      temp = z2;
      z2   = z1;
      z1   = temp;
    }

    r1.Set(x1,y1,z1);
    r2.Set(x2,y2,z2);

  };

  void InitMinMax(void)
  {
    r1.Set(1E9,1E9,1E9);
    r2.Set(-1E9,-1E9,-1E9);
  };

  void MinMax(const Vector3d<Type> &pos)
  {
	  MinMax(pos.GetX(),pos.GetY(),pos.GetZ());
  };

  void MinMax(const HeF32 *pos)
  {
	  MinMax(pos[0],pos[1],pos[2]);
  };

  void MinMax(Type x,Type y,Type z)
  {
    if ( x < r1.GetX() ) r1.SetX(x);
    if ( y < r1.GetY() ) r1.SetY(y);
    if ( z < r1.GetZ() ) r1.SetZ(z);

    if ( x > r2.GetX() ) r2.SetX(x);
    if ( y > r2.GetY() ) r2.SetY(y);
    if ( z > r2.GetZ() ) r2.SetZ(z);
  };

  bool Inside(const Rect3d<Type>& test) const
  {
    if ( test.r1.GetX() >= r1.GetX() && test.r2.GetX() <= r2.GetX() &&
         test.r1.GetY() >= r1.GetY() && test.r2.GetY() <= r2.GetY() &&
         test.r1.GetZ() >= r1.GetZ() && test.r2.GetZ() <= r2.GetZ() )
    {
      return true;
    }
  }

  bool Inside(const Vector3d<Type>& pos) const
  {
    if ( pos.GetZ() >= r1.GetZ() && pos.GetZ() <= r2.GetZ() &&
         pos.GetY() >= r1.GetY() && pos.GetY() <= r2.GetY() &&
         pos.GetX() >= r1.GetX() && pos.GetX() <= r2.GetX() ) return true;
    return false;
  };

  bool InsideXY(const Vector3d<Type>& pos) const
  {
    if ( pos.GetX() >= r1.GetX() && pos.GetX() <= r2.GetX() &&
         pos.GetY() >= r1.GetY() && pos.GetY() <= r2.GetY() ) return true;
    return false;
  };

  bool InsideXY(HeF32 x,HeF32 y) const
  {
    if ( x >= r1.GetX() && x <= r2.GetX() &&
         y >= r1.GetY() && y <= r2.GetY() ) return true;
    return false;
  };

  // Full polygon clip against this frustum to determine
  // *excat* intersection test!
  bool Intersect(const Vector3d<HeF32> &p1,
                 const Vector3d<HeF32> &p2,
                 const Vector3d<HeF32> &p3) const
  {

    HeI32 ocode1 = ClipTestXYZ(p1);
    if ( !ocode1 ) return true;
    HeI32 ocode2 = ClipTestXYZ(p2);
    if ( !ocode2 ) return true;
    HeI32 ocode3 = ClipTestXYZ(p3);
    if ( !ocode3 ) return true;
    HeI32 acode = ocode1 & ocode2 & ocode3;
    if ( acode ) return false;

    Vector3dVector poly;

    poly.push_back(p1);
    poly.push_back(p2);
    poly.push_back(p3);

    FrustumClipper c(r1,r2);
    Vector3dVector result;

    ClipResult ok = c.Clip(poly,result,true);

    if ( ok == CR_OUTSIDE ) return false;

    return true;
  };

  // Full polygon clip against this frustum to determine
  // *excat* intersection test!
  bool IntersectXY(const Vector3d<HeF32> &p1,
                 const Vector3d<HeF32> &p2,
                 const Vector3d<HeF32> &p3) const
  {
    Vector3dVector poly;
    poly.push_back(p1);
    poly.push_back(p2);
    poly.push_back(p3);
    FrustumClipper c(r1,r2);
    Vector3dVector result;

    ClipResult ok = c.Clip(poly,result,false);

    if ( ok == CR_OUTSIDE ) return false;
    return true;
  };

  bool Intersect(const Rect3d<Type> &test) const
  {
    if ( test.r2.GetZ() < r1.GetZ() ) return false;
    if ( test.r2.GetY() < r1.GetY() ) return false;
    if ( test.r2.GetX() < r1.GetX() ) return false;

    if ( test.r1.GetZ() > r2.GetZ() ) return false;
    if ( test.r1.GetY() > r2.GetY() ) return false;
    if ( test.r1.GetX() > r2.GetX() ) return false;
    return true;
  };

  HeI32 ClipTestXY(const Vector3d<HeF32>& p) const
  {
    HeI32 ocode = 0;
    if ( p.GetX() < r1.GetX() ) ocode|=OLEFT;
    if ( p.GetX() > r2.GetX() ) ocode|=ORIGHT;
    if ( p.GetY() < r1.GetY() ) ocode|=OTOP;
    if ( p.GetY() > r2.GetY() ) ocode|=OBOTTOM;
    return ocode;
  };

  HeI32 ClipTestXYZ(const Vector3d<HeF32>& p) const
  {
    HeI32 ocode = 0;
    if ( p.GetX() < r1.GetX() ) ocode|=OLEFT;
    if ( p.GetX() > r2.GetX() ) ocode|=ORIGHT;
    if ( p.GetY() < r1.GetY() ) ocode|=OTOP;
    if ( p.GetY() > r2.GetY() ) ocode|=OBOTTOM;
    if ( p.GetZ() < r1.GetZ() ) ocode|=OFRONT;
    if ( p.GetZ() > r2.GetZ() ) ocode|=OBACK;
    return ocode;
  };

  bool OverlapXY(const Vector3d<HeF32>& p1,
               const Vector3d<HeF32>& p2,
               const Vector3d<HeF32>& p3) const
  {
    HeI32 acode= OLEFT | ORIGHT | OTOP | OBOTTOM;

    HeI32 ocode = ClipTestXY(p1);

    acode&=ocode;

    ocode = ClipTestXY(p2);
    acode&=ocode;

    ocode = ClipTestXY(p3);
    acode&=ocode;

    if ( acode ) return false; // if have an andcode then

    return true;
  }

  bool IntersectXY(const Rect3d<Type> &test)
  {
    if ( test.r2.GetY() < r1.GetY() ) return false;
    if ( test.r1.GetY() > r2.GetY() ) return false;
    if ( test.r2.GetX() < r1.GetX() ) return false;
    if ( test.r1.GetX() > r2.GetX() ) return false;
    return true;
  };

  bool Intersect(const Vector3d<Type>& p1,const Vector3d<Type>& p2) const
  {
    HeI32 ocode1 = ClipTestXYZ(p1);
    if ( !ocode1 ) return true;
    HeI32 ocode2 = ClipTestXYZ(p2);
    if ( (ocode1 & ocode2) ) return false;

    FrustumClipper c;

    c.SetFrustum( r1, r2 );
    Vector3d<HeF32> br1,br2;

    ClipResult ok = c.ClipRay( p1, p2, br1, br2, true );
    if ( ok == CR_OUTSIDE ) return false;

    return true;

  };


  Type GetLength(void) const
  {
    return r1.Distance(r2);
  };

  Vector3d<Type> GetCenter(void) const
  {
    Vector3d<Type> c;
    c.x = (r2.GetX() - r1.GetX())*0.5f + r1.GetX();
    c.y = (r2.GetY() - r1.GetY())*0.5f + r1.GetY();
    c.z = (r2.GetZ() - r1.GetZ())*0.5f + r1.GetZ();
    return c;
  };

  void TransformBoundAABB(const MyMatrix &transform,const Rect3d<HeF32> &b)
  {
    InitMinMax();
    BoundTest(transform,b.r1.x,b.r1.y,b.r1.z);
    BoundTest(transform,b.r2.x,b.r1.y,b.r1.z);
    BoundTest(transform,b.r2.x,b.r2.y,b.r1.z);
    BoundTest(transform,b.r1.x,b.r2.y,b.r1.z);
    BoundTest(transform,b.r1.x,b.r1.y,b.r2.z);
    BoundTest(transform,b.r2.x,b.r1.y,b.r2.z);
    BoundTest(transform,b.r2.x,b.r2.y,b.r2.z);
    BoundTest(transform,b.r1.x,b.r2.y,b.r2.z);
  };

  void InverseTransformBoundAABB(const MyMatrix &transform,const Rect3d<HeF32> &b)
  {
    InitMinMax();
    InverseBoundTest(transform,b.r1.x,b.r1.y,b.r1.z);
    InverseBoundTest(transform,b.r2.x,b.r1.y,b.r1.z);
    InverseBoundTest(transform,b.r2.x,b.r2.y,b.r1.z);
    InverseBoundTest(transform,b.r1.x,b.r2.y,b.r1.z);
    InverseBoundTest(transform,b.r1.x,b.r1.y,b.r2.z);
    InverseBoundTest(transform,b.r2.x,b.r1.y,b.r2.z);
    InverseBoundTest(transform,b.r2.x,b.r2.y,b.r2.z);
    InverseBoundTest(transform,b.r1.x,b.r2.y,b.r2.z);
  };

  void BoundTest(const MyMatrix &transform,HeF32 x,HeF32 y,HeF32 z)
  {
    Vector3d<HeF32> pos(x,y,z);
    Vector3d<HeF32> t;
    transform.Transform(pos,t);
    MinMax(t);
  };

  void InverseBoundTest(const MyMatrix &transform,HeF32 x,HeF32 y,HeF32 z)
  {
    Vector3d<HeF32> pos(x,y,z);
    Vector3d<HeF32> t;
    transform.InverseRotateTranslate(pos,t);
    MinMax(t);
  };

  void MinMax(const Rect3d<HeF32> &b)
  {
    if ( b.r1.x < r1.x ) r1.x = b.r1.x;
    if ( b.r1.y < r1.y ) r1.y = b.r1.y;
    if ( b.r1.z < r1.z ) r1.z = b.r1.z;

    if ( b.r2.x > r2.x ) r2.x = b.r2.x;
    if ( b.r2.y > r2.y ) r2.y = b.r2.y;
    if ( b.r2.z > r2.z ) r2.z = b.r2.z;
  }


  void Report(void);
//**private:
  Vector3d<Type> r1;
  Vector3d<Type> r2;
};


// A 2d bounding rectangle.
/** Represents a 2d bounding region.*/
template <class Type> class Rect2d
{
public:
  Rect2d(void){  };

  Rect2d(const Vector2d<Type> &v1,const Vector2d<Type>& v2)
  {
    r1 = v1;
    r2 = v2;
  };

  Rect2d(Type x1,Type y1,
         Type x2,Type y2)
  {
    Set(x1,y1,x2,y2);
  };

  bool operator==(const Rect2d<Type> &a) const
  {
    return( a.r1 == r1 && a.r2 == r2 );
  };

  bool operator!=(const Rect2d<Type> &a) const
  {
    return( a.r1 != r1 || a.r2 != r2 );
  };

  void Set(Type x1,Type y1,Type x2,Type y2)
  {
    r1.Set(x1,y1);
    r2.Set(x2,y2);
  };


  void InitMinMax(void)
  {
    r1.Set(1E9,1E9);
    r2.Set(-1E9,-1E9);
  };

  void MinMax(const Vector2d<Type> &pos)
  {
	  MinMax(pos.GetX(),pos.GetY());
  };


  void MinMax(Type x,Type y)
  {
    if ( x < r1.GetX() ) r1.SetX(x);
    if ( y < r1.GetY() ) r1.SetY(y);

    if ( x > r2.GetX() ) r2.SetX(x);
    if ( y > r2.GetY() ) r2.SetY(y);
  };


  HeI32 ClipTestXY(const Vector2d<HeF32>& p) const
  {
    HeI32 ocode = 0;
    if ( p.GetX() < r1.GetX() ) ocode|=OLEFT;
    if ( p.GetX() > r2.GetX() ) ocode|=ORIGHT;
    if ( p.GetY() < r1.GetY() ) ocode|=OTOP;
    if ( p.GetY() > r2.GetY() ) ocode|=OBOTTOM;
    return ocode;
  };

  bool Overlap(const Vector2d<HeF32>& p1,
               const Vector2d<HeF32>& p2,
               const Vector2d<HeF32>& p3) const
  {
    HeI32 acode=0xFFFF;
    HeI32 ocode = ClipTestXY(p1);
    acode&=ocode;
    ocode = ClipTestXY(p2);
    acode&=ocode;
    ocode = ClipTestXY(p3);
    acode&=ocode;
    if ( acode ) return true;
    return false;
  }

  bool Inside(const Rect2d<Type>& test) const
  {
    if ( test.r1.GetX() >= r1.GetX() && test.r2.GetX() <= r2.GetX() &&
         test.r1.GetY() >= r1.GetY() && test.r2.GetY() <= r2.GetY() )
    {
      return true;
    }
    return false;
  }

  bool Inside(const Vector2d<Type>& pos) const
  {
    if ( pos.GetY() >= r1.GetY() && pos.GetY() <= r2.GetY() &&
         pos.GetX() >= r1.GetX() && pos.GetX() <= r2.GetX() ) return true;
    return false;
  };

  bool Intersect(const Rect2d<Type> &test) const
  {
    if ( test.r2.GetY() < r1.GetY() ) return false;
    if ( test.r2.GetX() < r1.GetX() ) return false;

    if ( test.r1.GetY() > r2.GetY() ) return false;
    if ( test.r1.GetX() > r2.GetX() ) return false;
    return true;
  };

  Vector2d<Type> r1;
  Vector2d<Type> r2;
};

// Graphics Gems III triangle unit-cube intersection test.
HeI32 GemTriangleTest(const Vector3d<HeF32> &p1,
                  const Vector3d<HeF32> &p2,
                  const Vector3d<HeF32> &p3);


// builds a co-ordinate space that takes an input AABB and computes
// how to translate it into a unit cube.
// Then all co-ordinates which come through here for intersection
// testing can be converted into that co-ordinate space.
class UnitCube
{
public:
  void Set(const Rect3d<HeF32> &rect)
  {
    mWid      = (rect.r2 - rect.r1)*0.5f;
    mOrigin.x = rect.r1.x + mWid.x;
    mOrigin.y = rect.r1.y + mWid.y;
    mOrigin.z = rect.r1.z + mWid.z;
    mScale.x  = 0.5f / mWid.x;
    mScale.y  = 0.5f / mWid.y;
    mScale.z  = 0.5f / mWid.z;
  };

  void Transform(const Vector3d<HeF32> &source,Vector3d<HeF32> &dest) const
  {
    dest.x = (source.x - mOrigin.x ) * mScale.x;
    dest.y = (source.y - mOrigin.y ) * mScale.y;
    dest.z = (source.z - mOrigin.z ) * mScale.z;
  };

  bool Intersect(const Vector3d<HeF32> &p1,
                 const Vector3d<HeF32> &p2,
                 const Vector3d<HeF32> &p3) const
  {
    Vector3d<HeF32> t1,t2,t3;

    Transform(p1,t1);
    Transform(p2,t2);
    Transform(p3,t3);

    HeI32 code = GemTriangleTest(t1,t2,t3);
    if ( !code ) return true;
    return false;

  };
private:
  Vector3d<HeF32> mWid;
  Vector3d<HeF32> mOrigin;
  Vector3d<HeF32> mScale;
};

#endif
