#ifndef MENU_H

#define MENU_H

#define NOMINMAX
#include <windows.h>

extern bool gView3d; // true if viewing in 3d mode.

HMENU createMyMenu(void);
bool  processMenu(HWND hwnd,NxU32 cmd,NxF32 *bmin,NxF32 *bmax);
void  saveMenuState(void);
void  MenuSetup(void);

#endif
