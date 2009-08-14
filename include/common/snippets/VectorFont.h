#ifndef VECTOR_FONT_H

#define VECTOR_FONT_H

#include "UserMemAlloc.h"

/*!
**
** Copyright (c) 2009 by John W. Ratcliff mailto:jratcliffscarab@gmail.com
**
** If you wish to contact me you can use the following methods:
**
** email: jratcliffscarab@gmail.com
** Personal website: http://jratcliffscarab.blogspot.com
** Coding Website:   http://codesuppository.blogspot.com
** FundRaising Blog: http://amillionpixels.blogspot.com
** Fundraising site: http://www.amillionpixels.us
** New Temple Site:  http://newtemple.blogspot.com
**
**
** The MIT license:
**
** Permission is hereby granted, freeof charge, to any person obtaining a copy
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

// This code snippet allows you to render a high quality vector line font.
// The font it uses is called the 'Hershey Font' (search for it on Google).
// This font is completely public domain and you are free to use it in
// your own applications.
//
//
// Here is how you use this code snippet.
//
// Step #1 : Create an instance of the VectorFont pure virtual interface.
// Step #2 : call 'vprintf' with the transform, scale, center flag, and text you wish to print.
// Step #3 : Dispatch individual line segments to your own rendering framework.
// Step #4 : Release the VectorFont class.
//
// Here is an example usage in C++
//
// class MyLineDraw : public VectorFontInterfac
// {
//     public:
//      virtual void drawLine(NxF32 x1,NxF32 y1,NxF32 x2,NxF32 y2)
//      {
//        // Right here, in this callback, draw the line using your own line draw interface, OpenGL, D3D, or whatever works for you.
//      }
//    };
//
//
//    MyLineDraw mld; // create an instance of our callback class to draw individual line segments.
//
//    VectorFont *vf = createVectorFont(&mld);
//    vf->vprintf(NULL,1,true,"This is a test of the emergency broadcasting system.");
//    releaseVectorFont(vf);


class VectorFontInterface
{
public:
  virtual void drawLine(NxF32 x1,NxF32 y1,NxF32 x2,NxF32 y2) = 0;
};

class VectorFont
{
public:
  virtual void vprintf(const NxF32 *transform, // an *optional* 4x4 matrix transform to apply to each point as the font is rendered.
                       NxF32 textScale,        // A scaling value to apply to the vertices of the font.
                       bool centered,          // True if you want the text centered relative to its 3space position, specified in the translation component of 'transform' or 0,0,0 if no transform provided.
                       const char *fmt,        // A printf style format string
                       ...) = 0;               // Optional additional parameters based on the format spec.
};


VectorFont * createVectorFont(VectorFontInterface *vf);
void         releaseVectorFont(VectorFont *vf);

#endif
