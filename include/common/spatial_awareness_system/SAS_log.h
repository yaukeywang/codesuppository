#ifndef SAS_LOG_H

#define SAS_LOG_H

#include <stdio.h>

#include "spatial_awareness_system.h"

namespace SPATIAL_AWARENESS_SYSTEM
{

  class SAS_Log : public SpatialAwarenessSystem
  {
  public:
    SAS_Log(SpatialAwarenessObserver *observer);
    ~SAS_Log(void);

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
  private:
    SpatialAwarenessSystem     *mParent;
    FILE                       *mFph;
  };

}; // END OF NAMESPACE




#endif
