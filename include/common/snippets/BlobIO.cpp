#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <list>

#include "BlobIO.h"
#include "FastXml.h"
#include "UserMemAlloc.h"

#pragma warning(disable:4996)

using namespace NVSHARE;

namespace BLOB_IO
{

#define BLOB_LINE 256

static char gHexTable[16] = { '0', '1', '2', '3','4','5','6','7','8','9','A','B','C','D','E','F' };

static inline char getHex(NxU8 c)
{
    return gHexTable[c];
}

#pragma warning(disable:4100)

static inline bool getHex(char c,NxU8 &v)
{
    bool ret = true;
    if ( c >= '0' && c <= '9' )
    {
        v = c-'0';
    }
    else if ( c >= 'A' && c <= 'F' )
    {
        v = (c-'A')+10;
    }
    else
    {
        ret = false;
    }
    return ret;
}

static inline bool getHexValue(char c1,char c2,NxU8 &v)
{
    bool ret = false;
    NxU8 v1,v2;
    if ( getHex(c1,v1) && getHex(c2,v2) )
    {
        v = v1<<4 | v2;
        ret = true;
    }
    return ret;
}

class Blob : public Memalloc
{
public:
  Blob(const char *blobType,NxU32 client,NxU32 blobId,NxU32 olen,const char *data)
  {
    NxU32 slen = strlen(blobType);
    mClient   = client;
    mFinished = false;
    mError    = false;
    mBlobType = (char *)MEMALLOC_MALLOC(slen+1);
    strcpy(mBlobType,blobType);
    mBlobId = blobId;
    mBlobLen  = olen;
    mBlobData = (NxU8 *)MEMALLOC_MALLOC(olen);
    mBlobIndex = 0;
    addData(data);
  }

  ~Blob(void)
  {
    MEMALLOC_FREE(mBlobType);
    MEMALLOC_FREE(mBlobData);
  }

  void addData(const char *data)
  {

    while ( mBlobIndex < mBlobLen && *data )
    {
        char c1 = data[0];
        char c2 = data[1];

        if ( getHexValue(c1,c2,mBlobData[mBlobIndex]) )
        {
            mBlobIndex++;
        }
        else
        {
            break;
        }
        data+=2;
    }

    if ( mBlobIndex == mBlobLen )
    {
        mFinished = true;
    }
  }

  void addDataEnd(const char *data)
  {
    addData(data);
    assert( mFinished );
  }

  NxU32 getId(void) const { return mBlobId; };

  bool           mFinished;
  bool           mError;
  char          *mBlobType;
  NxU32   mBlobId;
  NxU32   mBlobLen;
  NxU8 *mBlobData;
  NxU32   mBlobIndex;
  NxU32   mClient;
};

typedef std::list< Blob * > BlobList;

class MyBlobIO : public BlobIO, public FastXmlInterface, public Memalloc
{
public:
  MyBlobIO(BlobIOInterface *iface)
  {
    mCallback = iface;
    mBlobId   = 0;
    mLastBlob = 0;
    mFastXml = createFastXml();
  }

  ~MyBlobIO(void)
  {
    releaseFastXml(mFastXml);
    BlobList::iterator i;
    for (i=mBlobs.begin(); i!=mBlobs.end(); ++i)
    {
        Blob *b = (*i);
        delete b;
    }
    delete mLastBlob;
  }

  // convert a blob of binary data into multiple lines of ascii data
  virtual bool sendBlob(NxU32 client,const char *blobType,const void *blobData,NxU32 blobLen)
  {
	bool ret = false;
    if ( mCallback && blobLen > 0 )
    {
		assert(blobType);
		assert(blobData);
        if ( blobLen <= BLOB_LINE )
        {
            char blobText[BLOB_LINE*2+1];
            const NxU8 *scan = (const NxU8 *)blobData;
            char *dest = blobText;
            for (NxU32 i=0; i<blobLen; i++)
            {
                NxU8 c = *scan++;
                dest[0] = getHex(c>>4);
                dest[1] = getHex(c&0xF);
                dest+=2;
            }
            *dest = 0;
            mCallback->sendBlobText(client,"<telnetBlob blob=\"%s\" len=\"%d\">%s</telnetBlob>\r\n", blobType, blobLen, blobText );
        }
        else
        {
            mBlobId++;
            char blobText[BLOB_LINE*2+1];
            const NxU8 *scan = (const NxU8 *)blobData;
            char *dest = blobText;
            for (NxU32 i=0; i<BLOB_LINE; i++)
            {
                NxU8 c = *scan++;
                dest[0] = getHex(c>>4);
                dest[1] = getHex(c&0xF);
                dest+=2;
            }
            *dest = 0;
            mCallback->sendBlobText(client,"<telnetBlob blob=\"%s\" blobId=\"%d\" len=\"%d\">%s</telnetBlob>\r\n", blobType, mBlobId, blobLen, blobText );
            blobLen-=BLOB_LINE;
            while ( blobLen > BLOB_LINE )
            {
              char *dest = blobText;
              for (NxU32 i=0; i<BLOB_LINE; i++)
              {
                  NxU8 c = *scan++;
                  dest[0] = getHex(c>>4);
                  dest[1] = getHex(c&0xF);
                  dest+=2;
              }
              *dest = 0;
              blobLen-=BLOB_LINE;
              mCallback->sendBlobText(client,"<telnetBlobData blobId=\"%d\">%s</telnetBlobData>\r\n", mBlobId, blobText );
            }
            dest = blobText;
            for (NxU32 i=0; i<blobLen; i++)
            {
                NxU8 c = *scan++;
                dest[0] = getHex(c>>4);
                dest[1] = getHex(c&0xF);
                dest+=2;
            }
            *dest = 0;
            mCallback->sendBlobText(client,"<telnetBlobEnd blobId=\"%d\">%s</telnetBlobEnd>\r\n", mBlobId, blobText );
        }
    }
	return ret;
  }

  virtual const char * receiveBlob(NxU32 &client,const void *&data,NxU32 &dlen)
  {
    const char *ret  = 0;
	client = 0;
    data = 0;
    dlen = 0;

    delete mLastBlob;
    mLastBlob = 0;

    if ( !mBlobs.empty() )
    {
        BlobList::iterator i;
        for (i=mBlobs.begin(); i!=mBlobs.end(); ++i)
        {
            Blob *b = (*i);
            if ( b->mFinished )
            {
                mLastBlob = b;
                client = b->mClient;
                data   = b->mBlobData;
                dlen   = b->mBlobLen;
                ret    = b->mBlobType;
                mBlobs.erase(i);
                break;
            }
        }
    }

    return ret;
  }

  virtual bool processIncomingBlobText(NxU32 client,const char *text)
  {
	  bool ret = false;

	  client;

	  if ( strncmp(text,"<telnetBlob",11) == 0 )
	  {
		  size_t len = strlen(text);
          mClient = client;
          ret = mFastXml->processXml(text,len,this);
          if ( !ret )
          {
            NxI32 lineno;
            const char *error = mFastXml->getError(lineno);
            printf("Error: %s at line %d\r\n", error, lineno );
          }
	  }
	  return ret;
  }

  virtual bool processElement(const char *elementName,         // name of the element
                              NxI32         argc,                // number of attributes
                              const char **argv,               // list of attributes.
                              const char  *elementData,        // element data, null if none
                              NxI32         lineno)         // line number in the source XML file
  {
    bool ret = true;

	lineno;
	elementData;

    if ( elementData )
    {

      NxI32 len = 0;
      NxI32 blobId = 0;
      const char *blobName=0;
  	  NxI32 acount = argc/2;
      for (NxI32 i=0; i<acount; i++)
      {
          const char * atr   = argv[i*2];
          const char * value = argv[i*2+1];
          if ( strcmp(atr,"blob") == 0 )
          {
              blobName = value;
          }
          else if ( strcmp(atr,"blobId") == 0 )
          {
              blobId = atoi(value);
          }
          else if ( strcmp(atr,"len") == 0 )
          {
              len = atoi(value);
          }
      }
      {
        if ( strcmp(elementName,"telnetBlob") == 0 )
        {
            Blob *check = locateBlob(blobId,mClient);
            assert(check==0);
			assert(blobName);
            //
            assert(len > 0 );
            if ( len > 0 && check == 0 && blobName )
            {
              Blob *b = MEMALLOC_NEW(Blob)(blobName,mClient,blobId,len,elementData);
              mBlobs.push_back(b);
            }
        }
        else if ( strcmp(elementName,"telnetBlobData") == 0 )
        {
          Blob *b = locateBlob(blobId,mClient);
          if ( b )
          {
            b->addData(elementData);
          }
        }
        else if ( strcmp(elementName,"telnetBlobEnd") == 0 )
        {
          Blob *b = locateBlob(blobId,mClient);
          if ( b )
          {
            b->addDataEnd(elementData);
          }
        }
      }
    }

    return ret;
  }

  Blob * locateBlob(NxU32 id,NxU32 client) const
  {
    Blob *ret = 0;
    if ( id != 0 )
    {
      BlobList::const_iterator i;
      for (i=mBlobs.begin(); i!=mBlobs.end(); i++)
      {
        Blob *b = (*i);
        if ( b->getId() == id && b->mClient == client )
        {
            ret = b;
            break;
        }
	  }
    }
    return ret;
  }

private:
  NxU32    mClient;
  NxU32    mBlobId;
  BlobIOInterface *mCallback;
  FastXml         *mFastXml;
  Blob            *mLastBlob;
  BlobList         mBlobs;
};

}; // end of BLOB_IO namespace

using namespace BLOB_IO;

BlobIO * createBlobIO(BlobIOInterface *iface)
{
    MyBlobIO * m = MEMALLOC_NEW(MyBlobIO)(iface);
    return static_cast< BlobIO *>(m);
}

void     releaseBlobIO(BlobIO *b)
{
    MyBlobIO *m = static_cast< MyBlobIO *>(b);
    delete m;
}
