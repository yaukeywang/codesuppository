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

#include "common/snippets/UserMemAlloc.h"
#include "renderdebug/renderdebug.h"
#include "common/snippets/pool.h"
#include "common/snippets/FloatMath.h"

#pragma warning(disable:4996)

#if USE_PD3D
#include "Pd3d/pd3d.h"
PD3D::Pd3d *gPd3d=0;
#else
#include "HeGrDriver/HeGrDriver.h"
#include "HeGrDriver/HeGrShader.h"
#include "HeTextureManager/HeTextureManager.h"
using namespace HEGRDRIVER;
using namespace HETEXTUREMANAGER;
#endif

#ifndef PLUGINS_EMBEDDED
SendTextMessage *gSendTextMessage=0;
JOB_SWARM::JobSwarmContext *gJobSwarmContext=0;
#endif

bool doShutdown(void);

namespace RENDER_DEBUG
{

class InternalRenderDebug;


void DejaDescriptor(const InternalRenderDebug &obj);


#define MAXWIRETRI 10000000
#define MAX_WIRETRI_START 1000
#define MAX_WIRETRI_GROW  1000

#define MAXLINEPOINTS  4096
#define MAXSOLIDPOINTS 4096

class SolidVertex
{
public:
  HeF32	       mPos[3];
  HeF32        mNormal[3];
  HeU32 mColor;
};

class DebugVertex
{
public:

	HeF32        mPos[3];
	HeU32 mColor;
};


RenderDebug *gRenderDebug=0;

const HeF32 debug_cylinder[32*9] =
{
0.0000f, 0.0000f, 0.0000f,    0.7071f, 0.7071f, 0.0000f,  1.0000f, 0.0000f, 0.0000f,
0.0000f, 0.0000f, 0.0000f,    0.0000f, 1.0000f, 0.0000f,  0.7071f, 0.7071f, 0.0000f,
0.0000f, 0.0000f, 0.0000f,    -0.7071f, 0.7071f, 0.0000f,  0.0000f, 1.0000f, 0.0000f,
0.0000f, 0.0000f, 0.0000f,    -1.0000f, 0.0000f, 0.0000f,  -0.7071f, 0.7071f, 0.0000f,
0.0000f, 0.0000f, 0.0000f,    -0.7071f, -0.7071f, 0.0000f,  -1.0000f, 0.0000f, 0.0000f,
0.0000f, 0.0000f, 0.0000f,    0.0000f, -1.0000f, 0.0000f,  -0.7071f, -0.7071f, 0.0000f,
0.0000f, 0.0000f, 0.0000f,    0.7071f, -0.7071f, 0.0000f,  0.0000f, -1.0000f, 0.0000f,
0.0000f, 0.0000f, 0.0000f,    1.0000f, 0.0000f, 0.0000f,  0.7071f, -0.7071f, 0.0000f,
1.0000f, 0.0000f, 0.0000f,    0.7071f, 0.7071f, 1.0000f,  1.0000f, 0.0000f, 1.0000f,
1.0000f, 0.0000f, 0.0000f,    0.7071f, 0.7071f, 0.0000f,  0.7071f, 0.7071f, 1.0000f,
0.7071f, 0.7071f, 0.0000f,    0.0000f, 1.0000f, 1.0000f,  0.7071f, 0.7071f, 1.0000f,
0.7071f, 0.7071f, 0.0000f,    0.0000f, 1.0000f, 0.0000f,  0.0000f, 1.0000f, 1.0000f,
0.0000f, 1.0000f, 0.0000f,    -0.7071f, 0.7071f, 1.0000f,  0.0000f, 1.0000f, 1.0000f,
0.0000f, 1.0000f, 0.0000f,    -0.7071f, 0.7071f, 0.0000f,  -0.7071f, 0.7071f, 1.0000f,
-0.7071f, 0.7071f, 0.0000f,    -1.0000f, 0.0000f, 1.0000f,  -0.7071f, 0.7071f, 1.0000f,
-0.7071f, 0.7071f, 0.0000f,    -1.0000f, 0.0000f, 0.0000f,  -1.0000f, 0.0000f, 1.0000f,
-1.0000f, 0.0000f, 0.0000f,    -0.7071f, -0.7071f, 1.0000f,  -1.0000f, 0.0000f, 1.0000f,
-1.0000f, 0.0000f, 0.0000f,    -0.7071f, -0.7071f, 0.0000f,  -0.7071f, -0.7071f, 1.0000f,
-0.7071f, -0.7071f, 0.0000f,    0.0000f, -1.0000f, 1.0000f,  -0.7071f, -0.7071f, 1.0000f,
-0.7071f, -0.7071f, 0.0000f,    0.0000f, -1.0000f, 0.0000f,  0.0000f, -1.0000f, 1.0000f,
0.0000f, -1.0000f, 0.0000f,    0.7071f, -0.7071f, 1.0000f,  0.0000f, -1.0000f, 1.0000f,
0.0000f, -1.0000f, 0.0000f,    0.7071f, -0.7071f, 0.0000f,  0.7071f, -0.7071f, 1.0000f,
 0.7071f, -0.7071f, 0.0000f,    1.0000f, 0.0000f, 1.0000f,  0.7071f, -0.7071f, 1.0000f,
0.7071f, -0.7071f, 0.0000f,    1.0000f, 0.0000f, 0.0000f,  1.0000f, 0.0000f, 1.0000f,
0.0000f, 0.0000f, 1.0000f,    1.0000f, 0.0000f, 1.0000f,  0.7071f, 0.7071f, 1.0000f,
0.0000f, 0.0000f, 1.0000f,    0.7071f, 0.7071f, 1.0000f,  0.0000f, 1.0000f, 1.0000f,
0.0000f, 0.0000f, 1.0000f,    0.0000f, 1.0000f, 1.0000f,  -0.7071f, 0.7071f, 1.0000f,
0.0000f, 0.0000f, 1.0000f,    -0.7071f, 0.7071f, 1.0000f,  -1.0000f, 0.0000f, 1.0000f,
0.0000f, 0.0000f, 1.0000f,    -1.0000f, 0.0000f, 1.0000f,  -0.7071f, -0.7071f, 1.0000f,
0.0000f, 0.0000f, 1.0000f,    -0.7071f, -0.7071f, 1.0000f,  0.0000f, -1.0000f, 1.0000f,
0.0000f, 0.0000f, 1.0000f,    0.0000f, -1.0000f, 1.0000f,  0.7071f, -0.7071f, 1.0000f,
0.0000f, 0.0000f, 1.0000f,    0.7071f, -0.7071f, 1.0000f,  1.0000f, 0.0000f, 1.0000f
};

const HeF32 debug_sphere[32*9] =
{
 0.0000f, 0.0000f, 1.0000f,    0.7071f, 0.0000f, 0.7071f,  0.0000f, 0.7071f, 0.7071f,
 0.7071f, 0.0000f, 0.7071f,    1.0000f, 0.0000f, 0.0000f,  0.7071f, 0.7071f, 0.0000f,
 0.7071f, 0.0000f, 0.7071f,    0.7071f, 0.7071f, 0.0000f,  0.0000f, 0.7071f, 0.7071f,
 0.0000f, 0.7071f, 0.7071f,    0.7071f, 0.7071f, 0.0000f,  0.0000f, 1.0000f, 0.0000f,
 0.0000f, 0.0000f, 1.0000f,    0.0000f, 0.7071f, 0.7071f,  -0.7071f, 0.0000f, 0.7071f,
 0.0000f, 0.7071f, 0.7071f,    0.0000f, 1.0000f, 0.0000f,  -0.7071f, 0.7071f, 0.0000f,
 0.0000f, 0.7071f, 0.7071f,    -0.7071f, 0.7071f, 0.0000f,  -0.7071f, 0.0000f, 0.7071f,
 -0.7071f, 0.0000f, 0.7071f,    -0.7071f, 0.7071f, 0.0000f,  -1.0000f, 0.0000f, 0.0000f,
 0.0000f, 0.0000f, 1.0000f,    -0.7071f, 0.0000f, 0.7071f,  0.0000f, -0.7071f, 0.7071f,
 -0.7071f, 0.0000f, 0.7071f,    -1.0000f, 0.0000f, 0.0000f,  -0.7071f, -0.7071f, 0.0000f,
 -0.7071f, 0.0000f, 0.7071f,    -0.7071f, -0.7071f, 0.0000f,  0.0000f, -0.7071f, 0.7071f,
 0.0000f, -0.7071f, 0.7071f,    -0.7071f, -0.7071f, 0.0000f,  0.0000f, -1.0000f, 0.0000f,
 0.0000f, 0.0000f, 1.0000f,    0.0000f, -0.7071f, 0.7071f,  0.7071f, 0.0000f, 0.7071f,
 0.0000f, -0.7071f, 0.7071f,    0.0000f, -1.0000f, 0.0000f,  0.7071f, -0.7071f, 0.0000f,
 0.0000f, -0.7071f, 0.7071f,    0.7071f, -0.7071f, 0.0000f,  0.7071f, 0.0000f, 0.7071f,
 0.7071f, 0.0000f, 0.7071f,    0.7071f, -0.7071f, 0.0000f,  1.0000f, 0.0000f, 0.0000f,
 0.0000f, 0.0000f, -1.0000f,    0.0000f, 0.7071f, -0.7071f,  0.7071f, 0.0000f, -0.7071f,
 0.0000f, 0.7071f, -0.7071f,    0.0000f, 1.0000f, 0.0000f,  0.7071f, 0.7071f, 0.0000f,
 0.0000f, 0.7071f, -0.7071f,    0.7071f, 0.7071f, 0.0000f,  0.7071f, 0.0000f, -0.7071f,
 0.7071f, 0.0000f, -0.7071f,    0.7071f, 0.7071f, 0.0000f,  1.0000f, 0.0000f, 0.0000f,
 0.0000f, 0.0000f, -1.0000f,    -0.7071f, 0.0000f, -0.7071f,  0.0000f, 0.7071f, -0.7071f,
 -0.7071f, 0.0000f, -0.7071f,    -1.0000f, 0.0000f, 0.0000f,  -0.7071f, 0.7071f, 0.0000f,
 -0.7071f, 0.0000f, -0.7071f,    -0.7071f, 0.7071f, 0.0000f,  0.0000f, 0.7071f, -0.7071f,
 0.0000f, 0.7071f, -0.7071f,    -0.7071f, 0.7071f, 0.0000f,  0.0000f, 1.0000f, 0.0000f,
 0.0000f, 0.0000f, -1.0000f,    0.0000f, -0.7071f, -0.7071f,  -0.7071f, 0.0000f, -0.7071f,
 0.0000f, -0.7071f, -0.7071f,    0.0000f, -1.0000f, 0.0000f,  -0.7071f, -0.7071f, 0.0000f,
 0.0000f, -0.7071f, -0.7071f,    -0.7071f, -0.7071f, 0.0000f,  -0.7071f, 0.0000f, -0.7071f,
 -0.7071f, 0.0000f, -0.7071f,    -0.7071f, -0.7071f, 0.0000f,  -1.0000f, 0.0000f, 0.0000f,
 0.0000f, 0.0000f, -1.0000f,    0.7071f, 0.0000f, -0.7071f,  0.0000f, -0.7071f, -0.7071f,
 0.7071f, 0.0000f, -0.7071f,    1.0000f, 0.0000f, 0.0000f,  0.7071f, -0.7071f, 0.0000f,
 0.7071f, 0.0000f, -0.7071f,    0.7071f, -0.7071f, 0.0000f,  0.0000f, -0.7071f, -0.7071f,
 0.0000f, -0.7071f, -0.7071f,    0.7071f, -0.7071f, 0.0000f,  0.0000f, -1.0000f, 0.0000f,
};

const HeF32 debug_halfsphere[16*9] =
{
 0.0000f, 0.0000f, 1.0000f,    0.7071f, 0.0000f, 0.7071f,  0.0000f, 0.7071f, 0.7071f,
 0.7071f, 0.0000f, 0.7071f,    1.0000f, 0.0000f, 0.0000f,  0.7071f, 0.7071f, 0.0000f,
 0.7071f, 0.0000f, 0.7071f,    0.7071f, 0.7071f, 0.0000f,  0.0000f, 0.7071f, 0.7071f,
 0.0000f, 0.7071f, 0.7071f,    0.7071f, 0.7071f, 0.0000f,  0.0000f, 1.0000f, 0.0000f,
 0.0000f, 0.0000f, 1.0000f,    0.0000f, 0.7071f, 0.7071f,  -0.7071f, 0.0000f, 0.7071f,
 0.0000f, 0.7071f, 0.7071f,    0.0000f, 1.0000f, 0.0000f,  -0.7071f, 0.7071f, 0.0000f,
 0.0000f, 0.7071f, 0.7071f,    -0.7071f, 0.7071f, 0.0000f,  -0.7071f, 0.0000f, 0.7071f,
 -0.7071f, 0.0000f, 0.7071f,    -0.7071f, 0.7071f, 0.0000f,  -1.0000f, 0.0000f, 0.0000f,
};

const HeF32 debug_point[3*6] =
{
	-1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
	 0.0f,-1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
	 0.0f, 0.0f,-1.0f,  0.0f, 0.0f, 1.0f
};

const char line_shader[] =
"uniform float4x4 ModelToProj;\n"
"struct SVertex\n"
"{\n"
"  float3  position  : POSITION;\n"
"  float4  color     : COLOR;\n"
"};\n"
"struct SVarying\n"
"{\n"
"  float4 position         : POSITION;\n"
"  float4 color            : TEXCOORD1;\n"
"};\n"
"struct SFragment\n"
"{\n"
"  float4 color : COLOR;\n"
"};\n"
"SVarying VSMain( SVertex input )\n"
"{\n"
"  SVarying output;\n"
"  output.position = mul( float4( input.position, 1.0f ), ModelToProj );\n"
"  output.color = input.color;\n"
"  return output;\n"
"}\n"
"SFragment FSMain( SVarying input )\n"
"{\n"
"  SFragment output;\n"
"  output.color = input.color;\n"
"  return output;\n"
"}\n"
"technique t0\n"
"{\n"
"  pass p0\n"
"  {\n"
"    VertexShader = compile vsx VSMain();\n"
"    PixelShader = compile psx FSMain();\n"
"  }\n"
"}\n";

const char solid_shader[] =
"uniform float4x4 ModelToProj;\n"
"uniform float3 LocalCameraPos;\n"
"struct SVertex\n"
"{\n"
"  float3 position : POSITION;\n"
"  float3 normal   : NORMAL;\n"
"  float4 color    : COLOR;\n"
"};\n"
"struct SVarying\n"
"{\n"
"  float4 position         : POSITION;\n"
"  float4 color            : TEXCOORD1;\n"
"  float3 normal           : TEXCOORD2;\n"
"  float3 localEyePos      : TEXCOORD3;\n"
"};\n"
"struct SFragment\n"
"{\n"
"  float4 color : COLOR;\n"
"};\n"
"SVarying VSMain( SVertex input )\n"
"{\n"
"  SVarying output;\n"
"  output.position = mul( float4( input.position, 1.0f ), ModelToProj );\n"
"  output.color = input.color;\n"
"  output.normal = input.normal;\n"
"  output.localEyePos = LocalCameraPos - input.position;\n"
"  return output;\n"
"}\n"
"SFragment FSMain( SVarying input )\n"
"{\n"
"  SFragment output;\n"
"  float3 lightDir = normalize( float3( 0.0f, 1.0f, 0.2f ) );\n"
"  float diffuse = dot( input.normal, lightDir )+0.3;\n"
"  float4 color = diffuse * input.color;\n"
"  float specular = dot( input.normal, normalize( input.localEyePos + lightDir ) );\n"
"  specular = pow( specular, 20.0f );\n"
"  specular *= step( 0.0, diffuse );\n"
"  color.rgb += specular.rrr;\n"
"  output.color = color;\n"
"  return output;\n"
"}\n"
"technique t0\n"
"{\n"
"  pass p0\n"
"  {\n"
"    VertexShader = compile vsx VSMain();\n"
"    PixelShader = compile psx FSMain();\n"
"  }\n"
"}\n";


const char white_shader[] =
"uniform float4x4 ModelToProj;\n"
"uniform texture Tex;\n"
"sampler s0 = sampler_state\n"
"{\n"
"  texture = <Tex>;\n"
"  mipfilter = linear;\n"
"  minfilter = linear;\n"
"  magfilter = linear;\n"
"};\n"
"struct SVertex\n"
"{\n"
"  float3 position : POSITION;\n"
"  float3 normal   : NORMAL;\n"
"  float2 uv       : TEXCOORD0;\n"
"};\n"
"struct SVarying\n"
"{\n"
"  float4 position         : POSITION;\n"
"  float2 uv               : TEXCOORD1;\n"
"  float3 normal           : TEXCOORD2;\n"
"};\n"
"struct SFragment\n"
"{\n"
"  float4 color : COLOR;\n"
"};\n"
"SVarying VSMain( SVertex input )\n"
"{\n"
"  SVarying output;\n"
"  output.position = mul( float4( input.position, 1.0f ), ModelToProj );\n"
"  output.uv = input.uv;\n"
"  output.normal = input.normal;\n"
"  return output;\n"
"}\n"
"SFragment FSMain( SVarying input )\n"
"{\n"
"  SFragment output;\n"
"  output.color.xyzw = 1;\n"
"  return output;\n"
"}\n"
"technique t0\n"
"{\n"
"  pass p0\n"
"  {\n"
"    VertexShader = compile vsx VSMain();\n"
"    PixelShader = compile psx FSMain();\n"
"  }\n"
"}\n";


const char texture_shader[] =
"uniform float4x4 ModelToProj;\n"
"uniform float3 LocalCameraPos;\n"
"uniform texture Tex;\n"
"sampler s0 = sampler_state\n"
"{\n"
"  texture = <Tex>;\n"
"  mipfilter = linear;\n"
"  minfilter = linear;\n"
"  magfilter = linear;\n"
"};\n"
"struct SVertex\n"
"{\n"
"  float3 position : POSITION;\n"
"  float3 normal   : NORMAL;\n"
"  float2 uv       : TEXCOORD0;\n"
"};\n"
"struct SVarying\n"
"{\n"
"  float4 position         : POSITION;\n"
"  float2 uv               : TEXCOORD1;\n"
"  float3 normal           : TEXCOORD2;\n"
"  float3 localEyePos      : TEXCOORD3;\n"
"};\n"
"struct SFragment\n"
"{\n"
"  float4 color : COLOR;\n"
"};\n"
"SVarying VSMain( SVertex input )\n"
"{\n"
"  SVarying output;\n"
"  output.position = mul( float4( input.position, 1.0f ), ModelToProj );\n"
"  output.uv = input.uv;\n"
"  output.normal = input.normal;\n"
"  output.localEyePos = LocalCameraPos - input.position;\n"
"  return output;\n"
"}\n"
"SFragment FSMain( SVarying input )\n"
"{\n"
"  SFragment output;\n"
"  float3 lightDir = normalize( float3( 0.0f, 1.0f, 0.2f ) );\n"
"  float ambient = 0.3;\n"
"  float diffuse = dot( input.normal, lightDir );\n"
"  float specular = dot( input.normal, normalize( input.localEyePos + lightDir ) );\n"
"  specular = pow( specular, 20.0f );\n"
"  specular *= step( 0.0, diffuse );\n"
"  float4 color = tex2D( s0, input.uv );\n"
"  color.rgb *= specular + diffuse + ambient;\n"
"  output.color = color;\n"
"  return output;\n"
"}\n"
"technique t0\n"
"{\n"
"  pass p0\n"
"  {\n"
"    VertexShader = compile vsx VSMain();\n"
"    PixelShader = compile psx FSMain();\n"
"  }\n"
"}\n";

enum WireTriFlag
{
  WTF_USEZ       = (1<<0),
  WTF_LINE       = (1<<1),
  WTF_SOLID      = (1<<2),
  WTF_NORMALS    = (1<<3)
};


class WireTri
{
public:
	WireTri(void)
	{
		mNext     = 0;
		mPrevious = 0;
    mFlags = WTF_USEZ;
	};

  void setWireTriFlag(WireTriFlag flag) { mFlags|=flag; };
  void clearWireTriFlag(WireTriFlag flag) { mFlags&=~flag; };

  bool hasWireTriFlag(WireTriFlag flag) const
  {
    bool ret = false;
    if ( mFlags & flag ) ret = true;
    return ret;
  }


	void Set(const HeF32 *p1,
					 const HeF32 *p2,
					 const HeF32 *p3,
					 HeU32 color,
					 HeF32 lifespan,
           WireTriFlag flag,
           HeF32 renderScale)
	{
    mFlags = flag;
		mP1.mPos[0] = p1[0]*renderScale;
		mP1.mPos[1] = p1[1]*renderScale;
		mP1.mPos[2] = p1[2]*renderScale;

		mP2.mPos[0] = p2[0]*renderScale;
		mP2.mPos[1] = p2[1]*renderScale;
		mP2.mPos[2] = p2[2]*renderScale;

		if ( p3 )
		{
			mP3.mPos[0] = p3[0]*renderScale;
			mP3.mPos[1] = p3[1]*renderScale;
			mP3.mPos[2] = p3[2]*renderScale;
		}

		mP1.mColor = color;
		mP2.mColor = color;
		mP3.mColor = color;

		mLifeSpan = lifespan;

    if ( flag & WTF_SOLID )
    {
      fm_computePlane(mP3.mPos,mP2.mPos,mP1.mPos,mN1);

      mN2[0] = mN1[0];
      mN2[1] = mN1[1];
      mN2[2] = mN1[2];

      mN3[0] = mN1[0];
      mN3[1] = mN1[1];
      mN3[2] = mN1[2];

    }

	};


	void Set(const HeF32 *p1,
					 const HeF32 *p2,
					 const HeF32 *p3,
           const HeF32 *n1,
           const HeF32 *n2,
           const HeF32 *n3,
					 HeU32 color,
					 HeF32 lifespan,
           WireTriFlag flag,
           HeF32 renderScale)
	{
    mFlags = flag;

		mP1.mPos[0] = p1[0]*renderScale;
		mP1.mPos[1] = p1[1]*renderScale;
		mP1.mPos[2] = p1[2]*renderScale;

		mP2.mPos[0] = p2[0]*renderScale;
		mP2.mPos[1] = p2[1]*renderScale;
		mP2.mPos[2] = p2[2]*renderScale;

  	mP3.mPos[0] = p3[0]*renderScale;
		mP3.mPos[1] = p3[1]*renderScale;
		mP3.mPos[2] = p3[2]*renderScale;

    mN1[0] = n1[0];
    mN1[1] = n1[1];
    mN1[2] = n1[2];

    mN2[0] = n2[0];
    mN2[1] = n2[1];
    mN2[2] = n2[2];

    mN3[0] = n3[0];
    mN3[1] = n3[1];
    mN3[2] = n3[2];


		mP1.mColor = color;
		mP2.mColor = color;
		mP3.mColor = color;

		mLifeSpan = lifespan;

	};


	DebugVertex * RenderLine(HeF32 dtime,DebugVertex *current,bool &remove,bool flush)
	{

		current[0] = mP1;
		current[1] = mP2;

		if ( hasWireTriFlag(WTF_LINE) )
		{
			current+=2;
		}
		else
		{
			current[2] = mP2;
			current[3] = mP3;
			current[4] = mP3;
			current[5] = mP1;
			current+=6;
		}

		if ( flush )
		{
			mLifeSpan-=dtime;
			if ( mLifeSpan < 0 )
			{
				remove = true;
			}
			else
			{
				remove = false;
			}
		}

		return current;

	}

	SolidVertex * RenderSolid(HeF32 dtime,SolidVertex *current,bool &remove,bool flush)
	{
    current[0].mPos[0]   = mP1.mPos[0];
    current[0].mPos[1]   = mP1.mPos[1];
    current[0].mPos[2]   = mP1.mPos[2];

    current[0].mColor    = mP1.mColor;
    current[0].mNormal[0] = mN1[0];
    current[0].mNormal[1] = mN1[1];
    current[0].mNormal[2] = mN1[2];

    current[1].mPos[0]   = mP2.mPos[0];
    current[1].mPos[1]   = mP2.mPos[1];
    current[1].mPos[2]   = mP2.mPos[2];
    current[1].mColor    = mP2.mColor;
    current[1].mNormal[0] = mN2[0];
    current[1].mNormal[1] = mN2[1];
    current[1].mNormal[2] = mN2[2];

    current[2].mPos[0]   = mP3.mPos[0];
    current[2].mPos[1]   = mP3.mPos[1];
    current[2].mPos[2]   = mP3.mPos[2];
    current[2].mColor    = mP3.mColor;
    current[2].mNormal[0] = mN3[0];
    current[2].mNormal[1] = mN3[1];
    current[2].mNormal[2] = mN3[2];

		current+=3;

		if ( flush )
		{
			mLifeSpan-=dtime;
			if ( mLifeSpan < 0 )
			{
				remove = true;
			}
			else
			{
				remove = false;
			}
		}

		return current;

	}

	void SetNext(WireTri *tri) { mNext = tri; };
	void SetPrevious(WireTri *tri) { mPrevious = tri; };

	WireTri * GetNext(void) { return mNext; };
	WireTri * GetPrevious(void) { return mPrevious; };

//private:
	WireTri        *mNext;
	WireTri        *mPrevious;
	DebugVertex     mP1;
	DebugVertex     mP2;
	DebugVertex     mP3;
	HeF32           mLifeSpan;
  HeF32           mN1[3];
  HeF32           mN2[3];
  HeF32           mN3[3];
  HeU32    mFlags;
};


class InternalRenderDebug : public RenderDebug
{
private:

public:

	InternalRenderDebug(void)
	{

    mRenderScale = 1;
    mDebug.Set(MAX_WIRETRI_START,MAX_WIRETRI_GROW,MAXWIRETRI,"RenderDebug->WireTri",__FILE__,__LINE__); // initialize the wireframe triangle pool.
#if USE_PD3D
    strcpy(mWoodMaterial.mName,"wood.dds");
    strcpy(mWoodMaterial.mTexture,"wood.dds");
    strcpy(mWaterMaterial.mName,"water.dds");
    strcpy(mWaterMaterial.mTexture,"water.dds");
    strcpy(mTerrainMaterial.mName,"terrain.dds");
    strcpy(mTerrainMaterial.mTexture,"terrain.dds");
#else
    mDriver = 0;
    mTextureManager = 0;
    mSolidShader = 0;
    mWhiteShader = 0;
    mLineShader = 0;
    mTextureShader = 0;
#endif

	}

	~InternalRenderDebug(void)
	{
	}

#pragma warning( disable : 4189 )
#pragma warning( disable : 4100 )
  virtual void Initialize( HEGRDRIVER::HeGrDriver* driver, HETEXTUREMANAGER::HeTextureManager* textureManager )
  {
#if USE_PD3D
#else
    mDriver = driver;
    mTextureManager = textureManager;

    // load textures.
    mDeviceTextures[ ET_WOOD ] = mTextureManager->createTexture( HETEXTURE_LOCAL_FILE, "wood.dds", __FILE__, __LINE__, "" );
    mDeviceTextures[ ET_WATER ] = mTextureManager->createTexture( HETEXTURE_LOCAL_FILE, "water.dds", __FILE__, __LINE__, "" );
    mDeviceTextures[ ET_TERRAIN ] = mTextureManager->createTexture( HETEXTURE_LOCAL_FILE, "terrain.dds", __FILE__, __LINE__, "" );

    // initialize shaders.
    if ( mDriver )
    {
      mWhiteShader = mDriver->createShader( white_shader, sizeof( white_shader ) );
      mSolidShader = mDriver->createShader( solid_shader, sizeof( solid_shader ) );
      mLineShader = mDriver->createShader( line_shader, sizeof( line_shader ) );
      mTextureShader = mDriver->createShader( texture_shader, sizeof( texture_shader ) );

      // initialize shader parameters.
      mWhiteShaderMVP = mWhiteShader->getUniformLocation( "ModelToProj" );

      mSolidShaderMVP = mSolidShader->getUniformLocation( "ModelToProj" );
      mSolidShaderLocalCameraPos = mSolidShader->getUniformLocation( "LocalCameraPos" );

      mLineShaderMVP = mLineShader->getUniformLocation( "ModelToProj" );

      mTextureShaderMVP = mTextureShader->getUniformLocation( "ModelToProj" );
      mTextureShaderLocalCameraPos = mTextureShader->getUniformLocation( "LocalCameraPos" );
      mTextureShaderTexture = mTextureShader->getSamplerLocation( "Tex" );
    }
#endif
  }

	virtual void Render(HeF32 dtime, bool flush, bool zPass)
	{
    HeMat44 mvp = mViewProjectionMatrix;
    HeVec3 localCameraPos = mEyePos;
    // only render if the driver is available.
#if USE_PD3D

		HeI32 tricount = mDebug.Begin();
		if ( tricount )
		{


			DebugVertex *stop    = &mLinePoints[MAXLINEPOINTS-6];
			DebugVertex *current = mLinePoints;

      SolidVertex *s_stop    = &mSolidPoints[MAXLINEPOINTS-3];
      SolidVertex *s_current = mSolidPoints;


			for (HeI32 i=0; i<tricount; i++)
			{
				WireTri *tri = mDebug.GetNext();

        if ( tri->hasWireTriFlag(WTF_SOLID) )
        {
          if ( zPass )
          {
					  bool remove;
					  s_current = tri->RenderSolid(dtime,s_current,remove,flush);
					  if ( remove )
					  {
						  mDebug.Release(tri);
					  }
					  if ( s_current >= s_stop )
					  {
						  HeU32 count = (HeU32)(s_current - mSolidPoints)/3;
              gPd3d->renderSolid(count,(const PD3D::Pd3dSolidVertex *)mSolidPoints);
						  s_current = mSolidPoints;
					  }
          }
        }
				else if ( tri->hasWireTriFlag(WTF_USEZ) == zPass )
				{
					bool remove;
					current = tri->RenderLine(dtime,current,remove,flush);
					if ( remove )
					{
						mDebug.Release(tri);
					}
					if ( current >= stop )
					{
						HeU32 count = (HeU32)(current - mLinePoints)/2;
						gPd3d->renderLines(count,(const PD3D::Pd3dLineVertex *)mLinePoints,zPass);
						current = mLinePoints;
					}
				}
			}

      {
  			HeU32 count = (HeU32)(current - mLinePoints)/2;
  			if ( count )
  			{
  				gPd3d->renderLines(count,(const PD3D::Pd3dLineVertex *)mLinePoints,zPass);
  			}
      }

      {
  			HeU32 count = (HeU32)(s_current - mSolidPoints)/3;
  			if ( count )
  			{
          gPd3d->renderSolid(count,(const PD3D::Pd3dSolidVertex *)mSolidPoints);
  			}
      }
		}

#else
    if ( mDriver )
    {
      // configure shaders.
      mSolidShader->setUniformMatrix( mSolidShaderMVP, mvp );
      mSolidShader->setUniformVec4( mSolidShaderLocalCameraPos, localCameraPos.x, localCameraPos.y, localCameraPos.z, 1.0f );
      mLineShader->setUniformMatrix( mLineShaderMVP, mvp );

      // get the previous shader so that we can restore it's state.
      HeGrShader* prevShader = mDriver->getShader();
		  HeI32 tricount = mDebug.Begin();
		  if ( tricount )
		  {
			  DebugVertex *stop    = &mLinePoints[MAXLINEPOINTS-6];
			  DebugVertex *current = mLinePoints;

        SolidVertex *s_stop    = &mSolidPoints[MAXLINEPOINTS-3];
        SolidVertex *s_current = mSolidPoints;


			  for (HeI32 i=0; i<tricount; i++)
			  {
				  WireTri *tri = mDebug.GetNext();

          if ( tri->hasWireTriFlag(WTF_SOLID) )
          {
            if ( zPass )
            {
					    bool remove;
					    s_current = tri->RenderSolid(dtime,s_current,remove,flush);
					    if ( remove )
					    {
						    mDebug.Release(tri);
					    }
					    if ( s_current >= s_stop )
					    {
						    HeU32 count = (HeU32)(s_current - mSolidPoints);

                // TODO: Use HeGrDriver
                mDriver->setShader( mSolidShader );
                mDriver->renderStream(PT_TRIANGLE_LIST, VF_POSITION | VF_NORMAL | VF_COLOR, mSolidPoints, count );
						    s_current = mSolidPoints;
					    }
            }
          }
				  else if ( tri->hasWireTriFlag(WTF_USEZ) == zPass )
				  {
					  bool remove;
					  current = tri->RenderLine(dtime,current,remove,flush);
					  if ( remove )
					  {
						  mDebug.Release(tri);
					  }
					  if ( current >= stop )
					  {
						  HeU32 count = (HeU32)(current - mLinePoints);
              mDriver->setShader( mLineShader );
              mDriver->renderLines(VF_POSITION | VF_COLOR, mLinePoints, count );
						  current = mLinePoints;
					  }
				  }
			  }

        // check for any extra lines.
        {
  			  HeU32 count = (HeU32)(current - mLinePoints);
  			  if ( count )
          {
            mDriver->setShader( mLineShader );
            mDriver->renderLines(VF_POSITION | VF_COLOR, mLinePoints, count );
          }
        }

        // check for any extra triangles.
        {
  			  HeU32 count = (HeU32)(s_current - mSolidPoints);
  			  if ( count )
          {
            mDriver->setShader( mSolidShader );
            mDriver->renderStream(PT_TRIANGLE_LIST, VF_POSITION | VF_NORMAL | VF_COLOR, mSolidPoints, count );
          }
        }
		  }

      // restore the previous shader.
      mDriver->setShader( prevShader );
    }
#endif
	}

#pragma warning( default : 4100 )
#pragma warning( default : 4189 )

	virtual void DebugSolidTri(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,HeU32 color,HeF32 duration)
  {

    WireTri *tri = mDebug.GetFreeLink(); // pull a MEMALLOC_FREE triangle from the pool
    if ( tri )
    {
      tri->Set(p1,p2,p3, color | 0xFF000000, duration, (WireTriFlag)(WTF_USEZ | WTF_SOLID), mRenderScale ); // set condition of the triangle
    }

  }

	virtual void DebugTri(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,HeU32 color,HeF32 duration,bool useZ)
	{
		WireTri *tri = mDebug.GetFreeLink(); // pull a MEMALLOC_FREE triangle from the pool
		if ( tri )
		{
      HeU32 flag = 0;
      if ( useZ ) flag = WTF_USEZ;
			tri->Set(p1,p2,p3, color | 0xFF000000, duration, (WireTriFlag)flag, mRenderScale ); // set condition of the triangle
		}
	}

  void adjust(const HeF32 *p,HeF32 *d,HeF32 arrowSize)
  {
    d[0] = d[0]*arrowSize+p[0];
    d[1] = d[1]*arrowSize+p[1];
    d[2] = d[2]*arrowSize+p[2];
  }

	void DebugRay(const HeF32 *p1,const HeF32 *p2,HeF32 arrowSize,HeU32 color,HeU32 arrowColor,HeF32 duration,bool useZ)
  {

    DebugLine(p1,p2,color,duration,useZ);

    HeF32 dir[3];

    dir[0] = p2[0] - p1[0];
    dir[1] = p2[1] - p1[1];
    dir[2] = p2[2] - p1[2];

    HeF32 mag = fm_normalize(dir);

    if ( arrowSize > (mag*0.2f) )
    {
      arrowSize = mag*0.2f;
    }

    HeF32 ref[3] = { 0, 1, 0 };

    HeF32 quat[4];

    fm_rotationArc(ref,dir,quat);

    HeF32 matrix[16];
    fm_quatToMatrix(quat,matrix);
    fm_setTranslation(p2,matrix);


    HeU32 pcount = 0;
    HeF32 points[24*3];
    HeF32 *dest = points;

    for (HeF32 a=30; a<=360; a+=30)
    {
      HeF32 r = a*FM_DEG_TO_RAD;
      HeF32 x = cosf(r)*arrowSize;
      HeF32 y = sinf(r)*arrowSize;

      dest[0] = x;
      dest[1] = -3*arrowSize;
      dest[2] = y;
      dest+=3;
      pcount++;
    }

    HeF32 *prev = &points[(pcount-1)*3];
    HeF32 *p = points;
    HeF32 center[3] = { 0, -2.5f*arrowSize, 0 };
    HeF32 top[3]    = { 0, 0, 0 };

    HeF32 _center[3];
    HeF32 _top[3];

    fm_transform(matrix,center,_center);
    fm_transform(matrix,top,_top);

    for (HeU32 i=0; i<pcount; i++)
    {

      HeF32 _p[3];
      HeF32 _prev[3];
      fm_transform(matrix,p,_p);
      fm_transform(matrix,prev,_prev);

      DebugSolidTri(_p,_center,_prev,arrowColor,duration);
      DebugSolidTri(_prev,_top,_p,arrowColor,duration);
      DebugTri(_p,_center,_prev,0xFFFFFF,duration,useZ);
      DebugTri(_prev,_top,_p,0xFFFFFF,duration,useZ);

      prev = p;
      p+=3;
    }
  }

	virtual void DebugLine(const HeF32 *p1,const HeF32 *p2,HeU32 color,HeF32 duration,bool useZ)
	{
		WireTri *tri = mDebug.GetFreeLink(); // pull a MEMALLOC_FREE triangle from the pool
		if ( tri )
		{
      HeU32 flag = WTF_LINE;
      if ( useZ ) flag|=WTF_USEZ;
			tri->Set(p1,p2,0, color | 0xFF000000, duration, (WireTriFlag) flag, mRenderScale ); // set condition of the triangle
		}
	}
	void DebugOrientedLine(const HeF32 *p1,const HeF32 *p2,const HeF32 *transform,HeU32 color,HeF32 duration,bool useZ)
	{
		WireTri *tri = mDebug.GetFreeLink(); // pull a MEMALLOC_FREE triangle from the pool
		if ( tri )
		{
      HeU32 flag = WTF_LINE;
      if ( useZ ) flag|=WTF_USEZ;

      HeF32 t1[3];
      HeF32 t2[3];

      fm_transform(transform,p1,t1);
      fm_transform(transform,p2,t2);

			tri->Set(t1,t2,0, color | 0xFF000000, duration, (WireTriFlag) flag, mRenderScale ); // set condition of the triangle
		}
	}

	virtual void DebugBound(const HeF32 *bmin,const HeF32 *bmax,HeU32 color,HeF32 duration,bool useZ,bool solid)
	{
		HeVec3 box[8];

		box[0].set( bmin[0], bmin[1], bmin[2] );
		box[1].set( bmax[0], bmin[1], bmin[2] );
		box[2].set( bmax[0], bmax[1], bmin[2] );
		box[3].set( bmin[0], bmax[1], bmin[2] );
		box[4].set( bmin[0], bmin[1], bmax[2] );
		box[5].set( bmax[0], bmin[1], bmax[2] );
		box[6].set( bmax[0], bmax[1], bmax[2] );
		box[7].set( bmin[0], bmax[1], bmax[2] );

    if ( !solid )
    {
  		DebugLine(box[0].ptr(), box[1].ptr(), color, duration, useZ );
  		DebugLine(box[1].ptr(), box[2].ptr(), color, duration, useZ );
  		DebugLine(box[2].ptr(), box[3].ptr(), color, duration, useZ );
  		DebugLine(box[3].ptr(), box[0].ptr(), color, duration, useZ );

  		DebugLine(box[4].ptr(), box[5].ptr(), color, duration, useZ );
  		DebugLine(box[5].ptr(), box[6].ptr(), color, duration, useZ );
  		DebugLine(box[6].ptr(), box[7].ptr(), color, duration, useZ );
  		DebugLine(box[7].ptr(), box[4].ptr(), color, duration, useZ );

  		DebugLine(box[0].ptr(), box[4].ptr(), color, duration, useZ );
  		DebugLine(box[1].ptr(), box[5].ptr(), color, duration, useZ );
  		DebugLine(box[2].ptr(), box[6].ptr(), color, duration, useZ );
  		DebugLine(box[3].ptr(), box[7].ptr(), color, duration, useZ );
    }
    else
    {
  		DebugSolidTri(box[2].ptr(),box[1].ptr(),box[0].ptr(),color,duration);
  		DebugSolidTri(box[3].ptr(),box[2].ptr(),box[0].ptr(),color,duration);

  		DebugSolidTri(box[7].ptr(),box[2].ptr(),box[3].ptr(),color,duration);
  		DebugSolidTri(box[7].ptr(),box[6].ptr(),box[2].ptr(),color,duration);

  		DebugSolidTri(box[5].ptr(),box[1].ptr(),box[2].ptr(),color,duration);
  		DebugSolidTri(box[5].ptr(),box[2].ptr(),box[6].ptr(),color,duration);

  		DebugSolidTri(box[5].ptr(),box[4].ptr(),box[1].ptr(),color,duration);
  		DebugSolidTri(box[4].ptr(),box[0].ptr(),box[1].ptr(),color,duration);

  		DebugSolidTri(box[4].ptr(),box[6].ptr(),box[7].ptr(),color,duration);
  		DebugSolidTri(box[4].ptr(),box[5].ptr(),box[6].ptr(),color,duration);

  		DebugSolidTri(box[4].ptr(),box[7].ptr(),box[0].ptr(),color,duration);
  		DebugSolidTri(box[7].ptr(),box[3].ptr(),box[0].ptr(),color,duration);
    }
	}

	virtual void DebugOrientedBound(const HeF32 *bmin,
																	const HeF32 *bmax,
																	const HeF32 *pos,
																	const HeF32 *rot,
																	HeU32 color,
																	HeF32 duration,
																	bool useZ,
                                  bool solid) // the rotation as a quaternion
	{
    HeMat44 m;
    HeQuat q;
    q.setXYZW(rot[0],rot[1],rot[2],rot[3]);
    m.fromQuat(q);
    HeVec3 p(pos);
    m.setPosition(p);
		DebugOrientedBound(bmin,bmax,m.ptr(),color,duration,useZ,solid);
	}

	virtual void DebugOrientedBound(const HeF32 *bmin,
																  const HeF32 *bmax,
																  const HeF32 *xform,
																	HeU32 color,
																	HeF32 duration,
																	bool useZ,
																	bool solid) // the rotation as a quaternion
	{
		HeVec3 box[8];

		box[0].set( bmin[0], bmin[1], bmin[2] );
		box[1].set( bmax[0], bmin[1], bmin[2] );
		box[2].set( bmax[0], bmax[1], bmin[2] );
		box[3].set( bmin[0], bmax[1], bmin[2] );
		box[4].set( bmin[0], bmin[1], bmax[2] );
		box[5].set( bmax[0], bmin[1], bmax[2] );
		box[6].set( bmax[0], bmax[1], bmax[2] );
		box[7].set( bmin[0], bmax[1], bmax[2] );

    HeMat44 m(xform);

    m.multiply(box[0],box[0]); // = m.Transform(box[0]);
		m.multiply(box[1],box[1]);
		m.multiply(box[2],box[2]);
		m.multiply(box[3],box[3]);
		m.multiply(box[4],box[4]);
		m.multiply(box[5],box[5]);
		m.multiply(box[6],box[6]);
		m.multiply(box[7],box[7]);

    if ( !solid )
    {
  		DebugLine(box[0].ptr(), box[1].ptr(), color, duration, useZ );
  		DebugLine(box[1].ptr(), box[2].ptr(), color, duration, useZ );
  		DebugLine(box[2].ptr(), box[3].ptr(), color, duration, useZ );
  		DebugLine(box[3].ptr(), box[0].ptr(), color, duration, useZ );

  		DebugLine(box[4].ptr(), box[5].ptr(), color, duration, useZ );
  		DebugLine(box[5].ptr(), box[6].ptr(), color, duration, useZ );
  		DebugLine(box[6].ptr(), box[7].ptr(), color, duration, useZ );
  		DebugLine(box[7].ptr(), box[4].ptr(), color, duration, useZ );

  		DebugLine(box[0].ptr(), box[4].ptr(), color, duration, useZ );
  		DebugLine(box[1].ptr(), box[5].ptr(), color, duration, useZ );
  		DebugLine(box[2].ptr(), box[6].ptr(), color, duration, useZ );
  		DebugLine(box[3].ptr(), box[7].ptr(), color, duration, useZ );
    }
    else
    {
  		DebugSolidTri(box[2].ptr(),box[1].ptr(),box[0].ptr(),color,duration);
  		DebugSolidTri(box[3].ptr(),box[2].ptr(),box[0].ptr(),color,duration);

  		DebugSolidTri(box[7].ptr(),box[2].ptr(),box[3].ptr(),color,duration);
  		DebugSolidTri(box[7].ptr(),box[6].ptr(),box[2].ptr(),color,duration);

  		DebugSolidTri(box[5].ptr(),box[1].ptr(),box[2].ptr(),color,duration);
  		DebugSolidTri(box[5].ptr(),box[2].ptr(),box[6].ptr(),color,duration);

  		DebugSolidTri(box[5].ptr(),box[4].ptr(),box[1].ptr(),color,duration);
  		DebugSolidTri(box[4].ptr(),box[0].ptr(),box[1].ptr(),color,duration);

  		DebugSolidTri(box[4].ptr(),box[6].ptr(),box[7].ptr(),color,duration);
  		DebugSolidTri(box[4].ptr(),box[5].ptr(),box[6].ptr(),color,duration);

  		DebugSolidTri(box[4].ptr(),box[7].ptr(),box[0].ptr(),color,duration);
  		DebugSolidTri(box[7].ptr(),box[3].ptr(),box[0].ptr(),color,duration);
    }
	}

	virtual void DebugOrientedBound(const HeF32 *sides,
  																const HeF32 *transform,
																	HeU32 color,
																  HeF32 duration,
																  bool useZ,
                                  bool solid) // the rotation as a quaternion
	{
		HeVec3 box[8];

		HeF32 bmin[3];
		HeF32 bmax[3];

		bmin[0] = -sides[0]*0.5f;
		bmin[1] = -sides[1]*0.5f;
		bmin[2] = -sides[2]*0.5f;

		bmax[0] = +sides[0]*0.5f;
		bmax[1] = +sides[1]*0.5f;
		bmax[2] = +sides[2]*0.5f;

		box[0].set( bmin[0], bmin[1], bmin[2] );
		box[1].set( bmax[0], bmin[1], bmin[2] );
		box[2].set( bmax[0], bmax[1], bmin[2] );
		box[3].set( bmin[0], bmax[1], bmin[2] );
		box[4].set( bmin[0], bmin[1], bmax[2] );
		box[5].set( bmax[0], bmin[1], bmax[2] );
		box[6].set( bmax[0], bmax[1], bmax[2] );
		box[7].set( bmin[0], bmax[1], bmax[2] );

		HeMat44 *m = (HeMat44 *) transform;

		m->multiply(box[0],box[0]);
		m->multiply(box[1],box[1]);
		m->multiply(box[2],box[2]);
		m->multiply(box[3],box[3]);
		m->multiply(box[4],box[4]);
		m->multiply(box[5],box[5]);
		m->multiply(box[6],box[6]);
		m->multiply(box[7],box[7]);

    if ( !solid )
    {
		  DebugLine(box[0].ptr(), box[1].ptr(), color, duration, useZ );
		  DebugLine(box[1].ptr(), box[2].ptr(), color, duration, useZ );
		  DebugLine(box[2].ptr(), box[3].ptr(), color, duration, useZ );
		  DebugLine(box[3].ptr(), box[0].ptr(), color, duration, useZ );

		  DebugLine(box[4].ptr(), box[5].ptr(), color, duration, useZ );
		  DebugLine(box[5].ptr(), box[6].ptr(), color, duration, useZ );
		  DebugLine(box[6].ptr(), box[7].ptr(), color, duration, useZ );
		  DebugLine(box[7].ptr(), box[4].ptr(), color, duration, useZ );

		  DebugLine(box[0].ptr(), box[4].ptr(), color, duration, useZ );
		  DebugLine(box[1].ptr(), box[5].ptr(), color, duration, useZ );
		  DebugLine(box[2].ptr(), box[6].ptr(), color, duration, useZ );
		  DebugLine(box[3].ptr(), box[7].ptr(), color, duration, useZ );
    }
    else
    {
// valid order
  		DebugSolidTri(box[2].ptr(),box[1].ptr(),box[0].ptr(),color,duration);
  		DebugSolidTri(box[3].ptr(),box[2].ptr(),box[0].ptr(),color,duration);

  		DebugSolidTri(box[7].ptr(),box[2].ptr(),box[3].ptr(),color,duration);
  		DebugSolidTri(box[7].ptr(),box[6].ptr(),box[2].ptr(),color,duration);

  		DebugSolidTri(box[5].ptr(),box[1].ptr(),box[2].ptr(),color,duration);
  		DebugSolidTri(box[5].ptr(),box[2].ptr(),box[6].ptr(),color,duration);

  		DebugSolidTri(box[5].ptr(),box[4].ptr(),box[1].ptr(),color,duration);
  		DebugSolidTri(box[4].ptr(),box[0].ptr(),box[1].ptr(),color,duration);

  		DebugSolidTri(box[4].ptr(),box[6].ptr(),box[7].ptr(),color,duration);
  		DebugSolidTri(box[4].ptr(),box[5].ptr(),box[6].ptr(),color,duration);

  		DebugSolidTri(box[4].ptr(),box[7].ptr(),box[0].ptr(),color,duration);
  		DebugSolidTri(box[7].ptr(),box[3].ptr(),box[0].ptr(),color,duration);
    }
	}

	virtual void DebugSphere(const HeF32 *pos,HeF32 radius,HeU32 color,HeF32 duration,bool useZ,bool solid)
	{
		const HeF32 *source = debug_sphere;

		for (HeI32 i=0; i<32; i++)
		{
			HeVec3 p1( source );
			source+=3;
			HeVec3 p2( source );
			source+=3;
			HeVec3 p3( source );
			source+=3;

			p1.x = p1.x*radius + pos[0];
			p1.y = p1.y*radius + pos[1];
			p1.z = p1.z*radius + pos[2];

			p2.x = p2.x*radius + pos[0];
			p2.y = p2.y*radius + pos[1];
			p2.z = p2.z*radius + pos[2];

			p3.x = p3.x*radius + pos[0];
			p3.y = p3.y*radius + pos[1];
			p3.z = p3.z*radius + pos[2];

      if ( solid )
  			DebugSolidTri(p1.ptr(),p2.ptr(),p3.ptr(),color,duration);
      else
	  		DebugTri(p1.ptr(),p2.ptr(),p3.ptr(),color,duration,useZ);
		}
	}

	virtual void DebugHalfSphere(const HeF32 *pos,HeF32 radius,HeU32 color,HeF32 duration,bool useZ)
	{
		const HeF32 *source = debug_halfsphere;

		for (HeI32 i=0; i<16; i++)
		{
			HeVec3 p1( source );
			source+=3;
			HeVec3 p2( source );
			source+=3;
			HeVec3 p3( source );
			source+=3;

			p1.x = p1.x*radius + pos[0];
			p1.y = p1.y*radius + pos[1];
			p1.z = p1.z*radius + pos[2];

			p2.x = p2.x*radius + pos[0];
			p2.y = p2.y*radius + pos[1];
			p2.z = p2.z*radius + pos[2];

			p3.x = p3.x*radius + pos[0];
			p3.y = p3.y*radius + pos[1];
			p3.z = p3.z*radius + pos[2];

			DebugTri(p1.ptr(),p2.ptr(),p3.ptr(),color,duration,useZ);
		}
	}

	virtual void DebugPoint(const HeF32 *pos,HeF32 radius,HeU32 color,HeF32 duration,bool useZ)
	{
		const HeF32 *source = debug_point;

		for (HeI32 i=0; i<3; i++)
		{
			HeVec3 p1( source );
			source+=3;
			HeVec3 p2( source );
			source+=3;

			p1.x = p1.x*radius + pos[0];
			p1.y = p1.y*radius + pos[1];
			p1.z = p1.z*radius + pos[2];

			p2.x = p2.x*radius + pos[0];
			p2.y = p2.y*radius + pos[1];
			p2.z = p2.z*radius + pos[2];

			DebugLine(p1.ptr(),p2.ptr(),color,duration,useZ);
		}
	}

	virtual void DebugOrientedSphere(HeF32 radius,const HeF32 *transform,HeU32 color,HeF32 duration,bool useZ,bool solid)
	{
		const HeF32 *source = debug_sphere;

		HeMat44 m(transform);

		for (HeI32 i=0; i<32; i++)
		{
			HeVec3 p1( source );
			source+=3;
			HeVec3 p2( source );
			source+=3;
			HeVec3 p3( source );
			source+=3;

      p1*=radius;
      p2*=radius;
      p3*=radius;

			m.multiply( p1, p1 );
			m.multiply( p2, p2 );
			m.multiply( p3, p3 );

      if ( solid )
      {
  			DebugSolidTri(p1.ptr(),p2.ptr(),p3.ptr(),color,duration);
      }
      else
      {
  			DebugTri(p1.ptr(),p2.ptr(),p3.ptr(),color,duration,useZ);
      }
		}
	}

	HeI32 ClampColor(HeI32 c)
	{
		if ( c < 0 )
			c = 0;
		else if ( c > 255 )
			c = 255;
		return c;
	};

	HeU32 GetColor(HeF32 r,HeF32 g,HeF32 b,HeF32 brightness)
	{
		HeI32 red   = HeI32(r*brightness*255.0f);
		HeI32 green = HeI32(g*brightness*255.0f);
		HeI32 blue  = HeI32(b*brightness*255.0f);
		red   = ClampColor(red);
		green = ClampColor(green);
		blue  = ClampColor(blue);
		HeU32 color = 0xFF000000 | (red<<16) | (green<<8) | blue;
		return color;
	}

	virtual void DebugAxes(const HeF32 *xform,HeF32 distance,HeF32 brightness,bool useZ)
	{
		HeMat44 m(xform);

		HeF32 dtime = 0.0001f;
		HeF32 side =  distance*0.015f;

		HeVec3 bmin(0,-side,-side);
		HeVec3 bmax(distance,side,side);

		HeU32 red    = GetColor(1,0,0,brightness);
		HeU32 green  = GetColor(0,1,0,brightness);
		HeU32 blue   = GetColor(0,0,1,brightness);
		HeU32 yellow = GetColor(1,1,0,brightness);

		DebugOrientedBound( bmin.ptr(), bmax.ptr(), xform, red, dtime, useZ, false );

		bmin.set(-side,0,-side);
		bmax.set(side,distance,side);

		DebugOrientedBound( bmin.ptr(), bmax.ptr(), xform, green, dtime,useZ, false );

		bmin.set(-side,-side,0);
		bmax.set(side,side,distance);

		DebugOrientedBound( bmin.ptr(), bmax.ptr(), xform, blue, dtime, useZ, false );

		HeVec3 o(0,0,0);
		m.multiply(o,o);
		DebugSphere( o.ptr(), distance*0.025f, yellow, dtime, useZ, false );

		// ok..now we need to draw the cone...
		{
			#define step 16
			HeVec3 p[step];
			HeF32 a = 0;
			HeF32 da = (FM_PI*2)/step;

			for (HeI32 i=0; i<step; i++)
			{
				p[i].x = cosf(a)*side*3;
				p[i].y = sinf(a)*side*3;
				a+=da;
			}

			{
				HeVec3 ox(distance*1.2f,0,0);
				HeVec3 oy(0,distance*1.2f,0);
				HeVec3 oz(0,0,distance*1.2f);

				m.multiply(ox,ox);
				m.multiply(oy,oy);
				m.multiply(oz,oz);

				for (HeI32 i=0; i<step; i++)
				{
					HeI32 i2 = i+1;
					if ( i2 == step )
						i2 = 0;

					const HeVec3 &p1 = p[i];
					const HeVec3 &p2 = p[i2];

					HeVec3 t1(distance, p1.x, p1.y );
					HeVec3 t2(distance, p2.x, p2.y );

					m.multiply(t1,t1);
					m.multiply(t2,t2);

					DebugTri(ox.ptr(), t1.ptr(), t2.ptr(), red, dtime, useZ );

					t1.set(p1.x,distance,p1.y);
					t2.set(p2.x,distance,p2.y);

					m.multiply(t1,t1);
					m.multiply(t2,t2);

					DebugTri(oy.ptr(), t1.ptr(), t2.ptr(), green, dtime,useZ );


					t1.set(p1.x,p1.y,distance);
					t2.set(p2.x,p2.y,distance);

					m.multiply(t1,t1);
					m.multiply(t2,t2);

					DebugTri(oz.ptr(), t1.ptr(), t2.ptr(), blue, dtime, useZ );
				}
			}
		}
	}

	virtual void Reset( void )
	{
    HeI32 count = mDebug.Begin();
    for (HeI32 i=0; i<count; i++)
    {
      WireTri *wt = mDebug.GetNext();
      mDebug.Release(wt);
    }
	}

  bool shutdown(void)
  {
    return doShutdown();
  }

	void DebugSolidTri(const HeF32 *p1,const HeF32 *p2,const HeF32 *p3,const HeF32 *n1,const HeF32 *n2,const HeF32 *n3,HeU32 color,HeF32 duration)
  {
    WireTri *tri = mDebug.GetFreeLink(); // pull a MEMALLOC_FREE triangle from the pool
    if ( tri )
    {
      tri->Set(p1,p2,p3,n1,n2,n3,color | 0xFF000000, duration, (WireTriFlag)(WTF_USEZ | WTF_SOLID | WTF_NORMALS ), mRenderScale ); // set condition of the triangle
    }
  }


	void DebugCapsule(const HeF32 *pos,HeF32 radius,HeF32 height,HeU32 color,HeF32 duration,bool useZ)
  {
    HeMat44 t;
    HeVec3 p(pos);
    t.setPosition(p);
    DebugOrientedCapsule(radius,height,t.ptr(),color,duration,useZ);
  }

	void DebugOrientedCapsule(HeF32 radius,HeF32 height,const HeF32 *transform,HeU32 color,HeF32 duration,bool useZ)
  {
    HeVec3 prev1;
    HeVec3 prev2;

    HeF32 h2 = height*0.5f;

    HeVec3 top(0,0,0);
    HeVec3 bottom(0,0,0);

    top.y+=(height*0.5f)+radius;
    bottom.y-=(height*0.5f)+radius;

    for (HeI32 a=0; a<=360; a+=15)
    {
      HeF32 r = (HeF32)a*FM_DEG_TO_RAD;

      HeF32 x = radius*cosf(r);
      HeF32 z = radius*sinf(r);

      HeVec3 p1(x,-h2,z);
      HeVec3 p2(x,h2,z);

      DebugOrientedLine( p1.ptr(), p2.ptr(), transform, color, duration, useZ );

      DebugOrientedLine( p2.ptr(), top.ptr(), transform, color, duration, useZ);
      DebugOrientedLine( p1.ptr(), bottom.ptr(), transform, color, duration, useZ );

      if ( a != 0 )
      {
        DebugOrientedLine(prev1.ptr(), p1.ptr(), transform, color, duration, useZ );
        DebugOrientedLine(prev2.ptr(), p2.ptr(), transform,color, duration, useZ );
      }

      prev1 = p1;
      prev2 = p2;
    }

  }

  bool getWireFrame(void)
  {
    bool ret = false;

    return ret;
  }

  HeF32 getRenderScale(void)
  {
    return mRenderScale;
  }

  void  setRenderScale(HeF32 scale)
  {
    mRenderScale = scale;
  }

  HeU32 executeCommand(SendTextMessage *tmessage,const char *cmd )
  {
    HeU32 ret  = 0;

    gSendTextMessage = tmessage;

    if ( stricmp(cmd,"ClcMemoryReport") == 0 )
    {
      MEMALLOC_REPORT("RenderDebug", tmessage, true );
    }
    
    return ret;
  }


  void debugPlane(const float *plane,float radius1,float radius2,unsigned int color,float duration,bool useZ,bool spokes)
  {

    float ref[3] = { 0, 1, 0 };

    float quat[4];
    fm_rotationArc(ref,plane,quat);

    float matrix[16];
    fm_quatToMatrix(quat,matrix);

    float stepsize = 360/20;

    float prev[3] = { 0, 0, 0 };

    float pos[3];
    float first[3];

		float origin[3] = { 0, -plane[3], 0 };
    float center[3];

    fm_transform(matrix,origin,center);
    fm_setTranslation(center,matrix);

    for (float d=0; d<360; d+=stepsize)
    {
      float a = d*FM_DEG_TO_RAD;

      float dx = cosf(a)*radius1;
      float dy = 0; //
      float dz = sinf(a)*radius2;

      float _pos[3] = { dx, dy, dz };

      fm_transform(matrix,_pos,pos);

      if ( spokes )
      {
        DebugLine(center,pos,color,duration,useZ);
      }

      if ( d == 0  )
      {
        first[0] = pos[0];
        first[1] = pos[1];
        first[2] = pos[2];
      }
      else
      {
        DebugLine(prev,pos,color,duration,useZ);
      }

      prev[0] = pos[0];
      prev[1] = pos[1];
      prev[2] = pos[2];


    }

    DebugLine(first,pos,color,duration,useZ);

  }


  void DebugPlane(const HeF32 *plane,HeF32 radius1,HeF32 radius2,HeU32 color,HeF32 duration,bool useZ)
  {

    debugPlane(plane,radius1*0.25f, radius2*0.25f, color,duration,useZ,false);
    debugPlane(plane,radius1*0.5f,  radius2*0.5f,  color,duration,useZ,false);
    debugPlane(plane,radius1*0.75f, radius2*0.75f, color,duration,useZ,false);
    debugPlane(plane,radius1*1.0f,  radius2*1.0f,  color,duration,useZ,true);
  }

  HeU32 getHeapSize(HeU32 &unused)
  {
    return MEMALLOC_GET_HEAP_SIZE(unused);
  }

  void frameBegin(SendTextMessage *stm)
  {
    MEMALLOC_FRAME_BEGIN(stm);
  }

  void frameEnd(SendTextMessage *stm,const char *header)
  {
    MEMALLOC_FRAME_END(stm,header);
  }

#pragma warning( disable : 4100 )
  void batchTriangles(EmbedTexture texture,const GraphicsVertex *vertices,HeU32 vcount,bool wireframe,bool overlay)
  {
    // if the driver is ready, then we need to render the triangles.
#if USE_PD3D

    if ( gPd3d )
    {
      bool preserve = gPd3d->getWireFrame();
      gPd3d->setWireFrame(wireframe);

      PD3D::Pd3dMaterial *material = &mWoodMaterial;

      switch ( texture )
      {
        case ET_WOOD:
          material = &mWoodMaterial;
          break;
        case ET_WATER:
          material = &mWaterMaterial;
          break;
        case ET_TERRAIN:
          material = &mTerrainMaterial;
          break;
      }

      gPd3d->renderSection(material, (PD3D::Pd3dGraphicsVertex *)vertices, vcount );
      if ( overlay )
      {
        gPd3d->setWireFrame(true);
        gPd3d->renderSection(material, (PD3D::Pd3dGraphicsVertex *)vertices, vcount );
      }
      gPd3d->setWireFrame(preserve);
    }

#else
    if ( mDriver )
    {
      // configure the texture shader.
      mTextureShader->setUniformMatrix( mTextureShaderMVP, mViewProjectionMatrix );
      mTextureShader->setUniformVec4( mTextureShaderLocalCameraPos, mEyePos.x, mEyePos.y, mEyePos.z, 1.0f );

      if ( mDeviceTextures[ texture ]->isReady() )
        mTextureShader->setSampler( mTextureShaderTexture, mDeviceTextures[ texture ]->getHeGrDeviceTexture() );

      // determine what our render states need to be.
      RenderStateFlags preserve = mDriver->getState();

      if (wireframe )
        mDriver->enableState(RS_WIREFRAME);
      else
        mDriver->disableState(RS_WIREFRAME);

      // render the mesh.
      mDriver->setShader(mTextureShader);
      mDriver->renderStream(PT_TRIANGLE_LIST, VF_POSITION | VF_NORMAL | VF_TEXCOORD0_2D, vertices, vcount);

      // render the wireframe overlay component.
      if ( overlay )
      {
        mWhiteShader->setUniformMatrix( mWhiteShaderMVP, mViewProjectionMatrix );
        mDriver->setShader(mWhiteShader);
        mDriver->enableState(RS_WIREFRAME);
        mDriver->renderStream(PT_TRIANGLE_LIST, VF_POSITION | VF_NORMAL | VF_TEXCOORD0_2D, vertices, vcount);
        mDriver->disableState(RS_WIREFRAME);
      }

      // restore the previous rendering state.
      mDriver->setState(preserve);
    }
#endif
  }

  void DebugThickRay(const HeF32 *p1,
                     const HeF32 *p2,
                     HeF32 raySize,
	                   HeF32 arrowSize,
        	           HeU32 color,
	                   HeU32 arrowColor,
	                   HeF32 duration,
	                   bool wireFrameArrow)
  {

    HeF32 dir[3];

    dir[0] = p2[0] - p1[0];
    dir[1] = p2[1] - p1[1];
    dir[2] = p2[2] - p1[2];

    HeF32 mag = fm_normalize(dir);

    if ( arrowSize > (mag*0.2f) )
    {
      arrowSize = mag*0.2f;
    }

    HeF32 ref[3] = { 0, 1, 0 };

    HeF32 quat[4];

    fm_rotationArc(ref,dir,quat);

    HeF32 matrix[16];
    fm_quatToMatrix(quat,matrix);
    fm_setTranslation(p2,matrix);


    HeU32 pcount = 0;
    HeF32 points[24*3];
    HeF32 *dest = points;

    for (HeF32 a=30; a<=360; a+=30)
    {
      HeF32 r = a*FM_DEG_TO_RAD;
      HeF32 x = cosf(r)*arrowSize;
      HeF32 y = sinf(r)*arrowSize;

      dest[0] = x;
      dest[1] = -3*arrowSize;
      dest[2] = y;
      dest+=3;
      pcount++;
    }

    HeF32 *prev = &points[(pcount-1)*3];
    HeF32 *p = points;
    HeF32 center[3] = { 0, -2.5f*arrowSize, 0 };
    HeF32 top[3]    = { 0, 0, 0 };

    HeF32 _center[3];
    HeF32 _top[3];

    fm_transform(matrix,center,_center);
    fm_transform(matrix,top,_top);

    DebugCylinder(p1,_center,raySize,color,duration,true,true);

    for (HeU32 i=0; i<pcount; i++)
    {

      HeF32 _p[3];
      HeF32 _prev[3];
      fm_transform(matrix,p,_p);
      fm_transform(matrix,prev,_prev);

      DebugSolidTri(_p,_center,_prev,arrowColor,duration);
      DebugSolidTri(_prev,_top,_p,arrowColor,duration);
      if ( wireFrameArrow )
      {
        DebugTri(_p,_center,_prev,0xFFFFFF,duration,true);
        DebugTri(_prev,_top,_p,0xFFFFFF,duration,true);
      }

      prev = p;
      p+=3;
    }

  }

#pragma warning(disable:4100)
  void DebugCylinder(const HeF32 *p1,const HeF32 *p2,HeF32 radius,HeU32 color,HeF32 duration,bool useZ,bool solid)
  {

    HeF32 dir[3];

    dir[0] = p2[0] - p1[0];
    dir[1] = p2[1] - p1[1];
    dir[2] = p2[2] - p1[2];

    HeF32 mag = fm_normalize(dir);

    if ( radius > (mag*0.2f) )
    {
      radius = mag*0.2f;
    }

    HeF32 ref[3] = { 0, 1, 0 };

    HeF32 quat[4];

    fm_rotationArc(ref,dir,quat);

    HeF32 matrix1[16];
    HeF32 matrix2[16];

    fm_quatToMatrix(quat,matrix1);
    fm_setTranslation(p2,matrix1);

    fm_quatToMatrix(quat,matrix2);
    fm_setTranslation(p1,matrix2);


    HeU32 pcount = 0;
    HeF32 points1[24*3];
    HeF32 points2[24*3];
    HeF32 *dest1 = points1;
    HeF32 *dest2 = points2;


    for (HeF32 a=30; a<=360; a+=30)
    {
      HeF32 r = a*FM_DEG_TO_RAD;
      HeF32 x = cosf(r)*radius;
      HeF32 y = sinf(r)*radius;

      HeF32 p[3] = { x, 0, y };

      fm_transform(matrix1,p,dest1);
      fm_transform(matrix2,p,dest2);

      dest1+=3;
      dest2+=3;
      pcount++;

    }
    assert( pcount < 24 );

    if ( solid )
    {
      HeF32 *prev1 = &points1[(pcount-1)*3];
      HeF32 *prev2 = &points2[(pcount-1)*3];

      HeF32 *scan1 = points1;
      HeF32 *scan2 = points2;

      for (HeU32 i=0; i<pcount; i++)
      {
        DebugSolidTri(scan1,prev2,prev1,color,duration);
        DebugSolidTri(scan2,prev2,scan1,color,duration);
        prev1 = scan1;
        prev2 = scan2;
        scan1+=3;
        scan2+=3;
      }

      DebugPolygon(pcount,points1,color,duration,useZ,solid,false);
      DebugPolygon(pcount,points2,color,duration,useZ,solid,true);

    }
    else
    {
      HeF32 *prev1 = &points1[(pcount-1)*3];
      HeF32 *prev2 = &points2[(pcount-1)*3];

      HeF32 *scan1 = points1;
      HeF32 *scan2 = points2;
      for (HeU32 i=0; i<pcount; i++)
      {
        DebugLine(scan1,scan2,color,duration,useZ);

        DebugLine(p2,scan1,color,duration,useZ);
        DebugLine(p1,scan2,color,duration,useZ);

        DebugLine(prev1,scan1,color,duration,useZ);
        DebugLine(prev2,scan2,color,duration,useZ);

        prev1 = scan1;
        prev2 = scan2;

        scan1+=3;
        scan2+=3;
      }


    }

  }

  void DebugPolygon(HeU32 pcount,const HeF32 *points,HeU32 color,HeF32 duration,bool useZ,bool solid,bool clockwise)
  {
    if ( pcount >= 3 )
    {
      if ( solid )
      {
        const HeF32 *p1 = points;
        const HeF32 *p2 = points+3;
        const HeF32 *p3 = points+6;
        if ( clockwise )
          DebugSolidTri(p1,p2,p3,color,duration);
        else
          DebugSolidTri(p3,p2,p1,color,duration);
        for (HeU32 i=3; i<pcount; i++)
        {
          p2 = p3;
          p3+=3;
          if ( clockwise )
            DebugSolidTri(p1,p2,p3,color,duration);
          else
            DebugSolidTri(p3,p2,p1,color,duration);
        }
      }
      else
      {
        const HeF32 *prev = &points[(pcount-1)*3];
        const HeF32 *p    = points;
        for (HeU32 i=0; i<pcount; i++)
        {
          DebugLine(prev,p,color,duration,useZ);
          prev = p;
          p+=3;
        }
      }
    }
  }

  HeU32 getColor(HeU32 r,HeU32 g,HeU32 b,HeF32 percent)
  {
    HeU32 dr = (HeU32)((HeF32)r*percent);
    HeU32 dg = (HeU32)((HeF32)g*percent);
    HeU32 db = (HeU32)((HeF32)b*percent);
    r-=dr;
    g-=dg;
    b-=db;
   	HeU32 c = (r<<16) | (g<<8) | b;
   	return c;
  }

  void SwapYZ(HeVec3 &p)
  {
    HeF32 y = p.y;
    p.y = p.z;
    p.z = y;
  }

	void drawGrid(bool zup) // draw a grid.
  {

    const HeI32  GRIDSIZE = 40;

 	  HeU32 c1 = getColor(133,153,181,0.1f);
 	  HeU32 c2 = getColor(133,153,181,0.3f);
 	  HeU32 c3 = getColor(133,153,181,0.5f);

 	  const HeF32 TSCALE   = 1.0f;

 	  HeF32 BASELOC = 0-0.05f;

  	for (HeI32 x=-GRIDSIZE; x<=GRIDSIZE; x++)
  	{
  	  HeU32 c = c1;
      if ( (x%10) == 0 ) c = c2;
  		if ( (x%GRIDSIZE) == 0 ) c = c3;

  		HeVec3 p1( (HeF32)x,(HeF32) -GRIDSIZE, BASELOC );
  		HeVec3 p2( (HeF32)x,(HeF32) +GRIDSIZE, BASELOC );

 		  p1*=TSCALE;
 		  p2*=TSCALE;

		  SwapYZ(p1);
		  SwapYZ(p2);

      DebugLine(&p1.x,&p2.x,c,0.00001f,true);


    }

 	  for (HeI32 y=-GRIDSIZE; y<=GRIDSIZE; y++)
 	  {
 		  HeU32 c = c1;

 		  if ( (y%10) == 0 ) c = c2;
 		  if ( (y%GRIDSIZE) == 0 ) c = c3;

 		  HeVec3 p1((HeF32) -GRIDSIZE, (HeF32)y, BASELOC );
  		HeVec3 p2( (HeF32)+GRIDSIZE, (HeF32)y, BASELOC );

 		  p1*=TSCALE;
 		  p2*=TSCALE;

		  SwapYZ(p1);
		  SwapYZ(p2);

      DebugLine(&p1.x,&p2.x,c,0.0001f,true);
    }

  }


  void         setScreenSize(HeU32 screenX,HeU32 screenY)
  {
    mScreenWidth = screenX;
    mScreenHeight = screenY;
  }

  void         getScreenSize(HeU32 &screenX,HeU32 &screenY)
  {
    screenX = mScreenWidth;
    screenY = mScreenHeight;
  }

  const HeF32 *getEyePos(void)
  {
    return mEyePos.ptr();
  }

  void         setViewProjectionMatrix(const HeF32 *view,const HeF32 *projection)
  {
    mViewMatrix.set(view);
    mProjectionMatrix.set(projection);
    mViewProjectionMatrix.multiply(mViewMatrix,mProjectionMatrix);

    // grab the world-space eye position.
    HeMat44 viewInverse;
    if ( mViewMatrix.getInverse( viewInverse ) )
      viewInverse.multiply( HeVec3( 0.0f, 0.0f, 0.0f ), mEyePos );
  }

  const HeF32 *getViewProjectionMatrix(void) const
  {
    return mViewProjectionMatrix.ptr();
  }

  const HeF32 *getViewMatrix(void) const
  {
    return mViewMatrix.ptr();
  }

  const HeF32 *getProjectionMatrix(void) const
  {
    return mProjectionMatrix.ptr();
  }

  bool         screenToWorld(HeI32 sx,HeI32 sy,HeF32 *world,HeF32 *direction)
  {
    bool ret = false;

  #if 0
    HeI32 wid = (HeI32) mScreenWidth;
    HeI32 hit = (HeI32) mScrenHeight;

    if ( sx >= 0 && sx <= wid && sy >= 0 && sy <= hit )
    {
    	HeVec3 vPickRayDir;
    	HeVec3 vPickRayOrig;

    	HeVec3 ptCursor(sx,sy,0);

    	// Compute the vector of the pick ray in screen space

    	D3DXVECTOR3 v;

    	v.x =  ( ( ( 2.0f * ptCursor.x ) / wid  ) - 1 ) / mProjection._11;
    	v.y = -( ( ( 2.0f * ptCursor.y ) / hit ) - 1 )  / mProjection._22;
    	v.z =  1.0f;

    	// Get the inverse view matrix
    	D3DXMATRIXA16 m;
    	D3DXMatrixInverse( &m, NULL, &mView );

    	// Transform the screen space pick ray into 3D space
    	vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
    	vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
    	vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;

      D3DXVec3Normalize(&vPickRayDir,&vPickRayDir);

    	vPickRayOrig.x = m._41;
    	vPickRayOrig.y = m._42;
    	vPickRayOrig.z = m._43;

    	world[0] = vPickRayOrig.x;
    	world[1] = vPickRayOrig.y;
    	world[2] = vPickRayOrig.z;

    	direction[0] = vPickRayDir.x;
    	direction[1] = vPickRayDir.y;
    	direction[2] = vPickRayDir.z;

      ret = true;
    }
  #endif

    return ret;
  }

  void setPd3d(PD3D::Pd3d *p) 
  {
#if USE_PD3D
    gPd3d = p;
#endif
  }

  void         setEmbedTexture(EmbedTexture t,LPHETEXTURE texture)
  {
#if USE_PD3D
#else
    mDeviceTextures[t] = texture;
#endif
  }

private:
  HeF32                     mRenderScale;
	Pool< WireTri >           mDebug;       // the pool to work from.
	DebugVertex               mLinePoints[MAXLINEPOINTS]; // our internal vertex buffer
  SolidVertex     mSolidPoints[MAXSOLIDPOINTS];
#if USE_PD3D
  PD3D::Pd3dMaterial       mWoodMaterial;
  PD3D::Pd3dMaterial       mWaterMaterial;
  PD3D::Pd3dMaterial       mTerrainMaterial;
#else
  HeGrDriver*               mDriver;
  HeTextureManager*         mTextureManager;
  HeGrShader*               mLineShader;
  HeGrShader*               mSolidShader;
  HeGrShader*               mWhiteShader;
  HeGrShader*               mTextureShader;
  HeGrShader::ParamHandle   mSolidShaderMVP;
  HeGrShader::ParamHandle   mWhiteShaderMVP;
  HeGrShader::ParamHandle   mLineShaderMVP;
  HeGrShader::ParamHandle   mTextureShaderMVP;
  HeGrShader::ParamHandle   mSolidShaderLocalCameraPos;
  HeGrShader::ParamHandle   mTextureShaderLocalCameraPos;
  HeGrShader::ParamHandle   mTextureShaderTexture;
  HeTexture*                mDeviceTextures[ 3 ];
#endif
  HeU32      mScreenWidth;
  HeU32      mScreenHeight;
  HeMat44    mViewMatrix;
  HeMat44    mProjectionMatrix;
  HeMat44    mViewProjectionMatrix;
  HeVec3     mEyePos;
};



#ifdef WIN32
#ifdef RENDER_DEBUG_EXPORTS
#define RENDER_DEBUG_API __declspec(dllexport)
#else
#define RENDER_DEBUG_API __declspec(dllimport)
#endif
#else
#define RENDER_DEBUG_API
#endif


#ifdef WIN32
#include <windows.h>

class RenderDebug;

extern "C"
{
RENDER_DEBUG_API RenderDebug * getInterface(HeI32 version_number);
};


#endif


static InternalRenderDebug *gInterface=0;

};  // end of namespace

using namespace RENDER_DEBUG;

extern "C"
{
#ifdef PLUGINS_EMBEDDED
RenderDebug * getInterfaceRenderDebug(HeI32 version_number)
#else
RENDER_DEBUG_API RenderDebug * getInterface(HeI32 version_number)
#endif
{
  RenderDebug *ret = 0;
  assert( gInterface == 0 );
  if ( gInterface == 0 && version_number == RENDER_DEBUG_VERSION )
  {
    if ( gInterface == 0 ) gInterface = MEMALLOC_NEW(InternalRenderDebug);
    ret = static_cast<RenderDebug *>(gInterface);
  }
  return ret;
}
};
#ifndef PLUGINS_EMBEDDED
bool doShutdown(void)
{
  bool ret = false;
  if ( gInterface )
  {
    ret = true;
    delete gInterface;
    gInterface = 0;
  }
  return ret;
}
#endif
#ifdef WIN32

#include <windows.h>
#ifndef PLUGINS_EMBEDDED
BOOL APIENTRY DllMain( HANDLE /*hModule*/,
                       DWORD  ul_reason_for_call,
                       LPVOID /*lpReserved*/)
{
  HeI32 ret = 0;

  switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
      ret = 1;
			break;
		case DLL_THREAD_ATTACH:
      ret = 2;
			break;
		case DLL_THREAD_DETACH:
      ret = 3;
			break;
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
#endif

#endif

