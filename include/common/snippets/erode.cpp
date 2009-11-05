#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>                     /* strtod() rand() */
#include <math.h>                       /* math stuff */
#include <time.h>                       /* time() */


#include "erode.h"
#include "ImportHeightMap.h"
#include "UserMemAlloc.h"

using namespace NVSHARE;

#pragma warning(disable:4996)

#pragma warning(disable:4996)
#pragma warning(disable:4018)
#pragma warning(disable:4100)
#pragma warning(disable:4101)
#pragma warning(disable:4189)
#pragma warning(disable:4244)

namespace ERODE
{

#define PCONST 0.6366197723     /* constant value 2/pi */

const NxF32 recip_scale = 1.0f / 1.414f;

NxI32 xo[9] =                      /* 8-dir index offset arrays */
   { 0,-1,0,1,1,1,0,-1,-1 };
NxI32 yo[9] =
   { 0,-1,-1,-1,0,1,1,1,0 };
NxI32 x4[4] =                      /* 4-dir index offset arr */
   { -1,0,1,0 };
NxI32 y4[4] =
   { 0,-1,0,1 };
NxI32 dhash[9] =                /* 8-direction hash mx */
  { 1,2,3,8,0,4,7,6,5 };


NxF64 sgn(NxF64 a)
{
  if (a > 0)
   return (1);
  else if (a < 0)
   return (-1);
  else
   return (0);
}

NxF64 sgn1(NxF64 a)
{
 NxF64 sign = 0;

  if (a > 0.0)
  {
   sign = 1.0;
  }
  else if (a < 0)
  {
   sign = -1.0;
  }
  return(a);
}


template< class Type> class HeightMap
{
public:
  HeightMap(void)
  {
    mWidth = 0;
    mDepth = 0;
    mData = 0;
    mScanLines = 0;
  }

  void init(NxI32 width,NxI32 depth,const Type *data)
  {
    mWidth = width;
    mDepth = depth;
    mMinV = 1e9;
    mMaxV = -1e9;
    NxI32 size = width*depth;
    mData = (Type *)MEMALLOC_MALLOC(size*sizeof(Type));
    memset(mData,0,sizeof(Type)*size);
    Type *scan = mData;
    mScanLines = (Type **)MEMALLOC_MALLOC( sizeof(Type*)*depth );
    for (NxI32 i=0; i<depth; i++)
    {
      mScanLines[i] = scan;
      scan+=width;
    }
    if ( data )
    {
      memcpy(mData,data,sizeof(NxF32)*size);
      for (NxI32 i=0; i<size; i++)
      {
        Type v = data[i];
        if ( v < mMinV ) mMinV = v;
        if ( v > mMaxV ) mMaxV = v;
      }
    }
  }

  ~HeightMap(void)
  {
    MEMALLOC_FREE(mData);
  }

  inline void set(NxI32 x,NxI32 z,Type v)
  {
    assert( x >= 0 && x < mWidth );
    assert( z >= 0 && z < mDepth );
    Type *scan = mScanLines[z];
    scan[x] = v;
  }

  inline void setMinMax(NxI32 x,NxI32 z,Type v)
  {
    assert( x >= 0 && x < mWidth );
    assert( z >= 0 && z < mDepth );
    Type *scan = mScanLines[z];
    scan[x] = v;
    if ( v < mMinV ) mMinV = v;
    if ( v > mMaxV ) mMaxV = v;
  }

  inline Type get(NxI32 x,NxI32 z) const
  {
    assert( x >= 0 && x < mWidth );
    assert( z >= 0 && z < mDepth );
    const Type *scan = mScanLines[z];
    return scan[x];
  }

  inline NxI32 clampX(NxI32 x) const
  {
    if ( x < 0 ) x = 0;
    else if ( x >= mWidth ) x = mWidth-1;
    return x;
  }

  inline NxI32 clampZ(NxI32 z) const
  {
    if ( z < 0 ) z = 0;
    else if ( z >= mDepth ) z = mDepth-1;
    return z;
  }

  inline void setSafe(NxI32 x,NxI32 z,Type v)
  {
    x = clampX(x);
    z = clampZ(z);
    mScanLines[z][x] = v;
  }

  inline Type getSafe(NxI32 x,NxI32 z) const
  {
    x = clampX(x);
    z = clampZ(z);
    return mScanLines[z][x];
  }

  void normalize(void)
  {
    Type diff = mMaxV - mMinV;
    if ( diff > 0 )
    {
      Type recip = 1.0f / diff;
      NxI32 size = mWidth*mDepth;
      for (NxI32 i=0; i<size; i++)
      {
        Type v = mData[i];
        v = (v-mMinV)*recip;
        mData[i] = v;
      }
    }
  }

  NxI32 getWidth(void) const { return mWidth; };
  NxI32 getDepth(void) const { return mDepth; };

  Type * getData(void) const { return mData; };

private:
  Type   mMinV;
  Type   mMaxV;
  NxI32   mWidth;
  NxI32   mDepth;
  Type  *mData;
  Type **mScanLines;
};


class MyErode : public Erode, public NVSHARE::Memalloc
{
public:
  MyErode(const ErodeDesc &desc)
  {
    mFirst = true;
    mDesc = desc;
    if ( mDesc.mData )
    {
      HeightMap<NxF32> temp;
      temp.init(desc.mWidth,desc.mDepth,desc.mData);

      mHF.init(desc.mWidth+2,desc.mDepth+2,0);
      mHF2.init(desc.mWidth+2,desc.mDepth+2,0);

      for (NxI32 z=-1; z<=desc.mDepth; z++)
      {
        for (NxI32 x=-1; x<=desc.mWidth; x++)
        {
          NxF32 v = temp.getSafe(x,z);
          mHF.setMinMax(x+1,z+1,v);
        }
      }

      mHF.normalize(); // normalize the input data.

      mDirection.init(desc.mWidth,desc.mDepth,0);
      mFlags.init(desc.mWidth,desc.mDepth,0);
      mUphill.init(desc.mWidth,desc.mDepth,0);
      mBasin.init(desc.mWidth,desc.mDepth,0);

    }
  }

  ~MyErode(void)
  {
  }



  void findUpflow(void)
  {
    NxI32 x, y,i;
    NxI32 xn, yn;
    NxI32 dir;
    NxI32 inflows;                    /* #nodes flowing in */
    NxF32 mval;            /* current minimum elevation value */
    NxF32 maxv;            /* current maximum */
    NxF32 tval;            /* temporary value */

    NxI32 xsize = mHF.getWidth()-2;
    NxI32 ysize = mHF.getDepth()-2;

    for (y = 0; y < ysize; y++)
    {
      for (x = 0; x < xsize; x++)
      {
    	  mval = mHF.get(x+1,y+1);
      	maxv = mval;

  	    tval = mHF.get(x,y);
        mFlags.set(x,y,0);  /* 0 means point is a peak */
      	inflows = 0;            /* # neighbors pointing downhill to me */

  	    for (i=1;i<9;i++)
 	      {
  	      tval = mHF.get(x+xo[i]+1,y+yo[i]+1);       /* neighbor higher? */
      	  if (tval > maxv)
    	    {
  	        maxv = (NxU8)tval;                /* yes, set uphill->this one */
  	        mFlags.set(x,y,(NxU8)i);
  	      }
  	      xn = x + xo[i];
  	      yn = y + yo[i];
  			  /* make sure not off edge of mx */
  	      if ((xn >= 0) && (xn < xsize) && (yn >= 0) && (yn < ysize))
 	        {
  	        dir = mDirection.get(xn,yn);       /* inflow from here? */
  	        if ((xo[i]+xo[dir]==0) && (yo[i]+yo[dir]==0))
 	          { /* inflow! */
     	        inflows++;
  	        }
  	      }
  	    }

  	    if (inflows == 0)
 	      {     /* no inflows: find_ua can start here */
          NxU8 v = mFlags.get(x,y) | 0x10;  /* set b4: summed */
  	      mFlags.set(x,y,v);  /* set b4: summed */
  	    }

  			/* and if we still think it's a peak...*/

  	    if (mFlags.get(x,y)==0)
 	      {       /* supposed to be peak */
    	    for(i=1;i<9;i++)
   	      {
  	        dir = mDirection.getSafe(x+xo[i],y+yo[i]);
  	        if ((xo[i]+xo[dir]==0) && (yo[i]+yo[dir]==0))
 	          { /* inflow! */
  	          mFlags.set(x,y,1);  /* arbitrary direction */
  	        }
  	      }
  	    }
      }
    }
  }

  void findFlow2(void)
  {
    NxI32 xsize = mHF.getWidth()-2;
    NxI32 ysize = mHF.getDepth()-2;
    for (NxI32 y = 0; y < ysize; y++)
    {
      for (NxI32 x = 0; x < xsize; x++)
      {
        NxF32 slope;
        NxU8 v = lowestn(mHF,x,y,slope);
        mDirection.set(x,y,v);
      }
    }
  }

  NxU8 lowestn(const HeightMap<NxF32> &hf,NxI32 x,NxI32 y,NxF32 &return_slope) // compute the direction of steepest descent
  {
    NxF32 slope, vmin;

    NxI32 mini = 0;
    vmin = (NxF32)1e30;

    NxF32 here = hf.get(x+1,y+1);

    for (NxI32 i=1;i<9;i++)
    {
      slope = hf.get(x+xo[i]+1,y+yo[i]+1) - here;
      if ( (i&1) ) // if an odd entry..
        slope = slope * recip_scale;
      if (slope < vmin)
      {
        mini = i;
        vmin = slope;
      }
    }
    return_slope = vmin;
    return (NxU8) mini;
  }

  void quickFlow(const HeightMap<NxF32> &hf)
  {
    NxI32 x1, y1;
    NxI32 x, y,i;
    NxF32 mval;            /* current minimum elevation value */
    NxF32 tval;            /* temporary value */

    NxI32 xsize = hf.getWidth()-2;
    NxI32 ysize = hf.getDepth()-2;

    for (y = 0; y < ysize; y++)
    {
      for (x = 0; x < xsize; x++)
      {
   	    mval = hf.get(x+1,y+1);          /* start with current point */
        mDirection.set(x,y,0);
      	for (i = 1;i<9;i++)
    	  {
   	      x1 = x+xo[i];
   	      y1 = y+yo[i];
   	      tval = hf.get(x1+1,y1+1);
      	  if (tval < mval)
     	    {
            mDirection.set(x,y,1);
      	  }
      	}
      }
    }
  }

  NxI32 fillBasin(const HeightMap<NxF32> &hf1,HeightMap<NxF32> &hf2)
  {
    NxI32 ix,iy,i;
    NxI32 changed;
    NxF32 sum, hmax, neb, wavg;
    NxF32 delta_h;         /* max elevation change this run */
    NxF32 dhmax;           /* maximum elevation (?) */

    NxI32 xsize = hf1.getWidth()-2;
    NxI32 ysize = hf1.getDepth()-2;

    NxI32 tsize = (xsize+2)*(ysize+2);
    NxF32 *dest = hf2.getData();
    const NxF32 *source = hf1.getData();
    memcpy(dest,source,sizeof(NxF32)*tsize);

    changed = 0;                  /* no changes yet */
    dhmax = 0;
    for(iy=2;iy<(ysize);iy++)
    {
      for (ix=2;ix<(xsize);ix++)
      {
        if ( mDirection.get(ix-1,iy-1) == 0)
        {           /* local depression */
        	sum = 0; hmax = (NxF32)-9E20;
    	    for (i=1;i<9;i++)
   	      {
       	    neb = hf1.get(ix+xo[i],iy+yo[i]);      /* neighbor elev. */
    	      sum += neb;                   /* get sum of neighbor elevs */
    	      if (neb > hmax) hmax = neb;   /* get max elev. */
    	    }
        	/* sum += hmax; */               /* add max a few more times */
    	    wavg = sum / 8;               /* simple average */
    	    delta_h = wavg - hf1.get(ix,iy);       /* diff between avg and this */
    	    if (delta_h > dhmax) dhmax = delta_h;
    	    hf2.set(ix,iy,wavg);       /* hf2(x,y) <- weighted local avg */
          NxF32 bs = mBasin.get(ix-1,iy-1);
          bs+=delta_h;
          mBasin.set(ix-1,iy-1,bs); // accumulate in the basin
       	  changed++;
        } /* end if */
        else
        {    /* if not depression, just copy old mx to new */
          NxF32 v = hf1.get(ix,iy);
          hf2.set(ix,iy,v);
        }
      }
    }
    return changed;
  }

  NxI32 fillBasins(NxI32 imax)
  {
    NxI32 icount = 0;
    NxI32 tmp = 1;

    while ((tmp > 0) && (icount<imax) )
    {
      quickFlow(mHF);              /* find local minima only */
      tmp = fillBasin(mHF,mHF2);    /* temp <- original */
      icount++;
      quickFlow(mHF2);              /* find local minima only */
      tmp = fillBasin(mHF2,mHF);    /* original <- temp */
      icount++;
    } /* end while  */
    return(icount);
  }

  void findUphillArea(void)
  {
    NxI32 ix,iy;
    NxI32 i;
    NxI32 dir;              /* direction this element points in */
    NxU8 ff;               /* flag variable for this loc */
    NxI32 o_summed;         /* flag indicating all neighbors summed */
    long added;            /* # found unsummed nodes this pass */
    NxF32 area;           /* sum of uphill area for this element */


    /* first pass: set local highpoints to "summed", area to 1 */
    NxI32 xsize = mHF.getWidth()-2;
    NxI32 ysize = mHF.getDepth()-2;

    for (iy = 0; iy<ysize; iy++)
    {
      for (ix = 0; ix<xsize; ix++)
      {
        if ( (mFlags.get(ix,iy) & 0x10) == 0x10)
        { /* pre-marked summed (no inflows) */
        	mUphill.set(ix,iy,1);
        }
        if ( (mFlags.get(ix,iy) & 0x0f) == 0)
        {   /* this point is a peak */
      	  mUphill.set(ix,iy,1);                     /* peak: unit area */
          NxU8 flag = mFlags.get(ix,iy) | 0x10;
      	  mFlags.set(ix,iy,flag);
        }
      }
    }

    /* 2nd through n passes: cumulative add areas in a downhill-flow hierarchy */
    added = 1;
    while ( added > 0 )
    {
      added = 0;
      for (iy = 1; iy<ysize-1; iy++)
      {
        for (ix = 1; ix<xsize-1; ix++)
        {
          ff = mFlags.get(ix,iy);
          if (!(ff & 0x10))
          {      /* flag bit b4: set = ua for this node done */
        	  /* if all neighbors flowing into this node done, can sum */
  	        o_summed = 1;           /* start assuming we're ok... 0=false*/
     	      for(i=1;i<9;i++)
   	        {      /* check if relevant neighbors finished */
  	          dir = mDirection.get(ix+xo[i],iy+yo[i]);
  	          if ( ((xo[dir]+xo[i])==0) && ((yo[dir]+yo[i])==0))		/* ie,that node points here */
           		if (!(mFlags.get(ix+xo[i],iy+yo[i]) & 0x10))
  		          o_summed = 0;
  	        }
         	  if (o_summed)
       	    {
  	          area = 1;
  	          for (i=1;i<9;i++)
	            {
  	            dir = mDirection.get(ix+xo[i],iy+yo[i]);
        	      if ( ((xo[dir]+xo[i])==0) && ((yo[dir]+yo[i])==0))
  	              area += mUphill.get(ix+xo[i],iy+yo[i]);
         	    }
  	          mUphill.set(ix,iy,area);
              NxU8 flag = mFlags.get(ix,iy);
              mFlags.set(ix,iy,flag | 0x10 );
  	          added++;  /* increment count of nodes summed */
  	        }
          }
        }
      }

      /* aaand now... do the edges */
      for (iy = 0; iy<ysize; iy++)
      {
        added += ua_iter(0,iy);
        added += ua_iter(xsize-1,iy);
      }
      for (ix = 1; ix<xsize-1; ix++)
      {
  	    added += ua_iter(ix,0);
  	    added += ua_iter(ix,ysize-1);
      }
    }
  }


  NxI32 ua_iter(NxI32 ix, NxI32 iy)   /* one iteration of uphill-area +bounds checks */
  {
    NxI32 i;
    NxI32 xx, yy;
    NxI32 tmp;
    NxI32 dir;              /* direction this element points in */
    NxU8 ff;               /* flag variable for this loc */
    NxI32 o_summed;         /* flag indicating all neighbors summed */
    NxI32 added=0;          /* if this node was added */
    NxF32 area;           /* sum of uphill area for this element */

    NxI32 xsize = mHF.getWidth()-2;
    NxI32 ysize = mHF.getDepth()-2;


    ff = mFlags.get(ix,iy);
    if ((ff & 0x10) != 0x10)
    { /* flag bit b4: set = ua for this node done */
    	/* if all neighbors flowing into this node done, can sum */
  	  o_summed = 1;           /* start assuming we're ok... 0=false*/
  	  for(i=1;i<9;i++)
 	    {      /* check if relevant neighbors finished */
  	    xx = ix+xo[i];
  	    yy = iy+yo[i];
  	    if ((xx >= 0) && (xx < xsize) && (yy >= 0) && (yy < ysize))
	      {
  	      dir = mDirection.get(xx,yy);
      	  if ( ((xo[dir]+xo[i])==0) && ((yo[dir]+yo[i])==0))
  	    	/* ie,that node points here */
      		if (!(mFlags.get(xx,yy) & 0x10))
  	    	   o_summed = 0;
  	    }
  	  }
    	if (o_summed)
  	  {
    	  area = 1;
    	  for (i=1;i<9;i++)
  	    {
    	    xx = ix+xo[i];
  	      yy = iy+yo[i];
  	      if ((xx >= 0) && (xx < xsize) && (yy >= 0) && (yy < ysize))
	        {
      	    dir = mDirection.get(xx,yy);
      	    if ( ((xo[dir]+xo[i])==0) && ((yo[dir]+yo[i])==0))
  	          area += mUphill.get(xx,yy);
      	  }
  	    }
    	  mUphill.set(ix,iy,area);
    	  tmp = mFlags.get(ix,iy);
    	  mFlags.set(ix,iy,(NxU8)(tmp | 0x10));  /* set b4: node summed */
    	  added = 1;
  	  }
    }
    return(added);       /* return 1 if this node was summed, 0 otherwise */
  }

  /* ------------------------------------------------------------ */
  /* erode  --  simulate erosion by shifting material in hf[]     */
  /*            downhill depending on slope and stream flow       */
  /*              depends on ua (uphill area) being set           */
  /* ------------------------------------------------------------ */
  void erosion(void)
  {
    erode_1(mHF,mHF2);
    correct_1(mHF2,mHF);
  }

  /* ------------------------------------------------------------------- */
  /* erode_1()  --  erode one step, taking input hf1[] into output hf2[] */
  /* ------------------------------------------------------------------- */
  void erode_1(HeightMap<NxF32> &hf1,HeightMap<NxF32> &hf2)
  {
    /* Erodef(slope,flow)  is the function controlling erosion rate */
    /*                               slope exponent   flow exponent */
    #define Erodef(a,b)  sgn1(a) * pow(fabs(atan(a)),1.4) * pow((b),0.8)
    #define EDGELEN         1000.0  /* image size (bottom edge length) in meters */
    #define MAXALT          100.0   /* meters elevation between 0.0 and 1.0 */
    #define MAXSLOPE        1.5       /* maximum slope in radians (ngl of repose) */
    #define MINSLOPE        1.0     /* slope in lowlands */

    NxI32 ix,iy,i,dir=0;              /* indexes into arrays */
    NxI32 upd;                       /* direction of uphill neighbor */
    NxF64 inslope, outslope;      /* product of slope and flow_rate */
    NxF64 outflow;                /* flow out of this cell */
    NxF64 erval;                  /* erosion potential at this point */
    NxI32 x1,y1;
    NxF64 slopefactor;            /* correction factor to scale slope */
    NxF64 h, dh, slope;  /* dh- delta height  dr- horizontal travel */

    NxI32 xsize = hf1.getWidth()-2;
    NxI32 ysize = hf1.getDepth()-2;

    slopefactor = MAXALT / (EDGELEN / xsize);  /* slope of 0.0 next to 1.0 */

    for (iy = 0; iy<ysize; iy++)
    {  /* skip the edges */
      for (ix = 0; ix<xsize; ix++)
      {

        h = hf1.get(ix+1,iy+1);        /* altitude at this point ix,iy */

        if (h < mDesc.mErodeThreshold)
        {
          inslope = 0;       /* find sum of inflowing neighbors (slope*flow) */
          for (i=1;i<9;i++)
          {               /* loop over neighbors of ix,iy */
            x1 = ix+xo[i];
            y1 = iy+yo[i];
  				  /* check if off edge of array */
            if ((x1>=0) && (y1>=0) && (x1<xsize) && (y1<ysize))
            {
  	          dir = mDirection.get(x1,y1);   /* direction index of this neighbor */
  	          if ((xo[i]+xo[dir]==0) && (yo[i]+yo[dir]==0))
 	            { /* inflow from here */
  	            slope = hf1.get(x1+1,y1+1) - hf1.get(ix+1,iy+1); /* pos = neigh. higher */
  	            slope *= slopefactor;                 /* scale to corr. units */
  	            if ( (i&1) )
            	    slope = slope * recip_scale;      /* diagonal neighbor; reduce slope */
  	            inslope += Erodef(slope,(NxF64)mUphill.get(x1,y1));
  	          }
            }
          }

          dir = mDirection.get(ix,iy);       /* direction of outflow */
  				/* slope positive: neighbor lower */
          if (dir > 0)
          { /* if not a minimum... */
            h = hf1.get(ix+1,iy+1);      /* current elevation at this point */
            if (h > 1.0)
            {
         	    h = 1.0;
            }
            slope = h - hf1.get(ix+xo[dir]+1,iy+yo[dir]+1);
            slope *= slopefactor;
  	        if ( (dir&1) )
      	     slope = slope * recip_scale;      /* diagonal neighbor; reduce slope */
            outflow = (NxF64) mUphill.get(ix,iy);

            if (outflow<1) outflow = 0.9;
            outslope = Erodef(slope,outflow);

          	/* positive dh = erosion happens, neg = sedimentation */
  	        /* sedimentation when (inslope) greater than (er_sedfac*outslope) */

            erval = mDesc.mErodeRate * (mDesc.mErodeSedimentation*outslope - inslope);
            erval *= pow(outflow,(NxF64)mDesc.mErodePower);
            dh = mDesc.mErodeRate * erval;
            h = hf1.get(ix+1,iy+1);      /* old altitude at this point */

            if (dh > mDesc.mErodeRate) dh = mDesc.mErodeRate;   /* don't erode too fast */
            if  ( (h-dh) < 0 )
            {  /* don't let altitudes go negative */
         	    dh = -h;           /* min elevation will be == 0.0 */
  	        }
        		/* don't sediment too fast... */
  		      /* this should really depend on neighboring elevations */
            if (dh < -(0.03*mDesc.mErodeRate)) dh = -(0.03*mDesc.mErodeRate);

  				  /* ------------------------------- */
  				  /* the actual erosion. right here! */
  				  /* ------------------------------- */
            hf2.set(ix+1,iy+1, (NxF32)(h - dh));
            mBasin.set(ix,iy, mBasin.get(ix,iy)-(NxF32)dh);
          }
          else
          {  /* if point is a local minimum (oops!) */
          	upd = mFlags.get(ix,iy) & 0x0f;   /* direction of uphill neighbor */
  	        hf2.set(ix+1,iy+1, hf1.get(ix+xo[upd]+1,iy+yo[upd]+1)+0.00001f);
          }
        }
        else
        {  /* NO erosion if point above erosion threshold */
      	  hf2.set(ix+1,iy+1, h );
        }
      }
    }
  }


   /* --------------------------------------------------------------------- */
   /* correct_1()  --      Input hf1, output hf2.  Checks that every point  */
   /*                      in hf1 has an output flow (from fl[]) that is    */
   /*                      actually downhill, and if it isn't, makes it so. */
   /*                      Does not set the border pixels in hf2[].         */
   /* --------------------------------------------------------------------- */
   NxI32 correct_1(HeightMap<NxF32> &hf1,HeightMap<NxF32> &hf2)
   {
     NxI32 ix, iy;
     NxI32 dir;
     NxI32 fixed;              /* number of pixels with altitude adjusted */
     NxF32 h, hdown;

     NxI32 xsize = hf1.getWidth()-2;
     NxI32 ysize = hf1.getDepth()-2;

     fixed = 0;
     for (iy = 0; iy < ysize; iy++)
     {
       for (ix = 0; ix < xsize; ix++)
       {
         h = hf1.get(ix+1,iy+1);         /* elevation at this point */
         dir = mDirection.get(ix,iy);         /* flow direction */
         hdown = hf1.get(ix+xo[dir]+1,iy+yo[dir]+1);   /* neighbor receiving flow */
         if (hdown > h)
         {            /* oops, flow is uphill! we over-eroded */
           /* so, this pixel -> just a tad higher than the downflow neighbor */
           hf2.set(ix+1,iy+1,0.0001f + (NxF32)hdown);
           mBasin.set(ix,iy, mBasin.get(ix,iy) + (NxF32)(0.0001 + hdown - h) );
           fixed++;
         }
         else
         {                    /* otherwise, just copy over to hf2 */
           hf2.set(ix+1,iy+1,h);
         }
       }
     }
     return(fixed);
   }


  /* ------------------------------------------------------------ *
   * slump  --  simulate land slippage; mudslides
   * ------------------------------------------------------------ */
  void slump(NxF64 rate)
  {
    NxI32 i,ix,iy;
    NxF32 h,dh,avg,sum1,sum2;
    NxF32 afac, n1, n2;
    NxF32 af1, af2;
    NxF32 dscale;    /* average difference between neighboring pixels */
    NxI32 dcount;

    /* find dscale by sampling a few pixels in the grid */
    NxI32 xsize = mHF.getWidth()-2;
    NxI32 ysize = mHF.getDepth()-2;

    dcount = 0;
    dscale = 0;
    for (iy = 0; iy<ysize; iy+=5)
    {  /* take samples of 1/25th of all */
      for (ix = 0; ix<xsize; ix+=5)
      {
        h = mHF.get(ix+1,iy+1);
        sum1 = 0;
        af1 = 4;
        for (i=1;i<9;i+=2)
        {
          n1 = mHF.get(ix+xo[i+1]+1,iy+yo[i+1]+1) - h; /* adj. neighbors */
          if (n1 < 0)
          {  /* count lower elements more */
     	      n1 *= 2.0;
  	        af1 += 1;
	        }
          sum1 += n1;
        } /* end for i... */
        dscale += sum1/af1;
        dcount++;                 /* count number of summations */
      }
    } /* end for iy.. */

    dscale /= dcount;       /* really, dscale should be a fixed value */

    /* found dscale */


    for (iy = 0; iy<ysize; iy++)
    {  /* skip the edges */
      for (ix = 0; ix<xsize; ix++)
      {
        h = mHF.get(ix+1,iy+1);
        sum1 = 0;
        sum2 = 0;
        af1 = 4;
        af2 = 4;
        for (i=1;i<9;i+=2)
        {
          n1 = mHF.get(ix+xo[i+1]+1,iy+yo[i+1]+1) - h; /* adj. neighbors */
          if (n1 < 0)
          {  /* count lower elements more */
       	    n1 *= 2.0;
  	        af1 += 1;
	        }
          n2 = mHF.get(ix+xo[i]+1,iy+yo[i]+1) - h;     /* diagonal neighbors */
          if (n2 < h)
          {
       	    n2 *= 2.0;
        	  af2 += 1;
      	  }
          sum1 += n1;
          sum2 += n2;
        }

        avg = 0.8f*(sum1/af1) + 0.2f*(sum2/af2); /* weight closest neighbors heavily */
        dh = avg;   /* difference between this point and avg of neighbors */
        if (h > 1.0) h = 1;
        if (h < 0.0) h = 0;
    		/* sign convention: positive afac = addition to elevation */
  	  	/* what is the effect of nonlinear smoothing? hmmm. */
#pragma warning(push)
#pragma warning(disable:4244)


        afac = dh * PCONST*atan( pow((NxF64)(rate*fabs(dh/dscale)),3.0) ); /* slump! */

#pragma warning(pop)
        if (h < mDesc.mErodeThreshold )
        {        /* only smooth terrain below altitude limit */
          mHF2.set(ix+1,iy+1, h + afac);
          NxF32 b = mBasin.get(ix,iy);
          b+=afac;
          mBasin.set(ix,iy,b);
        }
        else
        {          /* if above threshold, NO erosion */
          mHF2.set(ix+1,iy+1,h);
        }
      }
    }

    for (iy = 0; iy<ysize; iy++)
    {
      for (ix = 0; ix<xsize; ix++)
      {
        mHF.set(ix+1,iy+1, mHF2.get(ix+1,iy+1));
      }
    }
  }


  void getResults(void)  // returns the erosion results back into the original buffer passed in.
  {
    if ( mDesc.mData )
    {
      NxF32 *dest = mDesc.mData;
      NxI32 xsize = mHF.getWidth()-2;
      NxI32 ysize = mHF.getDepth()-2;
      for (NxI32 y=0; y<ysize; y++)
      {
        for (NxI32 x=0; x<xsize; x++)
        {
          NxF32 v = mHF.get(x+1,y+1);
          *dest++ = v;
        }
      }
    }
  }


  bool erode(void)  // perform a single erosion step
  {
    bool ret = false;

    if ( mDesc.mData )
    {
      if ( mFirst )
      {
        findFlow2();
        findUpflow();
        fillBasins(mDesc.mFillBasin);
        findFlow2();
        findUpflow();
        findUphillArea();
        mFirst = false;
      }
      erosion();
      slump( mDesc.mSmoothRate );
      fillBasins(mDesc.mFillBasinPer);
      findFlow2();
      findUpflow();
      findUphillArea();
      mDesc.mErodeCount++;
      if ( mDesc.mErodeCount >= mDesc.mErodeIterations )
      {
        ret = false;
      }
      else
      {
        ret = true;
      }

    }

    return ret;
  }

private:
  bool      mFirst;
  ErodeDesc mDesc;

  HeightMap<NxF32> mHF;
  HeightMap<NxF32> mHF2;

//
  HeightMap<NxU8>  mDirection;
  HeightMap<NxU8>  mFlags;
  HeightMap<NxF32> mUphill;
  HeightMap<NxF32> mBasin;

};


Erode * createErode(const ErodeDesc &desc)
{
  MyErode *m = MEMALLOC_NEW(MyErode)(desc);
  return static_cast< Erode *>(m);
}

void    releaseErode(Erode *erode)
{
  MyErode *m = static_cast< MyErode *>(erode);
  delete m;
}


}; // end of namespace
