#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <float.h>

#include <windows.h>

#include "fractal.h"
#include "common/snippets/pool.h"
#include "common/snippets/JobSwarm.h"
#include "common/snippets/StringDict.h"

#define SOLVER_PER 32768

#define MAX_DIFFICULTY 256

#define PIXEL_PER 64  // solve 64 fractal pixels per thread.
#define RECT_PER 512

static MultiFloatType gFloatType=MFT_MEDIUM;
static BigFloat gLimit=4;

class Frame
{
public:
  virtual void set(const BigFloat &x1,const BigFloat &y1,const BigFloat &x2,const BigFloat &y2,const BigFloat &fractalScale) = 0;
  virtual MultiFloatType getType(void) const = 0;
};

class FrameFixed32 : public Frame
{
public:

  void set(const BigFloat &x1,const BigFloat &y1,const BigFloat &x2,const BigFloat &y2,const BigFloat &fractalScale)
  {
    mX1 = getDouble(x1);
    mY1 = getDouble(y1);
    mX2 = getDouble(x2);
    mY2 = getDouble(y2);
    mFractalScale = getDouble(fractalScale);
  }

  MultiFloatType getType(void) const { return MFT_FIXED32; };

  Fixed32 mX1;
  Fixed32 mY1;
  Fixed32 mX2;
  Fixed32 mY2;
  Fixed32 mFractalScale;
};

class FrameMedium : public Frame
{
public:

  void set(const BigFloat &x1,const BigFloat &y1,const BigFloat &x2,const BigFloat &y2,const BigFloat &fractalScale)
  {
    mX1 = getDouble(x1);
    mY1 = getDouble(y1);
    mX2 = getDouble(x2);
    mY2 = getDouble(y2);
    mFractalScale = getDouble(fractalScale);
  }

  MultiFloatType getType(void) const { return MFT_MEDIUM; };

  HeF64  mX1;
  HeF64  mY1;
  HeF64  mX2;
  HeF64  mY2;
  HeF64  mFractalScale;
};

class FrameSmall : public Frame
{
public:

  void set(const BigFloat &x1,const BigFloat &y1,const BigFloat &x2,const BigFloat &y2,const BigFloat &fractalScale)
  {
    mX1 = (HeF32)getDouble(x1);
    mY1 = (HeF32)getDouble(y1);
    mX2 = (HeF32)getDouble(x2);
    mY2 = (HeF32)getDouble(y2);
    mFractalScale = (HeF32)getDouble(fractalScale);
  }

  MultiFloatType getType(void) const { return MFT_SMALL; };

  HeF32   mX1;
  HeF32   mY1;
  HeF32   mX2;
  HeF32   mY2;
  HeF32   mFractalScale;
};

class FrameBig : public Frame
{
public:

  void set(const BigFloat &x1,const BigFloat &y1,const BigFloat &x2,const BigFloat &y2,const BigFloat &fractalScale)
  {
    mX1 = x1;
    mY1 = y1;
    mX2 = x2;
    mY2 = y2;
    mFractalScale = fractalScale;
  }

  MultiFloatType getType(void) const { return MFT_BIG; };

  BigFloat   mX1;
  BigFloat   mY1;
  BigFloat   mX2;
  BigFloat   mY2;
  BigFloat   mFractalScale;
};


class RectTask
{
public:

  void set(HeU32 x1,HeU32 y1,HeU32 x2,HeU32 y2)
  {
    mX1 = x1;
    mY1 = y1;
    mX2 = x2;
    mY2 = y2;
  }

  HeU32 mX1;
  HeU32 mX2;
  HeU32 mY1;
  HeU32 mY2;

};

class QueueTask
{
public:
  QueueTask(void)
  {
    mNextJob = 0;
  }
  QueueTask(const RectTask &r)
  {
    mX1 = r.mX1;
    mX2 = r.mX2;
    mY1 = r.mY1;
    mY2 = r.mY2;
    mNextJob = 0;
  }


  QueueTask * GetNext(void) const { return mNext; };
  QueueTask * GetPrevious(void) const { return mPrevious; };
  void       SetNext(QueueTask *task) { mNext = task; };
  void       SetPrevious(QueueTask *task) { mPrevious = task; };

  QueueTask *mNext;
  QueueTask *mPrevious;

  QueueTask  *mNextJob; // singly linked list..

  HeU32 mX1;
  HeU32 mX2;
  HeU32 mY1;
  HeU32 mY2;
};

static inline HeU32 MandelbrotPoint(HeU32 iterations,HeF32 real,HeF32 imaginary)
{
	HeF32 fx,fy,xs,ys;
	HeU32 count;

  HeF32 two(2.0);

	fx = real;
	fy = imaginary;
  count = 0;

  do
  {
    xs = fx*fx;
    ys = fy*fy;
		fy = (two*fx*fy)+imaginary;
		fx = xs-ys+real;
    count++;
	} while ( xs+ys < 4.0f && count < iterations);

	return count;
}

static inline HeU32 MandelbrotPoint(HeU32 iterations,HeF64 real,HeF64 imaginary)
{
	HeF64 fx,fy,xs,ys;
	HeU32 count;

  HeF64 two(2.0);

	fx = real;
	fy = imaginary;
  count = 0;

  do
  {
    xs = fx*fx;
    ys = fy*fy;
		fy = (two*fx*fy)+imaginary;
		fx = xs-ys+real;
    count++;
	} while ( xs+ys < 4.0 && count < iterations);

	return count;
}

static BigFloat two(2);

static inline HeU32 MandelbrotPoint(HeU32 iterations,const BigFloat &real,const BigFloat &imaginary)
{
	BigFloat fx,fy,xs,ys;
	HeU32 count;


	fx = real;
	fy = imaginary;
  count = 0;

  do
  {
    xs = fx*fx;
    ys = fy*fy;
		fy = (two*fx*fy)+imaginary;
		fx = xs-ys+real;
    count++;
	} while ( xs+ys < gLimit && count < iterations);

	return count;
}

static inline HeU32 MandelbrotPoint(HeU32 iterations,const Fixed32 &real,const Fixed32 &imaginary)
{
  Fixed32 fx,fy,xs,ys;
  HeU32 count;

  Fixed32 two = (HeF64)2.0;
  Fixed32 limit = (HeF64)4.0;

  fx = real;
  fy = imaginary;
  count = 0;

  Fixed32 total;

  do
  {
    xs = fx*fx;
    ys = fy*fy;
    fy = (two*fx*fy);
    fy+=imaginary;
    fx = xs-ys;
    fx+=real;
    count++;
    total = xs+ys;
  } while ( total < limit && count < iterations);

  return count;
}



class FractalPixel
{
public:

  virtual void solve(HeU32 maxiter) = 0;
	virtual void set(HeU32 x,HeU32 y,Frame *frameReference) = 0;

  HeU32  mX;
  HeU32  mY;
  HeU32  mIterationCount;
};

class FractalPixelSmall : public FractalPixel
{
public:

  void solve(HeU32 maxiter)
  {
		mIterationCount = MandelbrotPoint(maxiter,mFX,mFY);
  }

	void set(HeU32 x,HeU32 y,Frame *frameReference)
	{
    FrameSmall *fs = (FrameSmall *)frameReference;
    mX = x;
    mY = y;
    mFX = (HeF32)x*fs->mFractalScale + fs->mX1;
    mFY = (HeF32)y*fs->mFractalScale + fs->mY1;
		mIterationCount = 0;
	}

  HeF32         mFX;
  HeF32         mFY;
};

class FractalPixelMedium : public FractalPixel
{
public:

  void solve(HeU32 maxiter)
  {
		mIterationCount = MandelbrotPoint(maxiter,mFX,mFY);
  }

	void set(HeU32 x,HeU32 y,Frame *frameReference)
	{
    FrameMedium *fs = (FrameMedium *)frameReference;
    mX = x;
    mY = y;
    mFX = (HeF64)x*fs->mFractalScale + fs->mX1;
    mFY = (HeF64)y*fs->mFractalScale + fs->mY1;
		mIterationCount = 0;
	}

  HeF64        mFX;
  HeF64        mFY;
};

class FractalPixelBig : public FractalPixel
{
public:

  void solve(HeU32 maxiter)
  {
		mIterationCount = MandelbrotPoint(maxiter,mFX,mFY);
  }

	void set(HeU32 x,HeU32 y,Frame *frameReference)
	{
    FrameBig *fs = (FrameBig *)frameReference;
    mX = x;
    mY = y;
    BigFloat bx(x);
    BigFloat by(y);
    mFX = bx*fs->mFractalScale + fs->mX1;
    mFY = by*fs->mFractalScale + fs->mY1;
		mIterationCount = 0;
	}

  BigFloat      mFX;
  BigFloat      mFY;
};

class FractalPixelFixed32 : public FractalPixel
{
public:

  void solve(HeU32 maxiter)
  {
		mIterationCount = MandelbrotPoint(maxiter,mFX,mFY);
  }

	void set(HeU32 x,HeU32 y,Frame *frameReference)
	{
    FrameFixed32 *fs = (FrameFixed32 *)frameReference;
    mX = x;
    mY = y;

    Fixed32 bx(x);
    Fixed32 by(y);

    mFX = bx*fs->mFractalScale + fs->mX1;
    mFY = by*fs->mFractalScale + fs->mY1;
		mIterationCount = 0;
	}

  Fixed32       mFX;
  Fixed32       mFY;
};

class FractalThread : public JOB_SWARM::JobSwarmInterface
{
public:
  FractalThread(void)
  {
    mLastType = MFT_LAST;
    mPixelsSmall = 0;
    mPixelsMedium = 0;
    mPixelsBig = 0;
    mPixelsFixed32 = 0;
    mSwarmJob = 0;
    mCancelled = false;
  }

  FractalThread(Fractal *parent,Frame *referenceFrame)
  {
    mSwarmJob = 0;
    mReferenceFrame = referenceFrame;
    mParent = parent;
    mCount = 0;
    mRectCount = 0;
    mCancelled = false;

    if ( mLastType != referenceFrame->getType() )
    {
      switch ( mLastType )
      {
        case MFT_SMALL:
          delete []mPixelsSmall;
          mPixelsSmall = 0;
          break;
        case MFT_MEDIUM:
          delete []mPixelsMedium;
          mPixelsMedium = 0;
          break;
        case MFT_BIG:
          delete []mPixelsBig;
          mPixelsBig = 0;
          break;
        case MFT_FIXED32:
          delete []mPixelsFixed32;
          mPixelsFixed32 = 0;
          break;
      }

      mLastType = referenceFrame->getType();

      switch ( mLastType )
      {
        case MFT_SMALL:
          mPixelsSmall = MEMALLOC_NEW_ARRAY(FractalPixelSmall,PIXEL_PER)[PIXEL_PER];
          break;
        case MFT_MEDIUM:
          mPixelsMedium = MEMALLOC_NEW_ARRAY(FractalPixelMedium,PIXEL_PER)[PIXEL_PER];
          break;
        case MFT_BIG:
          mPixelsBig = MEMALLOC_NEW_ARRAY(FractalPixelBig,PIXEL_PER)[PIXEL_PER];
          break;
        case MFT_FIXED32:
          mPixelsFixed32 = MEMALLOC_NEW_ARRAY(FractalPixelFixed32,PIXEL_PER)[PIXEL_PER];
          break;
      }
    }
  }

  void job_process(void *userData,int userId);  // RUNS IN ANOTHER THREAD!! MUST BE THREAD SAFE
  void job_onFinish(void *userData,int userId); // notifies the application that the thread is complete.  This is run from 'processThreadJobs' and is in the main application thread
  void job_onCancel(void *userData,int userId);

  bool hasThreadJob(void) const
  {
    return mSwarmJob != 0;
  }

	FractalThread * GetNext(void) { return mNext; };
	FractalThread * GetPrevious(void) { return mPrevious; };
  void            SetNext(FractalThread *f) { mNext = f; };
  void            SetPrevious(FractalThread *f) { mPrevious = f; };

  bool add(HeU32 x,HeU32 y)
  {
    switch ( mLastType )
    {
      case MFT_SMALL:
        {
          FractalPixelSmall &f = mPixelsSmall[mCount];
          f.set(x,y,mReferenceFrame);
          mCount++;
        }
        break;
      case MFT_MEDIUM:
        {
          FractalPixelMedium &f = mPixelsMedium[mCount];
          f.set(x,y,mReferenceFrame);
          mCount++;
        }
        break;
      case MFT_BIG:
        {
          FractalPixelBig &f = mPixelsBig[mCount];
          f.set(x,y,mReferenceFrame);
          mCount++;
        }
        break;
      case MFT_FIXED32:
        {
          FractalPixelFixed32 &f = mPixelsFixed32[mCount];
          f.set(x,y,mReferenceFrame);
          mCount++;
        }
        break;
    }
    return mCount == PIXEL_PER;
  }

  bool addRect(HeU32 x1,HeU32 y1,HeU32 x2,HeU32 y2)
  {
    mRectangles[mRectCount].set(x1,y1,x2,y2);
    mRectCount++;
    return mRectCount == RECT_PER;
  }

  bool beginThreadJob(void)
  {
    assert(mSwarmJob==0);
    mSwarmJob = gJobSwarmContext->createSwarmJob(this,0,0);
    assert(mSwarmJob);
    return mSwarmJob != 0;
  }

  void cancel(void);

  bool isCancelled(void) const { return mCancelled; };


private:
  FractalThread        *mNext;
  FractalThread        *mPrevious;
  Frame                *mReferenceFrame;
  bool                  mCancelled;
  Fractal              *mParent;
  HeU32                 mCount;
  FractalPixelSmall    *mPixelsSmall;
  FractalPixelMedium   *mPixelsMedium;
  FractalPixelBig      *mPixelsBig;
  FractalPixelFixed32  *mPixelsFixed32;
  JOB_SWARM::SwarmJob  *mSwarmJob;

  MultiFloatType        mLastType;
  HeU32                 mRectCount;
  RectTask              mRectangles[RECT_PER];
};


#pragma warning(disable:4189)
class Fractal
{
public:
  Fractal(FractalInterface *iface,HeU32 fractalSize,HeU32 swidth,HeU32 sheight,HeU32 maxIterations,const BigFloat &xleft,const BigFloat &xright,const BigFloat &ytop,bool useRectangleSubdivision)
  {

    char scratch[512];
    sprintf(scratch,"Fractal%d", fractalSize );
    StringRef name = SGET(scratch);

		mCallback = iface;
    mFractalSize = fractalSize;
    mScreenWidth = swidth;
    mScreenHeight = sheight;
    mMaxIterations = maxIterations;

		HeF32 scale = (HeF32)mMaxIterations / (HeF32) MAX_DIFFICULTY;
		mDifficultyScale = (HeU32)(scale*256);

    mX1 = xleft;
    mY1 = ytop;
    mX2 = xright;
    mY2 = mY1+(mX2-mX1);

    mUseRectangleSubdivision = useRectangleSubdivision;
    mUseRectangleSubdivision = false;
    mSolveCount = mFractalSize*mFractalSize;
    mSolvePer   = SOLVER_PER;
    mSolveX = 0;
    mSolveY = 0;

    BigFloat fsize(mFractalSize);

    mFractalScale = (mX2-mX1) / fsize;
    mCurrent = 0;
    mThreads.Set(256,256,10000000,"FractalThread",__FILE__,__LINE__);
    mQueueTasks.Set(256,256,10000000,"QueueTask",__FILE__,__LINE__);

    mFractalBuffer     = MEMALLOC_NEW_ARRAY(HeU32,mFractalSize*mFractalSize)[mFractalSize*mFractalSize];
    mFractalBackBuffer = MEMALLOC_NEW_ARRAY(HeU32,mFractalSize*mFractalSize)[mFractalSize*mFractalSize];

    mActiveJobs = 0;

    for (HeU32 i=0; i<MAX_DIFFICULTY; i++)
      mPending[i] = 0;

    mMemSize = sizeof(HeU32)*mFractalSize*mFractalSize;

    memset(mFractalBuffer,0,mMemSize);
    memset(mFractalBackBuffer,0,mMemSize);

    frameSetup();

    if ( mUseRectangleSubdivision )
      begin(0,0,mFractalSize-1,mFractalSize-1);
  }

  Fractal(const Fractal &f,FractalInterface *iface,HeU32 fractalSize,HeU32 maxIterations)
  {

    char scratch[512];
    sprintf(scratch,"Fractal%d", fractalSize );
    StringRef name = SGET(scratch);

		mCallback = iface;
    mActiveJobs = 0;
    mCurrent = 0;
    mFractalSize   = fractalSize;
    mScreenWidth = f.mScreenWidth;
    mScreenHeight = f.mScreenHeight;

    mMaxIterations = maxIterations;
		mDifficultyScale = (mMaxIterations*256) / MAX_DIFFICULTY;
    mSolveX        = 0;
    mSolveY        = 0;
    mSolveCount    = mFractalSize*mFractalSize;

    mSolvePer      = f.mSolvePer;
    mUseRectangleSubdivision = f.mUseRectangleSubdivision;
    mUseRectangleSubdivision = false;
    mX1 = f.mX1;
    mX2 = f.mX2;
    mY1 = f.mY1;
    mY2 = f.mY2;

    mFractalScale = (mX2-mX1) / (BigFloat) mFractalSize;

    mThreads.Set(256,256,1000000,"FractalThread",__FILE__,__LINE__);
    mQueueTasks.Set(256,256,1000000,"QueueTask",__FILE__,__LINE__);

    mFractalBuffer     = MEMALLOC_NEW_ARRAY(HeU32,mFractalSize*mFractalSize)[mFractalSize*mFractalSize];
    mFractalBackBuffer = MEMALLOC_NEW_ARRAY(HeU32,mFractalSize*mFractalSize)[mFractalSize*mFractalSize];

    mMemSize = sizeof(HeU32)*mFractalSize*mFractalSize;

    memset(mFractalBuffer,0,mMemSize);
    memset(mFractalBackBuffer,0,mMemSize);

    for (HeU32 i=0; i<MAX_DIFFICULTY; i++)
      mPending[i] = 0;

    frameSetup();
    if ( mUseRectangleSubdivision )
      begin(0,0,mFractalSize-1,mFractalSize-1);
  }

  ~Fractal(void)
  {
    cancel();
    delete []mFractalBuffer;
    delete []mFractalBackBuffer;
  }

  bool process(HeF32 /*dtime*/)
  {
    bool ret = true; // by default continue processing..

    if ( mUseRectangleSubdivision )
    {

      HeI32 count = mThreads.Begin(); // we cannot process rectangles unless all plotting is caught up.

      if ( count == 0 )
      {
        for (HeU32 i=0; i<MAX_DIFFICULTY; i++)
        {
          if ( mPending[i] ) // post the lowest priority ones first
          {
            QueueTask *qt = mPending[i];
            SplitRect(qt->mX1,qt->mY1,qt->mX2,qt->mY2);
            QueueTask *next = qt->mNextJob;
            mQueueTasks.Release(qt);
            while ( next )
            {
              qt = next;
              next = qt->mNextJob;
              SplitRect(qt->mX1,qt->mY1,qt->mX2,qt->mY2);
              mQueueTasks.Release(qt);
            }
            mPending[i] = 0;
            break;
          }
        }
        if ( mQueueTasks.GetUsedCount() == 0 )
        {
          postCurrent();
        }
      }
    }
    else
    {
      if ( mSolveCount )
      {
        for (HeU32 i=0; i<mSolvePer; i++)
        {
          add(mSolveX,mSolveY);

          mSolveX++;
          if ( mSolveX == mFractalSize )
          {
            mSolveX = 0;
            mSolveY++;
          }

          mSolveCount--;

          if ( mSolveCount == 0 )
          {
            postCurrent();
            break;
          }

        }
      }
    }

    if ( mCurrent )
    {
      postCurrent();
    }

    if ( isFinished() )
    {
      ret = false;
    }

    return ret;
  }

  bool isFinished(void)
  {
    bool ret = true;

    if ( mThreads.GetUsedCount() || mSolveCount || mQueueTasks.GetUsedCount() )
      ret = false;

    return ret;
  }

	bool diffcolor(HeU32 x1,HeU32 y1,HeU32 x2,HeU32 y2,HeU32 &color)
	{
		color = getPixel(x1,y1);
		for (HeU32 x=x1; x<=x2; x++)
		{
			HeU32 c = getPixel(x,y1);
			if ( c != color ) return true;
			c = getPixel(x,y2);
			if ( c != color ) return true;
		}
		for (HeU32 y=y1; y<=y2; y++)
		{
			HeU32 c = getPixel(x1,y);
			if ( c != color ) return true;
			c = getPixel(x2,y);
			if ( c != color ) return true;
		}
		return false;
	}

  void SplitRect(HeU32 x1,HeU32 y1,HeU32 x2,HeU32 y2)
  {
    HeU32 color;


    if ( diffcolor(x1,y1,x2,y2,color) )
    {
      HeU32 xwid = x2-x1;
      HeU32 ywid = y2-y1;
      if ( xwid <= 5 || ywid <= 5 )
      {
        for (HeU32 y=y1; y<=y2; y++)
        {
          for (HeU32 x=x1; x<=x2; x++)
          {
            add(x,y); // add it to be computed...
          }
        }
      }
      else
      {
  			HeU32 xc = x1+(xwid/2);
        HeU32 yc = y1+(ywid/2);

        if (xwid >= ywid)
        {
  				FRAC_vline(xc,y1+1,y2-1);
					FRAC_QueueTask(x1,y1,xc,y2);
					FRAC_QueueTask(xc,y1,x2,y2);
        }
        else
        {
  				FRAC_hline(yc,x1+1,x2-1);
					FRAC_QueueTask(x1,y1,x2,yc);
					FRAC_QueueTask(x1,yc,x2,y2);
        }
      }
    }
    else
    {
      for (HeU32 y=y1; y<=y2; y++)
      {
        for (HeU32 x=x1; x<=x2; x++)
        {
          mFractalBuffer[y*mFractalSize+x] = color;
          mCallback->fractalPixel(this,x,y,color);
        }
      }
    }
  }

  void action(FractalAction fa,bool /*astate*/,HeI32 _mx,HeI32 _my,HeI32 zoomFactor,HeI32 &mdx,HeI32 &mdy)
  {
    BigFloat mx = _mx;
    BigFloat my = _my;

    mx = mx*mFractalSize;
    BigFloat sw(mScreenWidth);
    mx = mx/sw;

		if ( fa != FA_MOUSE_DRAG )
		{
      mx = (mx*mFractalSize)  / (BigFloat)mScreenWidth;
      my = (my*mFractalSize)  / (BigFloat)mScreenHeight;
		}

    cancel(); // cancel all outstanding tasks!

		mdx = 0;
		mdy = 0;

    BigFloat delta = (BigFloat)mFractalScale*16;


    switch ( fa )
    {
      case FA_MOUSE_DRAG:
        mX1+=mFractalScale*(BigFloat)mx;
        mX2+=mFractalScale*(BigFloat)mx;

        mY1+=mFractalScale*(BigFloat)my;
        mY2+=mFractalScale*(BigFloat)my;

        mx.ToInt(mdx);
        my.ToInt(mdy);

        break;
      case FA_LEFT:
        mX1-=delta;
        mX2-=delta;
        mdx = -16;
        mdy = 0;
        break;
      case FA_RIGHT:
        mX1+=delta;
        mX2+=delta;
        mdx = 16;
        mdy = 0;
        break;
      case FA_UP:
        mY1-=delta;
        mY2-=delta;
        mdy = -16;
        mdx = 0;
        break;
      case FA_DOWN:
        mY1+=delta;
        mY2+=delta;
        mdy = 16;
        mdx = 0;
        break;
      case FA_MOUSE_ZOOM_IN:
      case FA_MOUSE_ZOOM_OUT:
      case FA_MOUSE_CENTER:
        {
          BigFloat fscale = 1;
          BigFloat zscale;
          zscale.FromDouble(0.01);
          BigFloat one(1);
          BigFloat minsize;
          minsize.FromDouble(0.05);

          BigFloat zfactor = zscale*(BigFloat)zoomFactor;

          if ( fa == FA_MOUSE_ZOOM_IN )
          {
            fscale = one - zfactor;
            if ( fscale < minsize ) fscale = minsize;
          }
          else if ( fa == FA_MOUSE_ZOOM_OUT )
          {
            fscale = one + zfactor;
          }
          BigFloat cx = mx*mFractalScale+mX1;
          BigFloat cy = my*mFractalScale+mY1;

          BigFloat wid = (mX2-mX1)*fscale;
          BigFloat hit = (mY2-mY1)*fscale;

          BigFloat point5;
          point5.FromDouble(0.5);

          mX1 = cx - wid*point5;
          mX2 = cx + wid*point5;

          mY1 = cy - hit*point5;
          mY2 = cy + hit*point5;
        }
        break;
      case FA_ZOOM_IN:
        {
          BigFloat point5;
          point5.FromDouble(0.5);
          BigFloat p95;
          p95.FromDouble(0.95);

          BigFloat cx = (mX1+mX2)*point5;
          BigFloat cy = (mY1+mY2)*point5;

          BigFloat wid = (mX2-mX1)*p95;
          BigFloat hit = (mY2-mY1)*p95;

          mX1 = cx - wid*point5;
          mX2 = cx + wid*point5;

          mY1 = cy - hit*point5;
          mY2 = cy + hit*point5;

        }
        break;
      case FA_ZOOM_OUT:
        {
          BigFloat point5;
          point5.FromDouble(0.5);
          BigFloat p105;
          p105.FromDouble(1.05);

          BigFloat cx = (mX1+mX2)*point5;
          BigFloat cy = (mY1+mY2)*point5;
          BigFloat wid = (mX2-mX1)*p105;
          BigFloat hit = (mY2-mY1)*p105;
          mX1 = cx - wid*point5;
          mX2 = cx + wid*point5;
          mY1 = cy - hit*point5;
          mY2 = cy + hit*point5;
        }
        break;
    }

//    mdx = 0; mdy = 0;

    mSolveCount = mFractalSize*mFractalSize;

    mSolveX = 0;
    mSolveY = 0;
    mFractalScale = (mX2-mX1) / (BigFloat)mFractalSize;

    if ( (mdx == 0 && mdy == 0 ) )
    {
      memset(mFractalBuffer,0,mMemSize);
    }
    else
    {
      adjustBegin(mdx,mdy);
    }

		if ( mUseRectangleSubdivision )
      begin(0,0,mFractalSize-1,mFractalSize-1);

    frameSetup();

  }

  void adjustBegin(HeI32 mdx,HeI32 mdy)
  {
    memcpy(mFractalBackBuffer,mFractalBuffer,mMemSize);

    for (HeU32 y=0; y<mFractalSize; y++)
    {
      for (HeU32 x=0; x<mFractalSize; x++)
      {

        HeI32 sx = (HeI32)x + mdx;
        HeI32 sy = (HeI32)y + mdy;

        HeU32 color = 0;

        if ( sx >= 0 && sx < (HeI32)mFractalSize && sy >= 0 && sy < (HeI32)mFractalSize )
        {
          HeI32 index = sy*mFractalSize+sx;
          color = mFractalBackBuffer[index];
        }
        mFractalBuffer[y*mFractalSize+x] = color;
      }
    }
  }

  void copy(const Fractal &f,HeI32 mdx,HeI32 mdy)
  {
    mX1 = f.mX1;
    mX2 = f.mX2;
    mY1 = f.mY1;
    mY2 = f.mY2;
    mSolveCount = mFractalSize*mFractalSize;
    mSolveX = 0;
    mSolveY = 0;
    mFractalScale = (mX2-mX1) / (BigFloat)mFractalSize;

		cancel();

    if ( (mdx == 0 && mdy == 0 ) )
    {
			memset(mFractalBuffer,0,mMemSize);
    }
    else
    {
      adjustBegin(mdx,mdy);
    }

		if ( mUseRectangleSubdivision )
      begin(0,0,mFractalSize-1,mFractalSize-1);

    frameSetup();
  }

  void frameSetup(void)
  {
    switch ( gFloatType )
    {
      case MFT_SMALL:
        mFrameSmall.set(mX1,mY1,mX2,mY2,mFractalScale);
        mReferenceFrame = &mFrameSmall;
        break;
      case MFT_MEDIUM:
        mFrameMedium.set(mX1,mY1,mX2,mY2,mFractalScale);
        mReferenceFrame = &mFrameMedium;
        break;
      case MFT_BIG:
        mFrameBig.set(mX1,mY1,mX2,mY2,mFractalScale);
        mReferenceFrame = &mFrameBig;
        break;
      case MFT_FIXED32:
        mFrameFixed32.set(mX1,mY1,mX2,mY2,mFractalScale);
        mReferenceFrame = &mFrameFixed32;
        break;
    }
  }


  void release(FractalThread *ft)
  {
    mActiveJobs--;
    assert( mActiveJobs >= 0 );
    mThreads.Release(ft);
  }

  void cancel(void)
  {

    if ( mCurrent )
    {
      mThreads.Release(mCurrent);
      mCurrent = 0;
    }

    {
      HeI32 count = mThreads.Begin();
      for (HeI32 i=0; i<count; i++)
      {
        FractalThread *ft = mThreads.GetNext();
        if  ( !ft->isCancelled() )
        {
          ft->cancel();
        }
      }
    }


    {
      HeI32 count = mQueueTasks.Begin();
      for (HeI32 i=0; i<count; i++)
      {
        QueueTask *t = mQueueTasks.GetNext();
        mQueueTasks.Release(t);
      }
    }

    {
      for (HeU32 i=0; i<MAX_DIFFICULTY; i++)
        mPending[i] = 0;
    }

  }

	void fractalPixel(HeU32 x,HeU32 y,HeU32 ic)
	{
		HeU32 index = y*mFractalSize+x;
		mFractalBuffer[index] = ic;
		mCallback->fractalPixel(this,x,y,ic);
	}

  void begin(HeU32 x1,HeU32 y1,HeU32 x2,HeU32 y2)
  {

    if ( mUseRectangleSubdivision )
    {
		  mSolveCount = 0;

 		  FRAC_hline(y1,x1,x2);
		  FRAC_hline(y2,x1,x2);
		  FRAC_vline(x1,y1,y2);
		  FRAC_vline(x2,y1,y2);

      FRAC_QueueTask(x1,y1,x2,y2);
    }
  }

  FractalThread * getCurrent(void)
  {
    if ( mCurrent == 0 )
    {
      mCurrent = mThreads.GetFreeLink();
      new ( mCurrent ) FractalThread(this,mReferenceFrame);
    }
    return mCurrent;
  }

  void postCurrent(void)
  {
    if ( mCurrent )
    {
      mCurrent->beginThreadJob();
      mActiveJobs++;
      mCurrent = 0;
    }
  }


  void FRAC_QueueTask(HeU32 x1,HeU32 y1,HeU32 x2,HeU32 y2)
  {
    getCurrent();
    if ( mCurrent->addRect(x1,y1,x2,y2) )
      postCurrent();
  }

  void FRAC_hline(HeU32 y,HeU32 x1,HeU32 x2)
  {
    for (HeU32 x=x1; x<=x2; x++)
    {
      add(x,y);
    }
  }

	HeU32 getPixel(HeU32 x,HeU32 y )
	{
		HeU32 index = y*mFractalSize+x;
		return mFractalBuffer[index];
	}

	HeU32 computeDifficulty(HeU32 x1,HeU32 y1,HeU32 x2,HeU32 y2)
	{
		HeU32 ret = 0;

		HeU32 total=0;
		HeU32 count = 0;
		for (HeU32 x=x1; x<=x2; x++)
		{
			total+=getPixel(x,y1);
			total+=getPixel(x,y2);
			count+=2;
		}
		for (HeU32 y=y1+1; y<=(y2-2); y++)
		{
			total+=getPixel(x1,y);
			total+=getPixel(x2,y);
			count+=2;
		}

		HeU32 mean = total / count;
		HeU32 difficulty = (mean * mDifficultyScale)>>8;
		if ( difficulty >= MAX_DIFFICULTY )
			difficulty = 255;

		return ret;
	}

  void FRAC_vline(HeU32 x,HeU32 y1,HeU32 y2)
  {
    for (HeU32 y=y1; y<=y2; y++)
      add(x,y);
  }

  void add(HeU32 x,HeU32 y)
  {
    HeU32 color = mFractalBuffer[y*mFractalSize+x];
    if ( color == 0 )
    {
      getCurrent();
      if ( mCurrent->add(x,y) )
      {
        postCurrent();
      }
    }
  }

	void setIterationCount(HeU32 icount)
	{
    mMaxIterations = icount;
	}

	void useRectangleSubdivision(bool state)
	{
		mUseRectangleSubdivision = state;
	}

  HeU32 getMaxIterations(void) const { return mMaxIterations; };

  void createQueueTask(RectTask &rt)
  {
    HeU32 difficulty = computeDifficulty(rt.mX1,rt.mY1,rt.mX2,rt.mY2);
    QueueTask *qt = mQueueTasks.GetFreeLink();
    new ( qt ) QueueTask(rt);
    QueueTask *next = mPending[difficulty];
    mPending[difficulty] = qt;
    qt->mNextJob = next;
  }

  void setCoordinates(const BigFloat &xleft,const BigFloat &xright,const BigFloat &ytop)
  {

    cancel(); // cancel all outstanding tasks!


    mX1 = xleft;
    mY1 = ytop;
    mX2 = xright;
    mY2 = mY1+(mX2-mX1);

    mSolveCount = mFractalSize*mFractalSize;
    mSolveX = 0;
    mSolveY = 0;

    mFractalScale = (mX2-mX1) / (BigFloat) mFractalSize;

    frameSetup();

    begin(0,0,mFractalSize-1,mFractalSize-1);

  }

  void getCoordinates(BigFloat &xleft,BigFloat &xright,BigFloat &ytop)
  {
    xleft = mX1;
    xright = mX2;
    ytop = mY1;
  }

  HeU32 * getData(HeU32 &wid,HeU32 &hit)
  {
    wid = mFractalSize;
    hit = mFractalSize;
    return mFractalBuffer;
  }

  void redraw(void)
  {
    const HeU32 *source = mFractalBuffer;
    for (HeU32 y=0; y<mFractalSize; y++)
    {
      for (HeU32 x=0; x<mFractalSize; x++)
      {
        HeU32 color = *source++;
        if ( color )
        {
          mCallback->fractalPixel(this,x,y,color);
        }
      }
    }
  }

  void filter(void)
  {
    if ( mFractalBuffer )
    {
      memset(mFractalBuffer,0xFF,mMemSize);
    }
  }


  void      setFloatingPointResolution(MultiFloatType type)
  {
    gFloatType = type;
  }

private:
  HeU32                 mFractalSize;
  HeU32                 mScreenWidth;
  HeU32                 mScreenHeight;
  HeU32                 mMaxIterations;
	HeU32                 mDifficultyScale;
  HeI32                 mActiveJobs;
  BigFloat              mFractalScale;
  BigFloat              mX1;
  BigFloat              mY1;
  BigFloat              mX2;
  BigFloat              mY2;
  bool                  mUseRectangleSubdivision;
  HeU32                 mSolveCount;
  HeU32                 mSolvePer;
  HeU32                 mSolveX;
  HeU32                 mSolveY;

	FractalInterface     *mCallback;
  FractalThread        *mCurrent;
  Pool< FractalThread > mThreads;
  Pool< QueueTask >     mQueueTasks;

  QueueTask            *mPending[MAX_DIFFICULTY];

  HeU32                 mMemSize;
  HeU32                *mFractalBuffer;
  HeU32                *mFractalBackBuffer;

  Frame                *mReferenceFrame;

  FrameSmall            mFrameSmall;
  FrameMedium           mFrameMedium;
  FrameBig              mFrameBig;
  FrameFixed32          mFrameFixed32;


};


void FractalThread::job_process(void * /*userData*/,int /*userId*/)  // RUNS IN ANOTHER THREAD!! MUST BE THREAD SAFE!
{
  HeU32 maxiter = mParent->getMaxIterations();

  switch ( mLastType )
  {
    case MFT_SMALL:
      {
        FractalPixelSmall *pixel = &mPixelsSmall[0];
        for (HeU32 i=0; i<mCount; i++)
        {
          pixel->solve(maxiter);
          pixel++;
          if ( mCancelled ) break;
        }
      }
      break;
    case MFT_MEDIUM:
      {
        FractalPixelMedium *pixel = &mPixelsMedium[0];
        for (HeU32 i=0; i<mCount; i++)
        {
          pixel->solve(maxiter);
          pixel++;
          if ( mCancelled ) break;
        }
      }
      break;
    case MFT_BIG:
      {
        FractalPixelBig *pixel = &mPixelsBig[0];
        for (HeU32 i=0; i<mCount; i++)
        {
          pixel->solve(maxiter);
          pixel++;
          if ( mCancelled ) break;
        }
      }
      break;
    case MFT_FIXED32:
      {
        FractalPixelFixed32 *pixel = &mPixelsFixed32[0];
        for (HeU32 i=0; i<mCount; i++)
        {
          pixel->solve(maxiter);
          pixel++;
          if ( mCancelled ) break;
        }
      }
      break;
  }
}

void FractalThread::job_onFinish(void * /*userData*/,int /*userId*/)
{

  assert(mSwarmJob);

  switch ( mLastType )
  {
    case MFT_SMALL:
      {
        FractalPixelSmall *pixel = &mPixelsSmall[0];
        for (HeU32 i=0; i<mCount; i++)
        {
          mParent->fractalPixel(pixel->mX,pixel->mY,pixel->mIterationCount);
          pixel++;
        }
      }
      break;
    case MFT_MEDIUM:
      {
        FractalPixelMedium *pixel = &mPixelsMedium[0];
        for (HeU32 i=0; i<mCount; i++)
        {
          mParent->fractalPixel(pixel->mX,pixel->mY,pixel->mIterationCount);
          pixel++;
        }
      }
      break;
    case MFT_BIG:
      {
        FractalPixelBig *pixel = &mPixelsBig[0];
        for (HeU32 i=0; i<mCount; i++)
        {
          mParent->fractalPixel(pixel->mX,pixel->mY,pixel->mIterationCount);
          pixel++;
        }
      }
      break;
    case MFT_FIXED32:
      {
        FractalPixelFixed32 *pixel = &mPixelsFixed32[0];
        for (HeU32 i=0; i<mCount; i++)
        {
          mParent->fractalPixel(pixel->mX,pixel->mY,pixel->mIterationCount);
          pixel++;
        }
      }
      break;
  }

  RectTask *rt = mRectangles;
  for (HeU32 i=0; i<mRectCount; i++)
  {
    mParent->createQueueTask(*rt);
    rt++;
  }

  mParent->release(this);

  mSwarmJob = 0;
}

void FractalThread::job_onCancel(void * /*userData*/,int /*userId*/)
{
  assert(mSwarmJob);
  mParent->release(this);
  mSwarmJob = 0;
}

void FractalThread::cancel(void)
{
  if ( !mCancelled )
  {
    assert(mSwarmJob);
    mCancelled = true;
    gJobSwarmContext->cancel(mSwarmJob);
  }
}


Fractal * fc_create(FractalInterface *iface,HeU32 fractalSize,HeU32 swidth,HeU32 sheight,HeU32 maxIterations,const BigFloat &xleft,const BigFloat &xright,const BigFloat &ytop,bool useRectangleSubdivision)
{
  Fractal *f = MEMALLOC_NEW(Fractal)(iface,fractalSize,swidth,sheight,maxIterations,xleft,xright,ytop,useRectangleSubdivision);
  return f;
}

void      fc_release(Fractal *f)
{
  delete f;
}


bool      fc_process(Fractal *f,HeF32 dtime)
{
  return f->process(dtime);
}


void      fc_action(Fractal *f,FractalAction faction,bool astate,HeI32 mx,HeI32 my,HeI32 zoomFactor,HeI32 &dx,HeI32 &dy)
{
  f->action(faction,astate,mx,my,zoomFactor,dx,dy);
}

Fractal * fc_create(Fractal *f,FractalInterface *iface,HeU32 fractalSize,HeU32 maxIterations)
{
  Fractal *ret = MEMALLOC_NEW(Fractal)(*f,iface,fractalSize,maxIterations);
  return ret;
}

void      fc_copy(const Fractal *source,Fractal *dest,HeI32 mdx,HeI32 mdy)
{
  dest->copy(*source,mdx,mdy);
}

void      fc_setIterationCount(Fractal *f,HeU32 icount)
{
	f->setIterationCount(icount);
}

void      fc_useRectangleSubdivision(Fractal *f,bool state)
{
	f->useRectangleSubdivision(state);
}


void      fc_useThreading(bool state)
{
  gJobSwarmContext->setUseThreads(state);
}

void      fc_redraw(Fractal *f)
{
  f->redraw();
}



void      fc_setCoordinates(Fractal *f,const BigFloat &xleft,const BigFloat &xright,const BigFloat &ytop)
{
  f->setCoordinates(xleft,xright,ytop);
}

void      fc_getCoordinates(Fractal *f,BigFloat &xleft,BigFloat &xright,BigFloat &ytop)
{
  f->getCoordinates(xleft,xright,ytop);
}

HeU32 * fc_getData(Fractal *f,HeU32 &wid,HeU32 &hit)
{
  return f->getData(wid,hit);
}


void      fc_filter(Fractal *f)
{
  f->filter();
}

void      fc_setFloatingPointResolution(Fractal *f,MultiFloatType type)
{
  f->setFloatingPointResolution(type);
}
