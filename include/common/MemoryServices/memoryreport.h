#ifndef MEMORY_REPORT_H

#define MEMORY_REPORT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef WIN32
#include <windows.h>
#endif


#include "../snippets/SendTextMessage.h"
#include "../snippets/UserMemAlloc.h"
#include "../snippets/htmltable.h"
#include "memalloc.h"

class MemoryServices
{
public:

  virtual unsigned int getMemoryUsed(unsigned int &unused)
  {
    return MEMALLOC::getMemoryUsed(MEMALLOC::gMemAlloc,unused);
  }

  virtual void memorySummaryReport(HTML_TABLE::HtmlTable *table,const char *header,MemoryServices * /*ms*/)
  {
    MEMALLOC::summaryReport(table,0,header);
  }

  virtual void reportByClass(const char *header,HTML_TABLE::HtmlDocument *document)
  {
    MEMALLOC::reportByClass(header,document);
  }

  virtual void reportBySourceFile(const char *header,HTML_TABLE::HtmlDocument *document)
  {
    MEMALLOC::reportBySourceFile(header,document);
  }

  virtual void processReport(const char *header,HTML_TABLE::HtmlDocument *document)
  {
    MEMALLOC::processReport(header,document);
  }

  virtual void fixedPoolReport(const char *header,HTML_TABLE::HtmlDocument *document)
  {
    MEMALLOC::fixedPoolReport(header,document);
  }

  virtual void setMaxFixedMemorySize(HeU32 fsize)
  {
    MEMALLOC::setMaxFixedMemorySize(fsize);
  }

  virtual void  getMemoryStatistics(HeU32 &mcount,            // number of non-fixed memory allocation.
    HeU32 &msize,             // size of non-fixed memory allocations
    HeU32 &fmcount,           // fixed memory allocation count.
    HeU32 &fmsize)           // fixed memory size
  {
    MEMALLOC::getMemoryStatistics(mcount,msize,fmcount,fmsize);
  }

  virtual void reportResources(const char * /*header*/,HTML_TABLE::HtmlDocument * /*document*/)
  {
    // optional report of resource utilization.
  }


};



class MemoryReport
{
public:

  MemoryReport(void)
  {
    mInterface = HTML_TABLE::getHtmlTableInterface();
    mDocument = mInterface->createHtmlDocument("HeroWorld Summary Memory Report of various Plugin Components");
    mTable = 0;
  }

  ~MemoryReport(void)
  {
    mInterface->releaseHtmlDocument(mDocument);
  }

#if HE_USE_MEMORY_TRACKING
  void beginSummaryReport(void)
  {
    mTable = MEMALLOC::summaryHeader(mDocument);
  }

  void summaryReport(const char *header,MemoryServices *ms=0)
  {
    if ( !mTable )
    {
      beginSummaryReport();
    }
    if ( ms )
      ms->memorySummaryReport(mTable,header,0);
    else
      MEMALLOC::summaryReport(mTable,0,header);
  }
#endif /* HE_USE_MEMORY_TRACKING */

  bool isLineFeed(char c) const
  {
    bool ret = false;
    if ( c == 10 || c == 13 )
      ret = true;
    return ret;
  }

  const char * skipLineFeed(const char *scan) const
  {
    while ( *scan && isLineFeed(*scan) ) scan++;
    return scan;
  }

  void echoText(SendTextMessage *stm,HTML_TABLE::HtmlSaveType type)
  {
    size_t len;
    const char *data = mDocument->saveDocument(len,type);
    const char *mem = data;
    if ( mem )
    {
      while ( *mem )
      {
        mem = skipLineFeed(mem);
        char scratch[8191];
        char *dest = scratch;
        while ( *mem && !isLineFeed(*mem) )
        {
          *dest++ = *mem++;
        }
        *dest = 0;
        stm->sendTextMessage(0,"%s\r\n", scratch );
      }
      mDocument->releaseDocumentMemory(data);
    }
  }

  bool saveExcel(const char *fname)
  {
    return mDocument->saveExcel(fname);
  }

  const char * getDocument(size_t & len, HTML_TABLE::HtmlSaveType type)
  {
    return mDocument->saveDocument(len,type);
  }

  void releaseDocumentMemory(const char * data)
  {
    mDocument->releaseDocumentMemory(data);
  }

  void saveFile(const char *fname,HTML_TABLE::HtmlSaveType type)
  {
    size_t len;
    const char *data = mDocument->saveDocument(len,type);
    const char *mem = data;
    if ( mem )
    {
      FILE *fph = fopen(fname,"wb");
      if ( fph )
      {
        fwrite(mem,len,1,fph);
        fclose(fph);
      }
      mDocument->releaseDocumentMemory(data);
    }
  }

#if HE_USE_MEMORY_TRACKING
  void processReport(const char *header,MemoryServices *ms=0)
  {
    if ( ms )
      ms->processReport(header,mDocument);
    else
      MEMALLOC::processReport(header,mDocument);
  }

  void fixedPoolReport(const char *header,MemoryServices *ms=0)
  {
    if ( ms )
      ms->fixedPoolReport(header,mDocument);
    else
      MEMALLOC::fixedPoolReport(header,mDocument);
  }

  void reportByClass(const char *header,MemoryServices *ms=0)
  {
    if ( ms )
      ms->reportByClass(header,mDocument);
    else
      MEMALLOC::reportByClass(header,mDocument);
  }

  void reportBySourceFile(const char *header,MemoryServices *ms=0)
  {
    if ( ms )
      ms->reportBySourceFile(header,mDocument);
    else
      MEMALLOC::reportBySourceFile(header,mDocument);
  }

#endif /* HE_USE_MEMORY_TRACKING */

  HTML_TABLE::HtmlTableInterface * getInterface(void) const { return mInterface; };
  HTML_TABLE::HtmlTable         *  getTable(void) const { return mTable; };
  HTML_TABLE::HtmlDocument      *  getDocument(void) const { return mDocument; };

private:
  HTML_TABLE::HtmlTableInterface *mInterface;
  HTML_TABLE::HtmlTable         *mTable;
  HTML_TABLE::HtmlDocument      *mDocument;
};

#endif
