#ifndef HASHFUNCTION_H
#define HASHFUNCTION_H


#include "NxSimpleTypes.h"

// Disable pointer trnc warning as we do this on purpose.
#pragma warning(push)
#pragma warning(disable: 4311)

/*!
Central definition of hash functions
*/

// Hash functions
template<class T>
NxU32 HashFunction(const T& key)
{
	return (NxU32)key;
}

// Thomas Wang's 32 bit mix
// http://www.cris.com/~Ttwang/tech/inthash.htm
template<>
inline NxU32 HashFunction<NxU32>(const NxU32& key)
{
	NxU32 k = key;
	k += ~(k << 15);
	k ^= (k >> 10);
	k += (k << 3);
	k ^= (k >> 6);
	k += ~(k << 11);
	k ^= (k >> 16);
	return (NxU32)k;
}

template<>
inline NxU32 HashFunction<NxI32>(const NxI32& key)
{
	return HashFunction<NxU32>((NxU32)key);
}

// Thomas Wang's 64 bit mix
// http://www.cris.com/~Ttwang/tech/inthash.htm
template<>
inline NxU32 HashFunction<NxU64>(const NxU64& key)
{
	NxU64 k = key;
	k += ~(k << 32);
	k ^= (k >> 22);
	k += ~(k << 13);
	k ^= (k >> 8);
	k += (k << 3);
	k ^= (k >> 15);
	k += ~(k << 27);
	k ^= (k >> 31);
	return (NxU32)k;
}

// Helper for pointer hashing
template<NxI32 size>
NxU32 PointerHash(void* ptr);

template<>
inline NxU32 PointerHash<4>(void* ptr)
{
	return HashFunction<NxU32>(reinterpret_cast<NxU32>(ptr));
}


template<>
inline NxU32 PointerHash<8>(void* ptr)
{
	return HashFunction<NxU32>(reinterpret_cast<NxU32>(ptr));
}

// Hash function for pointers
template<class T>
inline NxU32 HashFunction(T* key)
{
	return PointerHash<sizeof(const void*)>(key);
}

#pragma warning(pop)

#endif
