#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TestClipper.h"

#include "common/snippets/SendTextMessage.h"

void testClipper(void)
{
  SEND_TEXT_MESSAGE(0,"Demonstration of the Cohen-Sutherland clipping algorithm originally published on March 7, 2006\r\n");
  SEND_TEXT_MESSAGE(0,"NO YET IMPLEMENTED\r\n");
}

