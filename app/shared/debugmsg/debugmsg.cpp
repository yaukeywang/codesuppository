#include "debugmsg.h"
#include "common/snippets/winmsg.h"
#include <assert.h>
#include <string.h>


enum DebugMessageType
{
  DMT_DEBUG_TRI,     // render a debug triangle.
  DMT_RESET
};


class DebugMessage
{
public:
  DebugMessage(void)
  {
    mId[0] = 'J';
    mId[1] = 'O';
    mId[2] = 'H';
    mId[3] = 'N';
  }

  char              mId[4];
  DebugMessageType  mType;
};

class ResetMessage : DebugMessage
{
public:
  ResetMessage(void)
  {
    mType = DMT_RESET;
  }
};

enum SingleTriFlag
{
  STF_USEZ = (1<<0),
  STF_LINE = (1<<1),
  STF_SOLID = (1<<2),
  STF_POINT = (1<<3),
  STF_NORMALS = (1<<4),
  STF_SPHERE = (1<<5),
};

class SingleTri
{
public:
  void set(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,HeU32 color,HeF32 duration,SingleTriFlag flag)
  {
    mP1[0] = p1[0];
    mP1[1] = p1[1];
    mP1[2] = p1[2];

    mP2[0] = p2[0];
    mP2[1] = p2[1];
    mP2[2] = p2[2];

    if (p3 )
    {
      mP3[0] = p3[0];
      mP3[1] = p3[1];
      mP3[2] = p3[2];
    }

    mColor = color;
    mDuration = duration;
    mFlags = flag;
  }

  void set(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,const HeF32 *n1,const HeF32 *n2,const HeF32 *n3,HeU32 color,HeF32 duration,SingleTriFlag flag)
  {
    mP1[0] = p1[0];
    mP1[1] = p1[1];
    mP1[2] = p1[2];

    mP2[0] = p2[0];
    mP2[1] = p2[1];
    mP2[2] = p2[2];

    mP3[0] = p3[0];
    mP3[1] = p3[1];
    mP3[2] = p3[2];

    mN1[0] = n1[0];
    mN1[1] = n1[1];
    mN1[2] = n1[2];

    mN2[0] = n2[0];
    mN2[1] = n2[1];
    mN2[2] = n2[2];

    mN3[0] = n3[0];
    mN3[1] = n3[1];
    mN3[2] = n3[2];

    mColor = color;
    mDuration = duration;
    mFlags = flag;
  }

  void set(const HeF32 *pos,HeF32 radius,HeU32 color,HeF32 duration,SingleTriFlag flag)
  {
    mP1[0] = pos[0];
    mP1[1] = pos[1];
    mP1[2] = pos[2];

    mP2[0] = radius;

    mColor = color;
    mDuration = duration;
    mFlags = flag;
  }

  void setSingleTriFlag(SingleTriFlag flag) { mFlags|=flag; };
  void clearSingleTriFlag(SingleTriFlag flag) { mFlags&=~flag; };
  bool hasSingleTriFlag(SingleTriFlag flag) const
  {
    bool ret = false;
    if ( mFlags & flag ) ret = true;
    return ret;
  }


  HeF32         mP1[3];
  HeF32         mP2[3];
  HeF32         mP3[3];

  HeF32         mN1[3];
  HeF32         mN2[3];
  HeF32         mN3[3];

  HeU32  mColor;
  HeF32         mDuration;
  HeU32  mFlags;
};

#define BATCHCOUNT 4096

class DebugMessageTri : public DebugMessage
{
public:
  DebugMessageTri(void)
  {
    mType = DMT_DEBUG_TRI;
    mCount = 0;
  }

  bool add(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,HeU32 color,HeF32 duration,SingleTriFlag flag)
  {
    bool ret = false;

    mSingleTri[mCount].set(p1,p2,p3,color,duration,flag);
    mCount++;
    if ( mCount == BATCHCOUNT )
      ret = true;
    return ret;
  }

  bool add(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,const HeF32 *n1,const HeF32 *n2,const HeF32 *n3,HeU32 color,HeF32 duration,SingleTriFlag flag)
  {
    bool ret = false;

    mSingleTri[mCount].set(p1,p2,p3,n1,n2,n3,color,duration,flag);
    mCount++;
    if ( mCount == BATCHCOUNT )
      ret = true;
    return ret;
  }

  bool add(const HeF32 *pos,HeF32 radius,HeU32 color,HeF32 duration,SingleTriFlag flag)
  {
    bool ret = false;

    mSingleTri[mCount].set(pos,radius,color,duration,flag);
    mCount++;
    if ( mCount == BATCHCOUNT )
      ret = true;
    return ret;
  }

  HeI32       mCount;
  SingleTri mSingleTri[BATCHCOUNT];
};


static DebugMessageTri sendTri;

#if DEBUG_SERVER

#include "RenderDebug/RenderDebug.h"

class DebugSystem : public WinMsgReceive
{
public:

	bool receiveMessage(const char *msg,HeU32 len)
  {
    bool ret = false;

    if ( len >= 6 && msg[0] == 'J' && msg[1] == 'O' && msg[2] == 'H' && msg[3] == 'N' )
    {
      const HeI32 *value = (const HeI32 *)&msg[4];
      switch ( *value )
      {
        case DMT_RESET:
          if ( gRenderDebug )
            gRenderDebug->Reset();
          break;
        case DMT_DEBUG_TRI:
          if ( gRenderDebug )
          {
            assert( len == sizeof(DebugMessageTri));
            if ( len == sizeof(DebugMessageTri) )
            {
              DebugMessageTri dmt;
              memcpy(&dmt,msg,len);
              SingleTri *st = dmt.mSingleTri;
              for (HeI32 i=0; i<dmt.mCount; i++)
              {

                if ( st->hasSingleTriFlag(STF_POINT) )
                  gRenderDebug->DebugPoint(st->mP1,st->mP2[0], st->mColor, st->mDuration, st->hasSingleTriFlag(STF_USEZ) );
                else if ( st->hasSingleTriFlag(STF_SPHERE) )
                  gRenderDebug->DebugSphere(st->mP1,st->mP2[0], st->mColor, st->mDuration, st->hasSingleTriFlag(STF_USEZ) );
                else if ( st->hasSingleTriFlag(STF_SOLID) )
                {
                  if ( st->hasSingleTriFlag(STF_NORMALS) )
                    gRenderDebug->DebugSolidTri(st->mP1,st->mP2,st->mP3, st->mN1, st->mN2, st->mN3, st->mColor, st->mDuration );
                  else
                    gRenderDebug->DebugSolidTri(st->mP1,st->mP2,st->mP3, st->mColor, st->mDuration );
                }
                else if ( st->hasSingleTriFlag(STF_LINE) )
                  gRenderDebug->DebugLine(st->mP1,st->mP2, st->mColor, st->mDuration, st->hasSingleTriFlag(STF_USEZ) );
                else
                  gRenderDebug->DebugTri(st->mP1,st->mP2,st->mP3, st->mColor, st->mDuration, st->hasSingleTriFlag(STF_USEZ) );
                st++;
              }
            }
          }
          break;
      }
    }
    return ret;
  }
};

static DebugSystem *gDebugSystem=0;

bool openDebug(void)
{
  bool ret = false;

  if ( gDebugSystem == 0 )
  {
    gDebugSystem = MEMALLOC_NEW(DebugSystem);
    gDebugSystem->initMsg("SkeletonD3D");
    ret = true;
  }

  return ret;
}

bool closeDebug(void)
{
  bool ret = false;
  if ( gDebugSystem )
  {
    delete gDebugSystem;
    gDebugSystem = 0;
    ret = true;
  }
  return ret;
}

bool processDebug(void)
{
  bool ret = false;

  if ( gDebugSystem )
  {
    gDebugSystem->checkWinMsg();
    ret = true;
  }
  return ret;
}

#endif

void debugTri(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,HeU32 color,HeF32 duration,bool useZ)
{
  HeU32 flag = 0;
  if ( useZ ) flag=STF_USEZ;

  bool send = sendTri.add(p1,p2,p3,color,duration,(SingleTriFlag) flag);
  if ( send )
  {
    sendWinMsgBinary("SkeletonD3D", &sendTri, sizeof(DebugMessageTri) );
    sendTri.mCount = 0;
  }
}

void debugSolidTri(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,HeU32 color,HeF32 duration)
{
  bool send = sendTri.add(p1,p2,p3,color,duration,STF_SOLID);
  if ( send )
  {
    sendWinMsgBinary("SkeletonD3D", &sendTri, sizeof(DebugMessageTri) );
    sendTri.mCount = 0;
  }
}

void debugSolidTri(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,const HeF32 *n1,const HeF32 *n2,const HeF32 *n3,HeU32 color,HeF32 duration)
{
  bool send = sendTri.add(p1,p2,p3,n1,n2,n3,color,duration,(SingleTriFlag)(STF_SOLID | STF_NORMALS));
  if ( send )
  {
    sendWinMsgBinary("SkeletonD3D", &sendTri, sizeof(DebugMessageTri) );
    sendTri.mCount = 0;
  }
}

void debugLine(const HeF32 *p1,const HeF32 *p2,HeU32 color,HeF32 duration,bool useZ)
{
  HeU32 flag = STF_LINE;
  if ( useZ ) flag=STF_USEZ;

  bool send = sendTri.add(p1,p2,0,color,duration,(SingleTriFlag) flag);
  if ( send )
  {
    sendWinMsgBinary("SkeletonD3D", &sendTri, sizeof(DebugMessageTri) );
    sendTri.mCount = 0;
  }
}

void flushDebug(void)
{
  if ( sendTri.mCount )
  {
    sendWinMsgBinary("SkeletonD3D", &sendTri, sizeof(DebugMessageTri) );
    sendTri.mCount = 0;
  }
}


void debugSphere(const HeF32 *pos,HeF32 radius,HeU32 color,HeF32 duration,bool useZ)
{
  HeU32 flag = STF_SPHERE;
  if ( useZ ) flag|=STF_USEZ;

  bool send = sendTri.add(pos,radius,color,duration,(SingleTriFlag) flag);
  if ( send )
  {
    sendWinMsgBinary("SkeletonD3D", &sendTri, sizeof(DebugMessageTri) );
    sendTri.mCount = 0;
  }
}


void debugPoint(const HeF32 *pos,HeF32 radius,HeU32 color,HeF32 duration,bool useZ)
{
  HeU32 flag = STF_POINT;
  if ( useZ ) flag|=STF_USEZ;

  bool send = sendTri.add(pos,radius,color,duration,(SingleTriFlag) flag);
  if ( send )
  {
    sendWinMsgBinary("SkeletonD3D", &sendTri, sizeof(DebugMessageTri) );
    sendTri.mCount = 0;
  }
}

void resetDebug(void)
{
  ResetMessage msg;
  sendWinMsgBinary("SkeletonD3D", &msg, sizeof(ResetMessage) );
}

