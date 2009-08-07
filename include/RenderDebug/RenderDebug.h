/* HeroEngine, Copyright 2002-2008 Simutronics, Corp. All rights reserved. */


/*!
 * \file RenderDebug.h
 *
 * \brief
 * Write brief comment for RenderDebug.h here.
 *
 * Write detailed description for RenderDebug.h here.
 */

#ifndef RENDERDEBUG_H

#define RENDERDEBUG_H

#include "NxMat44.h"
#include "BaseRenderDebug.h"

namespace PD3D
{
  class Pd3d;
}

namespace HEGRDRIVER
{
  class HeGrDriver;
  class HeGrShader;
};

namespace HETEXTUREMANAGER
{
  class HeTextureManager;
}

namespace RENDER_DEBUG
{

/*!
 * \brief
 * Write brief comment for RenderDebug here.
 *
 * Write detailed description for RenderDebug here.
 *
 * \remarks
 * Write remarks for RenderDebug here.
 *
 * \see
 * Separate items with the '|' character.
 */
class RenderDebug : public BaseRenderDebug
{
public:

  virtual void setPd3d(PD3D::Pd3d *p) = 0;
  virtual void Initialize( HEGRDRIVER::HeGrDriver* driver, HETEXTUREMANAGER::HeTextureManager* textureManager ) = 0;
  virtual void Render(NxF32 dtime, bool flush, bool zPass) = 0;
  

private:
};

}; // end of namespace

#define RENDER_DEBUG_VERSION 5

extern RENDER_DEBUG::RenderDebug *gRenderDebug;


#endif
