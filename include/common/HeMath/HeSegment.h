#ifndef HE_FOUNDATION_NXSEGMENT
#define HE_FOUNDATION_NXSEGMENT

#include "Hef.h"
#include "HeVec3.h"


class HeSegment;

/**
\brief Represents a line segment.

*/
class HeSegment
	{
	public:
	/**
	\brief Constructor
	*/
	HE_INLINE HeSegment()
		{
		}

	/**
	\brief Constructor
	*/
	HE_INLINE HeSegment(const HeVec3& _p0, const HeVec3& _p1) : p0(_p0), p1(_p1)
		{
		}

	/**
	\brief Copy constructor
	*/
	HE_INLINE HeSegment(const HeSegment& seg) : p0(seg.p0), p1(seg.p1)
		{
		}

	/**
	\brief Destructor
	*/
	HE_INLINE ~HeSegment()
		{
		}

	HE_INLINE const HeVec3& getOrigin() const
		{
		return p0;
		}

	HE_INLINE HeVec3 computeDirection() const
		{
		return p1 - p0;
		}

	HE_INLINE void computeDirection(HeVec3& dir) const
		{
		dir = p1 - p0;
		}

	HE_INLINE HeF32 computeLength() const
		{
		return p1.distance(p0);
		}

	HE_INLINE HeF32 computeSquareLength() const
		{
		return p1.distanceSquared(p0);
		}

	HE_INLINE void setOriginDirection(const HeVec3& origin, const HeVec3& direction)
		{
		p0 = p1 = origin;
		p1 += direction;
		}

	/**
	\brief Computes a point on the segment

	\param[out] pt point on segment
	\param[in] t point's parameter [t=0 => pt = mP0, t=1 => pt = mP1]
	 */
	HE_INLINE void computePoint(HeVec3& pt, HeF32 t) const
		{
		pt = p0 + t * (p1 - p0);
		}
#ifdef FOUNDATION_EXPORTS

	HE_INLINE HeF32 squareDistance(const HeVec3& point, HeF32* t=NULL) const
		{
		return HeComputeSquareDistance(*this, point, t);
		}

	HE_INLINE HeF32 distance(const HeVec3& point, HeF32* t=NULL) const
		{
		return sqrtf(squareDistance(point, t));
		}
#endif

	HeVec3	p0;		//!< Start of segment
	HeVec3	p1;		//!< End of segment
	};

#endif
