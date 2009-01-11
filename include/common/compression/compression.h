#ifndef COMPRESSION_H

#define COMPRESSION_H


#define UZE_MINI_LZO 0 // Since MINILZO is GPL, this implementation is disabled by default.
#define USE_BZIP     0
#define USE_LZMA     0

#include "../snippets/HeSimpleTypes.h"

namespace COMPRESSION
{

enum CompressionType
{
  CT_INVALID,
  CT_MINILZO,           // The MiniLZO library
  CT_ZLIB,              // The ZLIB library
  CT_BZIP,              // The BZIP library
  CT_LZMA,              // The LZMA library (of 7zip)
  CT_GZIP,              // GZIP format header on top of ZLIB
  CT_DEFAULT_COMPRESSION = CT_GZIP, // by default use native binary GZIP format.
};

enum CompressionLevel
{
  CL_DEFAULT_COMPRESSION,
  CL_BEST_SPEED,
  CL_BEST_COMPRESSION
};

void *           compressData(const void *source,
                              HeI32 len,
                              HeI32 &outlen,
                              CompressionType type,
                              HeI32 versionNumber,
                              CompressionLevel level=CL_BEST_SPEED);

void *           decompressData(const void *source,HeI32 clen,HeI32 &outlen);
void             deleteData(const void* mem);

CompressionType  getCompressionType(const void *mem,HeI32 len,HeI32 &versionNumber);

};


#endif
