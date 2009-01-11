#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TestWinMsg.h"

#include "common/snippets/SendTextMessage.h"

void testWinMsg(void)
{
  SEND_TEXT_MESSAGE(0,"Demonstrates how to use the WinMsg class for interprocess communication.  Originally published on May 112, 2006\r\n");
  SEND_TEXT_MESSAGE(0,"NO YET IMPLEMENTED\r\n");
}

