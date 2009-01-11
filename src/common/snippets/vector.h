#ifndef VECTOR_H

#define VECTOR_H

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




#pragma warning(disable:4786)

#include <math.h>
#include <float.h>
#include <vector>

#include "common/snippets/UserMemAlloc.h"



const HeF32 DEG_TO_RAD = ((2.0f * 3.14152654f) / 360.0f);
const HeF32 RAD_TO_DEG = (360.0f / (2.0f * 3.141592654f));

template <class Type> class Vector3d
{
public:
	Vector3d(void) { };  // null constructor, does not inialize point.

	Vector3d(const Vector3d &a) // constructor copies existing vector.
	{
		x = a.x;
		y = a.y;
		z = a.z;
	};

	Vector3d(Type a,Type b,Type c) // construct with initial point.
	{
		x = a;
		y = b;
		z = c;
	};

	Vector3d(const HeF32 *t)
	{
		x = t[0];
		y = t[1];
		z = t[2];
	};

	Vector3d(const HeF64 *t)
	{
		x = (HeF32)t[0];
		y = (HeF32)t[1];
		z = (HeF32)t[2];
	};

	Vector3d(const HeI32 *t)
	{
		x = t[0];
		y = t[1];
		z = t[2];
	};

	bool operator==(const Vector3d<Type> &a) const
	{
		return( a.x == x && a.y == y && a.z == z );
	};

	bool operator!=(const Vector3d<Type> &a) const
	{
		return( a.x != x || a.y != y || a.z != z );
	};

// Operators
		Vector3d& operator = (const Vector3d& A)          // ASSIGNMENT (=)
			{ x=A.x; y=A.y; z=A.z;
				return(*this);  };

		Vector3d operator + (const Vector3d& A) const     // ADDITION (+)
			{ Vector3d Sum(x+A.x, y+A.y, z+A.z);
				return(Sum); };

		Vector3d operator - (const Vector3d& A) const     // SUBTRACTION (-)
			{ Vector3d Diff(x-A.x, y-A.y, z-A.z);
				return(Diff); };

		Vector3d operator * (const HeF32 s) const       // MULTIPLY BY SCALAR (*)
			{ Vector3d Scaled(x*s, y*s, z*s);
				return(Scaled); };


		Vector3d operator + (const HeF32 s) const       // ADD CONSTANT TO ALL 3 COMPONENTS (*)
			{ Vector3d Scaled(x+s, y+s, z+s);
				return(Scaled); };




		Vector3d operator / (const HeF32 s) const       // DIVIDE BY SCALAR (/)
		{
			HeF32 r = 1.0f / s;
				Vector3d Scaled(x*r, y*r, z*r);
				return(Scaled);
		};

		void operator /= (Type A)             // ACCUMULATED VECTOR ADDITION (/=)
			{ x/=A; y/=A; z/=A; };

		void operator += (const Vector3d A)             // ACCUMULATED VECTOR ADDITION (+=)
			{ x+=A.x; y+=A.y; z+=A.z; };
		void operator -= (const Vector3d A)             // ACCUMULATED VECTOR SUBTRACTION (+=)
			{ x-=A.x; y-=A.y; z-=A.z; };
		void operator *= (const HeF32 s)        // ACCUMULATED SCALAR MULTIPLICATION (*=) (bpc 4/24/2000)
			{x*=s; y*=s; z*=s;}

		void operator += (const HeF32 A)             // ACCUMULATED VECTOR ADDITION (+=)
			{ x+=A; y+=A; z+=A; };


		Vector3d operator - (void) const                // NEGATION (-)
			{ Vector3d Negated(-x, -y, -z);
				return(Negated); };

		Type operator [] (const HeI32 i) const         // ALLOWS VECTOR ACCESS AS AN ARRAY.
			{ return( (i==0)?x:((i==1)?y:z) ); };
		Type & operator [] (const HeI32 i)
			{ return( (i==0)?x:((i==1)?y:z) ); };
//

	// accessor methods.
	Type GetX(void) const { return x; };
	Type GetY(void) const { return y; };
	Type GetZ(void) const { return z; };

	Type X(void) const { return x; };
	Type Y(void) const { return y; };
	Type Z(void) const { return z; };

	void SetX(Type t)   { x   = t; };
	void SetY(Type t)   { y   = t; };
	void SetZ(Type t)   { z   = t; };

	bool IsSame(const Vector3d<HeF32> &v,HeF32 epsilon) const
	{
		HeF32 dx = fabsf( x - v.x );
		if ( dx > epsilon ) return false;
		HeF32 dy = fabsf( y - v.y );
		if ( dy > epsilon ) return false;
		HeF32 dz = fabsf( z - v.z );
		if ( dz > epsilon ) return false;
		return true;
	}


	HeF32 ComputeNormal(const Vector3d<HeF32> &A,
										 const Vector3d<HeF32> &B,
										 const Vector3d<HeF32> &C)
	{
		HeF32 vx,vy,vz,wx,wy,wz,vw_x,vw_y,vw_z,mag;

		vx = (B.x - C.x);
		vy = (B.y - C.y);
		vz = (B.z - C.z);

		wx = (A.x - B.x);
		wy = (A.y - B.y);
		wz = (A.z - B.z);

		vw_x = vy * wz - vz * wy;
		vw_y = vz * wx - vx * wz;
		vw_z = vx * wy - vy * wx;

		mag = sqrtf((vw_x * vw_x) + (vw_y * vw_y) + (vw_z * vw_z));

		if ( mag < 0.000001f )
		{
			mag = 0;
		}
		else
		{
			mag = 1.0f/mag;
		}

		x = vw_x * mag;
		y = vw_y * mag;
		z = vw_z * mag;

		return mag;
	}


	void ScaleSumScale(HeF32 c0,HeF32 c1,const Vector3d<HeF32> &pos)
	{
		x = (x*c0) + (pos.x*c1);
		y = (y*c0) + (pos.y*c1);
		z = (z*c0) + (pos.z*c1);
	}

	void SwapYZ(void)
	{
		HeF32 t = y;
		y = z;
		z = t;
	};

	void Get(Type *v) const
	{
		v[0] = x;
		v[1] = y;
		v[2] = z;
	};

	void Set(const HeI32 *p)
	{
		x = (Type) p[0];
		y = (Type) p[1];
		z = (Type) p[2];
	}

	void Set(const HeF32 *p)
	{
		x = (Type) p[0];
		y = (Type) p[1];
		z = (Type) p[2];
	}


	void Set(Type a,Type b,Type c)
	{
		x = a;
		y = b;
		z = c;
	};

	void Zero(void)
	{
		x = y = z = 0;
	};

	const Type* Ptr() const { return &x; }
	Type* Ptr() { return &x; }


// return -(*this).
	Vector3d negative(void) const
	{
		Vector3d result;
		result.x = -x;
		result.y = -y;
		result.z = -z;
		return result;
	}

	Type Magnitude(void) const
	{
		return Type(sqrt(x * x + y * y + z * z));
	};

	Type FastMagnitude(void) const
	{
		return Type(fast_sqrt(x * x + y * y + z * z));
	};

	Type FasterMagnitude(void) const
	{
		return Type(faster_sqrt(x * x + y * y + z * z));
	};

	void Lerp(const Vector3d<Type>& from,const Vector3d<Type>& to,HeF32 slerp)
	{
		x = ((to.x - from.x) * slerp) + from.x;
		y = ((to.y - from.y) * slerp) + from.y;
		z = ((to.z - from.z) * slerp) + from.z;
	};

	// Highly specialized interpolate routine.  Will compute the interpolated position
	// shifted forward or backwards along the ray defined between (from) and (to).
	// Reason for existance is so that when a bullet collides with a wall, for
	// example, you can generate a graphic effect slightly *before* it hit the
	// wall so that the effect doesn't sort into the wall itself.
	void Interpolate(const Vector3d<HeF32> &from,const Vector3d<HeF32> &to,HeF32 offset)
	{
		x = to.x-from.x;
		y = to.y-from.y;
		z = to.z-from.z;
		HeF32 d = sqrtf( x*x + y*y + z*z );
		HeF32 recip = 1.0f / d;
		x*=recip;
		y*=recip;
		z*=recip; // normalize vector
		d+=offset; // shift along ray
		x = x*d + from.x;
		y = y*d + from.y;
		z = z*d + from.z;
	};

	bool BinaryEqual(const Vector3d<HeF32> &p) const
	{
		const HeI32 *source = (const HeI32 *) &x;
		const HeI32 *dest   = (const HeI32 *) &p.x;

		if ( source[0] == dest[0] &&
				 source[1] == dest[1] &&
				 source[2] == dest[2] ) return true;

		return false;
	};

	bool BinaryEqual(const Vector3d<HeI32> &p) const
	{
		if ( x == p.x && y == p.y && z == p.z ) return true;
		return false;
	}


/** Computes the reflection vector between two vectors.*/
	void Reflection(const Vector3d<Type> &a,const Vector3d<Type> &b)// compute reflection vector.
	{
		Vector3d<HeF32> c;
		Vector3d<HeF32> d;

		HeF32 dot = a.Dot(b) * 2.0f;

		c = b * dot;

		d = c - a;

		x = -d.x;
		y = -d.y;
		z = -d.z;
	};

	void AngleAxis(Type angle,const Vector3d<Type>& axis)
	{
		x = axis.x*angle;
		y = axis.y*angle;
		z = axis.z*angle;
	};

	Type Length(void) const          // length of vector.
	{
		return Type(sqrt( x*x + y*y + z*z ));
	};


	HeF32 ComputePlane(const Vector3d<HeF32> &A,
										 const Vector3d<HeF32> &B,
										 const Vector3d<HeF32> &C)
	{
		HeF32 vx,vy,vz,wx,wy,wz,vw_x,vw_y,vw_z,mag;

		vx = (B.x - C.x);
		vy = (B.y - C.y);
		vz = (B.z - C.z);

		wx = (A.x - B.x);
		wy = (A.y - B.y);
		wz = (A.z - B.z);

		vw_x = vy * wz - vz * wy;
		vw_y = vz * wx - vx * wz;
		vw_z = vx * wy - vy * wx;

		mag = sqrtf((vw_x * vw_x) + (vw_y * vw_y) + (vw_z * vw_z));

		if ( mag < 0.000001f )
		{
			mag = 0;
		}
		else
		{
			mag = 1.0f/mag;
		}

		x = vw_x * mag;
		y = vw_y * mag;
		z = vw_z * mag;


		HeF32 D = 0.0f - ((x*A.x)+(y*A.y)+(z*A.z));

		return D;
	}


	Type FastLength(void) const          // length of vector.
	{
		return Type(fast_sqrt( x*x + y*y + z*z ));
	};
	

	Type FasterLength(void) const          // length of vector.
	{
		return Type(faster_sqrt( x*x + y*y + z*z ));
	};

	Type Length2(void) const         // squared distance, prior to square root.
	{
		Type l2 = x*x+y*y+z*z;
		return l2;
	};

	Type Distance(const Vector3d<Type> &a) const   // distance between two points.
	{
		Vector3d<Type> d(a.x-x,a.y-y,a.z-z);
		return d.Length();
	}

	Type FastDistance(const Vector3d<Type> &a) const   // distance between two points.
	{
		Vector3d<Type> d(a.x-x,a.y-y,a.z-z);
		return d.FastLength();
	}
	
	Type FasterDistance(const Vector3d<Type> &a) const   // distance between two points.
	{
		Vector3d<Type> d(a.x-x,a.y-y,a.z-z);
		return d.FasterLength();
	}


	Type DistanceXY(const Vector3d<Type> &a) const
	{
	HeF32 dx = a.x - x;
	HeF32 dy = a.y - y;
		HeF32 dist = dx*dx + dy*dy;
	return dist;
	}

	Type Distance2(const Vector3d<Type> &a) const  // squared distance.
	{
		HeF32 dx = a.x - x;
		HeF32 dy = a.y - y;
		HeF32 dz = a.z - z;
		return dx*dx + dy*dy + dz*dz;
	};

	Type Partial(const Vector3d<Type> &p) const
	{
		return (x*p.y) - (p.x*y);
	}

	Type Area(const Vector3d<Type> &p1,const Vector3d<Type> &p2) const
	{
		Type A = Partial(p1);
		A+= p1.Partial(p2);
		A+= p2.Partial(*this);
		return A*0.5f;
	}

	inline HeF32 Normalize(void)       // normalize to a unit vector, returns distance.
	{
		HeF32 d = sqrtf( static_cast< HeF32 >( x*x + y*y + z*z ) );
		if ( d > 0 )
		{
			HeF32 r = 1.0f / d;
			x *= r;
			y *= r;
			z *= r;
		}
		else
		{
			x = y = z = 1;
		}
		return d;
	};

	inline HeF32 FastNormalize(void)       // normalize to a unit vector, returns distance.
	{
		HeF32 d = fast_sqrt( static_cast< HeF32 >( x*x + y*y + z*z ) );
		if ( d > 0 )
		{
			HeF32 r = 1.0f / d;
			x *= r;
			y *= r;
			z *= r;
		}
		else
		{
			x = y = z = 1;
		}
		return d;
	};

	inline HeF32 FasterNormalize(void)       // normalize to a unit vector, returns distance.
	{
		HeF32 d = faster_sqrt( static_cast< HeF32 >( x*x + y*y + z*z ) );
		if ( d > 0 )
		{
			HeF32 r = 1.0f / d;
			x *= r;
			y *= r;
			z *= r;
		}
		else
		{
			x = y = z = 1;
		}
		return d;
	};




	Type Dot(const Vector3d<Type> &a) const        // computes dot product.
	{
		return (x * a.x + y * a.y + z * a.z );
	};


	Vector3d<Type> Cross( const Vector3d<Type>& other ) const
	{
 		Vector3d<Type> result( y*other.z - z*other.y,  z*other.x - x*other.z,  x*other.y - y*other.x );

		return result;
	}

	void Cross(const Vector3d<Type> &a,const Vector3d<Type> &b)  // cross two vectors result in this one.
	{
		x = a.y*b.z - a.z*b.y;
		y = a.z*b.x - a.x*b.z;
		z = a.x*b.y - a.y*b.x;
	};

	/******************************************/
	// Check if next edge (b to c) turns inward
	//
	//    Edge from a to b is already in face
	//    Edge from b to c is being considered for addition to face
	/******************************************/
	bool Concave(const Vector3d<HeF32>& a,const Vector3d<HeF32>& b)
	{
		HeF32 vx,vy,vz,wx,wy,wz,vw_x,vw_y,vw_z,mag,nx,ny,nz,mag_a,mag_b;

		wx = b.x - a.x;
		wy = b.y - a.y;
		wz = b.z - a.z;

		mag_a = (HeF32) sqrtf((wx * wx) + (wy * wy) + (wz * wz));

		vx = x - b.x;
		vy = y - b.y;
		vz = z - b.z;

		mag_b = (HeF32) sqrtf((vx * vx) + (vy * vy) + (vz * vz));

		vw_x = (vy * wz) - (vz * wy);
		vw_y = (vz * wx) - (vx * wz);
		vw_z = (vx * wy) - (vy * wx);

		mag = (HeF32) sqrtf((vw_x * vw_x) + (vw_y * vw_y) + (vw_z * vw_z));

		// Check magnitude of cross product, which is a sine function
		// i.e., mag (a x b) = mag (a) * mag (b) * sin (theta);
		// If sin (theta) small, then angle between edges is very close to
		// 180, which we may want to call a concavity.	Setting the
		// CONCAVITY_TOLERANCE value greater than about 0.01 MAY cause
		// face consolidation to get stuck on particular face.	Most meshes
		// convert properly with a value of 0.0

		if (mag/(mag_a*mag_b) <= 0.0f )	return true;

		mag = 1.0f / mag;

		nx = vw_x * mag;
		ny = vw_y * mag;
		nz = vw_z * mag;

		// Dot product of tri normal with cross product result will
		// yield positive number if edges are convex (+1.0 if two tris
		// are coplanar), negative number if edges are concave (-1.0 if
		// two tris are coplanar.)

		mag = ( x * nx) + ( y * ny) + ( z * nz);

		if (mag > 0.0f ) return false;

		return(true);
	};

	bool PointTestXY(const Vector3d<HeF32> &i,const Vector3d<HeF32> &j) const
	{
		if (((( i.y <= y ) && ( y  < j.y )) ||
				 (( j.y <= y ) && ( y  < i.y ))) &&
					( x < (j.x - i.x) * (y - i.y) / (j.y - i.y) + i.x)) return true;
		return false;
	}

	// test to see if this point is inside the triangle specified by
	// these three points on the X/Y plane.
	bool PointInTriXY(const Vector3d<HeF32> &p1,
									const Vector3d<HeF32> &p2,
									const Vector3d<HeF32> &p3) const
	{
		HeF32 ax  = p3.x - p2.x;
		HeF32 ay  = p3.y - p2.y;
		HeF32 bx  = p1.x - p3.x;
		HeF32 by  = p1.y - p3.y;
		HeF32 cx  = p2.x - p1.x;
		HeF32 cy  = p2.y - p1.y;
		HeF32 apx = x - p1.x;
		HeF32 apy = y - p1.y;
		HeF32 bpx = x - p2.x;
		HeF32 bpy = y - p2.y;
		HeF32 cpx = x - p3.x;
		HeF32 cpy = y - p3.y;

		HeF32 aCROSSbp = ax*bpy - ay*bpx;
		HeF32 cCROSSap = cx*apy - cy*apx;
		HeF32 bCROSScp = bx*cpy - by*cpx;

		return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
	};

	// test to see if this point is inside the triangle specified by
	// these three points on the X/Y plane.
	bool PointInTriYZ(const Vector3d<HeF32> &p1,
									const Vector3d<HeF32> &p2,
									const Vector3d<HeF32> &p3) const
	{
		HeF32 ay  = p3.y - p2.y;
		HeF32 az  = p3.z - p2.z;
		HeF32 by  = p1.y - p3.y;
		HeF32 bz  = p1.z - p3.z;
		HeF32 cy  = p2.y - p1.y;
		HeF32 cz  = p2.z - p1.z;
		HeF32 apy = y - p1.y;
		HeF32 apz = z - p1.z;
		HeF32 bpy = y - p2.y;
		HeF32 bpz = z - p2.z;
		HeF32 cpy = y - p3.y;
		HeF32 cpz = z - p3.z;

		HeF32 aCROSSbp = ay*bpz - az*bpy;
		HeF32 cCROSSap = cy*apz - cz*apy;
		HeF32 bCROSScp = by*cpz - bz*cpy;

		return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
	};


	// test to see if this point is inside the triangle specified by
	// these three points on the X/Y plane.
	bool PointInTriXZ(const Vector3d<HeF32> &p1,
									const Vector3d<HeF32> &p2,
									const Vector3d<HeF32> &p3) const
	{
		HeF32 az  = p3.z - p2.z;
		HeF32 ax  = p3.x - p2.x;
		HeF32 bz  = p1.z - p3.z;
		HeF32 bx  = p1.x - p3.x;
		HeF32 cz  = p2.z - p1.z;
		HeF32 cx  = p2.x - p1.x;
		HeF32 apz = z - p1.z;
		HeF32 apx = x - p1.x;
		HeF32 bpz = z - p2.z;
		HeF32 bpx = x - p2.x;
		HeF32 cpz = z - p3.z;
		HeF32 cpx = x - p3.x;

		HeF32 aCROSSbp = az*bpx - ax*bpz;
		HeF32 cCROSSap = cz*apx - cx*apz;
		HeF32 bCROSScp = bz*cpx - bx*cpz;

		return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
	};

	// Given a point and a line (defined by two points), compute the closest point
	// in the line.  (The line is treated as infinitely long.)
	void NearestPointInLine(const Vector3d<Type> &point,
													const Vector3d<Type> &line0,
													const Vector3d<Type> &line1)
	{
		Vector3d<Type> &nearestPoint = *this;
		Vector3d<Type> lineDelta     = line1 - line0;

		// Handle degenerate lines
		if ( lineDelta == Vector3d<Type>(0, 0, 0) )
		{
			nearestPoint = line0;
		}
		else
		{
			HeF32 delta = (point-line0).Dot(lineDelta) / (lineDelta).Dot(lineDelta);
			nearestPoint = line0 + delta*lineDelta;
		}
	}

	// Given a point and a line segment (defined by two points), compute the closest point
	// in the line.  Cap the point at the endpoints of the line segment.
	void NearestPointInLineSegment(const Vector3d<Type> &point,
																 const Vector3d<Type> &line0,
																 const Vector3d<Type> &line1)
	{
		Vector3d<Type> &nearestPoint = *this;
		Vector3d<Type> lineDelta     = line1 - line0;

		// Handle degenerate lines
		if ( lineDelta == Vector3d<HeF32>(0, 0, 0) )
		{
			nearestPoint = line0;
		}
		else
		{
			HeF32 delta = (point-line0).Dot(lineDelta) / (lineDelta).Dot(lineDelta);

			// Clamp the point to conform to the segment's endpoints
			if ( delta < 0 )
				delta = 0;
			else if ( delta > 1 )
				delta = 1;

			nearestPoint = line0 + delta*lineDelta;
		}
	}

	// Given a point and a plane (defined by three points), compute the closest point
	// in the plane.  (The plane is unbounded.)
	void NearestPointInPlane(const Vector3d<Type> &point,
													 const Vector3d<Type> &triangle0,
													 const Vector3d<Type> &triangle1,
													 const Vector3d<Type> &triangle2)
	{
		Vector3d<Type> &nearestPoint = *this;
		Vector3d<Type> lineDelta0    = triangle1 - triangle0;
		Vector3d<Type> lineDelta1    = triangle2 - triangle0;
		Vector3d<Type> pointDelta    = point - triangle0;
		Vector3d<Type> normal;

		// Get the normal of the polygon (doesn't have to be a unit vector)
		normal.Cross(lineDelta0, lineDelta1);

		HeF32 delta = normal.Dot(pointDelta) / normal.Dot(normal);
		nearestPoint = point - delta*normal;
	}

	// Given a point and a plane (defined by a coplanar point and a normal), compute the closest point
	// in the plane.  (The plane is unbounded.)
	void NearestPointInPlane(const Vector3d<Type> &point,
													 const Vector3d<Type> &planePoint,
													 const Vector3d<Type> &planeNormal)
	{
		Vector3d<Type> &nearestPoint = *this;
		Vector3d<Type> pointDelta    = point - planePoint;

		HeF32 delta = planeNormal.Dot(pointDelta) / planeNormal.Dot(planeNormal);
		nearestPoint = point - delta*planeNormal;
	}

	// Given a point and a triangle (defined by three points), compute the closest point
	// in the triangle.  Clamp the point so it's confined to the area of the triangle.
	void NearestPointInTriangle(const Vector3d<Type> &point,
															const Vector3d<Type> &triangle0,
															const Vector3d<Type> &triangle1,
															const Vector3d<Type> &triangle2)
	{
		static const Vector3d<Type> zeroVector(0, 0, 0);

		Vector3d<Type> &nearestPoint = *this;

		Vector3d<Type> lineDelta0 = triangle1 - triangle0;
		Vector3d<Type> lineDelta1 = triangle2 - triangle0;

		// Handle degenerate triangles
		if ( (lineDelta0 == zeroVector) || (lineDelta1 == zeroVector) )
		{
			nearestPoint.NearestPointInLineSegment(point, triangle1, triangle2);
		}
		else if ( lineDelta0 == lineDelta1 )
		{
			nearestPoint.NearestPointInLineSegment(point, triangle0, triangle1);
		}

		else
		{
			static Vector3d<Type> axis[3];
			axis[0].NearestPointInLine(triangle0, triangle1, triangle2);
			axis[1].NearestPointInLine(triangle1, triangle0, triangle2);
			axis[2].NearestPointInLine(triangle2, triangle0, triangle1);

			Type axisDot[3];
			axisDot[0] = (triangle0-axis[0]).Dot(point-axis[0]);
			axisDot[1] = (triangle1-axis[1]).Dot(point-axis[1]);
			axisDot[2] = (triangle2-axis[2]).Dot(point-axis[2]);

			bool            bForce         = true;
			Type            bestMagnitude2 = 0;
			Type            closeMagnitude2;
			Vector3d<HeF32> closePoint;

			if ( axisDot[0] < 0 )
			{
				closePoint.NearestPointInLineSegment(point, triangle1, triangle2);
				closeMagnitude2 = point.Distance2(closePoint);
				if ( bForce || (bestMagnitude2 > closeMagnitude2) )
				{
					bForce         = false;
					bestMagnitude2 = closeMagnitude2;
					nearestPoint   = closePoint;
				}
			}
			if ( axisDot[1] < 0 )
			{
				closePoint.NearestPointInLineSegment(point, triangle0, triangle2);
				closeMagnitude2 = point.Distance2(closePoint);
				if ( bForce || (bestMagnitude2 > closeMagnitude2) )
				{
					bForce         = false;
					bestMagnitude2 = closeMagnitude2;
					nearestPoint   = closePoint;
				}
			}
			if ( axisDot[2] < 0 )
			{
				closePoint.NearestPointInLineSegment(point, triangle0, triangle1);
				closeMagnitude2 = point.Distance2(closePoint);
				if ( bForce || (bestMagnitude2 > closeMagnitude2) )
				{
					bForce         = false;
					bestMagnitude2 = closeMagnitude2;
					nearestPoint   = closePoint;
				}
			}

			// If bForce is true at this point, it means the nearest point lies
			// inside the triangle; use the nearest-point-on-a-plane equation
			if ( bForce )
			{
				Vector3d<Type> normal;

				// Get the normal of the polygon (doesn't have to be a unit vector)
				normal.Cross(lineDelta0, lineDelta1);

				Vector3d<HeF32> pointDelta = point - triangle0;
				HeF32 delta = normal.Dot(pointDelta) / normal.Dot(normal);

				nearestPoint = point - delta*normal;
			}
		}
	}


//private:

	Type x;
	Type y;
	Type z;
};


template <class Type> class Vector2d
{
public:
	Vector2d(void) { };  // null constructor, does not inialize point.

	Vector2d(const Vector2d &a) // constructor copies existing vector.
	{
		x = a.x;
		y = a.y;
	};

	Vector2d(const HeF32 *t)
	{
		x = t[0];
		y = t[1];
	};


	Vector2d(Type a,Type b) // construct with initial point.
	{
		x = a;
		y = b;
	};

	const Type* Ptr() const { return &x; }
	Type* Ptr() { return &x; }

	Vector2d & operator+=(const Vector2d &a) // += operator.
	{
		x+=a.x;
		y+=a.y;
		return *this;
	};

	Vector2d & operator-=(const Vector2d &a)
	{
		x-=a.x;
		y-=a.y;
		return *this;
	};

	Vector2d & operator*=(const Vector2d &a)
	{
		x*=a.x;
		y*=a.y;
		return *this;
	};

	Vector2d & operator/=(const Vector2d &a)
	{
		x/=a.x;
		y/=a.y;
		return *this;
	};

	bool operator==(const Vector2d<Type> &a) const
	{
		if ( a.x == x && a.y == y ) return true;
		return false;
	};

	bool operator!=(const Vector2d &a) const
	{
		if ( a.x != x || a.y != y ) return true;
		return false;
	};

	Vector2d operator+(Vector2d a) const
	{
		a.x+=x;
		a.y+=y;
		return a;
	};

	Vector2d operator-(Vector2d a) const
	{
		a.x = x-a.x;
		a.y = y-a.y;
		return a;
	};

	Vector2d operator - (void) const
	{
		return negative();
	};

	Vector2d operator*(Vector2d a) const
	{
		a.x*=x;
		a.y*=y;
		return a;
	};

	Vector2d operator*(Type c) const
	{
		Vector2d<Type> a;

		a.x = x * c;
		a.y = y * c;

		return a;
	};

	Vector2d operator/(Vector2d a) const
	{
		a.x = x/a.x;
		a.y = y/a.y;
		return a;
	};


	Type Dot(const Vector2d<Type> &a) const        // computes dot product.
	{
		return (x * a.x + y * a.y );
	};

	Type GetX(void) const { return x; };
	Type GetY(void) const { return y; };

	void SetX(Type t) { x   = t; };
	void SetY(Type t) { y   = t; };


	void Set(const HeI32 *p)
	{
		x = (Type) p[0];
		y = (Type) p[1];
	}

	void Set(const HeF32 *p)
	{
		x = (Type) p[0];
		y = (Type) p[1];
	}


	void Set(Type a,Type b)
	{
		x = a;
		y = b;
	};

	void Zero(void)
	{
		x = y = 0;
	};

	Vector2d negative(void) const
	{
		Vector2d result;
		result.x = -x;
		result.y = -y;
		return result;
	}

	Type magnitude(void) const
	{
		return (Type) sqrtf(x * x + y * y );
	}

	Type fastmagnitude(void) const
	{
		return (Type) fast_sqrt(x * x + y * y );
	}

	Type fastermagnitude(void) const
	{
		return (Type) faster_sqrt( x * x + y * y );
	}

	void Reflection(Vector2d &a,Vector2d &b); // compute reflection vector.

	Type Length(void) const          // length of vector.
	{
		return Type(sqrtf( x*x + y*y ));
	};

	Type FastLength(void) const          // length of vector.
	{
		return Type(fast_sqrt( x*x + y*y ));
	};

	Type FasterLength(void) const          // length of vector.
	{
		return Type(faster_sqrt( x*x + y*y ));
	};

	Type Length2(void)        // squared distance, prior to square root.
	{
		return x*x+y*y;
	}

	Type Distance(const Vector2d &a) const   // distance between two points.
	{
		Type dx = a.x - x;
		Type dy = a.y - y;
		Type d  = dx*dx+dy*dy;
		return sqrtf(d);
	};

	Type FastDistance(const Vector2d &a) const   // distance between two points.
	{
		Type dx = a.x - x;
		Type dy = a.y - y;
		Type d  = dx*dx+dy*dy;
		return fast_sqrt(d);
	};

	Type FasterDistance(const Vector2d &a) const   // distance between two points.
	{
		Type dx = a.x - x;
		Type dy = a.y - y;
		Type d  = dx*dx+dy*dy;
		return faster_sqrt(d);
	};

	Type Distance2(Vector2d &a) // squared distance.
	{
		Type dx = a.x - x;
		Type dy = a.y - y;
		return dx*dx + dy *dy;
	};

	void Lerp(const Vector2d<Type>& from,const Vector2d<Type>& to,HeF32 slerp)
	{
		x = ((to.x - from.x)*slerp) + from.x;
		y = ((to.y - from.y)*slerp) + from.y;
	};


	void Cross(const Vector2d<Type> &a,const Vector2d<Type> &b)  // cross two vectors result in this one.
	{
		x = a.y*b.x - a.x*b.y;
		y = a.x*b.x - a.x*b.x;
	};

	Type Normalize(void)       // normalize to a unit vector, returns distance.
	{
		Type l = Length();
		if ( l != 0 )
		{
			l = Type( 1 ) / l;
			x*=l;
			y*=l;
		}
		else
		{
			x = y = 0;
		}
		return l;
	};

	Type FastNormalize(void)       // normalize to a unit vector, returns distance.
	{
		Type l = FastLength();
		if ( l != 0 )
		{
			l = Type( 1 ) / l;
			x*=l;
			y*=l;
		}
		else
		{
			x = y = 0;
		}
		return l;
	};

	Type FasterNormalize(void)       // normalize to a unit vector, returns distance.
	{
		Type l = FasterLength();
		if ( l != 0 )
		{
			l = Type( 1 ) / l;
			x*=l;
			y*=l;
		}
		else
		{
			x = y = 0;
		}
		return l;
	};


	Type x;
	Type y;
};

class Line
{
public:
	Line(const Vector3d<HeF32> &from,const Vector3d<HeF32> &to)
	{
		mP1 = from;
		mP2 = to;
	};
	// JWR  Test for the intersection of two lines.

	bool Intersect(const Line& src,Vector3d<HeF32> &sect);
private:
	Vector3d<HeF32> mP1;
	Vector3d<HeF32> mP2;

};


typedef USER_STL::vector< Vector3d<HeF32> > Vector3dVector;
typedef USER_STL::vector< Vector2d<HeF32> > Vector2dVector;

template <class Type> Vector3d<Type> operator * (Type s, const Vector3d<Type> &v )
			{ Vector3d <Type> Scaled(v.x*s, v.y*s, v.z*s);
				return(Scaled); };

template <class Type> Vector2d<Type> operator * (Type s, const Vector2d<Type> &v )
			{ Vector2d <Type> Scaled(v.x*s, v.y*s);
				return(Scaled); };

class MyMatrix;

class BoundingBox
{
public:
	void InitMinMax(void)
	{
		bmin.Set(FLT_MAX,FLT_MAX,FLT_MAX);
		bmax.Set(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	}

	void MinMax(const Vector3d<HeF32> &p)
	{
		if ( p.x < bmin.x ) bmin.x = p.x;
		if ( p.y < bmin.y ) bmin.y = p.y;
		if ( p.z < bmin.z ) bmin.z = p.z;

		if ( p.x > bmax.x ) bmax.x = p.x;
		if ( p.y > bmax.y ) bmax.y = p.y;
		if ( p.z > bmax.z ) bmax.z = p.z;
	}

	void MinMax(const BoundingBox &b)
	{
		if ( b.bmin.x < bmin.x ) bmin.x = b.bmin.x;
		if ( b.bmin.y < bmin.y ) bmin.y = b.bmin.y;
		if ( b.bmin.z < bmin.z ) bmin.z = b.bmin.z;

		if ( b.bmax.x > bmax.x ) bmax.x = b.bmax.x;
		if ( b.bmax.y > bmax.y ) bmax.y = b.bmax.y;
		if ( b.bmax.z > bmax.z ) bmax.z = b.bmax.z;
	}


	void GetCenter(Vector3d<HeF32> &center) const
	{
		center.x = (bmax.x - bmin.x)*0.5f + bmin.x;
		center.y = (bmax.y - bmin.y)*0.5f + bmin.y;
		center.z = (bmax.z - bmin.z)*0.5f + bmin.z;
	}

	void GetSides(Vector3d<HeF32> &sides) const
	{
		sides.x = bmax.x - bmin.x;
		sides.y = bmax.y - bmin.y;
		sides.z = bmax.z - bmin.z;
	}

	HeF32 GetLength(void) const
	{
		return bmin.Distance(bmax);
	}

	void TransformBoundAABB(const MyMatrix &t,const BoundingBox &b);

	void BoundTest(const MyMatrix &transform,HeF32 x,HeF32 y,HeF32 z);

	Vector3d<HeF32> bmin;
	Vector3d<HeF32> bmax;
};

#endif
