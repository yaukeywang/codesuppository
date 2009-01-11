#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

#define INSTANT_MESSAGE 0

#pragma warning(disable:4996)

#include "InstantMessage.h"
#include "../common/snippets/He.h"

#if INSTANT_MESSAGE
#include "./AIM/AccCore.h"     // AIMCC main header
#include "./AIM/AccSupport.h"  // AIMCC C++ helper classes
#pragma comment(lib, "acccore.lib")    // aimcc main lib
#pragma comment(lib, "accuuidlib.lib") // aimcc uuid lib
using namespace AIMCC;
#endif

#if defined(LINUX)
#define _vsnprintf vsnprintf
#endif

#if INSTANT_MESSAGE
class MyInstantMessage : public InstantMessage, public CAccEventSink
#else
class MyInstantMessage : public InstantMessage
#endif
{
public:
  MyInstantMessage(InstantMessageInterface *i)
  {
    mInterface = i;
#if INSTANT_MESSAGE
    m_sp = NULL;
#endif
  }

  ~MyInstantMessage(void)
  {
    disconnect();
  }

  bool connect(const char * /*userName*/,const char * /*password*/)
  {
    bool ret = false;

    disconnect();

#if INSTANT_MESSAGE
    // 1. create aimcc main object, hook up for events
    // 2. set information to identify this client
    // 3. specify username and password, and sign on
    HRESULT hr;
    if (SUCCEEDED(hr = AccCreateSession(IID_IAccSession, (void**)&m_sp)) && SUCCEEDED(hr = Advise(m_sp)))
    {
      CAccPtr<IAccClientInfo> spClientInfo;
      hr = m_sp->get_ClientInfo(&spClientInfo);
      if (SUCCEEDED(hr))
      {
        CAccVariant desc("StatusMonitor (key=jr1us2cxLD2ItTW9)");
        spClientInfo->put_Property(AccClientInfoProp_Description, desc);
        if (SUCCEEDED(hr = m_sp->put_Identity(CAccBstr(userName))))
        {
          hr = m_sp->SignOn(CAccBstr(password));
          if ( SUCCEEDED(hr) )
          {
            ret = true;
          }
        }
      }
    }
#endif
    return ret;
  }

  bool disconnect(void)
  {
    bool ret = false;

#if INSTANT_MESSAGE
    if ( m_sp )
    {
      // clean up events and aimcc object
      Unadvise(m_sp);
      m_sp = NULL;
      ret = true;
    }
#endif

    return ret;
  }

  bool sendInstantMessage(const wchar_t * /*userId*/,const wchar_t * /*msg*/)
  {
    bool ret = false;

#if INSTANT_MESSAGE
    // ok...send the message to this user..
    if ( m_sp )
    {
      IAccImSession *session;
      HRESULT hr = m_sp->CreateImSession(userId,AccImSessionType_Im,&session);
      if ( SUCCEEDED(hr) )
      {
        CAccPtr<IAccIm> spiResponse;
        hr = m_sp->CreateIm(NULL, OLESTR("text/plain"), &spiResponse);
        if ( SUCCEEDED(hr) )
        {
          hr = spiResponse->put_Text(msg);
          session->SendIm(spiResponse);
        }
        session->Release();
      }
    }
#endif

    return ret;
  }

  bool sendInstantMessage(const char *userId,const char *fmt,...)
  {
    bool ret = false;

  	char buff[8192];
    buff[8191] = 0;
  	_vsnprintf(buff,8191, fmt, (char *)(&fmt+1));
    wchar_t wbuff[8192];
  	mbstowcs(wbuff,buff,8191);
    wchar_t user[512];
  	mbstowcs(user,userId,511);
    ret = sendInstantMessage(user,wbuff);

    return ret;
  }



  bool pump(void)
  {
    bool ret = false;

#if INSTANT_MESSAGE
    if ( m_sp )
    {
      AccDispatchMessages(1);
    }
#endif

    return ret;
  }

  const char * getInstantMessageStateStr(InstantMessageState state)
  {
    const char *ret = "IMS_NONE";
    switch ( state )
    {
      case IMS_NONE: ret = "IMS_NONE"; break;
      case IMS_OFFLINE: ret = "IMS_OFFLINE"; break;
      case IMS_DISCONNECTED: ret = "IMS_DISCONNECTED"; break;
      case IMS_QUERYING_DCS: ret = "IMS_QUERYING_DCS"; break;
      case IMS_CONNECTING: ret = "IMS_CONNECTING"; break;
      case IMS_CHALLENGING: ret = "IMS_CHALLENGING"; break;
      case IMS_VALIDATING: ret = "IMS_VALIDATING"; break;
      case IMS_SECURE_ID: ret = "IMS_SECURE_ID"; break;
      case IMS_SECURET_ID_NEXT_KEY: ret = "IMS_SECURET_ID_NEXT_KEY"; break;
      case IMS_TRANSFERRING: ret = "IMS_TRANSFERRING"; break;
      case IMS_NEGOTIATING: ret = "IMS_NEGOTIATING"; break;
      case IMS_STARTING: ret = "IMS_STARTING"; break;
      case IMS_ONLINE: ret = "IMS_ONLINE"; break;
      case IMS_WILL_SHUTDOWN: ret = "IMS_WILL_SHUTDOWN"; break;
      case IMS_SHUTDOWN: ret = "IMS_SHUTDOWN"; break;
      case IMS_PAUSED: ret = "IMS_PAUSED"; break;
      default:
        HE_ASSERT(0);
        break;
    }
    return ret;
  }


#if INSTANT_MESSAGE
  void OnStateChange(IAccSession* piSession, AccSessionState state, AccResult hr)
  {
    InstantMessageState ims = IMS_NONE;
    switch ( state )
    {
      case AccSessionState_Offline: ims = IMS_OFFLINE; break;
      case AccSessionState_Disconnected: ims = IMS_DISCONNECTED; break;
      case AccSessionState_QueryingDcs: ims = IMS_QUERYING_DCS; break;
      case AccSessionState_Connecting: ims = IMS_CONNECTING; break;
      case AccSessionState_Challenging: ims = IMS_CHALLENGING; break;
      case AccSessionState_Validating: ims = IMS_VALIDATING; break;
      case AccSessionState_SecurId: ims = IMS_SECURE_ID; break;
      case AccSessionState_SecurIdNextKey: ims = IMS_SECURET_ID_NEXT_KEY; break;
      case AccSessionState_Transferring: ims = IMS_TRANSFERRING; break;
      case AccSessionState_Negotiating: ims = IMS_NEGOTIATING; break;
      case AccSessionState_Starting: ims = IMS_STARTING; break;
      case AccSessionState_Online: ims = IMS_ONLINE; break;
      case AccSessionState_WillShutdown: ims = IMS_WILL_SHUTDOWN; break;
      case AccSessionState_Shutdown: ims = IMS_SHUTDOWN; break;
      case AccSessionState_Paused: ims = IMS_PAUSED; break;
      default:
        HE_ASSERT(0);
        break;
    }
    if (state == AccSessionState_Offline)
    {
      AccPostQuit(hr);
    }
    HE_ASSERT(mInterface);
    if (mInterface)
      mInterface->notifyInstantMessageState(ims);
  }

  void OnSecondarySessionStateChange(IAccSession* piSession, IAccSecondarySession* piSecSession, AccSecondarySessionState state, AccResult hr)
  {
    // always accept incoming IM sessions
    if (state == AccSecondarySessionState_ReceivedProposal)
    {
      AccSecondarySessionServiceId id;
      piSecSession->get_ServiceId(&id);
      if (id == AccSecondarySessionServiceId_Im)
      {
        piSecSession->Accept();
      }
    }
  }

  void OnImReceived(IAccSession* piSession, IAccImSession* piImSession, IAccParticipant* piSender, IAccIm* piIm)
  {
    xp_str pName;
    piSender->get_Name(&pName);
    CAccBstr text;
    piIm->GetConvertedText(OLESTR("text/plain"), &text);
    const wchar_t *from   = pName;
    const wchar_t *message = text;
    mInterface->receiveInstantMessage(from,message);
  }
#endif

private:
  InstantMessageInterface *mInterface;
#if INSTANT_MESSAGE
  CAccPtr<IAccSession>     m_sp;
#endif
};


InstantMessage * createInstantMessage(InstantMessageInterface *imi)
{
  MyInstantMessage *mim = new MyInstantMessage(imi);
  return static_cast< InstantMessage * >(mim);
}

void             releaseInstantMessage(InstantMessage *im)
{
  MyInstantMessage *mim = static_cast< MyInstantMessage * >(im);
  delete mim;
}

