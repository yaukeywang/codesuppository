/* HeroEngine, Copyright 2002-2008 Simutronics, Corp. All rights reserved. */

#ifndef _INCLUDED_COMMON_SPATIAL_AWARENESS_SYSTEM_H
#define _INCLUDED_COMMON_SPATIAL_AWARENESS_SYSTEM_H

#include <list>
#include <vector>

#include "../snippets/HeSimpleTypes.h"
//! \file spatial_awareness_system.h

//! The spatial awareness system is documented at <http://officewiki.play.net/mediawiki/index.php?title=User:ANDY/Spatial_Awareness_System>.
namespace SPATIAL_AWARENESS_SYSTEM
{


enum SpatialAwarenessStrategy
{
  SAS_LAZY_KDTREE,      // Use a 'lazy' KdTree.  The KdTree is only rebuilt ever 16 times, which can cause some rare false negatives (never false positives though).  Only 500 entities are evaluated per frame.
  SAS_LOG,              // create a spatial awareness system that logs all of the API calls.
  SAS_BUFFER,           // buffers all calls
};


//  define types compatible with the GOM

//! ID holds a user-supplied ID number for this instance, unique to the SpatialAwarenessSystem.
typedef HeU64             ID;                  //-ignore
typedef HeF64             SecondsType;         //-ignore
typedef bool              ActiveType;          //-ignore
typedef const wchar_t *   NameType;            //-ignore
typedef HeU64             FrameType;           //-ignore

typedef std::vector< ID > IDVector; // an STL vector of a list of ID's

enum AwareState
{
  AS_APPEARED,
  AS_DISAPPEARED,
  AS_ENTERED,
  AS_DEPARTED
};

class AwareMessageInterface
{
public:
  virtual void postAwareMessage(AwareState state,ID from,ID to) = 0;
};



class AwareMessage
{
public:
  AwareMessage(AwareState state,ID from,ID to,SecondsType time)
  {
    mState = state;
    mFrom  = from;
    mTo    = to;
    mTime  = time;
  }

  AwareState  mState;
  ID          mFrom;
  ID          mTo;
  SecondsType mTime;
};



class SpatialAwarenessObserver
{
public:
  virtual void SAO_entered(ID entity,ID subject) = 0;
  virtual void SAO_departed(ID entity,ID subject) = 0;
  virtual void SAO_appeared(ID entity,ID subject) = 0;
  virtual void SAO_disappeared(ID entity,ID subject) = 0;
};

class SpatialAwarenessIteratorCallback
{
public:
  virtual bool SAI_iterate(ID entity,ID subject) = 0;  // return true if you want to continue iterating or false to cancel iteration.
};

class SpatialAwarenessSystem
{
public:
  //! Pump is called periodically by the host program. Returns true if the pump occurred.
  virtual void Pump(SecondsType time_elapsed_since_last_pump) = 0;
  virtual void PrePump(void) { };
  virtual void PostPump(void) { };

  //! UpdateInstanceActive is called by the host program to set the value of the active flag of an instance of the entity awareness system.
  virtual bool SetActive(ActiveType new_active_flag) = 0;

  //! SetName is called by the host program to set the value of the name of an instance of the entity awareness system.
  virtual bool SetName(NameType new_name) = 0;

  //! SetUpdatePeriod is called by the host program to set the value of the update period of an instance of the entity awareness system.
  virtual bool SetUpdatePeriod(SecondsType new_update_period) = 0;


  //! AddEntity is called by the host program to add an entity to a specified instance of the entity awareness system.
  virtual bool AddEntity(ID entity_ID) = 0;

  //! DeleteEntity is called by the host program to delete an entity from a specified instance of the entity awareness system.
  virtual bool DeleteEntity(ID entity_ID,bool flushMessagesImmediately=false) = 0;

  //! UpdateEntityPosition is called by the host program to update the position of an entity in a specified instance of the entity awareness system.
  virtual bool UpdateEntityPosition(ID entity_ID, const HeF32 position_vec3[3]) = 0;

  //! UpdateEntityAwarenessRange indicates the range of awarness for this specific entity relative to others.  This is a 3d sphere.
  virtual bool UpdateEntityAwarenessRange(ID entity_ID, HeF32 range_f) = 0;

  //! UpdateEntityAwarenessTime indicates the 'time' before an awareness event will be generated.
  virtual bool UpdateEntityAwarenessTime(ID entity_ID,SecondsType time) = 0;

  virtual SpatialAwarenessStrategy getStrategy(void) const = 0;

  virtual unsigned int iterateAwareness(ID entity,IDVector &list) = 0;
  virtual unsigned int iterateAwareOf(ID entity,IDVector &list) = 0;

  virtual unsigned int iterateAll(SpatialAwarenessIteratorCallback *callback) = 0; //

  virtual bool setProperty(const char *key,const char *value) = 0;
  virtual bool setProperty(ID entity,const char *key,const char *value) = 0;



};

class Factory
{
public:
  //! Create a spatial awareness system.
  static SpatialAwarenessSystem* Create(SpatialAwarenessStrategy strategy,SpatialAwarenessObserver *observer);

  //! Destroy a spatial awareness system.
  //! NOTE: the pointer value will be NULLed out upon exit from this routine.
  static void Destroy(SpatialAwarenessSystem* & value_becomes_null_afterwards);

  static void Pump(SecondsType time_elapsed_since_last_pump);
  static void PrePump(void);
  static void PostPump(void);

  typedef std::list< SpatialAwarenessSystem * > SpatialAwarenessSystemList;
  static SpatialAwarenessSystemList const & GetList();

protected:
private:
  Factory(void);
  ~Factory(void);
  static SpatialAwarenessSystemList  mList;
};



} // END OF NAMESPACE


#endif
