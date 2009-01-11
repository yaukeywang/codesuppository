#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TestCompression.h"

#include "common/snippets/SendTextMessage.h"

void testCompression(void)
{
  SEND_TEXT_MESSAGE(0,"Demonstrates how to use the Gzip/Zlib compression library. Originally published on March 25, 2008\r\n");
  SEND_TEXT_MESSAGE(0,"NO YET IMPLEMENTED\r\n");
}

