#ifndef NX_SIMPLE_VECTOR
#define NX_SIMPLE_VECTOR

#include <malloc.h>

#include "../snippets/UserMemAlloc.h"

#define DEFAULT_SIMPLE_VECTOR_SIZE 16 // on power-of-two boundary, but leave a little slop for headers.

#define USE_MALLOC_FREE 0

/**
 \brief Simple 'MemoryContainer::vector' style template container.

 if no NxUserAllocator is specified, the NxDefaultAllocator is used.

 Note: the methods of this template are implemented inline in order to avoid the not yet very cross-compileable 'typename' keyword.
*/
template<class ElemType >
class SimpleVector
{
public:
  typedef ElemType * iterator;
  typedef const ElemType * const_iterator;
	typedef SimpleVector<ElemType> MyType;

	inline SimpleVector()
  {
    mTag      = "SimpleVector";
    mElements = 0;
    mCount    = 0;
    mSize     = 0;
    mMaxCount = 0;
  }


	inline SimpleVector(const MyType & other)
	{
    assign(other);
  }

	inline ~SimpleVector(void)
  {
    clear();
  }


  inline void setTag(const char *tag)
  {
    mTag = tag;
  }


  inline void clear(void)
  {
    if ( mElements )
    {

      ElemType *t = mElements;
      for (HeU32 i=0; i<mCount; i++)
      {
        t->~ElemType();
        t++;
      }
#if USE_MALLOC_FREE
      ::MEMALLOC_FREE(mElements);
#else
      MEMALLOC_FREE(mElements);
#endif
    }
    mElements = 0;
    mCount = 0;
    mSize  = 0;
    mMaxCount = 0;
  }


	inline HeU32 size() const { return mCount; };

	inline bool empty(void) const { return mCount == 0; };

	inline void push_back(const ElemType& x)
  {
#if 0
    if ( strcmp(mTag,"SimpleVector") == 0 )
    {
      printf("debug me");
    }
#endif
    if ( mCount >= mMaxCount )
    {
      grow();
    }
    mElements[mCount] = x;
    mCount++;
  }

  void grow(void)
  {
    if ( mSize == 0 )
    {
      HE_ASSERT(mElements==0);
      mSize = DEFAULT_SIMPLE_VECTOR_SIZE;

      while ( mSize < sizeof(ElemType) )
      {
        mSize*=2;
      }

      mMaxCount = mSize/sizeof(ElemType);

      #if USE_MALLOC_FREE
      mElements = (ElemType *) MEMALLOC_MALLOC(mSize);
      #else
      mElements = (ElemType *) MEMALLOC_MALLOC_TYPE(mSize,mTag,__FILE__,__LINE__);
      #endif
    }
    else
    {
      mSize     = mSize*2;
      mMaxCount = mSize/sizeof(ElemType);

      #if USE_MALLOC_FREE
      mElements = (ElemType *)realloc(mElements, mSize );
      #else
      mElements = (ElemType *)MEMALLOC_REALLOC(mElements, mSize );
      #endif
    }
    HE_ASSERT(mElements);
  }


	inline const ElemType & operator[](HeU32 n) const
	{
    HE_ASSERT( n < mCount );
    return mElements[n];
  }

	inline ElemType & operator[](HeU32 n)
  {
    HE_ASSERT( n < mCount );
    return mElements[n];
  }

	inline iterator begin(void)
  {
    return mElements;
  }

  inline const_iterator begin() const
	{
    return mElements;
  }

	inline iterator end(void)
  {
    iterator ret = 0;
    if ( mCount )
    {
      ret = &mElements[mCount];
    }
    return ret;
  }

	inline const_iterator end(void) const
  {
    const_iterator ret = 0;
    if ( mCount > 0 )
    {
      ret = &mElements[mCount];
    }
    return ret;
  }

	inline void erase(iterator from)
	{
    HE_ASSERT(0); // not implemented yet!
	}

	inline MyType& operator=(const MyType& other)
  {
    assign(other);
    return *this;
  }

private:
  inline void assign(const MyType &other)
  {
    if ( &other != this )
    {
      mTag = other.mTag;
      mCount = other.mCount;
      mSize = other.mSize;
      mMaxCount = other.mMaxCount;
      mElements = 0;
      if ( mSize > 0 )
      {
        #if USE_MALLOC_FREE
        mElements = (ElemType *)MEMALLOC_MALLOC(mSize );
        #else
        mElements = (ElemType *)MEMALLOC_MALLOC_TYPE(mSize,mTag,__FILE__,__LINE__ );
        #endif
        if ( mCount )
        {
          memcpy(mElements,other.mElements,sizeof(ElemType)*mCount);
        }
      }
    }
  }

  const char * mTag;           // a string identifier tag.
  HeU32 mMaxCount;      // the maximum number of items that can fit into our currently allocated buffer size.
  HeU32 mCount;         // the number of items currently in the vector.
  HeU32 mSize;          // the total bytes of memory allocated for the array.
  ElemType    *mElements;      // the array of memory holding the elements.
};



 /** @} */
#endif
