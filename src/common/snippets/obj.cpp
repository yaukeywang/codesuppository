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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "obj.h"
#include "common/snippets/filesystem.h"

bool OBJ::mNormalize = false;

static char * nextDir(char *start)
{

	while ( *start )
	{
		if ( *start == '\\' ) return start;
		if ( *start == '/' ) return start;
		start++;
	}
	return 0;
}

char * skipDir(char *start)
{
	char *slash = start;
  static bool gstate=true;
	while ( gstate )
	{
	  slash = nextDir(slash);
		if ( slash )
		{
			start = slash+1;
			slash = start;
		}
		else
			break;
	}
  return start;
}

HeI32 OBJ::LoadMesh(const char *fname,GeometryInterface *iface)
{
  HeI32 ret = 0;

  release();


  mMin.Set(FLT_MAX,FLT_MAX,FLT_MAX);
  mMax.Set(FLT_MIN,FLT_MIN,FLT_MIN);

  mCallback = iface;

  {
    mProcessMaterials = true;
    char scratch[512];
    strcpy(scratch,fname);
    char *dot = strrchr(scratch, '.' );
    if ( dot )
  	  *dot = 0;
    strcat(scratch,".mtl");
    InPlaceParser ipp(scratch,gFileSystem);
    ipp.Parse(this);
    mProcessMaterials = false;
  }



  InPlaceParser ipp(fname,gFileSystem);

  iface->NodeMesh(fname,0);

  char scratch[512];
  strcpy(scratch,fname);
	char *start = skipDir(scratch);

  char *dot = strrchr( start, '.' );
  if ( dot )
	  *dot = 0;
  strcat(start,".tga");

  iface->NodeMaterial(start,0);

  ipp.Parse(this);


  return ret;
}

void OBJ::GetVertex(GeometryVertex &v,const char *face,bool minusIndexing) const
{
  v.mPos[0] = 0;
  v.mPos[1] = 0;
  v.mPos[2] = 0;

  v.mTexel1[0] = 0;
  v.mTexel1[1] = 0;

  v.mTexel2[0] = 0;
  v.mTexel2[1] = 0;

  v.mNormal[0] = 0;
  v.mNormal[1] = 1;
  v.mNormal[2] = 0;

  HeI32 index = atoi( face );
  if ( minusIndexing )
    index--;

  if ( index < 0 )
    index = mVerts.size()+index+1;

  if ( index >= 0 && index < (HeI32)mVerts.size() )
  {

    const Vector3d<HeF32> &p = mVerts[index];

    v.mPos[0] = p.x;
    v.mPos[1] = p.y;
    v.mPos[2] = p.z;
  }


  const char *texel = strstr(face,"/");

  if ( texel )
  {
    HeI32 tindex = atoi( texel+1) - 1;
    if ( tindex < 0 )
      tindex = mTexels.size()+tindex+1;

    if ( tindex >=0 && tindex < (HeI32)mTexels.size() )
    {
      v.mTexel1[0] = mTexels[tindex].x;
      v.mTexel1[1] = mTexels[tindex].y;

      v.mTexel2[0] = v.mTexel1[0];
      v.mTexel2[1] = v.mTexel1[1];

    }

    const char *normal = strstr(texel+1,"/");
    if ( normal )
    {
      HeI32 nindex = atoi( normal+1 ) - 1;
      if ( nindex < 0 )
        nindex = mNormals.size()+nindex+1;
      if (nindex >= 0 && nindex < (HeI32)mNormals.size() )
      {
        v.mNormal[0] = mNormals[nindex].x;
        v.mNormal[1] = mNormals[nindex].y;
        v.mNormal[2] = mNormals[nindex].z;
      }
    }
  }

}

void OBJ::SetProperty(const char * /* key */,const char * /* value */)
{
  //TODO: Handle properties
}

HeI32 OBJ::ParseLine(HeI32 /* lineno */,HeI32 argc,const char **argv)  // return TRUE to continue parsing, return FALSE to abort parsing process
{
  HeI32 ret = 0;

  if ( argc >= 1 )
  {
    const char *foo = argv[0];
    if ( *foo != '#' )
    {

      if ( mProcessMaterials )
      {
        if ( stricmp(foo,"newmtl") == 0 )
        {
          StringRef matname;
          if ( argc >=2 ) matname = SGET(argv[1]);
          mCurrentMaterial = MEMALLOC_NEW(ObjMaterial)(matname);
          mMaterials.push_back(mCurrentMaterial);
        }
        else if ( mCurrentMaterial && stricmp(foo,"Ns") == 0 && argc >= 2 )
        {
          mCurrentMaterial->Ns = (HeF32)atof(argv[1]);
        }
        else if ( mCurrentMaterial && stricmp(foo,"Ka") == 0 && argc >= 4 )
        {
          mCurrentMaterial->Ka.x = (HeF32)atof(argv[1]);
          mCurrentMaterial->Ka.y = (HeF32)atof(argv[2]);
          mCurrentMaterial->Ka.z = (HeF32)atof(argv[3]);
        }
        else if ( mCurrentMaterial && stricmp(foo,"Kd") == 0 && argc >= 4 )
        {
          mCurrentMaterial->Kd.x = (HeF32)atof(argv[1]);
          mCurrentMaterial->Kd.y = (HeF32)atof(argv[2]);
          mCurrentMaterial->Kd.z = (HeF32)atof(argv[3]);
        }
        else if ( mCurrentMaterial && stricmp(foo,"Ks") == 0 && argc >= 4 )
        {
          mCurrentMaterial->Ks.x = (HeF32)atof(argv[1]);
          mCurrentMaterial->Ks.y = (HeF32)atof(argv[2]);
          mCurrentMaterial->Ks.z = (HeF32)atof(argv[3]);
        }
        else if ( mCurrentMaterial && stricmp(foo,"map_kd") == 0 && argc >= 2 )
        {
          mCurrentMaterial->map_kd = SGET( argv[1] );
        }
      }
      else
      {
        if ( strcmp(argv[0],"mtllib") == 0 && argc >= 2 )
        {
          // ok, load this material library...
          const char *fname = argv[1];
					if ( gFileSystem )
            fname = gFileSystem->FileOpenString(fname,true);
          mProcessMaterials = true;
          InPlaceParser ipp(fname,gFileSystem);
          ipp.Parse(this);
          mProcessMaterials = false;
        }
        else if ( strcmp(argv[0],"usemtl") == 0 && argc>= 2 )
        {
          for (size_t i=0; i<mMaterials.size(); i++)
          {
            ObjMaterial *mat = mMaterials[i];
            if ( strcmp(argv[1],mat->mName) == 0 )
            {
              mCallback->NodeMaterial( mat->map_kd, 0 );
            }
          }
        }
        else if ( stricmp(argv[0],"v") == 0 && argc == 4 )
        {
          Vector3d<HeF32> v;
          v.x = (HeF32) atof( argv[1] );
          v.y = (HeF32) atof( argv[2] );
          v.z = (HeF32) atof( argv[3] );

          if ( v.x < mMin.x ) mMin.x = v.x;
          if ( v.y < mMin.y ) mMin.y = v.y;
          if ( v.z < mMin.z ) mMin.z = v.z;

          if ( v.x > mMax.x ) mMax.x = v.x;
          if ( v.y > mMax.y ) mMax.y = v.y;
          if ( v.z > mMax.z ) mMax.z = v.z;

          mVerts.push_back(v);
        }
        else if ( stricmp(argv[0],"vt") == 0 && argc >= 3 )
        {
          Vector2d<HeF32> t;
          t.x = (HeF32)atof( argv[1] );
          t.y = (HeF32)atof( argv[2] );
					t.x = t.x;
					t.y = 1.0f - t.y;
          mTexels.push_back(t);
        }
        else if ( stricmp(argv[0],"vn") == 0 && argc == 4 )
        {
          Vector3d<HeF32> normal;
          normal.x = (HeF32) atof(argv[1]);
          normal.y = (HeF32) atof(argv[2]);
          normal.z = (HeF32) atof(argv[3]);
          mNormals.push_back(normal);
        }
        else if ( stricmp(argv[0],"t") == 0 && argc == 5 )
        {
          GeometryVertex v[4];

          GetVertex(v[0],argv[1],false );
          GetVertex(v[1],argv[2],false );
          GetVertex(v[2],argv[3],false );
          GetVertex(v[3],argv[4],false );
          sendTri(v[2],v[1],v[0]);
          sendTri(v[0],v[1],v[3]);
          sendTri(v[1],v[2],v[3]);
          sendTri(v[2],v[0],v[3]);
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

          sendTri(v[0],v[1],v[2]);

          if ( vcount >=3 ) // do the fan
          {
            for (HeI32 i=2; i<(vcount-1); i++)
            {
              sendTri(v[0],v[i],v[i+1]);
            }
          }

        }
      }
    }
  }

  return ret;
}


OBJ::OBJ(void)
{
  mCurrentMaterial = 0;
}

OBJ::~OBJ(void)
{
  release();
}

void OBJ::release(void)
{
  mCurrentMaterial = 0;
  for (size_t i=0; i<mMaterials.size(); i++)
  {
    ObjMaterial *om = mMaterials[i];
    delete om;
  }
  mMaterials.clear();
  mVerts.clear();
  mTexels.clear();
  mNormals.clear();
}


static void set(HeF32 *d,const Vector3d<HeF32> &t)
{
  d[0] = t.x;
  d[1] = t.y;
  d[2] = t.z;
}

void OBJ::sendTri(const GeometryVertex &tv1,const GeometryVertex &tv2,const GeometryVertex &tv3)
{

  GeometryVertex v1 = tv1;
  GeometryVertex v2 = tv2;
  GeometryVertex v3 = tv3;

  Vector3d<HeF32> p1( v1.mPos );
  Vector3d<HeF32> p2( v2.mPos );
  Vector3d<HeF32> p3( v3.mPos );

  if ( mNormalize )
  {
    HeF32 dist = mMin.Distance(mMax);
    HeF32 recip = 1.0f / dist;

    p1*=recip;
    p2*=recip;
    p3*=recip;

    set(v1.mPos,p1.Ptr());
    set(v2.mPos,p2.Ptr());
    set(v3.mPos,p3.Ptr());
  }


  if ( mNormals.empty() )
  {
    Vector3d<HeF32> n;
    n.ComputeNormal(p3,p2,p1);

    set(v1.mNormal,n);
    set(v2.mNormal,n);
    set(v3.mNormal,n);
  }

  if ( mTexels.empty() ) // if we don't have any texture co-ordinates then synthesize something
  {
  	#define TSCALE1 (1.0f/4.0f)

  	const HeF32 *tp1 = p1.Ptr();
  	const HeF32 *tp2 = p2.Ptr();
  	const HeF32 *tp3 = p3.Ptr();

  	HeI32 i1 = 0;
	  HeI32 i2 = 0;

  	HeF32 nx = fabsf(v1.mNormal[0]);
  	HeF32 ny = fabsf(v1.mNormal[1]);
  	HeF32 nz = fabsf(v1.mNormal[2]);

  	if ( nx <= ny && nx <= nz )
  		i1 = 0;
  	if ( ny <= nx && ny <= nz )
  		i1 = 1;
  	if ( nz <= nx && nz <= ny )
  		i1 = 2;

  	switch ( i1 )
  	{
	  	case 0:
  			if ( ny < nz )
  				i2 = 1;
  			else
  				i2 = 2;
  			break;
  		case 1:
  			if ( nx < nz )
  				i2 = 0;
  			else
  				i2 = 2;
  			break;
  		case 2:
  			if ( nx < ny )
  				i2 = 0;
  			else
  				i2 = 1;
			break;
  	}

	  v1.mTexel1[0] = tp1[i1]*TSCALE1;
  	v1.mTexel1[1] = tp1[i2]*TSCALE1;

  	v2.mTexel1[0] = tp2[i1]*TSCALE1;
  	v2.mTexel1[1] = tp2[i2]*TSCALE1;

  	v3.mTexel1[0] = tp3[i1]*TSCALE1;
  	v3.mTexel1[1] = tp3[i2]*TSCALE1;

	  v1.mTexel2[0] = tp1[i1]*TSCALE1;
  	v1.mTexel2[1] = tp1[i2]*TSCALE1;

  	v2.mTexel2[0] = tp2[i1]*TSCALE1;
  	v2.mTexel2[1] = tp2[i2]*TSCALE1;

  	v3.mTexel2[0] = tp3[i1]*TSCALE1;
  	v3.mTexel2[1] = tp3[i2]*TSCALE1;
  }

  mCallback->NodeTriangle(&v1,&v2,&v3);

}

