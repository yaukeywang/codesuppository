#ifndef MEMORY_SERVICES_H

#define MEMORY_SERVICES_H

/************************************************************************/
/* WARNING!                                                             */
/* Changing this interface class means that you MUST bump the version   */
/* number below, this will then cause the version number of every plugin*/
/* which inherits the MemoryServices interface to be reflected as well. */
/************************************************************************/

#include "../snippets/SendTextMessage.h"
#include "../snippets/UserMemAlloc.h"

#pragma warning(push)
#pragma warning(disable:4100)

namespace HTML_TABLE
{
  class HtmlDocument;
};

class MemoryServices : public SendTextMessage
{
public:
  virtual bool         shutdown(void) { return false; };
  virtual const char * isBadState(HeU32 &lineNo) const { lineNo = 0; return 0; };
  virtual void         setSendTextMessage(SendTextMessage *stm)
  {
    gSendTextMessage = stm;
  }

  virtual HeU32 executeCommand(SendTextMessage * /* tmessage */,const char * /*cmd*/)
  {
    return 0;
  }

  virtual HeU32 getHeapSize(HeU32 &unused)
  {
    return MEMALLOC_GET_HEAP_SIZE(unused);
  }

  virtual void heapWalk(void)
  {
    MEMALLOC_HEAP_CHECK();
  }

  virtual void memoryDump(SendTextMessage *stm,const char *header,bool csv)
  {
    MEMALLOC_CORE_DUMP(stm,header,csv);
  }

  virtual const char * getLogMessage(void) { return 0; }; // returns a single line log message for the application.

  virtual HeI32          getLogValue(const char * /* key */)
  {
    return 0;
  }; // return a value item based on this key-word.

  virtual void frameBegin(SendTextMessage * stm)
  {
    MEMALLOC_FRAME_BEGIN(stm);
  }

  virtual void frameEnd(SendTextMessage * stm,const char *header)
  {
    MEMALLOC_FRAME_END(stm,header);
  }

  virtual void setMemoryShutdown(MemoryShutdown *down)
  {
    MEMALLOC_SET_MEMORY_SHUTDOWN(down);
  }

#if HE_USE_MEMORY_TRACKING
  virtual void memorySummaryReport(HTML_TABLE::HtmlTable *table,const char *header)
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
#endif /* HE_USE_MEMORY_TRACKING */

  virtual void reportResources(const char * /*header*/,HTML_TABLE::HtmlDocument * /*document*/)
  {
    // optional report of resource utilization.
  }


};

#pragma warning(pop)

#define MEMORY_SERVICES_VERSION (17<<8)

#endif
