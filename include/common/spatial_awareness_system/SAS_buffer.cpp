#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <math.h>
#include <wchar.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#if defined(LINUX)
#include "linux_compat.h"
#endif

#pragma warning(disable:4996)


#include "SAS_buffer.h"

namespace SPATIAL_AWARENESS_SYSTEM
{



SAS_Buffer::SAS_Buffer(SpatialAwarenessObserver *observer)
{
  mObserver = observer;
  mParent = Factory::Create(SAS_LAZY_KDTREE,this);
}

SAS_Buffer::~SAS_Buffer(void)
{
  Factory::Destroy(mParent);
}


void SAS_Buffer::Pump(SecondsType time_elapsed_since_last_pump)
{
  mParent->Pump(time_elapsed_since_last_pump);
}

bool SAS_Buffer::SetActive(ActiveType new_active_flag)
{
  bool ret;

  ret = mParent->SetActive(new_active_flag);

  return true;
}

bool SAS_Buffer::SetName(NameType new_name)
{
  bool ret = false;

  ret = mParent->SetName(new_name);

  return ret;
}

bool SAS_Buffer::SetUpdatePeriod(SecondsType new_update_period)
{
  bool ret = false;

  ret = mParent->SetUpdatePeriod(new_update_period);

  return ret;
}

bool SAS_Buffer::AddEntity(ID entity_ID)
{
  bool ret = true;

  SasEntry sa(SC_ADD_ENTITY,entity_ID);
  mCommands.push_back(sa);

  return ret;
}

bool SAS_Buffer::DeleteEntity(ID entity_ID,bool flushMessagesImmediately)
{
  bool ret = true;

  SasEntry sa(SC_DELETE_ENTITY,entity_ID,flushMessagesImmediately);
  mCommands.push_back(sa);

  return ret;
}

bool SAS_Buffer::UpdateEntityPosition(ID entity_ID, const float position_vec3[3])
{
  bool ret = true;

  SasEntry sa(SC_UPDATE_ENTITY_POSITION,entity_ID,position_vec3);
  mCommands.push_back(sa);


  return ret;
}

bool SAS_Buffer::UpdateEntityAwarenessRange(ID entity_ID, float range_f)
{
  bool ret = true;

  SasEntry sa(SC_UPDATE_ENTITY_AWARENESS_RANGE,entity_ID,range_f);
  mCommands.push_back(sa);


  return ret;
}

unsigned int SAS_Buffer::iterateAwareness(ID entity,IDVector &list)
{
  unsigned int ret = 0;

  ret = mParent->iterateAwareness(entity,list);

  return ret;
}

unsigned int SAS_Buffer::iterateAll(SpatialAwarenessIteratorCallback *callback)
{
  unsigned int ret = 0;

  ret = mParent->iterateAll(callback);

  return ret;
}

bool SAS_Buffer::setProperty(const char *key,const char *value)
{
  bool ret = false;

  ret = mParent->setProperty(key,value);

  return ret;
}

bool SAS_Buffer::setProperty(ID entity,const char *key,const char *value)
{
  bool ret = true;

  StringRef _key = mStringDict.Get(key);
  StringRef _value = mStringDict.Get(value);
  SasEntry sa( SC_SET_PROPERTY, entity, key, value );
  mCommands.push_back(sa);

  return ret;
}

SpatialAwarenessStrategy SAS_Buffer::getStrategy(void) const
{
  SpatialAwarenessStrategy ret;

  ret = SAS_BUFFER;

  return ret;
}

bool SAS_Buffer::UpdateEntityAwarenessTime(ID entity_ID,SecondsType time)
{
  bool ret = true;

  SasEntry sa(SC_UPDATE_ENTITY_AWARENESS_TIME,entity_ID,(float)time);
  mCommands.push_back(sa);

  return ret;
}

unsigned int SAS_Buffer::iterateAwareOf(ID entity,IDVector &list)
{
  unsigned int ret = 0;

  ret = mParent->iterateAwareOf(entity,list);

  return ret;

}


void SAS_Buffer::PrePump(void)
{
  if ( !mCommands.empty() )
  {
    SasEntryVector::iterator i;
    for (i=mCommands.begin(); i!=mCommands.end(); ++i)
    {
      SasEntry &e = (*i);
      switch ( e.mCommand )
      {
        case SC_ADD_ENTITY:
          mParent->AddEntity(e.mId);
          break;
        case SC_DELETE_ENTITY:
          mParent->DeleteEntity(e.mId,e.mState);
          break;
        case SC_SET_PROPERTY:
          mParent->setProperty( e.mId, e.mKey, e.mKeyValue);
          break;
        case SC_UPDATE_ENTITY_AWARENESS_RANGE:
          mParent->UpdateEntityAwarenessRange(e.mId, e.mValue[0] );
          break;
        case SC_UPDATE_ENTITY_POSITION:
          mParent->UpdateEntityPosition(e.mId, e.mValue );
          break;
        case SC_UPDATE_ENTITY_AWARENESS_TIME:
          mParent->UpdateEntityAwarenessTime(e.mId, e.mValue[0] );
          break;

      }
    }
    mCommands.clear();
  }
}

void SAS_Buffer::PostPump(void)
{
  if ( !mMessages.empty() )
  {
    AwareMessageVector::iterator i;
    for (i=mMessages.begin(); i!=mMessages.end(); ++i)
    {
      AwareMessage &a = (*i);
      switch ( a.mState )
      {
        case AS_APPEARED:
          mObserver->SAO_appeared(a.mFrom,a.mTo);
          break;
        case AS_DISAPPEARED:
          mObserver->SAO_disappeared(a.mFrom,a.mTo);
          break;
        case AS_ENTERED:
          mObserver->SAO_entered(a.mFrom,a.mTo);
          break;
        case AS_DEPARTED:
          mObserver->SAO_departed(a.mFrom,a.mTo);
          break;
      }
    }
    mMessages.clear();
  }
}

void SAS_Buffer::SAO_entered(ID entity,ID subject)
{
  AwareMessage a(AS_ENTERED,entity,subject,0);
  mMessages.push_back(a);
}

void SAS_Buffer::SAO_departed(ID entity,ID subject)
{
  AwareMessage a(AS_DEPARTED,entity,subject,0);
  mMessages.push_back(a);
}

void SAS_Buffer::SAO_appeared(ID entity,ID subject)
{
  AwareMessage a(AS_APPEARED,entity,subject,0);
  mMessages.push_back(a);
}

void SAS_Buffer::SAO_disappeared(ID entity,ID subject)
{
  AwareMessage a(AS_DISAPPEARED,entity,subject,0);
  mMessages.push_back(a);
}


}; // end of namespace
