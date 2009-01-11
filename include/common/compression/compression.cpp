#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../../common/snippets/UserMemAlloc.h"
#include "compression.h"
#include "../FileInterface/FileInterface.h"


#if USE_MINI_LZO
#include "minilzo.h"
#endif

#include "zlib.h"

#if USE_BZIP
#include "bzlib.h"
#endif

#if USE_LZMA
#include "LzmaDecode.h"
#endif

#pragma warning(disable:4996)

namespace COMPRESSION
{


void deleteData(const void* mem)
{
  MEMALLOC_FREE((void*)mem);
}

struct CompressionHeader
{
  HeI32             mRawLength;
  HeI32             mCompressedLength;
  HeI32             mVersionNumber;
  char              mId[4];
};


void * compressMiniLZO(const void * /*source*/,HeI32 /*len*/,HeI32 &outlen,CompressionLevel /*level*/,HeI32 /*versionNumber*/)
{
#if USE_MINI_LZO
  const HeI32 SCRATCHPAD=65536;
  char wrkmem[LZO1X_1_MEM_COMPRESS];

  CompressionHeader *h = (CompressionHeader *) MEMALLOC_MALLOC(len+SCRATCHPAD+sizeof(CompressionHeader));
  HeU8 *dest = (HeU8 *)h;
  dest+=sizeof(CompressionHeader);
  lzo_init();
  HeI32 r = lzo1x_1_compress((const HeU8 *)source,len,dest,(HeU32 *)&outlen,wrkmem);

  if ( r == LZO_E_OK )
  {
    h->mRawLength        = len;
    h->mVersionNumber    = versionNumber;
    outlen+=sizeof(CompressionHeader);
    h->mCompressedLength = outlen;
    h->mId[0]            = 'm';
    h->mId[1]            = 'l';
    h->mId[2]            = 'z';
    h->mId[3]            = 'o';

  }
  else
  {
    MEMALLOC_FREE(h);
    h = 0;
  }


  return h;
#else

  outlen = 0;
  return 0;

#endif

}


void * compressZLIB(const void *source,HeI32 len,HeI32 &outlen,CompressionLevel /*level*/,HeI32 versionNumber)
{

  uLong csize = compressBound(len);

  CompressionHeader *h = (CompressionHeader *) MEMALLOC_MALLOC(csize+sizeof(CompressionHeader));
  HeU8 *dest = (HeU8 *)h;
  dest+=sizeof(CompressionHeader);


  HeI32 err = compress2((Bytef *)dest,&csize,(Bytef *)source,len, Z_BEST_SPEED);

  if ( err == Z_OK )
  {
    outlen = csize;

    h->mRawLength        = len;
    h->mVersionNumber    = versionNumber;
    outlen+=sizeof(CompressionHeader);
    h->mCompressedLength = outlen;
    h->mId[0]            = 'z';
    h->mId[1]            = 'l';
    h->mId[2]            = 'i';
    h->mId[3]            = 'b';
  }
  else
  {
    outlen = 0;
    MEMALLOC_FREE(h);
    h = 0;
  }

  return h;
}

#if USE_BZIP
void * compressBZIP(const void *source,HeI32 len,HeI32 &outlen,CompressionLevel /*level*/,HeI32 versionNumber)
{

  HeU32 csize = len+65536;

  CompressionHeader *h = (CompressionHeader *) MEMALLOC_MALLOC(csize+sizeof(CompressionHeader));
  HeU8 *dest = (HeU8 *)h;
  dest+=sizeof(CompressionHeader);


  HeI32 err = BZ2_bzBuffToBuffCompress((char *)dest,&csize,(char *)source,(HeU32)len,1,0,30);

  if ( err == 0 )
  {
    outlen = csize;

    h->mRawLength        = len;
    h->mVersionNumber    = versionNumber;
    outlen+=sizeof(CompressionHeader);
    h->mCompressedLength = outlen;
    h->mId[0]            = 'b';
    h->mId[1]            = 'z';
    h->mId[2]            = 'i';
    h->mId[3]            = 'p';
  }
  else
  {
    outlen = 0;
    MEMALLOC_FREE(h);
    h = 0;
  }

  return h;
}
#endif

#if USE_LZMA
void * compressLZMA(const void * /*source*/,HeI32 len,HeI32 &outlen,CompressionLevel /*level*/,HeI32 versionNumber)
{

  uLong csize = compressBound(len);

  CompressionHeader *h = (CompressionHeader *) MEMALLOC_MALLOC(csize+sizeof(CompressionHeader));
  HeU8 *dest = (HeU8 *)h;
  dest+=sizeof(CompressionHeader);


  HeI32 err = -1;
  //  int err = compress2((Bytef *)dest,&csize,(Bytef *)source,len, Z_BEST_SPEED);

  if ( err == 0 )
  {
    outlen = csize;

    h->mRawLength        = len;
    h->mVersionNumber    = versionNumber;
    outlen+=sizeof(CompressionHeader);
    h->mCompressedLength = outlen;
    h->mId[0]            = 'l';
    h->mId[1]            = 'z';
    h->mId[2]            = 'm';
    h->mId[3]            = 'a';
  }
  else
  {
    outlen = 0;
    MEMALLOC_FREE(h);
    h = 0;
  }

  return h;
}
#endif

void * compressGZIP(const void * source,HeI32 len,HeI32 &outlen,CompressionLevel /*level*/,HeI32 /*versionNumber*/)
{
  void *ret = 0;

  outlen = 0;

  if ( source && len > 0 )
  {
    gzFile f = gzopen("foo", "wmem", 0, 0 );
    if ( f )
    {
      gzwrite(f,source,len);
      gzflush(f,Z_FINISH);
      unsigned int olen;
      void *outmem = gzclose(f,&olen);
      if( outmem )
      {
        ret = MEMALLOC_MALLOC(olen);
        memcpy(ret,outmem,olen);
        outlen = olen;
        gzfree(outmem);
      }
    }
  }

  return ret;
}



void * compressData(const void *source,HeI32 len,HeI32 &outlen,CompressionType type,HeI32 versionNumber,CompressionLevel level)
{
  void *ret = 0;

  switch ( type )
  {
    case CT_MINILZO:
      ret = compressMiniLZO(source,len,outlen,level,versionNumber);
      break;
    case CT_ZLIB:
      ret = compressZLIB(source,len,outlen,level,versionNumber);
      break;
    case CT_BZIP:
#if USE_BZIP
      ret = compressBZIP(source,len,outlen,level,versionNumber);
#endif
      break;
    case CT_LZMA:
#if USE_LZMA
      ret = compressLZMA(source,len,outlen,level,versionNumber);
#endif
      break;
    case CT_GZIP:
      ret = compressGZIP(source,len,outlen,level,versionNumber);
      break;
  }
  return ret;
}



void * decompressMiniLZO(const void * /*source*/,HeI32 /*clen*/,HeI32 &outlen)
{
#if USE_MINI_LZO

  void * ret = 0;

  CompressionHeader *h = (CompressionHeader *) source;

  {
    const char *data = (const char *) h;
    data+=sizeof(CompressionHeader);
    HeU32 slen = clen-sizeof(CompressionHeader);
    {
      outlen = h->mRawLength;
      char *dest = (char *)MEMALLOC_MALLOC(h->mRawLength);
      lzo_init();

      HeI32 r = lzo1x_decompress((const HeU8 *)data,
                               slen,
                               (HeU8 *)dest,
                               (HeU32 *)&outlen,0);

      if ( r == LZO_E_OK )
      {
        ret = dest;
        HE_ASSERT( outlen == h->mRawLength );
        outlen = h->mRawLength;
      }
      else
      {
        MEMALLOC_FREE(dest);
        ret = 0;
      }
    }
  }

  return ret;
#else
  outlen = 0;
  return 0;
#endif
}

void * decompressZLIB(const void *source,HeI32 clen,HeI32 &outlen)
{
  void * ret = 0;

  CompressionHeader *h = (CompressionHeader *) source;

  {

    const char *data = (const char *) h;
    data+=sizeof(CompressionHeader);
    HeU32 slen = clen-sizeof(CompressionHeader);
    {

      outlen = h->mRawLength;
      char *dest = (char *)MEMALLOC_MALLOC(h->mRawLength);

      HeI32 err;

      uLongf destLen = outlen;

      err = uncompress( (Bytef *) dest,&destLen,(Bytef *) data, slen );

      HE_ASSERT( destLen == (uLongf)outlen );
      if ( destLen != (uLongf) outlen )
        err = -1;

      if ( err == Z_OK )
      {
        ret = dest;
      }
      else
      {
        MEMALLOC_FREE(dest);
        outlen = 0;
        ret = 0;
      }
    }
  }

  return ret;
}


#if USE_BZIP
void * decompressBZIP(const void *source,HeI32 clen,HeI32 &outlen)
{
  void * ret = 0;

  CompressionHeader *h = (CompressionHeader *) source;

  {

    const char *data = (const char *) h;
    data+=sizeof(CompressionHeader);
    HeU32 slen = clen-sizeof(CompressionHeader);
    {

      outlen = h->mRawLength;
      char *dest = (char *)MEMALLOC_MALLOC(h->mRawLength);

      HeI32 err;

      HeU32 destLen = outlen;

      err = BZ2_bzBuffToBuffDecompress( (char *)dest, &destLen, (char *)data, slen, 0, 0);

      HE_ASSERT( destLen == (uLongf)outlen );
      if ( destLen != (uLongf)outlen )
        err = -1;

      if ( err == 0 )
      {
        ret = dest;
      }
      else
      {
        MEMALLOC_FREE(dest);
        outlen = 0;
        ret = 0;
      }
    }
  }

  return ret;
}
#endif

#if USE_LZMA
void * decompressLZMA(const void *source,HeI32 clen,HeI32 &outlen)
{
  void * ret = 0;

  CompressionHeader *h = (CompressionHeader *) source;

  {

    const char *data = (const char *) h;
    data+=sizeof(CompressionHeader);
    HeU32 slen = clen-sizeof(CompressionHeader);
    {

      outlen = h->mRawLength;
      char *dest = (char *)MEMALLOC_MALLOC(h->mRawLength);

      HeI32 err = -1;

      CLzmaDecoderState state;

      SizeT inSizeProcessed;
      SizeT outSizeProcessed;

      LzmaDecode(&state, (const HeU8 *)data, slen, &inSizeProcessed, (HeU8 *)dest, outlen, &outSizeProcessed );
#if 0
      uLongf destLen = outlen;
      err = uncompress( (Bytef *) dest,&destLen,(Bytef *) data, slen );

      HE_ASSERT( destLen == outlen );
      if ( destLen != outlen )
        err = -1;
#endif

      if ( err == 0 )
      {
        ret = dest;
      }
      else
      {
        MEMALLOC_FREE(dest);
        outlen = 0;
        ret = 0;
      }
    }
  }

  return ret;
}
#endif


void * decompressData(const void *source,HeI32 clen,HeI32 &outlen)
{
  void * ret = 0;

  outlen = 0;
  HeI32 versionNumber;
  switch ( getCompressionType(source,clen,versionNumber) )
  {
    case CT_GZIP:

      {
        gzFile f = gzopen("foo","rb",(void *)source,clen);
        if ( f )
        {
          FILE_INTERFACE *fpout = fi_fopen("output", "wmem", 0, 0);
          if ( fpout )
          {
            bool ok = true;
            char scratch[4096];// read in 4k chunks
            while ( ok )
            {
              int v = gzread(f,scratch,4096);
              if ( v > 0 )
              {
                fi_fwrite(scratch,v,1,fpout);
              }
              else
              {
                ok = false;
              }
            }
            size_t olen;
            void *mem = fi_getMemBuffer(fpout,&olen);
            if ( mem )
            {
              outlen = (HeI32)olen;
              ret = MEMALLOC_MALLOC(outlen);
              memcpy(ret,mem,outlen);
            }
            fi_fclose(fpout);
          }
          unsigned int olen;
          gzclose(f,&olen);
        }

#if 0
        if ( ret )
        {
          static int gno = 0;
          gno++;
          char scratch[512];
          sprintf(scratch,"compress%04d.bin",gno);
          FILE *fph = fopen(scratch,"wb");
          if ( fph )
          {
            fwrite(ret,outlen,1,fph);
            fclose(fph);
          }
        }
#endif
      }

      break;
    case CT_MINILZO:
      ret = decompressMiniLZO(source,clen,outlen);
      break;
    case CT_ZLIB:
      ret = decompressZLIB(source,clen,outlen);
      break;
    case CT_BZIP:
#if USE_BZIP
      ret = decompressBZIP(source,clen,outlen);
#endif
      break;
    case CT_LZMA:
#if USE_LZMA
      ret = decompressLZMA(source,clen,outlen);
#endif
      break;
  }

  return ret;
}



CompressionType getCompressionType(const void *mem,HeI32 len,HeI32 &versionNumber)
{
  CompressionType ret = CT_INVALID;

  versionNumber = 0;

  const HeU8 *scan = (const HeU8 *) mem;
  if ( len >= 2 && (scan[0] == 0x1F) && (scan[1] == 0x8B) ) // has the 'magic' bytes that indicate this is in GZIP format!
  {
    ret = CT_GZIP;
  }
  else
  {
    if ( mem && len > sizeof(CompressionHeader) )
    {
      CompressionHeader *h = (CompressionHeader *) mem;

      if ( h->mCompressedLength == len )
      {
        if ( h->mId[0] == 'M' && h->mId[1] == 'L' && h->mId[2] == 'Z' && h->mId[3] == 'O' )
        {
          ret = CT_MINILZO;
        }
        else if ( h->mId[0] == 'Z' && h->mId[1] == 'L' && h->mId[2] == 'I' && h->mId[3] == 'B' )
        {
          ret = CT_ZLIB;
        }
        else if ( h->mId[0] == 'B' && h->mId[1] == 'Z' && h->mId[2] == 'I' && h->mId[3] == 'P' )
        {
          ret = CT_BZIP;
        }
        else if ( h->mId[0] == 'L' && h->mId[1] == 'Z' && h->mId[2] == 'M' && h->mId[3] == 'A' )
        {
          ret = CT_LZMA;
        }
        else if ( h->mId[0] == 'm' && h->mId[1] == 'l' && h->mId[2] == 'z' && h->mId[3] == 'o' )
        {
          ret = CT_MINILZO;
          versionNumber = h->mVersionNumber;
        }
        else if ( h->mId[0] == 'z' && h->mId[1] == 'l' && h->mId[2] == 'i' && h->mId[3] == 'b' )
        {
          ret = CT_ZLIB;
          versionNumber = h->mVersionNumber;
        }
        else if ( h->mId[0] == 'b' && h->mId[1] == 'z' && h->mId[2] == 'i' && h->mId[3] == 'p' )
        {
          ret = CT_BZIP;
          versionNumber = h->mVersionNumber;
        }
        else if ( h->mId[0] == 'l' && h->mId[1] == 'z' && h->mId[2] == 'm' && h->mId[3] == 'a' )
        {
          ret = CT_LZMA;
          versionNumber = h->mVersionNumber;
        }
      }
    }
  }

  return ret;
}


}; // end of namespace
