#ifndef RAT_TRACK_H

#define RAT_TRACK_H

#ifdef WIN32
#define USE_RAT_TRACK 1
#else
#define USE_RAT_TRACK 0
#endif

#if USE_RAT_TRACK

enum RatTrackFormat
{
    RTF_SIMPLE_TEXT,
    RTF_BORDER_TEXT,
    RTF_HTML,
    RTF_CVS
};

class RatTrack
{
  virtual void logMalloc( void* pMemory, size_t Size,const char *context,const char *type,const char *fileName,int lineno) = 0;
  virtual void logRealloc( void* pNewMem, void* pOldMem, size_t NewSize,const char *fileName,int lineno) = 0;
  virtual void logFree( void* pMemory,const char *fileName,int lineno ) = 0;
  virtual void logNew( void* pMemory, size_t Sizeconst char *context,const char *type,const char *fileName,int lineno) = 0;
  virtual void logDelete( void* pMemory ) = 0;
  virtual void logNewArray( void* pMemory, size_t Size, const wchar_t* pFileName, int Line ) = 0;
  virtual void logDeleteArray( void* pMemory,const char *fileName,int lineno ) = 0;

  virtual void generateReport(const char *context,const char *fname,RatTrackFormat format) = 0;
};


extern RatTrack *gRatTrack;

void initRatTrack(void); // initializes the tracking system.

#define RAT_LOG_MALLOC(mem,size,context,type,fileName,lineno) { initRatTrack(); if ( gRatTrack ) gRatTrack->logMalloc(mem,size,contet,type,filenName,lineno); }

#else

#define RAT_LOG_MALLOC(mem,size,context,type,fileName,lineno)

#endif

#endif
