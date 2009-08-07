#ifndef HASHSET_H
#define HASHSET_H

#include "HashFunction.h"
#include "SimplePool.h"
#include "UserMemAlloc.h"
#include <string.h>
#include <assert.h>

/*!
A generic hash set.

A hash set is a specialization of a hash map where the element and the
key are the same thing, so it is a unique non-associative container.

This hashmap have the requirement of needing a hash function and a equality
predicate to function.

Warning: Requires number of buckets to be power of 2
*/
template<class T>
class HashSet
{
public:
	HashSet(size_t initialBucketCount=2048)
		: mNumBuckets(initialBucketCount), mNumElements(0), mBuckets(0)
	{
		// ensure Pow2 buckets
		assert((mNumBuckets & (mNumBuckets-1)) == 0);

		//Allocate the bucket-list
		mBuckets = reinterpret_cast<ElementType**>(MEMALLOC_MALLOC(sizeof(ElementType*)*mNumBuckets));
		memset(mBuckets, 0, sizeof(ElementType*)*mNumBuckets);
	}

	~HashSet()
	{
		MEMALLOC_FREE(mBuckets);
	}

	/*!
	Insert element into hash set.

	Returns true if element is inserted, false if it already exists in the set
	*/
	inline bool insert(const T& entry)
	{
    bool ret = false;

		// See if it exists a bucket
		NxU32 bucketID = getBucketIndex(entry);

		if(mBuckets[bucketID])
		{
			//have a bucket, check elements and insert if needed
			ElementType* e = mBuckets[bucketID];
			while(e)
			{
				if(e->mEntry == entry)
					return false;	//exists, no need to add

				e = e->mNext;
			}

			// Does not have, so add it
			e = mHashElementPool.construct();
			e->mEntry = entry;
			e->mNext = mBuckets[bucketID];
			mBuckets[bucketID] = e;
			mNumElements++;
			ret = true; //added
		}
		else
		{
			// No bucket, add one
			ElementType* e = mHashElementPool.construct();
			e->mEntry = entry;
			mBuckets[bucketID] = e;
			mNumElements++;
			ret = true; //added
		}

		return ret;
	}

  inline bool exists(const T& entry)
  {
    bool ret = false;

		if(mNumElements > 0 )
    {
  		// See if it exists a bucket
  		NxU32 bucketID = getBucketIndex(entry);
  		if(mBuckets[bucketID])
  		{
  			//have a bucket, check elements and insert if needed
  			ElementType* el = mBuckets[bucketID];
        while ( el )
        {
    			if(el->mEntry == entry)
  	  		{
            ret = true;
          }
          el = el->mNext;
        }
      }
		}

		return ret;
  }

  inline bool exists(NxU32 hashValue,T& entry)
  {
    bool ret = false;

		if(mNumElements > 0 )
    {
  		// See if it exists a bucket
  		NxU32 bucketID = getBucketIndexHash(hashValue);
  		if(mBuckets[bucketID])
  		{
  			//have a bucket, check elements and insert if needed
  			ElementType* el = mBuckets[bucketID];
        while ( el )
        {
    			if(el->mEntry->getHashValue() == hashValue )
  	  		{
            entry = el->mEntry;
            ret = true;
            break;
          }
          el = el->mNext;
        }
      }
		}

		return ret;
  }

	/*!
	Remove element from hash set
	*/
	inline bool remove(const T& entry)
	{
		if(mNumElements == 0)
			return false; //cannot remove from empty set

		// See if it exists a bucket
		NxU32 bucketID = getBucketIndex(entry);

		if(mBuckets[bucketID])
		{
			//have a bucket, check elements and insert if needed
			ElementType* el = mBuckets[bucketID];
			if(el->mEntry == entry)
			{
				//Bucket, so remove it
				mBuckets[bucketID] = el->mNext;
				mHashElementPool.destroy(el);
				mNumElements--;
				return true;
			}

			ElementType* prevEl = el;
			el = el->mNext;
			while(el)
			{
				if(el->mEntry == entry)
				{
					//unlink
					prevEl->mNext = el->mNext;
					mHashElementPool.destroy(el);
					mNumElements--;
					return true;
				}

				prevEl = el; el = el->mNext;
			}
		}

		return false;
	}

	/*!
	Clear out the set
	*/
	inline void clear()
	{
		if(mNumElements == 0)
			return; //already empty

		for(NxU32 i = 0; i < mNumBuckets; i++)
		{
			ElementType* e = mBuckets[i];
			while(e)
			{
				ElementType* prevE = e;
				e = e->mNext;

				mHashElementPool.destroy(prevE);
			}
		}

		memset(mBuckets, 0, sizeof(ElementType*)*mNumBuckets);
		mNumElements = 0;
	}

	/*!
	Get number of elements in hash set
	*/
	inline NxU32 getSize() const
	{
		return mNumElements;
	}

	/*!
	Try to set the number of buckets to a new size.
	Remember it should be power of 2.

	Returns false if not being able to resize;
	*/
	inline bool tryResize(NxU32 newBucketCount)
	{
		if(mNumElements > 0 || newBucketCount == mNumBuckets)
			return false;

		assert((newBucketCount & (newBucketCount-1)) == 0);

		mNumBuckets = newBucketCount;
		MEMALLOC_FREE(mBuckets);

		mBuckets = reinterpret_cast<ElementType**>(MEMALLOC_MALLOC(sizeof(ElementType*)*mNumBuckets));
		memset(mBuckets, 0, sizeof(ElementType*)*mNumBuckets);

		return true;
	}
private:
	NxU32			mNumBuckets;
	NxU32			mNumElements;

	template<class T2>
	struct HashElement
	{
		HashElement()
			: mNext(0)
		{
		}

		T2				mEntry;
		HashElement<T2>*	mNext;
	};
	typedef HashElement<T>	ElementType;

	SimplePool<ElementType>	mHashElementPool;

	ElementType**	mBuckets;

	//Helpers
	inline NxU32 getBucketIndex(const T& entry)
	{
		return HashFunction(entry->getHashValue()) & (mNumBuckets-1);
	}

	inline NxU32 getBucketIndexHash(NxU32 hashValue)
	{
		return HashFunction(hashValue) & (mNumBuckets-1);
	}

	template<class T3>
	friend class HashSetIterator;
};


/*!
Iterate over the content of a hash set
*/
template<class T>
class HashSetIterator
{
public:
	HashSetIterator(const HashSet<T>& hash)
		: mHash(hash), mBucket(0), mCurrentElement(0)
	{
	}


	/*!
	Get one element from the hash.

	Returns true if any element was returned, else false
	*/
	inline bool get(T& element)
	{
		//try to find next element

		if(mCurrentElement)
		{
			mCurrentElement = mCurrentElement->mNext;
			if(!mCurrentElement) mBucket++;	//next one
		}

		//find new bucket
		if(!mCurrentElement)
		{
			while(!mHash.mBuckets[mBucket] && mBucket < mHash.mNumBuckets)
				mBucket++;

			if(mHash.mNumBuckets == mBucket)
				return false;

			mCurrentElement = mHash.mBuckets[mBucket];
		}

		element = mCurrentElement->mEntry;
		return true;
	}


private:
	typedef HashSet<T>			HashType;
	const HashType&					mHash;
	NxU32							mBucket;
	typename HashType::ElementType*	mCurrentElement;
};

#endif
