#ifndef PLOT_TEXTURE_H

#define PLOT_TEXTURE_H

#include "common/snippets/HeSimpleTypes.h"

namespace PD3D
{
  class Pd3dTexture;
};

class PlotTexture;

PlotTexture * pt_createPlotTexture(const char *fname,HeU32 width,HeU32 height);
void          pt_releasePlotTexture(PlotTexture *p);
void          pt_plotPixel(PlotTexture *p,HeU32 x,HeU32 y,HeU32 color);
HeU32  pt_getPixel(PlotTexture *p,HeU32 x,HeU32 y);
void          pt_renderScreenQuad(PlotTexture *p,HeU32 x,HeU32 y,HeU32 wid,HeU32 hit,HeU32 color);
PD3D::Pd3dTexture * pt_getTexture(PlotTexture *p); // return the device renderable texture.

void          pt_adjustImage(PlotTexture *p,HeI32 dx,HeI32 dy);

HeU32 *pt_getSurface(PlotTexture *p,HeU32 &wid,HeU32 &hit,HeU32 &stride);



#endif
