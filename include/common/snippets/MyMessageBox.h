#ifndef MY_MESSAGE_BOX_H

#define MY_MESSAGE_BOX_H

#include "UserMemAlloc.h"

void myMessageBox(const char *fmt, ...);
bool myMessageBoxYesNo(const char *fmt, ...);
const char * getFileName(const char *fileType,const char *initial,const char *description,bool saveMode);


#endif
