#ifndef KDTREE_H

#define KDTREE_H

#include "UserMemAlloc.h"

class KdTreeNode;

typedef std::vector< KdTreeNode * > KdTreeNodeVector;

enum Axes
{
  X_AXIS = 0,
  Y_AXIS = 1,
  Z_AXIS = 2
};

class KdTreeNode;

class KdTreeFilter
{
public:
  virtual bool filterNode(KdTreeNode *node) = 0;
};

class KdTreeFindNode
{
public:
  KdTreeFindNode(void)
  {
    mNode = 0;
    mDistance = 0;
  }
  KdTreeNode  *mNode;
  NxF32        mDistance;
};

class KdTreeInterface
{
public:
  virtual void foundNode(KdTreeNode *node,NxF32 distanceSquared) = 0;
};

class KdTreeNode
{
public:
  KdTreeNode(void)
  {
    mX = 0;
    mY = 0;
    mZ = 0;
    mUserData = 0;
    mIndex = 0;
    mLeft = 0;
    mRight = 0;
  }

  KdTreeNode(NxF32 x,NxF32 y,NxF32 z,NxF32 /*radius*/,void *userData,NxU32 index)
  {
    mX = x;
    mY = y;
    mZ = z;
    mUserData = userData;
    mIndex = index;
    mLeft = 0;
    mRight = 0;
  };

	~KdTreeNode(void)
  {
  }


  void add(KdTreeNode *node,Axes dim)
  {
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

  NxF32        getX(void) const { return mX;  }
  NxF32        getY(void) const {  return mY; };
  NxF32        getZ(void) const { return mZ; };
  NxU32 getIndex(void) const { return mIndex; };
  void *       getUserData(void) const { return mUserData; };

  void search(Axes axis,const NxF32 *pos,NxF32 radius,NxU32 &count,NxU32 maxObjects,KdTreeFindNode *found,bool xzOnly,KdTreeFilter *filter)
  {

    NxF32 dx = pos[0] - getX();
    NxF32 dy = pos[1] - getY();
    NxF32 dz = pos[2] - getZ();

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
        if ( xzOnly )
        {
          search1 = mLeft;
          search2 = mRight;
        }
        else
        {
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

    bool ok = true;

    if ( filter )
    {
      ok = filter->filterNode(this);
    }

    if ( ok )
    {

      NxF32 r2 = radius*radius;
      NxF32 m = dx*dx+dz*dz;

      if ( !xzOnly )
      {
        m+=dy*dy;
      }

      if ( m < r2 )
      {
        switch ( count )
        {
          case 0:
            found[count].mNode = this;
            found[count].mDistance = m;
            break;
          case 1:
            if ( m < found[0].mDistance )
            {
              if ( maxObjects == 1 )
              {
                found[0].mNode = this;
                found[0].mDistance = m;
              }
              else
              {
                found[1] = found[0];
                found[0].mNode = this;
                found[0].mDistance = m;
              }
            }
            else if ( maxObjects > 1)
            {
              found[1].mNode = this;
              found[1].mDistance = m;
            }
            break;
          default:
            {
              bool inserted = false;

              for (NxU32 i=0; i<count; i++)
              {
                if ( m < found[i].mDistance ) // if this one is closer than a pre-existing one...
                {
                  // insertion sort...
                  NxU32 scan = count;
                  if ( scan >= maxObjects ) scan=maxObjects-1;
                  for (NxU32 j=scan; j>i; j--)
                  {
                    found[j] = found[j-1];
                  }
                  found[i].mNode = this;
                  found[i].mDistance = m;
                  inserted = true;
                  break;
                }
              }

              if ( !inserted && count < maxObjects )
              {
                found[count].mNode = this;
                found[count].mDistance = m;
              }
            }
            break;
        }
        count++;
        if ( count > maxObjects )
        {
          count = maxObjects;
        }
      }
    }

    if ( search1 )
  		search1->search( axis, pos,radius, count, maxObjects, found,xzOnly,filter);
    if ( search2 )
	  	search2->search( axis, pos,radius, count, maxObjects, found,xzOnly,filter);
  }

  void search(Axes axis,const NxF32 *pos,NxF32 radius,NxU32 &count,KdTreeInterface *callback)
  {

    NxF32 dx = pos[0] - getX();
    NxF32 dy = pos[1] - getY();
    NxF32 dz = pos[2] - getZ();

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

    NxF32 r2 = radius*radius;
    NxF32 m  = dx*dx+dy*dy+dz*dz;

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

private:

  void setLeft(KdTreeNode *left) { mLeft = left; };
  void setRight(KdTreeNode *right) { mRight = right; };

	KdTreeNode *getLeft(void)         { return mLeft; }
	KdTreeNode *getRight(void)        { return mRight; }

  NxU32    mIndex;
  void           *mUserData;
  NxF32           mX;
  NxF32           mY;
  NxF32           mZ;
  KdTreeNode     *mLeft;
  KdTreeNode     *mRight;
};

#define MAX_BUNDLE_SIZE 1024  // 1024 nodes at a time, to minimize memory allocation and guarentee that pointers are persistent.

class KdTreeNodeBundle
{
public:

  KdTreeNodeBundle(void)
  {
    mNext = 0;
    mIndex = 0;
  }

  bool isFull(void) const
  {
    return (bool)( mIndex == MAX_BUNDLE_SIZE );
  }

  KdTreeNode * getNextNode(void)
  {
    assert(mIndex<MAX_BUNDLE_SIZE);
    KdTreeNode *ret = &mNodes[mIndex];
    mIndex++;
    return ret;
  }

  KdTreeNodeBundle * getNextBundle(void) { return mNext; };
  void               setNextBundle(KdTreeNodeBundle *b) { mNext = b; };

  size_t             mIndex;
  KdTreeNode         mNodes[MAX_BUNDLE_SIZE];
private:
  KdTreeNodeBundle  *mNext;
};

class KdTree
{
public:
  KdTree(void)
  {
    mRoot = 0;
    mBundle = 0;
    mVcount = 0;
  }

  ~KdTree(void)
  {
    reset();
  }

  NxU32 search(const NxF32 *pos,NxF32 radius,NxU32 maxObjects,KdTreeFindNode *found,KdTreeFilter *filter) const
  {
    if ( !mRoot )	return 0;
    NxU32 count = 0;
    mRoot->search(X_AXIS,pos,radius,count,maxObjects,found,false,filter);
    return count;
  }

  NxU32 searchXZ(const NxF32 *pos,NxF32 radius,NxU32 maxObjects,KdTreeFindNode *found,KdTreeFilter *filter) const
  {
    if ( !mRoot )	return 0;
    NxU32 count = 0;
    mRoot->search(X_AXIS,pos,radius,count,maxObjects,found,true,filter);
    return count;
  }


  NxU32 search(const NxF32 *pos,NxF32 radius,KdTreeInterface *callback) const
  {
    if ( !mRoot )	return 0;
    NxU32 count = 0;
    mRoot->search(X_AXIS,pos,radius,count,callback);
    return count;
  }

  void reset(void)
  {
    mRoot = 0;
    KdTreeNodeBundle *bundle = mBundle;

    while ( bundle )
    {
      KdTreeNodeBundle *next = bundle->getNextBundle();
      delete bundle;
      bundle = next;
    }

    mBundle = 0;
    mVcount = 0;
  }

  NxU32 add(NxF32 x,NxF32 y,NxF32 z,NxF32 radius,void *userData)
  {
    NxU32 ret = mVcount;
    mVcount++;
    KdTreeNode *node = getNewNode(ret);
    new ( node ) KdTreeNode( x, y, z, radius, userData, ret );
    if ( mRoot )
    {
      mRoot->add(node,X_AXIS);
    }
    else
    {
      mRoot = node;
    }
    return ret;
  }


  NxU32 getNearest(const NxF32 *pos,NxF32 radius,bool &_found) const // returns the nearest possible neighbor's index.
  {
    NxU32 ret = 0;

    _found = false;
    KdTreeFindNode found[1];
    NxU32 count = search(pos,radius,1,found,0);
    if ( count )
    {
      KdTreeNode *node = found[0].mNode;
      ret = node->getIndex();
      _found = true;
    }

    return ret;

  }

  KdTreeNode * getNewNode(size_t /*index*/)
  {
    if ( mBundle == 0 )
    {
      mBundle = MEMALLOC_NEW(KdTreeNodeBundle);
    }
    if ( mBundle->isFull() )
    {
      KdTreeNodeBundle *bundle = MEMALLOC_NEW(KdTreeNodeBundle);
      bundle->setNextBundle(mBundle);
      mBundle = bundle;
    }

    KdTreeNode *node = mBundle->getNextNode();

    return node;
  }


private:
  NxU32            mVcount;
  KdTreeNode      *mRoot;
  KdTreeNodeBundle       *mBundle;
};


#endif
