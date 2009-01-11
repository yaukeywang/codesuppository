#ifndef LOOK_AT_H

#define LOOK_AT_H

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

// CodeSnippet provided by John W. Ratcliff
// on April 3, 2006.
//
// mailto: jratcliff@infiniplex.net
//

//
// This snippet shows how to build a projection matrix
// a view matrix and a matrix that orientats an object
// relative to an origin and lookat location.
//
// This is a reference implementation that you should be able to use
// with virtually any vector or matrix class (so long as the matrices are (4x4)
// and your vector is 3 floats.  And who has a vector class that isn't three floats X,Y,Z ?
//
// The first one is a helpful routine to get an object to world transform if you know the origin
// of an object and some place it is 'looking'.  You must specify the 'up vector' for that object.
//
// I used this helpful routine when I wanted to place a bunch of dominos in a spiral.  Any time
// you have two points and you just want an object to 'face that way' you can use
// 'computeLookAt'
//
// computeView builds a view matrix from an eye position to a look at location with a
// reference 'up vector'.  It stores the result in a 4x4 matrix that use pass by address.
//
// The last routine computes a projection matrix.
//
// You can either use these three routines simply as a reference implementation or
// you can build your own camera classes out of them.  Maybe you don't even think they are
// useful, I don't know.  I, personally, like being able to use them independent of any
// vector or matrix classes.  I will be using them for the demo of the axis-aligned bounding
// volume tree code I will be releasing in a day or so.

#include "common/snippets/UserMemAlloc.h"


void computeLookAt(const HeF32 *eye,const HeF32 *look,const HeF32 *upVector,HeF32 *matrix);
void computeView(const HeF32 *eye,const HeF32 *look,const HeF32 *upVector,HeF32 *matrix);
void computeProjection(HeF32 fov,HeF32 aspect,HeF32 nearPlane,HeF32 farPlane,HeF32 *matrix);

#endif
