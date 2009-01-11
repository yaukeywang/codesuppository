#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ResourceUpdate.h"
#include "ResourceInterface.h"
#include "../snippets/sutil.h"
#include <list>
#include <map>

namespace FIRE_STORM_RESOURCE
{

typedef USER_STL::list< FireStormResourceInterface * > FireStormResourceInterfaceList;

class ResourceUpdate
{
public:

  bool cancel(FireStormResourceInterface *iface)
  {
    bool ret = false;
    if ( !mInterfaces.empty() )
    {
      FireStormResourceInterfaceList::iterator i = mInterfaces.begin();
      while ( i != mInterfaces.end() )
      {
        if ( (*i) == iface )
        {
          ret = true;
          i = mInterfaces.erase(i);
        }
        else
        {
          i++;
        }
      }
    }
    return ret;
  }

  bool empty(void) const
  {
    return mInterfaces.empty();
  }

  void add(FireStormResourceInterface *iface)
  {
    mInterfaces.push_back(iface);
  }

  void notify(const std::string &str)
  {
    wchar_t scratch[512];
    CharToWide(str.c_str(),scratch,512);
    FireStormResourceInterfaceList::iterator i;
    for (i=mInterfaces.begin(); i!=mInterfaces.end(); ++i)
    {
      (*i)->notifyResourceChanged(scratch);
    }
  }

  FireStormResourceInterfaceList mInterfaces;
};


typedef USER_STL::map< std::string, ResourceUpdate > ResourceUpdateMap;

class ResourceUpdateSystem
{
public:
  ResourceUpdateSystem(void)
  {
  }

  ~ResourceUpdateSystem(void)
  {
  }


  void registerUpdate(const char *_fqn,FireStormResourceInterface *iface)
  {
    char fqn[512];
    normalizeFQN(_fqn,fqn);
    std::string str = fqn;
    ResourceUpdateMap::iterator found = mResources.find(str);
    if ( found != mResources.end() )
    {
      (*found).second.add(iface);
    }
    else
    {
      ResourceUpdate r;
      r.add(iface);
      mResources[str] = r;
    }
  }

  void cancel(FIRE_STORM_RESOURCE::FireStormResourceInterface *iface)
  {
    if ( !mResources.empty() )
    {
      ResourceUpdateMap::iterator i = mResources.begin();
      ResourceUpdateMap::iterator itemp;
      while ( i != mResources.end() )
      {
        ResourceUpdate &r = (*i).second;
        if ( r.cancel(iface) )
        {
          if ( r.empty() )
          {
            itemp = i;
            itemp++;
            mResources.erase(i);
            i = itemp;
          }
          else
          {
            i++;
          }
        }
        else
        {
          i++;
        }
      }
    }
  }

  void process(const char *_fqn)
  {
    char fqn[512];
    normalizeFQN(_fqn,fqn);
    std::string str = fqn;
    ResourceUpdateMap::iterator found = mResources.find(str);
    if ( found != mResources.end() )
    {
      (*found).second.notify( (*found).first );
    }
  }

private:
  ResourceUpdateMap mResources;
};

static ResourceUpdateSystem gUpdateSystem;

void registerResourceUpdate(const char *fqn,FireStormResourceInterface *iface)
{
  gUpdateSystem.registerUpdate(fqn,iface);
}

void registerResourceCancel(FIRE_STORM_RESOURCE::FireStormResourceInterface *iface)
{
  gUpdateSystem.cancel(iface);
}

void processResourceUpdate(const char *fqn)
{
  gUpdateSystem.process(fqn);
}


};


