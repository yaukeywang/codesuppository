#include <assert.h>
#include "SAS_lazy_kdtree.h"

#include "../snippets/UserMemAlloc.h"
#include "SAS_log.h"
#include "SAS_buffer.h"


namespace SPATIAL_AWARENESS_SYSTEM
{

Factory::SpatialAwarenessSystemList  Factory::mList;

SpatialAwarenessSystem* Factory::Create(SpatialAwarenessStrategy strategy,SpatialAwarenessObserver *observer)
{
  SpatialAwarenessSystem *ret = 0;

  switch ( strategy )
  {
    case SAS_BUFFER:
      {
        SAS_Buffer *sas = MEMALLOC_NEW(SAS_Buffer)(observer);
        ret = static_cast< SpatialAwarenessSystem * >(sas);
      }
      break;
    case SAS_LOG:
      {
        SAS_Log *sas = MEMALLOC_NEW(SAS_Log)(observer);
        ret = static_cast< SpatialAwarenessSystem * >(sas);
      }
      break;
    case SAS_LAZY_KDTREE:
      {
        SAS_LazyKdTree *sas = MEMALLOC_NEW(SAS_LazyKdTree)(observer);
        ret = static_cast< SpatialAwarenessSystem * >(sas);
      }
      break;
  }

  if ( ret )
  {
    mList.push_back(ret);
  }

  return ret;
}

void Factory::Destroy(SpatialAwarenessSystem* & value_becomes_null_afterwards)
{
  if ( value_becomes_null_afterwards )
  {

    bool found = false;

    SpatialAwarenessSystemList::iterator i;
    for (i=mList.begin(); i!=mList.end(); i++)
    {
      if ( (*i) == value_becomes_null_afterwards )
      {
        mList.erase(i);
        found = true;
        break;
      }
    }

    assert(found);

    switch ( value_becomes_null_afterwards->getStrategy() )
    {
      case SAS_BUFFER:
        {
          SAS_Buffer *sas = static_cast< SAS_Buffer * >(value_becomes_null_afterwards);
          delete sas;
        }
        break;
      case SAS_LOG:
        {
          SAS_Log *sas = static_cast< SAS_Log * >(value_becomes_null_afterwards);
          delete sas;
        }
        break;
      case SAS_LAZY_KDTREE:
        {
          SAS_LazyKdTree *sas = static_cast< SAS_LazyKdTree * >(value_becomes_null_afterwards);
          delete sas;
        }
        break;
    }
  }
  value_becomes_null_afterwards = 0;
}

void Factory::PrePump(void)
{
  SpatialAwarenessSystemList::iterator i;
  for (i=mList.begin(); i!=mList.end(); i++)
  {
    (*i)->PrePump();
  }

}

void Factory::PostPump(void)
{
  SpatialAwarenessSystemList::iterator i;
  for (i=mList.begin(); i!=mList.end(); i++)
  {
    (*i)->PostPump();
  }

}


void Factory::Pump(SecondsType time_elapsed_since_last_pump)
{
  SpatialAwarenessSystemList::iterator i;
  for (i=mList.begin(); i!=mList.end(); i++)
  {
    (*i)->Pump(time_elapsed_since_last_pump);
  }

}

}; // END OF NAMESPACE
