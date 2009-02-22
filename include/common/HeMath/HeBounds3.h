#ifndef HE_FOUNDATION_NXBOUNDS3
#define HE_FOUNDATION_NXBOUNDS3

#include "Hef.h"
#include "HeVec3.h"
#include "HeMat44.h"

/**
	\brief Class representing 3D range or axis aligned bounding box.

	Stored as minimum and maximum extent corners. Alternate representation
	would be center and dimensions.
	May be empty or nonempty. If not empty, min <= max has to hold.
*/
class HeBounds3
	{
	public:
	HE_INLINE HeBounds3();
	HE_INLINE ~HeBounds3();
  HE_INLINE HeBounds3(const HeVec3 &bmin,const HeVec3 &bmax);

	/**
	\brief Sets empty to true
	*/
	HE_INLINE void setEmpty();

	/**
	\brief Sets infinite bounds
	*/
	HE_INLINE void setInfinite();
	
	/**
	\brief low level assignment.

	\param minx Minimum X value
	\param miny Minimum Y value
	\param minz Minimum Z value
	\param maxx Maximum X value
	\param maxy Maximum Y value
	\param maxz Maximum Z value
	*/
	HE_INLINE void set(HeReal minx, HeReal miny, HeReal minz, HeReal maxx, HeReal maxy,HeReal maxz);

	/**
	\brief vector assignment.

	\param min Minimum point of bounds.
	\param max Maximum point of bounds.
	*/
	HE_INLINE void set(const HeVec3& min, const HeVec3& max);

	/**
	\brief expands the volume to include v

	\param v Point to expand to.
	*/
	HE_INLINE void include(const HeVec3& v);

	/**
	\brief sets this to the union of this and b2.

	\param b2 Bounds to perform union with.
	*/
	HE_INLINE void combine(const HeBounds3& b2);

	/**
	\brief sets this to the AABB of the OBB passed.

	\param orientation Orientation of the OBB.
	\param translation Translation of the OBB.
	\param halfDims radii of the OBB.
	*/
	HE_INLINE void boundsOfOBB(const HeMat44& orientation, const HeVec3& translation, const HeVec3& halfDims);

	/**
	\brief transforms this volume as if it was an axis aligned bounding box, and then assigns the results' bounds to this.

	\param orientation Orientation to apply.
	\param translation Translation to apply(applied after orientation transform)
	*/
	HE_INLINE void transform(const HeMat44& orientation, const HeVec3& translation);

	HE_INLINE bool isEmpty() const;

	/**
	\brief indicates whether the intersection of this and b is empty or not.

	\param b Bounds to test for intersection.
	*/
	HE_INLINE bool intersects(const HeBounds3& b) const;

	/**
	\brief indicates whether the intersection of this and b is empty or not in the plane orthogonal to the axis passed (X = 0, Y = 1 or Z = 2).

	\param b Bounds to test for intersection.
	\param axisToIgnore Axis to ignore when performing the intersection test.
	*/
	HE_INLINE bool intersects2D(const HeBounds3& b, unsigned axisToIgnore) const;

	/**
	\brief indicates if these bounds contain v.

	\param v Point to test against bounds.
	*/
	HE_INLINE bool contain(const HeVec3& v) const;

	/**
	\brief returns the center of this axis aligned box.

	\param center The center of the bounds.
	*/
	HE_INLINE void getCenter(HeVec3& center) const;

	/**
	\brief returns the dimensions (width/height/depth) of this axis aligned box.

	\param dims The dimensions of the bounds.
	*/
	HE_INLINE void getDimensions(HeVec3& dims) const;

	/**
	\brief returns the extents, which are half of the width/height/depth.

	\param extents The extents/radii of the bounds.
	*/
	HE_INLINE void getExtents(HeVec3& extents) const;

	/**
	\brief setups an AABB from center & extents vectors.

	\param c Center vector
	\param e Extents vector
	*/
	HE_INLINE void setCenterExtents(const HeVec3& c, const HeVec3& e);

	/**
	\brief scales the AABB.

	\param scale Factor to scale AABB by.
	*/
	HE_INLINE void scale(HeF32 scale);

	/**
	fattens the AABB in all 3 dimensions by the given distance.
	*/
	HE_INLINE void fatten(HeReal distance);


	//HE_INLINE void combine(HeReal extension);

  HE_INLINE HeF32 getDiagonalDistance(void) const;

	HeVec3 min, max;
	};


HE_INLINE HeBounds3::HeBounds3()
	{
	// Default to empty boxes for compatibility TODO: PT: remove this if useless
	setEmpty();
	}


HE_INLINE HeBounds3::~HeBounds3()
	{
	//nothing
	}


HE_INLINE void HeBounds3::setEmpty()
	{
	// We know use this particular pattern for empty boxes
	set(HE_MAX_REAL, HE_MAX_REAL, HE_MAX_REAL,
		HE_MIN_REAL, HE_MIN_REAL, HE_MIN_REAL);
	}

HE_INLINE void HeBounds3::setInfinite()
	{
	set(HE_MIN_REAL, HE_MIN_REAL, HE_MIN_REAL,
		HE_MAX_REAL, HE_MAX_REAL, HE_MAX_REAL);
	}

HE_INLINE void HeBounds3::set(HeReal minx, HeReal miny, HeReal minz, HeReal maxx, HeReal maxy,HeReal maxz)
	{
	min.set(minx, miny, minz);
	max.set(maxx, maxy, maxz);
	}

HE_INLINE void HeBounds3::set(const HeVec3& _min, const HeVec3& _max)
	{
	min = _min;
	max = _max;
	}

HE_INLINE void HeBounds3::include(const HeVec3& v)
	{
	max.vmax(v);
	min.vmin(v);
	}

HE_INLINE void HeBounds3::combine(const HeBounds3& b2)
	{
	// - if we're empty, min = MAX,MAX,MAX => min will be b2 in all cases => it will copy b2, ok
	// - if b2 is empty, the opposite happens => keep us unchanged => ok
	// => same behavior as before, automatically
	min.vmin(b2.min);
	max.vmax(b2.max);
	}

HE_INLINE void HeBounds3::boundsOfOBB(const HeMat44& orientation, const HeVec3& translation, const HeVec3& halfDims)
	{
	HeReal dimx = halfDims[0];
	HeReal dimy = halfDims[1];
	HeReal dimz = halfDims[2];

	HeReal x = HeMath::abs(orientation(0,0) * dimx) + HeMath::abs(orientation(0,1) * dimy) + HeMath::abs(orientation(0,2) * dimz);
	HeReal y = HeMath::abs(orientation(1,0) * dimx) + HeMath::abs(orientation(1,1) * dimy) + HeMath::abs(orientation(1,2) * dimz);
	HeReal z = HeMath::abs(orientation(2,0) * dimx) + HeMath::abs(orientation(2,1) * dimy) + HeMath::abs(orientation(2,2) * dimz);

	set(-x + translation[0], -y + translation[1], -z + translation[2], x + translation[0], y + translation[1], z + translation[2]);
	}

HE_INLINE void HeBounds3::transform(const HeMat44& orientation, const HeVec3& translation)
{
	// convert to center and extents form
	HeVec3 center, extents;
	getCenter(center);
	getExtents(extents);
	center = orientation * center + translation;
	boundsOfOBB(orientation, center, extents);
}

HE_INLINE bool HeBounds3::isEmpty() const
	{
	// Consistency condition for (Min, Max) boxes: min < max
	// TODO: PT: should we test against the explicit pattern ?
	if(min.x < max.x)	return false;
	if(min.y < max.y)	return false;
	if(min.z < max.z)	return false;
	return true;
	}

HE_INLINE bool HeBounds3::intersects(const HeBounds3& b) const
	{
	if ((b.min.x > max.x) || (min.x > b.max.x)) return false;
	if ((b.min.y > max.y) || (min.y > b.max.y)) return false;
	if ((b.min.z > max.z) || (min.z > b.max.z)) return false;
	return true;
	}

HE_INLINE bool HeBounds3::intersects2D(const HeBounds3& b, unsigned axis) const
	{
	// TODO: PT: could be static and like this:
	// static unsigned i[3] = { 1,2,0,1 };
	// const unsigned ii = i[axis];
	// const unsigned jj = i[axis+1];
	const unsigned i[3] = { 1,0,0 };
	const unsigned j[3] = { 2,2,1 };
	const unsigned ii = i[axis];
	const unsigned jj = j[axis];
	if ((b.min[ii] > max[ii]) || (min[ii] > b.max[ii])) return false;
	if ((b.min[jj] > max[jj]) || (min[jj] > b.max[jj])) return false;
	return true;
	}

HE_INLINE bool HeBounds3::contain(const HeVec3& v) const
	{
	if ((v.x < min.x) || (v.x > max.x)) return false;
	if ((v.y < min.y) || (v.y > max.y)) return false;
	if ((v.z < min.z) || (v.z > max.z)) return false;
	return true;
	}

HE_INLINE void HeBounds3::getCenter(HeVec3& center) const
	{
	center.add(min,max);
	center *= HeReal(0.5);
	}

HE_INLINE void HeBounds3::getDimensions(HeVec3& dims) const
	{
	dims.subtract(max,min);
	}

HE_INLINE void HeBounds3::getExtents(HeVec3& extents) const
	{
	extents.subtract(max,min);
	extents *= HeReal(0.5);
	}

HE_INLINE void HeBounds3::setCenterExtents(const HeVec3& c, const HeVec3& e)
	{
	min = c - e;
	max = c + e;
	}

HE_INLINE void HeBounds3::scale(HeF32 scale)
	{
	HeVec3 center, extents;
	getCenter(center);
	getExtents(extents);
	setCenterExtents(center, extents * scale);
	}

HE_INLINE void HeBounds3::fatten(HeReal distance)
	{
	min.x -= distance;
	min.y -= distance;
	min.z -= distance;

	max.x += distance;
	max.y += distance;
	max.z += distance;
	}

HE_INLINE HeBounds3::HeBounds3(const HeVec3 &bmin,const HeVec3 &bmax)
{
  min = bmin;
  max = bmax;
}

HE_INLINE HeF32 HeBounds3::getDiagonalDistance(void) const
{
  return min.distance(max);
}

#endif
