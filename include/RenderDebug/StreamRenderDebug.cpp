#include "StreamRenderDebug.h"
#include "../common/snippets/streamer.h"

#pragma warning(disable:4996)
#pragma warning(disable:4100)

namespace RENDER_DEBUG
{

enum StreamRenderDebugCommand
{
  SRDC_DEBUG_LINE               = 1000,
  SRDC_DEBUG_ORIENTED_LINE,
  SRDC_DEBUG_RAY,
  SRDC_DEBUG_PLANE,
  SRDC_DEBUG_TRI,
  SRDC_DEBUG_BOUND,
  SRDC_DEBUG_SOLID_TRI_1,
  SRDC_DEBUG_SOLID_TRI_2,
  SRDC_DEBUG_ORIENTED_BOUND_1,
  SRDC_DEBUG_ORIENTED_BOUND_2,
  SRDC_DEBUG_ORIENTED_BOUND_3,
  SRDC_DEBUG_SPHERE,
  SRDC_DEBUG_ORIENTED_SPHERE,
  SRDC_DEBUG_CAPSULE,
  SRDC_DEBUG_ORIENTED_CAPSULE,
  SRDC_DEBUG_POINT,
  SRDC_DEBUG_AXES,
  SRDC_RESET,
  SRDC_GET_WIREFRAME,
  SRDC_SET_RENDER_SCALE,
  SRDC_GET_RENDER_SCALE,
  SRDC_BATCH_TRIANGLES,
  SRDC_DEBUG_THICK_RAY,
  SRDC_DEBUG_CYLINDER,
  SRDC_DEBUG_POLYGON,
  SRDC_LAST
};


void  StreamRenderDebug::DebugLine(const HeF32 *p1,const HeF32 *p2,HeU32 color,HeF32 duration,bool useZ)
{
  if ( isValidStream() )
  {
    mStream->write( (HeU32)SRDC_DEBUG_LINE );
    mStream->write(p1[0]);
    mStream->write(p1[1]);
    mStream->write(p1[2]);

    mStream->write(p2[0]);
    mStream->write(p2[1]);
    mStream->write(p2[2]);

    mStream->write(color);
    mStream->write(duration);
    mStream->write(useZ);
  }
}

void  StreamRenderDebug::DebugOrientedLine(const HeF32 *p1,const HeF32 *p2,const HeF32 *transform,HeU32 color,HeF32 duration,bool useZ)
{
}

void StreamRenderDebug::DebugThickRay(const HeF32 *p1,
                                     const HeF32 *p2,
                                     HeF32 raySize,
	                                   HeF32 arrowSize,
        	                           HeU32 color,
	                                   HeU32 arrowColor,
	                                   HeF32 duration,
	                                   bool wireFrameArrow)
{
  if ( isValidStream() )
  {
    mStream->write( (HeU32)SRDC_DEBUG_THICK_RAY );
    mStream->write( p1[0] );
    mStream->write( p1[1] );
    mStream->write( p1[2] );

    mStream->write( p2[0] );
    mStream->write( p2[1] );
    mStream->write( p2[2] );

    mStream->write( raySize );
    mStream->write( arrowSize );
    mStream->write( color );
    mStream->write( arrowColor );
    mStream->write( duration );
    mStream->write( wireFrameArrow );
  }
}


void StreamRenderDebug::DebugCylinder(const HeF32 *p1,const HeF32 *p2,HeF32 radius,HeU32 color,HeF32 duration,bool useZ,bool solid)
{
  if ( isValidStream() )
  {
    mStream->write( (HeU32)SRDC_DEBUG_CYLINDER );
    mStream->write( p1[0] );
    mStream->write( p1[1] );
    mStream->write( p1[2] );

    mStream->write( p2[0] );
    mStream->write( p2[1] );
    mStream->write( p2[2] );

    mStream->write( radius );
    mStream->write( color );
    mStream->write( duration );
    mStream->write( useZ );
    mStream->write( solid );
  }
}

void StreamRenderDebug::DebugPolygon(HeU32 pcount,const HeF32 *points,HeU32 color,HeF32 duration,bool useZ,bool solid,bool clockwise)
{
  if ( isValidStream() )
  {
    mStream->write( (HeU32)SRDC_DEBUG_POLYGON );
    mStream->write( pcount );
    const HeF32 *p = points;
    for (HeU32 i=0; i<pcount; i++)
    {
      mStream->write( p[0] );
      mStream->write( p[1] );
      mStream->write( p[2] );
      p+=3;
    }

    mStream->write( color );
    mStream->write( duration );
    mStream->write( useZ );
    mStream->write( solid );
    mStream->write( clockwise );
  }
}

void  StreamRenderDebug::DebugRay(const HeF32 *p1,const HeF32 *p2,HeF32 arrowSize,HeU32 color,HeU32 arrowColor,HeF32 duration,bool useZ)
{
  if ( isValidStream() )
  {
    mStream->write( (HeU32)SRDC_DEBUG_RAY );
    mStream->write( p1[0] );
    mStream->write( p1[1] );
    mStream->write( p1[2] );

    mStream->write( p2[0] );
    mStream->write( p2[1] );
    mStream->write( p2[2] );

    mStream->write( arrowSize );
    mStream->write( color );
    mStream->write( arrowColor );
    mStream->write( duration );
    mStream->write( useZ );
  }
}

void  StreamRenderDebug::DebugPlane(const HeF32 *plane,HeF32 radius1,HeF32 radius2,HeU32 color,HeF32 duration,bool useZ)
{
}

void  StreamRenderDebug::DebugTri(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,HeU32 color,HeF32 duration,bool useZ)
{
  if ( isValidStream() )
  {
    mStream->write( (HeU32) SRDC_DEBUG_TRI );
    mStream->writeData( p1, sizeof(HeF32)*3 );
    mStream->writeData( p2, sizeof(HeF32)*3 );
    mStream->writeData( p3, sizeof(HeF32)*3 );
    mStream->write(color);
    mStream->write(duration);
    mStream->write(useZ);
  }
}

void  StreamRenderDebug::DebugSolidTri(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,HeU32 color,HeF32 duration)
{
  if ( isValidStream() )
  {
    mStream->write( (HeU32) SRDC_DEBUG_SOLID_TRI_1 );
    mStream->writeData( p1, sizeof(HeF32)*3 );
    mStream->writeData( p2, sizeof(HeF32)*3 );
    mStream->writeData( p3, sizeof(HeF32)*3 );
    mStream->write(color);
    mStream->write(duration);
  }
}

void  StreamRenderDebug::DebugSolidTri(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,const HeF32 *n1,const HeF32 *n2,const HeF32 *n3,HeU32 color,HeF32 duration)
{
  if ( isValidStream() )
  {
    mStream->write( (HeU32) SRDC_DEBUG_SOLID_TRI_2 );
    mStream->writeData( p1, sizeof(HeF32)*3 );
    mStream->writeData( p2, sizeof(HeF32)*3 );
    mStream->writeData( p3, sizeof(HeF32)*3 );
    mStream->writeData( n1, sizeof(HeF32)*3 );
    mStream->writeData( n2, sizeof(HeF32)*3 );
    mStream->writeData( n3, sizeof(HeF32)*3 );
    mStream->write(color);
    mStream->write(duration);
  }
}

void  StreamRenderDebug::DebugBound(const HeF32 *bmin,const HeF32 *bmax,HeU32 color,HeF32 duration,bool useZ,bool solid)
{
  if ( isValidStream() )
  {
    mStream->write( (HeU32)SRDC_DEBUG_BOUND );
    mStream->write( bmin[0] );
    mStream->write( bmin[1] );
    mStream->write( bmin[2] );
    mStream->write( bmax[0] );
    mStream->write( bmax[1] );
    mStream->write( bmax[2] );
    mStream->write(color);
    mStream->write(duration);
    mStream->write(useZ);
    mStream->write(solid);
  }
}

void  StreamRenderDebug::DebugOrientedBound(const HeF32 *sides,const HeF32 *transform,HeU32 color,HeF32 duration,bool useZ,bool solid)
{
  if ( isValidStream() )
  {
    mStream->write( (HeU32)SRDC_DEBUG_ORIENTED_BOUND_1);
    mStream->writeData( sides, sizeof(HeF32)*3 );
    mStream->writeData( transform, sizeof(HeF32)*16 );
    mStream->write( color );
    mStream->write( duration );
    mStream->write( useZ );
    mStream->write( solid );
  }
}

void  StreamRenderDebug::DebugOrientedBound(const HeF32 *bmin,const HeF32 *bmax,const HeF32 *pos,const HeF32 *quat,HeU32 color,HeF32 duration,bool useZ,bool solid) // the rotation as a quaternio
{
  if ( isValidStream() )
  {
    mStream->write( (HeU32)SRDC_DEBUG_ORIENTED_BOUND_2);
    mStream->writeData( bmin, sizeof(HeF32)*3 );
    mStream->writeData( bmax, sizeof(HeF32)*3 );
    mStream->writeData( pos, sizeof(HeF32)*3 );
    mStream->writeData( quat, sizeof(HeF32)*4 );
    mStream->write( color );
    mStream->write( duration );
    mStream->write( useZ );
    mStream->write( solid );
  }
}

void  StreamRenderDebug::DebugOrientedBound(const HeF32 *bmin,const HeF32 *bmax,const HeF32 *xform,HeU32 color,HeF32 duration,bool useZ,bool solid) // the rotation as a quaternio
{
  if ( isValidStream() )
  {
    mStream->write( (HeU32)SRDC_DEBUG_ORIENTED_BOUND_3);
    mStream->writeData( bmin, sizeof(HeF32)*3 );
    mStream->writeData( bmax, sizeof(HeF32)*3 );
    mStream->writeData( xform, sizeof(HeF32)*16 );
    mStream->write( color );
    mStream->write( duration );
    mStream->write( useZ );
    mStream->write( solid );
  }
}

void  StreamRenderDebug::DebugSphere(const HeF32 *pos,HeF32 radius,HeU32 color,HeF32 duration,bool useZ,bool solid)
{
  if ( isValidStream() )
  {
    mStream->write( (HeU32) SRDC_DEBUG_SPHERE );
    mStream->write( pos[0] );
    mStream->write( pos[1] );
    mStream->write( pos[2] );
    mStream->write( radius );
    mStream->write( color );
    mStream->write( duration );
    mStream->write( useZ );
    mStream->write(solid);
  }
}

void  StreamRenderDebug::DebugOrientedSphere(HeF32 radius,const HeF32 *transform,HeU32 color,HeF32 duration,bool useZ,bool solid)
{
  if ( isValidStream() )
  {
    mStream->write( (HeU32) SRDC_DEBUG_ORIENTED_SPHERE );
    mStream->write( radius );
    mStream->writeData(transform,sizeof(HeF32)*16);
    mStream->write( color );
    mStream->write( duration );
    mStream->write( useZ );
    mStream->write(solid);
  }
}

void  StreamRenderDebug::DebugCapsule(const HeF32 *center,HeF32 radius,HeF32 height,HeU32 color,HeF32 duration,bool useZ)
{
}

void  StreamRenderDebug::DebugOrientedCapsule(HeF32 radius,HeF32 height,const HeF32 *transform,HeU32 color,HeF32 duration,bool useZ)
{
}

void  StreamRenderDebug::DebugPoint(const HeF32 *pos,HeF32 radius,HeU32 color,HeF32 duration,bool useZ)
{
  if ( isValidStream() )
  {
    mStream->write( (HeU32) SRDC_DEBUG_POINT );
    mStream->write( pos[0] );
    mStream->write( pos[1] );
    mStream->write( pos[2] );
    mStream->write( radius );
    mStream->write( color );
    mStream->write( duration );
    mStream->write( useZ );
  }
}

void  StreamRenderDebug::DebugAxes(const HeF32 *transform,HeF32 distance,HeF32 brightness,bool useZ)
{
}

void  StreamRenderDebug::Reset( void )
{
}

bool  StreamRenderDebug::getWireFrame(void)
{
  return false;
}

HeF32 StreamRenderDebug::getRenderScale(void)
{
  return mRenderScale;
}

void  StreamRenderDebug::setRenderScale(HeF32 scale)
{
  mRenderScale = scale;
  if (  isValidStream() )
  {
    mStream->write( (HeU32) SRDC_SET_RENDER_SCALE );
    mStream->write(scale);
  }
}

void StreamRenderDebug::process(BaseRenderDebug *output) // process the current stream and send the results to the output debug object.
{
  HeU32 token;
  if ( mStream )
  {
    while ( mStream->read(token) )
    {
      switch ( token )
      {
        case SRDC_SET_RENDER_SCALE:
          {
            mStream->read(mRenderScale);
            output->setRenderScale(mRenderScale);
          }
          break;
        case SRDC_DEBUG_LINE:
          {
            HeF32 p1[3];
            HeF32 p2[3];
            HeU32 color;
            HeF32 duration;
            bool useZ;
            mStream->read(p1[0]);
            mStream->read(p1[1]);
            mStream->read(p1[2]);
            mStream->read(p2[0]);
            mStream->read(p2[1]);
            mStream->read(p2[2]);
            mStream->read(color);
            mStream->read(duration);
            mStream->read(useZ);
            output->DebugLine(p1,p2,color,duration,useZ);
          }
          break;
        case SRDC_DEBUG_ORIENTED_LINE:
          HE_ASSERT(0);
          break;
        case SRDC_DEBUG_THICK_RAY:
          {
            HeF32 p1[3];
            HeF32 p2[3];
            HeF32 raySize;
            HeF32 arrowSize;
            HeU32 color;
            HeU32 arrowColor;
            HeF32 duration;
            bool wireFrameArrow;

            mStream->read( p1[0] );
            mStream->read( p1[1] );
            mStream->read( p1[2] );

            mStream->read( p2[0] );
            mStream->read( p2[1] );
            mStream->read( p2[2] );

            mStream->read( raySize );
            mStream->read( arrowSize );
            mStream->read( color );
            mStream->read( arrowColor );
            mStream->read( duration );
            mStream->read( wireFrameArrow );

            output->DebugThickRay(p1,p2,raySize,arrowSize,color,arrowColor,duration,wireFrameArrow);
          }
          break;
        case SRDC_DEBUG_RAY:
          {
            HeF32 p1[3];
            HeF32 p2[3];
            HeF32 arrowSize;
            HeU32 color;
            HeU32 arrowColor;
            HeF32 duration;
            bool useZ;

            mStream->read( p1[0] );
            mStream->read( p1[1] );
            mStream->read( p1[2] );

            mStream->read( p2[0] );
            mStream->read( p2[1] );
            mStream->read( p2[2] );

            mStream->read( arrowSize );
            mStream->read( color );
            mStream->read( arrowColor );
            mStream->read( duration );
            mStream->read( useZ );

            output->DebugRay(p1,p2,arrowSize,color,arrowColor,duration,useZ);
          }
          break;
        case SRDC_DEBUG_CYLINDER:
          {
            HeF32 p1[3];
            HeF32 p2[3];
            HeF32 radius;
            HeU32 color;
            HeF32 duration;
            bool useZ;
            bool solid;

            mStream->read( p1[0] );
            mStream->read( p1[1] );
            mStream->read( p1[2] );

            mStream->read( p2[0] );
            mStream->read( p2[1] );
            mStream->read( p2[2] );

            mStream->read( radius );
            mStream->read( color );
            mStream->read( duration );
            mStream->read( useZ );
            mStream->read( solid );

            output->DebugCylinder(p1,p2,radius,color,duration,useZ,solid);
          }
          break;
        case SRDC_DEBUG_PLANE:
          HE_ASSERT(0);
          break;
        case SRDC_DEBUG_TRI:
          {
            HeF32 p1[3];
            HeF32 p2[3];
            HeF32 p3[3];
            HeU32 color;
            HeF32 duration;
            bool useZ;
            mStream->readData( p1, sizeof(HeF32)*3 );
            mStream->readData( p2, sizeof(HeF32)*3 );
            mStream->readData( p3, sizeof(HeF32)*3 );
            mStream->read(color);
            mStream->read(duration);
            mStream->read(useZ);
            output->DebugTri(p1,p2,p3,color,duration,useZ);
          }
          break;
        case SRDC_DEBUG_BOUND:
          {
            HeF32 bmin[3];
            HeF32 bmax[3];
            HeU32 color;
            HeF32 duration;
            bool useZ;
            bool solid;
            mStream->read( bmin[0] );
            mStream->read( bmin[1] );
            mStream->read( bmin[2] );
            mStream->read( bmax[0] );
            mStream->read( bmax[1] );
            mStream->read( bmax[2] );
            mStream->read(color);
            mStream->read(duration);
            mStream->read(useZ);
            mStream->read(solid);
            output->DebugBound(bmin,bmax,color,duration,useZ,solid);
          }
          break;
        case SRDC_DEBUG_SOLID_TRI_1:
          {
            HeF32 p1[3];
            HeF32 p2[3];
            HeF32 p3[3];
            HeU32 color;
            HeF32 duration;
            mStream->readData( p1, sizeof(HeF32)*3 );
            mStream->readData( p2, sizeof(HeF32)*3 );
            mStream->readData( p3, sizeof(HeF32)*3 );
            mStream->read(color);
            mStream->read(duration);
            output->DebugSolidTri(p1,p2,p3,color,duration);
          }
          break;
        case SRDC_DEBUG_SOLID_TRI_2:
          {
            HeF32 p1[3];
            HeF32 p2[3];
            HeF32 p3[3];
            HeF32 n1[3];
            HeF32 n2[3];
            HeF32 n3[3];
            HeU32 color;
            HeF32 duration;
            mStream->readData( p1, sizeof(HeF32)*3 );
            mStream->readData( p2, sizeof(HeF32)*3 );
            mStream->readData( p3, sizeof(HeF32)*3 );
            mStream->readData( n1, sizeof(HeF32)*3 );
            mStream->readData( n2, sizeof(HeF32)*3 );
            mStream->readData( n3, sizeof(HeF32)*3 );
            mStream->read(color);
            mStream->read(duration);
            output->DebugSolidTri(p1,p2,p3,n1,n2,n3,color,duration);
          }
          break;
        case SRDC_DEBUG_ORIENTED_BOUND_1:
          {
            HeF32 sides[3];
            HeF32 transform[16];
            HeU32 color;
            HeF32 duration;
            bool useZ;
            bool solid;
            mStream->readData( sides, sizeof(HeF32)*3 );
            mStream->readData( transform, sizeof(HeF32)*16 );
            mStream->read( color );
            mStream->read( duration );
            mStream->read( useZ );
            mStream->read( solid );
            output->DebugOrientedBound(sides,transform,color,duration,useZ,solid);
          }
          break;
        case SRDC_DEBUG_ORIENTED_BOUND_2:
          {
            HeF32 bmin[3];
            HeF32 bmax[3];
            HeF32 pos[3];
            HeF32 quat[4];
            HeU32 color;
            HeF32 duration;
            bool useZ;
            bool solid;
            mStream->readData( bmin, sizeof(HeF32)*3 );
            mStream->readData( bmax, sizeof(HeF32)*3 );
            mStream->readData( pos, sizeof(HeF32)*3 );
            mStream->readData( quat, sizeof(HeF32)*4 );
            mStream->read( color );
            mStream->read( duration );
            mStream->read( useZ );
            mStream->read( solid );
            output->DebugOrientedBound(bmin,bmax,pos,quat,color,duration,useZ,solid);
          }
          break;
        case SRDC_DEBUG_POLYGON:
          {
            HeU32 pcount;
            HeU32 color;
            HeF32 duration;
            bool  useZ;
            bool  solid;
            bool clockwise;

            mStream->read( pcount );

            HeF32 *points = 0;

            if ( pcount > 0 )
            {
              points = new HeF32[pcount*3];
              HeF32 *p = points;
              for (HeU32 i=0; i<pcount; i++)
              {
                mStream->read( p[0] );
                mStream->read( p[1] );
                mStream->read( p[2] );
                p+=3;
              }
            }

            mStream->read( color );
            mStream->read( duration );
            mStream->read( useZ );
            mStream->read( solid );
            mStream->read( clockwise );
            if ( points )
            {
              output->DebugPolygon(pcount,points,color,duration,useZ,solid,clockwise);
              delete []points;
            }
          }
          break;
        case SRDC_DEBUG_ORIENTED_BOUND_3:
          {
            HeF32 bmin[3];
            HeF32 bmax[3];
            HeF32 xform[16];
            HeU32 color;
            HeF32 duration;
            bool useZ;
            bool solid;
            mStream->readData( bmin, sizeof(HeF32)*3 );
            mStream->readData( bmax, sizeof(HeF32)*3 );
            mStream->readData( xform, sizeof(HeF32)*16 );
            mStream->read( color );
            mStream->read( duration );
            mStream->read( useZ );
            mStream->read( solid );
            output->DebugOrientedBound(bmin,bmax,xform,color,duration,useZ,solid);
          }
          break;
        case SRDC_DEBUG_SPHERE:
          {
            HeF32 pos[3];
            HeF32 radius;
            HeU32 color;
            HeF32 duration;
            bool useZ;
            bool solid;
            mStream->read( pos[0] );
            mStream->read( pos[1] );
            mStream->read( pos[2] );
            mStream->read( radius );
            mStream->read( color );
            mStream->read( duration );
            mStream->read( useZ );
            mStream->read(solid);
            output->DebugSphere(pos,radius,color,duration,useZ,solid);
          }
          break;
        case SRDC_DEBUG_ORIENTED_SPHERE:
          {
            HeF32 xform[16];
            HeF32 radius;
            HeU32 color;
            HeF32 duration;
            bool useZ;
            bool solid;
            mStream->read( radius );
            mStream->readData(xform,sizeof(HeF32)*16);
            mStream->read( color );
            mStream->read( duration );
            mStream->read( useZ );
            mStream->read(solid);
            output->DebugOrientedSphere(radius,xform,color,duration,useZ,solid);
          }
          break;
        case SRDC_DEBUG_CAPSULE:
          HE_ASSERT(0);
          break;
        case SRDC_DEBUG_ORIENTED_CAPSULE:
          HE_ASSERT(0);
          break;
        case SRDC_DEBUG_POINT:
          {
            HeF32 pos[3];
            HeF32 radius;
            HeU32 color;
            HeF32 duration;
            bool useZ;
            mStream->read( pos[0] );
            mStream->read( pos[1] );
            mStream->read( pos[2] );
            mStream->read( radius );
            mStream->read( color );
            mStream->read( duration );
            mStream->read( useZ );
            output->DebugPoint(pos,radius,color,duration,useZ);
          }
          break;
        case SRDC_DEBUG_AXES:
          HE_ASSERT(0);
          break;
        case SRDC_RESET:
          HE_ASSERT(0);
          break;
        case SRDC_GET_WIREFRAME:
          HE_ASSERT(0);
          break;
        case SRDC_GET_RENDER_SCALE:
          HE_ASSERT(0);
          break;
        case SRDC_BATCH_TRIANGLES:
          {
            HeU32 vcount;
            bool wireframe;
            bool overlay;
            HeU32 texture;
            mStream->read(texture);
            mStream->read(vcount);
            mStream->read(wireframe);
            mStream->read(overlay);
            const RENDER_DEBUG::GraphicsVertex *vertices = (const RENDER_DEBUG::GraphicsVertex *)mStream->getReadAddress(sizeof(RENDER_DEBUG::GraphicsVertex)*vcount);
            if ( vertices )
            {
              output->batchTriangles((RENDER_DEBUG::EmbedTexture)texture,vertices,vcount,wireframe,overlay);
            }
          }
          break;
      }
    }
  }
}

void StreamRenderDebug::batchTriangles(EmbedTexture texture,const RENDER_DEBUG::GraphicsVertex *vertices,HeU32 vcount,bool wireframe,bool overlay)
{
  if (  isValidStream() )
  {
    mStream->write( (HeU32) SRDC_BATCH_TRIANGLES );
    mStream->write( (HeU32) texture );
    mStream->write( vcount );
    mStream->write( wireframe );
    mStream->write( overlay );
    mStream->writeData( vertices, sizeof(RENDER_DEBUG::GraphicsVertex)*vcount);
  }
}

bool StreamRenderDebug::isValidStream(void) const
{
  bool ret = false;

  if ( mStream && mStream->getLoc() < mMaxStreamSize )
  {
    ret = true;
  }

  return ret;
}

void StreamRenderDebug::drawGrid(bool zup)
{

}


void         StreamRenderDebug::setScreenSize(HeU32 screenX,HeU32 screenY)
{
  mScreenWidth = screenX;
  mScreenHeight = screenY;
}

void         StreamRenderDebug::getScreenSize(HeU32 &screenX,HeU32 &screenY)
{
  screenX = mScreenWidth;
  screenY = mScreenHeight;
}

const HeF32 *StreamRenderDebug::getEyePos(void)
{
  return mEyePos.ptr();
}

void         StreamRenderDebug::setViewProjectionMatrix(const HeF32 *view,const HeF32 *projection)
{
  mViewMatrix.set(view);
  mProjectionMatrix.set(projection);
  mViewProjectionMatrix.multiply(mViewMatrix,mProjectionMatrix);
}

const HeF32 *StreamRenderDebug::getViewProjectionMatrix(void) const
{
  return mViewProjectionMatrix.ptr();
}

const HeF32 *StreamRenderDebug::getViewMatrix(void) const
{
  return mViewMatrix.ptr();
}

const HeF32 *StreamRenderDebug::getProjectionMatrix(void) const
{
  return mProjectionMatrix.ptr();
}

bool         StreamRenderDebug::screenToWorld(HeI32 sx,HeI32 sy,HeF32 *world,HeF32 *direction)
{
  bool ret = false;

#if 0
  HeI32 wid = (HeI32) mScreenWidth;
  HeI32 hit = (HeI32) mScrenHeight;

  if ( sx >= 0 && sx <= wid && sy >= 0 && sy <= hit )
  {
  	HeVec3 vPickRayDir;
  	HeVec3 vPickRayOrig;

  	HeVec3 ptCursor(sx,sy,0);

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
#endif

  return ret;
}


void StreamRenderDebug::setEmbedTexture(EmbedTexture t,LPHETEXTURE texture)
{
}

}; // end of namespace
