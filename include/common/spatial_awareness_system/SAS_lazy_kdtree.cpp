#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#pragma warning(disable:4510)
#pragma warning(disable:4610)

#include "../snippets/UserMemAlloc.h"
#include "../snippets/simplevector.h"

#if defined(LINUX)
#include "linux_compat.h"
#include "../snippets/UserMemAlloc.h"
#endif
#include <vector>

#include "SAS_lazy_kdtree.h"

#pragma warning(disable:4996)
#pragma warning(disable:4800)
#pragma warning(disable:4100)


#include "AwareManager.h"

namespace SPATIAL_AWARENESS_SYSTEM
{

enum Axes
{
  X_AXIS = 0,
  Y_AXIS = 1,
  Z_AXIS = 2,
};

enum NodeFlag
{
  NF_HAVE_POSITION    = (1<<0),
  NF_HAVE_RANGE       = (1<<1),
  NF_DELETED          = (1<<2),
  NF_HAVE_ID          = (1<<3),
  NF_NEW_POSITION     = (1<<4),
  NF_FREE_LIST        = (1<<5),
  NF_HAVE_GRANULARITY = (1<<6),
};

class KdTreeNode;

static bool awareTime(SecondsType dtime,SecondsType ctime,SecondsType latency)
{
  bool ret = false;

  SecondsType diff = ctime - dtime;
  if ( diff > latency )
    ret = true;

  return ret;
}

class AwareNode
{
public:
  AwareNode(void)
  {
    mAwareNext = 0;
    mNode = 0;
  }
  AwareNode  *mAwareNext;
  KdTreeNode *mNode;
  SecondsType mTime;       // the time we became aware
};

#define AWARE_BLOCK_SIZE 8192

class AwareBlock
{
public:
  AwareBlock(void)
  {
    mCount = 0;
    mNextBlock = 0;
  }

  AwareNode * getAwareNode(void)
  {
    AwareNode *ret = 0;
    if ( mCount < AWARE_BLOCK_SIZE )
    {
      ret = &mNodes[mCount];
      mCount++;
    }
    return ret;
  }

  HeU32 mCount;
  AwareBlock  *mNextBlock;
  AwareNode    mNodes[AWARE_BLOCK_SIZE];
};

class AwarePool
{
public:
  AwarePool(void)
  {
    mFree = 0;
    mBlock = 0;
    mCurrent = 0;
  }

  ~AwarePool(void)
  {
    AwareBlock *block = mBlock;
    while ( block )
    {
      AwareBlock *next = block->mNextBlock;
      delete block;
      block = next;
    }
  }

  AwareNode * GetFreeLink(SecondsType time)
  {
    AwareNode *ret = 0;

    if ( mFree )
    {
      ret = mFree;
      mFree = ret->mAwareNext;
      ret->mAwareNext = 0;
    }
    else
    {
      if ( mBlock == 0 )
      {
        mCurrent = mBlock = MEMALLOC_NEW(AwareBlock);
      }
      ret = mCurrent->getAwareNode();
      if ( ret == 0 )
      {
        AwareBlock *block = MEMALLOC_NEW(AwareBlock);
        mCurrent->mNextBlock = block;
        mCurrent = block;
        ret = mCurrent->getAwareNode();
      }
    }

    if( ret )
    {
      ret->mTime = time;
    }

    return ret;
  }

  void Release(AwareNode *node)
  {
    node->mAwareNext = mFree;
    node->mNode = 0;
    mFree = node;
  }


  AwareNode *mFree;
  AwareBlock *mCurrent;
  AwareBlock *mBlock;
};

class KdTreeInterface
{
public:
  virtual void foundNode(KdTreeNode *node,HeF32 distanceSquared) = 0;
};

class KdTreeNode
{
public:
  KdTreeNode(void)
  {
    mNodePos[0] = mNodePos[1] = mNodePos[2] = 0;
    mPos[0] = mPos[1] = mPos[2];
    mRadius = 0;
    mLeft = 0;
    mRight = 0;
    mFlags = 0;
    mID    = 0;
    mAware = 0;
    mInverseAware = 0;
    mFrameNo = 0;
    mCreateFrameNo = 0;
    mIndex   = 0xFFFF;
    mGranularity = 0;
  }

  KdTreeNode(ID id,HeU16 index)
  {
    mID = id;
    mNodePos[0] = mNodePos[1] = mNodePos[2] = 0;
    mRadius = 0;
    mLeft = 0;
    mRight = 0;
    mFlags = NF_HAVE_ID;
    mIndex = index;
    mGranularity = 0;
  }

  void setCreateFrameNo(HeU32 fno)
  {
    mCreateFrameNo = fno;
  }

  const HeF32 * getPosition(void) const { return mPos; };

  inline bool setPosition(const HeF32 *pos)
  {
    bool ret = false;

    mPos[0] = pos[0];
    mPos[1] = pos[1];
    mPos[2] = pos[2];

    if ( hasNodeFlag(NF_HAVE_POSITION ) )
    {
      setNodeFlag(NF_NEW_POSITION);
    }
    else
    {
      mNodePos[0] = pos[0];
      mNodePos[1] = pos[1];
      mNodePos[2] = pos[2];
      setNodeFlag(NF_HAVE_POSITION);
      ret = true;
    }
    return ret;
  }

  inline void setRadius(HeF32 radius)
  {
    mRadius = radius;
    if ( radius > 0 )
      setNodeFlag(NF_HAVE_RANGE);
    else
      clearNodeFlag(NF_HAVE_RANGE);
  };

	~KdTreeNode(void)
  {
  }

  inline void setNodeFlag(NodeFlag flag)
  {
    mFlags|=flag;
  }

  inline void clearNodeFlag(NodeFlag flag)
  {
    mFlags&=~flag;
  }

  inline bool hasNodeFlag(NodeFlag flag) const
  {
    return (bool)(flag&mFlags);
  }

  void add(KdTreeNode *node,Axes dim)
  {
    assert( node->mID != 0xFFFFFFFF );

    switch ( dim )
    {
      case X_AXIS:
        if ( node->getX() <= getX() )
        {
          if ( mLeft )
            mLeft->add(node,Y_AXIS);
          else
            mLeft = node;
        }
        else
        {
          if ( mRight )
            mRight->add(node,Y_AXIS);
          else
            mRight = node;
        }
        break;
      case Y_AXIS:
        if ( node->getY() <= getY() )
        {
          if ( mLeft )
            mLeft->add(node,Z_AXIS);
          else
            mLeft = node;
        }
        else
        {
          if ( mRight )
            mRight->add(node,Z_AXIS);
          else
            mRight = node;
        }
        break;
      case Z_AXIS:
        if ( node->getZ() <= getZ() )
        {
          if ( mLeft )
            mLeft->add(node,X_AXIS);
          else
            mLeft = node;
        }
        else
        {
          if ( mRight )
            mRight->add(node,X_AXIS);
          else
            mRight = node;
        }
        break;
    }

  }

  inline HeF32        getX(void) const { return mNodePos[0];  }
  inline HeF32        getY(void) const {  return mNodePos[1]; };
  inline HeF32        getZ(void) const { return mNodePos[2]; };
  inline HeF32        getRadius(void) const { return mRadius; };

  void search(Axes axis,const HeF32 *pos,HeF32 radius,HeU32 &count,KdTreeInterface *callback)
  {

    HeF32 dx = pos[0] - getX();
    HeF32 dy = pos[1] - getY();
    HeF32 dz = pos[2] - getZ();

    KdTreeNode *search1 = 0;
    KdTreeNode *search2 = 0;

    switch ( axis )
    {
      case X_AXIS:
       if ( dx <= 0 )     // JWR  if we are to the left
       {
        search1 = mLeft; // JWR  then search to the left
        if ( -dx < radius )  // JWR  if distance to the right is less than our search radius, continue on the right as well.
          search2 = mRight;
       }
       else
       {
         search1 = mRight; // JWR  ok, we go down the left tree
         if ( dx < radius ) // JWR  if the distance from the right is less than our search radius
	  			search2 = mLeft;
        }
        axis = Y_AXIS;
        break;
      case Y_AXIS:
        if ( dy <= 0 )
        {
          search1 = mLeft;
          if ( -dy < radius )
    				search2 = mRight;
        }
        else
        {
          search1 = mRight;
          if ( dy < radius )
    				search2 = mLeft;
        }
        axis = Z_AXIS;
        break;
      case Z_AXIS:
        if ( dz <= 0 )
        {
          search1 = mLeft;
          if ( -dz < radius )
    				search2 = mRight;
        }
        else
        {
          search1 = mRight;
          if ( dz < radius )
    				search2 = mLeft;
        }
        axis = X_AXIS;
        break;
    }

    HeF32 r2 = radius*radius;

    dx = pos[0] - mPos[0]; // use the real current position!
    dy = pos[1] - mPos[1];
    dz = pos[2] - mPos[2];

    HeF32 m  = dx*dx+dy*dy+dz*dz;

    if ( m < r2 )
    {
      callback->foundNode(this,m);
      count++;
    }


    if ( search1 )
  		search1->search( axis, pos,radius, count, callback);

    if ( search2 )
	  	search2->search( axis, pos,radius, count, callback);

  }

  inline HeF32 distanceSquared(const HeF32 *pos) const
  {
    HeF32 dx = pos[0] - mNodePos[0];
    HeF32 dy = pos[1] - mNodePos[1];
    HeF32 dz = pos[2] - mNodePos[2];
    return dx*dx+dy*dy+dz*dz;
  }


  inline void setLeft(KdTreeNode *left) { mLeft = left; };
  inline void setRight(KdTreeNode *right) { mRight = right; };

	inline KdTreeNode *getLeft(void)         { return mLeft; }
	inline KdTreeNode *getRight(void)        { return mRight; }



  // patch it into the doubly linked list.
  inline void addAware(AwareNode *aware,KdTreeNode *node,HeU32 /*frameNo*/)
  {
    aware->mNode    = node;
    aware->mAwareNext     = mAware;
    mAware = aware;
  }

  inline void addInverseAware(AwareNode *aware,KdTreeNode *node,HeU32 /*frameNo*/)
  {
    aware->mNode    = node;
    aware->mAwareNext     = mInverseAware;
    mInverseAware = aware;
  }

  inline HeU32 flushDeleted(AwareMessageInterface *observer,AwarePool &pool,AwareManager &awareManager)
  {
    HeU32 ret = 0;
    AwareNode *n = mAware;
    AwareNode *pn = 0;

    while ( n )
    {
      if (  n->mNode->hasNodeFlag(NF_DELETED) )
      {
        observer->postAwareMessage(AS_DISAPPEARED,mID, n->mNode->mID ); // tell application this node disappeared.
        HeU16 s0 = mIndex;
        HeU16 s1 = n->mNode->mIndex;
        bool ok = awareManager.removePair(s0,s1);
        suppress_unused_variable_warning(ok);
        assert(ok);
        AwareNode *fnode = n;
        n->mNode->inverseUnaware(this,pool);
        n = n->mAwareNext;
        pool.Release(fnode); // put it back into the global free pool.

        if ( pn )
        {
          pn->mAwareNext    = n;
        }
        else
        {
          mAware = n;
        }
      }
      else
      {
        pn = n;
        n  = n->mAwareNext;
      }
    }
    return ret;
  }

  // this node is no longer aware of us, so we remove it from our inverse awareness list.
  inline void inverseUnaware(KdTreeNode *node,AwarePool &pool)
  {
    bool found = false;
    AwareNode *n = mInverseAware;
    AwareNode *pn = 0;
    while ( n )
    {
      if ( n->mNode == node )
      {
        found = true;
        AwareNode *fnode = n;
        n = n->mAwareNext;
        pool.Release(fnode);
        if ( pn )
        {
          pn->mAwareNext = n;
        }
        else
        {
          mInverseAware = n;
        }
        break;
      }
      else
      {
        pn = n;
        n  = n->mAwareNext;
      }
    }
    assert(found);
  }

  inline HeU32 unaware(HeU32 frameNo,AwareMessageInterface *observer,AwarePool &pool,AwareManager &awareManager)
  {
    HeU32 ret = 0;
    AwareNode *n = mAware;
    AwareNode *pn = 0;

    while ( n )
    {
      if ( n->mNode->mFrameNo != frameNo )
      {
        bool remove = true;

        if ( n->mNode->hasNodeFlag(NF_DELETED) )
        {
          observer->postAwareMessage(AS_DISAPPEARED, mID, n->mNode->mID ); // tell application this node disappeared.
        }
        else if ( hasNodeFlag(NF_HAVE_GRANULARITY) )
        {
          HeF32 r2 = mRadius + mGranularity;
          r2*=r2;
          HeF32 dx = mPos[0] - n->mNode->mPos[0];
          HeF32 dy = mPos[1] - n->mNode->mPos[1];
          HeF32 dz = mPos[2] - n->mNode->mPos[2];
          HeF32 d = dx*dx+dy*dy+dz*dz;
          if ( d >= r2 )
          {
            observer->postAwareMessage(AS_DEPARTED, mID, n->mNode->mID ); // tell application this node disappeared.
          }
          else
          {
            remove = false;
          }
        }
        else
        {
          observer->postAwareMessage(AS_DEPARTED, mID, n->mNode->mID ); // tell application this node disappeared.
        }

        if ( remove )
        {
          HeU16 s0 = mIndex;
          HeU16 s1 = n->mNode->mIndex;
          bool ok = awareManager.removePair(s0,s1);
          suppress_unused_variable_warning(ok);
          assert(ok);

          n->mNode->inverseUnaware(this,pool);

          AwareNode *fnode = n;
          n = n->mAwareNext;
          pool.Release(fnode); // put it back into the global free pool.

          if ( pn )
          {
            pn->mAwareNext    = n;
          }
          else
          {
            mAware = n;
          }
        }
        else
        {
          pn = n;
          n  = n->mAwareNext;
        }
      }
      else
      {
        pn = n;
        n  = n->mAwareNext;
      }
    }
    return ret;
  }

  unsigned int iterate(IDVector &list,SecondsType stime,SecondsType latency)
  {
    unsigned int ret = 0;

    AwareNode *a = mAware;
    while ( a )
    {
      if ( awareTime(a->mTime,stime,latency) )
      {
        list.push_back(a->mNode->mID);
        ret++;
      }
      a = a->mAwareNext;
    }

    return ret;
  }

  unsigned int iterateOf(IDVector &list,SecondsType stime,SecondsType latency)
  {
    unsigned int ret = 0;

    AwareNode *a = this->mInverseAware;
    while ( a )
    {
      if ( awareTime(a->mTime,stime,latency) )
      {
        list.push_back(a->mNode->mID);
        ret++;
      }
      a = a->mAwareNext;
    }

    return ret;
  }


  unsigned int iterate(SpatialAwarenessIteratorCallback *callback,bool &cancelled,SecondsType stime,SecondsType latency)
  {
    HeU32 ret = 0;
    cancelled = false;
    AwareNode *a = mAware;
    while ( a )
    {
      if ( awareTime(a->mTime,stime,latency) )
      {
        bool ok = callback->SAI_iterate(mID,a->mNode->mID);
        ret++;
        if ( !ok )
        {
          cancelled = true;
          break;
        }
      }
      a = a->mAwareNext;
    }

    return ret;
  }

  void setGranularity(HeF32 v)
  {

    if ( v == 0 )
      clearNodeFlag(NF_HAVE_GRANULARITY);
    else
      setNodeFlag(NF_HAVE_GRANULARITY);

    mGranularity = v;
  }

  bool setProperty(const char *key,const char *value)
  {
    bool ret = false;

    assert(key);
    assert(value);
    if ( key && value )
    {
      if ( stricmp(key,"Granularity") == 0 )
      {
        HeF32 v = (HeF32) atof(value);
        setGranularity(v);
        ret = true;
      }
    }
    return ret;
  }

  ID              mID;
  HeU16           mIndex;
  HeU16           mFlags;
  HeF32           mNodePos[3];        // position at the time it was inserted into the tree.
  HeF32           mPos[3];  // real-current position.
  HeF32           mRadius;
  HeF32           mGranularity;
  KdTreeNode     *mLeft;   // also used as the 'next' pointer in the free list.
  KdTreeNode     *mRight;
  AwareNode      *mAware;   // linked list of items we are aware of.
  AwareNode      *mInverseAware; // people that are aware of me.
  HeU32           mFrameNo;
  HeU32           mCreateFrameNo;
};

#define BUNDLE_SIZE 2048  // 2k worth of nodes at a time.

class KdTreeNodeBundle
{
public:
  KdTreeNodeBundle(KdTreeNodeBundle *next)
  {
    mNext = next;
    mCount = 0;
  }

  KdTreeNode * getNextNode(void)
  {
    KdTreeNode *ret = 0;

    if ( mCount < BUNDLE_SIZE )
    {
      ret = &mNodes[mCount];
      mCount++;
    }
    return ret;
  }

  KdTreeNodeBundle * getNextBundle(void) const { return mNext; };

  HeU32 getCount(void) const { return mCount; }
  void         setCount(HeU32 c) { mCount = c; };

  KdTreeNode * getNode(HeU32 i)
  {
    assert( i >= 0 && i < BUNDLE_SIZE );
    return &mNodes[i];
  }

  KdTreeNodeBundle  *mNext;
  HeU32       mCount;
  KdTreeNode         mNodes[BUNDLE_SIZE];
};

class KdTree
{
public:
  KdTree(void)
  {
    mIndex  = 0;
    mRoot   = 0;
    mBundle = 0;
    mFree   = 0;
    mStart = 0;
    mIterator = 0;
    mIteratorBundle = 0;
  }

  ~KdTree(void)
  {
    reset();
  }

  HeU32 search(const HeF32 *pos,HeF32 radius,KdTreeInterface *callback) const
  {
    if ( !mRoot )	return 0;
    HeU32 count = 0;
    mRoot->search(X_AXIS,pos,radius,count,callback);
    return count;
  }

  void reset(void)
  {
    mFree = 0;
    mRoot = 0;
    KdTreeNodeBundle *bundle = mBundle;
    while ( bundle )
    {
      KdTreeNodeBundle *next = bundle->getNextBundle();
      delete bundle;
      bundle = next;
    }
    mBundle = 0;
    mIterator = 0;
    mIteratorBundle = 0;
  }

  KdTreeNode * getNode(ID id)
  {
    KdTreeNode *node;

    if ( mFree )
    {
      node  = mFree;
      mFree = mFree->mLeft;
      new (node ) KdTreeNode(id,node->mIndex);
    }
    else
    {
      if ( mBundle == 0 )
      {
        mBundle = MEMALLOC_NEW(KdTreeNodeBundle)(0);
        mIteratorBundle = mBundle;
      }

      node = mBundle->getNextNode();
      if ( node == 0 )
      {
        mBundle = MEMALLOC_NEW(KdTreeNodeBundle)(mBundle);
        node = mBundle->getNextNode();
      }
      assert( mIndex != 0xFFFF );
      new (node ) KdTreeNode(id,mIndex++);
    }
    return node;
  }

  void setRadius(KdTreeNode *node,HeF32 radius)
  {
    node->setRadius(radius);
  }

  void setPosition(KdTreeNode *node,const HeF32 *pos)
  {

    bool first = node->setPosition(pos);

    if ( first )
    {
      if ( mRoot )
      {
        mRoot->add(node,X_AXIS);
      }
      else
      {
        mRoot = node;
      }
    }
  }

  void rebuildTree(void)
  {
    if ( mRoot )
    {
      mRoot = 0;

      KdTreeNodeBundle *bundle = mBundle;

      while ( bundle )
      {
        HeU32 scount = bundle->getCount();
        for (HeU32 i=0; i<scount; i++)
        {
          KdTreeNode *node = bundle->getNode(i);
          if ( !node->hasNodeFlag(NF_FREE_LIST) ) // if not already reserved on the 'free list'
          {
            if ( node->hasNodeFlag(NF_DELETED) )
            {
              assert( node->mAware == 0 );
              HeU16 index = node->mIndex;
              new ( node ) KdTreeNode;
              node->setNodeFlag(NF_FREE_LIST);
              node->mIndex = index;
              addFreeList(node);
            }
            else
            {
              node->mLeft = 0;
              node->mRight = 0;
              node->clearNodeFlag(NF_HAVE_POSITION);
              setPosition(node,node->mPos);
              AwareNode *a = node->mAware;
              while ( a )
              {
                assert( !a->mNode->hasNodeFlag(NF_DELETED) );
                a = a->mAwareNext;
              }
            }
          }
        }
        bundle = bundle->mNext;
      }
    }
  }

  // add to the 'free' list of nodes.
  void addFreeList(KdTreeNode *node)
  {
    node->mLeft = mFree;
    mFree = node;
  }

  KdTreeNode * iterate(HeU32 index,bool &wrapped)
  {
    KdTreeNode *ret = 0;
    if ( mIteratorBundle )
    {
      if ( mIterator >= mIteratorBundle->getCount() )
      {
        mIterator = 0;
        mIteratorBundle = mIteratorBundle->getNextBundle();
        if ( mIteratorBundle == 0 )
        {
          mIteratorBundle = mBundle;
        }
      }
      ret = mIteratorBundle->getNode(mIterator);
      mIterator++;
    }

    wrapped = false;

    if ( index == 0 )
    {
      mStart = ret;
    }
    else
    {
      if ( ret == mStart )
      {
        wrapped = true;
      }
    }
    return ret;
  }

private:
  KdTreeNode       *mFree;
  KdTreeNode       *mRoot;
  KdTreeNodeBundle *mBundle;
  KdTreeNode       *mStart;
  HeU16             mIndex;
  HeU32             mIterator;
  KdTreeNodeBundle *mIteratorBundle;

};

typedef USER_STL::hash_map< ID, KdTreeNode * > KdTreeNodeHash;
typedef USER_STL::list< AwareMessage >            AwareMessageList;
typedef SimpleVector< KdTreeNode * >         KdTreeNodeVector;

class LazyKdTree : public KdTreeInterface, public AwareMessageInterface
{
public:

  LazyKdTree(SpatialAwarenessObserver *observer)
  {
    mDeleteCount    = 0;
    mRefreshPer     = 500;
    mSearchPer      = 10000;
    mRebuildTree    = 16;
    mObserver       = observer;
    mTree           = MEMALLOC_NEW(KdTree);
    mName           = L"SAS_LazyKdTree";
    mTime           = 0;
    mLastUpdate     = 0;
    mUpdatePeriod   = 1.0f / 60.0f;
    mNode           = 0;
    mFrameNo        = 0;
    mUpdateFrameNo  = 0;
    mBuildCount     = 0;
    mDefaultGranularity = 0;
    mMessageLatency = 0.25; // 1/4 second message latency by default.
  }

  ~LazyKdTree(void)
  {
    delete mTree;
  }

  inline void postAwareMessage(AwareState state,ID from,ID to)
  {
    AwareMessage am(state,from,to,mTime);

    if ( state == AS_DEPARTED )
    {
      bool add = true;
      AwareMessageList::iterator i = mMessages.begin();
      while ( i!=mMessages.end() )
      {
        AwareMessage &a = (*i);
        if ( a.mFrom == from && a.mTo == to )
        {
          SecondsType diff = mTime - a.mTime;
          if ( diff < mMessageLatency )
          {
            add = false;
            i = mMessages.erase(i);
          }
          else
          {
            i++;
          }
        }
        else
        {
          i++;
        }
      }
      if ( add )
      {
        mDeparted.push_back(am);
      }
    }
    else if ( state == AS_DISAPPEARED )
    {
      bool add = true;
      AwareMessageList::iterator i = mMessages.begin();
      while ( i!=mMessages.end() )
      {
        AwareMessage &a = (*i);
        if ( a.mFrom == from && a.mTo == to )
        {
          SecondsType diff = mTime - a.mTime;
          if ( diff < mMessageLatency )
          {
            add = false;
            i = mMessages.erase(i);
          }
          else
          {
            i++;
          }
        }
        else
        {
          i++;
        }
      }
      if ( add )
      {
        mDisappeared.push_back(am);
      }
    }
    else
    {
      mMessages.push_back(am);
    }
  }

  void Pump(SecondsType time_elapsed_since_last_pump)
  {
    if ( mActive )
    {
      mTime+=time_elapsed_since_last_pump;
      SecondsType diff = mTime - mLastUpdate;
      if ( diff >= mUpdatePeriod )
      {
        mLastUpdate = mTime;

        flushMessages(); // flush any previously outstanding messages

        bool wrapped;
        mSearchCount = 0;
        for (HeU32 i=0; i<mRefreshPer; i++)
        {
          KdTreeNode *node = mTree->iterate(i,wrapped);
          if ( wrapped )
          {
            mUpdateFrameNo++;
            break;
          }
          if ( mSearchCount > mSearchPer )
          {
            break;
          }
          if ( node && !node->hasNodeFlag(NF_DELETED) && !node->hasNodeFlag(NF_FREE_LIST) )
          {
            refreshNode(node);
          }
        }
        flushMessages();

        if ( mDirtyTree )
        {
          mBuildCount++;
          if ( mBuildCount == mRebuildTree )
          {
            mDirtyTree = false;
            mTree->rebuildTree();
            mBuildCount = 0;
          }
        }
      }
    }
  }

  void refreshNode(KdTreeNode *node)
  {
    mFrameNo++;
    if ( node->hasNodeFlag(NF_HAVE_POSITION) && node->hasNodeFlag(NF_HAVE_RANGE) && !node->hasNodeFlag(NF_DELETED) )
    {
      mNode = node;
      mTree->search( mNode->mPos, mNode->mRadius, this );
      mNode->unaware(mFrameNo,this,mAwareness,mAwareManager);
    }
  }

  bool SetActive(ActiveType new_active_flag)
  {
    mActive = new_active_flag;
    return true;
  }

  bool SetName(NameType new_name)
  {
    mName = new_name;
    return true;
  }

  bool SetUpdatePeriod(SecondsType new_update_period)
  {
    mUpdatePeriod = new_update_period;
    return true;
  }

  bool AddEntity(ID entity_ID)
  {
    bool ret = false;

    KdTreeNodeHash::iterator found;
    found = mNodes.find(entity_ID);
    if ( found != mNodes.end() )
    {
      assert(0);
    }
    else
    {
      ret = true;
      KdTreeNode *node = mTree->getNode(entity_ID);
      assert( !node->hasNodeFlag(NF_DELETED));
      node->setGranularity(mDefaultGranularity);
      node->setCreateFrameNo(mUpdateFrameNo);
      mNodes[entity_ID] = node;
      assert( entity_ID == node->mID );
    }

    return ret;
  }

  bool DeleteEntity(ID entity_ID,bool flushMessagesImmediately)
  {
    bool ret = false;

    KdTreeNodeHash::iterator found = mNodes.find(entity_ID);
    if ( found == mNodes.end() )
    {
      assert(0);
    }
    else
    {
      mDirtyTree = true;

      mDeleteCount++;
      KdTreeNode *node = (*found).second;
      assert( (*found).first == node->mID );
      mNodes.erase(found);
      node->setNodeFlag(NF_DELETED);
      {
        AwareNode *a = node->mInverseAware;
        while ( a )
        {
          AwareNode *nextNode = a->mAwareNext;
          a->mNode->flushDeleted(this,mAwareness,mAwareManager);
          a = nextNode;
        }
      }

      AwareNode *a = node->mAware;
      while ( a )
      {
        HeU16 s0 = node->mIndex;
        HeU16 s1 = a->mNode->mIndex;
        bool ok = mAwareManager.removePair(s0,s1);
        suppress_unused_variable_warning(ok);
        assert(ok);
        AwareNode *fnode = a;
        a->mNode->inverseUnaware(node,mAwareness);
        a = a->mAwareNext;
        mAwareness.Release(fnode);
      }

      node->mAware = 0;

      if ( flushMessagesImmediately )
      {
        flushMessages();
      }

      ret = true;
    }

    return ret;
  }

  bool fdiff3(const float *p1,const float *p2) const
  {
    bool ret = false;

    float dx = p1[0] - p2[0];
    float dy = p1[1] - p2[1];
    float dz = p1[2] - p2[2];
    float d  = dx*dx+dy*dy+dz*dz;
    if ( d > 0.01f )
    {
      ret = true;
    }
    return ret;
  }

  bool UpdateEntityPosition(ID entity_ID, const HeF32 position_vec3[3])
  {
    bool ret = false;

    KdTreeNodeHash::iterator found = mNodes.find(entity_ID);
    if ( found == mNodes.end() )
    {
      assert(0);
    }
    else
    {
      KdTreeNode *node = (*found).second;
      assert( node->mID == entity_ID );
      assert( node->mID == (*found).first );
      if ( mDirtyTree )
        mTree->setPosition(node,position_vec3);
      else
      {
        const HeF32 *pos = node->getPosition();
        if ( fdiff3(pos,position_vec3) )
        {
          mTree->setPosition(node,position_vec3);
          mDirtyTree = true;
        }
      }
      ret = true;
    }

    return ret;
  }

  bool UpdateEntityAwarenessRange(ID entity_ID, HeF32 range_f)
  {
    bool ret = false;

    KdTreeNodeHash::iterator found = mNodes.find(entity_ID);
    if ( found == mNodes.end() )
    {
      assert(0);
    }
    else
    {
      KdTreeNode *node = (*found).second;
      assert( node->mID == entity_ID );
      assert( node->mID == (*found).first );
      node->setRadius(range_f);
      ret = true;
    }

    return ret;
  }

  void foundNode(KdTreeNode *node,HeF32 /*distanceSquared*/)
  {
    mSearchCount++;
    if ( node != mNode && !node->hasNodeFlag(NF_DELETED) )
    {
      HeU16 s0 = mNode->mIndex;
      HeU16 s1 = node->mIndex;
      const userPair * up = mAwareManager.findPair(s0,s1);

      if ( !up )
      {
        assert( up == 0 );
        AwareNode *a = mAwareness.GetFreeLink(mTime);
        mAwareManager.addPair(s0,s1,a,true);
        mNode->addAware(a,node,mFrameNo);
        AwareNode *ia = mAwareness.GetFreeLink(mTime);
        node->addInverseAware(ia,mNode,mFrameNo);
        if ( node->mCreateFrameNo == this->mUpdateFrameNo )
        {
          postAwareMessage(AS_APPEARED,mNode->mID,node->mID);
        }
        else
        {
          postAwareMessage(AS_ENTERED,mNode->mID,node->mID);
        }
      }
      node->mFrameNo = mFrameNo;

    }
  }

  void flushMessages(void)
  {
    // flush messages.  handle all deletions first!
    if ( !mDeparted.empty() )
    {
      AwareMessageList::iterator i;
      for (i=mDeparted.begin(); i!=mDeparted.end(); ++i)
      {
        AwareMessage &am = (*i);
        mObserver->SAO_departed( am.mFrom, am.mTo );
      }
      mDeparted.clear();
    }

    if ( !mDisappeared.empty() )
    {
      AwareMessageList::iterator i;
      for (i=mDisappeared.begin(); i!=mDisappeared.end(); ++i)
      {
        AwareMessage &am = (*i);
        mObserver->SAO_disappeared( am.mFrom, am.mTo );
      }
      mDisappeared.clear();
    }

    if ( !mMessages.empty() )
    {
      AwareMessageList::iterator i = mMessages.begin();
      while ( i != mMessages.end() )
      {
        AwareMessage &am = (*i);

        SecondsType diff = mTime-am.mTime;
        if ( diff < mMessageLatency )
          break;

        switch ( am.mState )
        {
          case AS_ENTERED:
            mObserver->SAO_entered( am.mFrom, am.mTo );
            break;
          case AS_APPEARED:
            mObserver->SAO_appeared( am.mFrom, am.mTo );
            break;
          default:
            assert(0);
            break;
        }
        i = mMessages.erase(i);
      }
    }
  }

  unsigned int iterateAwareness(ID entity,IDVector &list)
  {
    KdTreeNodeHash::iterator found;
    found = mNodes.find(entity);
    if ( found != mNodes.end() )
    {
      KdTreeNode *node = (*found).second;
      node->iterate(list,mTime,mMessageLatency);
    }

    return (unsigned int)list.size();
  }

  unsigned int iterateAwareOf(ID entity,IDVector &list)
  {
    KdTreeNodeHash::iterator found;
    found = mNodes.find(entity);
    if ( found != mNodes.end() )
    {
      KdTreeNode *node = (*found).second;
      node->iterateOf(list,mTime,mMessageLatency);
    }

    return (unsigned int)list.size();
  }

  HeU32 iterateAll(SpatialAwarenessIteratorCallback *callback)
  {
    HeU32 ret = 0;

    KdTreeNodeHash::iterator i;
    for (i=mNodes.begin(); i!=mNodes.end(); ++i)
    {
      KdTreeNode *node = (*i).second;
      bool cancelled;
      ret+=node->iterate(callback,cancelled,mTime,mMessageLatency);
      if ( cancelled )
        break;
    }

    return ret;
  }

  bool setProperty(const char *key,const char *value)
  {
    bool ret = false;

    assert( key );
    assert( value );

    if ( key && value )
    {

      if ( stricmp(key,"RefreshPer") == 0 )
      {
        HeI32 v = atoi(value);
        if ( v >= 1 )
        {
          mRefreshPer = v;
          ret = true;
        }
      }
      else if ( stricmp(key,"SearchPer")==0)
      {
        HeI32 v = atoi(value);
        if ( v >= 1 )
        {
          mSearchPer = v;
          ret = true;
        }
      }
      else if ( stricmp(key,"RebuildTree") == 0 )
      {
        HeI32 v = atoi(value);
        if ( v >= 1 )
        {
          mRebuildTree = v;
          ret = true;
        }
      }
      else if ( stricmp(key,"DefaultGranularity") == 0 )
      {
        mDefaultGranularity = (HeF32)atof(value);
        ret = true;
      }
      else if ( stricmp(key,"MessageLatency") == 0 )
      {
        mMessageLatency = (SecondsType)atof(value);
        ret = true;
      }
    }
    return ret;
  }

  bool setProperty(ID entity,const char *key,const char *value)
  {
    bool ret = false;

    KdTreeNodeHash::iterator found = mNodes.find(entity);
    if ( found == mNodes.end() )
    {
      assert(0);
    }
    else
    {
      KdTreeNode *node = (*found).second;
      ret = node->setProperty(key,value);
    }

    return ret;
  }


private:
  KdTree                    *mTree;
  HeU32                      mSearchCount;
  HeU32                      mDeleteCount;
  SpatialAwarenessObserver  *mObserver;
  bool                       mActive:1;
  bool                       mDirtyTree:1;
  SecondsType                mMessageLatency;
  SecondsType                mLastUpdate; // time we last did an update
  SecondsType                mTime;
  SecondsType                mUpdatePeriod;
  NameType                   mName;
  KdTreeNode                *mNode;
  KdTreeNodeHash             mNodes;
  HeU32                      mFrameNo;
  HeU32                      mUpdateFrameNo;
  HeU32                      mBuildCount;
  HeU32                      mRebuildTree;
  HeU32                      mRefreshPer;
  HeU32                      mSearchPer;
  AwarePool                  mAwareness;

  AwareMessageList           mDeparted;
  AwareMessageList           mDisappeared;
  AwareMessageList           mMessages;

  HeF32                      mDefaultGranularity;
  AwareManager               mAwareManager;
};


SAS_LazyKdTree::SAS_LazyKdTree(SpatialAwarenessObserver *observer)
{
  mLazy           = MEMALLOC_NEW(LazyKdTree)(observer);
}

SAS_LazyKdTree::~SAS_LazyKdTree(void)
{
  delete mLazy;
}

void SAS_LazyKdTree::Pump(SecondsType time_elapsed_since_last_pump)
{
  mLazy->Pump(time_elapsed_since_last_pump);
}

bool SAS_LazyKdTree::SetActive(ActiveType new_active_flag)
{
  return mLazy->SetActive(new_active_flag);
}

bool SAS_LazyKdTree::SetName(NameType new_name)
{
  return mLazy->SetName(new_name);
}

bool SAS_LazyKdTree::SetUpdatePeriod(SecondsType new_update_period)
{
  return mLazy->SetUpdatePeriod(new_update_period);
}

bool SAS_LazyKdTree::AddEntity(ID entity_ID)
{
  return mLazy->AddEntity(entity_ID);
}

bool SAS_LazyKdTree::DeleteEntity(ID entity_ID,bool flushMessagesImmediatey)
{
  return mLazy->DeleteEntity(entity_ID,flushMessagesImmediatey);
}

bool SAS_LazyKdTree::UpdateEntityPosition(ID entity_ID, const HeF32 position_vec3[3])
{
  return mLazy->UpdateEntityPosition(entity_ID,position_vec3);
}

bool SAS_LazyKdTree::UpdateEntityAwarenessRange(ID entity_ID, HeF32 range_f)
{
  return mLazy->UpdateEntityAwarenessRange(entity_ID,range_f);
}

unsigned int SAS_LazyKdTree::iterateAwareness(ID entity,IDVector &list)
{
  return mLazy->iterateAwareness(entity,list);
}
HeU32 SAS_LazyKdTree::iterateAll(SpatialAwarenessIteratorCallback *callback)
{
  return mLazy->iterateAll(callback);
}

bool SAS_LazyKdTree::setProperty(const char *key,const char *value)
{
  return mLazy->setProperty(key,value);
}

bool SAS_LazyKdTree::setProperty(ID entity,const char *key,const char *value)
{
  return mLazy->setProperty(entity,key,value);
}

bool SAS_LazyKdTree::UpdateEntityAwarenessTime(ID /*entity_ID*/,SecondsType /*time*/)
{
  bool ret = false;

  return ret;
}

unsigned int SAS_LazyKdTree::iterateAwareOf(ID entity,IDVector &list)
{
  return mLazy->iterateAwareOf(entity,list);
}

}; // END OF NAMESPACE

