#ifndef AWARE_MANAGER_H
#define AWARE_MANAGER_H

#include "../snippets/HeSimpleTypes.h"

namespace SPATIAL_AWARENESS_SYSTEM
{

#ifndef NULL
#define NULL 0
#endif


#pragma pack(1)
struct userPair
{
	HeU16	id0;
	HeU16	id1;
	void*	userData;
};
#pragma pack()

class AwareManager
{
public:
	AwareManager(void);
	~AwareManager(void);

	void			purge(void);

	const userPair*	addPair(HeU16 id0, HeU16 id1, void* userData, bool overwrite_data=true);
	bool		      	removePair(HeU16 id0, HeU16 id1, const void** userData=NULL);
	const userPair*	findPair(HeU16 id0, HeU16 id1)	const;

	inline	HeU32 getPairIndex(const userPair* pair)	const
  {
    return HeU32((size_t(pair) - size_t(activePairs))>>3);
  }

private:
	HeU32			hashSize;
	HeU32			mask;
	HeU32			nbActivePairs;		//number of pairs
	HeU32     *hashTable;
	HeU32     *next;
	userPair         *activePairs;		//dense arrat of pairs (if fill holes is defined)

	inline	userPair*		findPair(HeU16 id0, HeU16 id1, HeU32 hashValue) const;
};

}; // END OF NAMESPACE

#endif
