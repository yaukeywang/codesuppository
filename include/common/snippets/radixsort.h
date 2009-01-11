///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains source code from the article "Radix Sort Revisited".
 *	\file		IceRevisitedRadix.h
 *	\author		Pierre Terdiman
 *	\date		April, 4, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef RADIX_SORT_H

#define RADIX_SORT_H

#include "common/snippets/UserMemAlloc.h"

enum RadixHint
{
	RADIX_SIGNED,		//!< Input values are signed
	RADIX_UNSIGNED,		//!< Input values are unsigned
	RADIX_FORCE_DWORD = 0x7fffffff
};

class RadixSort
{
	public:
		// Constructor/Destructor
		RadixSort();
		~RadixSort();

		// Sorting methods
		RadixSort&		Sort(const HeU32* input, HeU32 nb, RadixHint hint=RADIX_SIGNED);
		RadixSort&		Sort(const HeF32* input,HeU32 nb);

		//! Access to results. mRanks is a list of indices in sorted order, i.e. in the order you may further process your data
		inline	const HeU32*	GetRanks()			const	{ return mRanks;		}

		//! mIndices2 gets trashed on calling the sort routine, but otherwise you can recycle it the way you want.
		inline	HeU32*			GetRecyclable()		const	{ return mRanks2;		}

		// Stats
		HeU32			GetUsedRam()		const;
		//! Returns the total number of calls to the radix sorter.
		inline	HeU32			GetNbTotalCalls()	const	{ return mTotalCalls;	}
		//! Returns the number of eraly exits due to temporal coherence.
		inline	HeU32			GetNbHits()			const	{ return mNbHits;		}

		bool			SetRankBuffers(HeU32* ranks0, HeU32* ranks1);

private:

		HeU32			mCurrentSize;		//!< Current size of the indices list
		HeU32*			mRanks;				//!< Two lists, swapped each pass
		HeU32*			mRanks2;
		// Stats
		HeU32			mTotalCalls;		//!< Total number of calls to the sort routine
		HeU32			mNbHits;			//!< Number of early exits due to coherence
		// Stack-radix
		bool			mDeleteRanks;		//!<
		// Internal methods
		void			CheckResize(HeU32 nb);
		bool			Resize(HeU32 nb);
};

#endif // RADIX_H
