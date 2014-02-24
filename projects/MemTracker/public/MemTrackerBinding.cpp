#include "MemTrackerBinding.h"
#include <Windows.h>

MEM_TRACKER::MemTracker *gMemTracker=NULL;

namespace MEM_TRACKER
{

MEM_TRACKER::MemTracker * getMemTracker(uint32_t versionNumber,const char *dllName) // Loads the MemTracker DLL
{
	MEM_TRACKER::MemTracker *ret = NULL;
	UINT errorMode = 0;
	errorMode = SEM_FAILCRITICALERRORS;
	UINT oldErrorMode = SetErrorMode(errorMode);
	HMODULE module = LoadLibraryA(dllName);
	SetErrorMode(oldErrorMode);
	if ( module )
	{
		void *proc = GetProcAddress(module,"createMemTracker");
		if ( proc )
		{
			typedef void * (__cdecl * NX_GetToolkit)(uint32_t versionNumber);
			ret = (MemTracker *)((NX_GetToolkit)proc)(versionNumber);
			gMemTracker = ret;
		}
	}
	return ret;
}

}; // end of MEM_TRACKER namesapce



