/*!  

** Written by John W. Ratcliff : jratcliffscarab@gmail.com
**
** BITCOIN TIP JAR: "1BT66EoaGySkbY9J6MugvQRhMMXDwPxPya"
**
** The MIT license:
**
** Permission is hereby granted, free of charge, to any person obtaining a copy 
** of this software and associated documentation files (the "Software"), to deal 
** in the Software without restriction, including without limitation the rights 
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
** copies of the Software, and to permit persons to whom the Software is furnished 
** to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all 
** copies or substantial portions of the Software.

** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
** WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/


#ifndef MEM_TRACKER_H

#define MEM_TRACKER_H

#include <stdint.h>

#define MEM_TRACKER_VERSION 100 // version number of the MemTracker API

namespace MEM_TRACKER
{

/**
\brief The layout format for the memory report.
*/
enum MemoryReportFormat
{
	MRF_SIMPLE_HTML,       // just a very simple HTML document containing the tables.
	MRF_CSV,               // Saves the Tables out as comma seperated value text
	MRF_TEXT,              // Saves the tables out in human readable text format.
	MRF_TEXT_EXTENDED,     // Saves the tables out in human readable text format, but uses the MS-DOS style extended ASCII character set for the borders.
};

/**
\brief This enumeration indicates the type of memory allocation that is being performed.
*/
enum MemoryType
{
	MT_NEW,						// Captured new operator
	MT_NEW_ARRAY,				// Captured new array operator
	MT_MALLOC,					// Standard heap allocation
	MT_FREE,					// Standard heap free
	MT_DELETE,					// Captured delete operator
	MT_DELETE_ARRAY,			// Captured array delete
	MT_GLOBAL_NEW,				// Allocation via Global new
	MT_GLOBAL_NEW_ARRAY,		// Allocation via global new array
	MT_GLOBAL_DELETE,			// Deallocation via global delete
	MT_GLOBAL_DELETE_ARRAY,		// Deallocation via global delete array
};

/**
\brief This data structure is used to return the current state of a particular block of allocated memory.
*/
struct TrackInfo
{
	const void		*mMemory;		// Address of memory
	MemoryType		 mType;			// Type of allocation
	size_t			mSize;			// Size of the memory allocation
	const char		*mContext;		// The context of the memory allocation.
	const char		*mClassName;	// The class or type name of this allocation.
	const char		*mFileName;		// Source code file name where this allocation occurred
	uint32_t		mLineNo;		// Source code line number where this allocation occurred
	size_t			mAllocCount; 	// Indicates which time this allocation occurred at this particular source file and line number.
};


class MemTracker
{
public:

  virtual void trackAlloc(size_t threadId,
                          void *mem,
                          size_t size,
                          MemoryType type,
                          const char *context,
                          const char *className,
                          const char *fileName,
                          uint32_t lineno) = 0;

  virtual void trackRealloc(size_t threadId,
	                          void *oldMem,
 	                          void *newMem,
	                          size_t newSize,
	                          const char *context,
	                          const char *className,
	                          const char *fileName,
	                          uint32_t lineno) = 0;

  virtual void trackFree(size_t threadId,
                        void *mem,
                        MemoryType type,
                        const char *context,
                        const char *fileName,uint32_t lineno) = 0;

  virtual const char * trackValidateFree(size_t threadId,
	  void *mem,
	  MemoryType type,
	  const char *context,
	  const char *fileName,uint32_t lineno) = 0;


   virtual void trackFrame(void) = 0;


   virtual bool trackInfo(const void *mem,TrackInfo &info)  = 0;


   virtual void *generateReport(MemoryReportFormat format,const char *fname,uint32_t &saveLen,bool reportAllLeaks) = 0;
   virtual void releaseReportMemory(void *mem) = 0;

    virtual void usage(void) = 0;
    virtual size_t detectLeaks(size_t &acount) = 0;
    virtual void setLogLevel(bool logEveryAllocation,bool logEveyFrame,bool verifySingleThreaded) = 0;

	virtual void release(void) = 0;

};


}; // end of MEM_TRACKER namespace

#endif
