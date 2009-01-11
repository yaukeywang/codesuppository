#ifndef TFRAC_H

#define TFRAC_H

#include "fractal.h"
#include <string.h>

enum TfracState
{
  TS_PREVIEW_ONLY,
  TS_ITERATION_COUNT,
	TS_RECTANGLE_SUBDIVISION,
  TS_THREADING,
	TS_CLOCK_CYCLES,
  TS_SMOOTH_COLOR,
  TS_SMALL_JOBS,
  TS_WIREFAME_OVERLAY,
  TS_CLAMP_LOW,
  TS_CLAMP_HIGH,
  TS_CLAMP_SCALE,
  TS_FILTER_FRACTAL,
  TS_OPTIMIZE_MESH,
};

class TfracSettings
{
public:
  TfracSettings(void)
  {
    mXleft.FromDouble(-2.5);
    mXright.FromDouble(0.7f);
    mYtop.FromDouble(-1.5);
    mUseRectangleSubdivision = true;
    mMaxIterations = 1024;
    strcpy(mPalette,"default.pal");
    mPreviewOnly = false;
    mUseThreading = false;
    mSmallJobs = false;
    mWireframeOverlay = false;
    mClampLow = 0;
    mClampHigh = 0.3f;
    mClampScale = 1;
    mClockCycles = 100;
    mSmoothColor = 16;
  }

  BigFloat       mXleft;
  BigFloat       mXright;
  BigFloat       mYtop;
  bool         mPreviewOnly;
  bool         mUseRectangleSubdivision;
  bool         mUseThreading;
  bool         mSmallJobs;
  bool         mWireframeOverlay;
  HeF32        mClampLow;
  HeF32        mClampHigh;
  HeF32        mClampScale;
  HeU32 mMaxIterations;
  char         mPalette[256];
  HeU32 mClockCycles;
  HeU32 mSmoothColor;
};


class Tfrac;

Tfrac * tf_create(HeU32 screenWidth,HeU32 screenHeight,const TfracSettings &settings);
void    tf_release(Tfrac *t);

void    tf_render(Tfrac *t,bool view3d,HeF32 dtime);

void    tf_process(Tfrac *t,HeF32 dtime);
void    tf_setPal(Tfrac *f,const char *fname);

void    tf_action(Tfrac *f,FractalAction action,bool astate,HeI32 mx,HeI32 my,HeI32 zoomFactor=1);


void    tf_state(Tfrac *f,TfracState state,bool astate=false,HeI32 ivalue=0,HeF32 fvalue=0);

bool    tf_togglePreviewOnly(Tfrac *f);
void    tf_setFractalCoordinates(Tfrac *f,const BigFloat &xleft,const BigFloat &xright,const BigFloat &ytop);
void    tf_getFractalCoordinates(Tfrac *f,BigFloat &xleft,BigFloat &xright,BigFloat &ytop);

bool    tf_actionReady(Tfrac *f);

void    tf_getSettings(Tfrac *f,TfracSettings &settings);
void    tf_setSettings(Tfrac *f,const TfracSettings &settings);

void    tf_setFloatingPointResolution(Tfrac *f,MultiFloatType type);

#endif
