#ifndef BASE_RENDER_DEBUG_H

#define BASE_RENDER_DEBUG_H

#include "../common/snippets/UserMemAlloc.h"
#include "../common/HeMath/HeFoundation.h"
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
  HeF32	       mPos[3];
  HeF32        mNormal[3];
  HeF32        mTexel[2];
};

class BaseRenderDebug
{
public:

    virtual void         setEmbedTexture(EmbedTexture t,LPHETEXTURE texture) = 0;

    virtual void         setScreenSize(HeU32 screenX,HeU32 screenY)                       = 0;
    virtual void         getScreenSize(HeU32 &screenX,HeU32 &screenY)                     = 0;
    virtual const HeF32 *getEyePos(void)                                                  = 0;
	  virtual void         setViewProjectionMatrix(const HeF32 *view,const HeF32 *projection) = 0;
    virtual const HeF32 *getViewProjectionMatrix(void) const                              = 0;
    virtual const HeF32 *getViewMatrix(void) const                                        = 0;
    virtual const HeF32 *getProjectionMatrix(void) const                                  = 0;
    virtual bool         screenToWorld(HeI32 sx,HeI32 sy,HeF32 *world,HeF32 *direction)   = 0;


  	virtual void DebugLine(const HeF64 *_p1,const HeF64 *_p2,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true)
    {
      HeF32 p1[3];
      HeF32 p2[3];
      p1[0] = (HeF32)_p1[0];
      p1[1] = (HeF32)_p1[1];
      p1[2] = (HeF32)_p1[2];

      p2[0] = (HeF32)_p2[0];
      p2[1] = (HeF32)_p2[1];
      p2[2] = (HeF32)_p2[2];

      DebugLine(p1,p2,color,duration,useZ);

    }
    	virtual void DebugRay(const HeF64 *_p1,const HeF64 *_p2,HeF32 arrowSize=0.1f,HeU32 color=0xFFFFFFFF,HeU32 arrowColor=0x00FF0000,HeF32 duration=0.001f,bool useZ=true)
      {
        HeF32 p1[3];
        HeF32 p2[3];
        p1[0] = (HeF32)_p1[0];
        p1[1] = (HeF32)_p1[1];
        p1[2] = (HeF32)_p1[2];

        p2[0] = (HeF32)_p2[0];
        p2[1] = (HeF32)_p2[1];
        p2[2] = (HeF32)_p2[2];

        DebugRay(p1,p2,arrowSize,color,arrowColor,duration,useZ);
      }

  virtual void DebugPolygon(HeU32 pcount,const HeF32 *points,HeU32 color=0xFFFFFF,HeF32 duration=0.001f,bool useZ=true,bool solid=true,bool clockwise=true) = 0;

	virtual void DebugLine(const HeF32 *p1,const HeF32 *p2,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true) = 0;
	virtual void DebugOrientedLine(const HeF32 *p1,const HeF32 *p2,const HeF32 *transform,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true) = 0;

	virtual void DebugRay(const HeF32 *p1,const HeF32 *p2,HeF32 arrowSize=0.1f,HeU32 color=0xFFFFFFFF,HeU32 arrowColor=0x00FF0000,HeF32 duration=0.001f,bool useZ=true) = 0;

  virtual void DebugCylinder(const HeF32 *p1,const HeF32 *p2,HeF32 radius,HeU32 color=0xFFFFFFFF,HeF32 duration=0.0001f,bool useZ=true,bool solid=false) = 0;

	virtual void DebugThickRay(const HeF32 *p1,
	                           const HeF32 *p2,
                             HeF32 raySize=0.02f,
	                           HeF32 arrowSize=0.1f,
	                           HeU32 color=0xFFFFFFFF,
	                           HeU32 arrowColor=0x00FF0000,
	                           HeF32 duration=0.001f,
	                           bool wireFrameArrow=true) = 0;

  virtual void DebugPlane(const HeF32 *plane,HeF32 radius1,HeF32 radius2,HeU32 color=0xFFFFFF,HeF32 duration=0.001f,bool useZ=true) = 0;

	virtual void DebugTri(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true) =0;

	virtual void DebugTri(const HeF64 *_p1,const HeF64 *_p2,const HeF64 *_p3,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true)
  {
    float p1[3];
    float p2[3];
    float p3[3];
    p1[0] = (float)_p1[0]; p1[1] = (float)_p1[1]; p1[2] = (float)_p1[2];
    p2[0] = (float)_p2[0]; p2[1] = (float)_p2[1]; p2[2] = (float)_p2[2];
    p3[0] = (float)_p3[0]; p3[1] = (float)_p3[1]; p3[2] = (float)_p3[2];
    DebugTri(p1,p2,p3,color,duration,useZ);
  }

	virtual void DebugSolidTri(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f) =0;

  void DebugSolidTri(const HeF64 *_p1,const HeF64 *_p2,const HeF64 *_p3,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f) 
  {
    float p1[3];
    float p2[3];
    float p3[3];
    p1[0] = (float)_p1[0]; p1[1] = (float)_p1[1]; p1[2] = (float)_p1[2];
    p2[0] = (float)_p2[0]; p2[1] = (float)_p2[1]; p2[2] = (float)_p2[2];
    p3[0] = (float)_p3[0]; p3[1] = (float)_p3[1]; p3[2] = (float)_p3[2];
    DebugSolidTri(p1,p2,p3,color,duration);
  }

	virtual void DebugSolidTri(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,const HeF32 *n1,const HeF32 *n2,const HeF32 *n3,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f) =0;

	virtual void DebugBound(const HeF32 *bmin,const HeF32 *bmax,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true,bool solid=false) = 0;

  virtual void DebugBound(const HeF64 *bmin,const HeF64 *bmax,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true,bool solid=false)
  {
    HeF32 b1[3] = { (HeF32)bmin[0], (HeF32)bmin[1],(HeF32)bmin[2] };
    HeF32 b2[3] = { (HeF32)bmax[0], (HeF32)bmax[1],(HeF32)bmax[2] };
    DebugBound(b1,b2,color,duration,useZ,solid);
  }

	virtual void DebugOrientedBound(const HeF32 *sides,const HeF32 *transform,HeU32 color=0xFFFFFFFF,HeF32 duration=0.0001f,bool useZ=true,bool solid=false) = 0;
	virtual void DebugOrientedBound(const HeF32 *bmin,const HeF32 *bmax,const HeF32 *pos,const HeF32 *quat,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true,bool solid=false) = 0; // the rotation as a quaternion
	virtual void DebugOrientedBound(const HeF32 *bmin,const HeF32 *bmax,const HeF32 *xform,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true,bool solid=false) = 0; // the rotation as a quaternion

	virtual void DebugSphere(const HeF32 *pos,HeF32 radius,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true,bool solid=false) = 0;

  virtual void DebugSphere(const HeF64 *_pos,HeF64 radius,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true,bool solid=false)
  {
    HeF32 pos[3];
    pos[0] = (HeF32)_pos[0];
    pos[1] = (HeF32)_pos[1];
    pos[2] = (HeF32)_pos[2];
    DebugSphere(pos,(float)radius,color,duration,useZ,solid);
  }

	virtual void DebugOrientedSphere(HeF32 radius,const HeF32 *transform,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true,bool solid=false) = 0;

	virtual void DebugCapsule(const HeF32 *center,HeF32 radius,HeF32 height,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true) = 0;
	virtual void DebugOrientedCapsule(HeF32 radius,HeF32 height,const HeF32 *transform=0,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true) = 0;

	virtual void DebugPoint(const HeF32 *pos,HeF32 radius,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true) = 0;

	virtual void DebugAxes(const HeF32 *transform,HeF32 distance,HeF32 brightness,bool useZ) = 0;

	virtual void Reset( void ) = 0;
  virtual bool getWireFrame(void) = 0;
  virtual HeF32 getRenderScale(void) = 0;
  virtual void  setRenderScale(HeF32 scale) = 0;

  virtual void batchTriangles(EmbedTexture texture,const GraphicsVertex *vertices,HeU32 vcount,bool wireframe,bool overlay) = 0;

  virtual HeU32 getDebugColor(void)
  {
    HeU32 colors[8] =
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

    static HeU32 cindex = 0;

    HeU32 color = colors[cindex];
    cindex++;
    if ( cindex == 8 ) cindex = 0;

    return color;
  }

  void getSpherePoint(HeF32 *point,HeU32 x,HeU32 y,const HeF32 *center,HeF32 radius,HeF32 scale,HeU32 stepCount)
  {
    if ( x == stepCount ) x = 0;
    if ( y == stepCount ) y = 0;
    HeF32 a = (HeF32)x*scale;
    HeF32 b = (HeF32)y*scale;
    HeF32 tpos[3];

    tpos[0] = sinf(a)*cosf(b);
    tpos[1] = sinf(a)*sinf(b);
    tpos[2] = cosf(a);

    point[0] = center[0]+tpos[0]*radius;
    point[1] = center[1]+tpos[1]*radius;
    point[2] = center[2]+tpos[2]*radius;

  }

	void DebugDetailedSphere(const HeF32 *pos,HeF32 radius,HeU32 stepCount,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true,bool solid=false)
  {
    const float pi = 3.1415926535897932384626433832795028841971693993751f;
    const float pi2 = pi*2.0f;

    HeF32 scale = pi2 / stepCount;

    for (HeU32 y=0; y<stepCount; y++)
    {
      for (HeU32 x=0; x<stepCount; x++)
      {
        HeF32 p1[3];
        HeF32 p2[3];
        HeF32 p3[3];
        HeF32 p4[3];

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
