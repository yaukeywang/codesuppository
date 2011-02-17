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

	void		SetUV( float u, float v )
	{
		mUV[0] = u;
		mUV[1] = v;
	}

	void		SetAttrib( int idx, float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f )
	{
		mAttributes[idx][0] = x;
		mAttributes[idx][1] = y;
		mAttributes[idx][2] = z;
		mAttributes[idx][3] = w;
	}

	void		SetAttribX( int idx, const float* p ) { SetAttrib( idx, p[0], 0.0f, 0.0f, 0.0f ); }
	void		SetAttribXY( int idx, const float* p ) { SetAttrib( idx, p[0], p[1], 0.0f, 0.0f ); }
	void		SetAttribXYZ( int idx, const float* p ) { SetAttrib( idx, p[0], p[1], p[2], 0.0f ); }
	void		SetAttribXYZW( int idx, const float* p ) { SetAttrib( idx, p[0], p[1], p[2], p[3] ); }


	NxF32	mUV[2];
	NxF32	mAttributes[16][4];
};

class UntileUV
{
public:
	virtual NxU32	untile(
		const UntileUVMeshVertex* vA,
		const UntileUVMeshVertex* vB,
		const UntileUVMeshVertex* vC )=0;

	virtual const UntileUVMeshVertex*	getVerts()=0;
	virtual const NxU32*				getIndices()=0;
	virtual NxU32						getTriCount()=0;
};

UntileUV*		createUntileUV();
void			releaseUntileUV( UntileUV *uuv );

#endif
