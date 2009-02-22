#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ResourceUpdate.h"
#include "ResourceInterface.h"
#include "../snippets/sutil.h"

#include <list>
#include <map>
#include <string>

namespace RESOURCE_INTERFACE
{

typedef USER_STL::list< ResourceInterfaceCallback * > ResourceInterfaceList;

class MyResourceUpdate
{
public:
  MyResourceUpdate(void)
  {
  }

  bool cancel(ResourceInterfaceCallback *iface)
  {
    bool ret = false;

    if ( !mInterfaces.empty() )
    {
      ResourceInterfaceList::iterator i = mInterfaces.begin();
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

  void add(ResourceInterfaceCallback *iface)
  {
    bool added = false;

    ResourceInterfaceList::iterator i;
    for (i=mInterfaces.begin(); i!=mInterfaces.end(); ++i)
    {
      ResourceInterfaceCallback *c = (*i);
      if ( c ) 
      {
        added = true;
        break;
      }
    }
    if ( !added )
    {
      mInterfaces.push_back(iface);
    }
  }

  void notify(const USER_STL::string &str)
  {
    ResourceInterfaceList temp = mInterfaces;
    ResourceInterfaceList::iterator i;
    for (i=temp.begin(); i!=temp.end(); ++i)
    {
      (*i)->notifyResourceChanged(str.c_str());
    }
  }

  ResourceInterfaceList mInterfaces;
};



typedef USER_STL::map< USER_STL::string, MyResourceUpdate > ResourceUpdateMap;

class ResourceUpdateSystem
{
public:
  ResourceUpdateSystem(void)
  {
  }

  ~ResourceUpdateSystem(void)
  {
  }

  void registerUpdate(const char *_fqn,ResourceInterfaceCallback *iface)
  {
    char fqn[512];
    normalizeFQN(_fqn,fqn);
    USER_STL::string str = fqn;
    ResourceUpdateMap::iterator found = mResources.find(str);
    if ( found != mResources.end() )
    {
      (*found).second.add(iface);
    }
    else
    {
      MyResourceUpdate r;
      r.add(iface);
      mResources[str] = r;
    }
  }

  void cancel(RESOURCE_INTERFACE::ResourceInterfaceCallback *iface)
  {
    if ( !mResources.empty() )
    {
      ResourceUpdateMap::iterator i = mResources.begin();
      ResourceUpdateMap::iterator itemp;
      while ( i != mResources.end() )
      {
        MyResourceUpdate &r = (*i).second;
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
    USER_STL::string str = fqn;
    ResourceUpdateMap::iterator found = mResources.find(str);
    if ( found != mResources.end() )
    {
      (*found).second.notify( (*found).first );
    }
  }

private:
  ResourceUpdateMap mResources;
};

static ResourceUpdateSystem *gUpdateSystem=0;

static void init(void)
{
  if ( gUpdateSystem == 0 )
  {
    gUpdateSystem = MEMALLOC_NEW(ResourceUpdateSystem);
  }
}

void registerResourceUpdate(const char *fqn,ResourceInterfaceCallback *iface)
{
  init();
  gUpdateSystem->registerUpdate(fqn,iface);
}

void registerResourceCancel(RESOURCE_INTERFACE::ResourceInterfaceCallback *iface)
{
  init();
  gUpdateSystem->cancel(iface);
}

void processResourceUpdate(const char *fqn)
{
  init();
  gUpdateSystem->process(fqn);
}

};


