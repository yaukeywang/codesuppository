#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#pragma warning(disable:4996)

/*!
**
** Copyright (c) 2007 by John W. Ratcliff mailto:jratcliff@infiniplex.net
**
** Portions of this source has been released with the PhysXViewer application, as well as
** Rocket, CreateDynamics, ODF, and as a number of sample code snippets.
**
** If you find this code useful or you are feeling particularily generous I would
** ask that you please go to http://www.amillionpixels.us and make a donation
** to Troy DeMolay.
**
** DeMolay is a youth group for young men between the ages of 12 and 21.
** It teaches strong moral principles, as well as leadership skills and
** public speaking.  The donations page uses the 'pay for pixels' paradigm
** where, in this case, a pixel is only a single penny.  Donations can be
** made for as small as $4 or as high as a $100 block.  Each person who donates
** will get a link to their own site as well as acknowledgement on the
** donations blog located here http://www.amillionpixels.blogspot.com/
**
** If you wish to contact me you can use the following methods:
**
** Skype Phone: 636-486-4040 (let it ring a long time while it goes through switches)
** Skype ID: jratcliff63367
** Yahoo: jratcliff63367
** AOL: jratcliff1961
** email: jratcliff@infiniplex.net
** Personal website: http://jratcliffscarab.blogspot.com
** Coding Website:   http://codesuppository.blogspot.com
** FundRaising Blog: http://amillionpixels.blogspot.com
** Fundraising site: http://www.amillionpixels.us
** New Temple Site:  http://newtemple.blogspot.com
**
**
** The MIT license:
**
** Permission is hereby granted, MEMALLOC_FREE of charge, to any person obtaining a copy
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



#include "common/snippets/UserMemAlloc.h"
#include "wavefront.h"
#include "common/snippets/FloatMath.h"

#include <vector>

typedef USER_STL::vector< HeI32 > IntVector;
typedef USER_STL::vector< HeF32 > FloatVector;

namespace WAVEFRONT
{

#if defined(__APPLE__) || defined(__CELLOS_LV2__) || defined(LINUX)
#define stricmp(a, b) strcasecmp((a), (b))
#endif

/*******************************************************************/
/******************** InParser.h  ********************************/
/*******************************************************************/
class InPlaceParserInterface
{
public:
	virtual HeI32 ParseLine(HeI32 lineno,HeI32 argc,const char **argv) =0;  // return TRUE to continue parsing, return FALSE to abort parsing process
};

enum SeparatorType
{
	ST_DATA,        // is data
	ST_HARD,        // is a hard separator
	ST_SOFT,        // is a soft separator
	ST_EOS          // is a comment symbol, and everything past this character should be ignored
};

class InPlaceParser
{
public:
	InPlaceParser(void)
	{
		Init();
	}

	InPlaceParser(char *data,HeI32 len)
	{
		Init();
		SetSourceData(data,len);
	}

	InPlaceParser(const char *fname)
	{
		Init();
		SetFile(fname);
	}

	~InPlaceParser(void);

	void Init(void)
	{
		mQuoteChar = 34;
		mData = 0;
		mLen  = 0;
		mMyAlloc = false;
		for (HeI32 i=0; i<256; i++)
		{
			mHard[i] = ST_DATA;
			mHardString[i*2] = (char)i;
			mHardString[i*2+1] = 0;
		}
		mHard[0]  = ST_EOS;
		mHard[32] = ST_SOFT;
		mHard[9]  = ST_SOFT;
		mHard[13] = ST_SOFT;
		mHard[10] = ST_SOFT;
	}

	void SetFile(const char *fname); // use this file as source data to parse.

	void SetSourceData(char *data,HeI32 len)
	{
		mData = data;
		mLen  = len;
		mMyAlloc = false;
	};

	HeI32  Parse(InPlaceParserInterface *callback); // returns true if entire file was parsed, false if it aborted for some reason

	HeI32 ProcessLine(HeI32 lineno,char *line,InPlaceParserInterface *callback);

	const char ** GetArglist(char *source,HeI32 &count); // convert source string into an arg list, this is a destructive parse.

	void SetHardSeparator(char c) // add a hard separator
	{
		mHard[c] = ST_HARD;
	}

	void SetHard(char c) // add a hard separator
	{
		mHard[c] = ST_HARD;
	}


	void SetCommentSymbol(char c) // comment character, treated as 'end of string'
	{
		mHard[c] = ST_EOS;
	}

	void ClearHardSeparator(char c)
	{
		mHard[c] = ST_DATA;
	}


	void DefaultSymbols(void); // set up default symbols for hard seperator and comment symbol of the '#' character.

	bool EOS(char c)
	{
		if ( mHard[c] == ST_EOS )
		{
			return true;
		}
		return false;
	}

	void SetQuoteChar(char c)
	{
		mQuoteChar = c;
	}

private:


	inline char * AddHard(HeI32 &argc,const char **argv,char *foo);
	inline bool   IsHard(char c);
	inline char * SkipSpaces(char *foo);
	inline bool   IsWhiteSpace(char c);
	inline bool   IsNonSeparator(char c); // non seperator,neither hard nor soft

	bool   mMyAlloc; // whether or not *I* allocated the buffer and am responsible for deleting it.
	char  *mData;  // ascii data to parse.
	HeI32    mLen;   // length of data
	SeparatorType  mHard[256];
	char   mHardString[256*2];
	char           mQuoteChar;
};

/*******************************************************************/
/******************** InParser.cpp  ********************************/
/*******************************************************************/
void InPlaceParser::SetFile(const char *fname)
{
	if ( mMyAlloc )
	{
		MEMALLOC_FREE(mData);
	}
	mData = 0;
	mLen  = 0;
	mMyAlloc = false;

	FILE *fph = fopen(fname,"rb");
	if ( fph )
	{
		fseek(fph,0L,SEEK_END);
		mLen = ftell(fph);
		fseek(fph,0L,SEEK_SET);
		if ( mLen )
		{
			mData = (char *) MEMALLOC_MALLOC(sizeof(char)*(mLen+1));
			HeI32 ok = fread(mData, mLen, 1, fph);
			if ( !ok )
			{
				MEMALLOC_FREE(mData);
				mData = 0;
			}
			else
			{
				mData[mLen] = 0; // zero byte terminate end of file marker.
				mMyAlloc = true;
			}
		}
		fclose(fph);
	}
}

InPlaceParser::~InPlaceParser(void)
{
	if ( mMyAlloc )
	{
		MEMALLOC_FREE(mData);
	}
}

#define MAXARGS 512

bool InPlaceParser::IsHard(char c)
{
	return mHard[c] == ST_HARD;
}

char * InPlaceParser::AddHard(HeI32 &argc,const char **argv,char *foo)
{
	while ( IsHard(*foo) )
	{
		const char *hard = &mHardString[*foo*2];
		if ( argc < MAXARGS )
		{
			argv[argc++] = hard;
		}
		foo++;
	}
	return foo;
}

bool   InPlaceParser::IsWhiteSpace(char c)
{
	return mHard[c] == ST_SOFT;
}

char * InPlaceParser::SkipSpaces(char *foo)
{
	while ( !EOS(*foo) && IsWhiteSpace(*foo) ) foo++;
	return foo;
}

bool InPlaceParser::IsNonSeparator(char c)
{
	if ( !IsHard(c) && !IsWhiteSpace(c) && c != 0 ) return true;
	return false;
}


HeI32 InPlaceParser::ProcessLine(HeI32 lineno,char *line,InPlaceParserInterface *callback)
{
	HeI32 ret = 0;

	const char *argv[MAXARGS];
	HeI32 argc = 0;

	char *foo = line;

	while ( !EOS(*foo) && argc < MAXARGS )
	{

		foo = SkipSpaces(foo); // skip any leading spaces

		if ( EOS(*foo) ) break;

		if ( *foo == mQuoteChar ) // if it is an open quote
		{
			foo++;
			if ( argc < MAXARGS )
			{
				argv[argc++] = foo;
			}
			while ( !EOS(*foo) && *foo != mQuoteChar ) foo++;
			if ( !EOS(*foo) )
			{
				*foo = 0; // replace close quote with zero byte EOS
				foo++;
			}
		}
		else
		{

			foo = AddHard(argc,argv,foo); // add any hard separators, skip any spaces

			if ( IsNonSeparator(*foo) )  // add non-hard argument.
			{
				bool quote  = false;
				if ( *foo == mQuoteChar )
				{
					foo++;
					quote = true;
				}

				if ( argc < MAXARGS )
				{
					argv[argc++] = foo;
				}

				if ( quote )
				{
					while (*foo && *foo != mQuoteChar ) foo++;
					if ( *foo ) *foo = 32;
				}

				// continue..until we hit an eos ..
				while ( !EOS(*foo) ) // until we hit EOS
				{
					if ( IsWhiteSpace(*foo) ) // if we hit a space, stomp a zero byte, and exit
					{
						*foo = 0;
						foo++;
						break;
					}
					else if ( IsHard(*foo) ) // if we hit a hard separator, stomp a zero byte and store the hard separator argument
					{
						const char *hard = &mHardString[*foo*2];
						*foo = 0;
						if ( argc < MAXARGS )
						{
							argv[argc++] = hard;
						}
						foo++;
						break;
					}
					foo++;
				} // end of while loop...
			}
		}
	}

	if ( argc )
	{
		ret = callback->ParseLine(lineno, argc, argv );
	}

	return ret;
}

HeI32  InPlaceParser::Parse(InPlaceParserInterface *callback) // returns true if entire file was parsed, false if it aborted for some reason
{
	HE_ASSERT( callback );
	if ( !mData ) return 0;

	HeI32 ret = 0;

	HeI32 lineno = 0;

	char *foo   = mData;
	char *begin = foo;


	while ( *foo )
	{
		if ( *foo == 10 || *foo == 13 )
		{
			lineno++;
			*foo = 0;

			if ( *begin ) // if there is any data to parse at all...
			{
				HeI32 v = ProcessLine(lineno,begin,callback);
				if ( v ) ret = v;
			}

			foo++;
			if ( *foo == 10 ) foo++; // skip line feed, if it is in the carraige-return line-feed format...
			begin = foo;
		}
		else
		{
			foo++;
		}
	}

	lineno++; // lasst line.

	HeI32 v = ProcessLine(lineno,begin,callback);
	if ( v ) ret = v;
	return ret;
}


void InPlaceParser::DefaultSymbols(void)
{
	SetHardSeparator(',');
	SetHardSeparator('(');
	SetHardSeparator(')');
	SetHardSeparator('=');
	SetHardSeparator('[');
	SetHardSeparator(']');
	SetHardSeparator('{');
	SetHardSeparator('}');
	SetCommentSymbol('#');
}


const char ** InPlaceParser::GetArglist(char *line,HeI32 &count) // convert source string into an arg list, this is a destructive parse.
{
	const char **ret = 0;

	static const char *argv[MAXARGS];
	HeI32 argc = 0;

	char *foo = line;

	while ( !EOS(*foo) && argc < MAXARGS )
	{

		foo = SkipSpaces(foo); // skip any leading spaces

		if ( EOS(*foo) ) break;

		if ( *foo == mQuoteChar ) // if it is an open quote
		{
			foo++;
			if ( argc < MAXARGS )
			{
				argv[argc++] = foo;
			}
			while ( !EOS(*foo) && *foo != mQuoteChar ) foo++;
			if ( !EOS(*foo) )
			{
				*foo = 0; // replace close quote with zero byte EOS
				foo++;
			}
		}
		else
		{

			foo = AddHard(argc,argv,foo); // add any hard separators, skip any spaces

			if ( IsNonSeparator(*foo) )  // add non-hard argument.
			{
				bool quote  = false;
				if ( *foo == mQuoteChar )
				{
					foo++;
					quote = true;
				}

				if ( argc < MAXARGS )
				{
					argv[argc++] = foo;
				}

				if ( quote )
				{
					while (*foo && *foo != mQuoteChar ) foo++;
					if ( *foo ) *foo = 32;
				}

				// continue..until we hit an eos ..
				while ( !EOS(*foo) ) // until we hit EOS
				{
					if ( IsWhiteSpace(*foo) ) // if we hit a space, stomp a zero byte, and exit
					{
						*foo = 0;
						foo++;
						break;
					}
					else if ( IsHard(*foo) ) // if we hit a hard separator, stomp a zero byte and store the hard separator argument
					{
						const char *hard = &mHardString[*foo*2];
						*foo = 0;
						if ( argc < MAXARGS )
						{
							argv[argc++] = hard;
						}
						foo++;
						break;
					}
					foo++;
				} // end of while loop...
			}
		}
	}

	count = argc;
	if ( argc )
	{
		ret = argv;
	}

	return ret;
}

/*******************************************************************/
/******************** Geometry.h  ********************************/
/*******************************************************************/

class GeometryVertex
{
public:
	HeF32        mPos[3];
	HeF32        mNormal[3];
	HeF32        mTexel[2];
};


class GeometryInterface
{
public:

	virtual void NodeTriangle(const GeometryVertex * /*v1*/,const GeometryVertex * /*v2*/,const GeometryVertex * /*v3*/)
	{
	}

};


/*******************************************************************/
/******************** Obj.h  ********************************/
/*******************************************************************/


class OBJ : public InPlaceParserInterface
{
public:
  HeI32          LoadMesh(const char *fname,GeometryInterface *callback);
  HeI32 ParseLine(HeI32 lineno,HeI32 argc,const char **argv);  // return TRUE to continue parsing, return FALSE to abort parsing process
private:

  void GetVertex(GeometryVertex &v,const char *face) const;

  FloatVector     mVerts;
  FloatVector     mTexels;
  FloatVector     mNormals;

  GeometryInterface *mCallback;
};


/*******************************************************************/
/******************** Obj.cpp  ********************************/
/*******************************************************************/

HeI32 OBJ::LoadMesh(const char *fname,GeometryInterface *iface)
{
  HeI32 ret = 0;

  mVerts.clear();
  mTexels.clear();
  mNormals.clear();

  mCallback = iface;

  InPlaceParser ipp(fname);

  ipp.Parse(this);


  return ret;
}

void OBJ::GetVertex(GeometryVertex &v,const char *face) const
{
  v.mPos[0] = 0;
  v.mPos[1] = 0;
  v.mPos[2] = 0;

  v.mTexel[0] = 0;
  v.mTexel[1] = 0;

  v.mNormal[0] = 0;
  v.mNormal[1] = 1;
  v.mNormal[2] = 0;

  HeI32 index = atoi( face )-1;

  const char *texel = strstr(face,"/");

  if ( texel )
  {
    HeI32 tindex = atoi( texel+1) - 1;

    if ( tindex >=0 && tindex < (HeI32)(mTexels.size()/2) )
    {
    	const HeF32 *t = &mTexels[tindex*2];

      v.mTexel[0] = t[0];
      v.mTexel[1] = t[1];

    }

    const char *normal = strstr(texel+1,"/");
    if ( normal )
    {
      HeI32 nindex = atoi( normal+1 ) - 1;

      if (nindex >= 0 && nindex < (HeI32)(mNormals.size()/3) )
      {
      	const HeF32 *n = &mNormals[nindex*3];

        v.mNormal[0] = n[0];
        v.mNormal[1] = n[1];
        v.mNormal[2] = n[2];
      }
    }
  }

  if ( index >= 0 && index < (HeI32)(mVerts.size()/3) )
  {

    const HeF32 *p = &mVerts[index*3];

    v.mPos[0] = p[0];
    v.mPos[1] = p[1];
    v.mPos[2] = p[2];
  }

}

HeI32 OBJ::ParseLine(HeI32 /*lineno*/,HeI32 argc,const char **argv)  // return TRUE to continue parsing, return FALSE to abort parsing process
{
  HeI32 ret = 0;

  if ( argc >= 1 )
  {
    const char *foo = argv[0];
    if ( *foo != '#' )
    {
      if ( stricmp(argv[0],"v") == 0 && argc == 4 )
      {
        HeF32 vx = (HeF32) atof( argv[1] );
        HeF32 vy = (HeF32) atof( argv[2] );
        HeF32 vz = (HeF32) atof( argv[3] );
        mVerts.push_back(vx);
        mVerts.push_back(vy);
        mVerts.push_back(vz);
      }
      else if ( stricmp(argv[0],"vt") == 0 && argc == 3 )
      {
        HeF32 tx = (HeF32) atof( argv[1] );
        HeF32 ty = (HeF32) atof( argv[2] );
        mTexels.push_back(tx);
        mTexels.push_back(ty);
      }
      else if ( stricmp(argv[0],"vn") == 0 && argc == 4 )
      {
        HeF32 normalx = (HeF32) atof(argv[1]);
        HeF32 normaly = (HeF32) atof(argv[2]);
        HeF32 normalz = (HeF32) atof(argv[3]);
        mNormals.push_back(normalx);
        mNormals.push_back(normaly);
        mNormals.push_back(normalz);
      }
      else if ( stricmp(argv[0],"f") == 0 && argc >= 4 )
      {
        GeometryVertex v[32];

        HeI32 vcount = argc-1;

        for (HeI32 i=1; i<argc; i++)
        {
          GetVertex(v[i-1],argv[i] );
        }

        // need to generate a normal!
#if 0 // not currently implemented
        if ( mNormals.empty() )
        {
          Vector3d<HeF32> p1( v[0].mPos );
          Vector3d<HeF32> p2( v[1].mPos );
          Vector3d<HeF32> p3( v[2].mPos );

          Vector3d<HeF32> n;
          n.ComputeNormal(p3,p2,p1);

          for (HeI32 i=0; i<vcount; i++)
          {
            v[i].mNormal[0] = n.x;
            v[i].mNormal[1] = n.y;
            v[i].mNormal[2] = n.z;
          }

        }
#endif

        mCallback->NodeTriangle(&v[0],&v[1],&v[2]);

        if ( vcount >=3 ) // do the fan
        {
          for (HeI32 i=2; i<(vcount-1); i++)
          {
            mCallback->NodeTriangle(&v[0],&v[i],&v[i+1]);
          }
        }

      }
    }
  }

  return ret;
}




class BuildMesh : public GeometryInterface
{
public:
  BuildMesh(HeF32 weldDistance)
  {
    mVertices = fm_createVertexIndex(weldDistance,false);
  }

  ~BuildMesh(void)
  {
    fm_releaseVertexIndex(mVertices);
  }

	virtual void NodeTriangle(const GeometryVertex *v1,const GeometryVertex *v2,const GeometryVertex *v3)
	{
    bool newPos;
		mIndices.push_back( mVertices->getIndex(v1->mPos,newPos) );
		mIndices.push_back( mVertices->getIndex(v2->mPos,newPos) );
		mIndices.push_back( mVertices->getIndex(v3->mPos,newPos) );
	}

  const HeF32 * getVertices(HeU32 &vcount)
  {
    vcount = mVertices->getVcount();
    return mVertices->getVerticesFloat();

  }

  const HeI32 * getIndices(HeU32 &tcount) const 
  { 
    const HeI32 *ret = 0;
    tcount = mIndices.size()/3;
    if ( tcount ) ret = &mIndices[0];
    return ret;
  };

private:
  fm_VertexIndex *mVertices;
  IntVector		    mIndices;
};

};

using namespace WAVEFRONT;

WavefrontObj::WavefrontObj(void)
{
	mVertexCount = 0;
	mTriCount    = 0;
	mIndices     = 0;
	mVertices    = 0;
}

WavefrontObj::~WavefrontObj(void)
{
	MEMALLOC_FREE(mIndices);
	MEMALLOC_FREE(mVertices);
}

HeU32 WavefrontObj::loadObj(const char *fname,HeF32 weldDistance) // load a wavefront obj returns number of triangles that were loaded.  Data is persists until the class is destructed.
{

	HeU32 ret = 0;

	MEMALLOC_FREE(mVertices);
	mVertices = 0;
	MEMALLOC_FREE(mIndices);
	mIndices = 0;
	mVertexCount = 0;
	mTriCount = 0;


  BuildMesh bm(weldDistance);

  OBJ obj;

  obj.LoadMesh(fname,&bm);


  HeU32 vcount;
	const HeF32 *vlist = bm.getVertices(vcount);
  HeU32 tcount;
	const HeI32 *indices = bm.getIndices(tcount);

	if ( vcount )
	{
		mVertexCount = vcount;
		mVertices = (HeF32 *) MEMALLOC_MALLOC(sizeof(HeF32)*mVertexCount*3);
		memcpy( mVertices, vlist, sizeof(HeF32)*mVertexCount*3 );
		mTriCount = tcount;
		mIndices = (HeI32 *) MEMALLOC_MALLOC(sizeof(HeI32)*mTriCount*3*sizeof(HeI32));
		memcpy(mIndices, indices, sizeof(HeI32)*mTriCount*3);
		ret = mTriCount;
	}


	return ret;
}


bool WavefrontObj::saveObj(const char *fname,HeU32 vcount,const HeF32 *vertices,HeU32 tcount,const HeU32 *indices)
{
  bool ret = false;

  FILE *fph = fopen(fname,"wb");
  if ( fph )
  {
    for (HeU32 i=0; i<vcount; i++)
    {
      fprintf(fph,"v %0.9f %0.9f %0.9f\r\n", vertices[0], vertices[1], vertices[2] );
      vertices+=3;
    }
    for (HeU32 i=0; i<tcount; i++)
    {
      fprintf(fph,"f %d %d %d\r\n", indices[0]+1, indices[1]+1, indices[2]+1 );
      indices+=3;
    }
    fclose(fph);
    ret = true;
  }
  return ret;
}
