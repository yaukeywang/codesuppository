#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common/snippets/UserMemAlloc.h"
#include "PlotTexture.h"
#include "Pd3d/pd3d.h"

class PlotTexture
{
public:
  PlotTexture(const char *fname,HeU32 width,HeU32 height)
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
      mPixels = (HeU8 *)gPd3d->lockTexture(mTexture,width,height,mPitch);
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

  void plotPixel(HeU32 x,HeU32 y,HeU32 color)
  {
    if ( mPixels )
    {
      HeU32 index = y*mPitch+(x*4);
      HeU32 *dest = (HeU32 *) &mPixels[index];
      dest[0] = color;
    }
  }

  HeU32 getPixel(HeU32 x,HeU32 y)
  {
    HeU32 color = 0;
    if ( mPixels )
    {
      HeU32 index = y*mPitch+(x*4);
      HeU32 *dest = (HeU32 *) &mPixels[index];
      color = dest[0];
    }
    return color;
  }


  void renderScreenQuad(HeU32 x1,HeU32 y1,HeU32 wid,HeU32 hit,HeU32 color)
  {
    if ( mTexture )
    {
      gPd3d->unlockTexture(mTexture);
      gPd3d->copyTexture(mTexture,mVideoTexture);
      mPixels = (HeU8 *)gPd3d->lockTexture(mTexture,mWidth,mHeight,mPitch);

      gPd3d->renderScreenQuad(mVideoTexture,(HeI32)x1,(HeI32)y1,1.0f,(HeI32)wid,(HeI32)hit,color);
      gPd3d->flush();

    }
  }

  PD3D::Pd3dTexture *getTexture(void)
  {
    gPd3d->unlockTexture(mTexture);
    gPd3d->copyTexture(mTexture,mVideoTexture);
    mPixels = (HeU8 *)gPd3d->lockTexture(mTexture,mWidth,mHeight,mPitch);
    return mVideoTexture;
  };

  HeU32 * getSurface(HeU32 &wid,HeU32 &hit,HeU32 &stride)
  {
    wid = mWidth;
    hit = mHeight;
    stride = mPitch;
    return (HeU32 *)mPixels;
  }

  void adjustImage(HeI32 dx,HeI32 dy)
  {
    if ( dx == 0 && dy == 0 )
    {
			memset(mPixels,0,mHeight*mPitch);
    }
    else
    {
      memcpy(mBackBuffer,mPixels,mHeight*mPitch);
			memset(mPixels,0,mHeight*mPitch);

      const HeU32 *source = (HeU32 *)mBackBuffer;
      HeU32 *dest         = (HeU32 *)mPixels;

      HeI32 wid                    = (HeI32) mWidth;
      HeI32 hit                    = (HeI32) mHeight;
      HeI32 pitch                  = (HeI32)mPitch/sizeof(HeU32);

			dx*=-1;
			dy*=-1;

      for (HeI32 y=0; y<hit; y++)
      {
        HeI32 sy = y+dy;

				if ( sy >=0 && sy < hit )
				{
					HeI32 sindex = y*pitch;
    			HeI32 dindex = sy*pitch+dx;
					HeI32 sx = dx;

          for (HeI32 x=0; x<wid; x++)
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
  HeU32 mWidth;
  HeU32 mHeight;
  HeU32 mPitch;
  HeU8 *mPixels;
  HeU8 *mBackBuffer;

  PD3D::Pd3dTexture *mTexture;
  PD3D::Pd3dTexture *mVideoTexture;
};

PlotTexture * pt_createPlotTexture(const char *fname,HeU32 width,HeU32 height)
{
  PlotTexture *ret = MEMALLOC_NEW(PlotTexture)(fname,width,height);
  return ret;
}

void          pt_releasePlotTexture(PlotTexture *p)
{
  delete p;
}

void          pt_plotPixel(PlotTexture *p,HeU32 x,HeU32 y,HeU32 color)
{
  p->plotPixel(x,y,color);
}

void          pt_renderScreenQuad(PlotTexture *p,HeU32 x,HeU32 y,HeU32 wid,HeU32 hit,HeU32 color)
{
  p->renderScreenQuad(x,y,wid,hit,color);
}

HeU32 pt_getPixel(PlotTexture *p,HeU32 x,HeU32 y)
{
  return p->getPixel(x,y);
}

PD3D::Pd3dTexture * pt_getTexture(PlotTexture *p) // return the device renderable texture.
{
  return p->getTexture();
}

HeU32 *pt_getSurface(PlotTexture *p,HeU32 &wid,HeU32 &hit,HeU32 &stride)
{
  return p->getSurface(wid,hit,stride);
}

void          pt_adjustImage(PlotTexture *p,HeI32 dx,HeI32 dy)
{
  p->adjustImage(dx,dy);
}
