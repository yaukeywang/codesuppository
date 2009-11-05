#ifndef SYSTEM_CALL_H

#define SYSTEM_CALL_H

namespace NVSHARE
{

bool systemCall(const char* strFunct,
                const char* strstrParams,
                bool hideWindow,
                bool waitForCompletion,
                bool createNewConsole);

}; // end of namespace

#endif
