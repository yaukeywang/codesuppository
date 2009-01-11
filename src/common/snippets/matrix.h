#ifndef MY_MATRIX_H

#define MY_MATRIX_H

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

#include <string.h> // need memcpy
#include <stdio.h>
#include <assert.h>

#include "common/snippets/UserMemAlloc.h"
#include "vector.h"

/** Represents a 4x4 rotation/translation matrix .*/
class Quat;
class MyMatrix
{
	friend class Quat; // quaternions can directly modify contents of matrices
public:
	MyMatrix(void)
	{
		Identity();
	};

	MyMatrix(const HeF32 *mem)
	{
		Set(mem);
	}

	MyMatrix(const MyMatrix &m,const Vector3d<HeF32> &t)
	{
		memcpy(mElement,m.mElement,sizeof(HeF32)*4*4);
		mElement[3][0]-=t.x;
		mElement[3][1]-=t.y;
		mElement[3][2]-=t.z;
	}

	MyMatrix(const HeF32 *quat,const HeF32 *pos); // init from a const float pointer to a quat and translation..

	const HeF32 * GetFloat(void) const { return &mElement[0][0]; };

	HeF32 * Ptr(void) { return &mElement[0][0]; };
	const HeF32 * CPtr(void) const { return &mElement[0][0]; };

	HeF32 Get(HeI32 i,HeI32 j) const
	{
		return mElement[i][j];
	};

	void Set(HeI32 i,HeI32 j,HeF32 v)
	{
		mElement[i][j] = v;
	};

	void Set(const char *foo);

	// just copy the matrix exactly as a block of memory.
	void Set(const HeF32 *matrix)
	{
		memcpy(mElement,matrix,sizeof(HeF32)*16);
	}

	void Identity(void)
	{
		mElement[0][0] = 1;
		mElement[1][1] = 1;
		mElement[2][2] = 1;
		mElement[3][3] = 1;

		mElement[1][0] = 0;
		mElement[2][0] = 0;
		mElement[3][0] = 0;

		mElement[0][1] = 0;
		mElement[2][1] = 0;
		mElement[3][1] = 0;

		mElement[0][2] = 0;
		mElement[1][2] = 0;
		mElement[3][2] = 0;

		mElement[0][3] = 0;
		mElement[1][3] = 0;
		mElement[2][3] = 0;

	};

	void Set(const Vector3d<HeF32>& m0,const Vector3d<HeF32> &m1,const Vector3d<HeF32> &m2)
	{
		mElement[0][0] = m0.x;
		mElement[0][1] = m0.y;
		mElement[0][2] = m0.z;
		mElement[0][3] = 0;

		mElement[1][0] = m1.x;
		mElement[1][1] = m1.y;
		mElement[1][2] = m1.z;
		mElement[1][3] = 0;

		mElement[2][0] = m2.x;
		mElement[2][1] = m2.y;
		mElement[2][2] = m2.z;
		mElement[2][3] = 0;

		mElement[3][0] = 0;
		mElement[3][1] = 0;
		mElement[3][2] = 0;
		mElement[3][3] = 1;
	}

	void Rotate(HeF32 x,HeF32 y,HeF32 z); // euler rotation in radians
	// set rotation using angle axis notation.
	void Rotate(HeF32 angle,HeF32 x,HeF32 y,HeF32 z);
	void GetEulerAngles(HeF32 &r,HeF32 &p,HeF32 &y) const;

	void Multiply(const MyMatrix& t1,const MyMatrix &t2)
	{
		const HeF32* pA = &t1.mElement[0][0];
		const HeF32* pB = &t2.mElement[0][0];
		HeF32* pM = &mElement[0][0];

		memset(pM, 0, sizeof(HeF32)*16);

		for(HeI32 i=0; i<4; i++ )
				for(HeI32 j=0; j<4; j++ )
						for(HeI32 k=0; k<4; k++ )
								pM[4*i+j] +=  pA[4*i+k] * pB[4*k+j];

	};

	// rls - special case only multiplies the 3x3 rotation.
	void MultiplyRotate(const MyMatrix& t1,const MyMatrix &t2)
	{
		const HeF32* pA = &t1.mElement[0][0];
		const HeF32* pB = &t2.mElement[0][0];
		HeF32* pM = &mElement[0][0];

		memset(pM, 0, sizeof(HeF32)*16);

		for(HeI32 i=0; i<3; i++ )
			for(HeI32 j=0; j<3; j++ )
				for(HeI32 k=0; k<3; k++ )
					pM[4*i+j] +=  pA[4*i+k] * pB[4*k+j];

	};


  inline HeF32 TransformZ(HeF32 x,HeF32 y,HeF32 z) const
  {
 	  return (mElement[0][2] * x) +  (mElement[1][2] * y) +  (mElement[2][2] * z) + mElement[3][2];
  }

	inline	Vector3d<HeF32>		Transform(const Vector3d<HeF32> & v) const
	{
		Vector3d<HeF32> t;

 	  t.x = (mElement[0][0] * v.x) +
 				  (mElement[1][0] * v.y) +
 				  (mElement[2][0] * v.z) + mElement[3][0];

 	  t.y = (mElement[0][1] * v.x) +
 				  (mElement[1][1] * v.y) +
 				  (mElement[2][1] * v.z) + mElement[3][1];

 	  t.z = (mElement[0][2] * v.x) +
 				  (mElement[1][2] * v.y) +
 				  (mElement[2][2] * v.z) + mElement[3][2];

		return t;
	};

	void Transform(const Vector3d<HeF32> & v,Vector3d<HeF32> &t) const
	{
 	  t.x = (mElement[0][0] * v.x) +
 				  (mElement[1][0] * v.y) +
 				  (mElement[2][0] * v.z) + mElement[3][0];

 	  t.y = (mElement[0][1] * v.x) +
 				  (mElement[1][1] * v.y) +
 				  (mElement[2][1] * v.z) + mElement[3][1];

 	  t.z = (mElement[0][2] * v.x) +
 				  (mElement[1][2] * v.y) +
 				  (mElement[2][2] * v.z) + mElement[3][2];

	};

	void Transform(const HeF32 *v,HeF32 *t) const
	{
 	  t[0] = (mElement[0][0] * v[0]) +
 				  (mElement[1][0] * v[1]) +
 				  (mElement[2][0] * v[2]) + mElement[3][0];

 	  t[1] = (mElement[0][1] * v[0]) +
 				  (mElement[1][1] * v[1]) +
 				  (mElement[2][1] * v[2]) + mElement[3][1];

 	  t[2] = (mElement[0][2] * v[0]) +
 				  (mElement[1][2] * v[1]) +
 				  (mElement[2][2] * v[2]) + mElement[3][2];

	};

	void Transform(const Vector3d<HeF32> & v,Vector3d<HeF32> &t,const Vector3d<HeF32> &o) const
	{
 	  t.x = (mElement[0][0] * v.x) +
 				  (mElement[1][0] * v.y) +
 				  (mElement[2][0] * v.z) + (mElement[3][0]-o.x);

 	  t.y = (mElement[0][1] * v.x) +
 				  (mElement[1][1] * v.y) +
 				  (mElement[2][1] * v.z) + (mElement[3][1]-o.y);

 	  t.z = (mElement[0][2] * v.x) +
 				  (mElement[1][2] * v.y) +
 				  (mElement[2][2] * v.z) + (mElement[3][2]-o.z);

	};

	void Transform(const Vector2d<HeF32> & v,Vector2d<HeF32> &t) const
	{

 	  t.x = (mElement[0][0] * v.x) +
 				  (mElement[1][0] * v.y) + mElement[2][0];

 	  t.y = (mElement[0][1] * v.x) +
 				  (mElement[1][1] * v.y) + mElement[2][1];

	};

	inline void TransformRotateOnly(const Vector3d<HeF32> &v, Vector3d<HeF32> &t) const
	{
		//Rotate the vector, but do not translate it
 	  t.x = (mElement[0][0] * v.x) +
 				  (mElement[1][0] * v.y) +
 				  (mElement[2][0] * v.z);

 	  t.y = (mElement[0][1] * v.x) +
 				  (mElement[1][1] * v.y) +
 				  (mElement[2][1] * v.z);

 	  t.z = (mElement[0][2] * v.x) +
 				  (mElement[1][2] * v.y) +
 				  (mElement[2][2] * v.z);
	}

	inline void TransformRotateOnly(const HeF32 *v,HeF32 *t) const
	{
		//Rotate the vector, but do not translate it
 	  t[0] = (mElement[0][0] * v[0]) +
 				  (mElement[1][0] * v[1]) +
 				  (mElement[2][0] * v[2]);

 	  t[1] = (mElement[0][1] * v[0]) +
 				  (mElement[1][1] * v[1]) +
 				  (mElement[2][1] * v[2]);

 	  t[2] = (mElement[0][2] * v[0]) +
 				  (mElement[1][2] * v[1]) +
 				  (mElement[2][2] * v[2]);
	}

	void ZeroMatrix(void)
	{
		memset(mElement,0,sizeof(HeF32)*16);
	};


	void GetPositionFromViewMatrix(Vector3d<HeF32> &pos) const
	{
		pos.x=-(mElement[3][0]*mElement[0][0] + mElement[3][1]*mElement[0][1] + mElement[3][2]*mElement[0][2]);
		pos.y=-(mElement[3][0]*mElement[1][0] + mElement[3][1]*mElement[1][1] + mElement[3][2]*mElement[1][2]);
		pos.z=-(mElement[3][0]*mElement[2][0] + mElement[3][1]*mElement[2][1] + mElement[3][2]*mElement[2][2]);
	}

	//-----------------------------------------------------------------------------
	// Name: D3DUtil_SetProjectionMatrix()
	// Desc: Sets the passed in 4x4 matrix to a perpsective projection matrix built
	//       from the field-of-view (fov, in y), aspect ratio, near plane (D),
	//       and far plane (F). Note that the projection matrix is normalized for
	//       element [3][4] to be 1.0. This is performed so that W-based range fog
	//       will work correctly.
	//-----------------------------------------------------------------------------
	void SetProjectionMatrix(HeF32 fFOV,
													 HeF32 fAspect,
													 HeF32 fNearPlane,
													 HeF32 fFarPlane)
	{
		if( fabsf(fFarPlane-fNearPlane) < 0.01f ) return;
		if( fabsf(sinf(fFOV*0.5f)) < 0.01f ) return;

		HeF32 w = fAspect * ( cosf(fFOV*0.5f)/sinf(fFOV*0.5f) );
		HeF32 h =   1.0f  * ( cosf(fFOV*0.5f)/sinf(fFOV*0.5f) );
		HeF32 Q = fFarPlane / ( fFarPlane - fNearPlane );

		ZeroMatrix();

		mElement[0][0] = w;
		mElement[1][1] = h;
		mElement[2][2] = Q;
		mElement[2][3] = 1.0f;
		mElement[3][2] = -Q*fNearPlane;
	}

	void SetViewMatrix(const Vector3d<HeF32> &eye,
										 const Vector3d<HeF32> &look,
										 const Vector3d<HeF32> &up)
	{
		Vector3d<HeF32> vFrom    = eye;
		Vector3d<HeF32> vAt      = look;
		Vector3d<HeF32> vWorldUp = up;

		// Get the z basis vector, which points straight ahead. This is the
		// difference from the eyepoint to the lookat point.
		Vector3d<HeF32> vView;

		vView.x = vAt.x - vFrom.x;
		vView.y = vAt.y - vFrom.y;
		vView.z = vAt.z - vFrom.z;

		HeF32 fLength = vView.Magnitude();

		if ( fLength < 1e-6f ) return; // don't set it, it's bogus.

		// Normalize the z basis vector
		HeF32 recip = 1.0f /fLength;
		vView.x*=recip;
		vView.y*=recip;
		vView.z*=recip;

		// Get the dot product, and calculate the projection of the z basis
		// vector onto the up vector. The projection is the y basis vector.
		HeF32 fDotProduct = vWorldUp.Dot(vView);
		Vector3d<HeF32> vUp;

		vUp.x = vWorldUp.x - fDotProduct*vView.x;
		vUp.y = vWorldUp.y - fDotProduct*vView.y;
		vUp.z = vWorldUp.z - fDotProduct*vView.z;

		// If this vector has near-zero length because the input specified a
		// bogus up vector, let's try a default up vector
		if( 1e-6f > ( fLength = vUp.Magnitude() ) )
		{
			vUp.x = 0.0f - vView.y*vView.x;
			vUp.y = 1.0f - vView.y*vView.y;
			vUp.z = 0.0f - vView.y*vView.z;

			// If we still have near-zero length, resort to a different axis.
			if( 1e-6f > ( fLength = vUp.Magnitude() ) )
			{
				vUp.x = 0.0f - vView.z*vView.x;
				vUp.y = 0.0f - vView.z*vView.y;
				vUp.z = 1.0f - vView.z*vView.z;

				if( 1e-6f > ( fLength = vUp.Magnitude() ) )  return;
			}
		}

		// Normalize the y basis vector
		recip = 1.0f / fLength;
		vUp.x*=recip;
		vUp.y*=recip;
		vUp.z*=recip;

		// The x basis vector is found simply with the cross product of the y
		// and z basis vectors
		Vector3d<HeF32> vRight;
		vRight.x = vUp.y*vView.z - vUp.z*vView.y;
		vRight.y = vUp.z*vView.x - vUp.x*vView.z;
		vRight.z = vUp.x*vView.y - vUp.y*vView.x;

		// Start building the matrix. The first three rows contains the basis
		// vectors used to rotate the view to point at the lookat point
		Identity();

		mElement[0][0] = vRight.x;
		mElement[0][1] = vUp.x;
		mElement[0][2] = vView.x;
		mElement[1][0] = vRight.y;
		mElement[1][1] = vUp.y;
		mElement[1][2] = vView.y;
		mElement[2][0] = vRight.z;
		mElement[2][1] = vUp.z;
		mElement[2][2] = vView.z;

		// Do the translation values (rotations are still about the eyepoint)
		mElement[3][0] = - vFrom.Dot(vRight);
		mElement[3][1] = - vFrom.Dot(vUp);
		mElement[3][2] = - vFrom.Dot(vView);
	};


	void LookAtMatrix(const Vector3d<HeF32> &from,const Vector3d<HeF32> &to,const Vector3d<HeF32> &up)
	{
		Identity();

		Vector3d<HeF32> row2 = to - from;
		row2.Normalize();

		Vector3d<HeF32> row0;
		Vector3d<HeF32> row1;

    row0.Cross( up, row2 );
    row1.Cross( row2, row0 );

    row0.Normalize();
    row1.Normalize();

		mElement[0][0] = row0.x;
		mElement[0][1] = row0.y;
		mElement[0][2] = row0.z;

		mElement[1][0] = row1.x;
		mElement[1][1] = row1.y;
		mElement[1][2] = row1.z;

		mElement[2][0] = row2.x;
		mElement[2][1] = row2.y;
		mElement[2][2] = row2.z;

		mElement[3][0] = from.x;
		mElement[3][1] = from.y;
		mElement[3][2] = from.z;

	}

	void LookAt (HeF32 eyex,HeF32 eyey,HeF32 eyez,
							 HeF32 centerx,HeF32 centery,HeF32 centerz,
							 HeF32 upx,HeF32 upy,HeF32 upz)
	{
		Vector3d<HeF32> vLookatPt(centerx,centery,centerz);
		Vector3d<HeF32> vEyePt(eyex,eyey,eyez);
		Vector3d<HeF32> vUpVec(upx,upy,upz);
		SetViewMatrix(vEyePt,vLookatPt,vUpVec);
	}

	void SetScale(const Vector3d<HeF32> &p)
	{
		MyMatrix work;

		work.mElement[0][0] = p.x;
  	work.mElement[1][1] = p.y;
	  work.mElement[2][2] = p.z;

		MyMatrix tmp;
		tmp.Multiply(*this,work);

		*this = tmp;
	}

	void SetScale(HeF32 x,HeF32 y,HeF32 z)
	{
		MyMatrix work;

		work.mElement[0][0] = x;
  	work.mElement[1][1] = y;
	  work.mElement[2][2] = z;

		MyMatrix tmp;
		tmp.Multiply(*this,work);

		*this = tmp;
	}


	void SetTranslation(HeF32 tx,HeF32 ty,HeF32 tz)
	{
		mElement[3][0] = tx;
		mElement[3][1] = ty;
		mElement[3][2] = tz;
	};

	void SetTranslation(const Vector3d<HeF32>& pos)
	{
		mElement[3][0] = pos.GetX();
		mElement[3][1] = pos.GetY();
		mElement[3][2] = pos.GetZ();
	};

	inline bool operator == (const MyMatrix& t) const
	{
		for (HeI32 r=0; r < 3; r++)
		{
			for (HeI32 c=0; c < 4; c++)
			{
				if (mElement[r][c] != t.mElement[r][c])
				{
					return false;
				}
			}
		}
		return true;
	};

	inline bool operator != (const MyMatrix& t) const
	{
		for (HeI32 r=0; r < 3; r++)
		{
			for (HeI32 c=0; c < 4; c++)
			{
				if (mElement[r][c] != t.mElement[r][c])
				{
					return true;
				}
			}
		}
		return false;
	};

	void Transpose(void)
	{
		HeF32 hold[4][4];

		memcpy(hold,mElement,sizeof(HeF32)*4*4);

		mElement[0][0] = hold[0][0];
		mElement[0][1] = hold[1][0];
		mElement[0][2] = hold[2][0];
		mElement[0][3] = hold[3][0];

		mElement[1][0] = hold[0][1];
		mElement[1][1] = hold[1][1];
		mElement[1][2] = hold[2][1];
		mElement[1][3] = hold[3][1];

		mElement[2][0] = hold[0][2];
		mElement[2][1] = hold[1][2];
		mElement[2][2] = hold[2][2];
		mElement[2][3] = hold[3][2];

		mElement[3][0] = hold[0][3];
		mElement[3][1] = hold[1][3];
		mElement[3][2] = hold[2][3];
		mElement[3][3] = hold[3][3];
	};

	void	GetTranspose(MyMatrix &transpose) const
	{
		for (HeI32 i=0; i < 3; i++)
		{
			for (HeI32 j=0; j < 3; j++)
			{
				transpose.mElement[j][i] = mElement[i][j];
			}
			transpose.mElement[3][i] = -mElement[3][i];
		}
	};

	void Get3x3Transpose(MyMatrix &transpose) const
	{
		for (HeI32 i=0; i < 3; i++)
		{
			for (HeI32 j=0; j < 3; j++)
			{
				transpose.mElement[j][i] = mElement[i][j];
			}
			transpose.mElement[3][i] = 0;
		}
		transpose.mElement[3][3] = 1;
	}

	void getSubMatrix( const HeI32 ki, const HeI32 kj,MyMatrix &pDst ) const
	{
		HeI32 row, col;
		HeI32 dstCol = 0, dstRow = 0;

		for ( col = 0; col < 4; col++ )
		{
			if ( col == kj )
			{
				continue;
			}
			for ( dstRow = 0, row = 0; row < 4; row++ )
			{
				if ( row == ki )
				{
					continue;
				}
				pDst.mElement[dstCol][dstRow] = mElement[col][row];
				dstRow++;
			}
			dstCol++;
		}
	}


	HeF32 getDeterminant(void) const
	{
		Vector3d< HeF32 > tmpv;

		Vector3d<HeF32> p0( mElement[0][0], mElement[0][1], mElement[0][2] );
		Vector3d<HeF32> p1( mElement[1][0], mElement[1][1], mElement[1][2] );
		Vector3d<HeF32> p2( mElement[2][0], mElement[2][1], mElement[2][2] );

		tmpv.Cross( p1, p2 );

		return p0.Dot( tmpv );
	}

	void Invert(MyMatrix &invert) const
	{
		HeF32 determinant = getDeterminant();
		HE_ASSERT( determinant > 0.0001f );
		determinant = 1.0f / determinant;
		for ( HeI32 i = 0; i < 4; i++ )
		{
			for ( HeI32 j = 0; j < 4; j++ )
			{
				HeI32 sign = 1 - ( ( i + j ) % 2 ) * 2;
				MyMatrix subMat;
				getSubMatrix( i, j, subMat );
				HeF32 subDeterminant = subMat.getDeterminant();
				invert.mElement[i][j] = ( subDeterminant * sign ) * determinant;
			}
		}
	}

	void setFromXZ(const Vector3d<HeF32> &rowX,const Vector3d<HeF32> &rowZ)
	{

    Identity();

		Vector3d<HeF32> rowY;
    rowY.Cross( rowZ, rowX );

		mElement[0][0] = rowX.x;
		mElement[0][1] = rowX.y;
		mElement[0][2] = rowX.z;


    mElement[1][0] = rowY.x;
    mElement[1][1] = rowY.y;
    mElement[1][2] = rowY.z;

    mElement[2][0] = rowZ.x;
    mElement[2][1] = rowZ.y;
    mElement[2][2] = rowZ.z;
	}

	void GetXaxis(Vector3d<HeF32> &axis) const
	{
		axis.x = mElement[0][0];
		axis.y = mElement[0][1];
		axis.z = mElement[0][2];
	}

	void GetYaxis(Vector3d<HeF32> &axis) const
	{
		axis.x = mElement[1][0];
		axis.y = mElement[1][1];
		axis.z = mElement[1][2];
	}

	void GetZaxis(Vector3d<HeF32> &axis) const
	{
		axis.x = mElement[2][0];
		axis.y = mElement[2][1];
		axis.z = mElement[2][2];
	}

	const HeF32 * GetTranslation(void) const
	{
		return &mElement[3][0];
	}

	void GetTranslation(HeF32 *d) const
	{
		d[0] = mElement[3][0];
		d[1] = mElement[3][1];
		d[2] = mElement[3][2];
	}

	void GetTranslation(HeF32 &tx,HeF32 &ty,HeF32 &tz) const
	{
		tx = mElement[3][0];
		ty = mElement[3][1];
		tz = mElement[3][2];
	};

	void GetTranslation(Vector3d<HeF32> &t) const
	{
		t.x = mElement[3][0];
		t.y = mElement[3][1];
		t.z = mElement[3][2];
	};


	// inverse rotate translate a point
	void InverseRotateTranslate(const Vector3d<HeF32> &v,Vector3d<HeF32> &t) const
	{
		// Invert translation of source vector

		HeF32 _x = v.x - mElement[3][0];
		HeF32 _y = v.y - mElement[3][1];
		HeF32 _z = v.z - mElement[3][2];

		// Multiply inverse-translated source vector by inverted rotation transform

		t.x = (mElement[0][0] * _x) +
					(mElement[0][1] * _y) +
					(mElement[0][2] * _z);

		t.y = (mElement[1][0] * _x) +
					(mElement[1][1] * _y) +
					(mElement[1][2] * _z);

		t.z = (mElement[2][0] * _x) +
					(mElement[2][1] * _y) +
					(mElement[2][2] * _z);

	}


	void OrthonormalizeOrientation()
	{
		//Compensate for floating-point error in the matrix by making sure 
		//  our rotation axes are three orthogonal unit vectors
		//Algorithm blatantly ganked from a demo app by Chris Hecker
		Vector3d<HeF32> XAxis(mElement[0][0], mElement[0][1], mElement[0][2]);
		Vector3d<HeF32> YAxis(mElement[1][0], mElement[1][1], mElement[1][2]);
		Vector3d<HeF32> ZAxis(mElement[2][0], mElement[2][1], mElement[2][2]);

		XAxis.Normalize();

		ZAxis.Cross(XAxis, YAxis);
		ZAxis.Normalize();

		YAxis.Cross(ZAxis, XAxis);
		YAxis.Normalize();

		mElement[0][0] = XAxis.x;
		mElement[0][1] = XAxis.y;
		mElement[0][2] = XAxis.z;
		mElement[1][0] = YAxis.x;
		mElement[1][1] = YAxis.y;
		mElement[1][2] = YAxis.z;
		mElement[2][0] = ZAxis.x;
		mElement[2][1] = ZAxis.y;
		mElement[2][2] = ZAxis.z;
	}

	void GLD3D(void) // flip the 3x3 rotation component from D3D to OGL or OGL to D3D format
	{
		HeF32 old[4][4];
		memcpy(old,mElement,sizeof(HeF32)*4*4);
		for (HeI32 i=0; i<3; i++)
		{
			for (HeI32 j=0; j<3; j++)
			{
				mElement[i][j] = old[j][i];
			}
		}
	}


	void Set(const Vector3d<HeF32> &basis,const Vector3d<HeF32> &normal);

	void SetFromXY(const Vector3d<HeF32> &ax,const Vector3d<HeF32> &ay)
	{
		mElement[0][0] = ax.x;
		mElement[0][1] = ax.y;
		mElement[0][2] = ax.z;

		mElement[1][0] = ay.x;
		mElement[1][1] = ay.y;
		mElement[1][2] = ay.z;

		Vector3d<HeF32> az;

		az.Cross(ax,ay);

		mElement[2][0] = az.x;
		mElement[2][1] = az.y;
		mElement[2][2] = az.z;

	}


	// takes an input pose, converts it to a 4x4 matrix..
	// then multiplies that times the parent...
	// then extracts the pose (translation and rotation) from
	// the combined matrix.
	void GetPose(const Vector3d<HeF32> &pos,
							const Quat            &rot,
							Vector3d<HeF32>       &tpos,
							Quat                  &trot) const;

	void Report(const char *header) const;

	void InvertUnscaled(const MyMatrix &invert)
	{
			/* Given T = (R,p), calculates T' = (R', p') where R' = transpose(R), p' = -R'p
				 I'm sure this can be done in fewer than 5 lines!

				 NB if matrix has any scale or shear, this produces garbage.
			*/

			invert.Get3x3Transpose(*this);

			Vector3d<HeF32> p, q;
			invert.GetTranslation(p);
			TransformRotateOnly(p,q);
			SetTranslation(-q);
	}

	// get rows of the transformation matrix in a hardware friendly manner
	void GetRow0(HeF32 *row0) const
	{
		row0[0] = mElement[0][0];
		row0[1] = mElement[1][0];
		row0[2] = mElement[2][0];
		row0[3] = mElement[3][0];
	}

	void GetRow1(HeF32 *row1) const
	{
		row1[0] = mElement[0][1];
		row1[1] = mElement[1][1];
		row1[2] = mElement[2][1];
		row1[3] = mElement[3][1];
	}

	void GetRow2(HeF32 *row2) const
	{
		row2[0] = mElement[0][2];
		row2[1] = mElement[1][2];
		row2[2] = mElement[2][2];
		row2[3] = mElement[3][2];
	}


	// Converts a position in worldspace to screen space.  Appropiate for use on view*projection matrices.
	bool WorldToScreen(const HeF32 *p,HeF32 screenwid,HeF32 screenhit,HeF32 &w,HeF32 *screen) const
	{
		HeF32 wp = mElement[0][3]*p[0] + mElement[1][3]*p[1] + mElement[2][3]*p[2] + mElement[3][3];

		if ( wp < 0.2f )
		{
 			w = -0.1f;
 			return false;	// don't divide by zero
 		}

		HeF32 xp = mElement[0][0]*p[0] + mElement[1][0]*p[1] + mElement[2][0]*p[2] + mElement[3][0];
		HeF32 yp = mElement[0][1]*p[0] + mElement[1][1]*p[1] + mElement[2][1]*p[2] + mElement[3][1];
		HeF32 zp = mElement[0][2]*p[0] + mElement[1][2]*p[1] + mElement[2][2]*p[2] + mElement[3][2];

 		HeF32 wwp = 1.0f / wp;

		screen[0] = (1.0f + (xp*wwp) ) * screenwid;
		screen[1] = (1.0f - (yp*wwp) ) * screenhit;
		screen[2] = zp*wwp;

		w      = wwp;

		return true;
	}



//private:
	HeF32 mElement[4][4];
};

extern MyMatrix gIdentity;

#endif
