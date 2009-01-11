#ifndef ENCRYPTION_H

#define ENCRYPTION_H

#include "common/snippets/UserMemAlloc.h"
#include "rand.h"
#include "crc.h"


typedef HeU32 CRC32; // crc datatype.
const CRC32 CRC32_POLYNOMIAL=0xEDB88320L;

#define ENABLED 1

// Encrpt some data with some key.
class Encryption
{
public:
  Encryption(void)
  {
    mFirst = true;
  }

  void Encrypt(HeU8 *mem,HeI32 len,HeI32 key)
  {
#if ENABLED
    Rand r(key); // seed random number generator.
    HeU8 *source = mem;
    HeU8 *dest   = mem;
    HeI32 i;
    for (i=0; i<len; i++)
    {
      HeU8 k = (HeU8)r.get();
      *dest++ = *source++ ^ k;
    }
#endif
  }

  void Decrypt(HeU8 *mem,HeI32 len,HeI32 key)
  {
    Encrypt(mem,len,key);
  }

	HeU32 ComputeCRC(const void *buffer,HeI32 count,HeU32 crc=0)
  {
    crc = crc^count;
    const HeU8 * p = (const HeU8*) buffer;
    while ( count-- != 0 )
    {
      ComputeCRC( *p++, crc );
    }
    return( crc&0x7FFFFFFF );
  }

  HeU32 ComputeCRC(char c,HeU32 &crc)  // add into accumulated crc.
  {
    if ( mFirst )
    {
      BuildCRCTable();
      mFirst = false;
    }
    HeU32 temp1 = ( crc >> 8 ) & 0x00FFFFFFL;
    HeU32 temp2 = CRCTable[ ( (HeI32) crc ^ c ) & 0xff ];
    crc = temp1 ^ temp2;
    return crc;
  }

private:

  void BuildCRCTable(void)
  {
    HeI32 i;
    HeI32 j;
    HeU32 crc;

    for ( i = 0; i <= 255 ; i++ )
    {
      crc = i;
      for ( j = 8 ; j > 0; j-- )
      {
        if ( crc & 1 )
  				crc = ( crc >> 1 ) ^ CRC32_POLYNOMIAL;
        else
  				crc >>= 1;
      }
  		CRCTable[i] = crc;
    }
  }

  bool         mFirst;
  HeU32 CRCTable[256];

};

#endif




