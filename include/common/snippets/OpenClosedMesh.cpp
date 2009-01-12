#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "OpenClosedMesh.h"
#include "UserMemAlloc.h"
#include <vector>

namespace OPEN_CLOSED_MESH
{

typedef USER_STL::vector< size_t > size_tVector;

class Edge
{
public:
  Edge(void)
  {
    mI1 = mI2 = 0;
    mNextEdge = 0;
  }

	void init(size_t i1,size_t i2)
	{
		mI1 = i1;
		mI2 = i2;
		mNextEdge = 0;
	}

  size_t getHash(void) const
  {
    size_t ret;
    if ( mI1 < mI2 )
      ret = (mI1<<16)|mI2;
    else
      ret = (mI2<<16)|mI1;
    return ret;
  }

  size_t mI1;
  size_t mI2;
  Edge  *mNextEdge;
};

typedef USER_STL::hash_map< size_t, Edge * > EdgeMap;

class MyOpenClosedMesh : public OpenClosedMesh
{
public:
  MyOpenClosedMesh(void)
  {
    mEdges = 0;
  }

  ~MyOpenClosedMesh(void)
  {
    reset();
  }

  void reset(void)
  {
    delete []mEdges;
    mEdges = 0;
    mEdgeMap.clear();
    mOpenEdges.clear();
  }

  const size_t * isClosedMesh(size_t tcount,size_t *indices,size_t &ecount)
  {
    const size_t *ret = 0;
    ecount = 0;

    reset();
    mEdgeCount = tcount*3;
    mEdges = new Edge[mEdgeCount];

    Edge *e = mEdges;
    for (size_t i=0; i<tcount; i++)
    {
      size_t i1 = *indices++;
      size_t i2 = *indices++;
      size_t i3 = *indices++;
      e = addEdge(e,i1,i2);
      e = addEdge(e,i2,i3);
      e = addEdge(e,i3,i1);
    }

		{
			EdgeMap::iterator i;
			for (i=mEdgeMap.begin(); i!=mEdgeMap.end(); ++i)
			{
				Edge *e = (*i).second;
				if ( e->mNextEdge == 0 )
				{
          mOpenEdges.push_back(e->mI1);
          mOpenEdges.push_back(e->mI2);
				}
			}
		}

    if ( !mOpenEdges.empty() )
    {
      ret = &mOpenEdges[0];
      ecount = mOpenEdges.size()/2;
    }

    return ret;
  }

  Edge * addEdge(Edge *e,size_t i1,size_t i2)
  {
    assert( i1 < 65536 );
    assert( i2 < 65536 );
    e->init(i1,i2);
    size_t hash = e->getHash();
    EdgeMap::iterator found;
    found = mEdgeMap.find(hash);
    if ( found == mEdgeMap.end() )
    {
      mEdgeMap[hash] = e;
    }
    else
    {
      e->mNextEdge = (*found).second;
      mEdgeMap[hash] = e;
    }
    return e+1;
  }

  EdgeMap  mEdgeMap;
  size_t   mEdgeCount;
  Edge    *mEdges;
  size_tVector mOpenEdges;
};


};

using namespace OPEN_CLOSED_MESH;

OpenClosedMesh * createOpenClosedMesh(void)
{
  MyOpenClosedMesh *m = new MyOpenClosedMesh;
  return static_cast< OpenClosedMesh *>(m);
}

void             releaseOpenClosedMesh(OpenClosedMesh *mesh)
{
  MyOpenClosedMesh *m = static_cast< MyOpenClosedMesh *>(mesh);
  delete m;
}
