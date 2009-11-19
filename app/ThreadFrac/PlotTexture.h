#ifndef PLOT_TEXTURE_H

#define PLOT_TEXTURE_H

#include "common/snippets/UserMemAlloc.h"

namespace NVSHARE
{
  class Pd3dTexture;
};

class PlotTexture;

PlotTexture * pt_createPlotTexture(const char *fname,NxU32 width,NxU32 height);
void          pt_releasePlotTexture(PlotTexture *p);
void          pt_plotPixel(PlotTexture *p,NxU32 x,NxU32 y,NxU32 color);
NxU32  pt_getPixel(PlotTexture *p,NxU32 x,NxU32 y);
void          pt_renderScreenQuad(PlotTexture *p,NxU32 x,NxU32 y,NxU32 wid,NxU32 hit,NxU32 color);
NVSHARE::Pd3dTexture * pt_getTexture(PlotTexture *p); // return the device renderable texture.

void          pt_adjustImage(PlotTexture *p,NxI32 dx,NxI32 dy);

NxU32 *pt_getSurface(PlotTexture *p,NxU32 &wid,NxU32 &hit,NxU32 &stride);



#endif
