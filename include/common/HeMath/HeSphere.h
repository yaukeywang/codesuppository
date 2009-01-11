#ifndef HE_FOUNDATION_NXSPHERE
#define HE_FOUNDATION_NXSPHERE

#include "../snippets/He.h"
#include "HeVec3.h"

/**
\brief Enum to control the sphere generation method from a set of points.
*/
enum HeBSphereMethod
	{
		HE_BS_NONE,
		HE_BS_GEMS,
		HE_BS_MINIBALL,

		HE_BS_FORCE_DWORD	= 0x7fffffff
	};

/**
\brief Represents a sphere defined by its center point and radius.
*/
class HeSphere
	{
	public:
	/**
	\brief Constructor
	*/
	HE_INLINE HeSphere()
		{
		}

	/**
	\brief Constructor
	*/
	HE_INLINE HeSphere(const HeVec3& _center, HeF32 _radius) : center(_center), radius(_radius)
		{
		}
#ifdef FOUNDATION_EXPORTS
	/**
	\brief Constructor
	*/
	HE_INLINE HeSphere(unsigned nb_verts, const HeVec3* verts)
		{
		HeComputeSphere(*this, nb_verts, verts);
		}
#endif
	/**
	\brief Copy constructor
	*/
	HE_INLINE HeSphere(const HeSphere& sphere) : center(sphere.center), radius(sphere.radius)
		{
		}
#ifdef FOUNDATION_EXPORTS

	/**
	\brief Union of spheres
	*/
	HE_INLINE HeSphere(const HeSphere& sphere0, const HeSphere& sphere1)
		{
		HeMergeSpheres(*this, sphere0, sphere1);
		}
#endif
	/**
	\brief Destructor
	*/
	HE_INLINE ~HeSphere()
		{
		}
#ifdef FOUNDATION_EXPORTS

	HE_INLINE HeBSphereMethod compute(unsigned nb_verts, const HeVec3* verts)
		{
		return HeComputeSphere(*this, nb_verts, verts);
		}

	HE_INLINE bool fastCompute(unsigned nb_verts, const HeVec3* verts)
		{
		return HeFastComputeSphere(*this, nb_verts, verts);
		}
#endif
	/**
	\brief Checks the sphere is valid.

	\return		true if the sphere is valid
	*/
	HE_INLINE bool IsValid() const
		{
		// Consistency condition for spheres: Radius >= 0.0f
		return radius >= 0.0f;
		}

	/**
	\brief Tests if a point is contained within the sphere.

	\param[in] p the point to test
	\return	true if inside the sphere
	*/
	HE_INLINE bool Contains(const HeVec3& p) const
		{
		return center.distanceSquared(p) <= radius*radius;
		}

	/**
	\brief Tests if a sphere is contained within the sphere.

	\param		sphere	[in] the sphere to test
	\return		true if inside the sphere
	*/
	HE_INLINE bool Contains(const HeSphere& sphere)	const
		{
		// If our radius is the smallest, we can't possibly contain the other sphere
		if(radius < sphere.radius)	return false;
		// So r is always positive or null now
		float r = radius - sphere.radius;
		return center.distanceSquared(sphere.center) <= r*r;
		}

	/**
	\brief Tests if a box is contained within the sphere.

	\param		min		[in] min value of the box
	\param		max		[in] max value of the box
	\return		true if inside the sphere
	*/
	HE_INLINE bool Contains(const HeVec3& min, const HeVec3& max) const
		{
		// I assume if all 8 box vertices are inside the sphere, so does the whole box.
		// Sounds ok but maybe there's a better way?
		HeF32 R2 = radius * radius;
		HeVec3 p;
		p.x=max.x; p.y=max.y; p.z=max.z;	if(center.distanceSquared(p)>=R2)	return false;
		p.x=min.x;							if(center.distanceSquared(p)>=R2)	return false;
		p.x=max.x; p.y=min.y;				if(center.distanceSquared(p)>=R2)	return false;
		p.x=min.x;							if(center.distanceSquared(p)>=R2)	return false;
		p.x=max.x; p.y=max.y; p.z=min.z;	if(center.distanceSquared(p)>=R2)	return false;
		p.x=min.x;							if(center.distanceSquared(p)>=R2)	return false;
		p.x=max.x; p.y=min.y;				if(center.distanceSquared(p)>=R2)	return false;
		p.x=min.x;							if(center.distanceSquared(p)>=R2)	return false;

		return true;
		}

	/**
	\brief Tests if the sphere intersects another sphere

	\param		sphere	[in] the other sphere
	\return		true if spheres overlap
	*/
	HE_INLINE bool Intersect(const HeSphere& sphere) const
		{
		HeF32 r = radius + sphere.radius;
		return center.distanceSquared(sphere.center) <= r*r;
		}

	HeVec3	center;		//!< Sphere's center
	HeF32	radius;		//!< Sphere's radius
	};

#endif
