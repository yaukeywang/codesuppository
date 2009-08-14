#ifndef MENU_H

#define MENU_H

#define NOMINMAX
#include <windows.h>
#include "UserMemAlloc.h"

HMENU createMyMenu(void);
bool  processMenu(HWND hwnd,NxU32 cmd,NxF32 *bmin,NxF32 *bmax);
void  saveMenuState(void);

#endif
