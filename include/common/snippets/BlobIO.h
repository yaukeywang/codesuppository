#ifndef BLOB_IO_H

#define BLOB_IO_H

#include "UserMemAlloc.h"

// transmits and receives binary 'blobs' using ASCII test lines.

class BlobIOInterface
{
public:
  virtual void sendBlobText(NxU32 client,const char *fmt,...) = 0;
};

class BlobIO
{
public:
  virtual bool sendBlob(NxU32 client,const char *blobType,const void *blobData,NxU32 blobLen) = 0;
  virtual const char * receiveBlob(NxU32 &client,const void *&data,NxU32 &dlen) = 0;
  virtual bool processIncomingBlobText(NxU32 client,const char *text) = 0;
protected:
  BlobIO(void) { };
};


BlobIO * createBlobIO(BlobIOInterface *iface);
void     releaseBlobIO(BlobIO *b);

#endif
