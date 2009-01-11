#ifndef HE_FOUNDATION_HeQuatT
#define HE_FOUNDATION_HeQuatT

#include "Hef.h"
#include "HeVec3.h"

/**
\brief This is a quaternion class. For more information on quaternion mathematics
consult a mathematics source on complex numbers.
 
*/

class HeQuat
	{
	public:
	/**
	\brief Default constructor, does not do any initialization.
	*/
	HE_INLINE HeQuat();

	/**
	\brief Copy constructor.
	*/
	HE_INLINE HeQuat(const HeQuat&);

	/**
	\brief copies xyz elements from v, and scalar from w (defaults to 0).
	*/
	HE_INLINE HeQuat(const HeVec3& v, HeReal w = 0);

	/**
	\brief creates from angle-axis representation.

	note that if Angle > 360 the resulting rotation is Angle mod 360.
	
	<b>Unit:</b> Degrees
	*/
	HE_INLINE HeQuat(const HeReal angle, const HeVec3 & axis);

	/**
	\brief Creates from orientation matrix.

	\param[in] m Rotation matrix to extract quaternion from.
	*/
	HE_INLINE HeQuat(const class HeMat44 &m); /* defined in HeMat44.h */


	/**
	\brief Set the quaternion to the identity rotation.
	*/
	HE_INLINE void id();

	/**
	\brief Test if the quaternion is the identity rotation.
	*/
	HE_INLINE bool isIdentityRotation() const;

	//setting:

	/**
	\brief Set the members of the quaternion, in order WXYZ
	*/
	HE_INLINE void setWXYZ(HeReal w, HeReal x, HeReal y, HeReal z);

	/**
	\brief Set the members of the quaternion, in order XYZW
	*/
	HE_INLINE void setXYZW(HeReal x, HeReal y, HeReal z, HeReal w);

	/**
	\brief Set the members of the quaternion, in order WXYZ
	*/
	HE_INLINE void setWXYZ(const HeReal *);

	/**
	\brief Set the members of the quaternion, in order XYZW
	*/
	HE_INLINE void setXYZW(const HeReal *);

	HE_INLINE HeQuat& operator=  (const HeQuat&);

	/**
	\brief Implicitly extends vector by a 0 w element.
	*/
	HE_INLINE HeQuat& operator=  (const HeVec3&);

	HE_INLINE void setx(const HeReal& d);
	HE_INLINE void sety(const HeReal& d);
	HE_INLINE void setz(const HeReal& d);
	HE_INLINE void setw(const HeReal& d);

	HE_INLINE void getWXYZ(HeF32 *) const;
	HE_INLINE void getXYZW(HeF32 *) const;

	HE_INLINE void getWXYZ(HeF64 *) const;
	HE_INLINE void getXYZW(HeF64 *) const;

	/**
	\brief returns true if all elements are finite (not NAN or INF, etc.)
	*/
	HE_INLINE bool isFinite() const;

	/**
	\brief sets to the quat [0,0,0,1]
	*/
	HE_INLINE void zero();

	/**
	\brief creates a random unit quaternion.
	*/
	HE_INLINE void random();
	/**
	\brief creates from angle-axis representation.

	Note that if Angle > 360 the resulting rotation is Angle mod 360.
	
	<b>Unit:</b> Degrees
	*/
	HE_INLINE void fromAngleAxis(HeReal angle, const HeVec3 & axis);

	/**
	\brief Creates from angle-axis representation.

	Axis must be normalized!

	<b>Unit:</b> Radians
	*/
	HE_INLINE void fromAngleAxisFast(HeReal AngleRadians, const HeVec3 & axis);

	/**
	\brief Sets this to the opposite rotation of this.
	*/
	HE_INLINE void invert();

  HE_INLINE void fromEuler(const HeVec3 &angles);

  HE_INLINE void getEuler(HeVec3 &angles);

  HE_INLINE void fromYawPitchRollDX(HeReal yaw,HeReal pitch,HeReal roll);

	/**
	\brief Fetches the Angle/axis given by the HeQuat.

	<b>Unit:</b> Degrees
	*/
	HE_INLINE void getAngleAxis(HeReal& Angle, HeVec3 & axis) const;

	/**
	\brief Gets the angle between this quat and the identity quaternion.

	<b>Unit:</b> Degrees
	*/
	HE_INLINE HeReal getAngle() const;

	/**
	\brief Gets the angle between this quat and the argument

	<b>Unit:</b> Degrees
	*/
	HE_INLINE HeReal getAngle(const HeQuat &) const;

	/**
	\brief This is the squared 4D vector length, should be 1 for unit quaternions.
	*/
	HE_INLINE HeReal magnitudeSquared() const;

	/**
	\brief returns the scalar product of this and other.
	*/
	HE_INLINE HeReal dot(const HeQuat &other) const;

	//modifiers:
	/**
	\brief maps to the closest unit quaternion.
	*/
	HE_INLINE void normalize();

	/*
	\brief assigns its own conjugate to itself.

	\note for unit quaternions, this is the inverse.
	*/
	HE_INLINE void conjugate();

	/**
	this = a * b
	*/
	HE_INLINE void multiply(const HeQuat& a, const HeQuat& b);

	/**
	this = a * v
	v is interpreted as quat [xyz0]
	*/
	HE_INLINE void multiply(const HeQuat& a, const HeVec3& v);

	/**
	this = slerp(t, a, b)
	*/
	HE_INLINE void slerp(const HeReal t, const HeQuat& a, const HeQuat& b);

  HE_INLINE const HeReal * ptr(void) const { return &x; };

	/**
	rotates passed vec by rot expressed by unit quaternion.  overwrites arg with the result.
	*/
	HE_INLINE void rotate(HeVec3 &) const;

	/**
	rotates passed vec by this (assumed unitary)
	*/
	HE_INLINE const HeVec3 rot(const HeVec3 &) const;

	/**
	inverse rotates passed vec by this (assumed unitary)
	*/
	HE_INLINE const HeVec3 invRot(const HeVec3 &) const;

	/**
	transform passed vec by this rotation (assumed unitary) and translation p
	*/
	HE_INLINE const HeVec3 transform(const HeVec3 &v, const HeVec3 &p) const;

	/**
	inverse rotates passed vec by this (assumed unitary)
	*/
	HE_INLINE const HeVec3 invTransform(const HeVec3 &v, const HeVec3 &p) const;


	/**
	rotates passed vec by opposite of rot expressed by unit quaternion.  overwrites arg with the result.
	*/
	HE_INLINE void inverseRotate(HeVec3 &) const;



	/**
	negates all the elements of the quat.  q and -q represent the same rotation.
	*/
	HE_INLINE void negate();
	HE_INLINE HeQuat operator -() const;

	HE_INLINE HeQuat& operator*= (const HeQuat&);
	HE_INLINE HeQuat& operator+= (const HeQuat&);
	HE_INLINE HeQuat& operator-= (const HeQuat&);
	HE_INLINE HeQuat& operator*= (const HeReal s);

	/** the quaternion elements */
    HeReal x,y,z,w;

	/** quaternion multiplication */
	HE_INLINE HeQuat operator *(const HeQuat &) const;

	/** quaternion addition */
	HE_INLINE HeQuat operator +(const HeQuat &) const;

	/** quaternion subtraction */
	HE_INLINE HeQuat operator -(const HeQuat &) const;

	/** quaternion conjugate */
	HE_INLINE HeQuat operator !() const;

    /*
	ops we decided not to implement:
	bool  operator== (const HeQuat&) const;
	HeVec3  operator^  (const HeQuat& r_h_s) const;//same as normal quat rot, but casts itself into a vector.  (doesn't compute w term)
	HeQuat  operator*  (const HeVec3& v) const;//implicitly extends vector by a 0 w element.
	HeQuat  operator*  (const HeReal Scale) const;
	*/

	friend class HeMat44;
	private:
		HE_INLINE HeQuat(HeReal ix, HeReal iy, HeReal iz, HeReal iw);
	};




HE_INLINE HeQuat::HeQuat()
	{
	//nothing
	}


HE_INLINE HeQuat::HeQuat(const HeQuat& q) : x(q.x), y(q.y), z(q.z), w(q.w)
	{
	}


HE_INLINE HeQuat::HeQuat(const HeVec3& v, HeReal s)						// copy constructor, assumes w=0 
	{
	x = v.x;
	y = v.y;
	z = v.z;
	w = s;
	}


HE_INLINE HeQuat::HeQuat(const HeReal angle, const HeVec3 & axis)				// creates a HeQuat from an Angle axis -- note that if Angle > 360 the resulting rotation is Angle mod 360
	{
	fromAngleAxis(angle,axis);
	}


HE_INLINE void HeQuat::id()
	{
	x = HeReal(0);
	y = HeReal(0);
	z = HeReal(0);
	w = HeReal(1);
	}

HE_INLINE  bool HeQuat::isIdentityRotation() const
{
	return x==0 && y==0 && z==0 && fabsf(w)==1;
}


HE_INLINE void HeQuat::setWXYZ(HeReal sw, HeReal sx, HeReal sy, HeReal sz)
	{
	x = sx;
	y = sy;
	z = sz;
	w = sw;
	}


HE_INLINE void HeQuat::setXYZW(HeReal sx, HeReal sy, HeReal sz, HeReal sw)
	{
	x = sx;
	y = sy;
	z = sz;
	w = sw;
	}


HE_INLINE void HeQuat::setWXYZ(const HeReal * d)
	{
	x = d[1];
	y = d[2];
	z = d[3];
	w = d[0];
	}


HE_INLINE void HeQuat::setXYZW(const HeReal * d)
	{
	x = d[0];
	y = d[1];
	z = d[2];
	w = d[3];
	}


HE_INLINE void HeQuat::getWXYZ(HeF32 *d) const
	{
	d[1] = (HeF32)x;
	d[2] = (HeF32)y;
	d[3] = (HeF32)z;
	d[0] = (HeF32)w;
	}


HE_INLINE void HeQuat::getXYZW(HeF32 *d) const
	{
	d[0] = (HeF32)x;
	d[1] = (HeF32)y;
	d[2] = (HeF32)z;
	d[3] = (HeF32)w;
	}


HE_INLINE void HeQuat::getWXYZ(HeF64 *d) const
	{
	d[1] = (HeF64)x;
	d[2] = (HeF64)y;
	d[3] = (HeF64)z;
	d[0] = (HeF64)w;
	}


HE_INLINE void HeQuat::getXYZW(HeF64 *d) const
	{
	d[0] = (HeF64)x;
	d[1] = (HeF64)y;
	d[2] = (HeF64)z;
	d[3] = (HeF64)w;
	}

//const methods
 
HE_INLINE bool HeQuat::isFinite() const
	{
	return HeMath::isFinite(x) 
		&& HeMath::isFinite(y) 
		&& HeMath::isFinite(z)
		&& HeMath::isFinite(w);
	}



HE_INLINE void HeQuat::zero()
	{
	x = HeReal(0.0);
	y = HeReal(0.0);
	z = HeReal(0.0);
	w = HeReal(1.0);
	}


HE_INLINE void HeQuat::negate()
	{
	x = -x;
	y = -y;
	z = -z;
	w = -w;
	}

HE_INLINE HeQuat HeQuat::operator-() const
	{
	return HeQuat(-x,-y,-z,-w);
	}


HE_INLINE void HeQuat::random()
	{
	x = HeMath::rand(HeReal(0.0),HeReal(1.0));
	y = HeMath::rand(HeReal(0.0),HeReal(1.0));
	z = HeMath::rand(HeReal(0.0),HeReal(1.0));
	w = HeMath::rand(HeReal(0.0),HeReal(1.0));
	normalize();
	}


HE_INLINE void HeQuat::fromAngleAxis(HeReal Angle, const HeVec3 & axis)			// set the HeQuat by Angle-axis (see AA constructor)
	{
	x = axis.x;
	y = axis.y;
	z = axis.z;

	// required: Normalize the axis

	const HeReal i_length =  HeReal(1.0) / HeMath::sqrt( x*x + y*y + z*z );

	x = x * i_length;
	y = y * i_length;
	z = z * i_length;

	// now make a clQuaternionernion out of it
	HeReal Half = HeMath::degToRad(Angle * HeReal(0.5));

	w = HeMath::cos(Half);//this used to be w/o deg to rad.
	const HeReal sin_theta_over_two = HeMath::sin(Half );
	x = x * sin_theta_over_two;
	y = y * sin_theta_over_two;
	z = z * sin_theta_over_two;
	}

HE_INLINE void HeQuat::fromAngleAxisFast(HeReal AngleRadians, const HeVec3 & axis)
	{
	HeReal s;
	HeMath::sinCos(AngleRadians * 0.5f, s, w);
	x = axis.x * s;
	y = axis.y * s;
	z = axis.z * s;
	}

HE_INLINE void HeQuat::invert()
	{
	x = -x;
	y = -y;
	z = -z;
	}

HE_INLINE void HeQuat::setx(const HeReal& d) 
	{ 
	x = d;
	}


HE_INLINE void HeQuat::sety(const HeReal& d) 
	{ 
	y = d;
	}


HE_INLINE void HeQuat::setz(const HeReal& d) 
	{ 
	z = d;
	}


HE_INLINE void HeQuat::setw(const HeReal& d) 
	{ 
	w = d;
	}


HE_INLINE void HeQuat::getAngleAxis(HeReal& angle, HeVec3 & axis) const
	{
	//return axis and angle of rotation of quaternion
    angle = HeMath::acos(w) * HeReal(2.0);		//this is getAngle()
    HeReal sa = HeMath::sqrt(HeReal(1.0) - w*w);
	if (sa)
		{
		axis.set(x/sa,y/sa,z/sa);
		angle = HeMath::radToDeg(angle);
		}
	else
		axis.set(HeReal(1.0),HeReal(0.0),HeReal(0.0));

	}



HE_INLINE HeReal HeQuat::getAngle() const
	{
	return HeMath::acos(w) * HeReal(2.0);
	}



HE_INLINE HeReal HeQuat::getAngle(const HeQuat & q) const
	{
	return HeMath::acos(dot(q)) * HeReal(2.0);
	}


HE_INLINE HeReal HeQuat::magnitudeSquared() const

//modifiers:
	{
	return x*x + y*y + z*z + w*w;
	}


HE_INLINE HeReal HeQuat::dot(const HeQuat &v) const
	{
	return x * v.x + y * v.y + z * v.z  + w * v.w;
	}


HE_INLINE void HeQuat::normalize()											// convert this HeQuat to a unit clQuaternionernion
	{
	const HeReal mag = HeMath::sqrt(magnitudeSquared());
	if (mag)
		{
		const HeReal imag = HeReal(1.0) / mag;
		
		x *= imag;
		y *= imag;
		z *= imag;
		w *= imag;
		}
	}


HE_INLINE void HeQuat::conjugate()											// convert this HeQuat to a unit clQuaternionernion
	{
	x = -x;
	y = -y;
	z = -z;
	}


HE_INLINE void HeQuat::multiply(const HeQuat& left, const HeQuat& right)		// this = a * b
	{
	HeReal a,b,c,d;

	a =left.w*right.w - left.x*right.x - left.y*right.y - left.z*right.z;
	b =left.w*right.x + right.w*left.x + left.y*right.z - right.y*left.z;
	c =left.w*right.y + right.w*left.y + left.z*right.x - right.z*left.x;
	d =left.w*right.z + right.w*left.z + left.x*right.y - right.x*left.y;

	w = a;
	x = b;
	y = c;
	z = d;
	}


HE_INLINE void HeQuat::multiply(const HeQuat& left, const HeVec3& right)		// this = a * b
	{
	HeReal a,b,c,d;

	a = - left.x*right.x - left.y*right.y - left.z *right.z;
	b =   left.w*right.x + left.y*right.z - right.y*left.z;
	c =   left.w*right.y + left.z*right.x - right.z*left.x;
	d =   left.w*right.z + left.x*right.y - right.x*left.y;

	w = a;
	x = b;
	y = c;
	z = d;
	}

HE_INLINE void HeQuat::slerp(const HeReal t, const HeQuat& left, const HeQuat& right) // this = slerp(t, a, b)
	{
	const HeReal	quatEpsilon = (HeReal(1.0e-8f));

	*this = left;

	HeReal cosine = 
		x * right.x + 
		y * right.y + 
		z * right.z + 
		w * right.w;		//this is left.dot(right)

	HeReal sign = HeReal(1);
	if (cosine < 0)
		{
		cosine = - cosine;
		sign = HeReal(-1);
		}

	HeReal Sin = HeReal(1) - cosine*cosine;

	if(Sin>=quatEpsilon*quatEpsilon)	
		{
		Sin = HeMath::sqrt(Sin);
		const HeReal angle = HeMath::atan2(Sin, cosine);
		const HeReal i_sin_angle = HeReal(1) / Sin;



		HeReal lower_weight = HeMath::sin(angle*(HeReal(1)-t)) * i_sin_angle;
		HeReal upper_weight = HeMath::sin(angle * t) * i_sin_angle * sign;

		w = (w * (lower_weight)) + (right.w * (upper_weight));
		x = (x * (lower_weight)) + (right.x * (upper_weight));
		y = (y * (lower_weight)) + (right.y * (upper_weight));
		z = (z * (lower_weight)) + (right.z * (upper_weight));
		}
	}


HE_INLINE void HeQuat::rotate(HeVec3 & v) const						//rotates passed vec by rot expressed by quaternion.  overwrites arg ith the result.
	{
	//HeReal msq = HeReal(1.0)/magnitudeSquared();	//assume unit quat!
	HeQuat myInverse;
	myInverse.x = -x;//*msq;
	myInverse.y = -y;//*msq;
	myInverse.z = -z;//*msq;
	myInverse.w =  w;//*msq;

	//v = ((*this) * v) ^ myInverse;

	HeQuat left;
	left.multiply(*this,v);
	v.x =left.w*myInverse.x + myInverse.w*left.x + left.y*myInverse.z - myInverse.y*left.z;
	v.y =left.w*myInverse.y + myInverse.w*left.y + left.z*myInverse.x - myInverse.z*left.x;
	v.z =left.w*myInverse.z + myInverse.w*left.z + left.x*myInverse.y - myInverse.x*left.y;
	}


HE_INLINE void HeQuat::inverseRotate(HeVec3 & v) const				//rotates passed vec by opposite of rot expressed by quaternion.  overwrites arg ith the result.
	{
	//HeReal msq = HeReal(1.0)/magnitudeSquared();	//assume unit quat!
	HeQuat myInverse;
	myInverse.x = -x;//*msq;
	myInverse.y = -y;//*msq;
	myInverse.z = -z;//*msq;
	myInverse.w =  w;//*msq;

	//v = (myInverse * v) ^ (*this);
	HeQuat left;
	left.multiply(myInverse,v);
	v.x =left.w*x + w*left.x + left.y*z - y*left.z;
	v.y =left.w*y + w*left.y + left.z*x - z*left.x;
	v.z =left.w*z + w*left.z + left.x*y - x*left.y;
	}


HE_INLINE HeQuat& HeQuat::operator=  (const HeQuat& q)
	{
	x = q.x;
	y = q.y;
	z = q.z;
	w = q.w;
	return *this;
	}

#if 0
HE_INLINE HeQuat& HeQuat::operator=  (const HeVec3& v)		//implicitly extends vector by a 0 w element.
	{
	x = v.x;
	y = v.y;
	z = v.z;
	w = HeReal(1.0);
	return *this;
	}
#endif

HE_INLINE HeQuat& HeQuat::operator*= (const HeQuat& q)
	{
	HeReal xx[4]; //working Quaternion
	xx[0] = w*q.w - q.x*x - y*q.y - q.z*z;
	xx[1] = w*q.x + q.w*x + y*q.z - q.y*z;
	xx[2] = w*q.y + q.w*y + z*q.x - q.z*x;
	z=w*q.z + q.w*z + x*q.y - q.x*y;

	w = xx[0];
	x = xx[1];
	y = xx[2];
	return *this;
	}


HE_INLINE HeQuat& HeQuat::operator+= (const HeQuat& q)
	{
	x+=q.x;
	y+=q.y;
	z+=q.z;
	w+=q.w;
	return *this;
	}


HE_INLINE HeQuat& HeQuat::operator-= (const HeQuat& q)
	{
	x-=q.x;
	y-=q.y;
	z-=q.z;
	w-=q.w;
	return *this;
	}


HE_INLINE HeQuat& HeQuat::operator*= (const HeReal s)
	{
	x*=s;
	y*=s;
	z*=s;
	w*=s;
	return *this;
	}

HE_INLINE HeQuat::HeQuat(HeReal ix, HeReal iy, HeReal iz, HeReal iw)
{
	x = ix;
	y = iy;
	z = iz;
	w = iw;
}

HE_INLINE HeQuat HeQuat::operator*(const HeQuat &q) const
{
	return HeQuat(w*q.x + q.w*x + y*q.z - q.y*z,
				  w*q.y + q.w*y + z*q.x - q.z*x,
				  w*q.z + q.w*z + x*q.y - q.x*y,
				  w*q.w - x*q.x - y*q.y - z*q.z);
}

HE_INLINE HeQuat HeQuat::operator+(const HeQuat &q) const
{
	return HeQuat(x+q.x,y+q.y,z+q.z,w+q.w);
}

HE_INLINE HeQuat HeQuat::operator-(const HeQuat &q) const
{
	return HeQuat(x-q.x,y-q.y,z-q.z,w-q.w);
}

HE_INLINE HeQuat HeQuat::operator!() const
{
	return HeQuat(-x,-y,-z,w);
}



HE_INLINE const HeVec3 HeQuat::rot(const HeVec3 &v) const
    {
	HeVec3 qv(x,y,z);

	return (v*(w*w-0.5f) + (qv^v)*w + qv*(qv|v))*2;
    }

HE_INLINE const HeVec3 HeQuat::invRot(const HeVec3 &v) const
    {
	HeVec3 qv(x,y,z);

	return (v*(w*w-0.5f) - (qv^v)*w + qv*(qv|v))*2;
    }



HE_INLINE const HeVec3 HeQuat::transform(const HeVec3 &v, const HeVec3 &p) const
    {
	return rot(v)+p;
    }

HE_INLINE const HeVec3 HeQuat::invTransform(const HeVec3 &v, const HeVec3 &p) const
    {
	return invRot(v-p);
    }


HE_INLINE void HeQuat::fromEuler(const HeVec3 &angles)
{
	HeReal roll  = angles.x*0.5f;
	HeReal pitch = angles.y*0.5f;
	HeReal yaw   = angles.z*0.5f;

	HeReal cr = cosf(roll);
	HeReal cp = cosf(pitch);
	HeReal cy = cosf(yaw);

	HeReal sr = sinf(roll);
	HeReal sp = sinf(pitch);
	HeReal sy = sinf(yaw);

	HeReal cpcy = cp * cy;
	HeReal spsy = sp * sy;
	HeReal spcy = sp * cy;
	HeReal cpsy = cp * sy;

	x   = ( sr * cpcy - cr * spsy);
	y   = ( cr * spcy + sr * cpsy);
	z   = ( cr * cpsy - sr * spcy);
	w   = cr * cpcy + sr * spsy;
}


HE_INLINE void HeQuat::getEuler(HeVec3 &angles)
{
	HeReal sint	     = (2.0f * w * y) - (2.0f * x * z);
	HeReal cost_temp = 1.0f - (sint * sint);
	HeReal cost	   	 = 0;

	if ( (HeReal)fabsf(cost_temp) > 0.001f )
	{
		cost = sqrtf( cost_temp );
	}

	HeReal sinv, cosv, sinf, cosf;
	if ( (HeReal)fabs(cost) > 0.001f )
	{
    cost = 1.0f / cost;
		sinv = ((2.0f * y * z) + (2.0f * w * x)) * cost;
		cosv = (1.0f - (2.0f * x * x) - (2.0f * y * y)) * cost;
		sinf = ((2.0f * x * y) + (2.0f * w * z)) * cost;
		cosf = (1.0f - (2.0f * y * y) - (2.0f * z * z)) * cost;
	}
	else
	{
		sinv = (2.0f * w * x) - (2.0f * y * z);
		cosv = 1.0f - (2.0f * x * x) - (2.0f * z * z);
		sinf = 0;
		cosf = 1.0f;
	}

	// compute output rotations
	angles.x	= atan2( sinv, cosv );
	angles.y	= atan2( sint, cost );
  angles.z	= atan2( sinf, cosf );

}

HE_INLINE void HeQuat::fromYawPitchRollDX(HeReal yaw,HeReal pitch,HeReal roll)
{

  float sinY, cosY, sinP, cosP, sinR, cosR;

  sinY = sinf(0.5f * yaw);
  cosY = cosf(0.5f * yaw);

  sinP = sinf(0.5f * pitch);
  cosP = cosf(0.5f * pitch);

  sinR = sinf(0.5f * roll);
  cosR = cosf(0.5f * roll);

//  x = cosR * sinP * cosY + sinR * cosP * sinY;
//  y = cosR * cosP * sinY - sinR * sinP * cosY;
//  z = sinR * cosP * cosY - cosR * sinP * sinY;
//  w = cosR * cosP * cosY + sinR * sinP * sinY;

  x = cosY * sinP * cosR + sinY * cosP * sinR;
  y = sinY * cosP * cosR - cosY * sinP * sinR;
  z = cosY * cosP * sinR - sinY * sinP * cosR;
  w = cosY * cosP * cosR + sinY * sinP * sinR;
}

#endif
