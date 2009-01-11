#ifndef MENU_H

#define MENU_H

#define NOMINMAX
#include <windows.h>

HMENU createMyMenu(void);
bool  processMenu(HWND hwnd,HeU32 cmd,HeF32 *bmin,HeF32 *bmax);
void  saveMenuState(void);

#endif
