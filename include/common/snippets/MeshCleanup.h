#ifndef MESH_CLEANUP_H

#define MESH_CLEANUP_H

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

#include "UserMemAlloc.h"

namespace RENDER_DEBUG
{
  class RenderDebug;
};

namespace MESH_CLEANUP
{

class MeshCleanupDesc
{
public:
  MeshCleanupDesc(void)
  {
    inputTcount = 0;
    inputTstride = 0;
    inputIndices = 0;
    inputVcount = 0;
    inputVertices = 0;
    inputVstride = 0;
    outputTcount = 0;
    outputVcount = 0;
    outputIndices = 0;
    outputVertices = 0;
    degenerateCount = 0;
    duplicateCount = 0;
    doubleSidedCount = 0;
  }

  NxU32  inputTcount;
  NxU32  inputTstride;
  const NxU32 *inputIndices;

  NxU32  inputVcount;
  const NxF32   *inputVertices;
  NxU32  inputVstride;

  NxU32  outputTcount;
  NxU32  outputVcount;
  NxU32 *outputIndices;
  NxF32         *outputVertices;

  NxU32   degenerateCount;
  NxU32   duplicateCount;
  NxU32   doubleSidedCount;


};

// searches for double-sided triangles, and either removes them or
// extrudes them.

bool meshCleanup(MeshCleanupDesc &desc,NxF32 weldDistance=0.001f,NxF32 projectDistance=0.01f,RENDER_DEBUG::RenderDebug *debug=0);
void releaseMeshCleanup(MeshCleanupDesc &desc);

};

#endif
