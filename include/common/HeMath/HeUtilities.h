#ifndef HE_FOUNDATION_NXUTILITIES
#define HE_FOUNDATION_NXUTILITIES

#include "Hef.h"
#include <string.h>
#include "HeVec3.h"
#include "HeBounds3.h"

/**
 \brief Utility calls that don't fit anywhere else.

 Gathers elements seperated by stride byes into source.

 \param src Source memory block.
 \param dst Destination memory block.
 \param nbElem Number of elements to copy.
 \param elemSize Size of each element.
 \param stride Number of bytes from one element to the next.
*/

HE_INLINE void HeFlexiCopy(const void* src, void* dst, HeU32 nbElem, HeU32 elemSize, HeU32 stride)
{
	const HeU8* s = (const HeU8*)src;
	HeU8* d = (HeU8*)dst;
	while(nbElem--)
	{
		memcpy(d, s, elemSize);
		d += elemSize;
		s += stride;
	}
}

		/*
			Find next power of 2.

		*/
HE_INLINE HeU32 HeNextPowerOfTwo(HeU32 x)
{
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
  return x+1;
}

/**
	\brief Returns the angle between two (possibly un-normalized) vectors
	\param v0 First Vector.
	\param v1 Second Vector.
	*/
HE_INLINE HeF32 HeAngle(const HeVec3& v0, const HeVec3& v1)
{
	HeF32 cos = v0|v1;					// |v0|*|v1|*Cos(Angle)
	HeF32 sin = (v0^v1).magnitude();	// |v0|*|v1|*Sin(Angle)
	return HeMath::atan2(sin, cos);
}

/**
	\brief Make an edge longer by a factor of its length.

	\param p0 First point of edge.
	\param p1 Second point of edge.
	\param fatCoeff Factor by which to make fatter by.
*/
HE_INLINE void HeMakeFatEdge(HeVec3& p0, HeVec3& p1, HeF32 fatCoeff)
{
	HeVec3 delta = p1 - p0;
	delta.setMagnitude(fatCoeff);
	p0 -= delta;
	p1 += delta;
}

/**

	\param normalCompo
	\param outwardDir
	\param outwardNormal
*/

HE_INLINE void HeComputeNormalCompo(HeVec3& normalCompo, const HeVec3& outwardDir, const HeVec3& outwardNormal)
{
	normalCompo = outwardNormal * (outwardDir|outwardNormal);
}

/**

	\param outwardDir
	\param outwardNormal
*/
HE_INLINE void HeComputeTangentCompo(HeVec3& outwardDir, const HeVec3& outwardNormal)
{
	outwardDir -= outwardNormal * (outwardDir|outwardNormal);
}

/**

	\param normalCompo
	\param tangentCompo
	\param outwardDir
	\param outwardNormal
*/
HE_INLINE void HeDecomposeVector(HeVec3& normalCompo, HeVec3& tangentCompo, const HeVec3& outwardDir, const HeVec3& outwardNormal)
{
	normalCompo = outwardNormal * (outwardDir|outwardNormal);
	tangentCompo = outwardDir - normalCompo;
}

/**
	\brief Computes a point on a triangle using barycentric coordinates.

	It's only been extracted as a function so that there's no confusion regarding the order in
	which u and v should be used.

	pt = (1 - u - v) * p0 + u * p1 + v * p2

	\param pt Contains the computed point.
	\param p0 First point of triangle.
	\param p1 Second.
	\param p2 Third.
	\param u U parameter.
	\param v V parameter.
*/
HE_INLINE void HeComputeBarycentricPoint(HeVec3& pt, const HeVec3& p0, const HeVec3& p1, const HeVec3& p2, float u, float v)
{
	HeF32 w = 1.0f - u - v;
	pt.x = w*p0.x + u*p1.x + v*p2.x;
	pt.y = w*p0.y + u*p1.y + v*p2.y;
	pt.z = w*p0.z + u*p1.z + v*p2.z;
}


HE_INLINE void HeCatmullRom(HeVec3 &out_vector,const HeVec3 &p1,const HeVec3 &p2,const HeVec3 &p3,const HeVec3 &p4,const HeF32 s)
{
  HeF32 s_squared = s * s;
  HeF32 s_cubed = s_squared * s;

  HeF32 coefficient_p1 = -s_cubed + 2*s_squared - s;
  HeF32 coefficient_p2 = 3 * s_cubed - 5 * s_squared + 2;
  HeF32 coefficient_p3 = -3 * s_cubed +4 * s_squared + s;
  HeF32 coefficient_p4 = s_cubed - s_squared;

  out_vector[0] = (coefficient_p1 * p1[0] + coefficient_p2 * p2[0] + coefficient_p3 * p3[0] + coefficient_p4 * p4[0])*0.5f;
  out_vector[1] = (coefficient_p1 * p1[1] + coefficient_p2 * p2[1] + coefficient_p3 * p3[1] + coefficient_p4 * p4[1])*0.5f;
  out_vector[2] = (coefficient_p1 * p1[2] + coefficient_p2 * p2[2] + coefficient_p3 * p3[2] + coefficient_p4 * p4[2])*0.5f;
}


#endif
