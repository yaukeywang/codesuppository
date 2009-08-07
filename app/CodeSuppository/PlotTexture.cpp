#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "PlotTexture.h"
#include "Pd3d/pd3d.h"
#include "UserMemAlloc.h"

class PlotTexture
{
public:
  PlotTexture(const char *fname,NxU32 width,NxU32 height)
  {
    mWidth = width;
    mHeight = height;
    char scratch[512];
    sprintf(scratch,"%s_1", fname );
    mTexture      = gPd3d->createTexture(scratch,width,height,4,true);
    sprintf(scratch,"%s_2", fname );
    mVideoTexture = gPd3d->createTexture(scratch,width,height,4,false);
    mPitch = 0;
    mPixels = 0;
    mBackBuffer = 0;
    if ( mTexture )
    {
      mPixels = (NxU8 *)gPd3d->lockTexture(mTexture,width,height,mPitch);
      mBackBuffer = MEMALLOC_NEW_ARRAY(unsigned char,height*mPitch)[height*mPitch];
    }
  }

  ~PlotTexture(void)
  {
    if ( mTexture )
    {
      gPd3d->unlockTexture(mTexture);
      gPd3d->releaseTexture(mTexture);
    }
    if ( mVideoTexture )
    {
      gPd3d->releaseTexture(mVideoTexture);
    }
    delete []mBackBuffer;
  }

  void plotPixel(NxU32 x,NxU32 y,NxU32 color)
  {
    if ( mPixels )
    {
      NxU32 index = y*mPitch+(x*4);
      NxU32 *dest = (NxU32 *) &mPixels[index];
      dest[0] = color;
    }
  }

  NxU32 getPixel(NxU32 x,NxU32 y)
  {
    NxU32 color = 0;
    if ( mPixels )
    {
      NxU32 index = y*mPitch+(x*4);
      NxU32 *dest = (NxU32 *) &mPixels[index];
      color = dest[0];
    }
    return color;
  }


  void renderScreenQuad(NxU32 x1,NxU32 y1,NxU32 wid,NxU32 hit,NxU32 color)
  {
    if ( mTexture )
    {
      gPd3d->unlockTexture(mTexture);
      gPd3d->copyTexture(mTexture,mVideoTexture);
      mPixels = (NxU8 *)gPd3d->lockTexture(mTexture,mWidth,mHeight,mPitch);

      gPd3d->renderScreenQuad(mVideoTexture,(NxI32)x1,(NxI32)y1,1.0f,(NxI32)wid,(NxI32)hit,color);
      gPd3d->flush();

    }
  }

  PD3D::Pd3dTexture *getTexture(void)
  {
    gPd3d->unlockTexture(mTexture);
    gPd3d->copyTexture(mTexture,mVideoTexture);
    mPixels = (NxU8 *)gPd3d->lockTexture(mTexture,mWidth,mHeight,mPitch);
    return mVideoTexture;
  };

  NxU32 * getSurface(NxU32 &wid,NxU32 &hit,NxU32 &stride)
  {
    wid = mWidth;
    hit = mHeight;
    stride = mPitch;
    return (NxU32 *)mPixels;
  }

  void adjustImage(NxI32 dx,NxI32 dy)
  {
    if ( dx == 0 && dy == 0 )
    {
			memset(mPixels,0,mHeight*mPitch);
    }
    else
    {
      memcpy(mBackBuffer,mPixels,mHeight*mPitch);
			memset(mPixels,0,mHeight*mPitch);

      const NxU32 *source = (NxU32 *)mBackBuffer;
      NxU32 *dest         = (NxU32 *)mPixels;

      NxI32 wid                    = (NxI32) mWidth;
      NxI32 hit                    = (NxI32) mHeight;
      NxI32 pitch                  = (NxI32)mPitch/sizeof(NxU32);

			dx*=-1;
			dy*=-1;

      for (NxI32 y=0; y<hit; y++)
      {
        NxI32 sy = y+dy;

				if ( sy >=0 && sy < hit )
				{
					NxI32 sindex = y*pitch;
    			NxI32 dindex = sy*pitch+dx;
					NxI32 sx = dx;

          for (NxI32 x=0; x<wid; x++)
          {
            if ( sx >= 0 && sx < wid )
            {
							dest[dindex] = source[sindex];
            }
						sindex++;
						dindex++;
						sx++;
					}

        }
      }
    }
  }

private:
  NxU32 mWidth;
  NxU32 mHeight;
  NxU32 mPitch;
  NxU8 *mPixels;
  NxU8 *mBackBuffer;

  PD3D::Pd3dTexture *mTexture;
  PD3D::Pd3dTexture *mVideoTexture;
};

PlotTexture * pt_createPlotTexture(const char *fname,NxU32 width,NxU32 height)
{
  PlotTexture *ret = MEMALLOC_NEW(PlotTexture)(fname,width,height);
  return ret;
}

void          pt_releasePlotTexture(PlotTexture *p)
{
  delete p;
}

void          pt_plotPixel(PlotTexture *p,NxU32 x,NxU32 y,NxU32 color)
{
  p->plotPixel(x,y,color);
}

void          pt_renderScreenQuad(PlotTexture *p,NxU32 x,NxU32 y,NxU32 wid,NxU32 hit,NxU32 color)
{
  p->renderScreenQuad(x,y,wid,hit,color);
}

NxU32 pt_getPixel(PlotTexture *p,NxU32 x,NxU32 y)
{
  return p->getPixel(x,y);
}

PD3D::Pd3dTexture * pt_getTexture(PlotTexture *p) // return the device renderable texture.
{
  return p->getTexture();
}

NxU32 *pt_getSurface(PlotTexture *p,NxU32 &wid,NxU32 &hit,NxU32 &stride)
{
  return p->getSurface(wid,hit,stride);
}

void          pt_adjustImage(PlotTexture *p,NxI32 dx,NxI32 dy)
{
  p->adjustImage(dx,dy);
}
