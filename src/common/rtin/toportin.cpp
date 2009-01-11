#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "toportin.h"
#include "errorh.h"
#include "common/snippets/UserMemAlloc.h"

TopoRtin::TopoRtin(const HeightData *hfield)
{
  mError = 0;
  mSealevel = 0.0f;
  mEdges = true;
  mIgnoreSealevel = true;
  mDoLod = false; // by default don't scale by distance from camera!
  mErrorThreshold = 14; // default error threshold.
  mFloatThreshold = 14;
  mDistanceThreshold = 8; // default distance threshold.

  mField = hfield;
  HeI32 wid = mField->GetWidth();
  HeI32 hit = mField->GetHeight();
  mFieldSize = wid;
  if ( hit > wid ) mFieldSize = hit;
  initHier();
};

/**********************************************************************/
/*******START OF    ADDRESS.CPP ***************************************/
/**********************************************************************/

/* SOUTH,WEST,NORTH,EAST,NW,SE,NE,SW */
static HeI32 lorient[8] = {SE,SW,NW,NE,WEST,EAST,NORTH,SOUTH};
static HeI32 rorient[8] = {SW,NW,NE,SE,NORTH,SOUTH,EAST,WEST};
static HeI32 b1orient[8] = {EAST,SOUTH,WEST,NORTH,SW,NE,NW,SE};
static HeI32 b2orient[8] = {WEST,NORTH,EAST,SOUTH,NE,SW,SE,NW};
static HeI32 b3orient[8] = {NORTH,EAST,SOUTH,WEST,SE,NW,SW,NE};

/* NW,SE,NE,SW (parity of x determines row) */
static HeI32 porient1[8] = {WEST,SOUTH,NORTH,WEST,NORTH,EAST,EAST,SOUTH};
/* SOUTH,WEST,NORTH,EAST (parity of x+y determines row) */
static HeI32 porient0[8] = {SE,NW,NW,SE,SW,SW,NE,NE};

/* SOUTH,WEST,NORTH,EAST,NW,SE,NE,SW */
static HeI32 lxinc[8]  = {0,0,1,1,0,0,0,0};
static HeI32 lyinc[8]  = {0,1,1,0,0,0,0,0};
static HeI32 rxinc[8]  = {1,0,0,1,0,0,0,0};
static HeI32 ryinc[8]  = {0,0,1,1,0,0,0,0};
static HeI32 b1xinc[8] = {0,0,0,0,0,0,+1,-1};
static HeI32 b1yinc[8] = {0,0,0,0,+1,-1,0,0};
static HeI32 b2xinc[8] = {0,0,0,0,-1,+1,0,0};
static HeI32 b2yinc[8] = {0,0,0,0,0,0,+1,-1};
static HeI32 b3xinc[8] = {0,-1,0,+1,0,0,0,0};
static HeI32 b3yinc[8] = {-1,0,+1,0,0,0,0,0};

#define S1(t) 2*(t.y*xmax[t.depth]+t.y+t.x)+(t.orient&1)+ntriabove[t.depth]
#define S0(t) 4*(t.y*xmax[t.depth]+t.y+t.x)+t.orient+ntriabove[t.depth]

void TopoRtin::leftChild(TopoTriIndex &t, TopoTriIndex &lt)
{
  lt.depth = t.depth+1;
  lt.orient = lorient[t.orient];
  if (t.depth&1)
  {
    lt.x = t.x;
    lt.y = t.y;
    lt.s = S0(lt);
  }
  else
  {
    lt.x = t.x<<1 | lxinc[t.orient];
    lt.y = t.y<<1 | lyinc[t.orient];
    if (lt.x > xmax[lt.depth] || lt.y > ymax[lt.depth])
      lt.s = 0;
    else
      lt.s = S1(lt);
  }
}

void TopoRtin::rightChild(TopoTriIndex &t, TopoTriIndex &rt)
{
  rt.depth = t.depth+1;
  rt.orient = rorient[t.orient];
  if (t.depth&1)
  {
    rt.x = t.x;
    rt.y = t.y;
    rt.s = S0(rt);
  }
  else
  {
    rt.x = t.x<<1 | rxinc[t.orient];
    rt.y = t.y<<1 | ryinc[t.orient];
    if (rt.x > xmax[rt.depth] || rt.y > ymax[rt.depth])
      rt.s = 0;
    else
      rt.s = S1(rt);
  }
}

void TopoRtin::parent(TopoTriIndex &t, TopoTriIndex &p)
{
  p.depth = t.depth-1;
  if (t.depth&1)
  {
    p.orient = porient1[((t.x&1)<<2)|(t.orient&3)];
    p.x = t.x>>1;
    p.y = t.y>>1;
    p.s = S0(p);
  }
  else
  {
    p.orient = porient0[(((t.x^t.y)&1)<<2)|(t.orient&3)];
    p.x = t.x;
    p.y = t.y;
    p.s = S1(p);
  }
}

void TopoRtin::bro1(TopoTriIndex &t, TopoTriIndex &b1)
{
  b1.depth = t.depth;
  b1.x = t.x + b1xinc[t.orient];
  b1.y = t.y + b1yinc[t.orient];
  b1.orient = b1orient[t.orient];
  if (b1.x > xmax[t.depth] || b1.y > ymax[t.depth] || b1.x < 0 || b1.y < 0)
    b1.s = 0;			/* set b1 to be OUTOFBOUNDS */
  else
  {
    if (t.depth&1)
      b1.s = S1(b1);
    else
      b1.s = S0(b1);
  }
}

void TopoRtin::bro2(TopoTriIndex &t, TopoTriIndex &b2)
{
  b2.depth = t.depth;
  b2.x = t.x + b2xinc[t.orient];
  b2.y = t.y + b2yinc[t.orient];
  b2.orient = b2orient[t.orient];
  if (b2.x > xmax[t.depth] || b2.y > ymax[t.depth] || b2.x < 0 || b2.y < 0)
    b2.s = 0;			/* set b2 to be OUTOFBOUNDS */
  else
  {
    if (t.depth&1)
      b2.s = S1(b2);
    else
      b2.s = S0(b2);
  }
}

void TopoRtin::bro3(TopoTriIndex &t, TopoTriIndex &b3)
{
  b3.x = t.x+b3xinc[t.orient];
  b3.y = t.y+b3yinc[t.orient];
  b3.orient = b3orient[t.orient];
  b3.depth = t.depth;
  if (b3.x > xmax[t.depth] || b3.y > ymax[t.depth] || b3.x < 0 || b3.y < 0)
    b3.s = 0;			/* set b3 to be OUTOFBOUNDS */
  else
  {
    if (t.depth&1)
      b3.s = S1(b3);
    else
      b3.s = S0(b3);
  }
}


void TopoRtin::nbr1(TopoTriIndex &t, TopoTriIndex &n1)
{
  TopoTriIndex b1;

  bro1(t,b1);

  if (b1.s && SMALLN1(t))
  {
    leftChild(b1,n1);
  }
  else
  {
    TRIEQUATE(n1,b1);
  }
}

void TopoRtin::nbr2(TopoTriIndex &t, TopoTriIndex &n2)
{
  TopoTriIndex b2;

  bro2(t,b2);

  if (b2.s && SMALLN2(t))
  {
    rightChild(b2,n2);
  }
  else
  {
    TRIEQUATE(n2,b2);
  }
}

void TopoRtin::nbr3(TopoTriIndex &t, TopoTriIndex &n3)
{
  TopoTriIndex b3;

  bro3(t,b3);

  if (b3.s && BIGN3(t))
  {
    parent(b3,n3);
  }
  else
  {
    TRIEQUATE(n3,b3);
  }
}


HeI32 TopoRtin::init_ntriabove(HeI32 depth)
/* initialize ntriabove[] and return size of needed physical address space */
{
  HeI32 d,xm,ym;

  /* initialize special Tri's */
  TopoTriIndex& rootL = mRootL;
  TopoTriIndex& rootR = mRootR;

  rootL.x=0; rootL.y=0; rootL.depth=1; rootL.orient=NW; rootL.s=1;
  rootR.x=0; rootR.y=0; rootR.depth=1; rootR.orient=SE; rootR.s=2;

  xm = mFieldSize-2;	/* count squares (starting at (0,0)) */
  ym = mFieldSize-2;

  d = hierDepth = depth;
  while (d >= 1)
  {
    xmax[d] = xm;
    ymax[d] = ym;
    xm >>= 1;
    ym >>= 1;
    d--;
    xmax[d] = xm;
    ymax[d] = ym;
    d--;
  }

  ntriabove[0] = 0;
  ntriabove[1] = 1;
  d = 1;
  while(d <= depth)
  {
    /* odd depth: NW,SW,NE,SE triangles */
    ntriabove[d+1]=2*(xmax[d]+1)*(ymax[d]+1)+ntriabove[d];
    d++;
    if (d>depth) break;
    /* even depth: N,S,E,W triangles */
    ntriabove[d+1] = 4*(xmax[d]+1)*(ymax[d]+1)+ntriabove[d];
    d++;
  }
  return(ntriabove[depth+1]+1);/* loop went far enough to calc. this */
}

/**********************************************************************/
/*********END OF    ADDRESS.CPP ***************************************/
/**********************************************************************/

/**********************************************************************/
/*************START OF EMIT.CPP ***************************************/
/**********************************************************************/
static HeI32 surfType;		/* surface type: WIRE2D, WIRE3D, or FILL3D */


/*  emitSurface( type) -- draw surface from triangle hierarchy  */
HeU16 * TopoRtin::emitSurface(HeU16 *indices)
{
  indices = emitDFS( mRootL, indices, sw, ne, nw, IT_PARTIAL);	/* NW corner of square */
  indices = emitDFS( mRootR, indices, ne, sw, se, IT_PARTIAL);	/* SE corner of square */
  return indices;
}



/*  emitDFS( t, p1, p2, p3) -- emit triangle, possibly subdivided  */
HeU16 * TopoRtin::emitDFS(TopoTriIndex &t,
                               HeU16 *indices,
                               const Coord2d &p1,
                               const Coord2d &p2,
                               const Coord2d &p3,
                               IntersectTest vstate)
{
  TopoTriIndex lt,rt;

  if ( t.s )
  {
    if ( vstate == IT_PARTIAL )
    {
      vstate = InBlock(p1,p2,p3);
    }

    if ( vstate == IT_OUTSIDE ) return indices;

    if ( ONSURFACE2(t))
    {
      if ( vstate == IT_INSIDE ) indices = getVerts(p1,p2,p3,indices);
    }
    else
    {
	    Coord2d mid;
      MidPoint(p1, p2, mid);

      leftChild(t,lt);
      rightChild(t,rt);

      indices = emitDFS( lt, indices, p3, p1, mid,vstate);
      indices = emitDFS( rt, indices, p2, p3, mid,vstate);
    }

  }

  return indices;
}

/**********************************************************************/
/*************END   OF EMIT.CPP ***************************************/
/**********************************************************************/


/*  inROD(p1, p2, p3)  */
HeI32 TopoRtin::inROD(Coord2d p1, Coord2d p2, Coord2d p3)
{
  HeI32 r;
  HeI32 txmax = MAX(p1[0], MAX(p2[0], p3[0]));
  HeI32 txmin = MIN(p1[0], MIN(p2[0], p3[0]));
  HeI32 tymax = MAX(p1[1], MAX(p2[1], p3[1]));
  HeI32 tymin = MIN(p1[1], MIN(p2[1], p3[1]));

  if (txmax > mFieldSize - 1)
  {
    if (txmin >= mFieldSize - 1)
    {
      r = 0;
    }
    else
    {
      r = PARTINROD;
    }
  }
  else
  {
    if (tymax > mFieldSize - 1)
    {
      if (tymin >= mFieldSize - 1)
      {
        r = 0;
      }
      else
      {
        r = PARTINROD;
      }
    }
    else
    {
      r = ALLINROD;
    }
  }


      r = ALLINROD;

  return r;
}

/**********************************************************************/
/*************START OF SPLIT.CPP **************************************/
/**********************************************************************/
/* Triangle t is split into lt and rt.  Neighbors 1 & 2 of lt and
 * rt are known to be the same size as lt and rt.  Neighbor 3 of
 * lt is neighbor 2 of t (n3lt == n2t).  If n2t is smaller than
 * t then n3lt is same size as lt, o.w. n3lt is bigger than lt (and
 * n1n2t == lt is smaller than n2t == n3lt).
 * Similar gibberish holds for rt.
 */
void TopoRtin::kidsOnSurf(TopoTriIndex &t)
/* Push the surface down to the children of t.  t must not be a leaf. */
{
  TopoTriIndex lt, rt, n2t, n1t;

  assert(!ISLEAF(t));
  assert(t.s);
  assert(ONSURFACE(t));

  mInfo[t.s]&=~SURFACE;		/* take t from surface */

  leftChild(t,lt);			/* put t's children on surface */

  if (lt.s) PUTONSURFACE(lt);

  rightChild(t,rt);

  if (rt.s) PUTONSURFACE(rt);

  /* Let t's neighbors know they should point to t's children */

  nbr2(t,n2t);

  if (n2t.s && lt.s)
  {

    assert(ONSURFACE(n2t));

    if (SMALLN2(t))
    {		/* n2t is same size as lt */
      mInfo[n2t.s]&=~BIGGER3;
    }
    else
    {				/* n2t is bigger than lt */
      mInfo[n2t.s] |= SMALLER1;
      mInfo[lt.s] |= BIGGER3;
    }
  }

  nbr1(t,n1t);

  if (n1t.s && rt.s)
  {
    assert(ONSURFACE(n1t));
    if (SMALLN1(t))
    {		/* n1t is same size as rt */
      mInfo[n1t.s]&=~BIGGER3;
    }
    else
    {				/* n1t is bigger than lt */
      mInfo[n1t.s]|=SMALLER2;
      mInfo[rt.s]|=BIGGER3;
    }
  }

}


void TopoRtin::split(TopoTriIndex &t)
{
  TopoTriIndex n3t,tmp;

  assert(t.s);
  assert(ONSURFACE(t));

  nbr3(t,n3t);

  if (n3t.s == 0) goto N3INVALID;

  if (BIGN3(t))
  {
    split( n3t);	/* propagate split: t's 3-neighbor might change */
    if (IAMLEFTCHILD(t))
    {
      rightChild(n3t,tmp);
    }
    else
    {
      leftChild(n3t,tmp);
    }

    if (tmp.s == 0) goto N3INVALID;

    TRIEQUATE(n3t,tmp);
  }

  kidsOnSurf( n3t);		/* t is not LEAF so n3t is not LEAF */

N3INVALID:
  kidsOnSurf(t);
}

/**********************************************************************/
/*************END OF   SPLIT.CPP **************************************/
/**********************************************************************/

/**********************************************************************/
/*************START OF SURFACE.CPP ************************************/
/**********************************************************************/
#define LEFT_TURN(a, b, c) (((b)[1]-(a)[1])*((c)[0]-(a)[0])-((b)[0]-(a)[0])*((c)[1]-(a)[1]) < 0)

void TopoRtin::rodSplit(TopoTriIndex &t)
{
  TopoTriIndex lt,rt;

  if (t.s && !ISLEAF( t) && BORDER(t))
  {

    if (ONSURFACE(t))
    {
      split(t);
    }

    leftChild(t,lt);
    rightChild(t,rt);
    rodSplit( lt);
    rodSplit( rt);
  }

}


void TopoRtin::eyeSplit(TopoTriIndex &t, Coord2d p1, Coord2d p2, Coord2d p3)
{
  Coord2d mid;
  TopoTriIndex lt,rt;

  if ( t.s && !ISLEAF( t))
  {
    if (ONSURFACE(t))
    {
      split( t);
    }

    midpoint(p1, p2, mid);
    leftChild(t,lt);
    rightChild(t,rt);

    if (LEFT_TURN(mid, p3, eyeP))
    {
      eyeSplit( lt, p3, p1, mid);
      rodSplit( rt);
    }
    else
    {
      eyeSplit( rt, p2, p3, mid);
      rodSplit( lt);
    }

  }
}

/* baseSurface --
 * calculates surface with finest level of detail around eye position
 * and no triangles spanning border of rectangle of definition.
 */
void TopoRtin::baseSurface(void)
{

  /* SURFACE starts as triangles 1 and 2 */
  PUTONSURFACE( mRootL);
  PUTONSURFACE( mRootR);

  if (LEFT_TURN(sw, ne, eyeP))
  {
    eyeSplit( mRootL, sw, ne, nw);	/* NW corner of square */
    rodSplit( mRootR);
  }
  else
  {
    eyeSplit( mRootR, ne, sw, se);	/* SE corner of square */
    rodSplit( mRootL);
  }

}

HeF32 Dist2(Coord2d p1,Coord2d p2)
{
  HeF32 dx = HeF32( p1[0] - p2[0] );
  HeF32 dy = HeF32( p1[1] - p2[1] );
  return dx*dx + dy*dy;
}

#define LONGEST_EDGE (128*128) //we support up to 32 meter long edges

/*-------------------ERROR SURFACE-----------------------------*/
bool TopoRtin::tooCoarse(TopoTriIndex &t,
                    Coord2d p1,
                    Coord2d p2,
                   	Coord2d p3)
{

   {
     HeF32 d1 = Dist2(p1,p2);
     if ( d1 > LONGEST_EDGE ) return true;

     HeF32 d2 = Dist2(p2,p3);
     if ( d2 > LONGEST_EDGE ) return true;

     HeF32 d3 = Dist2(p3,p1);
     if ( d3 > LONGEST_EDGE ) return true;
  }

  HeF32 d = 20; // default constant lod viewing distance.

  /* Check error threshold */
  HeI32 h = mErr[t.s];

  HeF32 err = mFloatThreshold;

  if ( mError )
  {

    HeF32 s1 = mError->Get( p1[0], p1[1] );
    HeF32 s2 = mError->Get( p2[0], p2[1] );
    HeF32 s3 = mError->Get( p3[0], p3[2] );

    HeF32 scale = (s1+s2+s3)*0.333f;

    err*=scale;
  }

  HeI32 allow = HeI32(d*err);

  if ( h > allow )  return true;

  return false;
}


void TopoRtin::errSplit(TopoTriIndex &t, Coord2d p1, Coord2d p2, Coord2d p3)
{
  TopoTriIndex lt,rt;
  Coord2d mid;

  if (t.s && tooCoarse( t, p1, p2, p3))
  {
    if ( ONSURFACE( t) ) split( t);
    midpoint(p1, p2, mid);
    leftChild(t,lt);
    rightChild(t,rt);

    errSplit(lt,p3,p1,mid);
    errSplit(rt,p2,p3,mid);

  }

}


void TopoRtin::errSurface(void)
{
  baseSurface();
  errSplit(mRootL,sw,ne,nw);
  errSplit(mRootR,ne,sw,se);
}

/**********************************************************************/
/*************END   OF SURFACE.CPP ************************************/
/**********************************************************************/

/**********************************************************************/
/*************START OF TRI.CPP     ************************************/
/**********************************************************************/

HeI32 TopoRtin::calcErr(Coord2d p1, Coord2d p2, Coord2d p3)
{
  HeI32 x1, x2, x3, y1, y2, y3;
  HeF32 z1, z2, z3;
  HeF32 ystart, yend, xstart, xend;
  HeF32 deltaxstart, deltaxend, deltaystart, deltayend;
  HeI32 xmini, xmaxi, ymini, ymaxi;
  HeF32 z, zstart, zend, deltazstart, deltazend, deltaz;
  HeF32 max_eps, eps;
  HeI32 x, y;
  HeI32 xstarti, xendi, ystarti, yendi;

  x1 = p1[0];
  y1 = p1[1];
  z1 = GetZ(x1, y1);

  x2 = p2[0];
  y2 = p2[1];
  z2 = GetZ(x2, y2);

  x3 = p3[0];
  y3 = p3[1];
  z3 = GetZ(x3, y3);

  assert(x1 <= mFieldSize );
  assert(x2 <= mFieldSize );
  assert(x3 <= mFieldSize );
  assert(y1 <= mFieldSize );
  assert(y2 <= mFieldSize );
  assert(y3 <= mFieldSize );

  max_eps = 0.0;

  if (y1 == y2)
  {
    if (x1 > x2)
  	{
      xmini = x2;
      xmaxi = x1;
      ymini = y3;
      ymaxi = y1;

      xstart = HeF32(x3);
      xend = xstart;
      deltaxstart = -x_per_y;
      deltaxend = x_per_y;
      zstart = z3;
      zend = z3;
      deltazstart = (z2 - zstart) / (HeF32)(y1 - y3);
      deltazend = (z1 - zend) / (HeF32)(y1 - y3);
    }
    else
    {
      xmini = x1;
      xmaxi = x2;
      ymini = y1;
      ymaxi = y3;

      xstart = HeF32(x1);
      xend = HeF32(x2);
      deltaxstart = x_per_y;
      deltaxend = -x_per_y;
      zstart = z1;
      zend = z2;
      deltazstart = (z3 - zstart) / (HeF32)(y3 - y1);
      deltazend = (z3 - zend) / (HeF32)(y3 - y1);
    }

    for (y = ymini; y <= ymaxi; ++y)
    {
      xstarti = MAX(xmini, ROUND(xstart));
      xendi = MIN(xmaxi, ROUND(xend));
      z = zstart;
      if (xstarti != xendi)
      {
      	deltaz = (zend - zstart) / (xendi - xstarti);
      }
      else
      {
      	deltaz = 0.0;
      }
      for (x = xstarti; x <= xendi; ++x)
      {
      	eps = ABS(GetZ(x, y) - z);
      	if (eps > max_eps)
    	  {
      	  max_eps = eps;
      	}
       	z = z + deltaz;
      }
      zstart = zstart + deltazstart;
      zend = zend + deltazend;
      xstart = xstart + deltaxstart;
      xend = xend + deltaxend;
    }
  }
  else
  {
    if (x1 == x2)
    {
      if (y1 > y2)
      {
    	  xmini = x1;
      	xmaxi = x3;
      	ymini = y2;
      	ymaxi = y1;

      	ystart = HeF32(y2);
      	yend = HeF32(y1);
      	deltaystart = y_per_x;
      	deltayend = -y_per_x;
      	zstart = z2;
      	zend = z1;
      	deltazstart = (z3 - zstart) / (HeF32)(x3 - x1);
      	deltazend = (z3 - zend) / (HeF32)(x3 - x1);
      }
      else
      {
  	    xmini = x3;
	      xmaxi = x1;
	      ymini = y1;
  	    ymaxi = y2;

	      ystart = HeF32(y3);
	      yend = ystart;
	      deltaystart = -y_per_x;
	      deltayend = y_per_x;
	      zstart = z3;
	      zend = z3;
	      deltazstart = (z1 - zstart) / (HeF32)(x1 - x3);
	      deltazend = (z2 - zend) / (HeF32)(x1 - x3);
      }
    }
    else
    {
      if (x1 < x2)
      {
	      if (y1 > y2)
        {
	        xmini = x1;
      	  xmaxi = x2;
      	  ymini = y2;
      	  ymaxi = y3;

      	  ystart = HeF32(y1);
      	  yend = ystart;
      	  deltaystart = -y_per_x;
      	  deltayend = 0.0;
      	  zstart = z1;
      	  zend = z1;
      	  deltazstart = (z2 - zstart) / (HeF32)(x3 - x1);
      	  deltazend = (z3 - zend) / (HeF32)(x3 - x1);
      	}
      	else
     	  {
	        xmini = x1;
      	  xmaxi = x2;
      	  ymini = y1;
      	  ymaxi = y3;

      	  ystart = HeF32(y1);
      	  yend = HeF32(y3);
      	  deltaystart = y_per_x;
      	  deltayend = 0.0;
       	  zstart = z1;
      	  zend = z3;
      	  deltazstart = (z2 - zstart) / (HeF32)(x2 - x3);
      	  deltazend = (z2 - zend) / (HeF32)(x2 - x3);
      	}
      }
      else
      {
      	if (y1 > y2)
     	  {
      	  xmini = x2;
      	  xmaxi = x3;
      	  ymini = y3;
      	  ymaxi = y1;

      	  ystart = HeF32(y2);
      	  yend = ystart;
      	  deltaystart = 0.0;
      	  deltayend = y_per_x;
      	  zstart = z2;
      	  zend = z2;
      	  deltazstart = (z3 - zstart) / (HeF32)(x3 - x2);
      	  deltazend = (z1 - zend) / (HeF32)(x3 - x2);
      	}
      	else
     	  {
   	      xmini = x3;
       	  xmaxi = x1;
      	  ymini = y3;
      	  ymaxi = y2;

      	  ystart = HeF32(y3);
      	  yend = HeF32(y2);
      	  deltaystart = 0.0;
      	  deltayend = -y_per_x;
      	  zstart = z3;
      	  zend = z2;
      	  deltazstart = (z1 - zstart) / (HeF32)(x1 - x3);
      	  deltazend = (z1 - zend) / (HeF32)(x1 - x3);
      	}
      }
    }
    for (x = xmini; x <= xmaxi; ++x)
    {
      ystarti = MAX(ymini, ROUND(ystart));
      yendi   = MIN(ymaxi, ROUND(yend));
      z = zstart;

      if (ystarti != yendi)
      {
      	deltaz = (zend - zstart) / (yendi - ystarti);
      }
      else
      {
      	deltaz = 0.0;
      }

      for (y = ystarti; y <= yendi; ++y)
      {
	      eps = ABS(GetZ(x, y) - z);
      	if (eps > max_eps)
     	  {
      	  max_eps = eps;
      	}
      	z = z + deltaz;
      }
      zstart = zstart + deltazstart;
      zend = zend + deltazend;
      ystart = ystart + deltaystart;
      yend = yend + deltayend;
    }
  }

  return ROUND(max_eps * errPerEm);
}


/*  midpoint(p1, p2, m) -- set m to midpoint of segment between p1 and p2  */
void TopoRtin::midpoint(Coord2d p1,Coord2d p2, Coord2d m)
{
  m[0] = (p1[0] + p2[0]) / 2;
  m[1] = (p1[1] + p2[1]) / 2;
}

void TopoRtin::MidPoint(const Coord2d &p1,const Coord2d &p2, Coord2d &m)
{
  m[0] = (p1[0] + p2[0]) / 2;
  m[1] = (p1[1] + p2[1]) / 2;
}

HeI32 TopoRtin::ceilingLog2(HeI32 x)
{
  HeI32 m = 1;
  HeI32 i = 0;

  while (m < x && m < INT_MAX)
  {
    m <<= 1;
    ++i;
  }
  return i;
}

HeI32 TopoRtin::next2Power(HeI32 x)
{
  HeI32 m = 1;
  while (m < x && m < INT_MAX)
    m <<= 1;
  return m;
}

void TopoRtin::makeDFS(TopoTriIndex &t, Coord2d p1, Coord2d p2, Coord2d p3)
{
  TopoTriIndex lt,rt;
  Coord2d mid;
  HeI32 r;

  r = inROD(p1, p2, p3);

  if (t.s)
  {

    mInfo[t.s] = (HeU8)r;

    if (ISLEAF(t))
	  {
      mErr[t.s] = 0;
    }
  	else
	  {

      HeI32 err = calcErr(p1, p2, p3);

      mErr[t.s] = (HeU16)err;

      midpoint(p1, p2, mid);
      leftChild(t,lt);
      rightChild(t,rt);
      makeDFS( lt, p3, p1, mid);
      makeDFS( rt, p2, p3, mid);
    }
  }
}



/*  initHier() -- initialize triangle hierarchy  */
void TopoRtin::initHier(void)
{
	HeI32 dim;

	x_per_y = 1;
	y_per_x = 1;

	HeF32 dz = 1;

	{
		HeF32 minz = 1e9;
		HeF32 maxz = -1e9;

		for ( HeI32 y=0; y<mFieldSize; y++ )
		{
	  		for ( HeI32 x=0; x<mFieldSize; x++ )
	  		{
	  			HeF32 z = GetZ(x,y);
	  			if ( z < minz ) 
					minz = z;
	  			if ( z > maxz ) 
					maxz = z;
	  		}
		}
		dz = maxz - minz;
	}

	if ( fabs(dz) <= 0.000001f )
		dz = 1.0f;

	errPerEm = (HeF32)MAXERR / dz;

	dim = next2Power(MAX(mFieldSize, mFieldSize ) - 1) + 1;
	mMaxDepth = 2*ceilingLog2(dim-1) + 1;
	mSize = init_ntriabove(mMaxDepth);

	HeI32 wid = dim-1;

	if ( wid <= 64 )
	{
		mMinDepth = 0;
	}
	else
	{
		switch ( wid )
		{
  			case 128:
				mMinDepth = 4;
			break;
			
			case 256:
				mMinDepth = 6;
			break;
      
			case 512:
				mMinDepth = 8;
			break;
      
			case 1024:
				mMinDepth = 10;
			break;
      
			case 2048:
				mMinDepth = 12;
			break;
		
			case 4096:
				mMinDepth = 14;
			break;

			case 8192:
				mMinDepth = 16;
			break;
		}
	}

	mErr    = MEMALLOC_NEW_ARRAY(HeU16,mSize)[mSize];
	mInfo   = MEMALLOC_NEW_ARRAY(HeU8,mSize)[mSize];

	sw[0] = 0;
	sw[1] = 0;

	se[0] = dim - 1;
	se[1] = 0;

	ne[0] = dim - 1;
	ne[1] = dim - 1;

	nw[0] = 0;
	nw[1] = dim - 1;

	printf("Building triangle network for northwest corner bintree.\n");
	makeDFS( mRootL, sw, ne, nw);	/* NW corner of square */
	printf("Building triangle network for southeast corner bintree.\n");
	makeDFS( mRootR, ne, sw, se);	/* SE corner of square */
	mErr[0] = MAXERR;
	printf("Triangle network complete.\n");

	mInfo[0] = 0;
}

/**********************************************************************/
/*************END OF   TRI.CPP     ************************************/
/**********************************************************************/
HeU16 * TopoRtin::getVerts(const Coord2d &p1,
                                const Coord2d &p2,
                                const Coord2d &p3,
                                HeU16 *indices)
{
  HeI32 i1 = (p3[1]-mBlockTop)*mBlockLen+(p3[0]-mBlockLeft);
  HeI32 i2 = (p2[1]-mBlockTop)*mBlockLen+(p2[0]-mBlockLeft);
  HeI32 i3 = (p1[1]-mBlockTop)*mBlockLen+(p1[0]-mBlockLeft);

  assert( i1 >= 0 && i1 < ( mBlockLen*mBlockLen ) );
  assert( i2 >= 0 && i2 < ( mBlockLen*mBlockLen ) );
  assert( i3 >= 0 && i3 < ( mBlockLen*mBlockLen ) );

  if ( mIgnoreSealevel )
  {
    // check to see if it's at sea level!
    HeF32 z1 = GetZ( p1[0], p1[1] );
    HeF32 z2 = GetZ( p2[0], p2[1] );
    HeF32 z3 = GetZ( p3[0], p3[1] );

    if ( z1 <= mSealevel && z2 <= mSealevel && z3 <= mSealevel )
    {
      mHasSea = true;
      return indices;
    }
  }

  *indices++ = (HeU16)i1;
  *indices++ = (HeU16)i2;
  *indices++ = (HeU16)i3;

  return indices;
}

void TopoRtin::SetEye(HeI32 eyex,HeI32 eyey)
{
  eyeP[0] = eyex;
  eyeP[1] = eyey;
  errSurface(); // compute active rtin based on this error metric.
}

HeU16 * TopoRtin::BuildIndices(HeU16 *indices,
                                    HeI32 left,
                                    HeI32 top)
{
  mHasSea = false;
  mBlockLeft   = left;
  mBlockTop    = top;
  mBlockRight  = left+mBlockSize;
  mBlockBottom = top+mBlockSize;
  HeU16 *istart = indices;
  indices       = emitSurface(indices);
  if ( indices == istart ) return 0;
  return indices;
}

IntersectTest TopoRtin::InBlock(const Coord2d &p1,const Coord2d &p2,const Coord2d &p3)
{
  Coord2d min;
  Coord2d max;

  max[0] = min[0] = p1[0];
  max[1] = min[1] = p1[1];

  if ( p2[0] < min[0] ) min[0] = p2[0];
  if ( p2[1] < min[1] ) min[1] = p2[1];

  if ( p3[0] < min[0] ) min[0] = p3[0];
  if ( p3[1] < min[1] ) min[1] = p3[1];

  if ( p2[0] > max[0] ) max[0] = p2[0];
  if ( p2[1] > max[1] ) max[1] = p2[1];

  if ( p3[0] > max[0] ) max[0] = p3[0];
  if ( p3[1] > max[1] ) max[1] = p3[1];

  if ( max[0] < mBlockLeft ) return   IT_OUTSIDE;
  if ( max[1] < mBlockTop  ) return   IT_OUTSIDE;
  if ( min[0] > mBlockRight ) return  IT_OUTSIDE;
  if ( min[1] > mBlockBottom ) return IT_OUTSIDE;

  if ( min[0] >= mBlockLeft && max[0] <= mBlockRight &&
       min[1] >= mBlockTop  && max[1] <= mBlockBottom )
  {
    return IT_INSIDE;
  }
  return IT_PARTIAL;
}

bool TopoRtin::IsEdge(Coord2d p)
{
  if ( p[0] == 0 ) return true;
  if ( p[1] == 0 ) return true;

  if ( p[0] == mFieldSize ) return true;
  if ( p[1] == mFieldSize ) return true;

  return false;
}



bool TopoRtin::IsEdge(HeI32 x,HeI32 y)
{
  if ( x == 0 ) return true;
  if ( y == 0 ) return true;

  if ( x == (mFieldSize-1) ) return true;
  if ( y == (mFieldSize-1) ) return true;

  return false;
}
