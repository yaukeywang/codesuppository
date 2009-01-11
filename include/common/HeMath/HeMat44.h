#ifndef HE_FOUNDATION_HeMat34T
#define HE_FOUNDATION_HeMat34T


#include "HeVec3.h"
#include "HeVec4.h"
#include "HeQuat.h"


class He16Real
{
public:
  union
	{
    struct {
      HeReal        _11, _12, _13, _14;
      HeReal        _21, _22, _23, _24;
      HeReal        _31, _32, _33, _34;
      HeReal        _41, _42, _43, _44;

		} s;
 		HeReal m[4][4];
    struct
    {
      HeReal rotation[12];
      HeReal x;
      HeReal y;
      HeReal z;
      HeReal w;
    } t;
    struct
    {
      HeReal m[16];
    } f;
	};
};

class HeMat44 : public He16Real
{
public:
	/**
	\brief by default M is inited and t isn't.  Use this ctor to either init or not init in full.
	*/
	HE_INLINE explicit HeMat44(bool init = true);

  HE_INLINE HeMat44(const HeReal transform[16]);

  HE_INLINE void get(HeReal *matrix);

	HE_INLINE void zero(void);

	HE_INLINE void id(void);

	//element access:
  HE_INLINE HeReal& operator()(int row, int col);
  HE_INLINE HeReal operator() (int row, int col) const;

	/**
	\brief returns true for identity matrix
	*/
	HE_INLINE bool isIdentity(void) const;

	/**
	\brief returns true if all elems are finite (not NAN or INF, etc.)
	*/
	HE_INLINE bool isFinite(void) const;

  enum AXIS_NUMBER
  {
    AXIS_X = 0,
    AXIS_Y,
    AXIS_Z,
    AXIS_POSITION
  };
  /**
  \brief returns an axis of the matrix as a HeVec3
  */
  HE_INLINE HeVec3 getAxis(AXIS_NUMBER row) const;

	/**
	\brief computes determinant.  

	Returns 0 if singular (i.e. if no inverse exists).
	*/
	HE_INLINE HeReal getDeterminant() const;

	/**
	\brief assigns inverse to dest.

	Returns false if singular (i.e. if no inverse exists), setting dest to identity.  dest may equal this.
	*/
	HE_INLINE bool getInverse(HeMat44& dest) const;

	/**
	\brief same as #getInverse(), but assumes that M is orthonormal
	*/
	HE_INLINE bool getInverseRT(HeMat44& dest) const;

	/**
	\brief assigns transpose to dest.

	dest may equal this.
	*/
	HE_INLINE void getTranspose(HeMat44& dest) const;

	/**
	\brief dst = this * src
	*/
	HE_INLINE void multiply(const HeVec3 &src, HeVec3 &dst) const;

	/**
	\brief dst = this * src
	*/
	HE_INLINE void multiply(const HeVec3 &src, HeVec4 &dst) const;

	/**
	\brief operator wrapper for multiply
	*/
	HE_INLINE HeVec3 operator*  (const HeVec3 & src) const { HeVec3 dest; multiply(src, dest); return dest; }
	/**
	\brief dst = inverse(this) * src	-- assumes M is rotation matrix!!!
	*/
	HE_INLINE void multiplyByInverseRT(const HeVec3 &src, HeVec3 &dst) const;

	/**
	\brief operator wrapper for multiplyByInverseRT
	*/
	HE_INLINE HeVec3 operator%  (const HeVec3 & src) const { HeVec3 dest; multiplyByInverseRT(src, dest); return dest; }

	/**
	\brief this = left * right
	*/
	HE_INLINE void multiply(const HeMat44& left, const HeMat44& right);

	/**
	\brief operator wrapper for multiply
	*/
	HE_INLINE HeMat44 operator*  (const HeMat44 & right) const { HeMat44 dest(false); dest.multiply(*this, right); return dest; }

  // Decompose a transform into position, scale, and euler rotation
  HE_INLINE void decompose(HeVec3 &position,HeVec3 &scale,HeVec3 &rotation) const;

  HE_INLINE void compose(const HeVec3 &position,const HeVec3 &scale,const HeVec3 &rotation,bool useDxYawPitchRoll);

	/**
	\brief this = rotation matrix around X axis

	<b>Unit:</b> Radians
	*/
	HE_INLINE void rotX(HeReal angle);

	/**
	\brief this = rotation matrix around Y axis

	<b>Unit:</b> Radians
	*/
	HE_INLINE void rotY(HeReal angle);

	/**
	\brief this = rotation matrix around Z axis

	<b>Unit:</b> Radians
	*/
	HE_INLINE void rotZ(HeReal angle);

  HE_INLINE void setRotation(const HeVec3 &rotation,bool useDXYawPitchRoll);
  HE_INLINE void setScale(const HeVec3 &scale);
  HE_INLINE void setPosition(const HeVec3 &position);

  HE_INLINE void setToPosition(const HeVec3 &position);

  HE_INLINE void fromQuat(const HeQuat &q);

  HE_INLINE void getSubMatrix(int row, int col, HeMat44& out_matrix) const;

  HE_INLINE const HeF32 * ptr(void) const
  {
    return &m[0][0];
  }

  HE_INLINE void set(const HeF32 *mat)
  {
    memcpy(&m[0][0],mat,sizeof(m));
  }

};


HE_INLINE HeMat44::HeMat44(bool init)
{
	if (init)
	{
    id();
	}
}


HE_INLINE void HeMat44::zero(void)
{
  memset(f.m,0,sizeof(HeReal)*16);
}


HE_INLINE HeReal & HeMat44::operator()(int row, int col)
{
	return m[col][row];
}


HE_INLINE HeReal HeMat44::operator() (int row, int col) const
{
	return m[col][row];
}


HE_INLINE void HeMat44::id(void)
{
	f.m[0*4+0] = 1;
	f.m[1*4+1] = 1;
	f.m[2*4+2] = 1;
	f.m[3*4+3] = 1;

	f.m[1*4+0] = 0;
	f.m[2*4+0] = 0;
	f.m[3*4+0] = 0;

	f.m[0*4+1] = 0;
	f.m[2*4+1] = 0;
	f.m[3*4+1] = 0;

	f.m[0*4+2] = 0;
	f.m[1*4+2] = 0;
	f.m[3*4+2] = 0;

	f.m[0*4+3] = 0;
	f.m[1*4+3] = 0;
	f.m[2*4+3] = 0;
}


HE_INLINE bool HeMat44::isIdentity(void) const
{
  bool ret = true;

  HeMat44 m(true);

  for (HeU32 i=0; i<16; i++)
  {
    if ( m.f.m[i] != f.m[i] )
    {
      ret = false;
      break;
    }
  }

	return ret;
}


HE_INLINE bool HeMat44::isFinite() const
{
  bool ret = true;

  for (HeU32 i=0; i<16; i++)
  {
    if ( !HeMath::isFinite(f.m[i]) )
    {
      ret = false;
      break;
    }
  }
	return ret;
}


HE_INLINE HeVec3 HeMat44::getAxis(AXIS_NUMBER row) const
{
  return HeVec3(this->m[row]);
}

HE_INLINE HeReal HeMat44::getDeterminant() const
{
  HeVec3 tmpv;
  HeVec3 p0( m[0][0], m[0][1], m[0][2] );
  HeVec3 p1( m[1][0], m[1][1], m[1][2] );
  HeVec3 p2( m[2][0], m[2][1], m[2][2] );
  tmpv.cross(p1, p2);
  return p0.dot(tmpv);
}

HE_INLINE void HeMat44::getSubMatrix(int row, int col, HeMat44& out_matrix) const
{
  HeI32 in_row, in_col;
  HeI32 out_row = 0, out_col = 0;

  for ( in_col = 0; in_col < 4; in_col++ )
  {
    if ( in_col == col )
    {
      continue;
    }
    for ( out_row = 0, in_row = 0; in_row < 4; in_row++ )
    {
      if ( in_row == row )
      {
        continue;
      }
      out_matrix.m[out_col][out_row] = m[in_col][in_row];
      out_row++;
    }
    out_col++;
  }
}

HE_INLINE bool HeMat44::getInverse(HeMat44& dest) const
{
  HeF32 determinant = getDeterminant();
  if (determinant < HeReal(0.0001))
  {
    dest.id();
    return false;
  }
  determinant = HeReal(1.0) / determinant;
  HeMat44 result;
  result.id();

  for (HeI32 i = 0; i < 4; ++i)
  {
    for (HeI32 j = 0; j < 4; ++j)
    {
      HeI32 sign = 1 - ((i+j)%2)*2; // alternates sign per entrace through the loop
      HeMat44 subMatrix (true);
      getSubMatrix(i,j,subMatrix);
      HeReal sub_determinant = subMatrix.getDeterminant();
      result.m[i][j] = ( sub_determinant * sign) * determinant;
    }
  }
  dest = result;
  return true;
}

//  void Invert(MyMatrix &invert) const
//  {
//    HeF32 determinant = getDeterminant();
//    HE_ASSERT( determinant > 0.0001f );
//    determinant = 1.0f / determinant;
//    for ( HeI32 i = 0; i < 4; i++ )
//    {
//      for ( HeI32 j = 0; j < 4; j++ )
//      {
//        HeI32 sign = 1 - ( ( i + j ) % 2 ) * 2;
//        MyMatrix subMat;
//        getSubMatrix( i, j, subMat );
//        HeF32 subDeterminant = subMat.getDeterminant();
//        invert.mElement[i][j] = ( subDeterminant * sign ) * determinant;
//      }
//    }
//  }
//  bool ret = false;
//
//  HE_ASSERT(0); // not yet implemented
//
//	return ret;
//}


HE_INLINE bool HeMat44::getInverseRT(HeMat44& /*dest*/) const
{
  bool ret = false;

  HE_ASSERT(0); // not yet implemented

  return ret;
}

HE_INLINE void HeMat44::getTranspose(HeMat44& dest) const
{
  HeMat44 nvro;
  for (int row = 0; row < 4; ++row)
    for (int col = 0; col < 4; ++col)
      nvro.m[row][col] = m[col][row];
  dest = nvro;
}

HE_INLINE void HeMat44::multiply(const HeVec3 &v, HeVec3 &dst) const
{
  HeVec3 t;
  t[0] = (f.m[0*4+0] * v[0]) +  (f.m[1*4+0] * v[1]) + (f.m[2*4+0] * v[2]) + f.m[3*4+0];
  t[1] = (f.m[0*4+1] * v[0]) +  (f.m[1*4+1] * v[1]) + (f.m[2*4+1] * v[2]) + f.m[3*4+1];
  t[2] = (f.m[0*4+2] * v[0]) +  (f.m[1*4+2] * v[1]) + (f.m[2*4+2] * v[2]) + f.m[3*4+2];
  dst = t;
}

HE_INLINE void HeMat44::multiply(const HeVec3 &v, HeVec4 &dst) const
{
  HeVec4 t;
  t[0] = (f.m[0*4+0] * v[0]) +  (f.m[1*4+0] * v[1]) + (f.m[2*4+0] * v[2]) + f.m[3*4+0];
  t[1] = (f.m[0*4+1] * v[0]) +  (f.m[1*4+1] * v[1]) + (f.m[2*4+1] * v[2]) + f.m[3*4+1];
  t[2] = (f.m[0*4+2] * v[0]) +  (f.m[1*4+2] * v[1]) + (f.m[2*4+2] * v[2]) + f.m[3*4+2];
  t[3] = (f.m[0*4+3] * v[0]) +  (f.m[1*4+3] * v[1]) + (f.m[2*4+3] * v[2]) + f.m[3*4+3];
  dst = t;
}

HE_INLINE void HeMat44::multiplyByInverseRT(const HeVec3 &src, HeVec3 &dst) const
{
	HeReal _x = src[0] - f.m[3*4+0];
	HeReal _y = src[1] - f.m[3*4+1];
	HeReal _z = src[2] - f.m[3*4+2];

	// Multiply inverse-translated source vector by inverted rotation transform

	dst.x = (f.m[0*4+0] * _x) + (f.m[0*4+1] * _y) + (f.m[0*4+2] * _z);
	dst.y = (f.m[1*4+0] * _x) + (f.m[1*4+1] * _y) + (f.m[1*4+2] * _z);
	dst.z = (f.m[2*4+0] * _x) + (f.m[2*4+1] * _y) + (f.m[2*4+2] * _z);
}


HE_INLINE void HeMat44::multiply(const HeMat44& left, const HeMat44& right)
{
  HeReal a = left.f.m[0*4+0] * right.f.m[0*4+0] + left.f.m[0*4+1] * right.f.m[1*4+0] + left.f.m[0*4+2] * right.f.m[2*4+0] + left.f.m[0*4+3] * right.f.m[3*4+0];
  HeReal b = left.f.m[0*4+0] * right.f.m[0*4+1] + left.f.m[0*4+1] * right.f.m[1*4+1] + left.f.m[0*4+2] * right.f.m[2*4+1] + left.f.m[0*4+3] * right.f.m[3*4+1];
  HeReal c = left.f.m[0*4+0] * right.f.m[0*4+2] + left.f.m[0*4+1] * right.f.m[1*4+2] + left.f.m[0*4+2] * right.f.m[2*4+2] + left.f.m[0*4+3] * right.f.m[3*4+2];
  HeReal d = left.f.m[0*4+0] * right.f.m[0*4+3] + left.f.m[0*4+1] * right.f.m[1*4+3] + left.f.m[0*4+2] * right.f.m[2*4+3] + left.f.m[0*4+3] * right.f.m[3*4+3];

  HeReal e = left.f.m[1*4+0] * right.f.m[0*4+0] + left.f.m[1*4+1] * right.f.m[1*4+0] + left.f.m[1*4+2] * right.f.m[2*4+0] + left.f.m[1*4+3] * right.f.m[3*4+0];
  HeReal ff = left.f.m[1*4+0] * right.f.m[0*4+1] + left.f.m[1*4+1] * right.f.m[1*4+1] + left.f.m[1*4+2] * right.f.m[2*4+1] + left.f.m[1*4+3] * right.f.m[3*4+1];
  HeReal g = left.f.m[1*4+0] * right.f.m[0*4+2] + left.f.m[1*4+1] * right.f.m[1*4+2] + left.f.m[1*4+2] * right.f.m[2*4+2] + left.f.m[1*4+3] * right.f.m[3*4+2];
  HeReal h = left.f.m[1*4+0] * right.f.m[0*4+3] + left.f.m[1*4+1] * right.f.m[1*4+3] + left.f.m[1*4+2] * right.f.m[2*4+3] + left.f.m[1*4+3] * right.f.m[3*4+3];

  HeReal i = left.f.m[2*4+0] * right.f.m[0*4+0] + left.f.m[2*4+1] * right.f.m[1*4+0] + left.f.m[2*4+2] * right.f.m[2*4+0] + left.f.m[2*4+3] * right.f.m[3*4+0];
  HeReal j = left.f.m[2*4+0] * right.f.m[0*4+1] + left.f.m[2*4+1] * right.f.m[1*4+1] + left.f.m[2*4+2] * right.f.m[2*4+1] + left.f.m[2*4+3] * right.f.m[3*4+1];
  HeReal k = left.f.m[2*4+0] * right.f.m[0*4+2] + left.f.m[2*4+1] * right.f.m[1*4+2] + left.f.m[2*4+2] * right.f.m[2*4+2] + left.f.m[2*4+3] * right.f.m[3*4+2];
  HeReal l = left.f.m[2*4+0] * right.f.m[0*4+3] + left.f.m[2*4+1] * right.f.m[1*4+3] + left.f.m[2*4+2] * right.f.m[2*4+3] + left.f.m[2*4+3] * right.f.m[3*4+3];

  HeReal mm = left.f.m[3*4+0] * right.f.m[0*4+0] + left.f.m[3*4+1] * right.f.m[1*4+0] + left.f.m[3*4+2] * right.f.m[2*4+0] + left.f.m[3*4+3] * right.f.m[3*4+0];
  HeReal n = left.f.m[3*4+0] * right.f.m[0*4+1] + left.f.m[3*4+1] * right.f.m[1*4+1] + left.f.m[3*4+2] * right.f.m[2*4+1] + left.f.m[3*4+3] * right.f.m[3*4+1];
  HeReal o = left.f.m[3*4+0] * right.f.m[0*4+2] + left.f.m[3*4+1] * right.f.m[1*4+2] + left.f.m[3*4+2] * right.f.m[2*4+2] + left.f.m[3*4+3] * right.f.m[3*4+2];
  HeReal p = left.f.m[3*4+0] * right.f.m[0*4+3] + left.f.m[3*4+1] * right.f.m[1*4+3] + left.f.m[3*4+2] * right.f.m[2*4+3] + left.f.m[3*4+3] * right.f.m[3*4+3];

  f.m[0] = a;
  f.m[1] = b;
  f.m[2] = c;
  f.m[3] = d;

  f.m[4] = e;
  f.m[5] = ff;
  f.m[6] = g;
  f.m[7] = h;

  f.m[8] = i;
  f.m[9] = j;
  f.m[10] = k;
  f.m[11] = l;

  f.m[12] = mm;
  f.m[13] = n;
  f.m[14] = o;
  f.m[15] = p;

}


HE_INLINE HeMat44::HeMat44(const HeReal transform[16])
{
  memcpy(f.m,transform,sizeof(f.m));
}

HE_INLINE void HeMat44::get(HeReal *matrix)
{
  memcpy(matrix,f.m,sizeof(f.m));
}

HE_INLINE void HeMat44::decompose(HeVec3 &position,HeVec3 &scale,HeVec3 &rotation) const
{
  #define squared(x) ((x)*(x))
  position.set(&t.x);

  scale.x = sqrtf(squared(s._11) + squared(s._12) + squared(s._13));
  scale.y = sqrtf(squared(s._21) + squared(s._22) + squared(s._23));
  scale.z = sqrtf(squared(s._31) + squared(s._32) + squared(s._33));

  HeMat44 m = *this;

  HeReal sx = 1.0f / scale.x;
  HeReal sy = 1.0f / scale.y;
  HeReal sz = 1.0f / scale.z;

  m.s._11*=sx;
  m.s._12*=sx;
  m.s._13*=sx;

  m.s._21*=sy;
  m.s._22*=sy;
  m.s._23*=sy;

  m.s._31*=sz;
  m.s._32*=sz;
  m.s._33*=sz;

  // Decompose Rotation
  // -- Euler Angle decomposition

  // cosine Y = cy
  HeReal cy = sqrtf(squared(m.s._22) + squared(m.s._12));
  rotation.x = atan2f(-m.s._32, cy);
  if (cy > 16*FLT_EPSILON)
  {
    rotation.y = atan2f(m.s._31, m.s._33);
    rotation.z = atan2f(m.s._12, m.s._22);
  }
  else
  {
    rotation.y = atan2f(-m.s._13, m.s._11);
    rotation.z = 0;
  }
}

HE_INLINE void HeMat44::rotX(HeReal angle)
{
	HeReal Cos = cosf(angle);
	HeReal Sin = sinf(angle);
	id();
	m[1][1] = m[2][2] = Cos;
	m[1][2] = Sin;
	m[2][1] = -Sin;
}

HE_INLINE void HeMat44::rotY(HeReal angle)
{
	HeReal Cos = cosf(angle);
	HeReal Sin = sinf(angle);
	id();
	m[0][0] = m[2][2] = Cos;
	m[0][2] = -Sin;
	m[2][0] = Sin;
}

HE_INLINE void HeMat44::rotZ(HeReal angle)
{
	HeReal Cos = cosf(angle);
	HeReal Sin = sinf(angle);
	id();
	m[0][0] = m[1][1] = Cos;
	m[0][1] = Sin;
	m[1][0] = -Sin;
}


HE_INLINE void HeMat44::compose(const HeVec3 &position,const HeVec3 &scale,const HeVec3 &rotation,bool useDXYawPitchRoll)
{
  HeMat44 s;
  s.setScale(scale);

  HeMat44 r;
  r.setRotation(rotation,useDXYawPitchRoll);

  multiply(s,r);

  t.x = position.x;
  t.y = position.y;
  t.z = position.z;
}

HE_INLINE void HeMat44::setRotation(const HeVec3 &rotation,bool useDXYawPitchRoll)
{
  HeQuat q;
  if ( useDXYawPitchRoll )
    q.fromYawPitchRollDX(rotation.y,rotation.x,rotation.z);
  else
    q.fromEuler(rotation);
  fromQuat(q);
}

HE_INLINE void HeMat44::setScale(const HeVec3 &scale)
{
  id();
  m[0][0] = scale.x;
  m[1][1] = scale.y;
  m[2][2] = scale.z;
}

HE_INLINE void HeMat44::setPosition(const HeVec3 &position)
{
  id();
  t.x = position.x;
  t.y = position.y;
  t.z = position.z;
}

HE_INLINE void HeMat44::setToPosition(const HeVec3 &position)
{
  t.x = position.x;
  t.y = position.y;
  t.z = position.z;
}

HE_INLINE void HeMat44::fromQuat(const HeQuat &q)
{
	HeReal xx = q.x*q.x;
	HeReal yy = q.y*q.y;
	HeReal zz = q.z*q.z;
	HeReal xy = q.x*q.y;
	HeReal xz = q.x*q.z;
	HeReal yz = q.y*q.z;
	HeReal wx = q.w*q.x;
	HeReal wy = q.w*q.y;
	HeReal wz = q.w*q.z;

	f.m[0*4+0] = 1 - 2 * ( yy + zz );
	f.m[1*4+0] =     2 * ( xy - wz );
	f.m[2*4+0] =     2 * ( xz + wy );

	f.m[0*4+1] =     2 * ( xy + wz );
	f.m[1*4+1] = 1 - 2 * ( xx + zz );
	f.m[2*4+1] =     2 * ( yz - wx );

	f.m[0*4+2] =     2 * ( xz - wy );
	f.m[1*4+2] =     2 * ( yz + wx );
	f.m[2*4+2] = 1 - 2 * ( xx + yy );

	f.m[3*4+0] =f.m[3*4+1] = f.m[3*4+2] = (HeReal) 0.0f;
	f.m[0*4+3] =f.m[1*4+3] = f.m[2*4+3] = (HeReal) 0.0f;
	f.m[3*4+3] =(HeReal) 1.0f;

}

#endif
