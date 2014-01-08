#ifndef VIEW_PROJ_H

#define VIEW_PROJ_H

// This code snippet was released open-source on December 14, 2013 by John W. Ratcliff (mailto: jratcliffscarab@gmail.com)
// If you find this code useful, please send a bitcoin tip to the following address:
//
// BitCoin Tip Jar: 1BsD7og5WKuh8b6eh6CGYAMM9EE8H6wD6d
//
// This code snippet provides an implementation of three common D3DX utility functions which Microsoft does not provide source code for.
// It has *no* dependencies on DirectX, OpenGL, or any math libraries of any kind.  You should be able to simply drop this in to any code
// base you have and just use it as is.  You can also use it for a learning exercise.
//
// This code snippet implements:  D3DXMatrixPerspectiveFovLH : http://msdn.microsoft.com/en-us/library/windows/desktop/bb205350(v=vs.85).aspx
//								  D3DXMatrixOrthoLH : http://msdn.microsoft.com/en-us/library/windows/desktop/bb204940(v=vs.85).aspx
//								  D3DXMatrixLookAtLH : http://msdn.microsoft.com/en-us/library/windows/desktop/bb205342(v=vs.85).aspx
//
// These are the three essential routines needed to build a view and projection matrix, either perspective or orthographic.
// 
// Note, if you *are* using an existing math library, simply cast your 4x4 matrices and vectors as a float pointer to pass it into these routines.
//

#include <math.h> // Need access to the standard math library header for square root and arc tangent functions


// A helper method to compute the magnitude of a vector
inline float vp_magnitude(const float v[3])
{
	return ::sqrtf( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
}

// A helper method to normalize a vector
inline float vp_normalize(float *n) // normalize this vector
{
	float dist = (float)::sqrtf(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
	if ( dist > 0.0000001f )
	{
		float mag = 1.0f / dist;
		n[0]*=mag;
		n[1]*=mag;
		n[2]*=mag;
	}
	else
	{
		n[0] = 1;
		n[1] = 0;
		n[2] = 0;
	}

	return dist;
}

// A helper method to compute the dot-product of two vectors.
inline float vp_dot(const float *p1,const float *p2)
{
	return p1[0]*p2[0]+p1[1]*p2[1]+p1[2]*p2[2];
}

//	This code implements  D3DXMatrixLookAtLH : http://msdn.microsoft.com/en-us/library/windows/desktop/bb205342(v=vs.85).aspx
inline void vp_matrixLookAtLH(float matrix[16],const float vFrom[3],const float vAt[3],const float vWorldUp[3])
{
	// Get the z basis vector, which points straight ahead. This is the
	// difference from the eyepoint to the lookat point.
	float vView[3];

	vView[0] = vAt[0] - vFrom[0];
	vView[1] = vAt[1] - vFrom[1];
	vView[2] = vAt[2] - vFrom[2];

	vp_normalize(vView);

	// Get the dot product, and calculate the projection of the z basis
	// vector onto the up vector. The projection is the y basis vector.
	float fDotProduct = vp_dot(vWorldUp,vView);

	float vUp[3];

	vUp[0] = vWorldUp[0] - fDotProduct*vView[0];
	vUp[1] = vWorldUp[1] - fDotProduct*vView[1];
	vUp[2] = vWorldUp[2] - fDotProduct*vView[2];

	// If this vector has near-zero length because the input specified a
	// bogus up vector, let's try a default up vector
	float fLength;

	if( 1e-6f > ( fLength = vp_magnitude(vUp) ) )
	{
		vUp[0] = 0.0f - vView[1]*vView[0];
		vUp[1] = 1.0f - vView[1]*vView[1];
		vUp[2] = 0.0f - vView[1]*vView[2];

		// If we still have near-zero length, resort to a different axis.
		if( 1e-6f > ( fLength = vp_magnitude(vUp) ) )
		{
			vUp[0] = 0.0f - vView[2]*vView[0];
			vUp[1] = 0.0f - vView[2]*vView[1];
			vUp[2] = 1.0f - vView[2]*vView[2];

			if( 1e-6f > ( fLength = vp_magnitude(vUp) ) )  
			{
				vUp[0] = 1.0f - vView[2]*vView[0];
				vUp[1] = 0.0f - vView[2]*vView[1];
				vUp[2] = 0.0f - vView[2]*vView[2];
				if( 1e-6f > ( fLength = vp_magnitude(vUp) ) )  
				{
					fLength = 0.001f; // just pick an arbitrary length; this really should never happen anyway.
				}
			}
		}
	}

	// Normalize the y basis vector
	float recip = 1.0f / fLength;

	vUp[0]*=recip;
	vUp[1]*=recip;
	vUp[2]*=recip;

	// The x basis vector is found simply with the cross product of the y
	// and z basis vectors

	float vRight[3];

	vRight[0] = vUp[1]*vView[2] - vUp[2]*vView[1];
	vRight[1] = vUp[2]*vView[0] - vUp[0]*vView[2];
	vRight[2] = vUp[0]*vView[1] - vUp[1]*vView[0];

	// Start building the matrix. The first three rows contains the basis
	// vectors used to rotate the view to point at the lookat point

	matrix[0*4+0] = vRight[0];
	matrix[0*4+1] = vUp[0];
	matrix[0*4+2] = vView[0];
	matrix[0*4+3] = 0;

	matrix[1*4+0] = vRight[1];
	matrix[1*4+1] = vUp[1];
	matrix[1*4+2] = vView[1];
	matrix[1*4+3] = 0;

	matrix[2*4+0] = vRight[2];
	matrix[2*4+1] = vUp[2];
	matrix[2*4+2] = vView[2];
	matrix[2*4+3] = 0;

	// Do the translation values (rotations are still about the eyepoint)

	matrix[3*4+0] = -vp_dot(vFrom,vRight);
	matrix[3*4+1] = -vp_dot(vFrom,vUp);
	matrix[3*4+2] = -vp_dot(vFrom,vView);
	matrix[3*4+3] = 1;

}

//	This code implements D3DXMatrixOrthoLH : http://msdn.microsoft.com/en-us/library/windows/desktop/bb204940(v=vs.85).aspx
inline void vp_matrixOrthoLH(float matrix[16],float width, float height, float nearPlane, float farPlane)
{
	matrix[0]  =  2.0f / width;
	matrix[1] = 0;
	matrix[2] = 0;
	matrix[3] = 0;

	matrix[4] = 0;
	matrix[5]  =  2.0f/height;
	matrix[6] = 0;
	matrix[7] = 0;

	matrix[8] = 0;
	matrix[9] = 0;
	matrix[10] = 1.0f / (farPlane-nearPlane);
	matrix[11] = 0;

	matrix[12] = 0;
	matrix[13] = 0;
	matrix[14] = nearPlane/(nearPlane-farPlane);
	matrix[15] = 1;
}

// This code snippet implements:  D3DXMatrixPerspectiveFovLH : http://msdn.microsoft.com/en-us/library/windows/desktop/bb205350(v=vs.85).aspx
inline void vp_matrixPerspectiveFovLH(float matrix[16],float fov,float aspectRatio,float zn,float zf)
{
	float yScale = 1/tanf(fov/2);
	float xScale = yScale / aspectRatio;

	matrix[0]  = xScale;
	matrix[1] = 0;
	matrix[2] = 0;
	matrix[3] = 0;

	matrix[4] = 0;
	matrix[5]  = yScale;
	matrix[6] = 0;
	matrix[7] = 0;

	matrix[8] = 0;
	matrix[9] = 0;
	matrix[10] = zf / (zf-zn);
	matrix[11] = 1;

	matrix[12] = 0;
	matrix[13] = 0;
	matrix[14] = -zn*zf/(zf-zn);
	matrix[15] = 0;
}


#endif
