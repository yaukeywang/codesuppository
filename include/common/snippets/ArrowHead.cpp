#include <math.h>
#include <assert.h>

#include "ArrowHead.h"


//**** Below are the handful of math routines needed to build the properly oriented arrow head.  These were lifted from 'FloatMath'.
//**** They are copied here as static methods to make this a true code snippet (i.e. one header and one source file only)

static const NxF32 FM_PI = 3.141592654f;
static const NxF32 FM_DEG_TO_RAD = ((2.0f * FM_PI) / 360.0f);
static const NxF32 FM_RAD_TO_DEG = (360.0f / (2.0f * FM_PI));


static NxF32 fm_normalize(NxF32 *n) // normalize this vector
{

  NxF32 dist = sqrtf(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
  NxF32 mag = 0;

  if ( dist > 0.0000001f )
    mag = 1.0f / dist;

  n[0]*=mag;
  n[1]*=mag;
  n[2]*=mag;

  return dist;
}

static NxF32 fm_dot(const NxF32 *p1,const NxF32 *p2)
{
  return p1[0]*p2[0]+p1[1]*p2[1]+p1[2]*p2[2];
}

static void fm_cross(NxF32 *cross,const NxF32 *a,const NxF32 *b)
{
	cross[0] = a[1]*b[2] - a[2]*b[1];
	cross[1] = a[2]*b[0] - a[0]*b[2];
	cross[2] = a[0]*b[1] - a[1]*b[0];
}


// Reference, from Stan Melax in Game Gems I
//  Quaternion q;
//  vector3 c = CrossProduct(v0,v1);
//  NxF32   d = DotProduct(v0,v1);
//  NxF32   s = (NxF32)sqrt((1+d)*2);
//  q.x = c.x / s;
//  q.y = c.y / s;
//  q.z = c.z / s;
//  q.w = s /2.0f;
//  return q;
static void fm_rotationArc(const NxF32 *v0,const NxF32 *v1,NxF32 *quat)
{
  NxF32 cross[3];

  fm_cross(cross,v0,v1);
  NxF32 d = fm_dot(v0,v1);
  NxF32 s = sqrtf((1+d)*2);
  NxF32 recip = 1.0f / s;

  quat[0] = cross[0] * recip;
  quat[1] = cross[1] * recip;
  quat[2] = cross[2] * recip;
  quat[3] = s * 0.5f;

}


static void fm_quatToMatrix(const NxF32 *quat,NxF32 *matrix) // convert quaterinion rotation to matrix, zeros out the translation component.
{

	NxF32 xx = quat[0]*quat[0];
	NxF32 yy = quat[1]*quat[1];
	NxF32 zz = quat[2]*quat[2];
	NxF32 xy = quat[0]*quat[1];
	NxF32 xz = quat[0]*quat[2];
	NxF32 yz = quat[1]*quat[2];
	NxF32 wx = quat[3]*quat[0];
	NxF32 wy = quat[3]*quat[1];
	NxF32 wz = quat[3]*quat[2];

	matrix[0*4+0] = 1 - 2 * ( yy + zz );
	matrix[1*4+0] =     2 * ( xy - wz );
	matrix[2*4+0] =     2 * ( xz + wy );

	matrix[0*4+1] =     2 * ( xy + wz );
	matrix[1*4+1] = 1 - 2 * ( xx + zz );
	matrix[2*4+1] =     2 * ( yz - wx );

	matrix[0*4+2] =     2 * ( xz - wy );
	matrix[1*4+2] =     2 * ( yz + wx );
	matrix[2*4+2] = 1 - 2 * ( xx + yy );

	matrix[3*4+0] = matrix[3*4+1] = matrix[3*4+2] = (NxF32)0.0f;
	matrix[0*4+3] =matrix[1*4+3] = matrix[2*4+3] = (NxF32) 0.0f;
	matrix[3*4+3] =(NxF32) 1.0f;

}


static void  fm_setTranslation(const NxF32 *translation,NxF32 *matrix)
{
  matrix[12] = translation[0];
  matrix[13] = translation[1];
  matrix[14] = translation[2];
}


static void  fm_transform(const NxF32 *matrix,const NxF32 *v,NxF32 *t) // rotate and translate this point
{
  if ( matrix )
  {
    t[0] = (matrix[0*4+0] * v[0]) +  (matrix[1*4+0] * v[1]) + (matrix[2*4+0] * v[2]) + matrix[3*4+0];
    t[1] = (matrix[0*4+1] * v[0]) +  (matrix[1*4+1] * v[1]) + (matrix[2*4+1] * v[2]) + matrix[3*4+1];
    t[2] = (matrix[0*4+2] * v[0]) +  (matrix[1*4+2] * v[1]) + (matrix[2*4+2] * v[2]) + matrix[3*4+2];
  }
  else
  {
    t[0] = v[0];
    t[1] = v[1];
    t[2] = v[2];
  }
}


static inline NxF32 * pushTri(NxF32 *dest,const NxF32 *p1,const NxF32 *p2,const NxF32 *p3)
{
  dest[0] = p1[0];
  dest[1] = p1[1];
  dest[2] = p1[2];

  dest[3] = p2[0];
  dest[4] = p2[1];
  dest[5] = p2[2];

  dest[6] = p3[0];
  dest[7] = p3[1];
  dest[8] = p3[2];

  dest+=9;

  return dest;
}



const NxF32 * createArrowHead(const NxF32 *p1,const NxF32 *p2,NxF32 arrowSize,NxU32 &tcount)
{

  const NxF32 MIN_SIZE = 0.000001f;

  static NxF32 triangles[2*24*9]; // reserve room for the maximum number of triangles

  NxF32 dir[3];

  dir[0] = p2[0] - p1[0];
  dir[1] = p2[1] - p1[1];
  dir[2] = p2[2] - p1[2];

  NxF32 mag = fm_normalize(dir);

  if ( mag < MIN_SIZE )
  {
    mag = MIN_SIZE;
    dir[0] = 0;
    dir[1] = 1;
    dir[2] = 0;
  }

  if ( arrowSize > (mag*0.3f) )
  {
    arrowSize = mag*0.3f;
  }

  NxF32 ref[3] = { 0, 1, 0 };

  NxF32 quat[4];

  fm_rotationArc(ref,dir,quat);

  NxF32 matrix[16];
  fm_quatToMatrix(quat,matrix);
  fm_setTranslation(p2,matrix);


  NxU32 pcount = 0;
  NxF32 points[24*3];
  NxF32 *dest = points;

  for (NxF32 a=30; a<=360; a+=30)
  {
    NxF32 r = a*FM_DEG_TO_RAD;
    NxF32 x = cosf(r)*arrowSize;
    NxF32 y = sinf(r)*arrowSize;

    dest[0] = x;
    dest[1] = -3*arrowSize;
    dest[2] = y;
    dest+=3;
    pcount++;
    assert( pcount < 24 ); // no increment smaller than 15 degrees, because that is all the room we reserved for...
  }

  NxF32 *prev = &points[(pcount-1)*3];
  NxF32 *p = points;
  NxF32 center[3] = { 0, -2.5f*arrowSize, 0 };
  NxF32 top[3]    = { 0, 0, 0 };

  NxF32 _center[3];
  NxF32 _top[3];

  fm_transform(matrix,center,_center);
  fm_transform(matrix,top,_top);

  dest = triangles;

  tcount = 0;

  for (NxU32 i=0; i<pcount; i++)
  {

    NxF32 _p[3];
    NxF32 _prev[3];
    fm_transform(matrix,p,_p);
    fm_transform(matrix,prev,_prev);

    dest = pushTri(dest,_p,_center,_prev);
    dest = pushTri(dest,_prev,_top,_p);

    tcount+=2;

    prev = p;
    p+=3;
  }

  return triangles;
}

