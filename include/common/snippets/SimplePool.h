#ifndef SIMPLE_POOL_H
#define SIMPLE_POOL_H

#include "UserMemAlloc.h"
#include "SortedSet.h"

/*!
Simple allocation pool. Ported from LowLevel PxcSimplePool.
*/
template<class T>
class SimplePool
{
public:
	SimplePool (NxU32 elementsPerSlab = 32)
		: mElementsPerSlab (elementsPerSlab), mSlabSize (mElementSize * elementsPerSlab),
		mFreeElement (0)
	{
	}

	~SimplePool ()
	{
		disposeElements();
	}

	inline T* construct()
	{
		T* t = reinterpret_cast<T*>(allocateElement());

		if(t == 0)
			return 0;

		new (t) T;

		return t;
	}

	template<class A1>
	inline T* construct(const A1& a)
	{
		T* t = reinterpret_cast<T*>(allocateElement());

		if(t == 0)
			return 0;

		new (t) T (a);

		return t;
	}

	template<class A1>
	inline T* construct(A1& a)
	{
		T* t = reinterpret_cast<T*>(allocateElement());

		if(t == 0)
			return 0;

		new (t) T (a);

		return t;
	}

	template<class A1, class A2>
		inline T* construct(A1& a, A2& b)
	{
		T* t = reinterpret_cast<T*>(allocateElement());

		if(t == 0)
			return 0;

		new (t) T (a, b);

		return t;
	}

	template<class A1, class A2, class A3>
	inline T* construct(A1& a, A2& b, A3& c)
	{
		T* t = reinterpret_cast<T*>(allocateElement());

		if(t == 0)
			return 0;

		new (t) T (a, b, c);

		return t;
	}

	inline void destroy(T* const p)
	{
		if(p == 0)
			return;

		p->~T();
		freeElement(p);
	}

protected:
	struct FreeList
	{
		FreeList* mNext;
	};

	//Some static properties
	enum
	{
		mElementSize = sizeof(T)
	};

    // All the allocated slabs, sorted by pointer
	SortedSet<void*> mAllocatedSlabs;

	NxU32 mElementsPerSlab;
	NxU32 mSlabSize;

	FreeList* mFreeElement; // Head of free-list


	// Helper function to get bitmap of allocated elements

	// Allocate a slab and segregate it into the freelist
	inline void allocateSlab ()
	{
		NxU8* slab = reinterpret_cast<NxU8*>(MEMALLOC_MALLOC(mSlabSize));

		//Save the slab ptr
		mAllocatedSlabs.insert(slab);

		// Build a chain of nodes for the freelist
		FreeList* nextFree = mFreeElement;
		NxU8* node = slab + (mElementsPerSlab - 1) * mElementSize;
		for (; node >= slab; node -= mElementSize)
		{
			FreeList* element = reinterpret_cast<FreeList*> (node);
			element->mNext = nextFree;
			nextFree = element;
		}
		mFreeElement = nextFree;
	}

	// Free a slab
	inline void freeSlab (void* slab)
	{
		mAllocatedSlabs.remove(slab);
		MEMALLOC_FREE(slab);
	}

	// Free all slabs
	inline void freeAllSlabs ()
	{
		for(NxU32 i = 0; i < mAllocatedSlabs.getSize(); ++i)
		{
			MEMALLOC_FREE(mAllocatedSlabs[i]);
		}
		mAllocatedSlabs.release();

		mFreeElement = 0;
	}

	// Allocate space for single object
	inline void* allocateElement ()
	{
		if(mFreeElement == 0)
		{
			allocateSlab();
		}
		void* element = mFreeElement;

		mFreeElement = mFreeElement->mNext;

		return element;
	}

	// Put space for a single element back in the lists
	inline void freeElement (void* p)
	{
		FreeList* element = reinterpret_cast<FreeList*>(p);
		element->mNext = mFreeElement;
		mFreeElement = element;
	}

	/*
	Cleanup method. Go through all active slabs and call destructor for live objects,
	then free their memory
	*/
	inline void disposeElements ()
	{
		// Build a set of the free nodes
		SortedSet<void*> freeNodeSet;
		while(mFreeElement)
		{
			freeNodeSet.insert(mFreeElement);
			mFreeElement = mFreeElement->mNext;
		}

		if(freeNodeSet.getSize() != mElementsPerSlab * mAllocatedSlabs.getSize())
		{
			for(NxU32 i = 0; i < mAllocatedSlabs.getSize(); ++i)
			{
				NxU8* slab = reinterpret_cast<NxU8*>(mAllocatedSlabs[i]);
				for(NxU32 elId = 0; elId < mElementsPerSlab; ++elId)
				{
					void* element = slab + elId*mElementSize;
					if(freeNodeSet.contains(element))
						continue; //already freed, skip it

					T* t = reinterpret_cast<T*>(element);
          if ( t )
          {
					  t->~T(); // Destroy
          }
				}
			}
		}

		freeAllSlabs();
	}
};

#endif
