#ifndef HE_FOUNDATION_NXRAY
#define HE_FOUNDATION_NXRAY

#include "../snippets/He.h"
#include "HeVec3.h"

class HeRay;

/**
\brief Represents an infinite ray as an origin and direction.

The direction should be normalized.
*/
class HeRay
	{
	public:
	/**
	Constructor
	*/
	HE_INLINE HeRay()
		{
		}

	/**
	Constructor
	*/
	HE_INLINE HeRay(const HeVec3& _orig, const HeVec3& _dir) : orig(_orig), dir(_dir)
		{
		}

	/**
	Destructor
	*/
	HE_INLINE ~HeRay()
		{
		}
#ifdef FOUNDATION_EXPORTS

	HE_INLINE HeF32 distanceSquared(const HeVec3& point, HeF32* t=NULL) const
		{
		return HeComputeDistanceSquared(*this, point, t);
		}

	HE_INLINE HeF32 distance(const HeVec3& point, HeF32* t=NULL) const
		{
		return sqrtf(distanceSquared(point, t));
		}
#endif

	HeVec3	orig;	//!< Ray origin
	HeVec3	dir;	//!< Normalized direction
	};

	HE_INLINE void ComputeReflexionVector(HeVec3& reflected, const HeVec3& incoming_dir, const HeVec3& outward_normal)
	{
		reflected = incoming_dir - outward_normal * 2.0f * incoming_dir.dot(outward_normal);
	}

	HE_INLINE void ComputeReflexionVector(HeVec3& reflected, const HeVec3& source, const HeVec3& impact, const HeVec3& normal)
	{
		HeVec3 V = impact - source;
		reflected = V - normal * 2.0f * V.dot(normal);
	}

	HE_INLINE void ComputeNormalCompo(HeVec3& normal_compo, const HeVec3& outward_dir, const HeVec3& outward_normal)
	{
		normal_compo = outward_normal * outward_dir.dot(outward_normal);
	}

	HE_INLINE void ComputeTangentCompo(HeVec3& outward_dir, const HeVec3& outward_normal)
	{
		outward_dir -= outward_normal * outward_dir.dot(outward_normal);
	}

	HE_INLINE void DecomposeVector(HeVec3& normal_compo, HeVec3& tangent_compo, const HeVec3& outward_dir, const HeVec3& outward_normal)
	{
		normal_compo = outward_normal * outward_dir.dot(outward_normal);
		tangent_compo = outward_dir - normal_compo;
	}

#endif
