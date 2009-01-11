#ifndef FRACTAL_H

#define FRACTAL_H

#include "MultiFloat.h"

enum FractalAction
{
  FA_ZOOM_IN,
  FA_ZOOM_OUT,
  FA_LEFT,
  FA_RIGHT,
  FA_UP,
  FA_DOWN,
  FA_MOUSE_ZOOM_IN,
  FA_MOUSE_ZOOM_OUT,
  FA_MOUSE_CENTER,
  FA_MOUSE_DRAG,
  FA_LAST
};

class Fractal;

class FractalInterface
{
public:
  virtual void fractalPixel(Fractal *f,HeU32 x,HeU32 y,HeU32 iterationCount) = 0;
};



Fractal * fc_create(FractalInterface *iface,
                    HeU32 fractalSize,
                    HeU32 swidth,
                    HeU32 sheight,
                    HeU32 maxIterations,
                    const BigFloat &xleft,
                    const BigFloat &xright,
                    const BigFloat &ytop,
                    bool useRectangleSubdivision);

Fractal * fc_create(Fractal *f,FractalInterface *iface,HeU32 fractalSize,HeU32 maxIterations);

void      fc_release(Fractal *f);

bool      fc_process(Fractal *f,HeF32 dtime); // returns true to keep processing, false if completed.

void      fc_action(Fractal *f,FractalAction action,bool astate,HeI32 mouseX,HeI32 mouseY,HeI32 zoomFactor,HeI32 &dx,HeI32 &dy);

void      fc_copy(const Fractal *source,Fractal *dest,HeI32 mdx,HeI32 mdy);

void      fc_useThreading(bool state);

void      fc_setIterationCount(Fractal *f,HeU32 icount);

void      fc_useRectangleSubdivision(Fractal *f,bool state);

void      fc_redraw(Fractal *f);

void      fc_setCoordinates(Fractal *f,const BigFloat &xleft,const BigFloat &xright,const BigFloat &ytop);

void      fc_getCoordinates(Fractal *f,BigFloat &xleft,BigFloat &xright,BigFloat &ytop);

void      fc_redraw(Fractal *f);

HeU32 * fc_getData(Fractal *f,HeU32 &wid,HeU32 &hit);

void      fc_filter(Fractal *f);

void      fc_setFloatingPointResolution(Fractal *f,MultiFloatType type);

#endif
