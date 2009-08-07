#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TestInPlaceParser.h"

#include "SendTextMessage.h"

void testInPlaceParser(void)
{
  SEND_TEXT_MESSAGE(0,"Demonstration of the InPlaceParser fast ASCII parser originally published on March 6, 2006.\r\n");
  SEND_TEXT_MESSAGE(0,"NO YET IMPLEMENTED\r\n");
}

