#ifndef SAS_BUFFER_H

#define SAS_BUFFER_H

#include <stdio.h>

#include "spatial_awareness_system.h"
#include "../snippets/UserMemAlloc.h"
#include "../snippets/stringdict.h"
#include "../snippets/simplevector.h"

namespace SPATIAL_AWARENESS_SYSTEM
{

typedef SimpleVector< AwareMessage > AwareMessageVector;

enum SasCommand
{
  SC_ADD_ENTITY,
  SC_DELETE_ENTITY,
  SC_SET_PROPERTY,
  SC_UPDATE_ENTITY_AWARENESS_RANGE,
  SC_UPDATE_ENTITY_AWARENESS_TIME,
  SC_UPDATE_ENTITY_POSITION,
  SC_LAST
};

class SasEntry
{
public:
  SasEntry(SasCommand c,HeU64 id)
  {
    mCommand = c;
    mId      = id;
  }
  SasEntry(SasCommand c,HeU64 id,bool state)
  {
    mCommand = c;
    mId      = id;
    mState   = state;
  }

  SasEntry(SasCommand c,ID entity_ID, const float position_vec3[3])
  {
    mCommand = c;
    mId      = entity_ID;
    mValue[0] = position_vec3[0];
    mValue[1] = position_vec3[1];
    mValue[2] = position_vec3[2];
  }
  SasEntry(SasCommand c,ID entity_ID,float v)
  {
    mCommand = c;
    mId      = entity_ID;
    mValue[0] = v;
  }
  SasEntry(SasCommand c,ID entity_ID,const char *key,const char *value)
  {
    mCommand = c;
    mId      = entity_ID;
    mKey     = key;
    mKeyValue    = value;
  }


  SasCommand mCommand;
  HeU64      mId;
  HeF32      mValue[3];
  bool       mState;
  const char *mKey;
  const char *mKeyValue;
};

typedef SimpleVector< SasEntry > SasEntryVector;



  class SAS_Buffer : public SpatialAwarenessSystem, public SpatialAwarenessObserver
  {
  public:
    SAS_Buffer(SpatialAwarenessObserver *observer);
    ~SAS_Buffer(void);

    virtual HeSize getMemoryUsage();
    void Pump(SecondsType time_elapsed_since_last_pump);
    bool SetActive(ActiveType new_active_flag);
    bool SetName(NameType new_name);
    bool SetUpdatePeriod(SecondsType new_update_period);
    bool AddEntity(ID entity_ID);
    bool DeleteEntity(ID entity_ID,bool flushMessagesImmediately);
    bool UpdateEntityPosition(ID entity_ID, const float position_vec3[3]);
    bool UpdateEntityAwarenessRange(ID entity_ID, float range_f);
    bool UpdateEntityAwarenessTime(ID entity_ID,SecondsType time);

    unsigned int iterateAwareness(ID entity,IDVector &list);
    unsigned int iterateAwareOf(ID entity,IDVector &list);

    unsigned int iterateAll(SpatialAwarenessIteratorCallback *callback);
    bool setProperty(const char *key,const char *value);
    bool setProperty(ID entity,const char *key,const char *value);
    SpatialAwarenessStrategy getStrategy(void) const;

    void PrePump(void);
    void PostPump(void);
    void SAO_entered(ID entity,ID subject);
    void SAO_departed(ID entity,ID subject);
    void SAO_appeared(ID entity,ID subject);
    void SAO_disappeared(ID entity,ID subject);

  private:
    SpatialAwarenessSystem     *mParent;
    SpatialAwarenessObserver   *mObserver;
    AwareMessageVector          mMessages;
    SasEntryVector              mCommands;
    StringDict                  mStringDict;
  };

}; // END OF NAMESPACE




#endif
