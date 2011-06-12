#define NOMINMAX
#include <d3d9.h>
#include <d3dx9.h>
#include <d3d9types.h>
#include <ddraw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "UserMemAlloc.h"
#include "pd3d/pd3d.h"
#include "zvidcap.h"
#include "AgScreenPipe.h"
#include "stringdict.h"

#include <map>

#pragma warning(disable:4324) // warning about structure padding, really don't care...
#pragma warning(disable:4100)


/*!
**
** Copyright (c) 2007 by John W. Ratcliff mailto:jratcliff@infiniplex.net
**
** Portions of this source has been released with the PhysXViewer application, as well as
** Rocket, CreateDynamics, ODF, and as a number of sample code snippets.
**
** If you find this code useful or you are feeling particularily generous I would
** ask that you please go to http://www.amillionpixels.us and make a donation
** to Troy DeMolay.
**
** DeMolay is a youth group for young men between the ages of 12 and 21.
** It teaches strong moral principles, as well as leadership skills and
** public speaking.  The donations page uses the 'pay for pixels' paradigm
** where, in this case, a pixel is only a single penny.  Donations can be
** made for as small as $4 or as high as a $100 block.  Each person who donates
** will get a link to their own site as well as acknowledgement on the
** donations blog located here http://www.amillionpixels.blogspot.com/
**
** If you wish to contact me you can use the following methods:
**
** Skype Phone: 636-486-4040 (let it ring a long time while it goes through switches)
** Skype ID: jratcliff63367
** Yahoo: jratcliff63367
** AOL: jratcliff1961
** email: jratcliff@infiniplex.net
** Personal website: http://jratcliffscarab.blogspot.com
** Coding Website:   http://codesuppository.blogspot.com
** FundRaising Blog: http://amillionpixels.blogspot.com
** Fundraising site: http://www.amillionpixels.us
** New Temple Site:  http://newtemple.blogspot.com
**
**
** The MIT license:
**
** Permission is hereby granted, MEMALLOC_FREE of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is furnished
** to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.

** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
** WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/



#include "wood.h"
#include "water.h"
#include "terrain.h"
#include "shader.h"
#include "ResourceInterface.h"

//RESOURCE_INTERFACE::ResourceInterface *gResourceInterface=0;
//NVSHARE::Pd3d *gPd3d=0;

#ifdef WIN32
#ifdef PD3D_EXPORTS
#define PD3D_API __declspec(dllexport)
#else
#define PD3D_API __declspec(dllimport)
#endif
#else
#define PD3D_API
#endif

bool doShutdown(void);

namespace NVSHARE
{

  enum EmbedTexture
  {
    ET_WOOD,
    ET_TERRAIN,
    ET_WATER
  };



#define D3DFVF_TETRAVERTEX         ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEX2| D3DFVF_TEX3 )
#define D3DFVF_LINEVERTEX       ( D3DFVF_XYZ | D3DFVF_DIFFUSE )
#define D3DFVF_SOLIDVERTEX       ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE )
#define D3DFVF_SCREENVERTEX       ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE )

#define MAXLINES 32768

class Pd3dTexture;
class MyPd3d;

void DejaDescriptor(const Pd3dTexture &obj);
void DejaDescriptor(const MyPd3d &obj);


class Pd3dTexture : public AgScreenPipe, public Memalloc
{
public:
  Pd3dTexture(const StringRef &name,NxU32 width,NxU32 height,NxI32 depth,bool systemRam)
  {
    mHandle = 0;
    mName = name;
    createTexture(width,height,depth,systemRam);
  }

	Pd3dTexture(const StringRef &name)
	{
		mHandle = 0;
		mName   = name;
    mIsOk = false;
    mWidth = 0;
    mHeight = 0;
    loadTexture();
  }

  bool loadTexture(void)
  {

    mIsOk = false;

    static StringRef wood = SGET("wood.dds");
    static StringRef water = SGET("water.dds");
    static StringRef terrain = SGET("terrain.dds");
    if ( mName == wood )
    {
      mHandle = getEmbedHandle(ET_WOOD);
      mIsOk = true;
    }
    else if ( mName == water )
    {
      mHandle = getEmbedHandle(ET_WATER);
      mIsOk = true;
    }
    else if ( mName == terrain )
    {
      mHandle = getEmbedHandle(ET_TERRAIN);
      mIsOk = true;
    }
    else
    {
      RESOURCE_INTERFACE::RESOURCE_HANDLE rhandle = 0;
      NxU32 len = 0;
      void *mem        = 0;

      if ( gResourceInterface && strlen(mName.Get()) )
      {
        rhandle = gResourceInterface->getResource(mName.Get(),0,0,RESOURCE_INTERFACE::RIF_NONE);
        if ( rhandle )
        {
          RESOURCE_INTERFACE::ResourceInfo info;
          gResourceInterface->getResourceInfo(rhandle,info);
          if ( info.mData )
          {
            mem = info.mData;
            len = (NxU32)info.mLen;
            mIsOk = true;
          }
        }
      }

      LPDIRECT3DDEVICE9 device =(LPDIRECT3DDEVICE9) gPd3d->getDevice();

      if ( !mIsOk  )
      {
        mHandle = getEmbedHandle(ET_WOOD);
      }
      else
      {
   		  LPDIRECT3DTEXTURE9 pptex=0;
        if ( device )
      	  D3DXCreateTextureFromFileInMemory(device,(LPCVOID)mem,(UINT)len, &pptex);
  		  mHandle = pptex;
        if ( mHandle == 0 ) mIsOk = false;
      }
	  calcDimensions();

      if ( rhandle )
      {
        gResourceInterface->releaseResource(rhandle,RESOURCE_INTERFACE::RU_RELEASE_NO_UPDATE);
      }

      if ( mIsOk )
        mWasOk = true;
    }

    return mIsOk;
	}

	~Pd3dTexture(void)
	{

    release();

  }

  void calcDimensions(void)
  {
    if ( mHandle )
    {
		LPDIRECT3DTEXTURE9 pptex=(LPDIRECT3DTEXTURE9) mHandle;
		D3DSURFACE_DESC desc;
		HRESULT hr = pptex->GetLevelDesc( 0, &desc );
		assert( hr == D3D_OK );
		if ( hr == D3D_OK )
		{
			mWidth = desc.Width;
			mHeight = desc.Height;
		}
    }
  }

  void release(void)
  {

    if ( mHandle && !isEmbedHandle(mHandle) )
    {
   		LPDIRECT3DTEXTURE9 pptex=(LPDIRECT3DTEXTURE9) mHandle;
      pptex->Release();
      mHandle = 0;
    }
    else
    {
      mHandle = 0;
    }
	}

  static void releaseEmbedHandle(void)
  {

    if ( mWoodHandle )
    {
   		LPDIRECT3DTEXTURE9 pptex=(LPDIRECT3DTEXTURE9) mWoodHandle;
      pptex->Release();
      mWoodHandle = 0;
    }

    if ( mWaterHandle )
    {
   		LPDIRECT3DTEXTURE9 pptex=(LPDIRECT3DTEXTURE9) mWaterHandle;
      pptex->Release();
      mWaterHandle = 0;
    }

    if ( mTerrainHandle )
    {
   		LPDIRECT3DTEXTURE9 pptex=(LPDIRECT3DTEXTURE9) mTerrainHandle;
      pptex->Release();
      mTerrainHandle = 0;
    }

  }

	const StringRef& GetName(void) const
	{

		return mName;
	};

  LPDIRECT3DTEXTURE9 getHandle(void)
  {

    void *ret = 0;

    if ( mHandle == 0 )
    {
      if ( mWasOk )
        loadTexture();
    }
    ret = mHandle;
    if ( ret == 0 ) ret = mWoodHandle;
    LPDIRECT3DTEXTURE9 pptex=(LPDIRECT3DTEXTURE9) ret;
    return pptex;
  }

	void Flush(void) // virtual method inherited from the screen pipeline.  Flush array of screen aligned quads using this texture state.
	{

    gPd3d->renderSection(this,(Pd3dScreenVertex *)mVertices,mIndices,mVcount,(mDest-mIndices)/3 );
    AgScreenPipe::Flush();
	}

  bool isOk(void) const { return mIsOk; };

  bool createTexture(NxU32 width,NxU32 height,NxU32 depth,bool systemRam)
  {
    bool ret = false;

    release();

    mWidth = width;
    mHeight = height;
    mDepth = depth;

    LPDIRECT3DDEVICE9 device =(LPDIRECT3DDEVICE9) gPd3d->getDevice();
    LPDIRECT3DTEXTURE9 pptex=0;

    D3DFORMAT format;
    if ( depth == 3 )
      format = D3DFMT_A8R8G8B8;
    else
      format = D3DFMT_A8R8G8B8;             

    D3DPOOL pool = D3DPOOL_DEFAULT;
    if ( systemRam )
    {
      pool = D3DPOOL_SYSTEMMEM;
    }

    device->CreateTexture(mWidth,mHeight,1,D3DUSAGE_DYNAMIC,format,pool,&pptex,NULL);
    mHandle = pptex;
    if ( mHandle )
      ret = true;


    return ret;
  }

  void * lock(NxU32 &width,NxU32 &height,NxU32 &pitch)
  {

    void *ret = 0;
    width = 0;
    height = 0;

    if ( mHandle )
    {
      LPDIRECT3DTEXTURE9 pptex=(LPDIRECT3DTEXTURE9) mHandle;
      D3DLOCKED_RECT rect;
      HRESULT result = pptex->LockRect(0,&rect,0,D3DLOCK_DISCARD);
      if ( result == D3D_OK )
      {
        ret = rect.pBits;
        pitch = rect.Pitch;
        width = mWidth;
        height = mHeight;
      }
    }

    return ret;
  }

  void unlock(void)
  {

    if ( mHandle )
    {
      LPDIRECT3DTEXTURE9 pptex=(LPDIRECT3DTEXTURE9) mHandle;
      pptex->UnlockRect(0);
    }
  }

  const StringRef& getName(void) const { return mName; };
  NxU32 getWidth(void) const { return mWidth; }
  NxU32 getHeight(void) const { return mHeight; }

  static LPDIRECT3DTEXTURE9 getWoodHandle(void)
  {
    LPDIRECT3DTEXTURE9 pptex=(LPDIRECT3DTEXTURE9) mWoodHandle;
    return pptex;
  }

  static bool isEmbedHandle(void *handle)
  {
    bool ret = false;
    if ( handle )
    {
      if ( handle == mWoodHandle ||
           handle == mWaterHandle ||
           handle == mTerrainHandle )
      {
        ret = true;
      }
    }
    return ret;
  }

  static LPDIRECT3DTEXTURE9 getEmbedHandle(EmbedTexture type)
  {
    LPDIRECT3DTEXTURE9 pptex=0;

    LPDIRECT3DDEVICE9 device =(LPDIRECT3DDEVICE9) gPd3d->getDevice();


    switch( type )
    {
      case ET_WOOD:
        if ( mWoodHandle == 0 )
        {
          size_t len = sizeof(g_wood);
          void *mem        = g_wood;
       		LPDIRECT3DTEXTURE9 pptex=0;
          if ( device )
           	D3DXCreateTextureFromFileInMemory(device,(LPCVOID)mem,(UINT)len, &pptex);
        	mWoodHandle =  pptex;
        }
        pptex=(LPDIRECT3DTEXTURE9) mWoodHandle;
        break;
      case ET_TERRAIN:
        if ( mTerrainHandle == 0 )
        {
          size_t len = sizeof(g_terrain);
          void *mem        = g_terrain;
       		LPDIRECT3DTEXTURE9 pptex=0;
          if ( device )
           	D3DXCreateTextureFromFileInMemory(device,(LPCVOID)mem,(UINT)len, &pptex);
        	mTerrainHandle =  pptex;
        }
        pptex=(LPDIRECT3DTEXTURE9) mTerrainHandle;
        break;
      case ET_WATER:
        if ( mWaterHandle == 0 )
        {
          size_t len = sizeof(g_water);
          void *mem        = g_water;
       		LPDIRECT3DTEXTURE9 pptex=0;
          if ( device )
           	D3DXCreateTextureFromFileInMemory(device,(LPCVOID)mem,(UINT)len, &pptex);
        	mWaterHandle =  pptex;
        }
        pptex=(LPDIRECT3DTEXTURE9) mWaterHandle;
        break;
    }
    return pptex;
  }



private:
	StringRef mName;
	void  *mHandle;     // the actual texture handle
  bool   mIsOk;
  bool   mWasOk;   // means it was ok at one time.
  NxU32    mWidth;
  NxU32    mHeight;
  NxU32    mDepth;

static void *mWoodHandle;
static void *mWaterHandle;
static void *mTerrainHandle;
};

void *Pd3dTexture::mWoodHandle = 0;
void *Pd3dTexture::mWaterHandle = 0;
void *Pd3dTexture::mTerrainHandle = 0;

typedef std::map<StringRef, Pd3dTexture  *>      Pd3dTextureMap;

static void __cdecl MyMessageBox(const char *fmt, ...)
{
	char buff[2048];
	#if defined(WIN32)
	#if defined(UNICODE)
	wchar_t wbuff[2048];
	_vsnprintf(buff,2047, fmt, (char *)(&fmt+1));
	mbstowcs(wbuff,buff,2048);
	MessageBox(0,wbuff,L"Error",MB_OK | MB_ICONEXCLAMATION);
	#else
	MessageBox(0,buff,"Error",MB_OK | MB_ICONEXCLAMATION);
	#endif
	#elif defined(_XBOX)
	NxI32 dprintf(const char* format,...);
	dprintf("Error : [%s]\n",buff);
	assert(false);
	#else
	assert(false);
	#endif
}

// A Pimple class (pointer to implementation) to keep from polluting the public header file
class MyPd3d : public Pd3d, public Memalloc
{
public:
  MyPd3d(void)
  {

    mStateBlock = 0;
    mDevice = 0;
    mEffect = 0;
    mTechnique = 0;
	mLightMapTechnique = 0;
    mWorldViewHandle = 0;
    mWorldViewProjectionHandle = 0;
    mDiffuseHandle = 0;
    mLightMapHandle = 0;
    mDetailMapHandle = 0;
    mWorldHandle = 0;
    mViewProjectionHandle = 0;
    mProjectionHandle = 0;
    mEyePosHandle = 0;
    mWireFrame = false;
    mFractalMode = false;
    mWireFrameTechnique = 0;
    mFractalTechnique = 0;
    mSolidTechnique = 0;
    mEnvironmentTexture = 0;
    mEnvironmentTextureName[0] = 0;
  }

  ~MyPd3d(void)
  {
  	onDeviceReset(mDevice);
  }


  void   setDevice(void *d3device)
  {

  	if ( mDevice == 0 )
  	{
  		mDevice = (LPDIRECT3DDEVICE9) d3device;
  		mEffect =	loadFX(g_shader,sizeof(g_shader));

  		if ( mEffect )
  		{
			mLightMapTechnique              = mEffect->GetTechniqueByName("LightMapShader");
     	  mTechnique                        = mEffect->GetTechniqueByName("SoftBodyShader");
     	  mWireFrameTechnique               = mEffect->GetTechniqueByName("SoftBodyWireFrameShader");
     	  mSolidTechnique                   = mEffect->GetTechniqueByName("SoftBodySolidShader");
     	  mWireFrameWhiteTechnique          = mEffect->GetTechniqueByName("SoftBodyWireFrameWhite");
        mScreenTechnique                  = mEffect->GetTechniqueByName("SCREENQUAD");

     	  mFractalTechnique                 = mEffect->GetTechniqueByName("FractalShader");

     	  mWorldViewHandle                  = mEffect->GetParameterByName(0,"wv");
     	  mWorldViewProjectionHandle        = mEffect->GetParameterByName(0,"wvp");
        mWorldHandle                      = mEffect->GetParameterByName(0,"wm");
        mViewProjectionHandle             = mEffect->GetParameterByName(0,"vp");
        mProjectionHandle                 = mEffect->GetParameterByName(0,"proj");
        mEyePosHandle                     = mEffect->GetParameterByName(0,"EyePos");
     	  mDiffuseHandle                    = mEffect->GetParameterByName(0,"DiffuseMap");
     	  mLightMapHandle                    = mEffect->GetParameterByName(0,"LightMap");
     	  mDetailMapHandle                    = mEffect->GetParameterByName(0,"DetailMap");
        mEnvironmentHandle                = mEffect->GetParameterByName(0,"EnvironmentMap");
     	  mAmbientColorHandle               = mEffect->GetParameterByName(0,"AmbientColor");
     	  mDiffuseColorHandle               = mEffect->GetParameterByName(0,"DiffuseColor");
     	  mSpecularColorHandle              = mEffect->GetParameterByName(0,"SpecularColor");
     	  mSpecularPowerHandle              = mEffect->GetParameterByName(0,"SpecularPower");

     	  mClampLowHandle                   = mEffect->GetParameterByName(0,"ClampLow");
     	  mClampHighHandle                  = mEffect->GetParameterByName(0,"ClampHigh");
     	  mClampScaleHandle                 = mEffect->GetParameterByName(0,"ClampScale");
  		}
  	}
  }

  bool          copyTexture(Pd3dTexture *tsource,Pd3dTexture *tdest)
  {
    bool ret = false;

#if 0
    NxU32 swidth,sheight,spitch;
    NxU32 dwidth,dheight,dpitch;

    const char *source = (const char *)lockTexture(tsource,swidth,sheight,spitch);
    char *dest         = (char *) lockTexture(tdest,dwidth,dheight,dpitch);
    if ( source && dest && swidth == dwidth && sheight == dheight )
    {
      for (NxU32 y=0; y<sheight; y++)
      {
        memcpy(dest,source,swidth*4);
        source+=spitch;
        dest+=dpitch;
      }
    }

    if ( source )
      unlockTexture(tsource);
    if ( dest )
      unlockTexture(tdest);
#else
	HRESULT hr;

	LPDIRECT3DTEXTURE9 src = (LPDIRECT3DTEXTURE9)tsource->getHandle();
	LPDIRECT3DTEXTURE9 dst = (LPDIRECT3DTEXTURE9)tdest->getHandle();
	assert( src && dst );
	if ( src && dst )
	{
		LPDIRECT3DSURFACE9 srcSurf = 0;
		LPDIRECT3DSURFACE9 dstSurf = 0;

		hr = src->GetSurfaceLevel( 0, &srcSurf );
		assert( hr == D3D_OK );
		if ( hr != D3D_OK )
			return false;

		hr = dst->GetSurfaceLevel( 0, &dstSurf );
		assert( hr == D3D_OK );
		if ( hr != D3D_OK )
			return false;

		NxU32 swidth = tsource->getWidth();
		NxU32 sheight = tsource->getHeight();
		NxU32 dwidth = tdest->getWidth();
		NxU32 dheight = tdest->getHeight();

		DWORD filter = D3DX_FILTER_LINEAR;
		if ( swidth == dwidth && sheight == dheight )
			filter = D3DX_FILTER_NONE;

		HRESULT hr = D3DXLoadSurfaceFromSurface( dstSurf, NULL, NULL, srcSurf, NULL, NULL, filter, 0 );
		assert( hr == D3D_OK );
		ret = ( hr == D3D_OK );
	}
#endif

    return ret;
  }



  void   onDeviceReset(void * /* d3device */)
  {

		if ( mEffect )
		{
      mEffect->Release();
      mEffect = 0;
      mTechnique = 0;
	  mLightMapTechnique = 0;
      mWireFrameTechnique = 0;
      mFractalTechnique = 0;
      mSolidTechnique = 0;
      mWorldViewHandle = 0;
      mWorldViewProjectionHandle = 0;
      mDiffuseHandle = 0;
      mLightMapHandle = 0;
      mDetailMapHandle = 0;
      mEnvironmentHandle = 0;
      mWorldHandle = 0;
      mViewProjectionHandle = 0;
      mProjectionHandle = 0;
      mEyePosHandle = 0;
      mAmbientColorHandle = 0;
      mDiffuseColorHandle = 0;
      mSpecularColorHandle = 0;
      mSpecularPowerHandle = 0;
      mClampLowHandle = 0;
      mClampHighHandle = 0;
      mClampScaleHandle = 0;
		}

    Pd3dTextureMap::iterator i;
    for (i=mTextures.begin(); i!=mTextures.end(); ++i)
    {
      Pd3dTexture *tex = (*i).second;
      tex->release();
    }

    Pd3dTexture::releaseEmbedHandle();

    if ( mEnvironmentTexture )
    {
      mEnvironmentTexture->Release();
      mEnvironmentTexture = 0;
    }

    mDevice = 0;
  }

  void setWorldMatrix(const void *world)
  {
    if ( world )
  		mWorld       = *(D3DXMATRIXA16 *)world;
    else
      D3DXMatrixIdentity(&mWorld);

    applyTransforms();
  }

  void applyTransforms(void)
  {
  	D3DXMatrixMultiply(&mWorldView,&mWorld,&mView);
  	D3DXMatrixMultiply(&mWorldViewProjection,&mWorldView,&mProjection);
  	D3DXMatrixMultiply(&mViewProjection,&mView,&mProjection);

		D3DXVECTOR3 v(0,0,0);
		D3DXVECTOR4 eye;
		D3DXVec3Transform(&eye,&v,&mViewProjection);
		mEyePos.x = eye.x;
		mEyePos.y = eye.y;
		mEyePos.z = eye.z;

   	if ( mWorldViewHandle )
 			mEffect->SetMatrix( mWorldViewHandle, &mWorldView );

   	if ( mWorldViewProjectionHandle )
 			mEffect->SetMatrix(mWorldViewProjectionHandle, &mWorldViewProjection );

    if ( mWorldHandle )
      mEffect->SetMatrix(mWorldHandle, &mWorld );

    if ( mProjectionHandle )
      mEffect->SetMatrix(mProjectionHandle,&mProjection);

    if ( mViewProjectionHandle )
      mEffect->SetMatrix(mViewProjectionHandle,&mViewProjection);

    if ( mEyePosHandle )
		{
			D3DXVECTOR4 vp = mEyePos;
      mEffect->SetVector(mEyePosHandle,&vp);
    }
  }

  const void * getViewMatrix(void) const
  {
    return &mView;
  }

  const void * getProjectionMatrix(void) const
  {
    return &mProjection;
  }

	void   setViewProjectionMatrix(const void *view,const void *projection)
	{
  	mView        = *(D3DXMATRIXA16 *)view;
  	mProjection  = *(D3DXMATRIXA16 *)projection;
    applyTransforms();
	}


  void * createVertexBuffer(NxU32 vcount,const Pd3dGraphicsVertex *vertices)
  {
  	void * ret = 0;

  	if ( mDevice )
  	{
			LPDIRECT3DVERTEXBUFFER9 pvbVertexBuffer = 0;
			mDevice->CreateVertexBuffer(sizeof(Pd3dGraphicsVertex)*vcount, D3DUSAGE_WRITEONLY, D3DFVF_TETRAVERTEX, D3DPOOL_MANAGED, &pvbVertexBuffer, 0);
			if ( pvbVertexBuffer )
			{
				void* pvVertices=0;
				pvbVertexBuffer->Lock( 0, 0, &pvVertices, 0 );
				if ( pvVertices )
				{
					if ( vertices )
						memcpy( pvVertices, vertices, sizeof(Pd3dGraphicsVertex)*vcount );
					else
						memset( pvVertices, 0, sizeof(Pd3dGraphicsVertex)*vcount );

					pvbVertexBuffer->Unlock();
					ret = pvbVertexBuffer;
				}
			}
		}
  	return ret;
  }

  bool   releaseVertexBuffer(void *buffer)
  {
  	bool ret = false;
  	if ( buffer )
  	{
			LPDIRECT3DVERTEXBUFFER9 pvbVertexBuffer = (LPDIRECT3DVERTEXBUFFER9) buffer;
			pvbVertexBuffer->Release();
			ret = true;
  	}
  	return ret;
  }

  void * lockVertexBuffer(void *buffer)
  {
  	void * ret = 0;
  	if ( buffer )
  	{
			LPDIRECT3DVERTEXBUFFER9 pvbVertexBuffer = (LPDIRECT3DVERTEXBUFFER9) buffer;
			void* pvVertices=0;
			pvbVertexBuffer->Lock( 0, 0, &pvVertices, 0 );
			ret = pvVertices;
		}
  	return ret;
  }

  bool   unlockVertexBuffer(void *buffer)
  {
  	bool ret = false;
  	if ( buffer )
  	{
			LPDIRECT3DVERTEXBUFFER9 pvbVertexBuffer = (LPDIRECT3DVERTEXBUFFER9) buffer;
			pvbVertexBuffer->Unlock();
			ret = true;
  	}
  	return ret;
  }


  void * createIndexBuffer(NxU32 icount,const NxU32 *indices)
  {
  	void *ret = 0;

	  if ( mDevice )
	  {
  		LPDIRECT3DINDEXBUFFER9  idx=0;
  		NxI32 stride       = sizeof(NxU32);
  		D3DFORMAT	format = D3DFMT_INDEX32;
			mDevice->CreateIndexBuffer( icount*stride, D3DUSAGE_WRITEONLY, format, D3DPOOL_MANAGED, &idx, 0 );
			if ( idx )
			{
				void * pIndices=0;
				idx->Lock(0,0,&pIndices,0);
				if ( pIndices )
				{
					if ( indices )
						memcpy(pIndices, indices, icount*stride );
					else
						memset(pIndices, 0, icount*stride );
					idx->Unlock();
					ret = idx;
				}
			}
		}
  	return ret;
  }

  void * createIndexBuffer(NxU32 icount,const NxU16 *indices)
  {
    void *ret = 0;

    if ( mDevice )
    {
      LPDIRECT3DINDEXBUFFER9  idx=0;
      NxI32 stride       = sizeof(NxU16);
      D3DFORMAT	format = D3DFMT_INDEX16;
      mDevice->CreateIndexBuffer( icount*stride, D3DUSAGE_WRITEONLY, format, D3DPOOL_MANAGED, &idx, 0 );
      if ( idx )
      {
        void * pIndices=0;
        idx->Lock(0,0,&pIndices,0);
        if ( pIndices )
        {
          if ( indices )
            memcpy(pIndices, indices, icount*stride );
          else
            memset(pIndices, 0, icount*stride );
          idx->Unlock();
          ret = idx;
        }
      }
    }
    return ret;
  }


  bool   releaseIndexBuffer(void *ibuffer)
  {
  	bool ret = false;
  	if ( ibuffer )
  	{
  	  LPDIRECT3DINDEXBUFFER9 ib = (LPDIRECT3DINDEXBUFFER9) ibuffer;
  	  ib->Release();
  	  ret = true;
  	}
  	return ret;
  }

  void * lockIndexBuffer(void *ibuffer)
  {
  	void * ret = 0;
  	if ( ibuffer )
  	{
  	  LPDIRECT3DINDEXBUFFER9 ib = (LPDIRECT3DINDEXBUFFER9) ibuffer;
			void * pIndices=0;
			ib->Lock(0,0,&pIndices,0);
			if ( pIndices )
			{
				ret = pIndices;
			}
  	}
  	return ret;
  }

  bool   unlockIndexBuffer(void *ibuffer)
  {
  	bool ret = false;
  	if ( ret )
  	{
  	  LPDIRECT3DINDEXBUFFER9 ib = (LPDIRECT3DINDEXBUFFER9) ibuffer;
  	  ib->Unlock();
  	  ret = true;
  	}
  	return ret;
  }

	D3DXHANDLE getTechnique(Pd3dMaterial *material,bool &isLightMap)
	{
		isLightMap = false;
		D3DXHANDLE ret = mTechnique;
		if ( mWireFrame ) 
			ret = mWireFrameWhiteTechnique;
		else
		{
			char *plus = strchr(material->mTexture,'+');
			if ( plus )
			{
				isLightMap = true;
				ret = mLightMapTechnique;
			}
		}
		return ret;
	}

	bool shaderSetup(Pd3dMaterial *material)
	{
		bool ret = false;

		mDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
		mDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);		
		{
			bool isLightMap;
			D3DXHANDLE tech = getTechnique(material,isLightMap);
			if ( tech )
			{
				if ( material )
				{
					if ( isLightMap )
					{
						if ( material->mHandle == 0 )
						{
							char scratch[512];
							strcpy(scratch,material->mTexture);
							char *plus = strchr(scratch,'+');
							*plus = 0;
							material->mHandle = locateTexture(scratch);
						}
						if ( material->mLightMapHandle == 0 )
						{
							char scratch[512];
							strcpy(scratch,material->mTexture);
							char *plus = strchr(scratch,'+');
							plus++;
							char *detail = strchr(plus,'+');
							if ( detail )
							{
								*detail = 0;
								detail++;
							}
							material->mLightMapHandle = locateTexture(plus);
							if ( material->mDetailMapHandle == 0 && detail )
							{
								material->mDetailMapHandle = locateTexture(detail);
							}

						}
						if ( material->mHandle )
						{
							LPDIRECT3DBASETEXTURE9 pptex = material->mHandle->getHandle();
							mEffect->SetTexture(mDiffuseHandle,pptex);
						}
						if ( material->mLightMapHandle )
						{
							LPDIRECT3DBASETEXTURE9 pptex = material->mLightMapHandle->getHandle();
							mEffect->SetTexture(mLightMapHandle,pptex);
						}
						if ( material->mDetailMapHandle )
						{
							LPDIRECT3DBASETEXTURE9 pptex = material->mDetailMapHandle->getHandle();
							mEffect->SetTexture(mDetailMapHandle,pptex);
						}

					}
					else
					{
						if ( material->mHandle == 0 )
						{
							material->mHandle = locateTexture(material->mTexture);
						}

						if ( material->mHandle )
						{
							LPDIRECT3DBASETEXTURE9 pptex = material->mHandle->getHandle();
							mEffect->SetTexture(mDiffuseHandle,pptex);
						}

						if ( mAmbientColorHandle )
						{
							mEffect->SetFloatArray(mAmbientColorHandle,material->mAmbientColor,4);
						}

						if ( mDiffuseColorHandle )
						{
							mEffect->SetFloatArray(mDiffuseColorHandle,material->mDiffuseColor,4);
						}

						if ( mSpecularColorHandle )
						{
							mEffect->SetFloatArray(mSpecularColorHandle,material->mSpecularColor,4);
						}

						if ( mSpecularPowerHandle )
						{
							mEffect->SetFloat(mSpecularPowerHandle, material->mSpecularPower );
						}
					}
				}
				mEffect->SetTechnique(tech);
			}
			mDevice->SetFVF(D3DFVF_TETRAVERTEX);

			ret = true;
		}

		return ret;
	}

  bool shaderSetup(Pd3dTexture *texture)
  {
    bool ret = false;

    D3DXHANDLE tech = mScreenTechnique;
    if ( tech )
    {
      LPDIRECT3DBASETEXTURE9 pptex = texture->getHandle();
			mEffect->SetTexture(mDiffuseHandle,pptex);
      mEffect->SetTechnique(tech);
      mDevice->SetFVF(D3DFVF_SCREENVERTEX);
      ret = true;
    }

    return ret;
  }

  bool   renderSection(Pd3dMaterial *material,void *vbuffer,void *ibuffer,NxU32 vcount,NxU32 tcount)
  {
    bool ret = false;

    if ( shaderSetup(material) )
    {
      LPDIRECT3DINDEXBUFFER9  pib = (LPDIRECT3DINDEXBUFFER9) ibuffer;
      LPDIRECT3DVERTEXBUFFER9 pvb = (LPDIRECT3DVERTEXBUFFER9) vbuffer;

      mDevice->SetStreamSource(0,pvb,0,sizeof(Pd3dGraphicsVertex));
      mDevice->SetIndices(pib);

      UINT cPass = 0;

      {
        HRESULT ok= mEffect->Begin(&cPass,0);
        if ( SUCCEEDED(ok) )
        {
          for (UINT i=0; i<cPass; i++)
          {
            mEffect->BeginPass(i);

            mDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,vcount,0,tcount);

            mEffect->EndPass();
          }

          mEffect->End();
          ret = true;
        }
      }
    }

  	return ret;
  }

  bool   renderSection(Pd3dMaterial *material,void *vbuffer,NxU32 vcount)
  {
    bool ret = false;

    if ( shaderSetup(material) )
    {
      LPDIRECT3DVERTEXBUFFER9 pvb = (LPDIRECT3DVERTEXBUFFER9) vbuffer;
      mDevice->SetStreamSource(0,pvb,0,sizeof(Pd3dGraphicsVertex));
      UINT cPass = 0;
      HRESULT ok= mEffect->Begin(&cPass,0);
      if ( SUCCEEDED(ok) )
      {
        for (UINT i=0; i<cPass; i++)
        {
          mEffect->BeginPass(i);
          mDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,vcount/3);
          mEffect->EndPass();
        }

        mEffect->End();
        ret = true;
      }
    }

  	return ret;
  }

  ID3DXEffect * loadFX(const void *mem,NxU32 len)
  {
    ID3DXEffect *ret = 0;

   	ID3DXEffect        *effect;
   	LPD3DXBUFFER Errors = NULL;

		HRESULT hresult;
		hresult = D3DXCreateEffect( mDevice, mem, len, NULL, NULL, 0, NULL, &effect, &Errors );
   	if(FAILED(hresult))
   	{
   		if(Errors != NULL && Errors->GetBufferPointer() != NULL)
   		{
   			char *er = (char*)Errors->GetBufferPointer();
   			MyMessageBox("  Effect File Error : %s",er);
   		}
   		else
   		{
   			MyMessageBox("  Effect File Error: No details!");
   		}
   	}
   	else
   	{
   		ret = effect;
   	}
   	return ret;
  }

  bool   renderSection(Pd3dMaterial *material,Pd3dGraphicsVertex *vbuffer,NxU32 *ibuffer,NxU32 vcount,NxU32 tcount)
  {
    bool ret = false;

    if ( shaderSetup(material) )
    {
      UINT cPass = 0;

      HRESULT ok= mEffect->Begin(&cPass,0);
      if ( SUCCEEDED(ok) )
      {
        for (UINT i=0; i<cPass; i++)
        {
          mEffect->BeginPass(i);

          mDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,vcount,tcount, ibuffer, D3DFMT_INDEX32, vbuffer, sizeof(Pd3dGraphicsVertex) );

          mEffect->EndPass();
        }

        mEffect->End();
        ret = true;
      }
    }

  	return ret;
  }

  bool   renderSection(Pd3dMaterial *material,Pd3dGraphicsVertex *vbuffer,NxU32 vcount)
  {
    bool ret = false;

    if ( shaderSetup(material) )
    {
      UINT cPass = 0;
      HRESULT ok= mEffect->Begin(&cPass,0);
      if ( SUCCEEDED(ok) )
      {
        for (UINT i=0; i<cPass; i++)
        {
          mEffect->BeginPass(i);
          mDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST,vcount/3,vbuffer,sizeof(Pd3dGraphicsVertex) );
          mEffect->EndPass();
        }
        mEffect->End();
        ret = true;
      }
    }
    return ret;
  }

  void setWireFrame(bool state)
  {
    mWireFrame = state;
  }

  bool getWireFrame(void) const
  {
    return mWireFrame;
  }

  void   renderSolid(NxU32 tcount,const Pd3dSolidVertex *vtx)
  {
    setWorldMatrix(0);
    if ( tcount && mSolidTechnique )
    {
      if ( mDiffuseColorHandle )
      {
        NxF32 dcolor[4] = { 1, 1, 1, 1 };
        mEffect->SetFloatArray(mDiffuseColorHandle,dcolor,4);
      }
      mDevice->SetFVF(D3DFVF_SOLIDVERTEX);
      mEffect->SetTechnique(mSolidTechnique);
      UINT cPass = 0;
      HRESULT ok= mEffect->Begin(&cPass,0);
      if ( SUCCEEDED(ok) )
      {
        for (UINT i=0; i<cPass; i++)
        {
          mEffect->BeginPass(i);
          mDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST,tcount,vtx,sizeof(Pd3dSolidVertex) );
          mEffect->EndPass();
        }
        mEffect->End();
      }
    }

  }

  void   renderLines(NxU32 lcount,const Pd3dLineVertex *vtx,bool /* zpass */)
  {
    setWorldMatrix(0);

    if ( lcount && mWireFrameTechnique )
    {
      if ( mDiffuseColorHandle )
      {
        NxF32 dcolor[4] = { 1, 1, 1, 1 };
        mEffect->SetFloatArray(mDiffuseColorHandle,dcolor,4);
      }
      mDevice->SetRenderState(D3DRS_DEPTHBIAS,16);
      mDevice->SetFVF(D3DFVF_LINEVERTEX);
      mEffect->SetTechnique(mWireFrameTechnique);
      UINT cPass = 0;
      HRESULT ok= mEffect->Begin(&cPass,0);
      if ( SUCCEEDED(ok) )
      {
        for (UINT i=0; i<cPass; i++)
        {
          mEffect->BeginPass(i);
          mDevice->DrawPrimitiveUP(D3DPT_LINELIST,lcount,vtx,sizeof(Pd3dLineVertex) );
          mEffect->EndPass();
        }
        mEffect->End();
      }
      mDevice->SetRenderState(D3DRS_DEPTHBIAS,0);
    }
  }

  bool GetWindowSize(NxI32 &wid,NxI32 &hit) // get size of current render windowl
  {
  	bool ok = false;
  	if ( mDevice )
  	{
      D3DVIEWPORT9 vp;
      mDevice->GetViewport(&vp);
      wid = vp.Width;
      hit = vp.Height;
  		ok = true;
  	}
  	return ok;
  }

  bool   screenToWorld(NxI32 sx,      // screen x position
                       NxI32 sy,      // screen y position
                       NxF32 *world, // world position of the eye
                       NxF32 *direction) // direction vector into the world
  {
    bool ret = false;

  	NxI32 wid,hit;

    if ( 	GetWindowSize(wid,hit) )
    {
      if ( sx >= 0 && sx <= wid && sy >= 0 && sy <= hit )
      {
      	D3DXVECTOR3 vPickRayDir;
      	D3DXVECTOR3 vPickRayOrig;

      	POINT ptCursor;
      	ptCursor.x = sx;
      	ptCursor.y = sy;

      	// Compute the vector of the pick ray in screen space

      	D3DXVECTOR3 v;

      	v.x =  ( ( ( 2.0f * ptCursor.x ) / wid  ) - 1 ) / mProjection._11;
      	v.y = -( ( ( 2.0f * ptCursor.y ) / hit ) - 1 )  / mProjection._22;
      	v.z =  1.0f;

      	// Get the inverse view matrix
      	D3DXMATRIXA16 m;
      	D3DXMatrixInverse( &m, NULL, &mView );

      	// Transform the screen space pick ray into 3D space
      	vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
      	vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
      	vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;

        D3DXVec3Normalize(&vPickRayDir,&vPickRayDir);

      	vPickRayOrig.x = m._41;
      	vPickRayOrig.y = m._42;
      	vPickRayOrig.z = m._43;

      	world[0] = vPickRayOrig.x;
      	world[1] = vPickRayOrig.y;
      	world[2] = vPickRayOrig.z;

      	direction[0] = vPickRayDir.x;
      	direction[1] = vPickRayDir.y;
      	direction[2] = vPickRayDir.z;

        ret = true;
      }
    }
    return ret;
  }

  void set(D3DXVECTOR3 &v,NxF32 x,NxF32 y,NxF32 z)
  {
    v.x = x;
    v.y = y;
    v.z = z;
  }



  void   getEyePos(NxF32 *eye)
  {
    eye[0] = mEyePos.x;
    eye[1] = mEyePos.y;
    eye[2] = mEyePos.z;
  }

  void   preserveRenderState(void)
  {
    assert( mStateBlock == 0 );
		if ( mDevice )
		  mDevice->CreateStateBlock( D3DSBT_ALL, &mStateBlock );
  }

  void   restoreRenderState(void)
  {
	  if ( mStateBlock )
		{
		  mStateBlock->Apply();
   		mStateBlock->Release();
      mStateBlock = 0;
		}
  }

  const NxF32 *getViewProjection(void) const // return the current view projection matrix.
  {
    const NxF32 *ret = (const NxF32 *)&mViewProjection;
    return ret;
  }


  void setResourceInterface(RESOURCE_INTERFACE::ResourceInterface *ri)
  {
    gResourceInterface = ri;
  }

  bool shutdown(void)
  {
    return doShutdown();
  }

  void setCullMode(CullMode mode)
  {
    switch ( mode )
    {
      case CM_NO_CULL:
        mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE );
        break;
      case CM_CW:
        mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW );
        break;
      case CM_CCW:
        mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW );
        break;
    }
  }

  const char ** getVideoDevices(NxI32 &count)
  {
    const char **ret = 0;
    count = 0;

    ret = (const char **)zVidcapGetDevices(&count);

    return ret;
  }

  bool           startVideoDevice(const char *deviceName,NxI32 maxWidth,NxI32 maxHeight)
  {
    bool ret = false;
    char scratch[512];
    strncpy(scratch,deviceName,512);
    if ( zVidcapStartDevice(scratch,maxWidth,maxHeight) ) ret = true;
    return ret;
  }

  bool          stopVideoDevice(const char *deviceName)
  {
    bool ret = true;

    zVidcapShutdownDevice((char *)deviceName);

    return ret;
  }

  bool          stopAllVideoDevices(void)
  {
    bool ret = true;

    zVidcapShutdownAll();

    return ret;
  }

  void *        lockVideoDevice(const char *deviceName,NxI32 &frameNumber)
  {
    void * ret = 0;

    frameNumber = 0;

    ret = zVidcapLockNewest((char *)deviceName,&frameNumber);

    return ret;
  }

  bool          unlockVideoDevice(const char *deviceName)
  {
    bool ret = true;

    zVidcapUnlock((char *)deviceName);

    return ret;
  }

  NxI32           getVideoDeviceAverageFrameTime(const char *deviceName)
  {
    NxI32 ret = 0;

    ret = zVidcapGetAvgFrameTimeInMils((char *)deviceName);

    return ret;
  }

  bool          getVideoDeviceDesc(const char *deviceName,NxI32 &width,NxI32 &height,NxI32 &depth)
  {
    bool ret = true;

    width = 0;
    height = 0;
    depth = 0;

    zVidcapGetBitmapDesc((char *)deviceName,width,height,depth);


    return ret;
  }

  bool          showVideoDeviceFilterProperties(const char *deviceName)
  {
    bool ret = false;

    if ( zVidcapShowFilterPropertyPageModalDialog((char *)deviceName) )
    {
      ret = true;
    }

    return ret;
  }

  bool          showVideoDevicePinProperties(const char *deviceName)
  {
    bool ret = false;

    if ( zVidcapShowPinPropertyPageModalDialog((char *)deviceName) )
    {
      ret = true;
    }

    return ret;
  }

	void renderScreenQuad(Pd3dTexture *texture,NxF32 x,NxF32 y,NxF32 z,NxF32 wid,NxF32 hit,NxU32 color)
  {
  	renderScreenQuad(texture,x,y,z,wid,hit,0,0,1,1,color);
  }

	void renderScreenQuad(Pd3dTexture *texture,NxF32 ix,NxF32 iy,NxF32 iz,NxF32 iwid,NxF32 ihit,NxF32 s0,NxF32 t0,NxF32 s1,NxF32 t1,NxU32 color)
  {
  	if ( texture )
  	{
  		texture->RenderScreenQuad( ix, iy, iz, iwid, ihit, s0, t0, s1, t1, color );
  	}

  }


  Pd3dTexture * locateTexture(const char *fname)
  {
    Pd3dTexture *ret = 0;

    StringRef ref = SGET(fname);
    Pd3dTextureMap::iterator found;
    found = mTextures.find(ref);
    if ( found == mTextures.end() )
    {
      ret = MEMALLOC_NEW(Pd3dTexture)(SGET(fname));
      mTextures[ref] = ret;
    }
    else
    {
      ret = (*found).second;
    }
    return ret;
  }

  void * getDevice(void)
  {
    return mDevice;
  }

  void flush(void)
  {
    AgScreenPipe::FlushCurrent();
  }

	bool   renderSection(Pd3dTexture *texture,Pd3dScreenVertex *vbuffer,NxU16 *ibuffer,NxU32 vcount,NxU32 tcount)
  {
    bool ret = false;

    if ( shaderSetup(texture) )
    {
      UINT cPass = 0;
      HRESULT ok= mEffect->Begin(&cPass,0);
      if ( SUCCEEDED(ok) )
      {
        for (UINT i=0; i<cPass; i++)
        {

          mEffect->BeginPass(i);

    			mDevice->DrawIndexedPrimitiveUP( D3DPT_TRIANGLELIST, 0, vcount, tcount, ibuffer, D3DFMT_INDEX16, vbuffer, sizeof(Pd3dScreenVertex) );

          mEffect->EndPass();

        }
        mEffect->End();
        ret = true;
      }

    }

    return ret;
  }

	void renderScreenQuad(Pd3dTexture *texture,NxI32 x,NxI32 y,NxF32 z,NxI32 wid,NxI32 hit,NxU32 color)
  {
    renderScreenQuad(texture,(NxF32)x-0.5f,(NxF32)y-0.5f,z,(NxF32)wid,(NxF32)hit,color);
  }

  Pd3dTexture * createTexture(const char *fname,NxU32 width,NxU32 height,NxU32 depth,bool systemRam)
  {
    Pd3dTexture *ret = 0;

    StringRef ref = SGET(fname);
    Pd3dTextureMap::iterator found;
    found = mTextures.find(ref);
    if ( found == mTextures.end() )
    {
      ret = MEMALLOC_NEW(Pd3dTexture)(ref,width,height,depth,systemRam);
			mTextures[ref] = ret;
    }
    else
    {
      ret = (*found).second;
      ret->createTexture(width,height,depth,systemRam);
    }

    return ret;
  }

  void        * lockTexture(Pd3dTexture *texture,NxU32 &width,NxU32 &height,NxU32 &pitch)
  {
    void *ret = 0;
    width = 0;
    height = 0;

    if ( texture )
    {
      ret = texture->lock(width,height,pitch);
    }

    return ret;
  }

  void          unlockTexture(Pd3dTexture *texture)
  {
    if ( texture ) texture->unlock();
  }

  bool          releaseTexture(Pd3dTexture *texture)
  {
    bool ret = false;

    if ( texture )
    {
      const StringRef &ref = texture->getName();
      Pd3dTextureMap::iterator found;
      found = mTextures.find(ref);
      if ( found == mTextures.end() )
      {
        assert(0); // cannot ever happen!
      }
      else
      {
        mTextures.erase(found);
        delete texture;
        ret = true;
      }


    }

    return ret;
  }

  bool			saveTextureDDS(Pd3dTexture *texture, const char* fname)
  {
	  HRESULT hr = D3DXSaveTextureToFileA( fname, D3DXIFF_DDS, texture->getHandle(), NULL );
	  assert( hr == D3D_OK );
	  return ( hr == D3D_OK );
  }

  void          setClampConstants(NxF32 clampLow,NxF32 clampHigh,NxF32 clampScale)
  {
    if ( mClampLowHandle )   mEffect->SetFloat(mClampLowHandle,   clampLow );
    if ( mClampHighHandle )  mEffect->SetFloat(mClampHighHandle,  clampHigh );
    if ( mClampScaleHandle ) mEffect->SetFloat(mClampScaleHandle, clampScale );
  }

  void          setFractalMode(bool state)
  {
    mFractalMode = state;
  }

  bool          getFractalMode(void)
  {
    return mFractalMode;
  }

  void          setEnvironmentTexture(const char *t)
  {
    if ( mEnvironmentTexture )
    {
      mEnvironmentTexture->Release();
    }
    mbstowcs(mEnvironmentTextureName, t, 512 );
  	LPDIRECT3DCUBETEXTURE9 cubepptex=0;
		D3DXCreateCubeTextureFromFile(mDevice, mEnvironmentTextureName, &cubepptex );
 	  mEnvironmentTexture = (LPDIRECT3DTEXTURE9)cubepptex;
    if ( mEffect && mEnvironmentHandle )
    	mEffect->SetTexture(mEnvironmentHandle,mEnvironmentTexture);
  }

private:

	IDirect3DStateBlock9* mStateBlock;

	LPDIRECT3DTEXTURE9 mEnvironmentTexture;
  wchar_t             mEnvironmentTextureName[512];

	D3DXMATRIXA16      mWorld;
	D3DXMATRIXA16      mView;
	D3DXMATRIXA16      mProjection;
	D3DXMATRIXA16      mWorldView;
	D3DXMATRIXA16      mWorldViewProjection;
  D3DXMATRIXA16      mViewProjection;
  HWND               mHwnd;
  LPDIRECT3DDEVICE9  mDevice;
  D3DXVECTOR3        mEyePos;
  ID3DXEffect       *mEffect;
  D3DXVECTOR3        mRenderGameScale;
  D3DXHANDLE		mLightMapTechnique;
  D3DXHANDLE         mTechnique;
  D3DXHANDLE         mWireFrameTechnique;
  D3DXHANDLE         mSolidTechnique;
  D3DXHANDLE         mFractalTechnique;
  D3DXHANDLE         mScreenTechnique;
  D3DXHANDLE         mWireFrameWhiteTechnique;
  D3DXHANDLE         mWorldViewHandle;
  D3DXHANDLE         mWorldViewProjectionHandle;
  D3DXHANDLE         mDiffuseHandle;
  D3DXHANDLE         mLightMapHandle;
  D3DXHANDLE         mDetailMapHandle;
  D3DXHANDLE         mEnvironmentHandle;
  D3DXHANDLE         mWorldHandle;
  D3DXHANDLE         mViewProjectionHandle;
  D3DXHANDLE         mProjectionHandle;
  D3DXHANDLE         mEyePosHandle;
  D3DXHANDLE         mAmbientColorHandle;
  D3DXHANDLE         mDiffuseColorHandle;
  D3DXHANDLE         mSpecularColorHandle;
  D3DXHANDLE         mSpecularPowerHandle;
  D3DXHANDLE         mRenderGameScaleHandle;
//
  D3DXHANDLE         mClampLowHandle;
  D3DXHANDLE         mClampHighHandle;
  D3DXHANDLE         mClampScaleHandle;

  bool               mFractalMode;
  bool               mWireFrame;
	Pd3dTextureMap    mTextures;
};

static MyPd3d *gInterface=0;

#define MEGABYTE (1024*1024)

Pd3d * createPd3d(NxI32 version_number,NVSHARE::SystemServices *services)
{
	Pd3d *ret = 0;

	if ( services )
	{
		NVSHARE::gSystemServices = services;
	}

	assert( gInterface == 0 );
	if ( gInterface == 0 && version_number == PD3D_VERSION )
	{
		gInterface = MEMALLOC_NEW(MyPd3d);
		ret = static_cast<Pd3d *>(gInterface);
	}

	gPd3d = ret;

	return ret;
}



};

using namespace NVSHARE;


bool doShutdown(void)
{
  bool ret = false;
  if ( gInterface )
  {
    ret = true;
    delete gInterface;
    gInterface = 0;
  }
  return ret;
}

