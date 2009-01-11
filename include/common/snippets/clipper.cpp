#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

#include "clipper.h"



HeI32 FrustumClipper::ClipCode(const Vec3d &pos) const
{
  HeI32 code = 0;

  // build cohen sutherland clip codes.
  if ( pos.x < mEdges[CP_LEFT]   ) code|=(1<<CP_LEFT);
  if ( pos.x > mEdges[CP_RIGHT]  ) code|=(1<<CP_RIGHT);
  if ( pos.y < mEdges[CP_TOP]    ) code|=(1<<CP_TOP);
  if ( pos.y > mEdges[CP_BOTTOM] ) code|=(1<<CP_BOTTOM);
  if ( pos.z < mEdges[CP_NEAR]   ) code|=(1<<CP_NEAR);
  if ( pos.z > mEdges[CP_FAR]    ) code|=(1<<CP_FAR);

  return code;
}

HeI32 FrustumClipper::ClipCode(const Vec3d &pos,HeI32 &ocode,HeI32 &acode) const
{
  HeI32 code = ClipCode(pos);

  ocode|=code; // build or mask
  acode&=code; // build and mask

  return code;
}


ClipResult FrustumClipper::Clip(const Vec3d *polygon,HeU32 in_count,Vec3d *dest,HeU32 &ocount) const
{

	ocount    = 0;
  HeI32 ocode = 0;
  HeI32 acode = 0xFFFF;

  ClipVertex	list1[MAX_CLIP];
  ClipVertex  list2[MAX_CLIP];

	ClipVertex *input  = list1;
	ClipVertex *output = list2;

  for (HeU32 i=0; i<in_count; i++)
  {
  	input[i].Set( polygon[i], ClipCode(polygon[i], ocode, acode) );
  }

  if ( acode ) return CR_OUTSIDE; // points lie completely outside the frustum, no intersection of any kind

  if ( !ocode )
  {
  	dest[0] = polygon[0];
  	dest[1] = polygon[1];
  	dest[2] = polygon[2];
  	ocount = 3;
    return CR_INSIDE; // completely inside!
  }

  // ok..need to clip it!!

  HeU32 l;
  l = CP_LAST;

  for (HeU32 i=0; i<l; i++)
  {
    HeU32 mask = (1<<i); // this is the clip mask.
    if ( ocode & mask ) // if any vertices are clipped against this plane
    {
      ocount    = 0;
      HeI32 new_ocode = 0;
      HeI32 new_acode = 0xFFFF;
      for (HeU32 j=0; j<in_count; j++)
      {
        HeU32 k = j+1;
        if ( k == in_count ) k = 0;
        ClipVertex &v1 = input[j];
        ClipVertex &v2 = input[k];
        // if this vertice is coming into or exiting out from this plane
        if ( (v1.GetClipCode() ^ v2.GetClipCode() ) & mask )
        {
          ClipVertex v0(v1,v2,(ClipPlane)i,mEdges[i]);
          HeI32 code = ClipCode(v0.GetPos(),new_ocode,new_acode);
          v0.SetClipCode(code);
					output[ocount] = v0;
					ocount++;
        }
        if ( ! (v2.GetClipCode() & mask ) )
        {
        	output[ocount] = v2;
        	ocount++;
          new_acode&=v2.GetClipCode();
        }
      }

      // Result of clipping produced no valid polygon *or* clipped result
      // is completely outside the frustum.
      if ( ocount < 3 || new_acode )
      {
      	ocount = 0;
        return CR_OUTSIDE;
      }

	    ClipVertex *temp = input;
			input = output;
			output = temp;
			in_count = ocount;
    }
  }

	for (HeU32 i=0; i<in_count; i++)
 	{
 		dest[i] = input[i].mPos;
 	}

 	ocount = in_count;

  return CR_PARTIAL;
}


// clip vertex between v1 and v2 on this plane..
ClipVertex::ClipVertex(const ClipVertex &v1,
                       const ClipVertex &v2,
                       ClipPlane p,
                       HeF32 edge)  // the clipping boundary..
{
  HeF32 slope;

  switch ( p )
  {
    case CP_LEFT:
    case CP_RIGHT:
      slope   = (edge - v1.GetX() ) / (v2.GetX() - v1.GetX() );
      mPos.x = edge;
      mPos.y = v1.GetY() + ((v2.GetY() - v1.GetY()) * slope);
      mPos.z = v1.GetZ() + ((v2.GetZ() - v1.GetZ()) * slope);
      break;
    case CP_TOP:
    case CP_BOTTOM:
      slope   = (edge - v1.GetY() ) / (v2.GetY() - v1.GetY() );
      mPos.y = edge;
      mPos.x = v1.GetX() + ((v2.GetX() - v1.GetX()) * slope);
      mPos.z = v1.GetZ() + ((v2.GetZ() - v1.GetZ()) * slope);
      break;
    case CP_NEAR:
    case CP_FAR:
      slope   = (edge - v1.GetZ() ) / (v2.GetZ() - v1.GetZ() );
      mPos.z = edge;
      mPos.x = v1.GetX() + ((v2.GetX() - v1.GetX()) * slope);
      mPos.y = v1.GetY() + ((v2.GetY() - v1.GetY()) * slope);
      break;
    case CP_LAST:
    default:
      // Do nothing.
      break;
  }
}


ClipResult FrustumClipper::ClipRay(const Vec3d &r1a,
                                   const Vec3d &r2a,
                                   Vec3d &r1b,
                                   Vec3d &r2b)
{
  HeI32 ocode = 0;
  HeI32 acode = 0xFFFF;

  ClipVertex ray1( r1a, ClipCode( r1a, ocode, acode ) );
  ClipVertex ray2( r2a, ClipCode( r2a, ocode, acode ) );

  if ( acode ) return CR_OUTSIDE; // points lie completely outside the frustum, no intersection of any kind

  if ( !ocode )
  {
    r1b = r1a;
    r2b = r2a;
    return CR_INSIDE; // completely inside!
  }


  HeI32 l;
  l = CP_LAST;

  for (HeI32 i=0; i<l; i++)
  {

    HeI32 mask = (1<<i); // this is the clip mask.

    if ( ocode & mask ) // if any vertices are clipped against this plane
    {
      // if this RAY is coming into or exiting out from this plane
      HeI32 new_acode = 0xFFFF;
      HeI32 new_ocode = 0;

      if ( (ray1.GetClipCode() ^ ray2.GetClipCode() ) & mask )
      {

        ClipVertex v0(ray1,ray2,(ClipPlane)i,mEdges[i]);
        HeI32 code = ClipCode(v0.GetPos(),new_ocode,new_acode);
        v0.SetClipCode(code);

        if ( ray1.GetClipCode() & mask )
        {
          ray1 = v0;
          new_acode&=ray2.GetClipCode();
        }
        else
        {
          ray2 = v0;
          new_acode&=ray1.GetClipCode();
        }

        if ( new_acode ) return CR_OUTSIDE;

      }
    }
  }


  r1b = ray1.GetPos();
  r2b = ray2.GetPos();

  return CR_PARTIAL;


}
