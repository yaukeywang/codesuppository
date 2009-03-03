#ifndef SAS_LAZY_KDTREE_H

#define SAS_LAZY_KDTREE_H

#include "spatial_awareness_system.h"

namespace SPATIAL_AWARENESS_SYSTEM
{

// Key-Value properties:
//
// Key            :   Value
//=================================
// RefreshPer          500      : Designates how many entities have their awareness checked each pump.
// SearchPer         10000      : Designates maximum number of 'found' nodes allowed in each refresh frame.
// RebuildTree          16      : Designates how many pump loops we execute before rebuilding the KdTree
// DefaultGranularity    0      : Sets the default granularity size (slop range to avoid constant enter/exit events when objects hover near the activation distance)
// Granularity           0      : Set the granularity for a specific entity.
// MessageLatency       0.25    : Latency before a message will be transmitted.

class LazyKdTree;

class SAS_LazyKdTree : public SpatialAwarenessSystem
{
public:
  SAS_LazyKdTree(SpatialAwarenessObserver *observer);
  ~SAS_LazyKdTree(void);

   virtual HeSize getMemoryUsage();
   void Pump(SecondsType time_elapsed_since_last_pump);
   bool SetActive(ActiveType new_active_flag);
   bool SetName(NameType new_name);
   bool SetUpdatePeriod(SecondsType new_update_period);
   bool AddEntity(ID entity_ID);
   bool          DeleteEntity(ID entity_ID,bool flushMessagesImmediately);
   bool          UpdateEntityPosition(ID entity_ID, const float position_vec3[3]);
   bool          UpdateEntityAwarenessRange(ID entity_ID, float range_f);
   unsigned int  iterateAwareness(ID entity,IDVector &list);
   unsigned int  iterateAll(SpatialAwarenessIteratorCallback *callback);
   bool          UpdateEntityAwarenessTime(ID entity_ID,SecondsType time);
   unsigned int  iterateAwareOf(ID entity,IDVector &list);


  SpatialAwarenessStrategy getStrategy(void) const
  {
    return SAS_LAZY_KDTREE;
  }

  bool setProperty(const char *key,const char *value);
  bool setProperty(ID entity,const char *key,const char *value);


private:
  LazyKdTree                *mLazy;
};

}; // END OF NAMESPACE

#endif
