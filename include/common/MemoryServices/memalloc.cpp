//-ignore_file      Do not allow SCANCPP to process this file!

#pragma warning(disable: 4702)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>
#include <assert.h>

#include <cassert>
#include <vector>
#include <bitset>
#include <limits>


#if defined(WIN32)
#include <windows.h>
#include <psapi.h>
#elif defined(LINUX)
#define _vsnprintf vsnprintf
typedef void* HANDLE;
typedef struct _PROCESS_MEMORY_COUNTERS {
	unsigned long cb;
	unsigned long PageFaultCount;
	size_t WorkingSetSize;
	size_t QuotaPeakPagedPoolUsage;
	size_t QuotaPagedPoolUsage;
	size_t QuotaPeakNonPagedPoolUsage;
	size_t QuotaNonPagedPoolUsage;
	size_t PagefileUsage;
	size_t PeakPageFileUsage;
} PROCESS_MEMORY_COUNTERS;
#endif

#pragma warning(disable:4996)
#pragma warning(disable:4189)
#pragma warning(disable:4100)
#pragma warning(disable:4244)
#pragma warning(disable:4127)
#pragma warning(disable:4267)
#pragma warning(disable:4390)

#include "memalloc.h"

#define DEFAULT_PAGE_SIZE 8192
#define LOG 0

#define DEFAULT_ALIGNMENT 0
#define ZERO_MEMORY 0                // zero memory as it is allocated.  Not recommended!

#define SYS_WINDOWS_HEAP 0    // for system memory allocations use the windows heap, off by default
#define SYS_MALLOC_FREE  1    // for system memory allocations, use standard malloc/free
#define SYS_DLMALLOC     0    // for system memory allocations, use DLMALLOC, recommended


#include "dlmalloc.h"
#include "../snippets/htmltable.h"

#pragma comment(lib,"psapi.lib")

#if HE_USE_MEMORY_TRACKING

namespace MEMALLOC
{
  void * mallocInternal(MemAlloc *memAlloc,size_t sz);
  void   freeInternal(MemAlloc *memAlloc,void *p);
};


#include "../snippets/SendTextMessage.h"

#if defined(_WIN32)
#include <windows.h>
#endif

////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any
//     purpose is hereby granted without fee, provided that the above  copyright
//     notice appear in all copies and that both that copyright notice and this
//     permission notice appear in supporting documentation.
// The author or Addison-Wesley Longman make no representations about the
//     suitability of this software for any purpose. It is provided "as is"
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////


namespace MEMALLOC
{

static HeU32 gMaxFixedMemorySize=128;
static HeU32 gMcount=0;
static HeU32 gMsize=0;
static HeU32 gFmcount=0;
static HeU32 gFmsize=0;
static const char *gComponentName="default";

#undef max

template <class T> class MyAlloc
{
  public:
    // type definitions
    typedef T              value_type;
    typedef T*             pointer;
    typedef const T*       const_pointer;
    typedef T&             reference;
    typedef const T&       const_reference;
    typedef std::size_t    size_type;
    typedef std::ptrdiff_t difference_type;

    // rebind allocator to type U
    template <class U> struct rebind
    {
      typedef MyAlloc<U> other;
    };

    // return address of values
    pointer address (reference value) const
    {
      return &value;
    }

    const_pointer address (const_reference value) const
    {
      return &value;
    }

    /* constructors and destructor
     * - nothing to do because the allocator has no state
     */
    MyAlloc() throw()
    {
    }

    MyAlloc(const MyAlloc&) throw()
    {
    }

    template <class U> MyAlloc (const MyAlloc<U>&) throw()
    {
    }

    ~MyAlloc() throw()
    {
    }

    // return maximum number of elements that can be allocated
    size_type max_size () const throw()
    {
      return std::numeric_limits<std::size_t>::max() / sizeof(T);
    }

    // allocate but don't initialize num elements of type T
    pointer allocate (size_type num, const void* = 0)
    {
      pointer ret = (pointer)(::malloc(num*sizeof(T)));
      return ret;
    }

    // initialize elements of allocated storage p with value value
    void construct (pointer p, const T& value)
    {
      // initialize memory with placement new
      new((void*)p)T(value);
    }

    // destroy elements of initialized storage p
    void destroy (pointer p)
    {
      // destroy objects by calling their destructor
      p->~T();
    }

    // deallocate storage p of deleted elements
    void deallocate (pointer p, size_type num)
    {
      ::free((void*)p);
    }

}; // End of template 'MyAlloc'

// return that all specializations of this allocator are interchangeable
template <class T1, class T2>  bool operator== (const MyAlloc<T1>&, const MyAlloc<T2>&) throw()
{
  return true;
}

template <class T1, class T2> bool operator!= (const MyAlloc<T1>&, const MyAlloc<T2>&) throw()
{
  return false;
}




    /** @struct Chunk
        @ingroup SmallObjectGroupInternal
     Contains info about each allocated Chunk - which is a collection of
     contiguous blocks.  Each block is the same size, as specified by the
     FixedAllocator.  The number of blocks in a Chunk depends upon page size.
     This is a POD-style struct with value-semantics.  All functions and data
     are private so that they can not be changed by anything other than the
     FixedAllocator which owns the Chunk.

     @par Minimal Interface
     For the sake of runtime efficiency, no constructor, destructor, or
     copy-assignment operator is defined. The inline functions made by the
     compiler should be sufficient, and perhaps faster than hand-crafted
     functions.  The lack of these functions allows vector to create and copy
     Chunks as needed without overhead.  The Init and Release functions do
     what the default constructor and destructor would do.  A Chunk is not in
     a usable state after it is constructed and before calling Init.  Nor is
     a Chunk usable after Release is called, but before the destructor.

     @par Efficiency
     Down near the lowest level of the allocator, runtime efficiencies trump
     almost all other considerations.  Each function does the minimum required
     of it.  All functions should execute in constant time to prevent higher-
     level code from unwittingly using a version of Shlemiel the Painter's
     Algorithm.

     @par Stealth Indexes
     The first char of each empty block contains the index of the next empty
     block.  These stealth indexes form a singly-linked list within the blocks.
     A Chunk is corrupt if this singly-linked list has a loop or is shorter
     than blocksAvailable_.  Much of the allocator's time and space efficiency
     comes from how these stealth indexes are implemented.
     */
    class Chunk
    {
    private:
        friend class FixedAllocator;

        /** Initializes a just-constructed Chunk.
         @param blockSize Number of bytes per block.
         @param blocks Number of blocks per Chunk.
         @return True for success, false for failure.
         */
        bool Init( std::size_t blockSize, unsigned char blocks,MEMALLOC::MemAlloc *parent);

        /** Allocate a block within the Chunk.  Complexity is always O(1), and
         this will never throw.  Does not actually "allocate" by calling
         malloc, new, or any other function, but merely adjusts some internal
         indexes to indicate an already allocated block is no longer available.
         @return Pointer to block within Chunk.
         */
        void * Allocate( std::size_t blockSize );

        /** Deallocate a block within the Chunk. Complexity is always O(1), and
         this will never throw.  For efficiency, this assumes the address is
         within the block and aligned along the correct byte boundary.  An
         assertion checks the alignment, and a call to HasBlock is done from
         within VicinityFind.  Does not actually "deallocate" by calling free,
         delete, or other function, but merely adjusts some internal indexes to
         indicate a block is now available.
         */
        void Deallocate( void * p, std::size_t blockSize );

        /** Resets the Chunk back to pristine values. The available count is
         set back to zero, and the first available index is set to the zeroth
         block.  The stealth indexes inside each block are set to point to the
         next block. This assumes the Chunk's data was already using Init.
         */
        void Reset( std::size_t blockSize, unsigned char blocks );

        /// Releases the allocated block of memory.
        void Release(MEMALLOC::MemAlloc *parent);

        /** Determines if the Chunk has been corrupted.
         @param numBlocks Total # of blocks in the Chunk.
         @param blockSize # of bytes in each block.
         @param checkIndexes True if caller wants to check indexes of available
          blocks for corruption.  If false, then caller wants to skip some
          tests tests just to run faster.  (Debug version does more checks, but
          release version runs faster.)
         @return True if Chunk is corrupt.
         */
        bool IsCorrupt( unsigned char numBlocks, std::size_t blockSize,
            bool checkIndexes ) const;

        /** Determines if block is available.
         @param p Address of block managed by Chunk.
         @param numBlocks Total # of blocks in the Chunk.
         @param blockSize # of bytes in each block.
         @return True if block is available, else false if allocated.
         */
        bool IsBlockAvailable( void * p, unsigned char numBlocks,
            std::size_t blockSize ) const;

        /// Returns true if block at address P is inside this Chunk.
        inline bool HasBlock( void * p, std::size_t chunkLength ) const
        {
            unsigned char * pc = static_cast< unsigned char * >( p );
            return ( pData_ <= pc ) && ( pc < pData_ + chunkLength );
        }

        inline bool HasAvailable( unsigned char numBlocks ) const
        { return ( blocksAvailable_ == numBlocks ); }

        inline bool IsFilled( void ) const
        { return ( 0 == blocksAvailable_ ); }

        /// Pointer to array of allocated blocks.
        unsigned char * pData_;
        /// Index of first empty block.
        unsigned char firstAvailableBlock_;
        /// Count of empty blocks.
        unsigned char blocksAvailable_;
    };

    /** @class FixedAllocator
        @ingroup SmallObjectGroupInternal
     Offers services for allocating fixed-sized objects.  It has a container
     of "containers" of fixed-size blocks.  The outer container has all the
     Chunks.  The inner container is a Chunk which owns some blocks.

     @par Class Level Invariants
     - There is always either zero or one Chunk which is empty.
     - If this has no empty Chunk, then emptyChunk_ is NULL.
     - If this has an empty Chunk, then emptyChunk_ points to it.
     - If the Chunk container is empty, then deallocChunk_ and allocChunk_
       are NULL.
     - If the Chunk container is not-empty, then deallocChunk_ and allocChunk_
       are either NULL or point to Chunks within the container.
     - allocChunk_ will often point to the last Chunk in the container since
       it was likely allocated most recently, and therefore likely to have an
       available block.
     */
    class FixedAllocator
    {
    private:

        /** Deallocates the block at address p, and then handles the internal
         bookkeeping needed to maintain class invariants.  This assumes that
         deallocChunk_ points to the correct chunk.
         */
        void DoDeallocate( void * p );

        /** Creates an empty Chunk and adds it to the end of the ChunkList.
         All calls to the lower-level memory allocation functions occur inside
         this function, and so the only try-catch block is inside here.
         @return true for success, false for failure.
         */
        bool MakeNewChunk( void );

        /** Finds the Chunk which owns the block at address p.  It starts at
         deallocChunk_ and searches in both forwards and backwards directions
         from there until it finds the Chunk which owns p.  This algorithm
         should find the Chunk quickly if it is deallocChunk_ or is close to it
         in the Chunks container.  This goes both forwards and backwards since
         that works well for both same-order and opposite-order deallocations.
         (Same-order = objects are deallocated in the same order in which they
         were allocated.  Opposite order = objects are deallocated in a last to
         first order.  Complexity is O(C) where C is count of all Chunks.  This
         never throws.
         @return Pointer to Chunk that owns p, or NULL if no owner found.
         */
        Chunk * VicinityFind( void * p ) const;

        /// Not implemented.
        FixedAllocator(const FixedAllocator&);
        /// Not implemented.
        FixedAllocator& operator=(const FixedAllocator&);


        /// Type of container used to hold Chunks.
        typedef MyAlloc< Chunk > ChunkAllocator;
        typedef std::vector< Chunk, ChunkAllocator > Chunks;
        /// Iterator through container of Chunks.
        typedef Chunks::iterator ChunkIter;
        /// Iterator through const container of Chunks.
        typedef Chunks::const_iterator ChunkCIter;

        /// Fewest # of objects managed by a Chunk.
        static unsigned char MinObjectsPerChunk_;

        /// Most # of objects managed by a Chunk - never exceeds UCHAR_MAX.
        static unsigned char MaxObjectsPerChunk_;

        /// Number of bytes in a single block within a Chunk.
        std::size_t blockSize_;
        /// Number of blocks managed by each Chunk.
        unsigned char numBlocks_;

        /// Container of Chunks.
        Chunks chunks_;
        /// Pointer to Chunk used for last or next allocation.
        Chunk * allocChunk_;
        /// Pointer to Chunk used for last or next deallocation.
        Chunk * deallocChunk_;
        /// Pointer to the only empty Chunk if there is one, else NULL.
        Chunk * emptyChunk_;

        int mAllocCount;

        MEMALLOC::MemAlloc  *mParent; // the parent allocator to get chunk data from.

    public:
        /// Create a FixedAllocator which manages blocks of 'blockSize' size.
        FixedAllocator();

        /// Destroy the FixedAllocator and release all its Chunks.
        ~FixedAllocator();

        int getAllocCount(void) const { return mAllocCount; };

        /// Initializes a FixedAllocator by calculating # of blocks per Chunk.
        void Initialize( std::size_t blockSize, std::size_t pageSize,MEMALLOC::MemAlloc *parentAllocator);

        /** Returns pointer to allocated memory block of fixed size - or NULL
         if it failed to allocate.
         */
        void * Allocate( void );

        /** Deallocate a memory block previously allocated with Allocate.  If
         the block is not owned by this FixedAllocator, it returns false so
         that SmallObjAllocator can call the default deallocator.  If the
         block was found, this returns true.
         */
        bool Deallocate( void * p, Chunk * hint );

        /// Returns block size with which the FixedAllocator was initialized.
        inline std::size_t BlockSize() const { return blockSize_; }

        /** Releases the memory used by the empty Chunk.  This will take
         constant time under any situation.
         @return True if empty chunk found and released, false if none empty.
         */
        bool TrimEmptyChunk( void );

        /** Releases unused spots from ChunkList.  This takes constant time
         with respect to # of Chunks, but actual time depends on underlying
         memory allocator.
         @return False if no unused spots, true if some found and released.
         */
        bool TrimChunkList( void );

        /** Returns count of empty Chunks held by this allocator.  Complexity
         is O(C) where C is the total number of Chunks - empty or used.
         */
        std::size_t CountEmptyChunks( void ) const;

        /** Determines if FixedAllocator is corrupt.  Checks data members to
         see if any have erroneous values, or violate class invariants.  It
         also checks if any Chunk is corrupt.  Complexity is O(C) where C is
         the number of Chunks.  If any data is corrupt, this will return true
         in release mode, or assert in debug mode.
         */
        bool IsCorrupt( void ) const;

        /** Returns true if the block at address p is within a Chunk owned by
         this FixedAllocator.  Complexity is O(C) where C is the total number
         of Chunks - empty or used.
         */
        const Chunk * HasBlock( void * p ) const;
        inline Chunk * HasBlock( void * p )
        {
            return const_cast< Chunk * >(
                const_cast< const FixedAllocator * >( this )->HasBlock( p ) );
        }

    };


    unsigned char FixedAllocator::MinObjectsPerChunk_ = 8;
    unsigned char FixedAllocator::MaxObjectsPerChunk_ = UCHAR_MAX;

// Chunk::Init ----------------------------------------------------------------

bool Chunk::Init( std::size_t blockSize, unsigned char blocks,MEMALLOC::MemAlloc *parentAllocator)
{
    HE_ASSERT(blockSize > 0);
    HE_ASSERT(blocks > 0);
    // Overflow check
    const std::size_t allocSize = blockSize * blocks;
    HE_ASSERT( allocSize / blockSize == blocks);

    // malloc can't throw, so its only way to indicate an error is to return
    // a NULL pointer, so we have to check for that.
    pData_ = static_cast< unsigned char * >( mallocInternal(parentAllocator, allocSize ) );
    if ( NULL == pData_ ) return false;

    Reset( blockSize, blocks );
    return true;
}

// Chunk::Reset ---------------------------------------------------------------

void Chunk::Reset(std::size_t blockSize, unsigned char blocks)
{
    HE_ASSERT(blockSize > 0);
    HE_ASSERT(blocks > 0);
    // Overflow check
    HE_ASSERT((blockSize * blocks) / blockSize == blocks);

    firstAvailableBlock_ = 0;
    blocksAvailable_ = blocks;

    unsigned char i = 0;
    for ( unsigned char * p = pData_; i != blocks; p += blockSize )
    {
        *p = ++i;
    }
}

// Chunk::Release -------------------------------------------------------------

void Chunk::Release(MEMALLOC::MemAlloc *parent)
{
  HE_ASSERT( NULL != pData_ );
  MEMALLOC::freeInternal(parent, static_cast< void * >(pData_ ) );
}

// Chunk::Allocate ------------------------------------------------------------

void* Chunk::Allocate(std::size_t blockSize)
{
    if ( IsFilled() ) return NULL;

    HE_ASSERT((firstAvailableBlock_ * blockSize) / blockSize == 
        firstAvailableBlock_);
    unsigned char * pResult = pData_ + (firstAvailableBlock_ * blockSize);
    firstAvailableBlock_ = *pResult;
    --blocksAvailable_;

    return pResult;
}

// Chunk::Deallocate ----------------------------------------------------------

void Chunk::Deallocate(void* p, std::size_t blockSize)
{
    HE_ASSERT(p >= pData_);

    unsigned char* toRelease = static_cast<unsigned char*>(p);
    // Alignment check
    HE_ASSERT((toRelease - pData_) % blockSize == 0);
    unsigned char index = static_cast< unsigned char >(
        ( toRelease - pData_ ) / blockSize);

#if defined(DEBUG) || defined(_DEBUG)
    // Check if block was already deleted.  Attempting to delete the same
    // block more than once causes Chunk's linked-list of stealth indexes to
    // become corrupt.  And causes count of blocksAvailable_ to be wrong.
    if ( 0 < blocksAvailable_ )
        HE_ASSERT( firstAvailableBlock_ != index );
#endif

    *toRelease = firstAvailableBlock_;
    firstAvailableBlock_ = index;
    // Truncation check
    HE_ASSERT(firstAvailableBlock_ == (toRelease - pData_) / blockSize);

    ++blocksAvailable_;
}

// Chunk::IsCorrupt -----------------------------------------------------------

bool Chunk::IsCorrupt( unsigned char numBlocks, std::size_t blockSize,
    bool checkIndexes ) const
{

    if ( numBlocks < blocksAvailable_ )
    {
        // Contents at this Chunk corrupted.  This might mean something has
        // overwritten memory owned by the Chunks container.
        HE_ASSERT( false );
        return true;
    }
    if ( IsFilled() )
        // Useless to do further corruption checks if all blocks allocated.
        return false;
    unsigned char index = firstAvailableBlock_;
    if ( numBlocks <= index )
    {
        // Contents at this Chunk corrupted.  This might mean something has
        // overwritten memory owned by the Chunks container.
        HE_ASSERT( false );
        return true;
    }
    if ( !checkIndexes )
        // Caller chose to skip more complex corruption tests.
        return false;

    /* If the bit at index was set in foundBlocks, then the stealth index was
     found on the linked-list.
     */
    std::bitset< UCHAR_MAX > foundBlocks;
    unsigned char * nextBlock = NULL;

    /* The loop goes along singly linked-list of stealth indexes and makes sure
     that each index is within bounds (0 <= index < numBlocks) and that the
     index was not already found while traversing the linked-list.  The linked-
     list should have exactly blocksAvailable_ nodes, so the for loop will not
     check more than blocksAvailable_.  This loop can't check inside allocated
     blocks for corruption since such blocks are not within the linked-list.
     Contents of allocated blocks are not changed by Chunk.

     Here are the types of corrupted link-lists which can be verified.  The
     corrupt index is shown with asterisks in each example.

     Type 1: Index is too big.
      numBlocks == 64
      blocksAvailable_ == 7
      firstAvailableBlock_ -> 17 -> 29 -> *101*
      There should be no indexes which are equal to or larger than the total
      number of blocks.  Such an index would refer to a block beyond the
      Chunk's allocated domain.

     Type 2: Index is repeated.
      numBlocks == 64
      blocksAvailable_ == 5
      firstAvailableBlock_ -> 17 -> 29 -> 53 -> *17* -> 29 -> 53 ...
      No index should be repeated within the linked-list since that would
      indicate the presence of a loop in the linked-list.
     */
    for ( unsigned char cc = 0; ; )
    {
        nextBlock = pData_ + ( index * blockSize );
        foundBlocks.set( index, true );
        ++cc;
        if ( cc >= blocksAvailable_ )
            // Successfully counted off number of nodes in linked-list.
            break;
        index = *nextBlock;
        if ( numBlocks <= index )
        {
            /* This catches Type 1 corruptions as shown in above comments.
             This implies that a block was corrupted due to a stray pointer
             or an operation on a nearby block overran the size of the block.
             */
            HE_ASSERT( false );
            return true;
        }
        if ( foundBlocks.test( index ) )
        {
            /* This catches Type 2 corruptions as shown in above comments.
             This implies that a block was corrupted due to a stray pointer
             or an operation on a nearby block overran the size of the block.
             Or perhaps the program tried to delete a block more than once.
             */
            HE_ASSERT( false );
            return true;
        }
    }
    if ( foundBlocks.count() != blocksAvailable_ )
    {
        /* This implies that the singly-linked-list of stealth indexes was
         corrupted.  Ideally, this should have been detected within the loop.
         */
        HE_ASSERT( false );
        return true;
    }

    return false;
}

// Chunk::IsBlockAvailable ----------------------------------------------------

bool Chunk::IsBlockAvailable( void * p, unsigned char numBlocks,
    std::size_t blockSize ) const
{
    (void) numBlocks;
    
    if ( IsFilled() )
        return false;

    unsigned char * place = static_cast< unsigned char * >( p );
    // Alignment check
    HE_ASSERT( ( place - pData_ ) % blockSize == 0 );
    unsigned char blockIndex = static_cast< unsigned char >(
        ( place - pData_ ) / blockSize );

    unsigned char index = firstAvailableBlock_;
    HE_ASSERT( numBlocks > index );
    if ( index == blockIndex )
        return true;

    /* If the bit at index was set in foundBlocks, then the stealth index was
     found on the linked-list.
     */
    std::bitset< UCHAR_MAX > foundBlocks;
    unsigned char * nextBlock = NULL;
    for ( unsigned char cc = 0; ; )
    {
        nextBlock = pData_ + ( index * blockSize );
        foundBlocks.set( index, true );
        ++cc;
        if ( cc >= blocksAvailable_ )
            // Successfully counted off number of nodes in linked-list.
            break;
        index = *nextBlock;
        if ( index == blockIndex )
            return true;
        HE_ASSERT( numBlocks > index );
        HE_ASSERT( !foundBlocks.test( index ) );
    }

    return false;
}

// FixedAllocator::FixedAllocator ---------------------------------------------

FixedAllocator::FixedAllocator()
    : blockSize_( 0 )
    , numBlocks_( 0 )
    , chunks_( 0 )
    , allocChunk_( NULL )
    , deallocChunk_( NULL )
    , emptyChunk_( NULL )
    , mParent(NULL)
    , mAllocCount(0)
{
}

// FixedAllocator::~FixedAllocator --------------------------------------------

FixedAllocator::~FixedAllocator()
{
    for ( ChunkIter i( chunks_.begin() ); i != chunks_.end(); ++i )
       i->Release(mParent);
}

// FixedAllocator::Initialize -------------------------------------------------

void FixedAllocator::Initialize( std::size_t blockSize, std::size_t pageSize,MEMALLOC::MemAlloc *parent)
{
  mParent = parent;
    HE_ASSERT( blockSize > 0 );
    HE_ASSERT( pageSize >= blockSize );
    blockSize_ = blockSize;

    std::size_t numBlocks = pageSize / blockSize;
    if ( numBlocks > MaxObjectsPerChunk_ ) numBlocks = MaxObjectsPerChunk_;
    else if ( numBlocks < MinObjectsPerChunk_ ) numBlocks = MinObjectsPerChunk_;

    numBlocks_ = static_cast<unsigned char>(numBlocks);
    HE_ASSERT(numBlocks_ == numBlocks);
}

// FixedAllocator::CountEmptyChunks -------------------------------------------

std::size_t FixedAllocator::CountEmptyChunks( void ) const
{
    return ( NULL == emptyChunk_ ) ? 0 : 1;
}

// FixedAllocator::IsCorrupt --------------------------------------------------

bool FixedAllocator::IsCorrupt( void ) const
{
    const bool isEmpty = chunks_.empty();
    ChunkCIter start( chunks_.begin() );
    ChunkCIter last( chunks_.end() );
    const size_t emptyChunkCount = CountEmptyChunks();

    if ( isEmpty )
    {
        if ( start != last )
        {
            HE_ASSERT( false );
            return true;
        }
        if ( 0 < emptyChunkCount )
        {
            HE_ASSERT( false );
            return true;
        }
        if ( NULL != deallocChunk_ )
        {
            HE_ASSERT( false );
            return true;
        }
        if ( NULL != allocChunk_ )
        {
            HE_ASSERT( false );
            return true;
        }
        if ( NULL != emptyChunk_ )
        {
            HE_ASSERT( false );
            return true;
        }
    }

    else
    {
        const Chunk * front = &chunks_.front();
        const Chunk * back  = &chunks_.back();
        if ( start >= last )
        {
            HE_ASSERT( false );
            return true;
        }
        if ( back < deallocChunk_ )
        {
            HE_ASSERT( false );
            return true;
        }
        if ( back < allocChunk_ )
        {
            HE_ASSERT( false );
            return true;
        }
        if ( front > deallocChunk_ )
        {
            HE_ASSERT( false );
            return true;
        }
        if ( front > allocChunk_ )
        {
            HE_ASSERT( false );
            return true;
        }

        switch ( emptyChunkCount )
        {
            case 0:
                if ( emptyChunk_ != NULL )
                {
                    HE_ASSERT( false );
                    return true;
                }
                break;
            case 1:
                if ( emptyChunk_ == NULL )
                {
                    HE_ASSERT( false );
                    return true;
                }
                if ( back < emptyChunk_ )
                {
                    HE_ASSERT( false );
                    return true;
                }
                if ( front > emptyChunk_ )
                {
                    HE_ASSERT( false );
                    return true;
                }
                if ( !emptyChunk_->HasAvailable( numBlocks_ ) )
                {
                    // This may imply somebody tried to delete a block twice.
                    HE_ASSERT( false );
                    return true;
                }
                break;
            default:
                HE_ASSERT( false );
                return true;
        }
        for ( ChunkCIter it( start ); it != last; ++it )
        {
            const Chunk & chunk = *it;
            if ( chunk.IsCorrupt( numBlocks_, blockSize_, true ) )
                return true;
        }
    }

    return false;
}

// FixedAllocator::HasBlock ---------------------------------------------------

const Chunk * FixedAllocator::HasBlock( void * p ) const
{
    const std::size_t chunkLength = numBlocks_ * blockSize_;
    for ( ChunkCIter it( chunks_.begin() ); it != chunks_.end(); ++it )
    {
        const Chunk & chunk = *it;
        if ( chunk.HasBlock( p, chunkLength ) )
            return &chunk;
    }
    return NULL;
}

// FixedAllocator::TrimEmptyChunk ---------------------------------------------

bool FixedAllocator::TrimEmptyChunk( void )
{
    // prove either emptyChunk_ points nowhere, or points to a truly empty Chunk.
    HE_ASSERT( ( NULL == emptyChunk_ ) || ( emptyChunk_->HasAvailable( numBlocks_ ) ) );
    if ( NULL == emptyChunk_ ) return false;

    // If emptyChunk_ points to valid Chunk, then chunk list is not empty.
    HE_ASSERT( !chunks_.empty() );
    // And there should be exactly 1 empty Chunk.
    HE_ASSERT( 1 == CountEmptyChunks() );

    Chunk * lastChunk = &chunks_.back();
    if ( lastChunk != emptyChunk_ )
        std::swap( *emptyChunk_, *lastChunk );
    HE_ASSERT( lastChunk->HasAvailable( numBlocks_ ) );
    lastChunk->Release(mParent);
    chunks_.pop_back();

    if ( chunks_.empty() )
    {
        allocChunk_ = NULL;
        deallocChunk_ = NULL;
    }
    else
    {
        if ( deallocChunk_ == emptyChunk_ )
        {
            deallocChunk_ = &chunks_.front();
            HE_ASSERT( deallocChunk_->blocksAvailable_ < numBlocks_ );
        }
        if ( allocChunk_ == emptyChunk_ )
        {
            allocChunk_ = &chunks_.back();
            HE_ASSERT( allocChunk_->blocksAvailable_ < numBlocks_ );
        }
    }

    emptyChunk_ = NULL;
    HE_ASSERT( 0 == CountEmptyChunks() );

    return true;
}

// FixedAllocator::TrimChunkList ----------------------------------------------

bool FixedAllocator::TrimChunkList( void )
{
    if ( chunks_.empty() )
    {
        HE_ASSERT( NULL == allocChunk_ );
        HE_ASSERT( NULL == deallocChunk_ );
    }

    if ( chunks_.size() == chunks_.capacity() )
        return false;
    // Use the "make-a-temp-and-swap" trick to remove excess capacity.
    Chunks( chunks_ ).swap( chunks_ );

    return true;
}

// FixedAllocator::MakeNewChunk -----------------------------------------------

bool FixedAllocator::MakeNewChunk( void )
{
    bool allocated = false;
    try
    {
        std::size_t size = chunks_.size();
        // Calling chunks_.reserve *before* creating and initializing the new
        // Chunk means that nothing is leaked by this function in case an
        // exception is thrown from reserve.
        if ( chunks_.capacity() == size )
        {
            if ( 0 == size ) size = 4;
            chunks_.reserve( size * 2 );
        }
        Chunk newChunk;
        allocated = newChunk.Init( blockSize_, numBlocks_,mParent );
        if ( allocated )
            chunks_.push_back( newChunk );
    }
    catch ( ... )
    {
        allocated = false;
    }
    if ( !allocated ) return false;

    allocChunk_ = &chunks_.back();
    deallocChunk_ = &chunks_.front();
    return true;
}

// FixedAllocator::Allocate ---------------------------------------------------

void * FixedAllocator::Allocate( void )
{
  // prove either emptyChunk_ points nowhere, or points to a truly empty Chunk.
  HE_ASSERT( ( NULL == emptyChunk_ ) || ( emptyChunk_->HasAvailable( numBlocks_ ) ) );
  HE_ASSERT( CountEmptyChunks() < 2 );

  if ( ( NULL == allocChunk_ ) || allocChunk_->IsFilled() )
  {
    if ( NULL != emptyChunk_ )
    {
      allocChunk_ = emptyChunk_;
      emptyChunk_ = NULL;
    }
    else
    {
      for ( ChunkIter i( chunks_.begin() ); ; ++i )
      {
        if ( chunks_.end() == i )
        {
          if ( !MakeNewChunk() )
            return NULL;
          break;
        }
        if ( !i->IsFilled() )
        {
          allocChunk_ = &*i;
          break;
        }
      }
    }
  }
  else if ( allocChunk_ == emptyChunk_)
  {
    // detach emptyChunk_ from allocChunk_, because after
    // calling allocChunk_->Allocate(blockSize_); the chunk
    // is no longer empty.
    emptyChunk_ = NULL;
  }

  HE_ASSERT( allocChunk_ != NULL );
  HE_ASSERT( !allocChunk_->IsFilled() );
  void * place = allocChunk_->Allocate( blockSize_ );

  // prove either emptyChunk_ points nowhere, or points to a truly empty Chunk.
  HE_ASSERT( ( NULL == emptyChunk_ ) || ( emptyChunk_->HasAvailable( numBlocks_ ) ) );
  HE_ASSERT( CountEmptyChunks() < 2 );

  mAllocCount++;

  return place;
}

// FixedAllocator::Deallocate -------------------------------------------------

bool FixedAllocator::Deallocate( void * p, Chunk * hint )
{

  HE_ASSERT( p );
  HE_ASSERT( mAllocCount > 0 );

  HE_ASSERT(!chunks_.empty());
  HE_ASSERT(&chunks_.front() <= deallocChunk_);
  HE_ASSERT(&chunks_.back() >= deallocChunk_);
  HE_ASSERT( &chunks_.front() <= allocChunk_ );
  HE_ASSERT( &chunks_.back() >= allocChunk_ );
  HE_ASSERT( CountEmptyChunks() < 2 );

  Chunk * foundChunk = ( NULL == hint ) ? VicinityFind( p ) : hint;
  if ( NULL == foundChunk )
      return false;

  HE_ASSERT( foundChunk->HasBlock( p, numBlocks_ * blockSize_ ) );
#ifdef LOKI_CHECK_FOR_CORRUPTION
    if ( foundChunk->IsCorrupt( numBlocks_, blockSize_, true ) )
    {
        HE_ASSERT( false );
        return false;
    }
    if ( foundChunk->IsBlockAvailable( p, numBlocks_, blockSize_ ) )
    {
        HE_ASSERT( false );
        return false;
    }
#endif
    deallocChunk_ = foundChunk;
    DoDeallocate(p);
    HE_ASSERT( CountEmptyChunks() < 2 );

  mAllocCount--;

  return true;
}

// FixedAllocator::VicinityFind -----------------------------------------------

Chunk * FixedAllocator::VicinityFind( void * p ) const
{
    if ( chunks_.empty() ) return NULL;
    HE_ASSERT(deallocChunk_);

    const std::size_t chunkLength = numBlocks_ * blockSize_;
    Chunk * lo = deallocChunk_;
    Chunk * hi = deallocChunk_ + 1;
    const Chunk * loBound = &chunks_.front();
    const Chunk * hiBound = &chunks_.back() + 1;

    // Special case: deallocChunk_ is the last in the array
    if (hi == hiBound) hi = NULL;

    for (;;)
    {
        if (lo)
        {
            if ( lo->HasBlock( p, chunkLength ) ) return lo;
            if ( lo == loBound )
            {
                lo = NULL;
                if ( NULL == hi ) break;
            }
            else --lo;
        }

        if (hi)
        {
            if ( hi->HasBlock( p, chunkLength ) ) return hi;
            if ( ++hi == hiBound )
            {
                hi = NULL;
                if ( NULL == lo ) break;
            }
        }
    }

    return NULL;
}

// FixedAllocator::DoDeallocate -----------------------------------------------

void FixedAllocator::DoDeallocate(void* p)
{
    // Show that deallocChunk_ really owns the block at address p.
    HE_ASSERT( deallocChunk_->HasBlock( p, numBlocks_ * blockSize_ ) );
    // Either of the next two assertions may fail if somebody tries to
    // delete the same block twice.
    HE_ASSERT( emptyChunk_ != deallocChunk_ );
    HE_ASSERT( !deallocChunk_->HasAvailable( numBlocks_ ) );
    // prove either emptyChunk_ points nowhere, or points to a truly empty Chunk.
    HE_ASSERT( ( NULL == emptyChunk_ ) || ( emptyChunk_->HasAvailable( numBlocks_ ) ) );

    // call into the chunk, will adjust the inner list but won't release memory
    deallocChunk_->Deallocate(p, blockSize_);

    if ( deallocChunk_->HasAvailable( numBlocks_ ) )
    {
        HE_ASSERT( emptyChunk_ != deallocChunk_ );
        // deallocChunk_ is empty, but a Chunk is only released if there are 2
        // empty chunks.  Since emptyChunk_ may only point to a previously
        // cleared Chunk, if it points to something else besides deallocChunk_,
        // then FixedAllocator currently has 2 empty Chunks.
        if ( NULL != emptyChunk_ )
        {
            // If last Chunk is empty, just change what deallocChunk_
            // points to, and release the last.  Otherwise, swap an empty
            // Chunk with the last, and then release it.
            Chunk * lastChunk = &chunks_.back();
            if ( lastChunk == deallocChunk_ )
                deallocChunk_ = emptyChunk_;
            else if ( lastChunk != emptyChunk_ )
                std::swap( *emptyChunk_, *lastChunk );
            HE_ASSERT( lastChunk->HasAvailable( numBlocks_ ) );
            lastChunk->Release(mParent);
            chunks_.pop_back();
            if ( ( allocChunk_ == lastChunk ) || allocChunk_->IsFilled() ) 
                allocChunk_ = deallocChunk_;
        }
        emptyChunk_ = deallocChunk_;
    }

    // prove either emptyChunk_ points nowhere, or points to a truly empty Chunk.
    HE_ASSERT( ( NULL == emptyChunk_ ) || ( emptyChunk_->HasAvailable( numBlocks_ ) ) );
}

// GetOffset ------------------------------------------------------------------
/// @ingroup SmallObjectGroupInternal
/// Calculates index into array where a FixedAllocator of numBytes is located.
inline std::size_t GetOffset( std::size_t numBytes, std::size_t alignment )
{
    const std::size_t alignExtra = alignment-1;
    return ( numBytes + alignExtra ) / alignment;
}

} // end namespace Loki



namespace MEMALLOC
{

//==================================================================================
class MemMutex
{
	public:
		MemMutex(void);
		~MemMutex(void);

	public:
		// Blocking Lock.
		void Lock(void);

		// Unlock.
		void Unlock(void);

private:
		#if defined(_WIN32) || defined(_XBOX)
		CRITICAL_SECTION m_Mutex;
		#elif defined(__APPLE__) || defined(LINUX)
		pthread_mutex_t  m_Mutex;
	volatile	size_t           m_LockCount;
		#endif
};

//==================================================================================
MemMutex::MemMutex(void)
{
#if defined(_WIN32) || defined(_XBOX)
	InitializeCriticalSection(&m_Mutex);
#elif defined(__APPLE__) || defined(LINUX)
	pthread_mutex_init(&m_Mutex, 0);
	m_LockCount = 0;
#endif
}

//==================================================================================
MemMutex::~MemMutex(void)
{
#if defined(_WIN32) || defined(_XBOX)
	DeleteCriticalSection(&m_Mutex);
#elif defined(__APPLE__) || defined(LINUX)
	pthread_mutex_destroy(&m_Mutex);
#endif
}

//==================================================================================
// Blocking Lock.
//==================================================================================
void MemMutex::Lock(void)
{
#if defined(_WIN32) || defined(_XBOX)
	EnterCriticalSection(&m_Mutex);
#elif defined(__APPLE__) || defined(LINUX)
	if (!m_LockCount)
		pthread_mutex_lock(&m_Mutex);
	++m_LockCount;
#endif
}

//==================================================================================
// Unlock.
//==================================================================================
void MemMutex::Unlock(void)
{
#if defined(_WIN32) || defined(_XBOX)
	LeaveCriticalSection(&m_Mutex);
#elif defined(__APPLE__) || defined(LINUX)
	if (!m_LockCount)
		exit(-1);
	--m_LockCount;
	if (!m_LockCount)	
		pthread_mutex_unlock(&m_Mutex);
#endif
}


MemoryShutdown *gMemoryShutdown=0;

};

static SendTextMessage *gMemorySendTextMessage;

#define ODF_LOG if ( gMemorySendTextMessage ) gMemorySendTextMessage->sendTextMessage

#define USE_MUTEX 1

//*************************************************************
#ifdef new
#undef new
#endif

#ifdef delete
#undef
#endif

#ifdef malloc
#undef malloc
#endif

#ifdef realloc
#undef realloc
#endif

#ifdef free
#undef free
#endif


static const char * findLastSlash(const char *src)
{
  const char *ret = 0;
  while ( *src )
  {
    char c = *src++;
    if ( c == '/' || c == '\\' )
    {
      ret = src;
    }
  }

  return ret;

}

static const char * lastSlash(const char *src) // last forward or backward slash character, null if none found.
{

  const char *ret = src;

  while ( *src )
  {
    char c = *src++;
    if ( c == '/' || c == '\\' )
    {
      ret = src;
    }
  }

  return ret;
}

//==============================================================================
//
//  Next, the public functions which provide the standard C heap functionality.
//  These functions include appropriate DejaLib instrumentation.
//
//==============================================================================

//==============================================================================

#if defined(_WIN32WIN32)
	#ifndef _WIN32_WINNT
	#define _WIN32_WINNT 0x400
	#endif
	#include <windows.h>
#endif

#if defined(_XBOX)
	#include "NxXBOX.h"
#endif

#if defined(__APPLE__) || defined(LINUX)
	#include <pthread.h>
#endif


#pragma warning(disable:4311)
#pragma warning(disable:4786)

namespace MEMALLOC
{


//==================================================================================
enum MemHeaderFlag
{
  MHF_REPORTED    = (1<<0),
  MHF_NEW         = (1<<1),
  MHF_NEW_ARRAY   = (1<<2),
  MHF_MALLOC      = (1<<3),
  MHF_MICRO_ALLOC = (1<<4),    // a 'micro' allocation.  We don't do full tracking on this allocation type.
  MHF_INTERNAL    = (1<<5),
};



class FixedMemHeader
{
public:
	void SetMemHeaderFlag(MemHeaderFlag flag)
	{
		mFlags|=flag;
	};

	void ClearMemHeaderFlag(MemHeaderFlag flag)
	{
		mFlags&=~flag;
	};

	bool hasMemHeaderFlag(MemHeaderFlag flag) const
	{
		bool ret = false;
		if ( mFlags & flag )
			ret = true;
		return ret;
	};
  unsigned short  mMemoryPool;
  unsigned char   mShortSize;
  unsigned char   mFlags;
};

class MemHeader;
class MemFooter;

class KeyRep;

enum KeyReportType
{
	KR_BY_KEY,           // report sorted on key
	KR_BY_COUNT,         // report sorted on count
	KR_BY_SIZE           // report sorted by size.
};

enum MemSort
{
	MS_NONE,
	MS_BY_CLASS_NAME,
	MS_BY_SOURCE_FILE,
};


class DefaultSendTextMessage : public SendTextMessage
{
public:
	bool         sendTextMessage(unsigned int client,const char *fmt,...)
  {
    bool ret = true;

  	char wbuff[8192];
    wbuff[8191] = 0;
  	_vsnprintf(wbuff,8191, fmt, (char *)(&fmt+1));
#ifdef _WIN32
    OutputDebugStringA((LPCSTR)wbuff);
#else
    printf("%s", wbuff );
#endif

    return ret;
  }

};

class CoreMem
{
public:
  CoreMem     *mNext;
  unsigned int mSize;
};

//==================================================================================
static const char * formatNumber(int number,bool quotes=false) // JWR  format this integer into a fancy comma delimited string
{
#define MAXNUMERIC 32  // JWR  support up to 16 32 character long numeric formated strings
#define MAXFNUM    16
  static int   mIndex=0;
  static char  mFormat[MAXNUMERIC*MAXFNUM];
  char * dest = &mFormat[mIndex*MAXNUMERIC];

  ++mIndex;

  if ( mIndex == MAXFNUM )
    mIndex = 0;

  char scratch[512];

#if defined (LINUX_GENERIC) || defined(LINUX)
  sprintf(scratch, (const char*)10, "%d", number);
#else
  itoa(number,scratch,10);
#endif

  char *str = dest;

  if ( quotes )
  {
    *str++ = 34;
  }

  unsigned int len = (unsigned int)strlen(scratch);
  for (unsigned int i=0; i<len; i++)
  {
    int place = (len-1)-i;
    *str++ = scratch[i];
    if ( place && (place%3) == 0 ) *str++ = ',';
  }

  if ( quotes )
  {
    *str++ = 34;
  }
  *str = 0;

  return dest;
}

static DefaultSendTextMessage gDSTM;

typedef std::vector< MemAlloc * > MemAllocVector;

class MemAlloc
{
public:

	MemAlloc(const char *heapName);
	~MemAlloc(void);

  void   generateReport(unsigned int count,KeyRep **keys,KeyReportType type,HTML_TABLE::HtmlTable *table,MemSort ms);

  void * sysMalloc(size_t size)
  {
#if SYS_WINDOWS_HEAP
    return HeapAlloc(mHeap,0,size);
#endif

#if SYS_MALLOC_FREE
    return ::malloc(size);
#endif

#if SYS_DLMALLOC
    return mspace_malloc(mMspace,size);
#endif
  }

  void sysFree(void *mem)
  {

#if SYS_WINDOWS_HEAP
    HeapFree(mHeap,0,mem);
#endif

#if SYS_MALLOC_FREE
    ::free(mem);
#endif

#if SYS_DLMALLOC
    mspace_free(mMspace,mem);
#endif
  }

  void * sysRealloc(void *mem,size_t size)
  {
#if SYS_WINDOWS_HEAP
    return HeapReAlloc(mHeap,0,mem,size);
#endif
#if SYS_MALLOC_FREE
    return ::realloc(mem,size);
#endif
#if SYS_DLMALLOC
    return mspace_realloc(mMspace,mem,size);
#endif
  }

  void internalCoreDump(SendTextMessage *stm,const char *header,bool csv);

  char * alignBlock(char *buffer,unsigned int alignment);

	void * Malloc(unsigned int size,unsigned int alignment,const char *type,const char *file,int lineno,MemAllocType mtype);
	void   Free(void *mem,MemAllocType mtype);

	void * Realloc(void *mem,unsigned int size,const char *file,int lineno);
	void   WalkHeap(const char *label);

	void HeapCheck(void);

	int   Report(const char *heading,SendTextMessage *tmessage,bool checkLeak);

	void GetMemStat(int &mc,int &ms)
	{
		mc = mMemCount;
		ms = mMemSize;
	};

	unsigned int GetMemoryUsed(unsigned int &unused);

	bool IsEmpty(void)
	{
		if ( mMemSize == 0 ) return true;
		return false;
	}

  void    setSendTextMessage(SendTextMessage *stm)
  {
    gMemorySendTextMessage = stm;
  }


  void frameBegin(SendTextMessage *stm)
  {
    gMemorySendTextMessage = stm;
    mFrameCapture = true;
    mFrameAllocCount = 0;
    mFrameFreeCount  = 0;
    mFrameTotalAlloc = 0;
    mFrameTotalFree  = 0;
  }

  void frameEnd(SendTextMessage *stm,const char *header)
  {
    gMemorySendTextMessage = stm;

    ODF_LOG(0,"FRAME_END(%s) FrameAllocCount: %d FrameFreeCount: %d  FrameAllocBytes: %10s FrameFreeBytes: %10s  FrameLeak: %10s\r\n", header,  mFrameAllocCount,mFrameFreeCount,formatNumber(mFrameTotalAlloc), formatNumber( mFrameTotalFree ), formatNumber( mFrameTotalAlloc - mFrameTotalFree ) );

    mFrameCapture = false;
  }

  void heapCompact(SendTextMessage *stm)
  {
/********* TODO TODO
    unsigned int bsize,asize;
    unsigned int bcount = getHeapCount(mHeap,bsize);
    HeapCompact( mHeap, 0 );
    unsigned int acount = getHeapCount(mHeap,asize);
    if ( stm )
    {
      stm->sendTextMessage(0,"HEAP BEFORE: %s bytes AFTER COMPACTION: %s bytes.  Count before: %s Count After: %s\r\n", formatNumber(bsize), formatNumber(asize), formatNumber(bcount), formatNumber(acount) );
    }
**********/
  }

  void summaryReport(HTML_TABLE::HtmlTable *table,const char *header); // add a new row to the summary table



  FixedAllocator * getFixedAllocator(unsigned int size)
  {
    FixedAllocator *ret = 0;

    if ( size <= 4 )
      ret = &mAlloc4;
    else if ( size <= 8 )
      ret = &mAlloc8;
    else if ( size <= 16 )
      ret = &mAlloc16;
    else if ( size <= 32 )
      ret = &mAlloc32;
    else if ( size <= 64 )
      ret = &mAlloc64;
    else if ( size <= 128 )
      ret = &mAlloc128;

    return ret;
  }

	void   KeyReport(const char *key,unsigned int lineno,bool iskey,KeyRep &rep); // generate summary report,based on key, or filename

	void   GenerateReport(unsigned int count,KeyRep **keys,KeyReportType type,const char *header,MemSort ms );

  void setMemoryPoolIndex(unsigned short index)
  {
    mMemoryPoolIndex = index;
  }

  unsigned short getMemoryPoolIndex(void) const
  {
    return mMemoryPoolIndex;
  }


  void fixedReport(FixedAllocator &alloc,const char *header,int fsize,HTML_TABLE::HtmlTable *table)
  {
    std::size_t empty_chunks = mAlloc4.CountEmptyChunks();
    bool isCorrupt           = mAlloc4.IsCorrupt();
    const char *corrupt = "FALSE";
    if ( isCorrupt ) corrupt = "TRUE";

    int allocCount = alloc.getAllocCount();

    if ( allocCount > 0 )
    {
      table->addColumn(header);
      table->addColumn(allocCount);
      table->addColumn(allocCount*fsize);
      table->addColumn((unsigned int)empty_chunks);
      table->addColumn(corrupt);
      table->nextRow();
    }
  }

  void fixedDetailed(int acount,int asize,HTML_TABLE::HtmlTable *table)
  {
    if ( acount > 0 )
    {
      int fsize = 0;
      if ( asize <= 4 )
      {
        fsize = 4;
      }
      else if ( asize <= 8 )
      {
        fsize = 8;
      }
      else if ( asize <= 16 )
      {
        fsize = 16;
      }
      else if ( asize <= 32 )
      {
        fsize = 32;
      }
      else if ( asize <= 64 )
      {
        fsize = 64;
      }
      else
      {
        HE_ASSERT( asize <= 128 );
        fsize = 128;
      }

      char scratch[512];
      sprintf(scratch,"Bytes %3d", asize );
      table->addColumn(scratch);

      sprintf(scratch,"Fixed %3d", fsize );
      table->addColumn(scratch);

      table->addColumn(acount);
      table->addColumn(acount*asize);
      table->addColumn(acount*sizeof(FixedMemHeader));

      int unused = (fsize*acount) - (asize*acount);
      table->addColumn(unused);

      table->nextRow();

    }
  }

  void reportByClass(const char *header,HTML_TABLE::HtmlDocument *document);
  void reportBySourceFile(const char *header,HTML_TABLE::HtmlDocument *document);

  void fixedPoolReport(const char *header,HTML_TABLE::HtmlDocument *document)
  {
    if ( mMicroAllocCount > 0 )
    {
      char scratch[512];
      sprintf(scratch,"Fixed Memory Pool Report for Heap '%s' in '%s'", mHeapName, header );

      HTML_TABLE::HtmlTable *table = document->createHtmlTable(scratch);
      table->computeTotals();
      table->addHeader("Fixed/Size,Alloc/Count,Alloc/Size,Empty/Chunks,Corrupt");
      table->addSort("Sorted by Total Size",3,false,0,false);

      fixedReport(mAlloc4,  "Fixed  4",4,table);
      fixedReport(mAlloc8,  "Fixed  8",8,table);
      fixedReport(mAlloc16, "Fixed  16",16,table);
      fixedReport(mAlloc32, "Fixed  32",32,table);
      fixedReport(mAlloc64, "Fixed  64",64,table);
      fixedReport(mAlloc128,"Fixed 128",128,table);

      if ( 1 )
      {
        sprintf(scratch,"Detailed Fixed Memory Pool Report for Heap '%s' in '%s'", mHeapName, header );
        HTML_TABLE::HtmlTable *table = document->createHtmlTable(scratch);
        table->computeTotals();
        table->addHeader("Byte/Size,Fixed/Size,Alloc/Count,Alloc/Size,Overhead,Unused");
        table->addSort("Sorted by Unused Memory",5,false,0,false);

        for (int i=0; i<(128+1); i++)
        {
          int acount = mFixedAllocCount[i];
          fixedDetailed(acount,i,table);
        }

      }

    }
  }

  void setName(const char *name)
  {
    mHeapName = name;
  }

  const char *mHeapName;

#if LOG
  FILE *mLog;
#endif

#if SYS_WINDOWS_HEAP
  HANDLE              mHeap;
#endif
#if SYS_DLMALLOC
  mspace              mMspace;
#endif


  MemMutex            mMutex;

  unsigned short      mMemoryPoolIndex;

  int                 mHeapCheck;

  bool                mFrameCapture;

  int mFrameAllocCount;
  int mFrameFreeCount;
  int mFrameTotalAlloc;
  int mFrameTotalFree;



	MemHeader          *mUsed; // head of used list


  int                 mMicroAllocCount;
  int                 mMicroAllocSize;

	int                 mMemCount; // number of memory allocations performed since last query.
	int                 mMemSize;  // size of memory alloced.

	KeyReportType       mSortKey;

  FixedAllocator     *mAllocators[128+1];
  int                 mFixedAllocCount[128+1];

  FixedAllocator      mAlloc4;
  FixedAllocator      mAlloc8;
  FixedAllocator      mAlloc16;
  FixedAllocator      mAlloc32;
  FixedAllocator      mAlloc64;
  FixedAllocator      mAlloc128;


};

void GetHeapSize( int &used, int &unused );


#define HEAP_CHECK 0
#define DEBUG_ALLOC 0
#define DEBUG_GLOBAL_NEW 0
#define DEBUG_TYPE ""
static bool INTERNAL_DISABLE_TRACKING=false; // debug aid only.

MemAlloc *gMemAlloc=0;


const unsigned short MEMID=0xD0B4;


FORCE_NOINLINE void invalidMemoryBlock(void)
{
  HE_ASSERT(0);
  char *c = 0;
  *c = 0;          // null memory access,this will cause a crash.
}

FORCE_NOINLINE void invalidMemoryType(void)
{
  HE_ASSERT(0);
  char *c = 0;
  *c = 0;          // null memory access,this will cause a crash.
}


FORCE_NOINLINE void invalidMemoryFlag(void)
{
  HE_ASSERT(0);
  char *c = 0;
  *c = 0;          // null memory access,this will cause a crash.
}


FORCE_NOINLINE void invalidMemoryTypeField(void)
{
  HE_ASSERT(0);
  char *c = 0;
  *c = 0;          // null memory access,this will cause a crash.
}


FORCE_NOINLINE void missmatchedMemoryAllocation(void)
{
  HE_ASSERT(0);
  char *c = 0;
  *c = 0;          // null memory access,this will cause a crash.
}

FORCE_NOINLINE void invalidAlignment(void)
{
  HE_ASSERT(0);
  char *c = 0;
  *c = 0;          // null memory access,this will cause a crash.
}


FORCE_NOINLINE void invalidMemoryTypeMalloc(void)
{
  HE_ASSERT(0);
  char *c = 0;
  *c = 0;          // null memory access,this will cause a crash.
}


FORCE_NOINLINE void failedToAllocateMemory(void)
{
  HE_ASSERT(0);
  char *c = 0;
  *c = 0;          // null memory access,this will cause a crash.
}


FORCE_NOINLINE void corruptedMemoryHeap(void)
{
  HE_ASSERT(0);
  char *c = 0;
  *c = 0;          // null memory access,this will cause a crash.
}


class MemFooter
{
public:
	const char      *mTag;      //  4
	const char      *mFile;     //  8
	unsigned int     mLineNo;   // 12
	MemHeader       *mNext;     // 16
	MemHeader       *mPrevious; // 20
  MemAlloc        *mMemAlloc; // 24  The memory allocator this block of memory was allocated from!
};

//==================================================================================
//==================================================================================
class MemHeader
{
public:

	unsigned int ComputeCRC(void)
	{
    MemFooter *mf = getMemFooter();
    unsigned int c1 = (unsigned int) mBaseMemory;
    unsigned int c2 = (unsigned int)mf->mFile;
    unsigned int c3 = (unsigned int)mf->mLineNo;
    unsigned int c4 = (unsigned int)mSize;
    unsigned int c5 = (unsigned int)mf->mMemAlloc;
    unsigned int c6 = (unsigned int)mMemoryPool;

    unsigned int crc = c1 ^ c2 ^ c3 ^c4 ^ c5 ^ c6;

		return crc;
	}

	bool Validate(int heapCheck)
	{
    bool ret = true;
    if ( !hasMemHeaderFlag(MHF_MICRO_ALLOC) )
    {
      MemFooter *mf = getMemFooter();
		  HE_ASSERT( (ComputeCRC() == mCRC) );
		  if ( ComputeCRC() != mCRC )
      {
        ret = false;
        if ( heapCheck == 1 )
        {
          HE_ASSERT(0);
          if ( gMemoryShutdown )
          {
            gMemoryShutdown->memoryAllocationFailure("CORRUPTED HEAP!",__FILE__,__LINE__,0);
          }
          invalidMemoryBlock();
        }
      }
    }
		return ret;
	}

	void Report(void)
	{
    MemFooter *mf = getMemFooter();
		ODF_LOG(1,"                  MEMTYPE: %-32s    FILE: %-60s  LINE: %4d\r\n", mf->mTag, mf->mFile, mf->mLineNo );
	}

	void SetMemHeaderFlag(MemHeaderFlag flag)
	{
		mFlags|=flag;
	};

	void ClearMemHeaderFlag(MemHeaderFlag flag)
	{
		mFlags&=~flag;
	};

	bool hasMemHeaderFlag(MemHeaderFlag flag) const
	{
		bool ret = false;
		if ( mFlags & flag )
			ret = true;
		return ret;
	};

  const char * getMemTypeString(MemAllocType type) const
  {
    const char *ret = "unknown";
    switch ( type )
    {
      case MAT_NEW: ret = "operator new"; break;
      case MAT_MALLOC: ret = "std::malloc"; break;
      case MAT_NEW_ARRAY: ret = "operator new array"; break;
      case MAT_INTERNAL: ret = "internal"; break;
      default:
        HE_ASSERT(0);
        invalidMemoryType();
        break;
    }
    return ret;
  }

  const char * getMemTypeString(void) const
  {
    const char *ret = "unknown";

    unsigned int flag = mFlags & (MHF_NEW | MHF_MALLOC | MHF_NEW_ARRAY | MHF_INTERNAL);

    MemAllocType type = MAT_NEW;

    switch ( flag )
    {
      case MHF_NEW:    type = MAT_NEW; break;
      case MHF_MALLOC: type = MAT_MALLOC; break;
      case MHF_NEW_ARRAY:  type = MAT_NEW_ARRAY; break;
      case MHF_INTERNAL: type = MAT_INTERNAL; break;
      default:
        HE_ASSERT(0);
        invalidMemoryFlag();
        break;
    }

    return getMemTypeString(type);
  }

  void validateMemType(MemAllocType type,SendTextMessage *stm)
  {

    unsigned int flag = mFlags & (MHF_NEW | MHF_MALLOC | MHF_NEW_ARRAY | MHF_INTERNAL);

    bool bad = false;

    switch ( flag )
    {
      case MHF_INTERNAL:
        HE_ASSERT( type == MAT_INTERNAL );
        if ( type != MAT_INTERNAL )
        {
          bad = true;
        }
        break;
      case MHF_NEW:
        HE_ASSERT(type == MAT_NEW );
        if ( type != MAT_NEW )
        {
          if ( type == MAT_NEW_ARRAY )
          {
            if ( 1 )
            {
              bad = true;
              HE_ASSERT(0);
            }
          }
          else
          {
            bad = true;
            HE_ASSERT(0);
          }
        }
        break;
      case MHF_MALLOC:
        HE_ASSERT( type == MAT_MALLOC );
        if ( type != MAT_MALLOC )
        {
          bad = true;
        }
        break;
      case MHF_NEW_ARRAY:
        if ( type != MAT_NEW_ARRAY )
        {
          if ( type == MAT_NEW )
          {
            if ( 1 )
            {
              // Cannot treat this case as 'bad' because there is an error in the STD::memory header file!!
//              bad = true;
//              HE_ASSERT(0);
            }
          }
          else
          {
            HE_ASSERT(0);
            bad = true;
          }
        }
        break;
      default:
        HE_ASSERT(0);
        invalidMemoryTypeField();
        break;
    }

    if ( bad )
    {
      HE_ASSERT(0);

      if ( stm )
      {
        MemFooter *mf = getMemFooter();
        if ( mf )
        {
          stm->sendTextMessage(0,"MEMALLOC ERROR! -> block allocated with (%s) but deleted with %s.  MicroAllocSize: %d  Type(%s) File(%s) LineNo: %s\r\n",  getMemTypeString(type), getMemTypeString(), mSize, mf->mTag, mf->mFile, formatNumber(mf->mLineNo) );
        }
        else
        {
          stm->sendTextMessage(0,"MEMALLOC ERROR! -> block allocated with (%s) but deleted with %s.  MicroAllocSize: %d\r\n",  getMemTypeString(type), getMemTypeString(), mSize);
        }

      }
      else
      {
        missmatchedMemoryAllocation();
      }
    }

  }

  MemFooter * getMemFooter(void)
  {
    MemFooter *ret = 0;

    if ( !hasMemHeaderFlag(MHF_MICRO_ALLOC) )
    {
      MemHeader *mh = this;
      mh++;
      char *foo = (char *) mh;
      foo+=mSize;
      ret = (MemFooter *) foo;
    }

    return ret;
  }


	int              mSize;       // 4
  void            *mBaseMemory; // 12
  unsigned int     mCRC;        // 16
  unsigned short   mMemoryPool;
  unsigned char    mShortSize;
  unsigned char    mFlags;
};



//==================================================================================
MemAlloc::MemAlloc(const char *heapName)
{
#if LOG
  mLog = fopen("memlog.txt", "wb");
#endif

  mMemoryPoolIndex = 0;

  mHeapName = heapName;
  mHeapCheck = 0;
  mFrameCapture = false;
	mUsed = 0;
	mMemCount = 0;
	mMemSize  = 0;
  mMicroAllocCount = 0;
  mMicroAllocSize = 0;

  #if SYS_WINDOWS_HEAP
  mHeap = HeapCreate(0,DEFAULT_PAGE_SIZE,0);
  ULONG htype = 2;
  HeapSetInformation(mHeap,HeapCompatibilityInformation,&htype,sizeof(htype));
  #endif
  #if SYS_DLMALLOC
  mMspace = create_mspace(0,0);
  #endif

  mAlloc4.Initialize(4+8,DEFAULT_PAGE_SIZE,this);
  mAlloc8.Initialize(8+8,DEFAULT_PAGE_SIZE,this);
  mAlloc16.Initialize(16+8,DEFAULT_PAGE_SIZE,this);
  mAlloc32.Initialize(32+8,DEFAULT_PAGE_SIZE,this);
  mAlloc64.Initialize(64+8,DEFAULT_PAGE_SIZE,this);
  mAlloc128.Initialize(128+8,DEFAULT_PAGE_SIZE,this);

  for (unsigned int i=0; i<=128; i++)
  {
    mFixedAllocCount[i] = 0;
    mAllocators[i] = getFixedAllocator(i);
  }

}

//==================================================================================
MemAlloc::~MemAlloc(void)
{
#if SYS_DLMALLOC
  destroy_mspace(mMspace);
#endif
#if SYS_WINDOWS_HEAP
  HeapDestroy(mHeap);
#endif
#if LOG
  fclose(mLog);
#endif
}

char * MemAlloc::alignBlock(char *buffer,unsigned int alignment)
{
  char *ret = 0;

  unsigned int shift=0;

  switch ( alignment )
  {
  case 0:
  case 1:
    break;
  case 2:
    shift = 1;
    break;
  case 4:
    shift = 2;
    break;
  case 8:
    shift = 3;
    break;
  case 16:
    shift = 4;
    break;
  case 32:
    shift = 5;
    break;
  case 64:
    shift = 6;
    break;
  case 128:
    shift = 7;
    break;
  case 256:
    shift = 8;
    break;
  default:
    HE_ASSERT(0);
    invalidAlignment();
    break;
  }

  if ( shift )
  {
    HeU64 v = (unsigned int)buffer;
    v = v+(alignment+sizeof(MemHeader));
    v = v>>shift;
    v = v<<shift;
    buffer =  (char *) v;
  }
  return buffer;
}


//==================================================================================
void * MemAlloc::Malloc(unsigned int size,unsigned int alignment,const char *type,const char *file,int lineno,MemAllocType mtype)
{
	void * ret = 0;

  HE_ASSERT(type);
  HE_ASSERT(file);
  HE_ASSERT(lineno>0);

  if ( INTERNAL_DISABLE_TRACKING )
  {
    ret = sysMalloc(size);
  }
  else
  {

    #if USE_MUTEX
  	mMutex.Lock();
    #endif

    mFrameAllocCount++;
    mFrameTotalAlloc+=size;

  	unsigned char flags = 0;
    switch ( mtype )
    {
      case MAT_MALLOC:
        flags = MHF_MALLOC;
        break;
      case MAT_NEW:
        flags = MHF_NEW;
        break;
      case MAT_INTERNAL:
        flags = MHF_INTERNAL;
        break;
      case MAT_NEW_ARRAY:
        flags = MHF_NEW_ARRAY;
        break;
      default:
        HE_ASSERT(0);
        invalidMemoryTypeMalloc();
        break;
    }

    #if HEAP_CHECK
  	HeapCheck();
    #endif

    #if DEBUG_ALLOC
  	if ( stricmp(type,DEBUG_TYPE) == 0 )
  	{
  		printf("debug alloc");
  	}
    #endif

    #if DEBUG_GLOBAL_NEW
    if ( (strcmp(gComponentName,"default") != 0) && (strcmp(type,"GlobalNew") == 0) )
    {
      printf("debug global new");
    }
    #endif


    if ( size <= gMaxFixedMemorySize  && alignment <= 8 && HE_USE_FIXED_POOL_ALLOCATOR )
    {
      gFmcount++;
      gFmsize+=size;

      mMicroAllocCount++;
      mMicroAllocSize+=size;

      FixedAllocator *fa = mAllocators[size];
      mFixedAllocCount[size]++;

      char *baseMemory   = (char *)fa->Allocate();
      if ( baseMemory == 0 )
      {
        HE_ASSERT(0);
        if ( gMemoryShutdown )
        {
          gMemoryShutdown->memoryAllocationFailure(type,file,lineno,size + sizeof(MemHeader) + alignment*2);
        }
        failedToAllocateMemory();
      }

      FixedMemHeader *mh = (FixedMemHeader *) baseMemory;

      mh->mShortSize = size;
      mh->mFlags     = flags |MHF_MICRO_ALLOC;
      mh->mMemoryPool = mMemoryPoolIndex;

     	mh++;

     	ret = mh;
    }
    else
    {
      gMcount++;
      gMsize+=size;

      char *baseMemory  = (char *)sysMalloc( size + sizeof(MemHeader) + sizeof(MemFooter) + alignment*2 );
      if ( baseMemory == 0 )
      {
        HE_ASSERT(0);

        if ( gMemoryShutdown )
        {
          gMemoryShutdown->memoryAllocationFailure(type,file,lineno,size + sizeof(MemHeader) + sizeof(MemFooter) + alignment*2);
        }
        failedToAllocateMemory();
      }

      char *alignMemory = baseMemory;

      if ( alignment )
      {
        alignMemory = alignBlock(baseMemory,alignment);
        alignMemory-=sizeof(MemHeader);

        while ( alignMemory < baseMemory )
        {
          alignMemory+=alignment;
        }
      }

      MemHeader *mh = (MemHeader *) alignMemory;

     	mMemCount++;
     	mMemSize+=size;
      mh->mShortSize = size&0xFFFF; // just keep the bottom 16 bits.
     	mh->mFlags	    = flags;
     	mh->mSize	    	= size;
      mh->mBaseMemory = baseMemory;
      mh->mMemoryPool = mMemoryPoolIndex;

      MemFooter *mf = mh->getMemFooter();

     	mf->mTag	  = type;
     	mf->mFile   = file;
     	mf->mLineNo	= lineno;
     	mf->mNext		= mUsed;        // our next is the old head of the used list
     	mf->mPrevious	= 0;            // since we are the new head, we have no previous
      mf->mMemAlloc = this;

     	mh->mCRC		= mh->ComputeCRC();

     	if ( mUsed ) // the old head of the used list's previous is now us..
     	{
        MemFooter *mf2 = mUsed->getMemFooter();
     		mf2->mPrevious = mh;
     	}

     	mUsed            = mh;

     	mh++;

     	ret = mh;
    }

    if ( mFrameCapture )
    {
    	ODF_LOG(1,"Malloc(%08X) Size: %d Type: %s File: %s Line: %d\r\n", ret, size, type, file, lineno );
    }


    #if USE_MUTEX
  	mMutex.Unlock();
    #endif
  }

#if ZERO_MEMORY
  if ( ret )
  {
    memset(ret,0,size);
  }
#endif


#if LOG
  fprintf(mLog,"%08X = MemAlloc::malloc(%d,%d,%s,%s,%d)\r\n", ret, size, alignment, type, file, lineno );
  fflush(mLog);
#endif

	return ret;
}

//==================================================================================
void    MemAlloc::Free(void *mem,MemAllocType mtype)
{

#if LOG
  fprintf(mLog,"free(%08x)\r\n", mem );
  fflush(mLog);
#endif

  if ( INTERNAL_DISABLE_TRACKING )
  {
    sysFree(mem);
  }
  else
  {
    #if USE_MUTEX
  	mMutex.Lock();
    #endif

    #if HEAP_CHECK
  	HeapCheck();
    #endif

    #if DEBUG_ALLOC
  	if ( stricmp(type,DEBUG_TYPE) == 0 )
  	{
  		printf("debug alloc");
  	}
    #endif

  	if ( mem )
  	{
      FixedMemHeader *fmh = (FixedMemHeader *) mem;
      fmh--;
      if ( fmh->hasMemHeaderFlag(MHF_MICRO_ALLOC) )
      {
        {
          mMicroAllocCount--;
          mMicroAllocSize-=fmh->mShortSize;

          if ( mFrameCapture )
            ODF_LOG(1,"Free: %08X  Size: %d bytes\r\n", mem, (unsigned int)fmh->mShortSize );

          mFrameFreeCount++;
          mFrameTotalFree+=fmh->mShortSize;

          HE_ASSERT( fmh->mShortSize <= 128 );

          FixedAllocator *fa = mAllocators[fmh->mShortSize];
          HE_ASSERT( mFixedAllocCount[fmh->mShortSize] > 0 );
          mFixedAllocCount[fmh->mShortSize]--;

          bool ok = fa->Deallocate(fmh,0);

          HE_ASSERT(ok);
        }
      }
      else
      {


      	MemHeader *mh       = (MemHeader *) mem;
      	mh--;
      	mh->Validate(1); // validate that it is all good

        MemFooter *mf = mh->getMemFooter();

        mh->validateMemType(mtype,gMemorySendTextMessage);

        if ( mf->mMemAlloc != this )
        {
          MemAlloc *pool = mf->mMemAlloc;

//***          ODF_LOG(0,"MEM(%08X) Was allocated on '%s' but was attempted to be freed in '%s'. TAG(%s) File(%s) LineNo: %d\r\n",mem,  pool->mHeapName, mHeapName, mf->mTag, mf->mFile, mf->mLineNo );

          #if USE_MUTEX
        	mMutex.Unlock();
          #endif
          pool->Free(mem,mtype);
          #if USE_MUTEX
        	mMutex.Lock();
          #endif
        }
        else
        {

          HE_ASSERT( mh->mMemoryPool == mMemoryPoolIndex );

          mFrameFreeCount++;
          mFrameTotalFree+=mh->mSize;


          HE_ASSERT(mMemCount>0);
          HE_ASSERT( mf->mMemAlloc == this ); // If this assert fires, then we tried to de-allocate a block of memory on a different pool than it was allocated on!!!

          if ( mFrameCapture )
            ODF_LOG(1,"Free: %08X  Size: %d bytes Type: %s File: %d Lineno: %d\r\n", mem, mh->mSize, mf->mTag, mf->mFile, mf->mLineNo );

        	if ( 1 )
        	{
        		if ( mh == mUsed )
        		{
        			mUsed = mf->mNext;
        			if ( mUsed )
      				  mUsed->getMemFooter()->mPrevious = 0;
        		}
        		else
        		{
        			MemHeader *prev = mf->mPrevious;
        			MemHeader *next = mf->mNext;
        			if ( prev )
        			{
        				prev->getMemFooter()->mNext = next;
        			}
        			if ( next )
        			{
        				next->getMemFooter()->mPrevious = prev;
        			}
        		}

        		mMemCount--;
        		mMemSize -= mh->mSize;

        		if ( mMemSize < 0 || mMemCount < 0 )
        		{
              HE_ASSERT(0);
              corruptedMemoryHeap();
      		  }

        		HE_ASSERT( mMemCount >= 0 );
        		HE_ASSERT( mMemSize  >= 0 );

            mh->mCRC = 0;


         		sysFree(mh->mBaseMemory);
          }
        }
      }
  	}

    #if USE_MUTEX
  	mMutex.Unlock();
    #endif
  }

}


#define MAXTYPE 8192

//==================================================================================
//==================================================================================
class MemKey
{
public:
	const char *mKey;
	unsigned int mLineNo;
};

//==================================================================================

namespace
{
  void AddKey(const char *key,unsigned int lineno,MemKey *keys,unsigned int &count)
  {
	  for (unsigned int i=0; i<count; i++)
	  {
		  if ( ( strcmp(keys[i].mKey,key) == 0 ) && ( keys[i].mLineNo == lineno ) )
			  return;
	  }
	  if ( count < MAXTYPE )
	  {
		  keys[count].mKey    = key;
		  keys[count].mLineNo = lineno;
		  count++;
	  }
  }
}

//==================================================================================
//==================================================================================
class KeyRep
{
public:

  unsigned int mAllocCount;         // number of allocations done based on this key.
  unsigned int mAllocSize;          // total size of allocations performed on this key.
  const char * mKey;
  const char * mSubKey;
  int          mLineNo;

};

//==================================================================================
unsigned int MemAlloc::GetMemoryUsed(unsigned int &unused)
{
  unsigned int used = mMemSize;
  unused            = mMemCount*(sizeof(MemHeader)+sizeof(MemFooter));

  return used;
}

#define MEGABYTE 1048576

void MemAlloc::summaryReport(HTML_TABLE::HtmlTable *table,const char *header) // add a new row to the summary table
{

  unsigned int internal_count = 0;
  unsigned int internal_size  = 0;

  unsigned int global_new_count = 0;
  unsigned int global_new_size  = 0;

  unsigned int global_new_array_count = 0;
  unsigned int global_new_array_size  = 0;

  unsigned int general_count = 0;
  unsigned int general_size  = 0;


  MemHeader *mh = mUsed;

	while ( mh )
	{
    MemFooter *mf = mh->getMemFooter();

    if ( strcmp(mf->mTag,"@internal") == 0 )
    {
      internal_count++;
      internal_size+=mh->mSize;
    }
    else if ( strcmp(mf->mTag,"GlobalNew") == 0 )
    {
      global_new_count++;
      global_new_size+=mh->mSize;
    }
    else if ( strcmp(mf->mTag,"GlobalNewArray") == 0 )
    {
      global_new_array_count++;
      global_new_array_size+=mh->mSize;
    }
    else
    {
      general_count++;
      general_size+=mh->mSize;
    }

		mh = mf->mNext; // walk the linked list
	}


  // 1 Header
  // 2 Total Memory
  // 3 Allocation Count
  // 4 General Memory
  // 5 General Acount
  // 6 MicroAllocationSize
  // 7 MicroAllocationCount
  // 8 TotalMicroAllocationCount
  // 9  Object Pool Micro-Allocation Count
  // 10 Global New Memory
  // 11 Global New Allocation Count
  // 12 Global New Array memory
  // 13 Global New Array Allocation Count

  table->addColumn(header);
  table->addColumn(mMemSize);
  table->addColumn(mMemCount);

  table->addColumn(general_size);
  table->addColumn(general_count);

  table->addColumn(internal_size);
  table->addColumn(internal_count);
  table->addColumn(mMicroAllocCount);


  table->addColumn(global_new_size);
  table->addColumn(global_new_count);

  table->addColumn(global_new_array_size);
  table->addColumn(global_new_array_count);

  table->nextRow();


}

void MemAlloc::reportBySourceFile(const char *header,HTML_TABLE::HtmlDocument *document)
{

  char scratch[512];
  sprintf(scratch,"Memory Report by Source File for '%s'", header );
  HTML_TABLE::HtmlTable *table = document->createHtmlTable(scratch);
  table->computeTotals();

  table->addHeader("Source/File,Line/Number,Class/Tag,Alloc/Count,Alloc/Size,Full Source File Path Name");
  table->addSort("Sorted by Allocation Size",5,false,0,false);
//  table->addSort("Sorted by Source File and Line Number",1,true,2,true);

	MemHeader *mh = mUsed;

	unsigned int tfilecount = 0;

	MemKey       tfiles[MAXTYPE];

	while ( mh )
	{
    MemFooter *mf = mh->getMemFooter();
		AddKey(mf->mFile, mf->mLineNo, tfiles, tfilecount );
		mh = mh->getMemFooter()->mNext; // walk the linked list
	}

	KeyRep   filereport[MAXTYPE];
	KeyRep  *fileptr[MAXTYPE];

	if ( tfilecount )
	{
		for (unsigned int i=0; i<tfilecount; i++)
		{
			fileptr[i] = &filereport[i];
			KeyReport( tfiles[i].mKey, tfiles[i].mLineNo, false, filereport[i] );
		}
 		generateReport( tfilecount, fileptr, KR_BY_SIZE, table, MS_BY_SOURCE_FILE );
	}

}

static inline int mean(int sz,int ct)
{
  int ret = 0;
  if ( ct > 0 )
  {
    ret = sz / ct;
  }
  return ret;
}

void MemAlloc::reportByClass(const char *header,HTML_TABLE::HtmlDocument *document)
{
  char scratch[512];
  sprintf(scratch,"Memory Report by Class Name or Tag for '%s'", header );
  HTML_TABLE::HtmlTable *table = document->createHtmlTable(scratch);
  table->computeTotals();

  table->addHeader("Class-Tag/Source File,Alloc/Count,Alloc/Size,Full Source File Path Name");
  table->addSort("Sorted by Class Size",3,false,0,false);
//  table->addSort("Sorted by Class Name",1,true,0,false);


	MemHeader *mh = mUsed;

	unsigned int tkeycount = 0;

	MemKey       tkeys[MAXTYPE];

	while ( mh )
	{
    MemFooter *mf = mh->getMemFooter();

		AddKey(mf->mTag,  0,           tkeys,  tkeycount );
		mh = mh->getMemFooter()->mNext; // walk the linked list
	}

  KeyRep   keyreport[MAXTYPE];
  KeyRep  *keyptr[MAXTYPE];

	if ( tkeycount )
	{
		for (unsigned int i=0; i<tkeycount; i++)
		{
      keyptr[i] = &keyreport[i];
 			KeyReport( tkeys[i].mKey, 0, true, keyreport[i] );
 		}
 		generateReport( tkeycount, keyptr, KR_BY_SIZE, table, MS_BY_CLASS_NAME );
	}

}

//==================================================================================
int MemAlloc::Report(const char *heading,SendTextMessage *tmessage,bool checkLeak )
{
	int ret = mMemSize;

  if ( tmessage )
    gMemorySendTextMessage = tmessage;
  else
    gMemorySendTextMessage = &gDSTM;


  unsigned int tcount = 0;
  unsigned int tsize  = 0;



  unsigned int internal_count = 0;
  unsigned int internal_size  = 0;

  unsigned int global_new_count = 0;
  unsigned int global_new_size  = 0;

  unsigned int global_new_array_count = 0;
  unsigned int global_new_array_size  = 0;

  unsigned int general_count = 0;
  unsigned int general_size  = 0;


  MemHeader *mh = mUsed;

	while ( mh )
	{
    MemFooter *mf = mh->getMemFooter();

    if ( strcmp(mf->mTag,"@internal") == 0 )
    {
      internal_count++;
      internal_size+=mh->mSize;
    }
    else if ( strcmp(mf->mTag,"GlobalNew") == 0 )
    {
      global_new_count++;
      global_new_size+=mh->mSize;
    }
    else if ( strcmp(mf->mTag,"GlobalNewArray") == 0 )
    {
      global_new_array_count++;
      global_new_array_size+=mh->mSize;
    }
    else
    {
      general_count++;
      general_size+=mh->mSize;
    }

		mh = mf->mNext; // walk the linked list
	}

  ODF_LOG(0,"%s,%s,%s\r\n",heading,formatNumber(tsize+mMemSize),formatNumber(mMemCount+tcount) );

  return ret;
}

void MemAlloc::internalCoreDump(SendTextMessage *stm,const char *header,bool csv)
{
  // ok, now walk every single individual block using my own system.
  if ( csv )
  {
    stm->sendTextMessage(1,"[%s]\r\n", header );
    stm->sendTextMessage(1,"BaseAddress,Size,CRC,FLAGS,TAG,FILE,LINENO\r\n");
  }
  else
  {
    stm->sendTextMessage(1,"CoreDump of (%s) Tracked Memory items.  Count: %s  Size: %s\r\n", header, formatNumber(mMemCount), formatNumber(mMemSize));
  }
  unsigned int mcount = 0;
  unsigned int msize = 0;
  MemHeader *scan = mUsed;
  while ( scan )
  {
    MemFooter *mf = scan->getMemFooter();
    char flags[512];
    flags[0] = 0;
    if ( scan->mFlags & MHF_NEW ) strcat(flags,"new ");
    if ( scan->mFlags & MHF_NEW_ARRAY ) strcat(flags,"new array ");
    if ( scan->mFlags & MHF_MALLOC )    strcat(flags,"malloc ");
    if ( scan->mFlags & MHF_MICRO_ALLOC ) strcat(flags,"micro_alloc ");
    if ( scan->mFlags & MHF_INTERNAL ) strcat(flags,"internal ");
    if ( csv )
      stm->sendTextMessage(1,"\"0x%08X\",\"%s\",\"0x%08X\",\"%s\",\"%s\",\"%s\",\"%s\"\r\n", scan->mBaseMemory, formatNumber(scan->mSize), scan->mCRC, flags, mf->mTag, mf->mFile, formatNumber(mf->mLineNo));
    else
      stm->sendTextMessage(1,"BASEMEM(%08X) : size %10s CRC: %08X FLAGS: %-15s TAG: %-40s File: %-60s LineNo: %s\r\n", scan->mBaseMemory, formatNumber(scan->mSize), scan->mCRC, flags, mf->mTag, mf->mFile, formatNumber(mf->mLineNo));

    mcount++;
    msize+=scan->mSize;

    scan = mf->mNext;
  }
  if ( csv )
  {
    stm->sendTextMessage(1,"\"Count:%s\",\"Total:%s\"\r\n", formatNumber(mcount), formatNumber(msize) );
  }

}

//==================================================================================
void MemAlloc::KeyReport(const char *key,unsigned int lineno,bool iskey,KeyRep &rep)
{
	int acount = 0; // number of allocations
	int asize  = 0; // total allocations.

	MemHeader *mh = mUsed;

	const char *keyfound = "";

	while ( mh )
	{
		bool match = false;
    MemFooter *mf = mh->getMemFooter();

		if ( iskey )
		{
			if (strcmp(key,mf->mTag) == 0 )
				match = true;
		}
		else
		{
			if ( strcmp(key,mf->mFile) == 0 && mf->mLineNo == lineno )
			{
				match = true;
				keyfound = mf->mTag;
			}
		}

		if ( match )
		{
			acount++;
			asize += mh->mSize;
		}

		mh = mf->mNext;
	}

	rep.mAllocCount = acount;
	rep.mAllocSize  = asize;
	rep.mKey        = key;
	rep.mSubKey     = keyfound;
  rep.mLineNo     = lineno;
}

//==================================================================================
void * MemAlloc::Realloc(void *mem,unsigned int size,const char *file,int lineno)
{
	void * ret = 0;

#if LOG
  fprintf(mLog,"-->Start of Realloc\r\n");
#endif

  if ( INTERNAL_DISABLE_TRACKING )
  {
    ret = sysRealloc(mem,size);
  }
  else
  {
    if ( mem )
    {

  	  FixedMemHeader *fmh = (FixedMemHeader *) mem;

  	  fmh--;

      const char *otag = "realloc_micro";
      int osize = fmh->mShortSize;

      if ( !fmh->hasMemHeaderFlag(MHF_MICRO_ALLOC) )
      {
      	MemHeader *mh       = (MemHeader *) mem;
      	mh--;
      	mh->Validate(1); // validate that it is all good
        MemFooter *mf = mh->getMemFooter();
  	    otag  = mf->mTag;
        osize = mh->mSize;
      }

      ret = Malloc(size,DEFAULT_ALIGNMENT,otag,file,lineno,MEMALLOC::MAT_MALLOC);

  	  unsigned int msize = osize;
  	  if ( size < msize )
  		  msize = size;

  	  memcpy(ret,mem,msize); // copy from the old into the new.

      Free(mem,MEMALLOC::MAT_MALLOC); // free the old one
    }
    else
    {
      ret = Malloc(size,DEFAULT_ALIGNMENT,"ReallocZero",file,lineno,MEMALLOC::MAT_MALLOC);
    }
  }

#if LOG
  fprintf(mLog,"%08X = Memalloc::Realloc(%08X,%d,%s,%d)\r\n", ret, mem, size, file, lineno );
  fflush(mLog);
#endif

	return ret;
}

//==================================================================================
void MemAlloc::WalkHeap(const char *label)
{
	Report(label,0,true);
}

//==================================================================================
void MemAlloc::HeapCheck(void)
{
  mHeapCheck++;
	MemHeader *mh = mUsed;
	while ( mh )
	{
		mh->Validate(mHeapCheck);
		mh = mh->getMemFooter()->mNext;
	}
  mHeapCheck--;
}

void   MemAlloc::generateReport(unsigned int count,KeyRep **keys,KeyReportType type,HTML_TABLE::HtmlTable *table,MemSort ms)
{
  mSortKey = type;

  for (unsigned int i=0; i<count; i++)
  {
    KeyRep &k = *keys[i];

    const char *key = k.mKey;
    const char *slash = findLastSlash(k.mKey);
    if ( slash )
    {
      key = slash;
    }

    switch ( ms )
    {
      case MS_NONE:
        break;
      case MS_BY_SOURCE_FILE:
        table->addColumn(key);
        table->addColumn(k.mLineNo);
        if ( 1 )
        {
          const char *subKey = k.mSubKey;
          subKey = lastSlash(subKey);
          table->addColumn(subKey);
        }
        table->addColumn(k.mAllocCount );
        table->addColumn(k.mAllocSize );
        if ( slash )
        {
          table->addColumn(k.mKey);
        }
        table->nextRow();
        break;
      case MS_BY_CLASS_NAME:
        table->addColumn(key);
        table->addColumn(k.mAllocCount );
        table->addColumn(k.mAllocSize );
        if ( slash )
        {
          table->addColumn(k.mKey);
        }
        table->nextRow();
        break;
    }
  }
}



//==================================================================================
void   MemAlloc::GenerateReport(unsigned int count,KeyRep **keys,KeyReportType type,const char *header,MemSort ms )
{
	ODF_LOG(1,"====================================================================================================================================================\r\n");
	ODF_LOG(1,"[ --- %s (%s)         ---]\r\n",header, formatNumber(count));
	ODF_LOG(1,"====================================================================================================================================================\r\n");
	ODF_LOG(1,"MESIZE          ALLOCS        Key\r\n");
	ODF_LOG(1,"====================================================================================================================================================\r\n");

	mSortKey = type;

	for (unsigned int i=0; i<count; i++)
	{
		KeyRep &k = *keys[i];

		const char *key   = k.mKey;
		const char *sub   = k.mSubKey;
		const char *count = formatNumber( k.mAllocCount );
		const char *size  = formatNumber( k.mAllocSize );
		ODF_LOG(1, "%10s : %8s : %-60s : %s \r\n", size, count, key, sub );

		if ( ms != MS_NONE )
		{
//	    gNxErrorReport.AddMemLog(ms,key,sub,k.mAllocCount, k.mAllocSize );
		}
	}

	ODF_LOG(1,"====================================================================================================================================================\r\n");
	ODF_LOG(1,"\r\n");
}

//==================================================================================
void GetHeapSize( int &used, int &unused )
{
	used = 0;
	unused = 0;

#ifdef _WIN32
  _HEAPINFO hInfo;
	hInfo._pentry = 0;
	while( _HEAPOK == _heapwalk( &hInfo ) )
	{
		if( _USEDENTRY == hInfo._useflag )
		{
			used += (unsigned int)hInfo._size;
		}
		else
		{
			unused += (unsigned int)hInfo._size;
		}
	}
#endif
}


void doinit(void)
{
  if ( gMemAlloc == 0 )
  {
    gMemorySendTextMessage = &gDSTM;
    if ( gMemAlloc == 0 )
    {
      gMemAlloc = new( ::malloc(sizeof(MemAlloc))) MemAlloc("GlobalMemAlloc");;
    }
  }
}

void * malloc(MEMALLOC::MemAlloc *memAlloc,size_t size,size_t alignment,const char *type,const char *file,int lineno,MEMALLOC::MemAllocType mtype)
{
  void *ret = 0;

  if ( memAlloc == 0 )
  {
    doinit();
    memAlloc = gMemAlloc;
  }

  ret = memAlloc->Malloc(size,alignment,type,file,lineno,mtype);

  return ret;
}

void   free(MEMALLOC::MemAlloc *memAlloc,void *mem,MEMALLOC::MemAllocType mtype)
{
  HE_ASSERT(memAlloc);
  if ( memAlloc )
    memAlloc->Free(mem,mtype);
}

void * realloc(MEMALLOC::MemAlloc *memAlloc,void *mem,size_t size,const char *file,int lineno)
{
  void *ret = 0;
  if ( memAlloc == 0 )
  {
    doinit();
    memAlloc = gMemAlloc;
  }
  ret = memAlloc->Realloc(mem,size,file,lineno);
  return ret;
}

void    setMemoryShutdown(MemoryShutdown *shutdown)
{
  gMemoryShutdown = shutdown;
}

void    walkHeap(MEMALLOC::MemAlloc *memAlloc,const char *label)
{
  if ( memAlloc == 0 )
  {
    doinit();
    memAlloc = gMemAlloc;
  }
  memAlloc->WalkHeap(label);
}

void    heapCheck(MEMALLOC::MemAlloc *memAlloc)
{
  if ( memAlloc == 0 )
  {
    doinit();
    memAlloc = gMemAlloc;
  }
  memAlloc->HeapCheck();
}

int     report(MEMALLOC::MemAlloc *memAlloc,const char *heading, SendTextMessage *stm,bool checkLeak)
{
  int ret = 0;

  if ( memAlloc == 0 )
  {
    doinit();
    memAlloc = gMemAlloc;
  }
  ret = memAlloc->Report(heading,stm,checkLeak);

  return ret;
}


void  setMaxFixedMemorySize(HeU32 fsize)
{
  if ( fsize > 128 ) fsize = 128;
  gMaxFixedMemorySize = fsize;
}


void                       getMemoryStatistics(HeU32 &mcount,            // number of non-fixed memory allocation.
                                               HeU32 &msize,             // size of non-fixed memory allocations
                                               HeU32 &fmcount,           // fixed memory allocation count.
                                               HeU32 &fmsize)           // fixed memory size
{
  mcount  = gMcount;
  msize   = gMsize;
  fmcount = gFmcount;
  fmsize  = gFmsize;

  gMcount = 0;
  gMsize = 0;
  gFmcount = 0;
  gFmsize = 0;
}


void    getMemStat(MEMALLOC::MemAlloc *memAlloc,int &mc,int &ms)
{
  mc = 0;
  ms = 0;

  if ( memAlloc == 0 )
  {
    doinit();
    memAlloc = gMemAlloc;
  }
  memAlloc->GetMemStat(mc,ms);

}

unsigned int     getMemoryUsed(MEMALLOC::MemAlloc *memAlloc,unsigned int &unused)
{
  unsigned int ret = 0;

  if ( memAlloc == 0 )
  {
    doinit();
    memAlloc = gMemAlloc;
  }
  ret = memAlloc->GetMemoryUsed(unused);

  return ret;
}


#if defined(WIN32)
unsigned int getHeapCount(HANDLE heap,unsigned int &hsize)
{
  unsigned int ret = 0;
  hsize = 0;

  HeapLock(heap);

  PROCESS_HEAP_ENTRY phe;
  phe.lpData = 0;

  while( HeapWalk(heap,&phe) )
  {
    hsize+=(phe.cbData+phe.cbOverhead);
    ret++;
  }

  HeapUnlock(heap);

  return ret;
}
#endif

void processReport(const char *header,HTML_TABLE::HtmlDocument *document)
{
#ifdef WIN32
  _PROCESS_MEMORY_COUNTERS info;
  info.cb = sizeof(info);
  BOOL ok = GetProcessMemoryInfo(GetCurrentProcess(),&info,sizeof(info));
  if ( ok )
  {
    char scratch[512];
    sprintf(scratch,"Process Memory Report for '%s'", header );
    HTML_TABLE::HtmlTable *table = document->createHtmlTable(scratch);
    table->addHeader("Process Data,Value");

    table->addColumn("PageFaultCount");
    table->addColumn((unsigned int)info.PageFaultCount);
    table->nextRow();


    table->addColumn("PeakWorkingSetSize");
    table->addColumn((unsigned int)info.PeakWorkingSetSize);
    table->nextRow();

    table->addColumn("WorkingSetSize");
    table->addColumn((unsigned int)info.WorkingSetSize);
    table->nextRow();

    table->addColumn("QuotaPeakPagedPoolUsage");
    table->addColumn((unsigned int)info.QuotaPeakPagedPoolUsage );
    table->nextRow();

    table->addColumn("QuotaPagedPoolUsage");
    table->addColumn((unsigned int)info.QuotaPagedPoolUsage );
    table->nextRow();

    table->addColumn("QuotaPeakNonPagedPoolUsage");
    table->addColumn((unsigned int)info.QuotaPeakNonPagedPoolUsage );
    table->nextRow();

    table->addColumn("QuotaNonPagedPoolUsage");
    table->addColumn((unsigned int)info.QuotaNonPagedPoolUsage );
    table->nextRow();

    table->addColumn("PagefileUsage");
    table->addColumn((unsigned int)info.PagefileUsage );
    table->nextRow();

    table->addColumn("PeakPagefileUsage");
    table->addColumn((unsigned int)info.PeakPagefileUsage );
    table->nextRow();

  }
#endif
}


void  setComponentName(const char *name)
{
  gComponentName = name;
}

void                       reportByClass(const char *header,HTML_TABLE::HtmlDocument *document)
{
  gMemAlloc->reportByClass(header,document);
}

void                       reportBySourceFile(const char *header,HTML_TABLE::HtmlDocument *document)
{
  gMemAlloc->reportBySourceFile(header,document);
}

void fixedPoolReport(const char *header,HTML_TABLE::HtmlDocument *document)
{
  if ( gMemAlloc )
  {
    gMemAlloc->fixedPoolReport(header,document);
  }
}

void          setName(MemAlloc *m,const char *name)
{
  HE_ASSERT(m);
  if ( m )
  {
    m->setName(name);
  }
}

void heapCompact(MEMALLOC::MemAlloc *memAlloc,SendTextMessage *stm)
{
  memAlloc->heapCompact(stm);
}

void    setSendTextMessage(MEMALLOC::MemAlloc *memAlloc,SendTextMessage *stm)
{
  if ( memAlloc == 0 )
  {
    doinit();
    memAlloc = gMemAlloc;
  }
  memAlloc->setSendTextMessage(stm);
}


void * mallocInternal(MemAlloc *memAlloc,size_t sz)
{
  return memAlloc->Malloc(sz,0,"@internal",__FILE__,__LINE__,MEMALLOC::MAT_INTERNAL);
}

void   freeInternal(MemAlloc *memAlloc,void *p)
{
  memAlloc->Free(p,MEMALLOC::MAT_INTERNAL);
}


void    frameBegin(MEMALLOC::MemAlloc *memAlloc,SendTextMessage *tmessage)
{
  if ( memAlloc )
    memAlloc->frameBegin(tmessage);
}

void    frameEnd(MEMALLOC::MemAlloc *memAlloc,SendTextMessage *tmessage,const char *header)
{
  if ( memAlloc )
    memAlloc->frameEnd(tmessage,header);
}

#if defined(WIN32)
struct hinfo
{
  PVOID lpData;
  DWORD cbData;
  BYTE  cbOverhead;
  WORD  wFlags;
  BYTE  iRegionIndex;
};
#endif

unsigned int  getProcessMemory(unsigned int &workingset) // reports the amount of memory used by the current process
{
  unsigned int ret = 0;
  workingset = 0;
  _PROCESS_MEMORY_COUNTERS info;
  info.cb = sizeof(info);
#if defined(WIN32)
  BOOL ok = GetProcessMemoryInfo(GetCurrentProcess(),&info,sizeof(info));
#else
  bool ok = true;
#endif 
  if ( ok )
  {
    workingset = info.WorkingSetSize;
    ret = info.PagefileUsage;
  }
  return ret;
}

void coreDump(MEMALLOC::MemAlloc *memAlloc,SendTextMessage *stm,const char *header,HANDLE heap,bool csv)
{

#if 0

  PROCESS_HEAP_ENTRY phe;
  phe.lpData = 0;
  phe.wFlags = PROCESS_HEAP_REGION;

  unsigned int count = 0;
  unsigned int tsize = 0;
  unsigned int toverhead = 0;

  unsigned int hsize;
  unsigned int hcount = getHeapCount(heap,hsize); // report the total number of items in the heap.

  struct hinfo *temp = (struct hinfo *)sysMalloc(hcount*sizeof(hinfo));

// Because of multi-threading there is an extremely small possibility the heap could have changed since we computed the total previously.  So we will take that rare case into account..
  HeapLock(heap);

  struct hinfo *dest = temp;

	while( HeapWalk(heap,&phe) )
	{
    if ( phe.lpData != temp ) // don't include the memory we just allocated to generate this report!
    {

      dest->lpData     = phe.lpData;
      dest->cbData     = phe.cbData;
      dest->cbOverhead = phe.cbOverhead;
      dest->wFlags     = phe.wFlags;
      dest->iRegionIndex = phe.iRegionIndex;

      count++;
      tsize+=phe.cbData;
      toverhead+=phe.cbOverhead;
      dest++;
      if ( count == hcount )
        break;
    }
	}

  HeapUnlock(heap);


  stm->sendTextMessage(0,"Heap(%s) Contains %s entries for a total data size of %s bytes and an overhead of %s bytes.\r\n",header, formatNumber(count), formatNumber(tsize), formatNumber(toverhead) );

  const struct hinfo *scan = temp;

  for (unsigned int i=0; i<count; i++)
  {
    const hinfo &phe = *scan;

    char flags[512];
    flags[0] = 0;

    if ( phe.wFlags & PROCESS_HEAP_ENTRY_BUSY )        strcat(flags,"BUSY ");
    if ( phe.wFlags & PROCESS_HEAP_ENTRY_DDESHARE )    strcat(flags,"DDESHARE ");
    if ( phe.wFlags & PROCESS_HEAP_ENTRY_MOVEABLE )    strcat(flags,"MOVEABLE ");
    if ( phe.wFlags & PROCESS_HEAP_REGION )            strcat(flags,"REGION ");
    if ( phe.wFlags & PROCESS_HEAP_UNCOMMITTED_RANGE ) strcat(flags,"UNCOMMITTED_RANGE");

    stm->sendTextMessage(1,"HEAP(%08X) : REGION:%3d : DSIZE: %15s OVERHEAD: %3d FLAGS(%s)\r\n", phe.cbData, phe.iRegionIndex, formatNumber( phe.cbData ), (int) phe.cbOverhead, flags );

    scan++;

	}

  sysFree(temp);
#endif
  if ( memAlloc )
    memAlloc->internalCoreDump(stm,header,csv);

}

void coreDump(MEMALLOC::MemAlloc *memAlloc,SendTextMessage *stm,const char *header,bool csv)
{


#ifdef _WIN32
  _HEAPINFO hInfo;
	hInfo._pentry = 0;
  size_t used  =0;
  size_t unused=0;
  size_t ucount = 0;
  size_t uucount = 0;

	while( _HEAPOK == _heapwalk( &hInfo ) )
	{
		if( _USEDENTRY == hInfo._useflag )
		{
      ucount++;
			used += (unsigned int)hInfo._size;
//      stm->sendTextMessage(1,"USED  :%08X,%15s\r\n", hInfo._pentry, formatNumber(hInfo._size) );
		}
		else
		{
      uucount++;
			unused += (unsigned int)hInfo._size;
//      stm->sendTextMessage(1,"UNUSED:%08X,%15s\r\n", hInfo._pentry, formatNumber(hInfo._size) );
		}
	}
  stm->sendTextMessage(1,"Process Heap contains: %s used blocks totalling %s bytes of memory and %s unused blocks for %s bytes of memory.\r\n",
    formatNumber(ucount),
    formatNumber(used),
    formatNumber(uucount),
    formatNumber(unused) );
#endif

  memAlloc->internalCoreDump(stm,header,csv);
}

void processCoreDump(MEMALLOC::MemAlloc *memAlloc,SendTextMessage *stm,const char *header,bool csv)
{
//** TODO TODO

/********************
  unsigned int bsize,asize;
  unsigned int bcount = getHeapCount(GetProcessHeap(),bsize);
  HeapCompact( gHeap, 0 );
  unsigned int acount = getHeapCount(GetProcessHeap(),asize);
  if ( stm )
  {
    stm->sendTextMessage(0,"PROCESS HEAP BEFORE: %s bytes AFTER COMPACTION: %s bytes.  Count before: %s Count After: %s\r\n", formatNumber(bsize), formatNumber(asize), formatNumber(bcount), formatNumber(acount) );
  }
  coreDump(memAlloc,stm,header, GetProcessHeap(), csv );
****************/
}


HTML_TABLE::HtmlTable * summaryHeader(HTML_TABLE::HtmlDocument *document) // create a new table for the summary data.
{
  HTML_TABLE::HtmlTable *ret = 0;

  if ( document )
  {
    ret = document->createHtmlTable("Summary Memory Report");
    ret->computeTotals();
    ret->addHeader("%s","Executable/DLL,Total/Memory,Allocation/Count,General/Memory,General/Count,Micro/Alloc,Micro/Count,Total/Micro,GlobalNew/Memory,GlobalNew/Count,GlobalNewArray/Memory,GlobalNewArray/Count");
    ret->setHeaderColor(0x00FFFF);
    ret->setFooterColor(0xCCFFFF);
    ret->setColumnColor(1,0xCCFFCC);
    ret->setColumnColor(2,0xFFFF99);
    ret->setColumnColor(3,0xFFFF99);
    ret->setColumnColor(4,0xFFCC99);
    ret->setColumnColor(5,0xFFCC99);
    ret->setColumnColor(6,0xFFCC99);
    ret->setColumnColor(7,0xFFCC99);
    ret->setColumnColor(8,0xFFCC99);
    ret->setColumnColor(9,0xFF99CC);
    ret->setColumnColor(10,0xFF99CC);
    ret->setColumnColor(11,0xFF99CC);
    ret->setColumnColor(12,0xFF99CC);
    ret->addSort("Sorted by Total Memory Size",2,false,0,false);
  }

  return ret;
}


void summaryReport(HTML_TABLE::HtmlTable *table,MEMALLOC::MemAlloc *memAlloc,const char *header) // add a new row to the summary table
{
  if ( memAlloc == 0 )
  {
    doinit();
    memAlloc = gMemAlloc;
  }
  memAlloc->summaryReport(table,header);
}


};

#endif


void * mallocAlign(size_t size,size_t alignment)
{
  void *ret = ::malloc(size);
  return ret;
}


