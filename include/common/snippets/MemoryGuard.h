#ifndef MEMORY_GUARD_H

#define MEMORY_GUARD_H

#include <assert.h>

#pragma warning(disable:4702)

class MemoryGuard
{
public:
  MemoryGuard(void)
  {
    const char *id = "MEMORYG";
    size_t count = 0;
    char *dest = mKey;
    while ( *id  && count < 8 )
    {
      *dest++ = *id++;
      count++;
    }
  }

  ~MemoryGuard(void)
  {
    const char *id = "MEMORYG";
    size_t count = 0;
    while ( *id && count < 8 )
    {
      assert( *id == mKey[count] );
      if ( *id != mKey[count] )
      {
        char *c = 0; // force a crash
        *c = 0;
      }
      mKey[count] = 0;
      id++;
      count++;
    }
  }

  bool validate(void) const
  {
    bool ret = true;

    const char *id = "MEMORYG";
    size_t count = 0;
    while ( *id && count < 8 )
    {
      assert( *id == mKey[count] );
      if ( *id != mKey[count] )
      {
        ret = false;
        break;
      }
      id++;
      count++;
    }
    return ret;
  }

  char        mKey[8];
};



#endif
