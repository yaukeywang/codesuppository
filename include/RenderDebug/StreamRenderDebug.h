#ifndef STREAM_RENDER_DEBUG

#define STREAM_RENDER_DEBUG

#include "BaseRenderDebug.h"

class Streamer;

namespace RENDER_DEBUG
{

class StreamRenderDebug : public BaseRenderDebug
{
public:
  StreamRenderDebug(void)
  {
    mStream = 0;
    mRenderScale = 1;
    mMaxStreamSize = 100000000;
  }

  ~StreamRenderDebug(void)
  {
  }

  void setMaxStreamSize(HeU32 maxStreamSize)
  {
    mMaxStreamSize = maxStreamSize;
  }

  void setStreamer(Streamer *s) // set the stream source.
  {
    mStream = s;
  }

  void process(BaseRenderDebug *output); // process the current stream and send the results to the output debug object.

  void         setEmbedTexture(EmbedTexture t,LPHETEXTURE texture);

	void  DebugLine(const HeF32 *p1,const HeF32 *p2,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true);
	void  DebugOrientedLine(const HeF32 *p1,const HeF32 *p2,const HeF32 *transform,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true);
	void  DebugRay(const HeF32 *p1,const HeF32 *p2,HeF32 arrowSize=0.1f,HeU32 color=0xFFFFFFFF,HeU32 arrowColor=0x00FF0000,HeF32 duration=0.001f,bool useZ=true);
  void  DebugPlane(const HeF32 *plane,HeF32 radius1,HeF32 radius2,HeU32 color=0xFFFFFF,HeF32 duration=0.001f,bool useZ=true);
	void  DebugTri(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true);
	void  DebugSolidTri(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f);
	void  DebugSolidTri(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,const HeF32 *n1,const HeF32 *n2,const HeF32 *n3,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f);
	void  DebugBound(const HeF32 *bmin,const HeF32 *bmax,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true,bool solid=false);
	void  DebugOrientedBound(const HeF32 *sides,const HeF32 *transform,HeU32 color=0xFFFFFFFF,HeF32        duration=0.0001f,bool useZ=true,bool solid=false);
	void  DebugOrientedBound(const HeF32 *bmin,const HeF32 *bmax,const HeF32 *pos,const HeF32 *quat,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true,bool solid=false); // the rotation as a quaternion
	void  DebugOrientedBound(const HeF32 *bmin,const HeF32 *bmax,const HeF32 *xform,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true,bool solid=false); // the rotation as a quaternion
	void  DebugSphere(const HeF32 *pos,HeF32 radius,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true,bool solid=false);
	void  DebugOrientedSphere(HeF32 radius,const HeF32 *transform,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true,bool solid=false);
	void  DebugCapsule(const HeF32 *center,HeF32 radius,HeF32 height,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true);
	void  DebugOrientedCapsule(HeF32 radius,HeF32 height,const HeF32 *transform=0,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true);
	void  DebugPoint(const HeF32 *pos,HeF32 radius,HeU32 color=0xFFFFFFFF,HeF32 duration=0.001f,bool useZ=true);
	void  DebugAxes(const HeF32 *transform,HeF32 distance,HeF32 brightness,bool useZ);
	void  Reset( void );
  bool  getWireFrame(void);
  HeF32 getRenderScale(void);
  void  setRenderScale(HeF32 scale);
  void batchTriangles(EmbedTexture texture,const GraphicsVertex *vertices,HeU32 vcount,bool wireframe,bool overlay);

  void DebugCylinder(const HeF32 *p1,const HeF32 *p2,HeF32 radius,HeU32 color=0xFFFFFFFF,HeF32 duration=0.0001f,bool useZ=true,bool solid=false);

  void DebugThickRay(const HeF32 *p1,
    const HeF32 *p2,
    HeF32 raySize,
    HeF32 arrowSize,
    HeU32 color,
    HeU32 arrowColor,
    HeF32 duration,
    bool wireFrameArrow);

  bool isValidStream(void) const; // returns true if we have a valid write stream and we have not exceeded the maximum stream size.

  void DebugPolygon(HeU32 pcount,const HeF32 *points,HeU32 color,HeF32 duration,bool useZ,bool solid,bool clockwise);

  void drawGrid(bool zup);
  void         setScreenSize(HeU32 screenX,HeU32 screenY);
  void         getScreenSize(HeU32 &screenX,HeU32 &screenY);
  const HeF32 *getEyePos(void);
	void         setViewProjectionMatrix(const HeF32 *view,const HeF32 *projection);
  const HeF32 *getViewProjectionMatrix(void) const;
  const HeF32 *getViewMatrix(void) const;
  const HeF32 *getProjectionMatrix(void) const;
  bool         screenToWorld(HeI32 sx,HeI32 sy,HeF32 *world,HeF32 *direction);

private:
  Streamer  *mStream;
  HeF32      mRenderScale;
  HeU32 mMaxStreamSize;

  HeU32      mScreenWidth;
  HeU32      mScreenHeight;
  HeMat44    mViewMatrix;
  HeMat44    mProjectionMatrix;
  HeMat44    mViewProjectionMatrix;
  HeVec3     mEyePos;
};

}; // end of namspace.

extern RENDER_DEBUG::StreamRenderDebug *gStreamRenderDebug;

#endif
