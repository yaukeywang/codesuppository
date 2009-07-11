#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "BlobIO.h"
#include "UserMemAlloc.h"

#define BLOB_LINE 256

static char gHexTable[16] = { '0', '1', '2', '3','4','5','6','7','8','9','A','B','C','D','E','F' };

static inline char getHex(unsigned char c)
{
    return gHexTable[c];
}

class MyBlobIO : public BlobIO
{
public:
  MyBlobIO(BlobIOInterface *iface)
  {
    mCallback = iface;
    mBlobId   = 0;
  }

  // convert a blob of binary data into multiple lines of ascii data
  virtual bool sendBlob(unsigned int client,const char *blobType,const void *blobData,unsigned int blobLen)
  {
	bool ret = false;
    if ( mCallback && blobLen > 0 )
    {
		assert(blobType);
		assert(blobData);
        if ( blobLen <= BLOB_LINE )
        {
            char blobText[BLOB_LINE*2+1];
            const unsigned char *scan = (const unsigned char *)blobData;
            char *dest = blobText;
            for (unsigned int i=0; i<blobLen; i++)
            {
                unsigned char c = *scan++;
                dest[0] = getHex(c>>4);
                dest[1] = getHex(c&0xF);
                dest+=2;
            }
            *dest = 0;
            mCallback->sendBlobText(client,"<telnetBlob blob=%s>%s</telnetBlob>\r\n", blobType, blobText );
        }
        else
        {
            mBlobId++;
            char blobText[BLOB_LINE*2+1];
            const unsigned char *scan = (const unsigned char *)blobData;
            char *dest = blobText;
            for (unsigned int i=0; i<BLOB_LINE; i++)
            {
                unsigned char c = *scan++;
                dest[0] = getHex(c>>4);
                dest[1] = getHex(c&0xF);
                dest+=2;
            }
            *dest = 0;
            blobLen-=BLOB_LINE;
            mCallback->sendBlobText(client,"<telnetBlob blob=%s blobId=%d>%s</telnetBlob>\r\n", blobType, mBlobId, blobText );
            while ( blobLen > BLOB_LINE )
            {
              char *dest = blobText;
              for (unsigned int i=0; i<BLOB_LINE; i++)
              {
                  unsigned char c = *scan++;
                  dest[0] = getHex(c>>4);
                  dest[1] = getHex(c&0xF);
                  dest+=2;
              }
              *dest = 0;
              blobLen-=BLOB_LINE;
              mCallback->sendBlobText(client,"<telnetBlobData blobId=%d>%s</telnetBlobData>\r\n", mBlobId, blobText );
            }
            dest = blobText;
            for (unsigned int i=0; i<blobLen; i++)
            {
                unsigned char c = *scan++;
                dest[0] = getHex(c>>4);
                dest[1] = getHex(c&0xF);
                dest+=2;
            }
            *dest = 0;
            mCallback->sendBlobText(client,"<telnetBlobEnd blobId=%d>%s</telnetBlobEnd>\r\n", mBlobId, blobText );
        }
    }
	return ret;
  }

  virtual const char * receiveBlob(unsigned int &client,const void *&data,unsigned int &dlen)
  {
    const char *ret  = 0;
	client = 0;
    data = 0;
    dlen = 0;
    return ret;
  }

private:
  unsigned int    mBlobId;
  BlobIOInterface *mCallback;
};



BlobIO * createBlobIO(BlobIOInterface *iface)
{
    MyBlobIO * m = MEMALLOC_NEW(MyBlobIO)(iface);
    return static_cast< BlobIO *>(m);
}

void     releaseBlobIO(BlobIO *b)
{
    MyBlobIO *m = static_cast< MyBlobIO *>(b);
    MEMALLOC_DELETE(MyBlobIO,m);
}
