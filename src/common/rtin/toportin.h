#ifndef TOPORTIN_H

#define TOPORTIN_H

#include "common/snippets/HeSimpleTypes.h"
/** @file toportin.h
 *  @brief This is the system that does the actual work of building the right-triangle integrated network.
 *
 *  @author John W. Ratcliff
*/


/** @file toportin.cpp
 *  @brief This is the system that does the actual work of building the right-triangle integrated network.
 *
 *  @author John W. Ratcliff
*/



#define MAXERR USHRT_MAX	/* maximum scaled triangle error */

/* triangle info flag bits */
#define SMALLER1	  1	/* Is neighbor 1 smaller? */
#define SMALLER2	  2	/* Is neighbor 2 smaller? */
#define BIGGER3		  4	/* Is neighbor 3 larger? */
#define SURFACE		  8	/* on surface? */
#define ALLINROD	 16	/* completely in rectangle of definition? */
#define PARTINROD	 32	/* partially in rectangle of definition? */

class ErrorHeuristic;

// These never exist physically in a permanent way, they are created
// by recursively parsing the binary tree.
class TopoTriIndex
{
public:
  HeI32 x;        // x/y base co-ordinate.
  HeI32 y;
  HeI32 depth;		/* depth of triangle */
  HeI32 orient;		/* orientation */
  HeI32 s;		    /* physical address */
};

/* initial values */

#include <limits.h>

/*
 * =========== Common Constants =============
 */
#ifndef PI
#define PI 3.1415926535897932385f
#endif

/*
 * =========== Common Macros =============
 */
#ifndef MAX
#define MAX(a, b) (((a)>(b))?(a):(b))
#endif

#ifndef MIN
#define MIN(a, b) (((a)>(b))?(b):(a))
#endif

#ifndef ABS
#define ABS(x) (((x)<0)?(-(x)):(x))
#endif

#ifndef ROUND
#define ROUND(a) ((int)((a)+0.5))
#endif

/*
 * ============ Common Structures ============
 */

typedef HeI32 Coord2d[2];

// also 2^n + 1
#define BLOCKSIZE 128
#define BLOCKLEN (BLOCKSIZE+1)

#define WORLDSCALE 1.0f
#define ZWORLDSCALE 1.0f

#define RECIPWORLDSCALE (1.0f/WORLDSCALE)



class HeightData
{
public:
  virtual HeI32 GetWidth(void) const = 0;
  virtual HeI32 GetHeight(void) const = 0;

	virtual HeF32 Get(HeI32 x,HeI32 y) const = 0;
//	virtual float GetDZ(void) const = 0;
//  virtual float GetScaledZ(int x,int y) const = 0;
private:
};



enum IntersectTest
{
  IT_INSIDE,              // totally inside bounding region
  IT_PARTIAL,             // partially inside bounding region
  IT_OUTSIDE              // totally outside
};

enum Directions
{
 SOUTH,
 WEST,
 NORTH,
 EAST,
 NW,
 SE,
 NE,
 SW,
 DLAST // end of directions.
};

class TopoRtin
{
public:

  TopoRtin(const HeightData *hfield);

  void SetBlockSize(HeI32 blocksize)
  {
    mBlockSize = blocksize;
    mBlockLen  = blocksize+1;
  };

  void SetEye(HeI32 eyex,HeI32 eyey); // actually causes the network to be rebuilt!

  void SetLod(bool lod)
  {
    mDoLod = lod;
  };

  void SetErrorThreshold(HeI32 et)
  {
    mErrorThreshold = et;
    mFloatThreshold = HeF32(et);
  };

  void SetErrorHeuristic(ErrorHeuristic *error)
  {
    mError = error;
  }

  void SetDistanceThreshold(HeI32 dt)
  {
    mDistanceThreshold = dt;
  }

  void SetEdges(bool e) { mEdges = e; };
  void SetIgnoreSealevel(bool i) { mIgnoreSealevel = i; };
  void SetSeaLevel(HeF32 s) { mSealevel = s; };

  bool GetHasSea(void) const { return mHasSea; };

  HeU16 * BuildIndices(HeU16 *indices,
                                HeI32 left,
                                HeI32 top);


private:
//************************************************
// General *private* support routines.
//************************************************
   IntersectTest InBlock(const Coord2d &p1,const Coord2d &p2,const Coord2d &p3);
   void BlockTest(const Coord2d &p,HeI32 &acode);

  HeF32 GetZ(HeI32 x,HeI32 y) const
  {
	  return mField->Get(x,y);
  };


// ********************************************
// Part of addressing scheme in address.cpp
// ********************************************
   inline HeI32 init_ntriabove(HeI32 depth);
   inline void leftChild(TopoTriIndex &t, TopoTriIndex &lt);
   inline void rightChild(TopoTriIndex &t, TopoTriIndex &rt);
   inline void parent(TopoTriIndex &t, TopoTriIndex &p);
   inline void bro1(TopoTriIndex &t, TopoTriIndex &b1);
   inline void bro2(TopoTriIndex &t, TopoTriIndex &b2);
   inline void bro3(TopoTriIndex &t, TopoTriIndex &b3);
   inline void nbr1(TopoTriIndex &t, TopoTriIndex &n1);
   inline void nbr2(TopoTriIndex &t, TopoTriIndex &n2);
   inline void nbr3(TopoTriIndex &t, TopoTriIndex &n3);

   HeU16 *getVerts(const Coord2d &p1,
                                  const Coord2d &p2,
                                  const Coord2d &p3,
                                  HeU16 *indices);

// ********************************************
// Found in EMIT.CPP
// ********************************************
   HeU16 * emitSurface(HeU16 *indices);

   HeU16 * emitDFS(TopoTriIndex &t,
                                  HeU16 *indices,
                                  const Coord2d& p1,
                                  const Coord2d& p2,
                                  const Coord2d& p3,
                                  IntersectTest vstate);

   HeI32 inROD(Coord2d p1, Coord2d p2, Coord2d p3);

// ********************************************
// Found in SPLIT.CPP
// ********************************************
   inline void kidsOnSurf(TopoTriIndex &t);
   inline void split(TopoTriIndex &t);

// ********************************************
// Found in SURFACE.CPP
// ********************************************
   inline void rodSplit(TopoTriIndex &t);
   inline void eyeSplit(TopoTriIndex &t, Coord2d p1, Coord2d p2, Coord2d p3);
   inline bool tooCoarse(TopoTriIndex &t,Coord2d p1, Coord2d p2, Coord2d p3);
   inline void errSplit(TopoTriIndex &t, Coord2d p1, Coord2d p2, Coord2d p3);
   void baseSurface(void);
   void errSurface(void);

// ********************************************
// Found in TRI.CPP
// ********************************************
   HeI32    calcErr(Coord2d p1, Coord2d p2, Coord2d p3);
   void   midpoint(Coord2d p1,Coord2d p2,Coord2d m);
   void   MidPoint(const Coord2d &p1,const Coord2d &p2,Coord2d &m);
   HeI32    ceilingLog2(HeI32 x);
   HeI32    next2Power(HeI32 x);
   void   makeDFS(TopoTriIndex &t, Coord2d p1, Coord2d p2, Coord2d p3);
   void   initHier(void);

// These were macros, but have now been made methods.
   HeI32 BORDER(TopoTriIndex &t)
  {
    return (mInfo[t.s] & PARTINROD);
  };

   HeI32 INROD(TopoTriIndex &t)
  {
    return (mInfo[t.s] & ALLINROD);
  };

   void PUTONSURFACE(TopoTriIndex &t)
  {
    mInfo[t.s] = (mInfo[t.s]&(ALLINROD|PARTINROD))|SURFACE;
  };

   HeI32 ONSURFACE(TopoTriIndex &t)
  {
    return (mInfo[t.s] & SURFACE);
  };

  HeI32 ONSURFACE2(TopoTriIndex &t)
  {
    return (mInfo[t.s] & SURFACE);
  };

   bool SMALLN1(TopoTriIndex &t)
  {
    return (mInfo[t.s] & SMALLER1);
  };

   HeI32 SMALLN2(TopoTriIndex &t)
  {
    return (mInfo[t.s] & SMALLER2);
  };

   HeI32 BIGN3(TopoTriIndex &t)
  {
    return (mInfo[t.s] & BIGGER3);
  };

  /* these macros also take TopoTriIndex arguments */
   bool ISLEAF(TopoTriIndex &t)
  {
    return (t.depth >= mMaxDepth);
  };

   bool IAMLEFTCHILD(TopoTriIndex &t)
  {
    return (((t.depth&1)&&((t.orient^t.x)&1)) || (((t.depth&1)==0)&&((t.orient^t.x^t.y)&1)));
  };

   void TRIEQUATE(TopoTriIndex &a,const TopoTriIndex &b) { a = b; };

  bool IsEdge(Coord2d p);
	bool IsEdge(HeI32 x,HeI32 y);

// ********************************************
// * Misc. data variables.
// ********************************************
  Coord2d se, sw, ne, nw;

  HeI32 xmax[64];
  HeI32 ymax[64];
  HeI32 ntriabove[64];

  HeI32 hierDepth;

  Coord2d eyeP;

  HeF32 x_per_y;
  HeF32 y_per_x;
  HeF32 errPerEm;

  TopoTriIndex mRootL;
  TopoTriIndex mRootR;
  Coord2d  mSE;
  Coord2d  mSW;
  Coord2d  mNE;
  Coord2d  mNW;

  const HeightData *mField;

  HeI32 mBlockLeft;
  HeI32 mBlockTop;
  HeI32 mBlockRight;
  HeI32 mBlockBottom;

  HeI32 mFieldSize;  // size of entire height field.
// was in hier
  HeI32 mMaxDepth;			      /* maximum depth */
  HeI32 mSize;			        /* number of triangles in hierarchy */
  HeU16 *mErr;	/* error of triangles */
  HeU8 *mInfo;	/* surface,neighbor */
  bool mDoLod;
  HeI32  mErrorThreshold;
  HeF32 mFloatThreshold;
  HeI32  mDistanceThreshold;
  bool mEdges; // full resolution edges?
  bool mIgnoreSealevel;
  HeI32 mMinDepth;
  bool mHasSea;
  HeF32 mSealevel;
  ErrorHeuristic *mError;
  HeI32 mBlockSize;
  HeI32 mBlockLen;
};

extern Coord2d se, sw, ne, nw;

#endif
