#ifndef HE_FOUNDATION_NXI16VEC3
#define HE_FOUNDATION_NXI16VEC3

#include "../snippets/He.h"
#include "HeVec3.h"

class HeI16Vec3
{

public:

	HeI16 x;
	HeI16 y;
	HeI16 z;

	HE_INLINE HeI16Vec3()
	{
	}

	HE_INLINE HeI16Vec3(const HeI16Vec3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	HE_INLINE HeI16Vec3(HeI16 _x, HeI16 _y, HeI16 _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	HE_INLINE bool operator==(const HeI16Vec3& v) const
	{
		return ((x == v.x) && (y == v.y) && (z == v.z));
	}

	HE_INLINE bool operator!=(const HeI16Vec3& v) const
	{
		return ((x != v.x) || (y != v.y) || (z != v.z));
	}

	HE_INLINE const HeI16Vec3 operator+(const HeI16Vec3& v)
	{
		HeI16Vec3 res;
		res.x = x + v.x;
		res.y = y + v.y;
		res.z = z + v.z;
		return res;
	}

	HE_INLINE const HeI16Vec3& operator+=(const HeI16Vec3& v)
	{
		x = (HeI16)v.x+x;
		y = (HeI16)v.y+y;
		z = (HeI16)v.z+z;
		return *this;
	}

	HE_INLINE const HeI16Vec3& operator=(const HeI16Vec3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}
	
	HE_INLINE const HeI16Vec3 operator << ( const HeU32 shift) const
	{
		HeI16Vec3 res;
		res.x = (HeI16)(x << shift);
		res.y = (HeI16)(y << shift);
		res.z = (HeI16)(z << shift);
		return res;
	}

	HE_INLINE const HeI16Vec3 operator >> ( const HeU32 shift) const
	{
		HeI16Vec3 res;
		res.x = (HeI16)(x >> shift);
		res.y = (HeI16)(y >> shift);
		res.z = (HeI16)(z >> shift);
		return res;
	}

	HE_INLINE const HeI16Vec3& operator <<= ( const HeU32 shift)
	{
		x <<= shift;
		y <<= shift;
		z <<= shift;
		return *this;
	}

	HE_INLINE const HeI16Vec3& operator >>= ( const HeU32 shift)
	{
		x >>= shift;
		y >>= shift;
		z >>= shift;
		return *this;
	}

	HE_INLINE void set( const HeVec3& realVec, HeReal scale)
	{
		x = static_cast<HeI16>(HeMath::floor(realVec.x * scale));
		y = static_cast<HeI16>(HeMath::floor(realVec.y * scale));
		z = static_cast<HeI16>(HeMath::floor(realVec.z * scale));
	}

	HE_INLINE void set( const HeVec3& realVec)
	{
		x = static_cast<HeI16>(HeMath::floor(realVec.x));
		y = static_cast<HeI16>(HeMath::floor(realVec.y));
		z = static_cast<HeI16>(HeMath::floor(realVec.z));
	}

	HE_INLINE void set( HeI16 _x, HeI16 _y, HeI16 _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	HE_INLINE void zero()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	HE_INLINE bool isZero()
	{
		return x == 0 && y == 0 && z == 0;
	}
};

#endif
