#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include "hershey.h"
#include "FloatMath.h"
#include <vector>

#define FONT_VERSION 1
#define FONT_SCALE 0.01f

#pragma warning(disable:4100)

typedef std::vector< unsigned int > UIntVector;

class Line
{
public:
	Line(float _x1,float _y1,float _x2,float _y2)
	{
		x1 = _x1;
		y1 = _y1;
		x2 = _x2;
		y2 = _y2;
	}
  float x1;
  float x2;
  float y1;
  float y2;
};

typedef std::vector< Line > LineVector;

class HersheyBuilder : public HersheyCallback
{
public:
  HersheyBuilder(void)
  {
    mVertices = fm_createVertexIndex(0.0001f,false);
	bool first = true;
	float baseY = 0;
    for (unsigned char c=0; c<128; c++)
	{
	  unsigned int base_index = mIndices.size();
	  mCharacters.push_back(base_index);
      mLines.clear();
      hersheyChar((char)c,this);
	  if ( !mLines.empty() )
	  {
		  if ( first )
		  {
			  baseY = mMinY;
			  first = false;
		  }
		  LineVector::iterator i;
		  for (i=mLines.begin(); i!=mLines.end(); ++i)
		  {
			  Line &l = (*i);
			float p1[3] = { l.x1-mMinX, l.y1-baseY, 0 };
			float p2[3] = { l.x2-mMinX, l.y2-baseY, 0 };

			bool newPos;
			unsigned int i1 = mVertices->getIndex(p1,newPos);
			unsigned int i2 = mVertices->getIndex(p2,newPos);

			mIndices.push_back(i1);
			mIndices.push_back(i2);
		  }
	  }
	}
    unsigned int base_index = mIndices.size();
	mCharacters.push_back(base_index);
  }

  ~HersheyBuilder(void)
  {
    fm_releaseVertexIndex(mVertices);
  }

  virtual void line(float x1,float y1,float x2,float y2)
  {
    printf("Line: %0.4f,%0.4f to %0.4f,%0.4f\r\n", x1, y1, x2, y2 );

    x1*=FONT_SCALE;
    y1*=FONT_SCALE;
    x2*=FONT_SCALE;
    y2*=FONT_SCALE;

    if ( mLines.empty() )
    {
        mMinX = x1;
        mMaxX = x1;
        mMinY = y1;
        mMaxY = y1;
    }

    Line l(x1,y1,x2,y2);
    mLines.push_back(l);

    if ( x1 < mMinX ) mMinX = x1;
    if ( x2 < mMinX ) mMinX = x2;
    if ( x1 > mMaxX ) mMaxX = x1;
    if ( x2 > mMaxX ) mMaxX = x2;

    if ( y1 < mMinY ) mMinY = y1;
    if ( y2 < mMinY ) mMinY = y2;
    if ( y1 > mMaxY ) mMaxY = y1;
    if ( y2 > mMaxY ) mMaxY = y2;

  }

  void save(void)
  {
    FILE *fph = fopen("font.bin","wb");
    if ( fph )
    {
      unsigned int icount = 0;
      unsigned int ccount = 0;
      for (int i=0; i<127; i++)
      {
        unsigned int i1 = mCharacters[i];
        unsigned int i2 = mCharacters[i+1];
        unsigned int diff = i2-i1;
        if ( diff > 0 )
        {
            ccount++;
            icount+=diff;
        }
      }


      fwrite("FONT",4,1,fph);
      unsigned int version = FONT_VERSION;
      unsigned int maxVertex = mVertices->getVcount();
      fwrite(&version,sizeof(unsigned int),1,fph);
      fwrite(&maxVertex, sizeof(unsigned int),1,fph);
      fwrite(&ccount, sizeof(unsigned int),1,fph);
      fwrite(&icount, sizeof(unsigned int),1,fph);

      printf("Font contains %d vertices, %d indices, and %d unique characters.\r\n", maxVertex, icount, ccount );

      const float *vertices = mVertices->getVerticesFloat();
      for (unsigned int i=0; i<maxVertex; i++)
      {
        fwrite(vertices,sizeof(float)*2,1,fph);
        vertices+=3;
      }

      for (int i=0; i<127; i++)
      {
        unsigned int i1 = mCharacters[i];
        unsigned int i2 = mCharacters[i+1];
        unsigned int diff = i2-i1;
        if ( diff > 0 )
        {
          unsigned char cc = (unsigned char)i;
          fwrite(&cc,sizeof(char),1,fph);
          fwrite(&diff,sizeof(unsigned short),1,fph);
          for (unsigned int i=0; i<diff; i++)
          {
            unsigned short index = (unsigned short)mIndices[i1+i];
            fwrite(&index,sizeof(unsigned short),1,fph);
          }
        }
      }
      fclose(fph);
    }
  }

  fm_VertexIndex *mVertices;
  UIntVector    mCharacters;
  UIntVector	mIndices;
  LineVector    mLines;
  float         mMinX;
  float         mMinY;
  float         mMaxX;
  float         mMaxY;
};

void main(int argc,const char **argv)
{
    HersheyBuilder b;
    b.save();
}
