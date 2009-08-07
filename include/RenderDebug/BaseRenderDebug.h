#ifndef BASE_RENDER_DEBUG_H

#define BASE_RENDER_DEBUG_H

#include "UserMemAlloc.h"
#include "NxFoundation.h"
#include <math.h>
#include <float.h>

namespace HETEXTUREMANAGER
{
  class HeTexture;
};

typedef HETEXTUREMANAGER::HeTexture * LPHETEXTURE;



namespace RENDER_DEBUG
{

enum EmbedTexture
{
  ET_WOOD,
  ET_WATER,
  ET_TERRAIN
};

class GraphicsVertex
{
public:
  NxF32	       mPos[3];
  NxF32        mNormal[3];
  NxF32        mTexel[2];
};

class BaseRenderDebug
{
public:

    virtual void         setEmbedTexture(EmbedTexture t,LPHETEXTURE texture) = 0;

    virtual void         setScreenSize(NxU32 screenX,NxU32 screenY)                       = 0;
    virtual void         getScreenSize(NxU32 &screenX,NxU32 &screenY)                     = 0;
    virtual const NxF32 *getEyePos(void)                                                  = 0;
	  virtual void         setViewProjectionMatrix(const NxF32 *view,const NxF32 *projection) = 0;
    virtual const NxF32 *getViewProjectionMatrix(void) const                              = 0;
    virtual const NxF32 *getViewMatrix(void) const                                        = 0;
    virtual const NxF32 *getProjectionMatrix(void) const                                  = 0;
    virtual bool         screenToWorld(NxI32 sx,NxI32 sy,NxF32 *world,NxF32 *direction)   = 0;


  	virtual void DebugLine(const NxF64 *_p1,const NxF64 *_p2,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true)
    {
      NxF32 p1[3];
      NxF32 p2[3];
      p1[0] = (NxF32)_p1[0];
      p1[1] = (NxF32)_p1[1];
      p1[2] = (NxF32)_p1[2];

      p2[0] = (NxF32)_p2[0];
      p2[1] = (NxF32)_p2[1];
      p2[2] = (NxF32)_p2[2];

      DebugLine(p1,p2,color,duration,useZ);

    }
    	virtual void DebugRay(const NxF64 *_p1,const NxF64 *_p2,NxF32 arrowSize=0.1f,NxU32 color=0xFFFFFFFF,NxU32 arrowColor=0x00FF0000,NxF32 duration=0.001f,bool useZ=true)
      {
        NxF32 p1[3];
        NxF32 p2[3];
        p1[0] = (NxF32)_p1[0];
        p1[1] = (NxF32)_p1[1];
        p1[2] = (NxF32)_p1[2];

        p2[0] = (NxF32)_p2[0];
        p2[1] = (NxF32)_p2[1];
        p2[2] = (NxF32)_p2[2];

        DebugRay(p1,p2,arrowSize,color,arrowColor,duration,useZ);
      }

  virtual void DebugPolygon(NxU32 pcount,const NxF32 *points,NxU32 color=0xFFFFFF,NxF32 duration=0.001f,bool useZ=true,bool solid=true,bool clockwise=true) = 0;

	virtual void DebugLine(const NxF32 *p1,const NxF32 *p2,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true) = 0;
	virtual void DebugOrientedLine(const NxF32 *p1,const NxF32 *p2,const NxF32 *transform,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true) = 0;

	virtual void DebugRay(const NxF32 *p1,const NxF32 *p2,NxF32 arrowSize=0.1f,NxU32 color=0xFFFFFFFF,NxU32 arrowColor=0x00FF0000,NxF32 duration=0.001f,bool useZ=true) = 0;

  virtual void DebugCylinder(const NxF32 *p1,const NxF32 *p2,NxF32 radius,NxU32 color=0xFFFFFFFF,NxF32 duration=0.0001f,bool useZ=true,bool solid=false) = 0;

	virtual void DebugThickRay(const NxF32 *p1,
	                           const NxF32 *p2,
                             NxF32 raySize=0.02f,
	                           NxF32 arrowSize=0.1f,
	                           NxU32 color=0xFFFFFFFF,
	                           NxU32 arrowColor=0x00FF0000,
	                           NxF32 duration=0.001f,
	                           bool wireFrameArrow=true) = 0;

  virtual void DebugPlane(const NxF32 *plane,NxF32 radius1,NxF32 radius2,NxU32 color=0xFFFFFF,NxF32 duration=0.001f,bool useZ=true) = 0;

	virtual void DebugTri(const NxF32 *p1,const NxF32 *p2,const NxF32 *p3,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true) =0;

	virtual void DebugTri(const NxF64 *_p1,const NxF64 *_p2,const NxF64 *_p3,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true)
  {
    float p1[3];
    float p2[3];
    float p3[3];
    p1[0] = (float)_p1[0]; p1[1] = (float)_p1[1]; p1[2] = (float)_p1[2];
    p2[0] = (float)_p2[0]; p2[1] = (float)_p2[1]; p2[2] = (float)_p2[2];
    p3[0] = (float)_p3[0]; p3[1] = (float)_p3[1]; p3[2] = (float)_p3[2];
    DebugTri(p1,p2,p3,color,duration,useZ);
  }

	virtual void DebugSolidTri(const NxF32 *p1,const NxF32 *p2,const NxF32 *p3,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f) =0;

  void DebugSolidTri(const NxF64 *_p1,const NxF64 *_p2,const NxF64 *_p3,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f) 
  {
    float p1[3];
    float p2[3];
    float p3[3];
    p1[0] = (float)_p1[0]; p1[1] = (float)_p1[1]; p1[2] = (float)_p1[2];
    p2[0] = (float)_p2[0]; p2[1] = (float)_p2[1]; p2[2] = (float)_p2[2];
    p3[0] = (float)_p3[0]; p3[1] = (float)_p3[1]; p3[2] = (float)_p3[2];
    DebugSolidTri(p1,p2,p3,color,duration);
  }

	virtual void DebugSolidTri(const NxF32 *p1,const NxF32 *p2,const NxF32 *p3,const NxF32 *n1,const NxF32 *n2,const NxF32 *n3,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f) =0;

	virtual void DebugBound(const NxF32 *bmin,const NxF32 *bmax,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true,bool solid=false) = 0;

  virtual void DebugBound(const NxF64 *bmin,const NxF64 *bmax,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true,bool solid=false)
  {
    NxF32 b1[3] = { (NxF32)bmin[0], (NxF32)bmin[1],(NxF32)bmin[2] };
    NxF32 b2[3] = { (NxF32)bmax[0], (NxF32)bmax[1],(NxF32)bmax[2] };
    DebugBound(b1,b2,color,duration,useZ,solid);
  }

	virtual void DebugOrientedBound(const NxF32 *sides,const NxF32 *transform,NxU32 color=0xFFFFFFFF,NxF32 duration=0.0001f,bool useZ=true,bool solid=false) = 0;
	virtual void DebugOrientedBound(const NxF32 *bmin,const NxF32 *bmax,const NxF32 *pos,const NxF32 *quat,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true,bool solid=false) = 0; // the rotation as a quaternion
	virtual void DebugOrientedBound(const NxF32 *bmin,const NxF32 *bmax,const NxF32 *xform,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true,bool solid=false) = 0; // the rotation as a quaternion

	virtual void DebugSphere(const NxF32 *pos,NxF32 radius,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true,bool solid=false) = 0;

  virtual void DebugSphere(const NxF64 *_pos,NxF64 radius,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true,bool solid=false)
  {
    NxF32 pos[3];
    pos[0] = (NxF32)_pos[0];
    pos[1] = (NxF32)_pos[1];
    pos[2] = (NxF32)_pos[2];
    DebugSphere(pos,(float)radius,color,duration,useZ,solid);
  }

	virtual void DebugOrientedSphere(NxF32 radius,const NxF32 *transform,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true,bool solid=false) = 0;

	virtual void DebugCapsule(const NxF32 *center,NxF32 radius,NxF32 height,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true) = 0;
	virtual void DebugOrientedCapsule(NxF32 radius,NxF32 height,const NxF32 *transform=0,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true) = 0;

	virtual void DebugPoint(const NxF32 *pos,NxF32 radius,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true) = 0;

	virtual void DebugAxes(const NxF32 *transform,NxF32 distance,NxF32 brightness,bool useZ) = 0;

	virtual void Reset( void ) = 0;
  virtual bool getWireFrame(void) = 0;
  virtual NxF32 getRenderScale(void) = 0;
  virtual void  setRenderScale(NxF32 scale) = 0;

  virtual void batchTriangles(EmbedTexture texture,const GraphicsVertex *vertices,NxU32 vcount,bool wireframe,bool overlay) = 0;

  virtual NxU32 getDebugColor(bool reset_index=false)
  {
    NxU32 colors[8] =
    {
      0xFF0000,
      0x00FF00,
      0x0000FF,
      0xFFFF00,
      0x00FFFF,
      0xFF00FF,
      0xFFFFFF,
      0x808080
    };

    static NxU32 cindex = 0;

    if ( reset_index )
      cindex = 0;

    NxU32 color = colors[cindex];
    cindex++;
    if ( cindex == 8 ) cindex = 0;

    return color;
  }

  void getSpherePoint(NxF32 *point,NxU32 x,NxU32 y,const NxF32 *center,NxF32 radius,NxF32 scale,NxU32 stepCount)
  {
    if ( x == stepCount ) x = 0;
    if ( y == stepCount ) y = 0;
    NxF32 a = (NxF32)x*scale;
    NxF32 b = (NxF32)y*scale;
    NxF32 tpos[3];

    tpos[0] = sinf(a)*cosf(b);
    tpos[1] = sinf(a)*sinf(b);
    tpos[2] = cosf(a);

    point[0] = center[0]+tpos[0]*radius;
    point[1] = center[1]+tpos[1]*radius;
    point[2] = center[2]+tpos[2]*radius;

  }

	void DebugDetailedSphere(const NxF32 *pos,NxF32 radius,NxU32 stepCount,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true,bool solid=false)
  {
    const float pi = 3.1415926535897932384626433832795028841971693993751f;
    const float pi2 = pi*2.0f;

    NxF32 scale = pi2 / stepCount;

    for (NxU32 y=0; y<stepCount; y++)
    {
      for (NxU32 x=0; x<stepCount; x++)
      {
        NxF32 p1[3];
        NxF32 p2[3];
        NxF32 p3[3];
        NxF32 p4[3];

        getSpherePoint(p1,x,y,pos,radius,scale,stepCount);
        getSpherePoint(p2,x+1,y,pos,radius,scale,stepCount);
        getSpherePoint(p3,x+1,y+1,pos,radius,scale,stepCount);
        getSpherePoint(p4,x,y+1,pos,radius,scale,stepCount);

        if ( solid )
        {
          DebugSolidTri(p1,p2,p3,color,duration);
          DebugSolidTri(p1,p3,p4,color,duration);
        }
        else
        {
          DebugTri(p1,p2,p3,color,duration,useZ);
          DebugTri(p1,p3,p4,color,duration,useZ);
        }
      }
    }
  }


	virtual void drawGrid(bool zup) = 0; // draw a grid.

};

}; // end of namespace

extern RENDER_DEBUG::BaseRenderDebug *gBaseRenderDebug;



#endif
