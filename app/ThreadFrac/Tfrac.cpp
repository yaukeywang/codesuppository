#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <vector>

#include "PlotTexture.h"
#include "Tfrac.h"
#include "fractal.h"
#include "mappal.h"
#include "common/snippets/cycle.h"
#include "common/snippets/log.h"
#include "common/snippets/clock.h"
#include "terrain.h"
#include "common/snippets/JobSwarm.h"

#define PREVIEW_ONLY false

static NxU64 gMaxClockCycles=50000000;

using namespace NVSHARE;

typedef std::vector< NxU32 > PixelVector;

class Tfrac : public FractalInterface
{
public:
  Tfrac(NxU32 screenWidth,NxU32 screenHeight,const TfracSettings &settings)
  {

    gJobSwarmContext = JOB_SWARM::createJobSwarmContext(6);

    _controlfp(_PC_64,_MCW_PC);

    NxU32 size = screenWidth;
    if ( screenHeight > size ) size = screenHeight;
    size = (size/4)*4;


    mTextureOnly           = false;
    mActionWait            = 0;

    mScreenWidth           = screenWidth;
    mScreenHeight          = screenHeight;

    mClampLow              = settings.mClampLow;
    mClampHigh             = settings.mClampHigh;
    mClampScale            = settings.mClampScale;
    mWireframe             = settings.mWireframeOverlay;
    mSmoothColor           = settings.mSmoothColor;
    mPreviewOnly           = settings.mPreviewOnly;
    mIterationCount        = settings.mMaxIterations;
    mUseThreading          = settings.mUseThreading;
    mClockCycles           = settings.mClockCycles;
    gMaxClockCycles = (NxU64) mClockCycles*1000000;

    fc_useThreading(mUseThreading);

    mStartTime             = 0;
    mFinished              = false;
    mFractalSize           = size;
    mPreviewIterationCount = mIterationCount/2;
    mPreviewSize           = mFractalSize/4;
    mStep                  = 1;
    mTexture               = pt_createPlotTexture("fractal",mFractalSize,mFractalSize);
    mPreviewTexture        = pt_createPlotTexture("preview_fractal",mPreviewSize,mPreviewSize);
    mTerrain               = createTerrain(mFractalSize,mFractalSize,pt_getTexture(mTexture));
    mPreviewTerrain        = createTerrain(mPreviewSize,mPreviewSize,pt_getTexture(mPreviewTexture));
    mColorScale            = 1.0f / (NxF32)mIterationCount;
    mPreviewColorScale     = 1.0f / (NxF32)mPreviewIterationCount;

    mFractal               = fc_create(this,mFractalSize,mScreenWidth,mScreenHeight,mIterationCount,settings.mXleft,settings.mXright,settings.mYtop,settings.mUseRectangleSubdivision);
    mPreviewFractal        = fc_create(mFractal,this,mPreviewSize,mPreviewIterationCount);

    terrainClampRange(mTerrain,mClampLow,mClampHigh,mClampScale,mIterationCount);
    terrainClampRange(mPreviewTerrain,mClampLow,mClampHigh,mClampScale,mIterationCount);

    mPreview               = true;
    mMapPal                = 0;

    for (NxU32 y=0; y<mFractalSize; y++)
    {
      for (NxU32 x=0; x<mFractalSize; x++)
      {
        pt_plotPixel(mTexture,x,y,0);
      }
    }

    for (NxU32 y=0; y<mPreviewSize; y++)
    {
      for (NxU32 x=0; x<mPreviewSize; x++)
      {
        pt_plotPixel(mPreviewTexture,x,y,0);
      }
    }

    setPal(settings.mPalette,false);
  }

  ~Tfrac(void)
  {
    fc_release(mFractal);
		fc_release(mPreviewFractal);
    pt_releasePlotTexture(mTexture);
		pt_releasePlotTexture(mPreviewTexture);
    releaseTerrain(mTerrain);
    releaseTerrain(mPreviewTerrain);
    JOB_SWARM::releaseJobSwarmContext(gJobSwarmContext);
  }

  void render(bool view3d,NxF32 dtime)
  {
    if ( view3d )
    {
      if ( mPreview || mPreviewOnly || !mFinished )
      {
        pt_getTexture(mPreviewTexture);
        renderTerrain(mPreviewTerrain,mWireframe,dtime);
      }
      else
      {
        pt_getTexture(mTexture);
        renderTerrain(mTerrain,mWireframe,dtime);
      }
    }
    else
    {
      pt_renderScreenQuad(mPreviewTexture,0,0,1024,768,0xFFFFFFFF);
			if ( !mPreviewOnly && !mPreview )
			{
        pt_renderScreenQuad(mTexture,0,0,1024,768,0xFFFFFFFF);
      }
    }

    if ( mActionWait > 0 )
      mActionWait--;
  }

  virtual void fractalPixel(Fractal *f,NxU32 x,NxU32 y,NxU32 iterationCount)
  {
    NxU32 ic = iterationCount&0xFF;
    NxU32 color = (ic<<16)|(ic<<8)|ic;

    if ( mMapPal )
    {
			color = mMapPal->Get(iterationCount*mSmoothColor);
    }

    color|=0xFF000000;

    if ( f == mFractal )
		{
			if ( iterationCount == mIterationCount )
				color = 0xFF000000;
      pt_plotPixel(mTexture,x,y,color);
      if ( !mTextureOnly )
        plot(mTerrain,x,y,iterationCount);
		}
    else
		{
			if ( iterationCount == (mIterationCount/2) )
				color = 0xFF000000;
      pt_plotPixel(mPreviewTexture,x,y,color);
      if ( !mTextureOnly )
        plot(mPreviewTerrain,x,y,iterationCount);
		}
  }

  void process(NxF32 dtime)
  {
		_controlfp(_PC_64,_MCW_PC);

    gJobSwarmContext->processSwarmJobs();


    Cycle c;
    c.Begin();

    while ( !mFinished )
    {

      if ( mPreview )
      {
        mPreview = fc_process(mPreviewFractal,dtime);
        if ( mPreview == false && mPreviewOnly )
        {
          mFinished = true;
          NxU32 wid,hit;
          terrainComplete(mPreviewTerrain, fc_getData(mPreviewFractal,wid,hit) );
          NxF32 rot = getRotation(mPreviewTerrain);
          setRotation(mTerrain,rot);
        }
      }
      else
      {
        if ( mTexture && mFractal && !mPreviewOnly )
        {
          bool morework = fc_process(mFractal,dtime);
          if ( !morework )
          {
            NxU32 wid,hit;
            terrainComplete(mTerrain, fc_getData(mFractal,wid,hit) );
            NxF32 rot = getRotation(mPreviewTerrain);
            setRotation(mTerrain,rot);
            mFinished = true;
            NxF32 stopTime = CLOCK::getSystemTime();
            NxF32 diff = stopTime-mStartTime;
            gLog->Display("Fractal took: %0.2f seconds to process.\r\n", diff );
          }
        }
      }

      NxU64 cycles = c.End();
      if ( cycles > gMaxClockCycles )
        break;
    }
  }

  void setPal(const char *fname,bool refresh)
  {
    delete mMapPal;
    mMapPal = MEMALLOC_NEW(MapPal)(fname);
    strcpy(mPalette,fname);
    if ( refresh )
    {
      mTextureOnly = true;
      fc_redraw(mPreviewFractal);
      fc_redraw(mFractal);
      mTextureOnly = false;
    }
  }

  void action(FractalAction taction,bool astate,NxI32 mx,NxI32 my,NxI32 zoomFactor)
  {

    NxI32 dx,dy;

    mPreview  = true;
    mFinished = false;

    terrainReset(mTerrain);
    terrainReset(mPreviewTerrain);


    fc_action(mFractal,taction,astate,mx,my,zoomFactor,dx,dy);
    fc_copy(mFractal,mPreviewFractal,dx/4,dy/4);

    if ( !mPreviewOnly )
    {
      pt_adjustImage(mTexture,dx,dy);
    }

    pt_adjustImage(mPreviewTexture,dx/4,dy/4);

    {
      NxU32 wid,hit;
      NxU32 *data = fc_getData(mPreviewFractal,wid,hit);
      for (NxU32 y=0; y<hit; y++)
      {
        for (NxU32 x=0; x<wid; x++)
        {
          plot(mPreviewTerrain,x,y,*data++);
        }
      }
    }


    {
      NxU32 wid,hit;
      NxU32 *data = fc_getData(mFractal,wid,hit);
      for (NxU32 y=0; y<hit; y++)
      {
        for (NxU32 x=0; x<wid; x++)
        {
          plot(mTerrain,x,y,*data++);
        }
      }
    }


    mStartTime = CLOCK::getSystemTime();

  }

  void    state(TfracState state,bool astate,NxI32 ivalue,NxF32 fvalue)
  {
    bool refresh = false;


    switch ( state )
    {
      case TS_OPTIMIZE_MESH:
        if ( mFinished && !mPreview )
        {
          optimize(mTerrain);
        }
        break;
      case TS_FILTER_FRACTAL:
        if ( mFinished && !mPreview )
        {
          filter(mTerrain);
        }
        break;
      case TS_SMOOTH_COLOR:
        mSmoothColor = ivalue;
        mTextureOnly = true;
        fc_redraw(mFractal);
        fc_redraw(mPreviewFractal);
        mTextureOnly = false;
        break;
		  case TS_RECTANGLE_SUBDIVISION:
				fc_useRectangleSubdivision(mFractal,astate);
				fc_useRectangleSubdivision(mPreviewFractal,astate);
				refresh = true;
				break;
      case TS_PREVIEW_ONLY:
        mPreviewOnly = astate;
        refresh = true;
        break;
      case TS_THREADING:
        mUseThreading = astate;
        fc_useThreading(astate);
        refresh = true;
        break;
			case TS_CLOCK_CYCLES:
        mClockCycles = ivalue;
        gMaxClockCycles = ivalue*1000000;
				break;
      case TS_WIREFAME_OVERLAY:
        mWireframe = astate;
        break;
      case TS_CLAMP_LOW:
      case TS_CLAMP_HIGH:
      case TS_CLAMP_SCALE:
		  case TS_ITERATION_COUNT:
        if ( state == TS_CLAMP_LOW )
          mClampLow = fvalue;
        else if ( state == TS_CLAMP_HIGH )
          mClampHigh = fvalue;
        else if ( state == TS_CLAMP_SCALE )
          mClampScale = fvalue;
        else
        {
  				mIterationCount = ivalue;
  				fc_setIterationCount(mFractal,ivalue);
  				fc_setIterationCount(mPreviewFractal,ivalue/2);
        }
        terrainClampRange(mTerrain,mClampLow,mClampHigh,mClampScale,mIterationCount);
        terrainClampRange(mPreviewTerrain,mClampLow,mClampHigh,mClampScale,mIterationCount);
        break;
    }
    if ( refresh )
    {
      mFinished = false;
      mPreview = true;
      action(FA_MOUSE_CENTER,false,mScreenWidth/2,mScreenHeight/2,1);
      mStartTime = CLOCK::getSystemTime();
    }
  }


  bool togglePreviewOnly(void)
  {
    mPreviewOnly = mPreviewOnly ? false : true;
    return mPreviewOnly;
  }

  void redraw(void)
  {
    fc_redraw(mPreviewFractal);
    fc_redraw(mFractal);
  }

  void setFractalCoordinates(const BigFloat &xleft,const BigFloat &xright,const BigFloat &ytop)
  {
    fc_setCoordinates(mPreviewFractal,xleft,xright,ytop);
    fc_setCoordinates(mFractal,xleft,xright,ytop);
    mFinished = false;
    mPreview = true;
    mStartTime = CLOCK::getSystemTime();
    action(FA_MOUSE_CENTER,false,mScreenWidth/2,mScreenHeight/2,1);
  }

  void getFractalCoordinates(BigFloat &xleft,BigFloat &xright,BigFloat &ytop)
  {
    fc_getCoordinates(mFractal,xleft,xright,ytop);
  }

  bool actionReady(void) const
  {
    bool ret = false;
    if ( mActionWait == 0 ) ret = true;
    return ret;
  }

  void getSettings(TfracSettings &settings)
  {
    fc_getCoordinates(mFractal,settings.mXleft,settings.mXright,settings.mYtop);
    settings.mClampLow = mClampLow;
    settings.mClampHigh = mClampHigh;
    settings.mClampScale = mClampScale;
    settings.mWireframeOverlay = mWireframe;
    settings.mSmoothColor = mSmoothColor;
    settings.mPreviewOnly = mPreviewOnly;
    settings.mMaxIterations = mIterationCount;
    settings.mUseThreading = mUseThreading;
    strcpy(settings.mPalette,mPalette);
  }

  void setSettings(const TfracSettings &settings)
  {
    setFractalCoordinates(settings.mXleft,settings.mXright,settings.mYtop);
    setPal(settings.mPalette,false);
  }

  void    setFloatingPointResolution(MultiFloatType type)
  {
    fc_setFloatingPointResolution(mFractal,type);
    fc_setFloatingPointResolution(mPreviewFractal,type);

    mFinished = false;
    mPreview = true;
    action(FA_MOUSE_CENTER,false,mScreenWidth/2,mScreenHeight/2,1);
    mStartTime = CLOCK::getSystemTime();
  }

private:
  NxI32          mActionWait;
  bool         mUseThreading;
  bool         mTextureOnly;
  bool         mWireframe;
  bool         mPreview;
  bool         mFinished;
  bool         mPreviewOnly;
  NxU32 mClockCycles;
  NxU32 mSmoothColor;
  NxU32 mStep;
  NxU32 mFractalSize;
  NxU32 mIterationCount;
  NxF32        mStartTime;
  PlotTexture  *mTexture;
  Fractal      *mFractal;
  MapPal       *mMapPal;
  NxF32         mColorScale;
  NxF32         mPreviewColorScale;
  NxF32         mClampLow;
	NxF32         mClampHigh;
  NxF32         mClampScale;
  NxU32  mPreviewSize;
  NxU32  mPreviewIterationCount;
  PlotTexture  *mPreviewTexture;
  Fractal      *mPreviewFractal;
  Terrain      *mTerrain;
  Terrain      *mPreviewTerrain;
  NxU32  mScreenWidth;
  NxU32  mScreenHeight;
  char          mPalette[256];
};


Tfrac * tf_create(NxU32 screenWidth,NxU32 screenHeight,const TfracSettings &settings)
{
  Tfrac *ret = MEMALLOC_NEW(Tfrac)(screenWidth,screenHeight,settings);
  return ret;
}

void    tf_release(Tfrac *t)
{
  delete t;
}

void    tf_render(Tfrac *t,bool view3d,NxF32 dtime)
{
  t->render(view3d,dtime);
}

void    tf_process(Tfrac *t,NxF32 dtime)
{
  t->process(dtime);
}


void    tf_setPal(Tfrac *f,const char *fname)
{
  f->setPal(fname,true);
}


void    tf_action(Tfrac *f,FractalAction taction,bool astate,NxI32 mx,NxI32 my,NxI32 zoomFactor)
{
  f->action(taction,astate,mx,my,zoomFactor);
}


void    tf_state(Tfrac *f,TfracState state,bool astate,NxI32 ivalue,NxF32 fvalue)
{
  f->state(state,astate,ivalue,fvalue);
}

bool    tf_togglePreviewOnly(Tfrac *f)
{
  return f->togglePreviewOnly();
}

void    tf_setFractalCoordinates(Tfrac *f,const BigFloat &xleft,const BigFloat &xright,const BigFloat &ytop)
{
  f->setFractalCoordinates(xleft,xright,ytop);
}
void    tf_getFractalCoordinates(Tfrac *f,BigFloat &xleft,BigFloat &xright,BigFloat &ytop)
{
  f->getFractalCoordinates(xleft,xright,ytop);
}

bool tf_actionReady(Tfrac *f)
{
  return f->actionReady();
}

void    tf_getSettings(Tfrac *tf,TfracSettings &settings)
{
  tf->getSettings(settings);
}

void    tf_setSettings(Tfrac *tf,const TfracSettings &settings)
{
  tf->setSettings(settings);
}

void    tf_setFloatingPointResolution(Tfrac *f,MultiFloatType type)
{
  f->setFloatingPointResolution(type);
}
