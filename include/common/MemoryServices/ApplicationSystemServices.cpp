#include "../snippets/SystemServices.h"
#include "../snippets/UserMemAlloc.h"
#include "ApplicationSystemServices.h"
#include <new>

bool globalNewDeleteExists(void);

class SendTextMessage;
SendTextMessage *gSendTextMessage=0;

namespace RESOURCE_INTERFACE
{
  class ResourceInterface;
};

RESOURCE_INTERFACE::ResourceInterface *gResourceInterface=0;

namespace SYSTEM_SERVICES
{

SystemServices *gSystemServices=0;

#if HE_USE_MEMORY_TRACKING
class DefaultSystemServices : public ApplicationSystemServices
{
public:
  DefaultSystemServices(void)
  {
    globalNewDeleteExists();
    gSystemServices = static_cast< SystemServices *>(this);
  }
};


SystemServices * init(void)
{
  if ( gSystemServices == 0 )
  {
    void *foo = ::malloc(sizeof(DefaultSystemServices));
    gSystemServices = new ( foo ) DefaultSystemServices;
  }
  return gSystemServices;
}
#endif

}; // end of namespace
