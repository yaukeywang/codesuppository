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
  NxF32        mClampLow;
  NxF32        mClampHigh;
  NxF32        mClampScale;
  NxU32 mMaxIterations;
  char         mPalette[256];
  NxU32 mClockCycles;
  NxU32 mSmoothColor;
};


class Tfrac;

Tfrac * tf_create(NxU32 screenWidth,NxU32 screenHeight,const TfracSettings &settings);
void    tf_release(Tfrac *t);

void    tf_render(Tfrac *t,bool view3d,NxF32 dtime);

void    tf_process(Tfrac *t,NxF32 dtime);
void    tf_setPal(Tfrac *f,const char *fname);

void    tf_action(Tfrac *f,FractalAction action,bool astate,NxI32 mx,NxI32 my,NxI32 zoomFactor=1);


void    tf_state(Tfrac *f,TfracState state,bool astate=false,NxI32 ivalue=0,NxF32 fvalue=0);

bool    tf_togglePreviewOnly(Tfrac *f);
void    tf_setFractalCoordinates(Tfrac *f,const BigFloat &xleft,const BigFloat &xright,const BigFloat &ytop);
void    tf_getFractalCoordinates(Tfrac *f,BigFloat &xleft,BigFloat &xright,BigFloat &ytop);

bool    tf_actionReady(Tfrac *f);

void    tf_getSettings(Tfrac *f,TfracSettings &settings);
void    tf_setSettings(Tfrac *f,const TfracSettings &settings);

void    tf_setFloatingPointResolution(Tfrac *f,MultiFloatType type);

#endif
