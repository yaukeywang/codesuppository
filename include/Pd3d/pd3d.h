#ifndef PD3D_H

#define PD3D_H

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



#pragma warning(disable:4996)

#include <string.h>

namespace BINKVIDEO
{
class BinkVideo;
};

namespace RESOURCE_INTERFACE
{
class ResourceInterface;
};

class ExternalEffectSpec;
class ExternalEffectInterface;
class ExternalMaterialSpec;


namespace NVSHARE
{

enum CullMode
{
  CM_NO_CULL,  // no backface culling
  CM_CW,    // cull clockwise
  CM_CCW    // cull counterclockwise
};


class Pd3dSolidVertex
{
public:
  NxF32	       mPos[3];
  NxF32        mNormal[3];
  NxU32 mColor;
};


class Pd3dGraphicsVertex : public NVSHARE::Memalloc
{
public:
  NxF32	       mPos[3];
  NxF32        mNormal[3];
  NxF32        mTexel[2];
};

class Pd3dDeformVertex
{
public:
	NxF32	         mPos[3];
	NxF32          mNormal[3];
	NxF32          mTexel1[2];
	NxF32          mTexel2[2];
	NxF32          mWeight[4];
	NxU16 mBone[4];
};

#define DEFAULT_EFFECT "effect=light"
//#define DEFAULT_EFFECT "effect=wireframe"

class Pd3dTexture;

class Pd3dMaterial
{
public:

	Pd3dMaterial(void)
  {
    mName[0] = 0;
  	mTexture[0] = 0;
    strcpy(mEffect,DEFAULT_EFFECT);
  	mHandle = 0;
    setDiffuseColor(0xFFFFFFFF);
    setAmbientColor(0xFF606060);
    setSpecularColor(0xFFFFFFFF);
    setSpecularPower(32.0f);
    mExternalEffectSpec = 0;
    mExternalMaterialSpec = 0;
  }

	Pd3dMaterial(const char *name,const char *texture,NxU32 ambient,NxU32 diffuse,NxU32 specular,NxF32 specularPower)
  {
    mName[0] = 0;
  	mTexture[0] = 0;
    strcpy(mEffect,DEFAULT_EFFECT);

    if ( name )
      strncpy(mName,name,512);

    if ( texture )
      strncpy(mTexture,texture,512);


  	mHandle = 0;
    setDiffuseColor(diffuse);
    setAmbientColor(ambient);
    setSpecularColor(specular);
    setSpecularPower(specularPower);
    mExternalEffectSpec = 0;
    mExternalMaterialSpec = 0;
  }

	Pd3dMaterial(const char *name)
  {
    strcpy(mName,name);
  	mTexture[0] = 0;
    strcpy(mEffect,DEFAULT_EFFECT);
  	mHandle = 0;
    setDiffuseColor(0xFFFFFFFF);
    setAmbientColor(0xFF606060);
    setSpecularColor(0xFFFFFFFF);
    setSpecularPower(32.0f);
    mExternalEffectSpec = 0;
    mExternalMaterialSpec = 0;
  }

	Pd3dMaterial(const Pd3dMaterial &tm)
	{
	  strcpy(mName,tm.mName);
	  strcpy(mTexture,tm.mTexture);
    strcpy(mEffect,tm.mEffect);
	  mSpecularPower = tm.mSpecularPower;
	  for (NxI32 i=0; i<4; i++)
	  {
	  	mAmbientColor[i] = tm.mAmbientColor[i];
	  	mDiffuseColor[i] = tm.mDiffuseColor[i];
	  	mSpecularColor[i] = tm.mSpecularColor[i];
	  }
	  mHandle = 0;
    mExternalEffectSpec = 0;
    mExternalMaterialSpec = 0;
	}

	bool operator==(const Pd3dMaterial &tm) const
	{

		bool ret = false;

		if ( strcmp(mName,tm.mName) == 0 &&
			   strcmp(mTexture,tm.mTexture) == 0 &&
			   mSpecularPower == tm.mSpecularPower &&
			   mAmbientColor[0] == tm.mAmbientColor[0] &&
			   mAmbientColor[1] == tm.mAmbientColor[1] &&
			   mAmbientColor[2] == tm.mAmbientColor[2] &&
			   mAmbientColor[3] == tm.mAmbientColor[3] &&
			   mDiffuseColor[0] == tm.mDiffuseColor[0] &&
			   mDiffuseColor[1] == tm.mDiffuseColor[1] &&
			   mDiffuseColor[2] == tm.mDiffuseColor[2] &&
			   mDiffuseColor[3] == tm.mDiffuseColor[3] &&
			   mSpecularColor[0] == tm.mSpecularColor[0] &&
			   mSpecularColor[1] == tm.mSpecularColor[1] &&
			   mSpecularColor[2] == tm.mSpecularColor[2] &&
			   mSpecularColor[3] == tm.mSpecularColor[3] ) ret = true;

	  return ret;
	}

	void setColor(NxU32 color,NxF32 *c)
  {
    NxU32 a = (color>>24)&0xFF;
    NxU32 r = (color>>16)&0xFF;
    NxU32 g = (color>>8)&0xFF;
    NxU32 b = color&0xFF;

    c[0] = (NxF32)r/255.0f;
    c[1] = (NxF32)g/255.0f;
    c[2] = (NxF32)b/255.0f;
    c[3] = (NxF32)a/255.0f;
  }

	void setDiffuseColor(NxU32 color)
  {
    setColor(color,mDiffuseColor);
  }

	void setAmbientColor(NxU32 color)
  {
    setColor(color,mAmbientColor);
  }

	void setSpecularColor(NxU32 color)
  {
    setColor(color,mSpecularColor);
  }

	void setSpecularPower(NxF32 power)
  {
    mSpecularPower = power;
  }

	char                      mName[512];
	char                      mTexture[512];
	char                      mEffect[512];
	NxF32                     mSpecularPower;
	NxF32                     mAmbientColor[4];  // ARGB color
	NxF32                     mDiffuseColor[4];
	NxF32                     mSpecularColor[4];
	Pd3dTexture             *mHandle; // handle to the device version
	ExternalEffectSpec       *mExternalEffectSpec;
	ExternalMaterialSpec     *mExternalMaterialSpec;
};

class Pd3dLineVertex
{
public:
	NxF32 mPos[3];
	NxU32 mColor;
};

class Pd3dScreenVertex
{
public:
  NxF32 mPos[3];
  NxF32 mTexel[3];
  NxU32 mColor;
};


class Pd3d 
{
public:

	virtual void   preserveRenderState(void) = 0;
	virtual void   restoreRenderState(void) = 0;
  virtual void   setResourceInterface(RESOURCE_INTERFACE::ResourceInterface *ri) = 0;
	virtual void   setDevice(void *d3device)=0;
  virtual void * getDevice(void)=0;
	virtual void   onDeviceReset(void *d3device)=0;
	virtual void   setWireFrame(bool state) = 0;
  virtual bool   getWireFrame(void) const = 0;
	virtual void   setViewProjectionMatrix(const void *view,const void *projection) = 0;
	virtual void   setWorldMatrix(const void *world) = 0;
	virtual const void * getViewMatrix(void) const =  0;
	virtual const void * getProjectionMatrix(void) const = 0;
	virtual void * createVertexBuffer(NxU32 vcount,const Pd3dGraphicsVertex *vertices) = 0; // allocate a VB for this vertex data.
	virtual bool   releaseVertexBuffer(void *buffer)                    = 0;
	virtual void * lockVertexBuffer(void *buffer)                       = 0;
	virtual bool   unlockVertexBuffer(void *buffer)                     = 0;
	virtual void * createIndexBuffer(NxU32 icount,const NxU32 *indices)   = 0;
	virtual void * createIndexBuffer(NxU32 icount,const NxU16 *indices)   = 0;
	virtual bool   releaseIndexBuffer(void *ibuffer)                    = 0;
	virtual void * lockIndexBuffer(void *buffer)                        = 0;
	virtual bool   unlockIndexBuffer(void *buffer)                      = 0;
	virtual bool   renderSection(Pd3dMaterial *material,void *buffer,void *ibufferm,NxU32 vcount,NxU32 tcount)            = 0;
	virtual bool   renderSection(Pd3dMaterial *material,void *vbuffer,NxU32 vcount) = 0;
	virtual void   renderLines(NxU32 lcount,const Pd3dLineVertex *vtx,bool useZbuffer) = 0;
  virtual void   renderSolid(NxU32 tcount,const Pd3dSolidVertex *vtx) = 0;
	virtual bool   screenToWorld(NxI32 sx,      // screen x position
                               NxI32 sy,      // screen y position
                               NxF32 *world, // world position of the eye
                               NxF32 *direction) = 0; // direction vector into the world

	virtual bool   renderSection(Pd3dMaterial *material,Pd3dGraphicsVertex *vbuffer,NxU32 *ibuffer,NxU32 vcount,NxU32 tcount) = 0;
	virtual bool   renderSection(Pd3dMaterial *material,Pd3dGraphicsVertex *vbuffer,NxU32 vcount) = 0;

	virtual bool   renderSection(Pd3dTexture *texture,Pd3dScreenVertex *vbuffer,NxU16 *ibuffer,NxU32 vcount,NxU32 tcount) = 0;

	virtual void renderScreenQuad(Pd3dTexture *texture,NxF32 x,NxF32 y,NxF32 z,NxF32 wid,NxF32 hit,NxU32 color)=0;
	virtual void renderScreenQuad(Pd3dTexture *texture,NxI32 x,NxI32 y,NxF32 z,NxI32 wid,NxI32 hit,NxU32 color)=0;

	virtual void renderScreenQuad(Pd3dTexture *texture,NxF32 x,NxF32 y,NxF32 z,NxF32 wid,NxF32 hit,NxF32 s0,NxF32 t0,NxF32 s1,NxF32 t1,NxU32 color)=0;

  virtual Pd3dTexture * locateTexture(const char *fname) = 0;

  virtual Pd3dTexture * createTexture(const char *fname,NxU32 width,NxU32 height,NxU32 depth,bool systemRam=false) = 0;
  virtual bool          copyTexture(Pd3dTexture *source,Pd3dTexture *dest) = 0;
  virtual void        * lockTexture(Pd3dTexture *texture,NxU32 &width,NxU32 &height,NxU32 &pitch) = 0;
  virtual void          unlockTexture(Pd3dTexture *texture) = 0;
  virtual bool          releaseTexture(Pd3dTexture *texture) = 0;
  virtual bool			saveTextureDDS(Pd3dTexture *texture, const char* fname) = 0;

  virtual void  flush(void) = 0; // flush pending draw calls

	virtual void   getEyePos(NxF32 *eye)=0;
  virtual const NxF32 *getViewProjection(void) const = 0; // return the current view projection matrix.
  virtual void setCullMode(CullMode mode) = 0;

  virtual const char ** getVideoDevices(NxI32 &count) = 0;
  virtual bool          startVideoDevice(const char *deviceName,NxI32 maxWidth,NxI32 maxHeight) = 0;
  virtual bool          stopVideoDevice(const char *deviceName) = 0;
  virtual bool          stopAllVideoDevices(void) = 0;
  virtual void *        lockVideoDevice(const char *deviceName,NxI32 &frameNumber) = 0;
  virtual bool          unlockVideoDevice(const char *deviceName) = 0;
  virtual NxI32           getVideoDeviceAverageFrameTime(const char *deviceName) = 0;
  virtual bool          getVideoDeviceDesc(const char *deviceName,NxI32 &width,NxI32 &height,NxI32 &depth) = 0;
  virtual bool          showVideoDeviceFilterProperties(const char *deviceName) = 0;
  virtual bool          showVideoDevicePinProperties(const char *deviceName) = 0;

  virtual void          setClampConstants(NxF32 clampLow,NxF32 clampLogScale,NxF32 clampScale) = 0;
  virtual void          setFractalMode(bool state) = 0;
  virtual bool          getFractalMode(void) = 0;

  virtual void          setEnvironmentTexture(const char *t) = 0;

};

}; // end of namespace

#define PD3D_VERSION 7  // version 0.01

extern NVSHARE::Pd3d *gPd3d;

#endif
