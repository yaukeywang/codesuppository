#include "AwareManager.h"
#include "../snippets/UserMemAlloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

namespace SPATIAL_AWARENESS_SYSTEM
{

//!	Fills a buffer with a given dword.
//!	\param		addr	[in] buffer address
//!	\param		nb		[in] number of dwords to write
//!	\param		value	[in] the dword value
//!	\warning	writes nb*4 bytes !
inline void storeDwords(HeU32* dest,HeU32 nb,HeU32 value)
{
#ifdef WIN32
  _asm push eax
  _asm push ecx
  _asm push edi
  _asm mov edi, dest
  _asm mov ecx, nb
  _asm mov eax, value
  _asm rep stosd
  _asm pop edi
  _asm pop ecx
  _asm pop eax
#else
   while(nb--)	*dest++ = value;
#endif
}


inline HeU32 NxNextPowerOfTwo(HeU32 x)
{
  x |= (x >> 1);
  x |= (x >> 2);
  x |= (x >> 4);
  x |= (x >> 8);
  x |= (x >> 16);
  return x+1;
}


inline HeI32 hash32(HeI32 key)
{
  key += ~(key << 15);
  key ^=  (key >> 10);
  key +=  (key << 3);
  key ^=  (key >> 6);
  key += ~(key << 11);
  key ^=  (key >> 16);
  return key;
}

#define INVALID_USER_ID	0xffff

inline HeU32 hash(HeU16 id0, HeU16 id1) { return hash32( HeU32(id0)|(HeU32(id1)<<16) );	}
inline bool differentPair(const userPair& p, HeU16 id0, HeU16 id1)	{ return (id0!=p.id0) || (id1!=p.id1); }

#define INVALID_ID		0xffffffff

AwareManager::AwareManager(void) :
	hashSize		(0),
	mask			(0),
	hashTable		(NULL),
	next			(NULL),
	nbActivePairs	(0),
	activePairs		(NULL)
{
}

AwareManager::~AwareManager(void)
{
	purge();
}

void AwareManager::purge(void)
{
	MEMALLOC_FREE(next);
	MEMALLOC_FREE(activePairs);
	MEMALLOC_FREE(hashTable);
	hashSize		= 0;
	mask			= 0;
	nbActivePairs	= 0;
}

const userPair* AwareManager::findPair(HeU16 id0, HeU16 id1) const
{
	if(!hashTable)	return NULL;	// Nothing has been allocated yet

	// Compute hash value for this pair
	HeU32 hashValue = hash(id0, id1) & mask;

	// Look for it in the table
	HeU32 offset = hashTable[hashValue];

	while(offset!=INVALID_ID && differentPair(activePairs[offset], id0, id1))
	{
		assert(activePairs[offset].id0!=INVALID_USER_ID);
		offset = next[offset];		// Better to have a separate array for this
	}

	if(offset==INVALID_ID)
	  return NULL;

	assert(offset<nbActivePairs);

	// Match activePairs[offset] => the pair is persistent
	return &activePairs[offset];
}

// Internal version saving hash computation
inline userPair* AwareManager::findPair(HeU16 id0, HeU16 id1, HeU32 hashValue) const
{
	if(!hashTable)	return NULL;	// Nothing has been allocated yet

	// Look for it in the table
	HeU32 offset = hashTable[hashValue];
	while(offset!=INVALID_ID && differentPair(activePairs[offset], id0, id1))
	{
		assert(activePairs[offset].id0!=INVALID_USER_ID);
		offset = next[offset];		// Better to have a separate array for this
	}
	if(offset==INVALID_ID)	return NULL;
	assert(offset<nbActivePairs);
	// Match activePairs[offset] => the pair is persistent
	return &activePairs[offset];
}

const userPair* AwareManager::addPair(HeU16 id0, HeU16 id1, void* userData/*, const unsigned int* inputHashValue*/, bool overwriteData)
{

	// PT: advanced usage, skip this if we know it's useless. Only use if you know what you're doing.
	HeU32 hashValue;
	{
		hashValue = hash(id0, id1) & mask;

		userPair* P = findPair(id0, id1, hashValue);
		if(P)
		{
			if(overwriteData)
				P->userData = userData;
			return P;	// Persistent pair
		}
	}

	// This is a new pair
	if(nbActivePairs>=hashSize)
	{
		// Get more entries
		hashSize = NxNextPowerOfTwo(nbActivePairs+1);
		mask = hashSize-1;

		MEMALLOC_FREE(hashTable);
		hashTable = (HeU32*)MEMALLOC_MALLOC_TYPE(hashSize*sizeof(HeU32),"hashTable",__FILE__,__LINE__);
		storeDwords(hashTable, hashSize, INVALID_ID);

		// Get some bytes for new entries
		userPair* newPairs	= (userPair*)MEMALLOC_MALLOC_TYPE(hashSize * sizeof(userPair),"userPairs",__FILE__,__LINE__);
		assert(newPairs);
		HeU32* newNext		= (HeU32*)MEMALLOC_MALLOC_TYPE(hashSize * sizeof(HeU32),"newNext",__FILE__,__LINE__);
		assert(newNext);

		// Copy old data if needed
		if (nbActivePairs)
		  memcpy(newPairs, activePairs, nbActivePairs*sizeof(userPair));

		for(HeU32 i=0;i<nbActivePairs;i++)
		{
			HeU32 hashValue = hash(activePairs[i].id0, activePairs[i].id1) & mask;	// New hash value with new mask
			newNext[i] = hashTable[hashValue];
			hashTable[hashValue] = i;
		}

		// Delete old data
		MEMALLOC_FREE(next);
		MEMALLOC_FREE(activePairs);

		// Assign new pointer
		activePairs = newPairs;
		next        = newNext;

		// Recompute hash value with new hash size
		hashValue = hash(id0, id1) & mask;
	}
	activePairs[nbActivePairs].id0 = id0;	// ### CMOVs would be nice here
	activePairs[nbActivePairs].id1 = id1;
	activePairs[nbActivePairs].userData = userData;
	next[nbActivePairs] = hashTable[hashValue];
	hashTable[hashValue] = nbActivePairs;
	return &activePairs[nbActivePairs++];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool AwareManager::removePair(HeU16 id0, HeU16 id1, const void** userData)
{

	HeU32 hashValue = hash(id0, id1) & mask;
	const userPair* P = findPair(id0, id1, hashValue);
	if(!P)	return false;
	assert(P->id0==id0);
	assert(P->id1==id1);

	// Return user-data from deleted pair, in case the user needs it
	if(userData)	*userData = P->userData;

	const HeU32 pairIndex = getPairIndex(P);

	// Walk the hash table to fix mNext
	HeU32 offset = hashTable[hashValue];
	assert(offset!=INVALID_ID);

	HeU32 previous=INVALID_ID;
	while(offset!=pairIndex)
	{
		previous = offset;
		offset = next[offset];
	}

	// Let us go/jump us
	if(previous!=INVALID_ID)
	{
		assert(next[previous]==pairIndex);
		next[previous] = next[pairIndex];
	}
	// else we were the first
	else hashTable[hashValue] = next[pairIndex];
	// we're now free to reuse next[pairIndex] without breaking the list


	// Fill holes
	{
		// 1) Remove last pair
		const HeU32 lastPairIndex = nbActivePairs-1;
		if(lastPairIndex==pairIndex)
		{
			nbActivePairs--;
			return true;
		}

		const userPair* last = &activePairs[lastPairIndex];
		const HeU32 lastHashValue = hash(last->id0, last->id1) & mask;

		// Walk the hash table to fix mNext
		HeU32 offset = hashTable[lastHashValue];
		assert(offset!=INVALID_ID);

		HeU32 previous=INVALID_ID;
		while(offset!=lastPairIndex)
		{
			previous = offset;
			offset = next[offset];
		}

		// Let us go/jump us
		if(previous!=INVALID_ID)
		{
			assert(next[previous]==lastPairIndex);
			next[previous] = next[lastPairIndex];
		}
		// else we were the first
		else
    {
		  hashTable[lastHashValue] = next[lastPairIndex];
    }
		// we're now free to reuse mNext[LastPairIndex] without breaking the list

		activePairs[pairIndex] = activePairs[lastPairIndex];
		next[pairIndex] = hashTable[lastHashValue];
		hashTable[lastHashValue] = pairIndex;

		nbActivePairs--;
	}
	return true;
}

}; // END OF NAMESPACE
