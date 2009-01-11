#ifndef HE_FOUNDATION_NXVEC3
#define HE_FOUNDATION_NXVEC3

#include "Hef.h"
#include "HeMath.h"

class  HeMat44;


/**
\brief Enum to classify an axis.
*/
	enum HeAxisType
	{
		HE_AXIS_PLUS_X,
		HE_AXIS_MINUS_X,
		HE_AXIS_PLUS_Y,
		HE_AXIS_MINUS_Y,
		HE_AXIS_PLUS_Z,
		HE_AXIS_MINUS_Z,
		HE_AXIS_ARBITRARY
	};


class HeVec3;

/** \cond Exclude from documentation */
typedef struct _He3F32
{
	HeReal x, y, z;
	
	HE_INLINE const _He3F32& operator=(const HeVec3& d);
} He3F32;
/** \endcond */

/**
\brief 3 Element vector class.

This is a vector class with public data members.
This is not nice but it has become such a standard that hiding the xyz data members
makes it difficult to reuse external code that assumes that these are public in the library.
The vector class can be made to use float or double precision by appropriately defining HeReal.
This has been chosen as a cleaner alternative to a template class.
*/
class HeVec3
	{
	public:
	//!Constructors

	/**
	\brief default constructor leaves data uninitialized.
	*/
	HE_INLINE HeVec3();

	/**
	\brief Assigns scalar parameter to all elements.
	
	Useful to initialize to zero or one.

	\param[in] a Value to assign to elements.
	*/
	explicit HE_INLINE HeVec3(HeReal a);

	/**
	\brief Initializes from 3 scalar parameters.

	\param[in] nx Value to initialize X component.
	\param[in] ny Value to initialize Y component.
	\param[in] nz Value to initialize Z component.
	*/
	HE_INLINE HeVec3(HeReal nx, HeReal ny, HeReal nz);
	
	/**
	\brief Initializes from He3F32 data type.

	\param[in] a Value to initialize with.
	*/
	HE_INLINE HeVec3(const He3F32 &a);

	/**
	\brief Initializes from an array of scalar parameters.

	\param[in] v Value to initialize with.
	*/
	HE_INLINE HeVec3(const HeReal v[]);

	/**
	\brief Copy constructor.
	*/
	HE_INLINE HeVec3(const HeVec3& v);

	/**
	\brief Assignment operator.
	*/
	HE_INLINE const HeVec3& operator=(const HeVec3&);

	/**
	\brief Assignment operator.
	*/
	HE_INLINE const HeVec3& operator=(const He3F32&);

	/**
	\brief Access the data as an array.

	\return Array of 3 floats.
	*/
	HE_INLINE const HeReal *get() const;

  HE_INLINE const HeReal *ptr(void) const { return &x; };
	
	/**
	\brief Access the data as an array.

	\return Array of 3 floats.
	*/
	HE_INLINE HeReal* get();

	/**
	\brief writes out the 3 values to dest.

	\param[out] dest Array to write elements to.
	*/
	HE_INLINE void get(HeF32 * dest) const;

	/**
	\brief writes out the 3 values to dest.
	*/
	HE_INLINE void get(HeF64 * dest) const;

	HE_INLINE HeReal& operator[](int index);
	HE_INLINE HeReal  operator[](int index) const;

	//Operators
	/**
	\brief true if all the members are smaller.
	*/
	HE_INLINE bool operator< (const HeVec3&) const;
	/**
	\brief returns true if the two vectors are exactly equal.

	use equal() to test with a tolerance.
	*/
	HE_INLINE bool operator==(const HeVec3&) const;
	/**
	\brief returns true if the two vectors are exactly unequal.

	use !equal() to test with a tolerance.
	*/
	HE_INLINE bool operator!=(const HeVec3&) const;

/*	HE_INLINE const HeVec3 &operator +=(const HeVec3 &);
	HE_INLINE const HeVec3 &operator -=(const HeVec3 &);
	HE_INLINE const HeVec3 &operator *=(HeReal);
	HE_INLINE const HeVec3 &operator /=(HeReal);
*/
//Methods
	HE_INLINE void  set(const HeVec3 &);

//legacy methods:
	HE_INLINE void setx(const HeReal & d);
	HE_INLINE void sety(const HeReal & d);
	HE_INLINE void setz(const HeReal & d);

  HE_INLINE void lerp(const HeVec3 &a,const HeVec3 &b,HeF32 t);

	/**
	\brief this = -a
	*/
	HE_INLINE void  setNegative(const HeVec3 &a);

	/**
	\brief this = -this
	*/
	HE_INLINE void  setNegative();

	/**
	\brief reads 3 consecutive values from the ptr passed
	*/
	HE_INLINE void  set(const HeF32 *);

	/**
	\brief reads 3 consecutive values from the ptr passed
	*/
	HE_INLINE void  set(const HeF64 *);
	HE_INLINE void  set(HeReal, HeReal, HeReal);
	HE_INLINE void  set(HeReal);

	HE_INLINE void  zero();
	
	/**
	\brief tests for exact zero vector
	*/
	HE_INLINE HE_BOOL isZero()	const
		{
		if((x != 0.0f) || (y != 0.0f) || (z != 0.0f))	return HE_FALSE;
		return HE_TRUE;
		}

	HE_INLINE void  setPlusInfinity();
	HE_INLINE void  setMinusInfinity();

	/**
	\brief this = element wise min(this,other)
	*/
	HE_INLINE void vmin(const HeVec3 &);
	/**
	\brief this = element wise max(this,other)
	*/
	HE_INLINE void vmax(const HeVec3 &);

	/**
	\brief this = a + b
	*/
	HE_INLINE void  add(const HeVec3 & a, const HeVec3 & b);
	/**
	\brief this = a - b
	*/
	HE_INLINE void  subtract(const HeVec3 &a, const HeVec3 &b);
	/**
	\brief this = s * a;
	*/
	HE_INLINE void  multiply(HeReal s,  const HeVec3 & a);

	/**
	\brief this[i] = a[i] * b[i], for all i.
	*/
	HE_INLINE void  arrayMultiply(const HeVec3 &a, const HeVec3 &b);


	/**
	\brief this = s * a + b;
	*/
	HE_INLINE void  multiplyAdd(HeReal s, const HeVec3 & a, const HeVec3 & b);

	/**
	\brief normalizes the vector
	*/
	HE_INLINE HeReal normalize();

	/**
	\brief sets the vector's magnitude
	*/
	HE_INLINE void	setMagnitude(HeReal);

	/**
	\brief snaps to closest axis
	*/
	HE_INLINE HeU32			closestAxis()	const;

	/**
	\brief snaps to closest axis
	*/
	HE_INLINE HeAxisType	snapToClosestAxis();

//const methods
	/**
	\brief returns true if all 3 elems of the vector are finite (not NAN or INF, etc.)
	*/
	HE_INLINE bool isFinite() const;

	/**
	\brief returns the scalar product of this and other.
	*/
	HE_INLINE HeReal dot(const HeVec3 &other) const;

	/**
	\brief compares orientations (more readable, user-friendly function)
	*/
	HE_INLINE bool sameDirection(const HeVec3 &) const;

	/**
	\brief returns the magnitude
	*/
	HE_INLINE HeReal magnitude() const;

	/**
	\brief returns the squared magnitude
	
	Avoids calling sqrt()!
	*/
	HE_INLINE HeReal magnitudeSquared() const;

	/**
	\brief returns (this - other).magnitude();
	*/
	HE_INLINE HeReal distance(const HeVec3 &) const;

	/**
	\brief returns (this - other).magnitudeSquared();
	*/
	HE_INLINE HeReal distanceSquared(const HeVec3 &v) const;

	/**
	\brief this = left x right
	*/
	HE_INLINE void cross(const HeVec3 &left, const HeVec3 & right);

	/**
	\brief Stuff magic values in the point, marking it as explicitly not used.
	*/
	HE_INLINE void setNotUsed();

	/**
	\brief Checks the point is marked as not used
	*/
	HE_BOOL isNotUsed() const;

	/**
	\brief returns true if this and arg's elems are within epsilon of each other.
	*/
	HE_INLINE bool equals(const HeVec3 &, HeReal epsilon) const;

	/**
	\brief negation
	*/
	HeVec3 operator -() const;
	/**
	\brief vector addition
	*/
	HeVec3 operator +(const HeVec3 & v) const;
	/**
	\brief vector difference
	*/
	HeVec3 operator -(const HeVec3 & v) const;
	/**
	\brief scalar post-multiplication
	*/
	HeVec3 operator *(HeReal f) const;
	/**
	\brief scalar division
	*/
	HeVec3 operator /(HeReal f) const;
	/**
	\brief vector addition
	*/
	HeVec3&operator +=(const HeVec3& v);
	/**
	\brief vector difference
	*/
	HeVec3&operator -=(const HeVec3& v);
	/**
	\brief scalar multiplication
	*/
	HeVec3&operator *=(HeReal f);
	/**
	\brief scalar division
	*/
	HeVec3&operator /=(HeReal f);
	/**
	\brief cross product
	*/
	HeVec3 cross(const HeVec3& v) const;

	/**
	\brief cross product
	*/
	HeVec3 operator^(const HeVec3& v) const;
	/**
	\brief dot product
	*/
	HeReal      operator|(const HeVec3& v) const;

	HeReal x,y,z;
	};

/** \cond Exclude from documentation */
HE_INLINE const _He3F32& He3F32::operator=(const HeVec3& d)
	{
	x=d.x;
	y=d.y;
	z=d.z;
	return *this;
	}
/** \endcond */

//HE_INLINE implementations:
HE_INLINE HeVec3::HeVec3(HeReal v) : x(v), y(v), z(v)
	{
	}

HE_INLINE HeVec3::HeVec3(HeReal _x, HeReal _y, HeReal _z) : x(_x), y(_y), z(_z)
	{
	}

HE_INLINE HeVec3::HeVec3(const He3F32 &d) : x(d.x), y(d.y), z(d.z)
	{
	}

HE_INLINE HeVec3::HeVec3(const HeReal v[]) : x(v[0]), y(v[1]), z(v[2])
	{
	}


HE_INLINE HeVec3::HeVec3(const HeVec3 &v) : x(v.x), y(v.y), z(v.z)
	{
	}


HE_INLINE HeVec3::HeVec3()
	{
	//default constructor leaves data uninitialized.
	}


HE_INLINE const HeVec3& HeVec3::operator=(const HeVec3& v)
	{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
	}

HE_INLINE const HeVec3& HeVec3::operator=(const He3F32& d)
	{
	x = d.x;
	y = d.y;
	z = d.z;
	return *this;
	}

// Access the data as an array.

HE_INLINE const HeReal* HeVec3::get() const
	{
	return &x;
	}


HE_INLINE HeReal* HeVec3::get()
	{
	return &x;
	}

 
HE_INLINE void  HeVec3::get(HeF32 * v) const
	{
	v[0] = (HeF32)x;
	v[1] = (HeF32)y;
	v[2] = (HeF32)z;
	}

 
HE_INLINE void  HeVec3::get(HeF64 * v) const
	{
	v[0] = (HeF64)x;
	v[1] = (HeF64)y;
	v[2] = (HeF64)z;
	}


HE_INLINE HeReal& HeVec3::operator[](int index)
	{
	HE_ASSERT(index>=0 && index<=2);
	return (&x)[index];
	}


HE_INLINE HeReal  HeVec3::operator[](int index) const
	{
	HE_ASSERT(index>=0 && index<=2);
	return (&x)[index];
	}

 
HE_INLINE void HeVec3::setx(const HeReal & d) 
	{ 
	x = d; 
	}

 
HE_INLINE void HeVec3::sety(const HeReal & d) 
	{ 
	y = d; 
	}

 
HE_INLINE void HeVec3::setz(const HeReal & d) 
	{ 
	z = d; 
	}

//Operators
 
HE_INLINE bool HeVec3::operator< (const HeVec3&v) const
	{
	return ((x < v.x)&&(y < v.y)&&(z < v.z));
	}

 
HE_INLINE bool HeVec3::operator==(const HeVec3& v) const
	{
	return ((x == v.x)&&(y == v.y)&&(z == v.z));
	}

 
HE_INLINE bool HeVec3::operator!=(const HeVec3& v) const
	{
	return ((x != v.x)||(y != v.y)||(z != v.z));
	}

//Methods
 
HE_INLINE void  HeVec3::set(const HeVec3 & v)
	{
	x = v.x;
	y = v.y;
	z = v.z;
	}

 
HE_INLINE void  HeVec3::setNegative(const HeVec3 & v)
	{
	x = -v.x;
	y = -v.y;
	z = -v.z;
	}

 
HE_INLINE void  HeVec3::setNegative()
	{
	x = -x;
	y = -y;
	z = -z;
	}


 
HE_INLINE void  HeVec3::set(const HeF32 * v)
	{
	x = (HeReal)v[0];
	y = (HeReal)v[1];
	z = (HeReal)v[2];
	}

 
HE_INLINE void  HeVec3::set(const HeF64 * v)
	{
	x = (HeReal)v[0];
	y = (HeReal)v[1];
	z = (HeReal)v[2];
	}


 
HE_INLINE void  HeVec3::set(HeReal _x, HeReal _y, HeReal _z)
	{
	this->x = _x;
	this->y = _y;
	this->z = _z;
	}

 
HE_INLINE void HeVec3::set(HeReal v)
	{
	x = v;
	y = v;
	z = v;
	}

 
HE_INLINE void  HeVec3::zero()
	{
	x = y = z = 0.0;
	}

 
HE_INLINE void  HeVec3::setPlusInfinity()
	{
	x = y = z = HE_MAX_F32; //TODO: this may be double too, but here we can't tell!
	}

 
HE_INLINE void  HeVec3::setMinusInfinity()
	{
	x = y = z = HE_MIN_F32; //TODO: this may be double too, but here we can't tell!
	}

 
HE_INLINE void HeVec3::vmax(const HeVec3 & v)
	{
	x = HeMath::vmax(x, v.x);
	y = HeMath::vmax(y, v.y);
	z = HeMath::vmax(z, v.z);
	}

 
HE_INLINE void HeVec3::vmin(const HeVec3 & v)
	{
	x = HeMath::vmin(x, v.x);
	y = HeMath::vmin(y, v.y);
	z = HeMath::vmin(z, v.z);
	}




HE_INLINE void  HeVec3::add(const HeVec3 & a, const HeVec3 & b)
	{
	x = a.x + b.x;
	y = a.y + b.y;
	z = a.z + b.z;
	}


HE_INLINE void  HeVec3::subtract(const HeVec3 &a, const HeVec3 &b)
	{
	x = a.x - b.x;
	y = a.y - b.y;
	z = a.z - b.z;
	}


HE_INLINE void  HeVec3::arrayMultiply(const HeVec3 &a, const HeVec3 &b)
	{
	x = a.x * b.x;
	y = a.y * b.y;
	z = a.z * b.z;
	}


HE_INLINE void  HeVec3::multiply(HeReal s,  const HeVec3 & a)
	{
	x = a.x * s;
	y = a.y * s;
	z = a.z * s;
	}


HE_INLINE void  HeVec3::multiplyAdd(HeReal s, const HeVec3 & a, const HeVec3 & b)
	{
	x = s * a.x + b.x;
	y = s * a.y + b.y;
	z = s * a.z + b.z;
	}

 
HE_INLINE HeReal HeVec3::normalize()
	{
	HeReal m = magnitude();
	if (m)
		{
		const HeReal il =  HeReal(1.0) / m;
		x *= il;
		y *= il;
		z *= il;
		}
	return m;
	}

 
HE_INLINE void HeVec3::setMagnitude(HeReal length)
	{
	HeReal m = magnitude();
	if(m)
		{
		HeReal newLength = length / m;
		x *= newLength;
		y *= newLength;
		z *= newLength;
		}
	}

 
HE_INLINE HeAxisType HeVec3::snapToClosestAxis()
	{
	const HeReal almostOne = 0.999999f;
			if(x >=  almostOne) { set( 1.0f,  0.0f,  0.0f);	return HE_AXIS_PLUS_X ; }
	else	if(x <= -almostOne) { set(-1.0f,  0.0f,  0.0f);	return HE_AXIS_MINUS_X; }
	else	if(y >=  almostOne) { set( 0.0f,  1.0f,  0.0f);	return HE_AXIS_PLUS_Y ; }
	else	if(y <= -almostOne) { set( 0.0f, -1.0f,  0.0f);	return HE_AXIS_MINUS_Y; }
	else	if(z >=  almostOne) { set( 0.0f,  0.0f,  1.0f);	return HE_AXIS_PLUS_Z ; }
	else	if(z <= -almostOne) { set( 0.0f,  0.0f, -1.0f);	return HE_AXIS_MINUS_Z; }
	else													return HE_AXIS_ARBITRARY;
	}


HE_INLINE HeU32 HeVec3::closestAxis() const
	{
	const HeF32* vals = &x;
	HeU32 m = 0;
	if(HeMath::abs(vals[1]) > HeMath::abs(vals[m])) m = 1;
	if(HeMath::abs(vals[2]) > HeMath::abs(vals[m])) m = 2;
	return m;
	}


//const methods
 
HE_INLINE bool HeVec3::isFinite() const
	{
	return HeMath::isFinite(x) && HeMath::isFinite(y) && HeMath::isFinite(z);
	}

 
HE_INLINE HeReal HeVec3::dot(const HeVec3 &v) const
	{
	return x * v.x + y * v.y + z * v.z;
	}

 
HE_INLINE bool HeVec3::sameDirection(const HeVec3 &v) const
	{
	return x*v.x + y*v.y + z*v.z >= 0.0f;
	}

 
HE_INLINE HeReal HeVec3::magnitude() const
	{
	return HeMath::sqrt(x * x + y * y + z * z);
	}

 
HE_INLINE HeReal HeVec3::magnitudeSquared() const
	{
	return x * x + y * y + z * z;
	}

 
HE_INLINE HeReal HeVec3::distance(const HeVec3 & v) const
	{
	HeReal dx = x - v.x;
	HeReal dy = y - v.y;
	HeReal dz = z - v.z;
	return HeMath::sqrt(dx * dx + dy * dy + dz * dz);
	}

 
HE_INLINE HeReal HeVec3::distanceSquared(const HeVec3 &v) const
	{
	HeReal dx = x - v.x;
	HeReal dy = y - v.y;
	HeReal dz = z - v.z;
	return dx * dx + dy * dy + dz * dz;
	}

 
HE_INLINE void HeVec3::cross(const HeVec3 &left, const HeVec3 & right)	//prefered version, w/o temp object.
	{
	// temps needed in case left or right is this.
	HeReal a = (left.y * right.z) - (left.z * right.y);
	HeReal b = (left.z * right.x) - (left.x * right.z);
	HeReal c = (left.x * right.y) - (left.y * right.x);

	x = a;
	y = b;
	z = c;
	}

 
HE_INLINE bool HeVec3::equals(const HeVec3 & v, HeReal epsilon) const
	{
	return 
		HeMath::equals(x, v.x, epsilon) &&
		HeMath::equals(y, v.y, epsilon) &&
		HeMath::equals(z, v.z, epsilon);
	}


 
HE_INLINE HeVec3 HeVec3::operator -() const
	{
	return HeVec3(-x, -y, -z);
	}

 
HE_INLINE HeVec3 HeVec3::operator +(const HeVec3 & v) const
	{
	return HeVec3(x + v.x, y + v.y, z + v.z);	// RVO version
	}

 
HE_INLINE HeVec3 HeVec3::operator -(const HeVec3 & v) const
	{
	return HeVec3(x - v.x, y - v.y, z - v.z);	// RVO version
	}



HE_INLINE HeVec3 HeVec3::operator *(HeReal f) const
	{
	return HeVec3(x * f, y * f, z * f);	// RVO version
	}


HE_INLINE HeVec3 HeVec3::operator /(HeReal f) const
	{
		f = HeReal(1.0) / f; return HeVec3(x * f, y * f, z * f);
	}


HE_INLINE HeVec3& HeVec3::operator +=(const HeVec3& v)
	{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
	}


HE_INLINE HeVec3& HeVec3::operator -=(const HeVec3& v)
	{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
	}


HE_INLINE HeVec3& HeVec3::operator *=(HeReal f)
	{
	x *= f;
	y *= f;
	z *= f;
	return *this;
	}


HE_INLINE HeVec3& HeVec3::operator /=(HeReal f)
	{
	f = 1.0f/f;
	x *= f;
	y *= f;
	z *= f;
	return *this;
	}


HE_INLINE HeVec3 HeVec3::cross(const HeVec3& v) const
	{
	HeVec3 temp;
	temp.cross(*this,v);
	return temp;
	}


HE_INLINE HeVec3 HeVec3::operator^(const HeVec3& v) const
	{
	HeVec3 temp;
	temp.cross(*this,v);
	return temp;
	}


HE_INLINE HeReal HeVec3::operator|(const HeVec3& v) const
	{
	return x * v.x + y * v.y + z * v.z;
	}

/**
scalar pre-multiplication
*/

HE_INLINE HeVec3 operator *(HeReal f, const HeVec3& v)
	{
	return HeVec3(f * v.x, f * v.y, f * v.z);
	}


HE_INLINE void HeVec3::lerp(const HeVec3 &a,const HeVec3 &b,HeF32 t)
{
  x = (b.x-a.x)*t+a.x;
  y = (b.y-a.y)*t+a.y;
  z = (b.z-a.z)*t+a.z;
}

#endif
