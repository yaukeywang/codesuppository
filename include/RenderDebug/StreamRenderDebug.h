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

  void setMaxStreamSize(NxU32 maxStreamSize)
  {
    mMaxStreamSize = maxStreamSize;
  }

  void setStreamer(Streamer *s) // set the stream source.
  {
    mStream = s;
  }

  void process(BaseRenderDebug *output); // process the current stream and send the results to the output debug object.

  void         setEmbedTexture(EmbedTexture t,LPHETEXTURE texture);

	void  DebugLine(const NxF32 *p1,const NxF32 *p2,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true);
	void  DebugOrientedLine(const NxF32 *p1,const NxF32 *p2,const NxF32 *transform,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true);
	void  DebugRay(const NxF32 *p1,const NxF32 *p2,NxF32 arrowSize=0.1f,NxU32 color=0xFFFFFFFF,NxU32 arrowColor=0x00FF0000,NxF32 duration=0.001f,bool useZ=true);
  void  DebugPlane(const NxF32 *plane,NxF32 radius1,NxF32 radius2,NxU32 color=0xFFFFFF,NxF32 duration=0.001f,bool useZ=true);
	void  DebugTri(const NxF32 *p1,const NxF32 *p2,const NxF32 *p3,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true);
	void  DebugSolidTri(const NxF32 *p1,const NxF32 *p2,const NxF32 *p3,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f);
	void  DebugSolidTri(const NxF32 *p1,const NxF32 *p2,const NxF32 *p3,const NxF32 *n1,const NxF32 *n2,const NxF32 *n3,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f);
	void  DebugBound(const NxF32 *bmin,const NxF32 *bmax,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true,bool solid=false);
	void  DebugOrientedBound(const NxF32 *sides,const NxF32 *transform,NxU32 color=0xFFFFFFFF,NxF32        duration=0.0001f,bool useZ=true,bool solid=false);
	void  DebugOrientedBound(const NxF32 *bmin,const NxF32 *bmax,const NxF32 *pos,const NxF32 *quat,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true,bool solid=false); // the rotation as a quaternion
	void  DebugOrientedBound(const NxF32 *bmin,const NxF32 *bmax,const NxF32 *xform,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true,bool solid=false); // the rotation as a quaternion
	void  DebugSphere(const NxF32 *pos,NxF32 radius,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true,bool solid=false);
	void  DebugOrientedSphere(NxF32 radius,const NxF32 *transform,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true,bool solid=false);
	void  DebugCapsule(const NxF32 *center,NxF32 radius,NxF32 height,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true);
	void  DebugOrientedCapsule(NxF32 radius,NxF32 height,const NxF32 *transform=0,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true);
	void  DebugPoint(const NxF32 *pos,NxF32 radius,NxU32 color=0xFFFFFFFF,NxF32 duration=0.001f,bool useZ=true);
	void  DebugAxes(const NxF32 *transform,NxF32 distance,NxF32 brightness,bool useZ);
	void  Reset( void );
  bool  getWireFrame(void);
  NxF32 getRenderScale(void);
  void  setRenderScale(NxF32 scale);
  void batchTriangles(EmbedTexture texture,const GraphicsVertex *vertices,NxU32 vcount,bool wireframe,bool overlay);

  void DebugCylinder(const NxF32 *p1,const NxF32 *p2,NxF32 radius,NxU32 color=0xFFFFFFFF,NxF32 duration=0.0001f,bool useZ=true,bool solid=false);

  void DebugThickRay(const NxF32 *p1,
    const NxF32 *p2,
    NxF32 raySize,
    NxF32 arrowSize,
    NxU32 color,
    NxU32 arrowColor,
    NxF32 duration,
    bool wireFrameArrow);

  bool isValidStream(void) const; // returns true if we have a valid write stream and we have not exceeded the maximum stream size.

  void DebugPolygon(NxU32 pcount,const NxF32 *points,NxU32 color,NxF32 duration,bool useZ,bool solid,bool clockwise);

  void drawGrid(bool zup);
  void         setScreenSize(NxU32 screenX,NxU32 screenY);
  void         getScreenSize(NxU32 &screenX,NxU32 &screenY);
  const NxF32 *getEyePos(void);
	void         setViewProjectionMatrix(const NxF32 *view,const NxF32 *projection);
  const NxF32 *getViewProjectionMatrix(void) const;
  const NxF32 *getViewMatrix(void) const;
  const NxF32 *getProjectionMatrix(void) const;
  bool         screenToWorld(NxI32 sx,NxI32 sy,NxF32 *world,NxF32 *direction);

private:
  Streamer  *mStream;
  NxF32      mRenderScale;
  NxU32 mMaxStreamSize;

  NxU32      mScreenWidth;
  NxU32      mScreenHeight;
  NxMat44    mViewMatrix;
  NxMat44    mProjectionMatrix;
  NxMat44    mViewProjectionMatrix;
  NxVec3     mEyePos;
};

}; // end of namspace.

extern RENDER_DEBUG::StreamRenderDebug *gStreamRenderDebug;

#endif
