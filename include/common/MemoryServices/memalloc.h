#ifndef MEMALLOC_H

#define MEMALLOC_H

//-ignore_file      Do not allow SCANCPP to process this file!

#include <new.h>
#include <malloc.h>

#pragma warning(disable:4702)

#include "../snippets/HeSimpleTypes.h"

class SendTextMessage;

namespace HTML_TABLE
{
  class HtmlDocument;
  class HtmlTable;
  class HtmlTableInterface;
}; // end of namespace

class MemoryShutdown
{
public:

  virtual void memoryAllocationFailure(const char *type,const char *file,int lineno,unsigned int size) = 0;

};

namespace MEMALLOC
{

enum MemAllocType
{
  MAT_MALLOC,
  MAT_NEW,
  MAT_NEW_ARRAY,
  MAT_INTERNAL    // used internally only so the fixed size allocator can recursively call back for new chunk memory as needed!
};


class MemAlloc;

extern MemAlloc *gMemAlloc;

void          setName(MemAlloc *m,const char *name);
void *        malloc(MemAlloc *m,size_t size,size_t align,const char *type,const char *file,int lineno,MemAllocType mtype);
void          free(MemAlloc *m,void *mem,MemAllocType mtype);
void *        realloc(MemAlloc *m,void *mem,size_t size,const char *file,int lineno);
void          walkHeap(MemAlloc *m,const char *label);
void          heapCheck(MemAlloc *m);
int           report(MemAlloc *m,const char *heading,SendTextMessage *tmessage,bool checkLeak);

void          getMemStat(MemAlloc *m,int &mc,int &ms);
unsigned int  getMemoryUsed(MemAlloc *m,unsigned int &unused);
void          setSendTextMessage(MemAlloc *m,SendTextMessage *stm);
void          frameBegin(MemAlloc *m,SendTextMessage *tmessage);
void          frameEnd(MemAlloc *m,SendTextMessage *tmessage,const char *header);
void          setMemoryShutdown(MemoryShutdown *shutdown);
void          coreDump(MemAlloc *m,SendTextMessage *tmessage,const char *header,bool csv); // perform a core dump of all memory addresses.
void          processCoreDump(MemAlloc *m,SendTextMessage *tmessage,const char *header,bool csv); // perform a core dump of all memory addresses.
void          heapCompact(MemAlloc *m,SendTextMessage *tmessage);
unsigned int  getProcessMemory(unsigned int &workingset); // reports the amount of memory used by the current process

HTML_TABLE::HtmlTable    * summaryHeader(HTML_TABLE::HtmlDocument *document); // create a new table for the summary data.
void                       summaryReport(HTML_TABLE::HtmlTable *table,MEMALLOC::MemAlloc *m,const char *header); // add a new row to the summary table

void                       processReport(const char *header,HTML_TABLE::HtmlDocument *document);

void                       fixedPoolReport(const char *header,HTML_TABLE::HtmlDocument *document);
void                       reportByClass(const char *header,HTML_TABLE::HtmlDocument *document);
void                       reportBySourceFile(const char *header,HTML_TABLE::HtmlDocument *document);

void                       setMaxFixedMemorySize(HeU32 fsize);

void                       getMemoryStatistics(HeU32 &mcount,            // number of non-fixed memory allocation.
                                               HeU32 &msize,             // size of non-fixed memory allocations
                                               HeU32 &fmcount,           // fixed memory allocation count.
                                               HeU32 &fmsize);           // fixed memory size

void                       setComponentName(const char *name);



}; // end namespace

#endif
