#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

#include "UserMemAlloc.h"
#include "MandelBulb.h"
#include "Tfrac.h"
#include "RenderDebug.h"
#include "NxVec3.h"

#pragma warning(disable:4100 4189)


class MandelBulbCompute : public NVSHARE::Memalloc
{
public:
	MandelBulbCompute(TfracSettings *settings)
  	{
  		mX1 = getDouble( settings->mXleft );
  		mDx = mDy = mDz = getDouble( settings->mXright ) - mX1;
  		mY1 = getDouble( settings->mYtop );
  		mZ1 = mX1;
  		mXloc = mYloc = mZloc = 0;
  		mComplete = false;

  		mGridX = 64;
  		mGridY = 64;
  		mGridZ = 64;

		mDeltaX = mDx / (NxF64)(mGridX);
		mDeltaY = mDy / (NxF64)(mGridY);
		mDeltaZ = mDz / (NxF64)(mGridZ);
  	}

	NxU32 solvePixel(NxU32 _x,NxU32 _y,NxU32 _z)
	{
	}

	NxU32 computeMandelBulb(NxF64 x0,NxF64 y0,NxF64 z0)
	{
#define n 8
       NxF64 x = 0.0;
       NxF64 y = 0.0;
       NxF64 z = 0.0;

       for (int i = 0; i < 254; i++)
       {
           NxF64 r = sqrt(x*x + y*y + z*z);
           NxF64 theta = atan2(sqrt(x*x + y*y), z);
           NxF64 phi = atan2(y, x);

           x = pow(r, n) * sin(theta * n) * cos(phi * n) + x0;
           y = pow(r, n) * sin(theta * n) * sin(phi * n) + y0;
           z = pow(r, n) * cos(theta*n);

           if (x*x + y*y + z*z > 2)
           {
               return 255 - i;
           }
       }
       return 0;
	}

	bool process(void)
	{
		if ( !mComplete )
		{
			NxF64 x = (NxF64)mXloc * mDeltaX + mX1;
			NxF64 y = (NxF64)mYloc * mDeltaY + mY1;
			NxF64 z = (NxF64)mZloc * mDeltaZ + mZ1;
			NxU32 count = computeMandelBulb(x,y,z);
			if ( count != 0 )
			{
				NxVec3 v1( (NxF32)x, (NxF32)y, (NxF32)z );
				NxVec3 v2( v1.x+(NxF32)mDeltaX, v1.y+(NxF32)mDeltaY, v1.z+(NxF32)mDeltaZ);
				v1*=10.0f;
				v2*=10.0f;
				NVSHARE::gRenderDebug->pushRenderState();

				NxU32 color = (count<<16)|(count<<8)|count;
				NVSHARE::gRenderDebug->setCurrentColor(color,color);
				NVSHARE::gRenderDebug->setCurrentDisplayTime(1000.0f);
				NVSHARE::gRenderDebug->DebugBound(&v1.x,&v2.x);
				NVSHARE::gRenderDebug->popRenderState();
			}
    		mXloc++;
    		if ( mXloc == mGridX )
    		{
    			mXloc = 0;
    			mYloc++;
    			if ( mYloc == mGridY )
    			{
    				mYloc = 0;
    				mZloc++;
    				if ( mZloc == mGridZ )
    				{
    					mComplete = true;
    				}
    			}
    		}
    	}
    	return mComplete;
	}



	bool	mComplete;
	NxU32	mXloc;
	NxU32	mYloc;
	NxU32	mZloc;

	NxU32	mGridX;
	NxU32	mGridY;
	NxU32	mGridZ;

	NxF64	mDeltaX;
	NxF64	mDeltaY;
	NxF64	mDeltaZ;

	NxF64	mDx;
	NxF64	mDy;
	NxF64	mDz;

	NxF64	mX1;
	NxF64	mY1;
	NxF64	mZ1;
};

class MandelBulb : public iMandelBulb, public NVSHARE::Memalloc
{
public:
	MandelBulb(void)
	{
		mMandelBulbCompute = 0;
	}

	~MandelBulb(void)
	{
		delete mMandelBulbCompute;
	}

	virtual void	render(TfracSettings *settings)
	{
		if ( mMandelBulbCompute == 0 )
		{
			mMandelBulbCompute = MEMALLOC_NEW(MandelBulbCompute)(settings);
		}
		for (NxU32 i=0; i<1000; i++)
		{
			bool complete = mMandelBulbCompute->process();
			if ( complete )
			{
				delete mMandelBulbCompute;
				mMandelBulbCompute = 0;
				break;
			}
		}
	}



	MandelBulbCompute	*mMandelBulbCompute;

};

iMandelBulb * createMandelBulb(void)
{
	MandelBulb *mb = MEMALLOC_NEW(MandelBulb);
	return static_cast< iMandelBulb *>(mb);
}

void releaseMandelBulb(iMandelBulb *b)
{
	MandelBulb *m = static_cast< MandelBulb *>(b);
	delete m;
}

