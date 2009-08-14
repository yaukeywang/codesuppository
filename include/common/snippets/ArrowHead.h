#ifndef ARROW_HEAD_H

#define ARROW_HEAD_H

#include "UserMemAlloc.h"

/*!
**
** Copyright (c) 2008 by John W. Ratcliff mailto:jratcliff@infiniplex.net
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


// This is a very useful code snippet that will generate an arrow head as a set of triangles.
// The arrow head is nicely shaped, with a slightly hollowed interior which makes it easier to
// visualize when flat-shading.  By default it generates a 24 triangle arrow head at 30 degree
// increments.
//
// Consider the line segment P1 to P2.  This will generate the geometry for a nice looking arrow head
// oriented correctly at the tip of P2.  Great for indicating ray segments.
//
// Usage:
//
//           p1    - is a const NxF32 pointer to the starting position of the ray (x,y,z)
//           p2    - is a const NxF32 pointer to the ending position of the ray, this is where the arrow head will go.
//arrowHeadSize    - this is the size of the arrow head.  If this is greater than 1/3 the length of the p1 to p2, it the arrow head will be shrunk to fit the line segment.
//       tcount    - a reference to an NxU32 that will contain the number of triangles produced by the routine.
//
// returns an array of const floats in the form of x1,y1,x1, x2,y2,z2, x3,y3,z3 9 floats per triangle.

const NxF32 * createArrowHead(const NxF32 *p1,const NxF32 *p2,NxF32 arrowHeadSize,NxU32 &tcount);


// Here is an example demonstration of how to use the function:
//
//
//    NxU32 tcount;
//    const NxF32 *triangles = createArrowHead(p1,p2,1,tcount);
//    for (NxU32 i=0; i<tcount; i++)
//    {
//      debugSolidTri(triangles,triangles+3,triangles+6,0xFF0000);  // debug this triangle solid shaded in red
//      debugWireTri(triangles,triangles+3,triangles+6,0xFFFFFF);   // overlay a white wireframe outline on top of it.
//      triangles+=9;
//    }

#endif
