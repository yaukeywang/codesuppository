#ifndef HE_ASTAR_H

#define HE_ASTAR_H

#include "common/snippets/UserMemAlloc.h"

class AI_Node
{
public:
  virtual HeF32        getDistance(const AI_Node *node,void *userData) = 0;
  virtual HeF32        getCost(const AI_Node *node,void *userData) = 0;
  virtual HeU32 getEdgeCount(void *userData) const = 0;
  virtual AI_Node *    getEdge(HeI32 index,void *userData) const = 0;

  virtual void debugRender(unsigned int color,float duration) = 0;

};


class HeAstar
{
public:
  virtual void              astarStartSearch(AI_Node *from,AI_Node *to,void *userData) = 0;  // start a search.
  virtual AI_Node         * astarGetFrom(void) = 0;
  virtual AI_Node         * astarGetTo(void) = 0;
  virtual bool              astarSearchStep(HeU32 &searchCount) = 0; // step the A star algorithm one time.  Return true if the search is completed.
  virtual AI_Node **        getSolution(HeU32 &count) = 0;  // retrieve the solution.  If this returns a null pointer and count of zero, it means no solution could be found.
  virtual void              setDirectSolution(void) = 0;
};

HeAstar         * createHeAstar(void);    // Create an instance of the HeAstar utility.
void              releaseHeAstar(HeAstar *astar);  // Release the intance of the HeAstar utility.


#endif
