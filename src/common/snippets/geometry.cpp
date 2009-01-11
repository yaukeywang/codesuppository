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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "geometry.h"

static const HeF32 PI=3.141592654f;


HeF32 * GeometryShape::CreateSphere(HeF32 r,HeI32 stepsize,HeI32 &vcount)
{
	HeF32 a = 0;
	HeF32 astep = (PI*2)/ (HeF32)stepsize;

	vcount = stepsize*stepsize;

	HeF32 *points = (HeF32 *) MEMALLOC_MALLOC(sizeof(HeF32)*stepsize*stepsize*3);

	for (HeI32 i=0; i<stepsize; i++)
	{
		HeF32 b = 0;
		HeF32 bstep = (PI*2)/stepsize;

		for (HeI32 j=0; j<stepsize; j++)
		{

			HeF32 p[3];

			p[0] = r*sinf(a)*cosf(b);
			p[1] = r*sinf(a)*sinf(b);
			p[2] = r*cosf(a);

			HeI32 index = (i*stepsize)+j;
			points[index*3+0] = p[0];
			points[index*3+1] = p[1];
			points[index*3+2] = p[2];

			b+=bstep;
		}

		a+=astep;
	}

	return points;
}

static void SwapYZ(HeF32 *p)
{
	HeF32 t = p[1];
	p[1] = p[2];
	p[2] = t;
}

static void SwapXZ(HeF32 *p)
{
	HeF32 t = p[0];
	p[0] = p[2];
	p[2] = t;
}


HeF32 * GeometryShape::CreateCylinder(HeF32 radius,           // radius of cylinder
																			HeF32 height,           // height of cylinder
																			GS_AXIS axis,           // dominant axis of the cylinder
																			HeI32 stepsize,           // step size
																			HeI32 &vcount)           // number of vertices produced
{
	vcount = stepsize*2;

	HeF32 a = 0;
	HeF32 astep = (PI*2)/ (HeF32)stepsize;

	HeF32 *points = (HeF32 *) MEMALLOC_MALLOC(sizeof(HeF32)*vcount*3);

	for (HeI32 i=0; i<stepsize; i++)
	{
		HeF32 p1[3];
		HeF32 p2[3];

		p1[0] = radius*cosf(a);
		p1[1] = radius*sinf(a);
		p1[2] = -height;

		p2[0] = radius*cosf(a);
		p2[1] = radius*sinf(a);
		p2[2] = +height;

		if ( axis == GS_YAXIS )
		{
			SwapYZ(p1);
			SwapYZ(p2);
		}
		else if ( axis == GS_XAXIS )
		{
			SwapXZ(p1);
			SwapXZ(p2);
		}

		HeI32 index = i*2;

		points[index*3+0] = p1[0];
		points[index*3+1] = p1[1];
		points[index*3+2] = p1[2];

		points[index*3+3] = p2[0];
		points[index*3+4] = p2[1];
		points[index*3+5] = p2[2];

		a+=astep;

	}


	return points;
}

HeF32 * GeometryShape::CreateCapsule(HeF32 radius,           // radius of cylinder
																		 HeF32 height,           // height of cylinder
																		 GS_AXIS axis,           // dominant axis of the cylinder
																		 HeI32 stepsize,           // step size
																		 HeI32 &vcount)           // number of vertices produced
{

	HeF32 *points = 0;

#if 0

	HeF32 sides[3];
	sides[0] = radius*2.0f;
	sides[1] = height;
	sides[2] = radius*2.0f;
	return CreateBox(sides,vcount);

#else


	{
		points = CreateSphere(radius, stepsize, vcount);

		HeF32 spread = height*0.5f;

		{
			for (HeI32 i=0; i<vcount; i++)
			{

				HeF32 *p = &points[i*3];

				if ( p[2] < 0 )
				{
					p[2]-=spread;
				}
				else
				{
					p[2]+=spread;
				}

				if ( axis == GS_YAXIS )
				{
					SwapYZ(p);
				}
				else if ( axis == GS_XAXIS )
				{
					SwapXZ(p);
				}

			}

		}


	}
#endif

	return points;
}

static HeF32 *AddPoint(HeF32 *dest,HeF32 x,HeF32 y,HeF32 z)
{
	dest[0] = x;
	dest[1] = y;
	dest[2] = z;
	dest+=3;

	return dest;
};

HeF32 * GeometryShape::CreateBox(const HeF32 *sides,HeI32 &vcount)
{
	vcount = 8;

	HeF32 x1 = -sides[0]*0.5f;
	HeF32 y1 = -sides[1]*0.5f;
	HeF32 z1 = -sides[2]*0.5f;

	HeF32 x2 = +sides[0]*0.5f;
	HeF32 y2 = +sides[1]*0.5f;
	HeF32 z2 = +sides[2]*0.5f;

	HeF32 *points = (HeF32 *) MEMALLOC_MALLOC(sizeof(HeF32)*vcount*3);

	HeF32 *dest = points;

	dest = AddPoint(dest,x1,y1,z1);
	dest = AddPoint(dest,x2,y1,z1);
	dest = AddPoint(dest,x2,y2,z1);
	dest = AddPoint(dest,x1,y2,z1);

	dest = AddPoint(dest,x1,y1,z2);
	dest = AddPoint(dest,x2,y1,z2);
	dest = AddPoint(dest,x2,y2,z2);
	dest = AddPoint(dest,x1,y2,z2);

	return points;
}

HeF32 * GeometryShape::CreateBox(const HeF32 *bmin,const HeF32 *bmax,HeI32 &vcount)
{
	vcount = 8;

	HeF32 x1 = bmin[0];
	HeF32 y1 = bmin[1];
	HeF32 z1 = bmin[2];

	HeF32 x2 = bmax[0];
	HeF32 y2 = bmax[1];
	HeF32 z2 = bmax[2];

	HeF32 *points = (HeF32 *) MEMALLOC_MALLOC(sizeof(HeF32)*vcount*3);

	HeF32 *dest = points;

	dest = AddPoint(dest,x1,y1,z1);
	dest = AddPoint(dest,x2,y1,z1);
	dest = AddPoint(dest,x2,y2,z1);
	dest = AddPoint(dest,x1,y2,z1);

	dest = AddPoint(dest,x1,y1,z2);
	dest = AddPoint(dest,x2,y1,z2);
	dest = AddPoint(dest,x2,y2,z2);
	dest = AddPoint(dest,x1,y2,z2);

	return points;
}


void GeometryShape::TexGenNormal(GeometryVertex &v1,GeometryVertex &v2,GeometryVertex &v3,HeF32 tscale1,HeF32 tscale2)
{

	HeF32 n[3];

	ComputeNormal(v1.mPos,v2.mPos,v3.mPos,n);

	v1.mNormal[0] = n[0];
	v1.mNormal[1] = n[1];
	v1.mNormal[2] = n[2];

	v2.mNormal[0] = n[0];
	v2.mNormal[1] = n[1];
	v2.mNormal[2] = n[2];

	v3.mNormal[0] = n[0];
	v3.mNormal[1] = n[1];
	v3.mNormal[2] = n[2];

	const HeF32 *tp1 = v1.mPos;
	const HeF32 *tp2 = v2.mPos;
	const HeF32 *tp3 = v3.mPos;

	HeI32 i1 = 0;
	HeI32 i2 = 0;

	HeF32 nx = fabsf(n[0]);
	HeF32 ny = fabsf(n[1]);
	HeF32 nz = fabsf(n[2]);

	if ( nx <= ny && nx <= nz ) i1 = 0;
	if ( ny <= nx && ny <= nz ) i1 = 1;
	if ( nz <= nx && nz <= ny ) i1 = 2;

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

	v1.mTexel1[0] = tp1[i1]*tscale1;
	v1.mTexel1[1] = tp1[i2]*tscale1;
	v1.mTexel2[0] = tp1[i1]*tscale2;
	v1.mTexel2[1] = tp1[i2]*tscale2;

	v2.mTexel1[0] = tp2[i1]*tscale1;
	v2.mTexel1[1] = tp2[i2]*tscale1;
	v2.mTexel2[0] = tp2[i1]*tscale2;
	v2.mTexel2[1] = tp2[i2]*tscale2;

	v3.mTexel1[0] = tp3[i1]*tscale1;
	v3.mTexel1[1] = tp3[i2]*tscale1;
	v3.mTexel2[0] = tp3[i1]*tscale2;
	v3.mTexel2[1] = tp3[i2]*tscale2;


}

HeF32 GeometryShape::ComputeNormal(const HeF32 *A,const HeF32 *B,const HeF32 *C,HeF32 *normal)
{
	HeF32 vx,vy,vz,wx,wy,wz,vw_x,vw_y,vw_z,mag;

	vx = (B[0] - C[0]);
	vy = (B[1] - C[1]);
	vz = (B[2] - C[2]);

	wx = (A[0] - B[0]);
	wy = (A[1] - B[1]);
	wz = (A[2] - B[2]);

	vw_x = vy * wz - vz * wy;
	vw_y = vz * wx - vx * wz;
	vw_z = vx * wy - vy * wx;

	mag = sqrtf((vw_x * vw_x) + (vw_y * vw_y) + (vw_z * vw_z));

	if ( mag < 0.000001f )
	{
		mag = 0;
	}
	else
	{
		mag = 1.0f/mag;
	}

	normal[0] = vw_x * mag;
	normal[1] = vw_y * mag;
	normal[2] = vw_z * mag;

	return mag;

}


void GeometryShape::TexGenNormal(GeometryDeformVertex &v1,GeometryDeformVertex &v2,GeometryDeformVertex &v3,HeF32 tscale1,HeF32 tscale2)
{

	HeF32 n[3];

	ComputeNormal(v1.mPos,v2.mPos,v3.mPos,n);

	v1.mNormal[0] = n[0];
	v1.mNormal[1] = n[1];
	v1.mNormal[2] = n[2];

	v2.mNormal[0] = n[0];
	v2.mNormal[1] = n[1];
	v2.mNormal[2] = n[2];

	v3.mNormal[0] = n[0];
	v3.mNormal[1] = n[1];
	v3.mNormal[2] = n[2];

	const HeF32 *tp1 = v1.mPos;
	const HeF32 *tp2 = v2.mPos;
	const HeF32 *tp3 = v3.mPos;

	HeI32 i1 = 0;
	HeI32 i2 = 0;

	HeF32 nx = fabsf(n[0]);
	HeF32 ny = fabsf(n[1]);
	HeF32 nz = fabsf(n[2]);

	if ( nx <= ny && nx <= nz ) i1 = 0;
	if ( ny <= nx && ny <= nz ) i1 = 1;
	if ( nz <= nx && nz <= ny ) i1 = 2;

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

	v1.mTexel1[0] = tp1[i1]*tscale1;
	v1.mTexel1[1] = tp1[i2]*tscale1;
	v1.mTexel2[0] = tp1[i1]*tscale2;
	v1.mTexel2[1] = tp1[i2]*tscale2;

	v2.mTexel1[0] = tp2[i1]*tscale1;
	v2.mTexel1[1] = tp2[i2]*tscale1;
	v2.mTexel2[0] = tp2[i1]*tscale2;
	v2.mTexel2[1] = tp2[i2]*tscale2;

	v3.mTexel1[0] = tp3[i1]*tscale1;
	v3.mTexel1[1] = tp3[i2]*tscale1;
	v3.mTexel2[0] = tp3[i1]*tscale2;
	v3.mTexel2[1] = tp3[i2]*tscale2;


}
