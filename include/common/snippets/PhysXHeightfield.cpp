#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma warning(disable:4996)

#define CONSOLIDATE 0

#include "PhysXHeightfield.h"

#if CONSOLIDATE
#include "ConsolidateMesh.h"
#endif

#define HOLE 2

struct Hsample
{
  short  height         : 16;
  unsigned char   materialIndex0 :  7;
  unsigned char   tessFlag       :  1;
  unsigned char   materialIndex1 :  7;
  unsigned char   unused         :  1;
};

static  inline unsigned int getIndex(int dx,int dy,int x,int y,int wid,int hit)
{
  x+=dx;
  y+=dy;
  if ( x < 0 ) x = 0;
  if ( x >= wid ) x = wid-1;
  if ( y < 0 ) y = 0;
  if ( y >= hit ) y =hit-1;
  return (unsigned int)((y*wid)+x);
}


static inline void getV(int x,int z,int wid,int hit,float rowScale,float columnScale,float heightScale,Hsample *scan,float *vertices,unsigned int &vcount)
{
  float *v = &vertices[vcount*3];

  v[0] = (float) z * columnScale;
  v[2] = (float) x * rowScale;

  unsigned int index = getIndex(0,0,x,z,wid,hit);

  v[1] = (float) scan[index].height * heightScale;

  vcount++;
}

static inline void pushTri(unsigned int i1,unsigned int i2,unsigned int i3,unsigned int *indices,unsigned int &tcount)
{
  unsigned int *dest = &indices[tcount*3];
  dest[0] = i1;
  dest[1] = i2;
  dest[2] = i3;
  tcount++;
}

void physXHeightFieldToMesh(int wid,                           // the width of the source heightfield
                            int hit,                           // the height of the source heightfield
                            const unsigned int *heightField,            // the raw 32 bit heightfield data.
                            float verticalExtent,                // the vertical extent of the heightfield
                            float columnScale,                   // the column scale
                            float rowScale,                      // the row scale
                            float heightScale,                   // the heightscale
                            float gameScale,                     // optional scale to change from game/graphics units into physics units.  Default should be 1 to 1
                            PhysXHeightFieldInterface *callback)// your interface to receive the mesh data an an indexed triangle mesh.
{
  int stepx = (wid+(HF_STEP_SIZE-1))/HF_STEP_SIZE;
  int stepy = (hit+(HF_STEP_SIZE-1))/HF_STEP_SIZE;

  Hsample *scan = (Hsample *)heightField;

  columnScale*=gameScale;
  rowScale*=gameScale;
  heightScale*=gameScale;

  bool reverse = false;

  if ( verticalExtent > 0 )
    reverse = true;

  #define MAX_TRIANGLES ((HF_STEP_SIZE+1)*(HF_STEP_SIZE+1)*2)
  #define MAX_VERTICES ((HF_STEP_SIZE+1)*(HF_STEP_SIZE+1))


  for (int iy=0; iy<stepy; iy++)
  {
    for (int ix=0; ix<stepx; ix++)
    {
      // ok..first build the points....
      int swid = wid-(ix*HF_STEP_SIZE);
      int shit = hit-(iy*HF_STEP_SIZE);
      if ( swid > HF_STEP_SIZE ) swid = HF_STEP_SIZE;
      if ( shit > HF_STEP_SIZE ) shit = HF_STEP_SIZE;

      unsigned int vcount = 0;
      unsigned int tcount = 0;
      float vertices[MAX_VERTICES*3];
      unsigned int indices[MAX_TRIANGLES*3];

      for (int ry=0; ry<=shit; ry++)
      {
        for (int rx=0; rx<=swid; rx++)
        {
          int x = rx+(ix*HF_STEP_SIZE);
          int y = ry+(iy*HF_STEP_SIZE);
          getV(x,y,wid,hit,rowScale,columnScale,heightScale,scan,vertices,vcount);
        }
      }

      for (int ry=0; ry<shit; ry++)
      {
        for (int rx=0; rx<swid; rx++)
        {
          int x = rx+(ix*HF_STEP_SIZE);
          int y = ry+(iy*HF_STEP_SIZE);
          if ( x < (wid-1) && y < (hit-1) )
          {
            unsigned int index = (y*wid)+x;
            const Hsample *sample = (const Hsample *) &heightField[index];
            bool ok1 = true;
            bool ok2 = true;

            if ( sample->materialIndex0 == HOLE || sample->materialIndex1 == HOLE )
            {
              if ( sample->materialIndex0 == HOLE && sample->materialIndex1 == HOLE )
                continue;
              if ( sample->materialIndex0 == HOLE )
                ok2 = false;
              else
                ok1 = false;
            }

            bool flip = reverse;

            unsigned int i1,i2,i3,i4;

            if ( sample->tessFlag )
            {
              i1 = getIndex(0,0,rx,ry,swid+1,shit+1);
              i2 = getIndex(1,0,rx,ry,swid+1,shit+1);
              i3 = getIndex(1,1,rx,ry,swid+1,shit+1);
              i4 = getIndex(0,1,rx,ry,swid+1,shit+1);
            }
            else
            {
              if ( flip )
                flip = false;
              else
                flip = true;
              i1 = getIndex(0,1,rx,ry,swid+1,shit+1);
              i2 = getIndex(1,1,rx,ry,swid+1,shit+1);
              i3 = getIndex(1,0,rx,ry,swid+1,shit+1);
              i4 = getIndex(0,0,rx,ry,swid+1,shit+1);
            }

            if ( flip )
            {
              if ( ok1 ) pushTri(i3,i2,i1,indices,tcount);
              if ( ok2 ) pushTri(i4,i3,i1,indices,tcount);
            }
            else
            {
              if ( ok1 ) pushTri(i1,i2,i3,indices,tcount);
              if ( ok2 ) pushTri(i1,i3,i4,indices,tcount);
            }
          }
        }
      }

      if ( tcount )
      {
        #if CONSOLIDATE
        ConsolidateMesh *cm = createConsolidateMesh();

        const unsigned int *idx = indices;

        for (unsigned int i=0; i<tcount; i++)
        {
          unsigned int i1 = *idx++;
          unsigned int i2 = *idx++;
          unsigned int i3 = *idx++;
          const float *p1 = &vertices[i1*3];
          const float *p2 = &vertices[i2*3];
          const float *p3 = &vertices[i3*3];
          cm->addTriangle(p1,p2,p3);
        }

        ConsolidateMeshOutput results;

        bool ok = cm->consolidateMesh(results);
        if ( ok )
        {
          callback->receiveHeightFieldMesh(results.mVcount,results.mVerticesFloat,results.mTcount,(const unsigned int *)results.mIndices);
        }
        else
        {
          static int fcount = 0;
          fcount++;
          char scratch[512];
          sprintf(scratch,"BadMesh%d.obj", fcount );
          FILE *fph = fopen(scratch,"wb");
          if ( fph )
          {
            const unsigned int *idx = indices;

            for (unsigned int i=0; i<vcount; i++)
            {
              const float *p = &vertices[i*3];
              fprintf(fph,"v %0.9f %0.9f %0.9f\r\n", p[0], p[1], p[2] );
            }

            for (unsigned int i=0; i<tcount; i++)
            {
              unsigned int i1 = *idx++;
              unsigned int i2 = *idx++;
              unsigned int i3 = *idx++;
              fprintf(fph,"f %d %d %d\r\n", i1+1, i2+1, i3+1 );
            }
            fclose(fph);
          }
          callback->receiveHeightFieldMesh(vcount,vertices,tcount,indices);
        }

        releaseConsolidateMesh(cm);
        #else
        callback->receiveHeightFieldMesh(vcount,vertices,tcount,indices);
        #endif
      }

    }
  }
}

