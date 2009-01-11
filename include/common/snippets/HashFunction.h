#ifndef HASHFUNCTION_H
#define HASHFUNCTION_H


#include "HeSimpleTypes.h"

// Disable pointer trnc warning as we do this on purpose.
#pragma warning(push)
#pragma warning(disable: 4311)

/*!
Central definition of hash functions
*/

// Hash functions
template<class T>
HeU32 HashFunction(const T& key)
{
	return (HeU32)key;
}

// Thomas Wang's 32 bit mix
// http://www.cris.com/~Ttwang/tech/inthash.htm
template<>
inline HeU32 HashFunction<HeU32>(const HeU32& key)
{
	HeU32 k = key;
	k += ~(k << 15);
	k ^= (k >> 10);
	k += (k << 3);
	k ^= (k >> 6);
	k += ~(k << 11);
	k ^= (k >> 16);
	return (HeU32)k;
}

template<>
inline HeU32 HashFunction<HeI32>(const HeI32& key)
{
	return HashFunction<HeU32>((HeU32)key);
}

// Thomas Wang's 64 bit mix
// http://www.cris.com/~Ttwang/tech/inthash.htm
template<>
inline HeU32 HashFunction<HeU64>(const HeU64& key)
{
	HeU64 k = key;
	k += ~(k << 32);
	k ^= (k >> 22);
	k += ~(k << 13);
	k ^= (k >> 8);
	k += (k << 3);
	k ^= (k >> 15);
	k += ~(k << 27);
	k ^= (k >> 31);
	return (HeU32)k;
}

// Helper for pointer hashing
template<int size>
HeU32 PointerHash(void* ptr);

template<>
inline HeU32 PointerHash<4>(void* ptr)
{
	return HashFunction<HeU32>(reinterpret_cast<HeU32>(ptr));
}


template<>
inline HeU32 PointerHash<8>(void* ptr)
{
	return HashFunction<HeU32>(reinterpret_cast<HeU32>(ptr));
}

// Hash function for pointers
template<class T>
inline HeU32 HashFunction(T* key)
{
	return PointerHash<sizeof(const void*)>(key);
}

#pragma warning(pop)

#endif
