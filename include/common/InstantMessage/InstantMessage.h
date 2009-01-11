#ifndef INSTANT_MESSAGE_H

#define INSTANT_MESSAGE_H

// This is a wrapper API to handle instant messaging on top of AOL instant messenger or AIM
// Requires the AIM instant message SDK to build.
//
// Please be aware of all of the licensing issues associated with using the AIM SDK
//
// This wrapper was written by John W. Ratcliff.  Please send bug-fixes feedback to jratcliff@infiniplex.net

enum InstantMessageState
{
  IMS_NONE,
  IMS_OFFLINE,
  IMS_DISCONNECTED,
  IMS_QUERYING_DCS,
  IMS_CONNECTING,
  IMS_CHALLENGING,
  IMS_VALIDATING,
  IMS_SECURE_ID,
  IMS_SECURET_ID_NEXT_KEY,
  IMS_TRANSFERRING,
  IMS_NEGOTIATING,
  IMS_STARTING,
  IMS_ONLINE,
  IMS_WILL_SHUTDOWN,
  IMS_SHUTDOWN,
  IMS_PAUSED,
};


class InstantMessageInterface
{
public:
  virtual void notifyInstantMessageState(InstantMessageState state) = 0;
  virtual void receiveInstantMessage(const wchar_t *from,const wchar_t *message) = 0;
};

class InstantMessage
{
public:

  virtual bool connect(const char *userName,const char *password) = 0;  // connect using this user id and password.
  virtual bool disconnect(void) = 0; // dissconnected
  virtual bool pump(void) = 0; // pump the instant message system on this thread, handles event processing.

  virtual const char * getInstantMessageStateStr(InstantMessageState state) = 0; // conver the instant message enum into a string.

  virtual bool sendInstantMessage(const wchar_t *userId,const wchar_t *msg) = 0; // send an instant message to a buddy (unicode string)
  virtual bool sendInstantMessage(const char *userId,const char *fmt,...) = 0;  // send an instant message to a buddy using the printf style syntax

};


InstantMessage * createInstantMessage(InstantMessageInterface *imi);
void             releaseInstantMessage(InstantMessage *im);

#endif
