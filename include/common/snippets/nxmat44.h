#ifndef NX_FOUNDATION_HeMat34T
#define NX_FOUNDATION_HeMat34T


#include "NxVec3.h"
#include "NxVec4.h"
#include "NxQuat.h"


class Nx16Real
{
public:
  union
	{
    struct {
      NxReal        _11, _12, _13, _14;
      NxReal        _21, _22, _23, _24;
      NxReal        _31, _32, _33, _34;
      NxReal        _41, _42, _43, _44;

		} s;
 		NxReal m[4][4];
    struct
    {
      NxReal rotation[12];
      NxReal x;
      NxReal y;
      NxReal z;
      NxReal w;
    } t;
    struct
    {
      NxReal m[16];
    } f;
	};
};

class NxMat44 : public Nx16Real
{
public:
	/**
	\brief by default M is inited and t isn't.  Use this ctor to either init or not init in full.
	*/
	NX_INLINE explicit NxMat44(bool init = true);

  NX_INLINE NxMat44(const NxReal transform[16]);

  NX_INLINE void get(NxReal *matrix);

	NX_INLINE void zero(void);

	NX_INLINE void id(void);

	//element access:
  NX_INLINE NxReal& operator()(NxI32 row, NxI32 col);
  NX_INLINE NxReal operator() (NxI32 row, NxI32 col) const;

	/**
	\brief returns true for identity matrix
	*/
	NX_INLINE bool isIdentity(void) const;

	/**
	\brief returns true if all elems are finite (not NAN or INF, etc.)
	*/
	NX_INLINE bool isFinite(void) const;

  enum AXIS_NUMBER
  {
    AXIS_X = 0,
    AXIS_Y,
    AXIS_Z,
    AXIS_POSITION
  };
  /**
  \brief returns an axis of the matrix as a NxVec3
  */
  NX_INLINE NxVec3 getAxis(AXIS_NUMBER row) const;

	/**
	\brief computes determinant.  

	Returns 0 if singular (i.e. if no inverse exists).
	*/
	NX_INLINE NxReal getDeterminant() const;

	/**
	\brief assigns inverse to dest.

	Returns false if singular (i.e. if no inverse exists), setting dest to identity.  dest may equal this.
	*/
	NX_INLINE bool getInverse(NxMat44& dest) const;

	/**
	\brief same as #getInverse(), but assumes that M is orthonormal
	*/
	NX_INLINE bool getInverseRT(NxMat44& dest) const;

	/**
	\brief assigns transpose to dest.

	dest may equal this.
	*/
	NX_INLINE void getTranspose(NxMat44& dest) const;

	/**
	\brief dst = this * src
	*/
	NX_INLINE void multiply(const NxVec3 &src, NxVec3 &dst) const;

	/**
	\brief dst = this * src
	*/
	NX_INLINE void multiply(const NxVec3 &src, NxVec4 &dst) const;

	/**
	\brief operator wrapper for multiply
	*/
	NX_INLINE NxVec3 operator*  (const NxVec3 & src) const { NxVec3 dest; multiply(src, dest); return dest; }
	/**
	\brief dst = inverse(this) * src	-- assumes M is rotation matrix!!!
	*/
	NX_INLINE void multiplyByInverseRT(const NxVec3 &src, NxVec3 &dst) const;

	/**
	\brief operator wrapper for multiplyByInverseRT
	*/
	NX_INLINE NxVec3 operator%  (const NxVec3 & src) const { NxVec3 dest; multiplyByInverseRT(src, dest); return dest; }

	/**
	\brief this = left * right
	*/
	NX_INLINE void multiply(const NxMat44& left, const NxMat44& right);

	/**
	\brief operator wrapper for multiply
	*/
	NX_INLINE NxMat44 operator*  (const NxMat44 & right) const { NxMat44 dest(false); dest.multiply(*this, right); return dest; }

  // Decompose a transform into position, scale, and euler rotation
  NX_INLINE void decompose(NxVec3 &position,NxVec3 &scale,NxVec3 &rotation) const;

  NX_INLINE void compose(const NxVec3 &position,const NxVec3 &scale,const NxVec3 &rotation,bool useDxYawPitchRoll);

	/**
	\brief this = rotation matrix around X axis

	<b>Unit:</b> Radians
	*/
	NX_INLINE void rotX(NxReal angle);

	/**
	\brief this = rotation matrix around Y axis

	<b>Unit:</b> Radians
	*/
	NX_INLINE void rotY(NxReal angle);

	/**
	\brief this = rotation matrix around Z axis

	<b>Unit:</b> Radians
	*/
	NX_INLINE void rotZ(NxReal angle);

  NX_INLINE void setRotation(const NxVec3 &rotation,bool useDXYawPitchRoll);
  NX_INLINE void setScale(const NxVec3 &scale);
  NX_INLINE void setPosition(const NxVec3 &position);

  NX_INLINE void setToPosition(const NxVec3 &position);

  NX_INLINE void fromQuat(const NxQuat &q);

  NX_INLINE void getSubMatrix(NxI32 row, NxI32 col, NxMat44& out_matrix) const;

  NX_INLINE const NxF32 * ptr(void) const
  {
    return &m[0][0];
  }

  NX_INLINE void set(const NxF32 *mat)
  {
    memcpy(&m[0][0],mat,sizeof(m));
  }

};


NX_INLINE NxMat44::NxMat44(bool init)
{
	if (init)
	{
    id();
	}
}


NX_INLINE void NxMat44::zero(void)
{
  memset(f.m,0,sizeof(NxReal)*16);
}


NX_INLINE NxReal & NxMat44::operator()(NxI32 row, NxI32 col)
{
	return m[col][row];
}


NX_INLINE NxReal NxMat44::operator() (NxI32 row, NxI32 col) const
{
	return m[col][row];
}


NX_INLINE void NxMat44::id(void)
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


NX_INLINE bool NxMat44::isIdentity(void) const
{
  bool ret = true;

  NxMat44 m(true);

  for (NxU32 i=0; i<16; i++)
  {
    if ( m.f.m[i] != f.m[i] )
    {
      ret = false;
      break;
    }
  }

	return ret;
}


NX_INLINE bool NxMat44::isFinite() const
{
  bool ret = true;

  for (NxU32 i=0; i<16; i++)
  {
    if ( !NxMath::isFinite(f.m[i]) )
    {
      ret = false;
      break;
    }
  }
	return ret;
}


NX_INLINE NxVec3 NxMat44::getAxis(AXIS_NUMBER row) const
{
  return NxVec3(this->m[row]);
}

NX_INLINE NxReal NxMat44::getDeterminant() const
{
  NxVec3 tmpv;
  NxVec3 p0( m[0][0], m[0][1], m[0][2] );
  NxVec3 p1( m[1][0], m[1][1], m[1][2] );
  NxVec3 p2( m[2][0], m[2][1], m[2][2] );
  tmpv.cross(p1, p2);
  return p0.dot(tmpv);
}

NX_INLINE void NxMat44::getSubMatrix(NxI32 row, NxI32 col, NxMat44& out_matrix) const
{
  NxI32 in_row, in_col;
  NxI32 out_row = 0, out_col = 0;

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

NX_INLINE bool NxMat44::getInverse(NxMat44& dest) const
{
  NxF32 determinant = getDeterminant();
  if (determinant < NxReal(0.0001))
  {
    dest.id();
    return false;
  }
  determinant = NxReal(1.0) / determinant;
  NxMat44 result;
  result.id();

  for (NxI32 i = 0; i < 4; ++i)
  {
    for (NxI32 j = 0; j < 4; ++j)
    {
      NxI32 sign = 1 - ((i+j)%2)*2; // alternates sign per entrace through the loop
      NxMat44 subMatrix (true);
      getSubMatrix(i,j,subMatrix);
      NxReal sub_determinant = subMatrix.getDeterminant();
      result.m[i][j] = ( sub_determinant * sign) * determinant;
    }
  }
  dest = result;
  return true;
}

//  void Invert(MyMatrix &invert) const
//  {
//    NxF32 determinant = getDeterminant();
//    NX_ASSERT( determinant > 0.0001f );
//    determinant = 1.0f / determinant;
//    for ( NxI32 i = 0; i < 4; i++ )
//    {
//      for ( NxI32 j = 0; j < 4; j++ )
//      {
//        NxI32 sign = 1 - ( ( i + j ) % 2 ) * 2;
//        MyMatrix subMat;
//        getSubMatrix( i, j, subMat );
//        NxF32 subDeterminant = subMat.getDeterminant();
//        invert.mElement[i][j] = ( subDeterminant * sign ) * determinant;
//      }
//    }
//  }
//  bool ret = false;
//
//  NX_ASSERT(0); // not yet implemented
//
//	return ret;
//}


NX_INLINE bool NxMat44::getInverseRT(NxMat44& /*dest*/) const
{
  bool ret = false;

  NX_ASSERT(0); // not yet implemented

  return ret;
}

NX_INLINE void NxMat44::getTranspose(NxMat44& dest) const
{
  NxMat44 nvro;
  for (NxI32 row = 0; row < 4; ++row)
    for (NxI32 col = 0; col < 4; ++col)
      nvro.m[row][col] = m[col][row];
  dest = nvro;
}

NX_INLINE void NxMat44::multiply(const NxVec3 &v, NxVec3 &dst) const
{
  NxVec3 t;
  t[0] = (f.m[0*4+0] * v[0]) +  (f.m[1*4+0] * v[1]) + (f.m[2*4+0] * v[2]) + f.m[3*4+0];
  t[1] = (f.m[0*4+1] * v[0]) +  (f.m[1*4+1] * v[1]) + (f.m[2*4+1] * v[2]) + f.m[3*4+1];
  t[2] = (f.m[0*4+2] * v[0]) +  (f.m[1*4+2] * v[1]) + (f.m[2*4+2] * v[2]) + f.m[3*4+2];
  dst = t;
}

NX_INLINE void NxMat44::multiply(const NxVec3 &v, NxVec4 &dst) const
{
  NxVec4 t;
  t[0] = (f.m[0*4+0] * v[0]) +  (f.m[1*4+0] * v[1]) + (f.m[2*4+0] * v[2]) + f.m[3*4+0];
  t[1] = (f.m[0*4+1] * v[0]) +  (f.m[1*4+1] * v[1]) + (f.m[2*4+1] * v[2]) + f.m[3*4+1];
  t[2] = (f.m[0*4+2] * v[0]) +  (f.m[1*4+2] * v[1]) + (f.m[2*4+2] * v[2]) + f.m[3*4+2];
  t[3] = (f.m[0*4+3] * v[0]) +  (f.m[1*4+3] * v[1]) + (f.m[2*4+3] * v[2]) + f.m[3*4+3];
  dst = t;
}

NX_INLINE void NxMat44::multiplyByInverseRT(const NxVec3 &src, NxVec3 &dst) const
{
	NxReal _x = src[0] - f.m[3*4+0];
	NxReal _y = src[1] - f.m[3*4+1];
	NxReal _z = src[2] - f.m[3*4+2];

	// Multiply inverse-translated source vector by inverted rotation transform

	dst.x = (f.m[0*4+0] * _x) + (f.m[0*4+1] * _y) + (f.m[0*4+2] * _z);
	dst.y = (f.m[1*4+0] * _x) + (f.m[1*4+1] * _y) + (f.m[1*4+2] * _z);
	dst.z = (f.m[2*4+0] * _x) + (f.m[2*4+1] * _y) + (f.m[2*4+2] * _z);
}


NX_INLINE void NxMat44::multiply(const NxMat44& left, const NxMat44& right)
{
  NxReal a = left.f.m[0*4+0] * right.f.m[0*4+0] + left.f.m[0*4+1] * right.f.m[1*4+0] + left.f.m[0*4+2] * right.f.m[2*4+0] + left.f.m[0*4+3] * right.f.m[3*4+0];
  NxReal b = left.f.m[0*4+0] * right.f.m[0*4+1] + left.f.m[0*4+1] * right.f.m[1*4+1] + left.f.m[0*4+2] * right.f.m[2*4+1] + left.f.m[0*4+3] * right.f.m[3*4+1];
  NxReal c = left.f.m[0*4+0] * right.f.m[0*4+2] + left.f.m[0*4+1] * right.f.m[1*4+2] + left.f.m[0*4+2] * right.f.m[2*4+2] + left.f.m[0*4+3] * right.f.m[3*4+2];
  NxReal d = left.f.m[0*4+0] * right.f.m[0*4+3] + left.f.m[0*4+1] * right.f.m[1*4+3] + left.f.m[0*4+2] * right.f.m[2*4+3] + left.f.m[0*4+3] * right.f.m[3*4+3];

  NxReal e = left.f.m[1*4+0] * right.f.m[0*4+0] + left.f.m[1*4+1] * right.f.m[1*4+0] + left.f.m[1*4+2] * right.f.m[2*4+0] + left.f.m[1*4+3] * right.f.m[3*4+0];
  NxReal ff = left.f.m[1*4+0] * right.f.m[0*4+1] + left.f.m[1*4+1] * right.f.m[1*4+1] + left.f.m[1*4+2] * right.f.m[2*4+1] + left.f.m[1*4+3] * right.f.m[3*4+1];
  NxReal g = left.f.m[1*4+0] * right.f.m[0*4+2] + left.f.m[1*4+1] * right.f.m[1*4+2] + left.f.m[1*4+2] * right.f.m[2*4+2] + left.f.m[1*4+3] * right.f.m[3*4+2];
  NxReal h = left.f.m[1*4+0] * right.f.m[0*4+3] + left.f.m[1*4+1] * right.f.m[1*4+3] + left.f.m[1*4+2] * right.f.m[2*4+3] + left.f.m[1*4+3] * right.f.m[3*4+3];

  NxReal i = left.f.m[2*4+0] * right.f.m[0*4+0] + left.f.m[2*4+1] * right.f.m[1*4+0] + left.f.m[2*4+2] * right.f.m[2*4+0] + left.f.m[2*4+3] * right.f.m[3*4+0];
  NxReal j = left.f.m[2*4+0] * right.f.m[0*4+1] + left.f.m[2*4+1] * right.f.m[1*4+1] + left.f.m[2*4+2] * right.f.m[2*4+1] + left.f.m[2*4+3] * right.f.m[3*4+1];
  NxReal k = left.f.m[2*4+0] * right.f.m[0*4+2] + left.f.m[2*4+1] * right.f.m[1*4+2] + left.f.m[2*4+2] * right.f.m[2*4+2] + left.f.m[2*4+3] * right.f.m[3*4+2];
  NxReal l = left.f.m[2*4+0] * right.f.m[0*4+3] + left.f.m[2*4+1] * right.f.m[1*4+3] + left.f.m[2*4+2] * right.f.m[2*4+3] + left.f.m[2*4+3] * right.f.m[3*4+3];

  NxReal mm = left.f.m[3*4+0] * right.f.m[0*4+0] + left.f.m[3*4+1] * right.f.m[1*4+0] + left.f.m[3*4+2] * right.f.m[2*4+0] + left.f.m[3*4+3] * right.f.m[3*4+0];
  NxReal n = left.f.m[3*4+0] * right.f.m[0*4+1] + left.f.m[3*4+1] * right.f.m[1*4+1] + left.f.m[3*4+2] * right.f.m[2*4+1] + left.f.m[3*4+3] * right.f.m[3*4+1];
  NxReal o = left.f.m[3*4+0] * right.f.m[0*4+2] + left.f.m[3*4+1] * right.f.m[1*4+2] + left.f.m[3*4+2] * right.f.m[2*4+2] + left.f.m[3*4+3] * right.f.m[3*4+2];
  NxReal p = left.f.m[3*4+0] * right.f.m[0*4+3] + left.f.m[3*4+1] * right.f.m[1*4+3] + left.f.m[3*4+2] * right.f.m[2*4+3] + left.f.m[3*4+3] * right.f.m[3*4+3];

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


NX_INLINE NxMat44::NxMat44(const NxReal transform[16])
{
  memcpy(f.m,transform,sizeof(f.m));
}

NX_INLINE void NxMat44::get(NxReal *matrix)
{
  memcpy(matrix,f.m,sizeof(f.m));
}

NX_INLINE void NxMat44::decompose(NxVec3 &position,NxVec3 &scale,NxVec3 &rotation) const
{
  #define squared(x) ((x)*(x))
  position.set(&t.x);

  scale.x = sqrtf(squared(s._11) + squared(s._12) + squared(s._13));
  scale.y = sqrtf(squared(s._21) + squared(s._22) + squared(s._23));
  scale.z = sqrtf(squared(s._31) + squared(s._32) + squared(s._33));

  NxMat44 m = *this;

  NxReal sx = 1.0f / scale.x;
  NxReal sy = 1.0f / scale.y;
  NxReal sz = 1.0f / scale.z;

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
  NxReal cy = sqrtf(squared(m.s._22) + squared(m.s._12));
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

NX_INLINE void NxMat44::rotX(NxReal angle)
{
	NxReal Cos = cosf(angle);
	NxReal Sin = sinf(angle);
	id();
	m[1][1] = m[2][2] = Cos;
	m[1][2] = Sin;
	m[2][1] = -Sin;
}

NX_INLINE void NxMat44::rotY(NxReal angle)
{
	NxReal Cos = cosf(angle);
	NxReal Sin = sinf(angle);
	id();
	m[0][0] = m[2][2] = Cos;
	m[0][2] = -Sin;
	m[2][0] = Sin;
}

NX_INLINE void NxMat44::rotZ(NxReal angle)
{
	NxReal Cos = cosf(angle);
	NxReal Sin = sinf(angle);
	id();
	m[0][0] = m[1][1] = Cos;
	m[0][1] = Sin;
	m[1][0] = -Sin;
}


NX_INLINE void NxMat44::compose(const NxVec3 &position,const NxVec3 &scale,const NxVec3 &rotation,bool useDXYawPitchRoll)
{
  NxMat44 s;
  s.setScale(scale);

  NxMat44 r;
  r.setRotation(rotation,useDXYawPitchRoll);

  multiply(s,r);

  t.x = position.x;
  t.y = position.y;
  t.z = position.z;
}

NX_INLINE void NxMat44::setRotation(const NxVec3 &rotation,bool useDXYawPitchRoll)
{
	assert(0);
	rotation;
	useDXYawPitchRoll;
//*** TODO!
/***
  NxQuat q;
  if ( useDXYawPitchRoll )
    q.fromYawPitchRollDX(rotation.y,rotation.x,rotation.z);
  else
    q.fromEuler(rotation);
  fromQuat(q);
***/
}

NX_INLINE void NxMat44::setScale(const NxVec3 &scale)
{
  id();
  m[0][0] = scale.x;
  m[1][1] = scale.y;
  m[2][2] = scale.z;
}

NX_INLINE void NxMat44::setPosition(const NxVec3 &position)
{
  id();
  t.x = position.x;
  t.y = position.y;
  t.z = position.z;
}

NX_INLINE void NxMat44::setToPosition(const NxVec3 &position)
{
  t.x = position.x;
  t.y = position.y;
  t.z = position.z;
}

NX_INLINE void NxMat44::fromQuat(const NxQuat &q)
{
	NxReal xx = q.x*q.x;
	NxReal yy = q.y*q.y;
	NxReal zz = q.z*q.z;
	NxReal xy = q.x*q.y;
	NxReal xz = q.x*q.z;
	NxReal yz = q.y*q.z;
	NxReal wx = q.w*q.x;
	NxReal wy = q.w*q.y;
	NxReal wz = q.w*q.z;

	f.m[0*4+0] = 1 - 2 * ( yy + zz );
	f.m[1*4+0] =     2 * ( xy - wz );
	f.m[2*4+0] =     2 * ( xz + wy );

	f.m[0*4+1] =     2 * ( xy + wz );
	f.m[1*4+1] = 1 - 2 * ( xx + zz );
	f.m[2*4+1] =     2 * ( yz - wx );

	f.m[0*4+2] =     2 * ( xz - wy );
	f.m[1*4+2] =     2 * ( yz + wx );
	f.m[2*4+2] = 1 - 2 * ( xx + yy );

	f.m[3*4+0] =f.m[3*4+1] = f.m[3*4+2] = (NxReal) 0.0f;
	f.m[0*4+3] =f.m[1*4+3] = f.m[2*4+3] = (NxReal) 0.0f;
	f.m[3*4+3] =(NxReal) 1.0f;

}

#endif
