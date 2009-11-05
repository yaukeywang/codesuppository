#ifndef __RENDER_DEBUG_H__
#define __RENDER_DEBUG_H__


#include "UserMemAlloc.h"

class NxVec3;
class NxMat34;
class NxMat33;
class NxQuat;
class NxPlane;
class NxBounds3;

namespace NVSHARE
{

class RenderDebugSolidVertex
{
public:
	NxF32	mPos[3];
	NxF32 mNormal[3];
	NxU32 mColor;
};

class RenderDebugVertex
{
public:

	NxF32 mPos[3];
	NxU32 mColor;
};

class RenderDebugInterface
{
public:
  virtual void debugRenderLines(NxU32 lcount,const RenderDebugVertex *vertices,bool useZ,bool isScreenSpace) = 0;
  virtual void debugRenderTriangles(NxU32 tcount,const RenderDebugSolidVertex *vertices,bool useZ,bool isScreenSpace) = 0;
};

struct DebugRenderState
{
	enum Enum
	{
		ScreenSpace      = (1<<0),  // true if rendering in screenspace
		NoZbuffer        = (1<<1),  // true if zbuffering is disabled.
        SolidShaded      = (1<<2),  // true if rendering solid shaded.
        SolidWireShaded  = (1<<3),  // Render both as a solid shaded triangle and as a wireframe overlay.
        CounterClockwise = (1<<4),  // true if winding order is counter clockwise.
        CameraFacing     = (1<<5),  // True if text should be displayed camera facing
        InfiniteLifeSpan = (1<<6),  // True if the lifespan is infinite (overrides current display time value)
        CenterText       = (1<<7),  // True if the text should be centered.
		DoubleSided      = (1<<8),  // Render the trinagle NxF64 sided
		DoubleSidedWire  = (1<<9),  // Render the trinagle NxF64 sided with a wireframe outline.
	};
};


class RenderDebug
{
public:
	virtual NxU32 getUpdateCount(void) const = 0;

    virtual void render(NxF32 dtime,RenderDebugInterface *iface) = 0;

	virtual void debugGraph(NxU32 numPoints, NxF32 * points, NxF32 graphMax, NxF32 graphXPos, NxF32 graphYPos, NxF32 graphWidth, NxF32 graphHeight, NxU32 colorSwitchIndex = -1) = 0;

    virtual void debugText(NxF32 x,NxF32 y,NxF32 z,const char *fmt,...) = 0;

	virtual void  reset(NxI32 blockIndex=-1) = 0; // -1 reset *everything*, 0 = reset everything except stuff inside blocks, > 0 reset a specific block of data.

	virtual NxU32 getDebugColor(bool reset_index=false) = 0;



	virtual void  drawGrid(bool zup=false,NxU32 gridSize=40) = 0; // draw a grid.



    virtual void  pushRenderState(void) = 0;

    virtual void  popRenderState(void) = 0;

	virtual void  setCurrentColor(NxU32 color=0xFFFFFF,NxU32 arrowColor=0xFF0000) = 0;

	virtual void  setCurrentDisplayTime(NxF32 displayTime=0.0001f) = 0;

	virtual NxF32 getRenderScale(void) = 0;

	virtual void  setRenderScale(NxF32 scale) = 0;

	virtual void  setCurrentState(NxU32 states=0) = 0;

	virtual void  addToCurrentState(DebugRenderState::Enum state) = 0; // OR this state flag into the current state.

	virtual void  removeFromCurrentState(DebugRenderState::Enum state) = 0; // Remove this bit flat from the current state

	virtual void  setCurrentTextScale(NxF32 textScale) = 0;

	virtual void  setCurrentArrowSize(NxF32 arrowSize) = 0;

	virtual NxU32 getCurrentState(void) const = 0;

	virtual void  setRenderState(NxU32 states=0,  // combination of render state flags
	                             NxU32 color=0xFFFFFF, // base color
                                 NxF32 displayTime=0.0001f, // duration of display items.
	                             NxU32 arrowColor=0xFF0000, // secondary color, usually used for arrow head
                                 NxF32 arrowSize=0.1f,
								 NxF32 renderScale=1.0f,
								 NxF32 textScale=1.0f) = 0;      // seconary size, usually used for arrow head size.


	virtual NxU32 getRenderState(NxU32 &color,NxF32 &displayTime,NxU32 &arrowColor,NxF32 &arrowSize,NxF32 &renderScale,NxF32 &textScale) const = 0;


	virtual void  endDrawGroup(void) = 0;

	virtual void  setDrawGroupVisible(NxI32 groupId,bool state) = 0;

	virtual void debugRect2d(NxF32 x1,NxF32 y1,NxF32 x2,NxF32 y2) = 0;

// Const NxF32 versions, supports floats and doubles.
// All vectors are assumed X,Y,Z
// All quaternions are assumed X,Y,Z,W
// All matrices are assumed 4x4 D3DX style format.
// Plane equations are assumed to be in the format of A,B,C,D
// These methods are provided as convenience functiosn for applications which have their own internal math classes.
// Where appropriate they can simply cast their existing vector, quaternion, and matrices to const NxF32 pointers.
// The NxF64 precision version of the routines are provided when debugging geometry processing code which often uses these data types.

	virtual void DebugLine(const NxF32 *p1,const NxF32 *p2) = 0;
	virtual void DebugLine(const NxF64 *p1,const NxF64 *p2) = 0;

	virtual void DebugTri(const NxF32 *p1,const NxF32 *p2,const NxF32 *p3) = 0;
	virtual void DebugTri(const NxF64 *p1,const NxF64 *p2,const NxF64 *p3) = 0;

	virtual void DebugTri(const NxF32 *p1,const NxF32 *p2,const NxF32 *p3,const NxF32 *n1,const NxF32 *n2,const NxF32 *n3) = 0;
	virtual void DebugTri(const NxF64 *p1,const NxF64 *p2,const NxF64 *p3,const NxF64 *n1,const NxF64 *n2,const NxF64 *n3) = 0;

	virtual void DebugBound(const NxF32 *bmin,const NxF32 *bmax) = 0;
	virtual void DebugBound(const NxF64 *bmin,const NxF64 *bmax) = 0;

	virtual void DebugOrientedBound(const NxF32 *bmin,const NxF32 *bmax,const NxF32 *xform) = 0;
	virtual void DebugOrientedBound(const NxF64 *bmin,const NxF64 *bmax,const NxF64 *xform) = 0;

	virtual void DebugOrientedBound(const NxF32 *bmin,const NxF32 *bmax,const NxF32 *pos,const NxF32 *quaternion) = 0;
	virtual void DebugOrientedBound(const NxF64 *bmin,const NxF64 *bmax,const NxF64 *pos,const NxF64 *quaternion) = 0;

	virtual void DebugOrientedBound(const NxF32 *sides,const NxF32 *transform) = 0;
	virtual void DebugOrientedBound(const NxF64 *sides,const NxF64 *transform) = 0;

	virtual void DebugSphere(const NxF32 *pos,NxF32 radius) = 0;
	virtual void DebugSphere(const NxF64 *pos,NxF64 radius) = 0;

	virtual void DebugHalfSphere(const NxF32 *pos,NxF32 radius) = 0;
	virtual void DebugHalfSphere(const NxF64 *pos,NxF64 radius) = 0;

	virtual void DebugPoint(const NxF32 *pos,NxF32 radius) = 0;
	virtual void DebugPoint(const NxF64 *pos,NxF64 radius) = 0;

	virtual void DebugOrientedSphere(NxF32 radius,const NxF32 *transform) = 0;
	virtual void DebugOrientedSphere(NxF64 radius,const NxF64 *transform) = 0;

	virtual void DebugCylinder(const NxF32 *p1,const NxF32 *p2,NxF32 radius) = 0;
	virtual void DebugCylinder(const NxF64 *p1,const NxF64 *p2,NxF64 radius) = 0;

	virtual void DebugPolygon(NxU32 pcount,const NxF32 *points) = 0;
	virtual void DebugPolygon(NxU32 pcount,const NxF64 *points) = 0;

	virtual void DebugOrientedLine(const NxF32 *p1,const NxF32 *p2,const NxF32 *transform) = 0;
	virtual void DebugOrientedLine(const NxF64 *p1,const NxF64 *p2,const NxF64 *transform) = 0;

	virtual void DebugRay(const NxF32 *p1,const NxF32 *p2) = 0;
	virtual void DebugRay(const NxF64 *p1,const NxF64 *p2) = 0;

	virtual void DebugThickRay(const NxF32 *p1,const NxF32 *p2,NxF32 raySize) = 0;
	virtual void DebugThickRay(const NxF64 *p1,const NxF64 *p2,NxF64 raySize) = 0;

	virtual void DebugPlane(const NxF32 *plane,NxF32 radius1,NxF32 radius2) = 0;
	virtual void DebugPlane(const NxF64 *plane,NxF64 radius1,NxF64 radius2) = 0;

	virtual void DebugCapsule(const NxF32 *pos,NxF32 radius,NxF32 height) = 0; // assumes Y-up as the dominant axis, same as the PhysX SDK
	virtual void DebugCapsule(const NxF64 *pos,NxF64 radius,NxF64 height) = 0; // assumes Y-up as the dominant axis, same as the PhysX SDK

	virtual void DebugOrientedCapsule(NxF32 radius,NxF32 height,const NxF32 *transform) = 0;
	virtual void DebugOrientedCapsule(NxF64 radius,NxF64 height,const NxF64 *transform) = 0;

	virtual void DebugAxes(const NxF32 *xform,NxF32 distance,NxF32 brightness) = 0;
	virtual void DebugAxes(const NxF64 *xform,NxF64 distance,NxF64 brightness) = 0;

// These versions uses the Nx classes.  Not available in the snippet only form.

	virtual void  debugPolygon(NxU32 pcount,const NxVec3 *points) = 0;

	virtual void  debugLine(const NxVec3 &p1,const NxVec3 &p2) = 0;

	virtual void  debugOrientedLine(const NxVec3 &p1,const NxVec3 &p2,const NxMat34 &transform) = 0;

	virtual void  debugRay(const NxVec3 &p1,const NxVec3 &p2) = 0;

	virtual void  debugCylinder(const NxVec3 &p1,const NxVec3 &p2,NxF32 radius) = 0;

	virtual void  debugThickRay(const NxVec3 &p1,const NxVec3 &p2,NxF32 raySize=0.02f) = 0;

	virtual void  debugPlane(const NxPlane &plane,NxF32 radius1,NxF32 radius2) = 0;

	virtual void  debugTri(const NxVec3 &p1,const NxVec3 &p2,const NxVec3 &p3) =0;

	virtual void  debugTriNormals(const NxVec3 &p1,const NxVec3 &p2,const NxVec3 &p3,const NxVec3 &n1,const NxVec3 &n2,const NxVec3 &n3) =0;

	virtual void  debugBound(const NxVec3 &bmin,const NxVec3 &bmax) = 0;

	virtual void  debugBound(const NxBounds3 &b) = 0;

	virtual void  debugOrientedBound(const NxVec3 &sides,const NxMat34 &transform) = 0;

	virtual void  debugOrientedBound(const NxVec3 &bmin,const NxVec3 &bmax,const NxVec3 &pos,const NxQuat &quat) = 0; // the rotation as a quaternion

	virtual void  debugOrientedBound(const NxVec3 &bmin,const NxVec3 &bmax,const NxMat34 &xform) = 0; // the rotation as a quaternion

	virtual void  debugSphere(const NxVec3 &pos,NxF32 radius) = 0;

	virtual void  debugOrientedSphere(NxF32 radius,const NxMat34 &transform) = 0;

	virtual void  debugCapsule(const NxVec3 &center,NxF32 radius,NxF32 height) = 0;

	virtual void  debugOrientedCapsule(NxF32 radius,NxF32 height,const NxMat34 &transform) = 0;

	virtual void  debugPoint(const NxVec3 &pos,NxF32 radius) = 0;

	virtual void  debugAxes(const NxMat34 &transform,NxF32 distance=0.1f,NxF32 brightness=1.0f) = 0;

    virtual void debugArc(const NxVec3 &center,const NxVec3 &p1,const NxVec3 &p2,NxF32 arrowSize=0.1f,bool showRoot=false) = 0;

    virtual void debugThickArc(const NxVec3 &center,const NxVec3 &p1,const NxVec3 &p2,NxF32 thickness=0.02f,bool showRoot=false) = 0;

    virtual void debugText(const NxVec3 &pos,const char *fmt,...) = 0;

    virtual void debugOrientedText(const NxVec3 &pos,const NxQuat &rot,const char *fmt,...) = 0;

    virtual void debugOrientedText(const NxMat34 &xform,const char *fmt,...) = 0;

	virtual void setViewMatrix(const NxMat34 &view) = 0;
	virtual void setProjectionMatrix(const NxMat34 &projection) = 0;

	virtual void  eulerToMatrix(const NxVec3 &angles,NxMat33 &rot) = 0; // angles are in degrees
	virtual void  eulerToQuat(const NxVec3 &angles,NxQuat &q) = 0; // angles are in degrees.

	virtual NxI32 beginDrawGroup(const NxMat34 &pose) = 0;

	virtual void  setDrawGroupPose(NxI32 blockId,const NxMat34 &pose) = 0;

	virtual void  debugDetailedSphere(const NxVec3 &pos,NxF32 radius,NxU32 stepCount) = 0;

	virtual void  setPose(const NxMat34 &pose) = 0;

protected:

	virtual ~RenderDebug(void) { };

};

RenderDebug * createRenderDebug(void);
void          releaseRenderDebug(RenderDebug *rd);

extern RenderDebug *gRenderDebug;

}; // end of namespace

#endif // __RENDER_DEBUG_H__
