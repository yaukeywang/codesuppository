#ifndef HE_FOUNDATION_NXCAPSULE
#define HE_FOUNDATION_NXCAPSULE

#include "../snippets/He.h"
#include "HeSegment.h"
#include "HeSphere.h"

class HeCapsule;
class HeBox;

/**
\brief Represents a capsule.
*/
class HeCapsule : public HeSegment
	{
	public:
	/**
	\brief Constructor
	*/
	HE_INLINE HeCapsule()
		{
		}

	/**
	\brief Constructor
	
	\param seg Line segment to create capsule from.
	\param _radius Radius of the capsule.
	*/
	HE_INLINE HeCapsule(const HeSegment& seg, HeF32 _radius) : HeSegment(seg), radius(_radius)
		{
		}

	/**
	\brief Destructor
	*/
	HE_INLINE ~HeCapsule()
		{
		}

#ifdef FOUNDATION_EXPORTS

	/**
	 Computes an OBB surrounding the capsule.
	 \param		box		[out] the OBB
	 */
	HE_INLINE void computeOBB(HeBox& box) const
		{
		HeComputeBoxAroundCapsule(*this, box);
		}
	/**
	 Tests if a point is contained within the capsule.
	 \param		pt	[in] the point to test
	 \return	true if inside the capsule
	 \warning	point and capsule must be in same space
	 */
	HE_INLINE bool contains(const HeVec3& pt) const
		{
		return squareDistance(pt) <= radius*radius;
		}

	/**
	 Tests if a sphere is contained within the capsule.
	 \param		sphere	[in] the sphere to test
	 \return	true if inside the capsule
	 \warning	sphere and capsule must be in same space
	 */
	HE_INLINE bool contains(const HeSphere& sphere) const
		{
		HeF32 d = radius - sphere.radius;
		if(d>=0.0f)	return squareDistance(sphere.center) <= d*d;
		else		return false;
		}

	/**
	 Tests if a capsule is contained within the capsule.
	 \param		capsule	[in] the capsule to test
	 \return	true if inside the capsule
	 \warning	both capsule must be in same space
	 */
	HE_INLINE bool contains(const HeCapsule& capsule) const
		{
		// We check the capsule contains the two spheres at the start and end of the sweep
		return contains(HeSphere(capsule.p0, capsule.radius)) && contains(HeSphere(capsule.p1, capsule.radius));
		}
#endif

	HeF32	radius;
	};

#endif
