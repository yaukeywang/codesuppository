#ifndef QUAT_H

#define QUAT_H

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

#include <math.h>

#include "common/snippets/UserMemAlloc.h"
#include "vector.h"
#include "matrix.h"


#ifndef PI
#define PI 3.14159265358979323846264338327950288419716939937510f
#endif

// quaternions are always represent as type float.
/** Represents 3d rotations as a quaternion number.*/
class Quat
{
public:
	Quat(void)
	{
		q.Set(0,0,0);
		w = 1;
	};

	Quat(HeF32 x,HeF32 y,HeF32 z,HeF32 _w)
	{
		q.Set(x,y,z);
		w = _w;
	};

	void Set(HeF32 x,HeF32 y,HeF32 z,HeF32 _w)
	{
		q.Set(x,y,z);
		w = _w;
	};

	void Get(HeF32 *dest) const
	{
		dest[0] = q.x;
		dest[1] = q.y;
		dest[2] = q.z;
		dest[3] = w;
	};

	void Set(const HeF32 *quat)
	{
		q.x = quat[0];
		q.y = quat[1];
		q.z = quat[2];
		w    = quat[3];
	};


	void SetRotationAxis(HeF32 x,HeF32 y,HeF32 z)
	{
		q.Set(x,y,z);
	};

	void SetRotationAngle(HeF32 a)
	{
		w = a;
	};


	Quat(const HeF32 *qt)
	{
		q.x = qt[0];
		q.y = qt[1];
		q.z = qt[2];
		w   = qt[3];
	}

	void Identity(void)
	{
		q.Set(0,0,0);
		w = 1;
	}


	bool BinaryEqual(const Quat &quat) const
	{
		const HeI32 *p1 = (const HeI32 *) &quat.q.x;
		const HeI32 *p2 = (const HeI32 *) &q.x;
		if ( p1[0] == p2[0] && p1[1] == p2[1] && p1[2] == p2[2] && p1[3] == p2[3] ) return true;
		return false;
	}

	void  QuatToMatrix(MyMatrix &t) const;  // convert quaternion into rotation matrix.

	void QuatToEuler(HeF32 &roll, HeF32 &pitch, HeF32 &yaw) const;

	void  EulerToQuat(HeF32 roll,HeF32 pitch,HeF32 yaw); // convert euler angles to quaternion.

	void  EulerToQuat(const Vector3d<HeF32> &v)
	{
		EulerToQuat(v.x,v.y,v.z);
	};

	// convert angle/axis into quaternion, and return
	// rotation matrix.
	void AngleAxis(HeF32 angle,const Vector3d<HeF32> &axis)
	{
		HeF32 halftheta    = angle*0.5f;
		HeF32 sinHalfTheta = (HeF32)sin( halftheta );
		HeF32 cosHalfTheta = (HeF32)cos( halftheta );

		q.x = axis.x*sinHalfTheta;
		q.y = axis.y*sinHalfTheta;
		q.z = axis.z*sinHalfTheta;
 	  w   = cosHalfTheta;

	};

	Quat & operator-=(const Quat &a)
	{
		q.x-=a.q.x;
		q.y-=a.q.y;
		q.z-=a.q.z;
		w-=a.w;
		Normalize();
		return *this;
	};

	Quat & operator+=(const Quat &a) // += operator.
	{
		q.x+=a.q.x;
		q.y+=a.q.y;
		q.z+=a.q.z;
		w+=a.w;
		return *this;
	};

	// Multiplying two quaternions adds their rotations.
	Quat & operator*=(const Quat &a)
	{
		Quat r;
		r.q.x = w * a.q.x + q.x * a.w   + q.y * a.q.z - q.z * a.q.y;
		r.q.y = w * a.q.y + q.y * a.w   + q.z * a.q.x - q.x * a.q.z;
		r.q.z = w * a.q.z + q.z * a.w   + q.x * a.q.y - q.y * a.q.x;
		r.w   = w * a.w   - q.x * a.q.x - q.y * a.q.y - q.z * a.q.z;
		r.Normalize();
		*this = r;
		return *this;
	};

	bool operator==(const Quat &a) const
	{
		if ( a.q.x == q.x && a.q.y == q.y && a.q.z == q.z && a.w == w ) return true;
		return false;
	};

	bool operator!=(const Quat &a) const
	{
		if ( a.q.x != q.x || a.q.y != q.y || a.q.z != q.z || a.w != w ) return true;
		return false;
	};

	// Taking the reciprocal of a quaternion makes its rotation go the other way
	void  Reciprocal( void )
	{
		q.x = -q.x;
		q.y = -q.y;
		q.z = -q.z;
	}

	void Multiply(const Quat &a,const Quat &b)
	{
		q.x = b.w * a.q.x + b.q.x * a.w   + b.q.y * a.q.z - b.q.z * a.q.y;
		q.y = b.w * a.q.y + b.q.y * a.w   + b.q.z * a.q.x - b.q.x * a.q.z;
		q.z = b.w * a.q.z + b.q.z * a.w   + b.q.x * a.q.y - b.q.y * a.q.x;
		w   = b.w * a.w   - b.q.x * a.q.x - b.q.y * a.q.y - b.q.z * a.q.z;
		Normalize();
	}

	void getAxisAngle(Vector3d<HeF32> &axis,HeF32 &angle) const // returns result in *DEGREES*
	{
    angle = acosf(w) * 2.0f;		//this is getAngle()
    HeF32 sa = sqrtf(1.0f - w*w);
  	if (sa)
		{
	  	axis.Set(q.x/sa,q.y/sa,q.z/sa);
  		angle*=RAD_TO_DEG;
		}
  	else
  	{
  		axis.Set(1,0,0);
  	}
	}

	void  MatrixToQuat(const MyMatrix &t); // convert rotation matrix to quaternion.
	void  Slerp(const Quat &from,const Quat &to,HeF32 t); // smooth interpolation.
	void  Lerp(const Quat &from,const Quat &to,HeF32 t);  // fast interpolation, not as smooth.

	void QuaternionInterpolate(HeF32 c0,HeF32 c1,const Quat &quat)
	{
		// Find the cosine of the angle between the quaternions by
		// taking the inner product

		HeF32 CosineAngle = q.x * quat.q.x +
												q.y * quat.q.y +
												q.z * quat.q.z +
												w   * quat.w ;

		HeF32 Sign = 1.0f;

		if(CosineAngle < 0.0f)
		{
			CosineAngle = -CosineAngle;
			Sign = -Sign;
		}

		// TODO: Pick this constant
		HeF32 const SphericalLinearInterpolationThreshold = 0.0001f;

		if ( (1.0f - CosineAngle) > SphericalLinearInterpolationThreshold )
		{
			// TODO: Change this to use ATan2.
			// Fit for spherical interpolation
			HeF32 const Angle = acosf(CosineAngle);
			HeF32 const OneOverSinAngle = 1.0f / sinf(Angle);
			c0 = sinf(c0 * Angle) * Sign * OneOverSinAngle;
			c1 = sinf(c1 * Angle) * OneOverSinAngle;
		}
		else
		{
			c0 *= Sign;
		}
		q.x = q.x*c0 + quat.q.x * c1;
		q.y = q.y*c0 + quat.q.y * c1;
		q.z = q.z*c0 + quat.q.z * c1;
			w =   w*c0 + quat.w   * c1;

	}

	void NormalizeCloseToOne(void)
	{
		HeF32 sum = q.x * q.x + q.y * q.y + q.z * q.z + w * w;
		sum = (3.0f - sum ) * 0.5f; // this is an approximation
		q.x*=sum;
		q.y*=sum;
		q.z*=sum;
		w*=sum;
	}

	void  Normalize(void)  // normalize quaternion.
	{
		HeF32	dist, square;

		square = q.x * q.x + q.y * q.y + q.z * q.z + w * w;
		if (square > 0.0f)
			dist = (HeF32)(1.0f / sqrt(square));
		else
			dist = 1;

		q.x*= dist;
		q.y*= dist;
		q.z*= dist;
		w*= dist;
	};

	const HeF32 * GetFloat(void) const
	{
		return &q.x;
	};

	HeF32 * Ptr(void)
	{
		return &q.x;
	};

	void RotationArc(const Vector3d<HeF32>& v0, const Vector3d<HeF32>& v1)
	{
    Vector3d<HeF32> _v0 = v0;
    Vector3d<HeF32> _v1 = v1;
    _v0.Normalize();
    _v1.Normalize();
    HeF32 s = sqrtf((1.0f + (v0.Dot(v1))) * 2.0f);
    q = (_v0.Cross(_v1)) / s;
    w = s * 0.5f;
	}

	void RandomRotation(bool x,bool y,bool z);

	Vector3d<HeF32> q; // x/y/z components of quaternion.
	HeF32 w;        // w component of quaternion.
};


// quaternion must be normalized and matrix t in column-major format
inline void Quat::QuatToMatrix(MyMatrix &t) const
{
	HeF32 xx = q.x*q.x; 
	HeF32 yy = q.y*q.y; 
	HeF32 zz = q.z*q.z;
	HeF32 xy = q.x*q.y;
	HeF32 xz = q.x*q.z;
	HeF32 yz = q.y*q.z;
	HeF32 wx = w*q.x;
	HeF32 wy = w*q.y; 
	HeF32 wz = w*q.z;

	t.mElement[0][0] = 1 - 2 * ( yy + zz ); 
	t.mElement[1][0] =     2 * ( xy - wz );
	t.mElement[2][0] =     2 * ( xz + wy );

	t.mElement[0][1] =     2 * ( xy + wz );
	t.mElement[1][1] = 1 - 2 * ( xx + zz );
	t.mElement[2][1] =     2 * ( yz - wx );

	t.mElement[0][2] =     2 * ( xz - wy );
	t.mElement[1][2] =     2 * ( yz + wx );
	t.mElement[2][2] = 1 - 2 * ( xx + yy );

	t.mElement[3][0] = t.mElement[3][1] = t.mElement[3][2] = 0.0f;
	t.mElement[0][3] = t.mElement[1][3] = t.mElement[2][3] = 0.0f;
	t.mElement[3][3] = 1.0f;

}



void  inline Quat::MatrixToQuat(const MyMatrix &t)
{
	HeF32 tr = t.mElement[0][0] + t.mElement[1][1] + t.mElement[2][2];
	// check the diagonal

	if (tr > 0.0f )
	{
		HeF32 s = (HeF32) sqrt ( (HeF64) (tr + 1.0f) );
		w = s * 0.5f;
		s = 0.5f / s;
		q.x = (t.mElement[1][2] - t.mElement[2][1]) * s;
		q.y = (t.mElement[2][0] - t.mElement[0][2]) * s;
		q.z = (t.mElement[0][1] - t.mElement[1][0]) * s;

	}
	else
	{
		// diagonal is negative
		HeI32 nxt[3] = {1, 2, 0};
		HeF32  qa[4];

		HeI32 i = 0;

		if (t.mElement[1][1] > t.mElement[0][0]) i = 1;
		if (t.mElement[2][2] > t.mElement[i][i]) i = 2;

		HeI32 j = nxt[i];
		HeI32 k = nxt[j];

		HeF32 s = sqrtf ( ((t.mElement[i][i] - (t.mElement[j][j] + t.mElement[k][k])) + 1.0f) );

		qa[i] = s * 0.5f;

		if (s != 0.0f ) s = 0.5f / s;

		qa[3] = (t.mElement[j][k] - t.mElement[k][j]) * s;
		qa[j] = (t.mElement[i][j] + t.mElement[j][i]) * s;
		qa[k] = (t.mElement[i][k] + t.mElement[k][i]) * s;

		q.x = qa[0];
		q.y = qa[1];
		q.z = qa[2];
		w   = qa[3];
	}

//**  Normalize();

};

// build quaternion based on euler angles
inline void Quat::EulerToQuat(HeF32 roll,HeF32 pitch,HeF32 yaw)
{
	roll  *= 0.5f;
	pitch *= 0.5f;
	yaw   *= 0.5f;

	HeF32 cr = (HeF32)cos(roll);
	HeF32 cp = (HeF32)cos(pitch);
	HeF32 cy = (HeF32)cos(yaw);

	HeF32 sr = (HeF32)sin(roll);
	HeF32 sp = (HeF32)sin(pitch);
	HeF32 sy = (HeF32)sin(yaw);

	HeF32 cpcy = cp * cy;
	HeF32 spsy = sp * sy;
	HeF32 spcy = sp * cy;
	HeF32 cpsy = cp * sy;

	w   = cr * cpcy + sr * spsy;
	q.SetX( sr * cpcy - cr * spsy);
	q.SetY( cr * spcy + sr * cpsy);
	q.SetZ( cr * cpsy - sr * spcy);
};

inline void Quat::QuatToEuler( HeF32 &roll, HeF32 &pitch, HeF32 &yaw ) const
{
	HeF32 sint		= (2.0f * w * q.y) - (2.0f * q.x * q.z);
	HeF32 cost_temp = 1.0f - (sint * sint);
	HeF32 cost		= 0;

	if ( (HeF32)fabs(cost_temp) > 0.001f )
	{
		cost = sqrtf( cost_temp );
	}

	HeF32 sinv, cosv, sinf, cosf;
	if ( (HeF32)fabs(cost) > 0.001f )
	{
		sinv = ((2.0f * q.y * q.z) + (2.0f * w * q.x)) / cost;
		cosv = (1.0f - (2.0f * q.x * q.x) - (2.0f * q.y * q.y)) / cost;
		sinf = ((2.0f * q.x * q.y) + (2.0f * w * q.z)) / cost;
		cosf = (1.0f - (2.0f * q.y * q.y) - (2.0f * q.z * q.z)) / cost;
	}
	else
	{
		sinv = (2.0f * w * q.x) - (2.0f * q.y * q.z);
		cosv = 1.0f - (2.0f * q.x * q.x) - (2.0f * q.z * q.z);
		sinf = 0;
		cosf = 1.0f;
	}

	// compute output rotations
	roll	= atan2( sinv, cosv );
	pitch	= atan2( sint, cost );
	yaw		= atan2( sinf, cosf );
}

class QuatPos
{
public:
  Vector3d<HeF32> p;  // the translation
  HeF32           r;  // radius/scale
  Quat            q;  // the quaternion rotation
};

#endif
