#ifndef HE_FOUNDATION_NXPLANE
#define HE_FOUNDATION_NXPLANE

#include "Hef.h"
#include "HeVec3.h"
#include "HeMat44.h"

/**
\brief Representation of a plane.

 Plane equation used: a*x + b*y + c*z + d = 0
*/
class HePlane
	{
	public:
	/**
	\brief Constructor
	*/
	HE_INLINE HePlane()
		{
		}

	/**
	\brief Constructor from a normal and a distance
	*/
	HE_INLINE HePlane(HeF32 nx, HeF32 ny, HeF32 nz, HeF32 _d)
		{
		set(nx, ny, nz, _d);
		}

	/**
	\brief Constructor from a point on the plane and a normal
	*/
	HE_INLINE HePlane(const HeVec3& p, const HeVec3& n)
		{
		set(p, n);
		}

	/**
	\brief Constructor from three points
	*/
	HE_INLINE HePlane(const HeVec3& p0, const HeVec3& p1, const HeVec3& p2)
		{
		set(p0, p1, p2);
		}

	/**
	\brief Constructor from a normal and a distance
	*/
	HE_INLINE HePlane(const HeVec3& _n, HeF32 _d) : normal(_n), d(_d)
		{
		}

	/**
	\brief Copy constructor
	*/
	HE_INLINE HePlane(const HePlane& plane) : normal(plane.normal), d(plane.d)
		{
		}

	/**
	\brief Destructor
	*/
	HE_INLINE ~HePlane()
		{
		}

	/**
	\brief Sets plane to zero.
	*/
	HE_INLINE HePlane& zero()
		{
		normal.zero();
		d = 0.0f;
		return *this;
		}

	HE_INLINE HePlane& set(HeF32 nx, HeF32 ny, HeF32 nz, HeF32 _d)
		{
		normal.set(nx, ny, nz);
		d = _d;
		return *this;
		}

	HE_INLINE HePlane& set(const HeVec3& _normal, HeF32 _d)
		{
		normal = _normal;
		d = _d;
		return *this;
		}

	HE_INLINE HePlane& set(const HeVec3& p, const HeVec3& _n)
		{
		normal = _n;
		// Plane equation: a*x + b*y + c*z + d = 0
		// p belongs to plane so:
		//     a*p.x + b*p.y + c*p.z + d = 0
		// <=> (n|p) + d = 0
		// <=> d = - (n|p)
		d = - p.dot(_n);
		return *this;
		}

	/**
	 \brief Computes the plane equation from 3 points.
	 */
	HePlane& set(const HeVec3& p0, const HeVec3& p1, const HeVec3& p2)
		{
		HeVec3 Edge0 = p1 - p0;
		HeVec3 Edge1 = p2 - p0;

		normal = Edge0.cross(Edge1);
		normal.normalize();

		// See comments in set() for computation of d
		d = -p0.dot(normal);

		return	*this;
		}

	HE_INLINE HeF32 distance(const HeVec3& p) const
		{
		// Valid for plane equation a*x + b*y + c*z + d = 0
		return p.dot(normal) + d;
		}

	HE_INLINE bool belongs(const HeVec3& p) const
		{
		return fabsf(distance(p)) < (1.0e-7f);
		}

	/**
	\brief projects p into the plane
	*/
	HE_INLINE HeVec3 project(const HeVec3 & p) const
		{
		// Pretend p is on positive side of plane, i.e. plane.distance(p)>0.
		// To project the point we have to go in a direction opposed to plane's normal, i.e.:
		return p - normal * distance(p);
//		return p + normal * distance(p);
		}

	/**
	\brief find an arbitrary point in the plane
	*/
	HE_INLINE HeVec3 pointInPlane() const
		{
		// Project origin (0,0,0) to plane:
		// (0) - normal * distance(0) = - normal * ((p|(0)) + d) = -normal*d
		return - normal * d;
//		return normal * d;
		}

	HE_INLINE void normalize()
		{
			HeF32 Denom = 1.0f / normal.magnitude();
			normal.x	*= Denom;
			normal.y	*= Denom;
			normal.z	*= Denom;
			d			*= Denom;
		}

	HE_INLINE operator HeVec3() const
		{
		return normal;
		}

	HE_INLINE void transform(const HeMat44 & transform, HePlane & transformed) const
		{
		transformed.normal = transform * normal;
    HeVec3 t( &transform.t.x);
		transformed.d = d - (t | transformed.normal);
		}

	HE_INLINE void inverseTransform(const HeMat44 & transform, HePlane & transformed) const
		{
		transformed.normal = transform % normal;
    HeVec3 t( &transform.t.x );
		HeVec3 it = transform%  t;
		transformed.d = d + (it | transformed.normal);
		}

	HeVec3	normal;		//!< The normal to the plane
	HeF32	d;			//!< The distance from the origin
	};

#endif
