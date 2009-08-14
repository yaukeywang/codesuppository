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
  NxU8   materialIndex0 :  7;
  NxU8   tessFlag       :  1;
  NxU8   materialIndex1 :  7;
  NxU8   unused         :  1;
};

static  inline NxU32 getIndex(NxI32 dx,NxI32 dy,NxI32 x,NxI32 y,NxI32 wid,NxI32 hit)
{
  x+=dx;
  y+=dy;
  if ( x < 0 ) x = 0;
  if ( x >= wid ) x = wid-1;
  if ( y < 0 ) y = 0;
  if ( y >= hit ) y =hit-1;
  return (NxU32)((y*wid)+x);
}


static inline void getV(NxI32 x,NxI32 z,NxI32 wid,NxI32 hit,NxF32 rowScale,NxF32 columnScale,NxF32 heightScale,Hsample *scan,NxF32 *vertices,NxU32 &vcount)
{
  NxF32 *v = &vertices[vcount*3];

  v[0] = (NxF32) z * columnScale;
  v[2] = (NxF32) x * rowScale;

  NxU32 index = getIndex(0,0,x,z,wid,hit);

  v[1] = (NxF32) scan[index].height * heightScale;

  vcount++;
}

static inline void pushTri(NxU32 i1,NxU32 i2,NxU32 i3,NxU32 *indices,NxU32 &tcount)
{
  NxU32 *dest = &indices[tcount*3];
  dest[0] = i1;
  dest[1] = i2;
  dest[2] = i3;
  tcount++;
}

void physXHeightFieldToMesh(NxI32 wid,                           // the width of the source heightfield
                            NxI32 hit,                           // the height of the source heightfield
                            const NxU32 *heightField,            // the raw 32 bit heightfield data.
                            NxF32 verticalExtent,                // the vertical extent of the heightfield
                            NxF32 columnScale,                   // the column scale
                            NxF32 rowScale,                      // the row scale
                            NxF32 heightScale,                   // the heightscale
                            NxF32 gameScale,                     // optional scale to change from game/graphics units into physics units.  Default should be 1 to 1
                            PhysXHeightFieldInterface *callback)// your interface to receive the mesh data an an indexed triangle mesh.
{
  NxI32 stepx = (wid+(HF_STEP_SIZE-1))/HF_STEP_SIZE;
  NxI32 stepy = (hit+(HF_STEP_SIZE-1))/HF_STEP_SIZE;

  Hsample *scan = (Hsample *)heightField;

  columnScale*=gameScale;
  rowScale*=gameScale;
  heightScale*=gameScale;

  bool reverse = false;

  if ( verticalExtent > 0 )
    reverse = true;

  #define MAX_TRIANGLES ((HF_STEP_SIZE+1)*(HF_STEP_SIZE+1)*2)
  #define MAX_VERTICES ((HF_STEP_SIZE+1)*(HF_STEP_SIZE+1))


  for (NxI32 iy=0; iy<stepy; iy++)
  {
    for (NxI32 ix=0; ix<stepx; ix++)
    {
      // ok..first build the points....
      NxI32 swid = wid-(ix*HF_STEP_SIZE);
      NxI32 shit = hit-(iy*HF_STEP_SIZE);
      if ( swid > HF_STEP_SIZE ) swid = HF_STEP_SIZE;
      if ( shit > HF_STEP_SIZE ) shit = HF_STEP_SIZE;

      NxU32 vcount = 0;
      NxU32 tcount = 0;
      NxF32 vertices[MAX_VERTICES*3];
      NxU32 indices[MAX_TRIANGLES*3];

      for (NxI32 ry=0; ry<=shit; ry++)
      {
        for (NxI32 rx=0; rx<=swid; rx++)
        {
          NxI32 x = rx+(ix*HF_STEP_SIZE);
          NxI32 y = ry+(iy*HF_STEP_SIZE);
          getV(x,y,wid,hit,rowScale,columnScale,heightScale,scan,vertices,vcount);
        }
      }

      for (NxI32 ry=0; ry<shit; ry++)
      {
        for (NxI32 rx=0; rx<swid; rx++)
        {
          NxI32 x = rx+(ix*HF_STEP_SIZE);
          NxI32 y = ry+(iy*HF_STEP_SIZE);
          if ( x < (wid-1) && y < (hit-1) )
          {
            NxU32 index = (y*wid)+x;
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

            NxU32 i1,i2,i3,i4;

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

        const NxU32 *idx = indices;

        for (NxU32 i=0; i<tcount; i++)
        {
          NxU32 i1 = *idx++;
          NxU32 i2 = *idx++;
          NxU32 i3 = *idx++;
          const NxF32 *p1 = &vertices[i1*3];
          const NxF32 *p2 = &vertices[i2*3];
          const NxF32 *p3 = &vertices[i3*3];
          cm->addTriangle(p1,p2,p3);
        }

        ConsolidateMeshOutput results;

        bool ok = cm->consolidateMesh(results);
        if ( ok )
        {
          callback->receiveHeightFieldMesh(results.mVcount,results.mVerticesFloat,results.mTcount,(const NxU32 *)results.mIndices);
        }
        else
        {
          static NxI32 fcount = 0;
          fcount++;
          char scratch[512];
          sprintf(scratch,"BadMesh%d.obj", fcount );
          FILE *fph = fopen(scratch,"wb");
          if ( fph )
          {
            const NxU32 *idx = indices;

            for (NxU32 i=0; i<vcount; i++)
            {
              const NxF32 *p = &vertices[i*3];
              fprintf(fph,"v %0.9f %0.9f %0.9f\r\n", p[0], p[1], p[2] );
            }

            for (NxU32 i=0; i<tcount; i++)
            {
              NxU32 i1 = *idx++;
              NxU32 i2 = *idx++;
              NxU32 i3 = *idx++;
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

