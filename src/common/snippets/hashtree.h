#ifndef HASH_TREE_H

#define HASH_TREE_H

#include <vector>
#include <hash_map>
#include <list>

#include "common/snippets/UserMemAlloc.h"

#ifndef INT64

#ifdef WIN32
typedef HeI64				HeI64;
#elif LINUX
typedef HeI64			HeI64;
#else
	#error Unknown platform!
#endif

#endif


class HashTreeNode;

inline HeI64 getINT64(HeF32 v,HeF32 inverseSnapDistance)
{
  HeI64 ret = (HeI64) (v*inverseSnapDistance);
  return ret;
}

inline HeI64 getHash(HeI64 ix,HeI64 iy,HeI64 iz)
{
  return (ix<<20)^(iy<<10)^iz;
}

inline HeI64 computeHash(HeF32 x,HeF32 y,HeF32 z,HeF32 inverseSnapDistance)
{

  HeI64 ix = getINT64(x,inverseSnapDistance);
  HeI64 iy = getINT64(y,inverseSnapDistance);
  HeI64 iz = getINT64(z,inverseSnapDistance);
  HeI64 hash = getHash(ix,iy,iz);

  return hash;
}

class HashTreeNode
{
public:

  HashTreeNode(HeF32 x,HeF32 y,HeF32 z,HeF32 radius,void *userData,HeU32 index)
  {
    mX = x;
    mY = y;
    mZ = z;
    mRadius = radius;
    mUserData = userData;
    mIndex = index;
  };

	~HashTreeNode(void)
  {
  }


  HeI64 getHash(HeF32 inverseSnapDistance) const
  {
    return computeHash(mX,mY,mZ,inverseSnapDistance);
  }


  HeF32        getX(void) const { return mX;  }
  HeF32        getY(void) const {  return mY; };
  HeF32        getZ(void) const { return mZ; };
  HeF32        getRadius(void) const { return mRadius; };
  HeU32 getIndex(void) const { return mIndex; };
  void *       getUserData(void) const { return mUserData; };


  HeF32 distanceSquared(const HeF32 *pos) const
  {
    HeF32 dx = pos[0] - mX;
    HeF32 dy = pos[1] - mY;
    HeF32 dz = pos[2] - mZ;
    return dx*dx+dy*dy+dz*dz;
  }

  bool getNearest(const HeF32 *pos,HeF32 radius,HeF32 &nearest)
  {
    bool ret = false;

    HeF32 d2 = distanceSquared(pos);
    if ( d2 < nearest )
    {
      nearest = d2;
      ret = true;
    }
    return ret;
  }


private:

  HeU32    mIndex;
  void           *mUserData;
  HeF32           mX;
  HeF32           mY;
  HeF32           mZ;
  HeF32           mRadius;
};


typedef USER_STL::vector< HashTreeNode  >               HashTreeNodeVector;

class HashEntry
{
public:
  HashEntry(void)
  {

  }

  HashEntry(HeU32 index)
  {
    mList.push_back(index);
  }

  void add(HeU32 index)
  {
    mList.push_back(index);
  }

  HeU32 getNearest(const HeF32 *pos,HeF32 radius,HashTreeNodeVector &nodes,HeF32 &nearest,bool &found)
  {
    HeU32 ret = 0;

    found = false;

    USER_STL::vector< HeU32  >::iterator i;
    for (i=mList.begin(); i!=mList.end(); ++i)
    {
      HeU32 index = (*i);
      HashTreeNode &node = nodes[index];
      if (  node.getNearest(pos,radius,nearest) )
      {
        ret = index;
        found = true;
      }
    }
    return ret;
  }

  USER_STL::vector< HeU32 > mList; // list of nodes that map to this hash value.
};

typedef USER_STL::hash_map< HeI64, HashEntry >      HashEntryHash;


class HashTree
{
public:
  HashTree(void)
  {
    mSnapDistance        = 0.1f;
    mInverseSnapDistance = 1.0f/mSnapDistance;
  }

  ~HashTree(void)
  {
    reset();
  }

  void setSnapDistance(HeF32 snapDistance)
  {
    mSnapDistance = snapDistance;
    mInverseSnapDistance = 1.0f / snapDistance;
  }

  void reset(void)
  {
    mObjects.clear();
    mHashTable.clear();
  }

  HeU32 add(HeF32 x,HeF32 y,HeF32 z,HeF32 radius,void *userData)
  {
    HeU32 ret = mObjects.size();
    HashTreeNode node( x, y, z, radius, userData, ret );
    mObjects.push_back(node);

    HeI64 hash = node.getHash(mInverseSnapDistance);
    HashEntryHash::iterator found;
    found = mHashTable.find(hash);
    if ( found == mHashTable.end() )
    {
      HashEntry he(ret);
      mHashTable[hash] = he;
    }
    else
    {
      (*found).second.add(ret);
    }

    return ret;
  }


  HeU32 getNearest(const HeF32 *pos,HeF32 radius,bool &_found) // returns the nearest possible neighbor's index.
  {
    HeU32 ret = 0;

    _found = false;

    HeF32 nearest=1e9;

    HeI64 ix = getINT64(pos[0],mInverseSnapDistance);
    HeI64 iy = getINT64(pos[1],mInverseSnapDistance);
    HeI64 iz = getINT64(pos[2],mInverseSnapDistance);

    // check 3x3 set of hash grid entries around this location.
    for (HeI64 x=ix-1; x<=ix+1; x++)
    {
      for (HeI64 y=iy-1; y<=iy+1; y++)
      {
        for (HeI64 z=iz-1; z<=iz+1; z++)
        {

          HeI64 hash = getHash(x,y,z);

          HashEntryHash::iterator f;

          f = mHashTable.find(hash);
          if ( f != mHashTable.end() )
          {
            bool found;
            HeU32 index = (*f).second.getNearest(pos,radius,mObjects,nearest,found);
            if ( found )
            {
              ret = index;
              _found = found;
            }
          }
        }
      }
    }


    return ret;

  }

  HashTreeNode * getNodeFromIndex(HeU32 index)
  {
    assert( index < mObjects.size() );
    return &mObjects[index];
  }

private:
  HeF32              mSnapDistance;
  HeF32              mInverseSnapDistance;
  HashTreeNodeVector mObjects;
  HashEntryHash      mHashTable;
};


#endif
