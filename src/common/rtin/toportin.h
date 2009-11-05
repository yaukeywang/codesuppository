#ifndef TOPORTIN_H

#define TOPORTIN_H

#include "common/snippets/UserMemAlloc.h"
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
  NxI32 x;        // x/y base co-ordinate.
  NxI32 y;
  NxI32 depth;		/* depth of triangle */
  NxI32 orient;		/* orientation */
  NxI32 s;		    /* physical address */
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

typedef NxI32 Coord2d[2];

// also 2^n + 1
#define BLOCKSIZE 128
#define BLOCKLEN (BLOCKSIZE+1)

#define WORLDSCALE 1.0f
#define ZWORLDSCALE 1.0f

#define RECIPWORLDSCALE (1.0f/WORLDSCALE)



class HeightData
{
public:
  virtual NxI32 GetWidth(void) const = 0;
  virtual NxI32 GetHeight(void) const = 0;

	virtual NxF32 Get(NxI32 x,NxI32 y) const = 0;
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

  void SetBlockSize(NxI32 blocksize)
  {
    mBlockSize = blocksize;
    mBlockLen  = blocksize+1;
  };

  void SetEye(NxI32 eyex,NxI32 eyey); // actually causes the network to be rebuilt!

  void SetLod(bool lod)
  {
    mDoLod = lod;
  };

  void SetErrorThreshold(NxI32 et)
  {
    mErrorThreshold = et;
    mFloatThreshold = NxF32(et);
  };

  void SetErrorHeuristic(ErrorHeuristic *error)
  {
    mError = error;
  }

  void SetDistanceThreshold(NxI32 dt)
  {
    mDistanceThreshold = dt;
  }

  void SetEdges(bool e) { mEdges = e; };
  void SetIgnoreSealevel(bool i) { mIgnoreSealevel = i; };
  void SetSeaLevel(NxF32 s) { mSealevel = s; };

  bool GetHasSea(void) const { return mHasSea; };

  NxU16 * BuildIndices(NxU16 *indices,
                                NxI32 left,
                                NxI32 top);


private:
//************************************************
// General *private* support routines.
//************************************************
   IntersectTest InBlock(const Coord2d &p1,const Coord2d &p2,const Coord2d &p3);
   void BlockTest(const Coord2d &p,NxI32 &acode);

  NxF32 GetZ(NxI32 x,NxI32 y) const
  {
	  return mField->Get(x,y);
  };


// ********************************************
// Part of addressing scheme in address.cpp
// ********************************************
   inline NxI32 init_ntriabove(NxI32 depth);
   inline void leftChild(TopoTriIndex &t, TopoTriIndex &lt);
   inline void rightChild(TopoTriIndex &t, TopoTriIndex &rt);
   inline void parent(TopoTriIndex &t, TopoTriIndex &p);
   inline void bro1(TopoTriIndex &t, TopoTriIndex &b1);
   inline void bro2(TopoTriIndex &t, TopoTriIndex &b2);
   inline void bro3(TopoTriIndex &t, TopoTriIndex &b3);
   inline void nbr1(TopoTriIndex &t, TopoTriIndex &n1);
   inline void nbr2(TopoTriIndex &t, TopoTriIndex &n2);
   inline void nbr3(TopoTriIndex &t, TopoTriIndex &n3);

   NxU16 *getVerts(const Coord2d &p1,
                                  const Coord2d &p2,
                                  const Coord2d &p3,
                                  NxU16 *indices);

// ********************************************
// Found in EMIT.CPP
// ********************************************
   NxU16 * emitSurface(NxU16 *indices);

   NxU16 * emitDFS(TopoTriIndex &t,
                                  NxU16 *indices,
                                  const Coord2d& p1,
                                  const Coord2d& p2,
                                  const Coord2d& p3,
                                  IntersectTest vstate);

   NxI32 inROD(Coord2d p1, Coord2d p2, Coord2d p3);

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
   NxI32    calcErr(Coord2d p1, Coord2d p2, Coord2d p3);
   void   midpoint(Coord2d p1,Coord2d p2,Coord2d m);
   void   MidPoint(const Coord2d &p1,const Coord2d &p2,Coord2d &m);
   NxI32    ceilingLog2(NxI32 x);
   NxI32    next2Power(NxI32 x);
   void   makeDFS(TopoTriIndex &t, Coord2d p1, Coord2d p2, Coord2d p3);
   void   initHier(void);

// These were macros, but have now been made methods.
   NxI32 BORDER(TopoTriIndex &t)
  {
    return (mInfo[t.s] & PARTINROD);
  };

   NxI32 INROD(TopoTriIndex &t)
  {
    return (mInfo[t.s] & ALLINROD);
  };

   void PUTONSURFACE(TopoTriIndex &t)
  {
    mInfo[t.s] = (mInfo[t.s]&(ALLINROD|PARTINROD))|SURFACE;
  };

   NxI32 ONSURFACE(TopoTriIndex &t)
  {
    return (mInfo[t.s] & SURFACE);
  };

  NxI32 ONSURFACE2(TopoTriIndex &t)
  {
    return (mInfo[t.s] & SURFACE);
  };

   bool SMALLN1(TopoTriIndex &t)
  {
    return (mInfo[t.s] & SMALLER1);
  };

   NxI32 SMALLN2(TopoTriIndex &t)
  {
    return (mInfo[t.s] & SMALLER2);
  };

   NxI32 BIGN3(TopoTriIndex &t)
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
	bool IsEdge(NxI32 x,NxI32 y);

// ********************************************
// * Misc. data variables.
// ********************************************
  Coord2d se, sw, ne, nw;

  NxI32 xmax[64];
  NxI32 ymax[64];
  NxI32 ntriabove[64];

  NxI32 hierDepth;

  Coord2d eyeP;

  NxF32 x_per_y;
  NxF32 y_per_x;
  NxF32 errPerEm;

  TopoTriIndex mRootL;
  TopoTriIndex mRootR;
  Coord2d  mSE;
  Coord2d  mSW;
  Coord2d  mNE;
  Coord2d  mNW;

  const HeightData *mField;

  NxI32 mBlockLeft;
  NxI32 mBlockTop;
  NxI32 mBlockRight;
  NxI32 mBlockBottom;

  NxI32 mFieldSize;  // size of entire height field.
// was in hier
  NxI32 mMaxDepth;			      /* maximum depth */
  NxI32 mSize;			        /* number of triangles in hierarchy */
  NxU16 *mErr;	/* error of triangles */
  NxU8 *mInfo;	/* surface,neighbor */
  bool mDoLod;
  NxI32  mErrorThreshold;
  NxF32 mFloatThreshold;
  NxI32  mDistanceThreshold;
  bool mEdges; // full resolution edges?
  bool mIgnoreSealevel;
  NxI32 mMinDepth;
  bool mHasSea;
  NxF32 mSealevel;
  ErrorHeuristic *mError;
  NxI32 mBlockSize;
  NxI32 mBlockLen;
};

extern Coord2d se, sw, ne, nw;

#endif
