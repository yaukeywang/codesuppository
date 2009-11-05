#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ImportHeightMap.h"
#include "UserMemAlloc.h"

using namespace NVSHARE;

namespace IMPORT_HEIGHTMAP
{


/* The version of this library */
#define LEVELLERLIB_VER		2

#define LOWEST_LEVFILE_VER		4
#define HIGHEST_LEVFILE_VER		7

/*
	If ints are not 32 bits long on your
	system, change the following typedef.
*/
typedef NxI32				INT32;
typedef NxU32	UINT32;


/* Basic Leveller types */
typedef INT32	FIX32;			/* One of Leveller's internal elevation storage formats */
typedef UINT32	LEV_POS;		/* A heightfield location coordinate member */
typedef UINT32	LEV_SIZE;		/* A heightfield dimension member */
typedef NxF32	ELEV;			/* Ideal elevation type */


/* Heightfield object */
typedef struct
{
	LEV_SIZE		width, breadth;
	ELEV*			pElevs;
} LEV_HF;


/* Handy converters between fixed and floating-point types */
#define FixedToFloat(hv)	((NxF32)(hv)/65536)
#define FloatToFixed(f)	((FIX32)((f)*65536))


/* Core Leveller heightfield routines. */
LEV_HF*	LevHF_Create(LEV_SIZE w, LEV_SIZE b);
void	LevHF_Delete(LEV_HF*);
LEV_HF* LevHF_Read(const char* pszFile);
NxI32		LevHF_Write(const LEV_HF*, const char* pszFile, NxI32 ver);
void	LevHF_GetSpan(const LEV_HF*, ELEV*, ELEV*);
void	LevHF_SetSpan(LEV_HF*, ELEV, ELEV);
void	LevHF_Flatten(LEV_HF*, ELEV);
ELEV	LevHF_GetHeight(const LEV_HF*, LEV_POS x, LEV_POS z);
void	LevHF_SetHeight(LEV_HF*, LEV_POS x, LEV_POS z, ELEV);

/*
	Leveller heightfield (digital elevation model)
	public library routines for ANSI C and EiC.
	Courtesy of Daylon Graphics Ltd.

	Please use a tab width of 4 to view this file.

	Copyright 2000-2005 Daylon Graphics Ltd.
	Permission is granted for MEMALLOC_FREE unlimited
	use of the source code herein for
	both personal and commercial use,
	as long as the files are distributed
	with this notice intact.

	Tested with MS DevStudio 97 and EiC 4.2.6

	Please report bugs and/or feature
	requests/submissions to support@daylongraphics.com

	rcg		apr 23/00	Created.
	rcg/la	apr 24/00	LevHF_Read() implemented.
	rcg		apr 24/00	Variable-endianness support added.
	rcg		dec 15/05	TER 6 support added (Leveller 2.5).
*/

#pragma warning(disable:4996)
/* #include <unistd.h> */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* Some basic utility macros */

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef SWAP
#define SWAP(a, b, t)	{ (t) = (a); (a) = (b); (b) = (t); }
#endif

#ifndef ABS
#define ABS(a)	((a) < 0 ? -(a) : (a))
#endif

#ifndef min
#define min(a, b)	((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a, b)	((a) > (b) ? (a) : (b))
#endif



/* Comment out the following macro to disable the sample main() function. */
//#define INCLUDE_MAIN

#define ASSERT_HF_OKAY(phf)	{ assert(phf != NULL); assert(phf->pElevs != NULL); }


/*
	LevHF_Create()
	Creates a new heightfield.

	w, b	Width and breadth of heightfield to make, in pixels.

	Returns pointer to heightfield, NULL if error.
*/
LEV_HF*	LevHF_Create(LEV_SIZE w, LEV_SIZE b)
{
	ELEV*	pElevs = NULL;
	LEV_HF*	pHF = NULL;

	pElevs = (ELEV*) MEMALLOC_MALLOC(sizeof(ELEV) * w * b);
	if(pElevs != NULL)
	{
		pHF = (LEV_HF *)MEMALLOC_MALLOC(sizeof(LEV_HF));
		if(pHF == NULL)
			MEMALLOC_FREE(pElevs);
		else
		{
			pHF->width = w;
			pHF->breadth = b;
			pHF->pElevs = pElevs;
		}
	}
	ASSERT_HF_OKAY(pHF);
	return pHF;
}


/*
	LevHF_Delete()
	Deletes (releases resources used by) an existing heightfield.

	pHF		Pointer to heightfield to be deleted.
*/
void LevHF_Delete(LEV_HF* pHF)
{
	ASSERT_HF_OKAY(pHF);

	MEMALLOC_FREE(pHF->pElevs);
	MEMALLOC_FREE(pHF);
}



/*
	LevHF_GetSpan()
	Gets a heightfield's current elevation span (i.e., min/max elevations).

	pHF		Pointer to heightfield to be converted.
	pLo		Pointer to returned low value.
	pHi		Pointer to returned high value.
*/
void LevHF_GetSpan(const LEV_HF* pHF, ELEV* pLo, ELEV* pHi)
{
	LEV_POS	x, z;
	ELEV	hv, lo = 1e+10, hi = -lo;

	ASSERT_HF_OKAY(pHF);

	for(z = 0; z < pHF->breadth; z++)
	{
		for(x = 0; x < pHF->width; x++)
		{
			hv = LevHF_GetHeight(pHF, x, z);
			lo = min(lo, hv);
			hi = max(hi, hv);
		}
	}
	*pLo = lo;
	*pHi = hi;
}



/*
	LevHF_SetSpan()
	Spans a heightfield between two values.
	If the heightfield is flat, it will be moved to the span's lower elevation.

	pHF		Pointer to heightfield to be converted.
	h1, h2	The range of elevations to span to. <h1> can be greater than <h2>.
*/
void LevHF_SetSpan(LEV_HF* pHF, ELEV newLo, ELEV newHi)
{
	ELEV hv, prevLo, prevHi, prevSpan, newSpan, spanAdjust;
	LEV_POS	x, z;

	ASSERT_HF_OKAY(pHF);

	if(newLo > newHi)
		SWAP(newLo, newHi, hv);

	newSpan = (newHi - newLo);

	if(newSpan == 0)
	{
		LevHF_Flatten(pHF, newLo);
		return;
	}

	LevHF_GetSpan(pHF, &prevLo, &prevHi);
	prevSpan = prevHi - prevLo;
	if(prevSpan == 0)
	{
		LevHF_Flatten(pHF, newLo);
		return;
	}

	spanAdjust = newSpan / prevSpan;

	for(z = 0; z < pHF->breadth; z++)
	{
		for(x = 0; x < pHF->width; x++)
			LevHF_SetHeight(pHF, x, z, 
				(LevHF_GetHeight(pHF, x, z) - prevLo) * spanAdjust + newLo);
	}
}



/*
	LevHF_Flatten()
	Flattens a heightfield to a specific elevation.

	pHF		Pointer to heightfield to be flattened.
	h		The elevation to flatten to.
*/
void LevHF_Flatten(LEV_HF* pHF, ELEV h)
{
	LEV_POS	x, z;

	ASSERT_HF_OKAY(pHF);

	for(z = 0; z < pHF->breadth; z++)
	{
		for(x = 0; x < pHF->width; x++)
			LevHF_SetHeight(pHF, x, z, h);
	}
}


/*
	LevHF_GetHeight()
	Gets an elevation for a specific pixel on a heightfield.

	pHF		Pointer to heightfield.
	x, z	The pixel to query.
*/
ELEV LevHF_GetHeight(const LEV_HF* pHF, LEV_POS x, LEV_POS z)
{
	ASSERT_HF_OKAY(pHF);
	assert(x < pHF->width);
	assert(z < pHF->breadth);

	return pHF->pElevs[z * pHF->width + x];
}

/*
	LevHF_SetHeight()
	Sets a specific heightfield pixel to a specific elevation.

	pHF		Pointer to heightfield.
	x, z	The pixel to change.
	hv		The elevation to set the pixel to.
*/
void LevHF_SetHeight(LEV_HF* pHF, LEV_POS x, LEV_POS z, ELEV hv)
{
	ASSERT_HF_OKAY(pHF);
	assert(x < pHF->width);
	assert(z < pHF->breadth);

	pHF->pElevs[z * pHF->width + x] = hv;
}


/*
	-- File I/O routines. ---------------------------------------------------
*/


/*
	Platform endian difference handlers.
	Call byteswapper_init() to determine the endianness of the platform, 
	so that subsequent byteswap_nn() calls will work.
	Call byteswap_int16() and byteswap_int32() to correct 
	16/32-bit ints, respectively, prior to reading/writing them to disk.
	If the endianness of the data matches that of the platform, 
	swapping will not be performed.
*/
typedef enum
{
	endian_little, endian_intel = endian_little,
	endian_big, endian_motorola = endian_big,
	endian_undefined
} Endianness;

static Endianness sEndianness = endian_undefined;
static Endianness sEndiannessOfData = endian_undefined;

void byteswapper_init(Endianness endiannessOfData)
{
	char*	psz = "\x00\x01";
	sEndianness = (*((NxI16*)psz) == 0x0001 ? endian_big : endian_little);
	assert(endiannessOfData != endian_undefined);
	sEndiannessOfData = endiannessOfData;
}

void byteswap_int16(void* p)
{
	NxU8 c;
	NxU8* pc;
	assert(sEndianness != endian_undefined);
	assert(sizeof(NxI16) == 2);
	if(sEndianness != sEndiannessOfData)
	{
		pc = (NxU8*)p;
		SWAP(pc[0], pc[1], c);
	}
}

void byteswap_int32(void* p)
{
	NxI16 w;
	NxI16* pw;
	assert(sEndianness != endian_undefined);
	assert(sizeof(INT32) == 4);
	if(sEndianness != sEndiannessOfData)
	{
		pw = (NxI16*)p;
		byteswap_int16(pw);
		byteswap_int16(pw+1);
		SWAP(pw[0], pw[1], w);
	}
}



#define LEVDOC_MAGIC		"trrn"
//#define LEVDOC_VER			4
#define MAX_DESCLEN			64
#define BUFSIZE				1024
#define write_item(_i)		writef(&(_i), sizeof(_i), f)
#define str_equal(s1, s2)	(strcmp((s1), (s2)) == 0)

typedef struct
{
	NxU8		descriptorLen;
	char				descriptor[MAX_DESCLEN+1];  // Note: not null-terminated
	UINT32				dataLen;
	void*				pData;
} Chunk;


NxI32 readf(void* pv, size_t bytes, FILE* f)
{
	assert(pv != NULL && f != NULL);
	if(1 == fread(pv, bytes, 1, f))
		return TRUE;
	fclose(f);
	return FALSE;
}


NxI32 readf_int16(void* pv, FILE* f)
{
	if(!readf(pv, 2, f)) { return FALSE; }
	byteswap_int16(pv); return TRUE;
}


NxI32 readf_int32(void* pv, FILE* f)
{
	if(!readf(pv, 4, f)) { return FALSE; }
	byteswap_int32(pv); return TRUE;
}


NxI32 writef(void* pv, size_t bytes, FILE* f)
{
	assert(pv != NULL && f != NULL);
	if(1 == fwrite(pv, bytes, 1, f))
		return TRUE;
	fclose(f);
	return FALSE;
}


NxI32 writef_int16(void* pv, FILE* f)
{
	byteswap_int16(pv);
	if(!writef(pv, 2, f)) { byteswap_int16(pv); return FALSE; }
	byteswap_int16(pv); return TRUE;
}


NxI32 writef_int32(void* pv, FILE* f)
{
	byteswap_int32(pv);
	if(!writef(pv, 4, f)) { byteswap_int32(pv); return FALSE; }
	byteswap_int32(pv); return TRUE;
}


NxI32 write_chunkheader(FILE* f, Chunk* pChunk)
{
	return (write_item(pChunk->descriptorLen) &&
		fputs(pChunk->descriptor, f) >= 0  &&
		writef_int32(&pChunk->dataLen, f));
}


NxI32 write_chunk(FILE* f, Chunk* pChunk)
{
	if(!write_chunkheader(f, pChunk)) 
		return FALSE;
	
	if(1 == fwrite(pChunk->pData, pChunk->dataLen, 1, f))
		return TRUE;

	fclose(f);
	return FALSE;
}


NxI32 write_int32chunk(FILE* f, char* pszDesc, INT32 i)
{
	Chunk chunk;
	INT32	n;

	chunk.descriptorLen = (NxU8)strlen(pszDesc);
	strcpy(chunk.descriptor, pszDesc);
	n = i;
	chunk.dataLen = sizeof(n);
	chunk.pData = &n;
	byteswap_int32(chunk.pData);
	return write_chunk(f, &chunk);
}


/*
	LevHF_Read()
	Read a heightfield from a Leveller document.

	pszFile		The Leveller document to read from.

	Returns pointer to heightfield if okay, NULL otherwise.
	Based on code provided courtesy of Lucas Ackerman.
*/
LEV_HF*	LevHF_Read(const char* pszFile)
{
	FILE*			f = NULL;
	LEV_HF*			pHF = NULL;
	Chunk			chunk;
	LEV_SIZE		hfWidth = 0, hfBreadth = 0;
	char			sz[10];

	/* Elevation data block transfer vars. */
	FIX32			buf[BUFSIZE];	/* Transfer buffer */
	NxI32			hfDataLoc = 0;	/* Location of elevation values block */
	UINT32			hfDataLen=0;		/* Byte length of elevation values block */
	UINT32			elevsToRead, elevsToXfer;
	UINT32			i;
	ELEV*			p = NULL;
	NxI32				ver;

	byteswapper_init(endian_intel);

	f = fopen(pszFile, "rb"); //open for read in binary mode
	if(f == NULL)
		return NULL;

	// Read magic number and perform sanity checks.
	if(!readf(sz, strlen(LEVDOC_MAGIC), f))
		return NULL;

	sz[strlen(LEVDOC_MAGIC)] = '\0';

	if(!str_equal(LEVDOC_MAGIC, sz))
	{
		fclose(f);
		return NULL;
	}

	// Read file format version, make sure it's compatible.
	if(!readf(sz, 1, f) 
		|| sz[0] < LOWEST_LEVFILE_VER
		|| sz[0] > HIGHEST_LEVFILE_VER)
		return NULL;

	ver = sz[0];

	hfWidth = hfBreadth = 0;

	
	while(!feof(f)) // Process chunks until EOF
	{
		// Read chunk header.
		if(!readf(&chunk.descriptorLen, sizeof(chunk.descriptorLen), f) ||
			!readf(chunk.descriptor, chunk.descriptorLen, f) ||
			!readf_int32(&chunk.dataLen, f))
		{
			f = fopen(pszFile, "rb");
			if(f == NULL)
				return NULL;
			break;
		}

		if(chunk.descriptorLen > MAX_DESCLEN)
		{
			fclose(f);
			return NULL;
		}

		// Append null char to Chunk Descriptor string
		chunk.descriptor[chunk.descriptorLen] = '\0';

		// read heightfield breadth chunk
		if(str_equal("hf_b", chunk.descriptor))
		{
			if(!readf_int32(&hfBreadth, f))
				return NULL;
		}
		else if(str_equal("hf_w", chunk.descriptor))
		{
			if(!readf_int32(&hfWidth, f))
				return NULL;
		}
		else if(str_equal("hf_data", chunk.descriptor))
		{
			hfDataLen = chunk.dataLen;
			hfDataLoc = ftell(f);
			fseek(f, chunk.dataLen, SEEK_CUR); 
		} 
		else
			fseek(f, chunk.dataLen, SEEK_CUR); 
			
	} /* while reading document */

	if(hfDataLoc == 0 || hfWidth == 0 || hfBreadth == 0 || 
		hfDataLen != 4 * hfWidth * hfBreadth)
	{
		fclose(f);
		return NULL;	/* Never found needed chunks, or invalid hf data length. */
	}

	pHF = LevHF_Create(hfWidth, hfBreadth);
	if(pHF == NULL)
	{
		fclose(f);
		return NULL;	/* Couldn't allocate heightfield. */
	}

	/* Read elevation values. */
	fseek(f, hfDataLoc, SEEK_SET);

	p = pHF->pElevs;
	elevsToRead = hfDataLen / sizeof(buf[0]);

	while(elevsToRead > 0)
	{
		elevsToXfer = min(BUFSIZE, elevsToRead);
		elevsToRead -= elevsToXfer;

		if(!readf(buf, elevsToXfer * sizeof(buf[0]), f))
		{
			LevHF_Delete(pHF);
			return NULL;
		}

		for(i = 0; i < elevsToXfer; i++, p++)
		{
			byteswap_int32(&buf[i]);
			if(ver >= 6)
				*p = *((ELEV*)&buf[i]);
			else
				*p = FixedToFloat(buf[i]);
		}
	}

	fclose(f);
	return pHF;
}



/*
	LevHF_Write()
	Write a heightfield to a Leveller document.

	pHF		Pointer to heightfield.
	pszFile	The file to write to.
	version The version to write (4, 5, or 6).

	Returns TRUE if okay, FALSE otherwise.
*/
NxI32	LevHF_Write(const LEV_HF* pHF, const char* pszFile, NxI32 ver)
{
	UINT32			i;
	FILE*			f;
	FIX32			buf[BUFSIZE]; /* Make file I/O go faster */
	ELEV*			p = NULL;
	Chunk			chunk;
	UINT32			elevsToWrite, elevsToXfer;

	ASSERT_HF_OKAY(pHF);

	byteswapper_init(endian_intel);

	elevsToWrite = pHF->width * pHF->breadth;

	f = fopen(pszFile, "wb");
	if(f == NULL)
		return FALSE;

    // Write header.
	fputs("trrn", f);
	fputc(ver, f);

	// Write desired chunks.
	chunk.descriptorLen = 7;
	strcpy(chunk.descriptor, "hf_data");
	chunk.dataLen = elevsToWrite * sizeof(ELEV);

	if(	!write_int32chunk(f, "hf_w", pHF->width) ||
		!write_int32chunk(f, "hf_b", pHF->breadth) ||
		!write_chunkheader(f, &chunk))
		return FALSE;

	/* Now write out the elevations in 16.16 fixed-point format. */
	p = pHF->pElevs;

	while(elevsToWrite > 0)
	{
		elevsToXfer = min(BUFSIZE, elevsToWrite);
		elevsToWrite -= elevsToXfer;

		for(i = 0; i < elevsToXfer; i++, p++)
		{
			if(ver >= 6)
				*((ELEV*)&buf[i]) = *p;
			else
				buf[i] = FloatToFixed(*p);
			byteswap_int32(&buf[i]);
		}

		if(!writef(buf, elevsToXfer * sizeof(buf[0]), f))
			return FALSE;
	}

	fclose(f);
	return TRUE;
}




class MyImportHeightMap : public ImportHeightMap, public NVSHARE::Memalloc
{
public:
  MyImportHeightMap(void)
  {
    mWidth = 0;
    mDepth = 0;
    mData  = 0;
  }

  ~MyImportHeightMap(void)
  {
    release();
  }

  void release(void)
  {
    MEMALLOC_FREE(mData);
    mData = 0;
    mWidth = 0;
    mDepth = 0;
  }

  bool  importHeightMap(const char * fname)
  {
    bool ret = false;

    release();

    LEV_HF *hf = LevHF_Read(fname);
    if ( hf )
    {
      mWidth = hf->width;
      mDepth = hf->breadth;

      NxU32 size = mWidth*mDepth;
      NxF32 minv = hf->pElevs[0];
      NxF32 maxv = hf->pElevs[0];
      for (NxU32 i=1; i<size; i++)
      {
        NxF32 v = hf->pElevs[i];
        if ( v < minv ) minv = v;
        if ( v > maxv ) maxv = v;
      }

      NxF32 diff = maxv-minv;
      NxF32 recip = 1;
      if ( diff > 0 )
      {
        recip = 1.0f / diff;
      }


      mData  = (NxF32 *)MEMALLOC_MALLOC( sizeof(NxF32)*mWidth*mDepth);

      for (NxU32 i=0; i<size; i++)
      {
        NxF32 v = hf->pElevs[i];
        v = (v-minv)*recip;
        mData[i] = v;
      }

      LevHF_Delete(hf);
      ret = true;
    }

    return ret;
  }

  NxU32 getWidth(void) const { return mWidth; };
  NxU32 getDepth(void) const { return mDepth; };
  NxF32 * getData(void) const { return mData; };
  NxF32 getPoint(NxU32 x,NxU32 y)
  {
    NxF32 ret = 0;

    if ( mData )
    {
      assert( x >= 0 && x < mWidth );
      assert( y >= 0 && y < mDepth );
      if ( x >= 0 && x < mWidth &&
           y >= 0 && y < mDepth )
      {
        NxU32 index = y*mWidth+x;
        ret = mData[index];
      }
    }
    return ret;
  }

  bool exportHeightMap(const char *fname)
  {
    bool ret = false;

    LEV_HF hf;

    hf.width = mWidth;
    hf.breadth = mDepth;
    hf.pElevs = mData;

    NxI32 ok = LevHF_Write(&hf,fname,6);
    if ( ok )
    {
      ret = true;
    }

    return ret;
  }

  NxF32 getHeightRange(NxF32 &vlow,NxF32 &vhigh)
  {
    vlow = mVlow;
    vhigh = mVhigh;
    return vhigh-vlow;
  }

private:
  NxF32         mVhigh;
  NxF32         mVlow;

  NxU32  mWidth;
  NxU32  mDepth;
  NxF32 *mData;

};

}; // end of the namespace

using namespace IMPORT_HEIGHTMAP;

ImportHeightMap * createImportHeightMap(void)
{
  MyImportHeightMap *m = MEMALLOC_NEW(MyImportHeightMap);
  return static_cast< ImportHeightMap *>(m);
}

void              releaseImportHeightMap(ImportHeightMap *hm)
{
  MyImportHeightMap *m = static_cast< MyImportHeightMap *>(hm);
  delete m;
}
