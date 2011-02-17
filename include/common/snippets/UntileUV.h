#ifndef UNTILE_UV_H
#define UNTILE_UV_H

#include "NxSimpleTypes.h"

class UntileUVMeshVertex
{
public:
	UntileUVMeshVertex()
	{
		mUV[0] = mUV[1] = 0.0f;
		for ( int i = 0; i < 16; ++i )
			for ( int j = 0; j < 4; ++j )
				mAttributes[i][j] = 0.0f;
	}

	void	setUV( NxF32 u, NxF32 v )
	{
		mUV[0] = u;
		mUV[1] = v;
	}

	void	setAttrib( int idx, NxF32 x = 0.0f, NxF32 y = 0.0f, NxF32 z = 0.0f, NxF32 w = 0.0f )
	{
		mAttributes[idx][0] = x;
		mAttributes[idx][1] = y;
		mAttributes[idx][2] = z;
		mAttributes[idx][3] = w;
	}

	void	setAttribX( int idx, const NxF32* p ) { setAttrib( idx, p[0], 0.0f, 0.0f, 0.0f ); }
	void	setAttribXY( int idx, const NxF32* p ) { setAttrib( idx, p[0], p[1], 0.0f, 0.0f ); }
	void	setAttribXYZ( int idx, const NxF32* p ) { setAttrib( idx, p[0], p[1], p[2], 0.0f ); }
	void	setAttribXYZW( int idx, const NxF32* p ) { setAttrib( idx, p[0], p[1], p[2], p[3] ); }

	void	interpolate( const UntileUVMeshVertex& vA, const UntileUVMeshVertex& vB, NxF32 t )
	{
		mUV[0] = vA.mUV[0] + t*(vB.mUV[0] - vA.mUV[0]);
		mUV[1] = vA.mUV[1] + t*(vB.mUV[1] - vA.mUV[1]);

		for ( int i = 0; i < 16; ++i )
		{
			mAttributes[i][0] = vA.mAttributes[i][0] + t*(vB.mAttributes[i][0] - vA.mAttributes[i][0]);
			mAttributes[i][1] = vA.mAttributes[i][1] + t*(vB.mAttributes[i][1] - vA.mAttributes[i][1]);
			mAttributes[i][2] = vA.mAttributes[i][2] + t*(vB.mAttributes[i][2] - vA.mAttributes[i][2]);
			mAttributes[i][3] = vA.mAttributes[i][3] + t*(vB.mAttributes[i][3] - vA.mAttributes[i][3]);
		}
	}

	NxF32	mUV[2];
	NxF32	mAttributes[16][4];
};

class UntileUV
{
public:
	virtual NxU32	untile(
		const UntileUVMeshVertex* vA,
		const UntileUVMeshVertex* vB,
		const UntileUVMeshVertex* vC,
		NxF32 epsilon = 1.0f / 1024.0f
		)=0;

	virtual const UntileUVMeshVertex*	getVerts()=0;
	virtual const NxU32*				getIndices()=0;
	virtual NxU32						getTriCount()=0;
};

UntileUV*		createUntileUV();
void			releaseUntileUV( UntileUV *uuv );

#endif
