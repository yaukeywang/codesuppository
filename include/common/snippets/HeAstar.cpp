#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "HeAstar.h"
#include "pool.h"
#include "HashFunction.h"
#include "HashSet.h"

#define DEBUG_SHOW 0
#define VALIDATION 0

#if DEBUG_SHOW
#include "RenderDebug/RenderDebug.h"
#include "SendTextMessage.h"
#endif

namespace HE_ASTAR
{

#pragma warning(disable:4505)

class AstarNode;

static AstarNode * getAstarNode(void);
static void        releaseAstarNode(AstarNode *a);

class AstarNode
{
public:
  AstarNode(void)
  {
    mNode = 0;
    mNext = 0;
    mPrevious = 0;
    mParent = 0;
    mChild  = 0;
    mNextNode = 0;
    mPreviousNode = 0;
    mH = 0;
    mF = 0;
    mG = 0;
  }

  AstarNode(AI_Node *n)
  {
    mNode = n;
    mParent = 0;
    mChild  = 0;
    mNextNode = 0;
    mPreviousNode = 0;

    mH = mF = mG = 0;
  }

  AI_Node   * getAINode(void) const { return mNode; };

  AstarNode  *getParent(void) const { return mParent; };
  void        setParent(AstarNode *p) { mParent = p; };

  AstarNode  *getChild(void) const { return mChild; };
  void        setChild(AstarNode *c) { mChild = c; };

  AstarNode  *getNextNode(void) const { return mNextNode; };
  void        setNextNode(AstarNode *a) { mNextNode = a; };
  AstarNode  *getPreviousNode(void) const { return mPreviousNode; };
  void        setPreviousNode(AstarNode *a) { mPreviousNode = a; };

  NxF32       getF(void) const { return mF; };
  NxF32       getG(void) const { return mG; };
  NxF32       getH(void) const { return mH; };

  void        setF(NxF32 f) { mF = f; };
  void        setG(NxF32 g) { mG = g; };
  void        setH(NxF32 h) { mH = h; };

  // used by the pool allocator
  AstarNode * GetNext(void) const { return mNext; };
  AstarNode * GetPrevious(void) const { return mPrevious; };
  void        SetNext(AstarNode *a) { mNext = a; };
  void        SetPrevious(AstarNode *a) { mPrevious = a; };

  NxU32 getHashValue(void) const
  {
    NxU32 ret = (NxU32) mNode;
    return ret;
  }

private:
  NxF32      mH;
  NxF32      mG;
  NxF32      mF;

  AI_Node   *mNode;
  AstarNode *mParent;
  AstarNode *mChild;

  AstarNode *mNextNode;             // used for the 'open list' and the 'closed list'
  AstarNode *mPreviousNode;

// used by the pool allocator, leave these pointers alone
  AstarNode *mNext;
  AstarNode *mPrevious;
};

class HeAstarAllocator
{
public:
  HeAstarAllocator(void)
  {
//    mNodes.Set(2048,8192,16384,"AstarNode");
    mNodes.Set(16384,16384,16384*32,"AstarNode",__FILE__,__LINE__);
  }

  AstarNode * getAstarNode(void)
  {
    return mNodes.GetFreeLink();
  }

  void        releaseAstarNode(AstarNode *a)
  {
    mNodes.Release(a);
  }

private:

  Pool < AstarNode > mNodes;
};

static HeAstarAllocator *gAllocator=0;


class MyHeAstar : public HeAstar
{
public:
  MyHeAstar(void)
  {
    mFrom = 0;
    mTo   = 0;
    mUserData = 0;
    mOpenHead = 0;
    mOpenTail = 0;
    mClosedHead = 0;
    mClosedTail = 0;
    mOpenCount = 0;
    mClosedCount = 0;
    mMaxSearch   = 10000;
    mSolutionNode = 0;
    mSolutionCount = 0;
    mSolutionSet = 0;
    mDirectSolution = false;
  }

  ~MyHeAstar(void)
  {
    release();
  }

  void release(void)
  {
    AstarNode *n = mOpenHead;
    while ( n )
    {
      AstarNode *next = n->getNextNode();
      releaseAstarNode(n);
      n = next;
    }
    n = mClosedHead;
    while ( n )
    {
      AstarNode *next = n->getNextNode();
      releaseAstarNode(n);
      n = next;
    }
    mUserData = 0;
    mOpenHead = 0;
    mOpenTail = 0;
    mClosedHead = 0;
    mClosedTail = 0;
    mOpenCount = 0;
    mClosedCount = 0;
    mClosedHash.clear();
    mSolutionNode = 0;
    mSolutionCount = 0;
    mOpenHash.clear();
    mDirectSolution = false;
    MEMALLOC_FREE(mSolutionSet);
  }

  virtual void astarStartSearch(AI_Node *from,AI_Node *to,void *userData)  // start a search.
  {

    release();

    mFrom           = from;
    mTo             = to;
    mUserData       = userData;
    AstarNode *node = getAstarNode();
    new ( node ) AstarNode(mFrom);
    if ( node )
    {
      node->setG(0);
      node->setH( from->getDistance(to,userData) );
      node->setF( node->getH() );
      mOpenCount      = 1;
      addOpenHash(node);
    }

    mOpenHead       = node;
    mOpenTail       = node;
  }

  virtual AI_Node         * astarGetFrom(void)
  {
    return mFrom;
  }

  virtual AI_Node         * astarGetTo(void)
  {
    return mTo;
  }

  // insert it into the open-list based on its priority...
  void insert(AstarNode *successor)
  {
    NxF32 f = successor->getF();

    AstarNode *previous = 0;
    AstarNode *scan = mOpenHead;

    while ( scan && f > scan->getF() )
    {
      previous = scan;
      scan = scan->getNextNode();
#if VALIDATION
      if ( scan )
        assert( scan->getPreviousNode() == previous );
#endif
    }

    successor->setNextNode(scan);
    successor->setPreviousNode(previous);

    if ( previous )
    {
      previous->setNextNode(successor);
    }
    else
    {
      mOpenHead = successor; // it's the new head
    }

    if ( scan )
    {
      scan->setPreviousNode(successor);
    }
    else
    {
#if VALIDATION
      assert( previous == mOpenTail );
#endif
      mOpenTail = successor; // it's the new tail
    }
    addOpenHash(successor);
    mOpenCount++;
  }


  void addClosedNode(AstarNode *node)
  {
    AstarNode *otail = mClosedTail;

    if ( otail )
    {
      otail->setNextNode(node);
      node->setNextNode(0);
      node->setPreviousNode(otail);
      mClosedTail = node;
    }
    else
    {
      mClosedHead = node;
      mClosedTail = node;
      node->setNextNode(0);
      node->setPreviousNode(0);
    }
    addClosedHash(node);
    mClosedCount++;
  }

  virtual bool astarSearchStep(NxU32 &searchCount) // step the A star algorithm one time.  Return true if the search is completed.
  {
    bool ret = false;

    searchCount = 0;

    // Remove the node from the open list and move it to the closed list
    if ( mOpenHead )
    {
      AstarNode *n = mOpenHead;

      releaseOpenNode(n);       // remove from the open node list
      addClosedNode(n);         // add to the end of the closed node list

      if ( n->getAINode() == mTo )
      {
        ret = true;

        mSolutionCount = 1;
        AstarNode *p = n->getParent();
        while ( p )
        {
          mSolutionCount++;
          p->setChild(n);
          n = p;
          p = n->getParent();
        }

        mSolutionNode = n;

      }
      else
      {
        NxU32 ecount = n->getAINode()->getEdgeCount(mUserData);

        for (NxU32 i=0; i<ecount; i++)
        {

          AI_Node *next = n->getAINode()->getEdge(i,mUserData);

          if ( next )
          {

            if ( (mClosedCount+mOpenCount) >= mMaxSearch )
            {
              break;
            }

            NxF32 newg = n->getG() + n->getAINode()->getCost(next,mUserData); // need to take the 'successor' into account!

            AstarNode *open = findOpenList(next);
            if ( open && open->getG() <= newg )
            {
              continue;
            }

            AstarNode *closed = findClosedList(next);
            if ( closed && closed->getG() <= newg )
            {
              continue;
            }

            AstarNode *successor = getAstarNode();

            if ( successor )
            {

              searchCount++;

              new ( successor ) AstarNode(next);

              successor->setParent(n);
              successor->setG(newg);
              successor->setH( successor->getAINode()->getDistance(mTo,mUserData) );
              successor->setF( successor->getG() + successor->getH() );

              if ( open )
              {
                releaseOpenNode(open); // patch the linked list
                releaseAstarNode(open); // release it back to the global astar memory pool
              }

              if ( closed )
              {
                releaseClosedNode(closed); // patch the linked list
                releaseAstarNode(closed);  // release it back to the global astar memory pool
              }


              insert( successor );


            }
          }
        }
      }
    }
    else
    {
#if DEBUG_SHOW
      SEND_TEXT_MESSAGE(0,"OpenList empty without finding the solution!.\r\n");
#endif
      ret = true; // finished, but found no solution...
    }

#if DEBUG_SHOW

    if ( gBaseRenderDebug )
    {
      gBaseRenderDebug->Reset();
      NxF32 s = gBaseRenderDebug->getRenderScale();
      gBaseRenderDebug->setRenderScale(0.1f);
      AstarNode *scan = mOpenHead;
      AstarNode *prev = 0;
      NxU32 ocount = 0;
      NxU32 ccount = 0;

      while ( scan )
      {
        ocount++;
        assert( scan->getPreviousNode() == prev );
        scan->getAINode()->debugRender(0xFF0000,600);
        prev = scan;
        scan = scan->getNextNode();
      }
      assert( prev == mOpenTail );

      prev = 0;
      scan = mClosedHead;
      while ( scan )
      {
        assert( scan->getPreviousNode() ==  prev );
        ccount++;
        scan->getAINode()->debugRender(0x00FF00,600);
        prev = scan;
        scan = scan->getNextNode();
      }
      assert( prev == mClosedTail );
      assert( ocount == mOpenCount );
      assert( ccount == mClosedCount );
      gBaseRenderDebug->setRenderScale(s);
      SEND_TEXT_MESSAGE(0,"Open: %d Closed: %d\r\n", mOpenCount, mClosedCount );
    }

#endif


    return ret;
  }

  AstarNode * findOpenList(AI_Node *n)
  {
#if VALIDATION
    AstarNode *ret = 0;

    AstarNode *scan = mOpenHead;
    while ( scan )
    {
      if ( scan->getAINode() == n )
      {
        ret = scan;
        break;
      }
      scan = scan->getNextNode();
    }

    NxU32 index = (NxU32)n;
    AstarNode *match=0;
    mOpenHash.exists(index,match);

    assert ( match == ret );

    return ret;
#else
    NxU32 index = (NxU32)n;
    AstarNode *match=0;
    mOpenHash.exists(index,match);
    return match;
#endif
  }

  AstarNode * findClosedList(AI_Node *n)
  {
#if VALIDATION
    AstarNode *ret = 0;

    AstarNode *scan = mClosedHead;
    while ( scan )
    {
      if ( scan->getAINode() == n )
      {
        ret = scan;
        break;
      }
      scan = scan->getNextNode();
    }

    NxU32 index = (NxU32)n;
    AstarNode *match = 0;
    mClosedHash.exists(index,match);
    assert( match == ret );

    return ret;
#else

    NxU32 index = (NxU32)n;
    AstarNode *match = 0;
    mClosedHash.exists(index,match);
    return match;

#endif
  }

  void releaseOpenNode(AstarNode *open)
  {

    removeOpenHash(open);

		AstarNode *prev = open->getPreviousNode();

    if ( open == mOpenTail )
    {
      mOpenTail = prev;
    }

		//  first patch old linked list.. his previous now points to his next

		if ( prev )
		{
			AstarNode *next = open->getNextNode();
			prev->setNextNode( next ); //  my previous now points to my next
			if ( next ) next->setPreviousNode(prev);
			//  list is patched!
		}
		else
		{
			AstarNode *next = open->getNextNode();
			mOpenHead = next;
			if ( mOpenHead )
			  mOpenHead->setPreviousNode(0);
		}

    open->setNextNode(0);
    open->setPreviousNode(0);

		mOpenCount--;
  }

  void releaseClosedNode(AstarNode *closed)
  {
    removeClosedHash(closed);

		AstarNode *prev = closed->getPreviousNode();

    if ( closed == mClosedTail )
    {
      mClosedTail = prev;
    }

		//  first patch old linked list.. his previous now points to his next

		if ( prev )
		{
			AstarNode *next = closed->getNextNode();
			prev->setNextNode( next ); //  my previous now points to my next
			if ( next ) next->setPreviousNode(prev);
			//  list is patched!
		}
		else
		{
			AstarNode *next = closed->getNextNode();
			mClosedHead = next;
			if ( mClosedHead )
			  mClosedHead->setPreviousNode(0);
		}

		mClosedCount--;
  }

  virtual AI_Node **        getSolution(NxU32 &count)  // retrieve the solution.  If this returns a null pointer and count of zero, it means no solution could be found.
  {
    AI_Node **ret = 0;

    count = 0;

    if ( mDirectSolution )
    {
      count = 2;
      ret = mDirectSolutionSet;
    }
    else
    {

      if ( mSolutionSet )
      {
        MEMALLOC_FREE(mSolutionSet);
        mSolutionSet = 0;
      }

      if ( mSolutionNode )
      {
        mSolutionSet = (AI_Node **)MEMALLOC_MALLOC( sizeof(AI_Node **)*mSolutionCount );
        AstarNode *n = mSolutionNode;
        while ( n )
        {
          mSolutionSet[count] = n->getAINode();
          count++;
          if ( count == mSolutionCount )
            break;
          n = n->getChild();
        }
        ret = mSolutionSet;
      }
    }

    return ret;
  }

  void addClosedHash(AstarNode *i)
  {
#if VALIDATION
    AstarNode *match = 0;
    assert( !mClosedHash.exists(i->getHashValue(),match) );
#endif
    mClosedHash.insert(i);
#if VALIDATION
    assert( mClosedHash.exists(i->getHashValue(),match) );
#endif
  }

  void addOpenHash(AstarNode *i)
  {
#if VALIDATION
    AstarNode *match = 0;
    assert( !mOpenHash.exists(i->getHashValue(),match) );
#endif
    mOpenHash.insert(i);
#if VALIDATION
    assert( mOpenHash.exists(i->getHashValue(),match) );
#endif
  }

  bool removeClosedHash(AstarNode *i)
  {
#if VALIDATION
    AstarNode *match = 0;
    assert( mClosedHash.exists(i->getHashValue(),match) );
#endif
    bool ret = mClosedHash.remove(i);
#if VALIDATION
    assert(ret);
    assert( !mClosedHash.exists(i->getHashValue(),match) );
#endif
    return ret;
  }

  bool removeOpenHash(AstarNode *i)
  {
#if VALIDATION
    AstarNode *match = 0;
    assert( mOpenHash.exists(i->getHashValue(),match) );
#endif
    bool ret = mOpenHash.remove(i);

#if VALIDATION
    assert(ret);
    assert( !mOpenHash.exists(i->getHashValue(),match) );
#endif

    return ret;
  }

  void              setDirectSolution(void)
  {
    release();
    mDirectSolution = true;
    mSolutionCount  = 2;
    mDirectSolutionSet[0] = mFrom;
    mDirectSolutionSet[1] = mTo;
  }


private:
  AI_Node   *mFrom;
  AI_Node   *mTo;
  void      *mUserData;
  NxU32      mMaxSearch;
  NxU32      mOpenCount;
  NxU32      mClosedCount;

  AstarNode *mSolutionNode;
  NxU32      mSolutionCount;

  AstarNode *mOpenHead;
  AstarNode *mOpenTail;

  AstarNode *mClosedHead;
  AstarNode *mClosedTail;

  HashSet< AstarNode * > mClosedHash;
  HashSet< AstarNode * > mOpenHash;

  bool                    mDirectSolution;
  AI_Node               **mSolutionSet;
  AI_Node                *mDirectSolutionSet[2];

};

static void init(void)
{
  if ( gAllocator == 0 )
  {
    gAllocator = MEMALLOC_NEW(HeAstarAllocator);
  }
}

static AstarNode * getAstarNode(void)
{
  init();
  return gAllocator->getAstarNode();
}

static void        releaseAstarNode(AstarNode *a)
{
  assert( gAllocator );
  assert( a );
  if ( a && gAllocator )
  {
    gAllocator->releaseAstarNode(a);
  }
}

};

using namespace HE_ASTAR;


HeAstar         * createHeAstar(void)    // Create an instance of the HeAstar utility.
{
  MyHeAstar *mha = new MyHeAstar;
  return static_cast< HeAstar *>(mha);
}

void              releaseHeAstar(HeAstar *astar)  // Release the intance of the HeAstar utility.
{
  MyHeAstar *mha = static_cast< MyHeAstar * >(astar);
  delete mha;
}


