#ifndef BLOB_IO

#define BLOB_IO

// transmits and receives binary 'blobs' using ASCII test lines.

class BlobIOInterface
{
public:
  virtual void sendBlobText(unsigned int client,const char *fmt,...) = 0;
};

class BlobIO
{
public:
  virtual bool sendBlob(unsigned int client,const char *blobType,const void *blobData,unsigned int blobLen) = 0;
  virtual const char * receiveBlob(unsigned int &client,const void *&data,unsigned int &dlen) = 0;
protected:
  BlobIO(void) { };
};


BlobIO * createBlobIO(BlobIOInterface *iface);
void     releaseBlobIO(BlobIO *b);

#endif
