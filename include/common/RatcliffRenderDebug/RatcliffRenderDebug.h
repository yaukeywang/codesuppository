#ifndef __NX_RATCLIFF_RENDER_DEBUG_H__
#define __NX_RATCLIFF_RENDER_DEBUG_H__


struct RatcliffDebugRenderState
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
	};
};

class RatcliffRenderDebugInterface
{
public:
};

// Const float versions, supports floats and doubles.
// All vectors are assumed X,Y,Z
// All quaternions are assumed X,Y,Z,W
// All matrices are assumed 4x4 D3DX style format.
// Plane equations are assumed to be in the format of A,B,C,D
// These methods are provided as convenience functiosn for applications which have their own internal math classes.
// Where appropriate they can simply cast their existing vector, quaternion, and matrices to const float pointers.
// The double precision version of the routines are provided when debugging geometry processing code which often uses these data types.
class RatcliffRenderDebug
{
public:

	virtual void setViewMatrix(const const float *view) = 0;
	virtual void setProjectionMatrix(const float *projection) = 0;

	virtual void  setFrameTime(float dtime) = 0;

	virtual void debugGraph(unsigned int numPoints, float * points, float graphMax, float graphXPos, float graphYPos, float graphWidth, float graphHeight, unsigned int colorSwitchIndex = -1) = 0;

    virtual void debugOrientedText(const float *xform,const char *fmt,...) = 0;
    virtual void debugText(const float *pos,const char *fmt,...) = 0;
    virtual void debugText(float x,float y,float z,const char *fmt,...) = 0;

	virtual void  reset(int blockIndex=-1) = 0; // -1 reset *everything*, 0 = reset everything except stuff inside blocks, > 0 reset a specific block of data.

	virtual unsigned int getDebugColor(bool reset_index=false) = 0;

	virtual void  debugDetailedSphere(const NxVec3 &pos,float radius,unsigned int stepCount) = 0;

	virtual void  drawGrid(bool zup=false,unsigned int gridSize=40) = 0; // draw a grid.

    virtual void  pushRenderState(void) = 0;
    virtual void  popRenderState(void) = 0;

	virtual void  setCurrentColor(unsigned int color=0xFFFFFF,unsigned int arrowColor=0xFF0000) = 0;
	virtual void  setCurrentDisplayTime(float displayTime=0.0001f) = 0;
	virtual float getRenderScale(void) = 0;
	virtual void  setRenderScale(float scale) = 0;
	virtual void  setCurrentState(unsigned int states=0) = 0;
	virtual void  addToCurrentState(NxApexDebugRenderState::Enum state) = 0; // OR this state flag into the current state.
	virtual void  removeFromCurrentState(NxApexDebugRenderState::Enum state) = 0; // Remove this bit flat from the current state
	virtual void  setCurrentTextScale(float textScale) = 0;
	virtual void  setCurrentArrowSize(float arrowSize) = 0;

	virtual unsigned int getCurrentState(void) const = 0;

	virtual void  setRenderState(unsigned int states=0,  // combination of render state flags
	                             unsigned int color=0xFFFFFF, // base color
                                 float displayTime=0.0001f, // duration of display items.
	                             unsigned int arrowColor=0xFF0000, // secondary color, usually used for arrow head
                                 float arrowSize=0.1f,
								 float renderScale=1.0f,
								 float textScale=1.0f) = 0;      // seconary size, usually used for arrow head size.


	virtual unsigned int getRenderState(unsigned int &color,float &displayTime,unsigned int &arrowColor,float &arrowSize,float &renderScale,float &textScale) const = 0;

	virtual int beginDrawGroup(const float *pose) = 0;
	virtual void  endDrawGroup(void) = 0;
	virtual void  setDrawGroupVisible(int groupId,bool state) = 0;
	virtual void  setDrawGroupPose(int blockId,const float *pose) = 0;

	virtual void  addDebugRenderable(const NxDebugRenderable &renderable) = 0;

	virtual void debugRect2d(float x1,float y1,float x2,float y2) = 0;


	virtual void debugLine(const float *p1,const float *p2) = 0;
	virtual void debugLine(const NxF64 *p1,const NxF64 *p2) = 0;

	virtual void debugTri(const float *p1,const float *p2,const float *p3) = 0;
	virtual void debugTri(const NxF64 *p1,const NxF64 *p2,const NxF64 *p3) = 0;

	virtual void debugTri(const float *p1,const float *p2,const float *p3,const float *n1,const float *n2,const float *n3) = 0;
	virtual void debugTri(const NxF64 *p1,const NxF64 *p2,const NxF64 *p3,const NxF64 *n1,const NxF64 *n2,const NxF64 *n3) = 0;

	virtual void debugBound(const float *bmin,const float *bmax) = 0;
	virtual void debugBound(const NxF64 *bmin,const NxF64 *bmax) = 0;

	virtual void debugOrientedBound(const float *bmin,const float *bmax,const float *xform) = 0;
	virtual void debugOrientedBound(const NxF64 *bmin,const NxF64 *bmax,const NxF64 *xform) = 0;

	virtual void debugOrientedBound(const float *bmin,const float *bmax,const float *pos,const float *quaternion) = 0;
	virtual void debugOrientedBound(const NxF64 *bmin,const NxF64 *bmax,const NxF64 *pos,const NxF64 *quaternion) = 0;

	virtual void debugOrientedBound(const float *sides,const float *transform) = 0;
	virtual void debugOrientedBound(const NxF64 *sides,const NxF64 *transform) = 0;

	virtual void debugSphere(const float *pos,float radius) = 0;
	virtual void debugSphere(const NxF64 *pos,NxF64 radius) = 0;

	virtual void debugHalfSphere(const float *pos,float radius) = 0;
	virtual void debugHalfSphere(const NxF64 *pos,NxF64 radius) = 0;

	virtual void debugPoint(const float *pos,float radius) = 0;
	virtual void debugPoint(const NxF64 *pos,NxF64 radius) = 0;

	virtual void debugOrientedSphere(float radius,const float *transform) = 0;
	virtual void debugOrientedSphere(NxF64 radius,const NxF64 *transform) = 0;

	virtual void debugCylinder(const float *p1,const float *p2,float radius) = 0;
	virtual void debugCylinder(const NxF64 *p1,const NxF64 *p2,NxF64 radius) = 0;

	virtual void debugPolygon(unsigned int pcount,const float *points) = 0;
	virtual void debugPolygon(unsigned int pcount,const NxF64 *points) = 0;

	virtual void debugOrientedLine(const float *p1,const float *p2,const float *transform) = 0;
	virtual void debugOrientedLine(const NxF64 *p1,const NxF64 *p2,const NxF64 *transform) = 0;

	virtual void debugRay(const float *p1,const float *p2) = 0;
	virtual void debugRay(const NxF64 *p1,const NxF64 *p2) = 0;

	virtual void debugThickRay(const float *p1,const float *p2,float raySize) = 0;
	virtual void debugThickRay(const NxF64 *p1,const NxF64 *p2,NxF64 raySize) = 0;

	virtual void debugPlane(const float *plane,float radius1,float radius2) = 0;
	virtual void debugPlane(const NxF64 *plane,NxF64 radius1,NxF64 radius2) = 0;

	virtual void debugCapsule(const float *pos,float radius,float height) = 0; // assumes Y-up as the dominant axis, same as the PhysX SDK
	virtual void debugCapsule(const NxF64 *pos,NxF64 radius,NxF64 height) = 0; // assumes Y-up as the dominant axis, same as the PhysX SDK

	virtual void debugOrientedCapsule(float radius,float height,const float *transform) = 0;
	virtual void debugOrientedCapsule(NxF64 radius,NxF64 height,const NxF64 *transform) = 0;

	virtual void debugAxes(const float *xform,float distance,float brightness) = 0;
	virtual void debugAxes(const NxF64 *xform,NxF64 distance,NxF64 brightness) = 0;


protected:

	virtual ~RatcliffRenderDebug(void) { };

};


#endif // __NX_RATCLIFF_RENDER_DEBUG_H__
