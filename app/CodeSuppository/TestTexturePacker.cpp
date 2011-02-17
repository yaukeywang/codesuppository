#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TestTexturePacker.h"
#include "MeshImport.h"
using namespace NVSHARE;
#include "TexturePacker.h"
using namespace TEXTURE_PACKER;
#include "shared/MeshSystem/MeshSystemHelper.h"
#include "RenderDebug.h"

#include "Pd3d/pd3d.h"

class LockedTexture
{
private:
	LockedTexture& operator=( const LockedTexture& );
	LockedTexture( const LockedTexture& );

public:
	LockedTexture()
	{
		mHandle = 0;
		mMem = 0;
		mPitch = 0;
		mWidth = 0;
		mHeight = 0;
	}

	void unlock()
	{
		if ( mMem )
		{
			gPd3d->unlockTexture( mHandle );
			mMem = 0;
			mPitch = 0;
		}
	}

	bool lock( Pd3dTexture* tex )
	{
		unlock();
		if ( tex )
		{
			mMem = gPd3d->lockTexture( tex, mWidth, mHeight, mPitch );
			assert( mMem );
			if ( mMem )
			{
				mHandle = tex;
				return true;
			}
		}
		return false;
	}

	void* getScanline( int y )
	{
		char* mem = (char*)mMem;
		return (void*)( mem + y*mPitch );
	}

	Pd3dTexture*	mHandle;
	void*			mMem;
	NxU32			mPitch;
	NxU32			mWidth;
	NxU32			mHeight;
};


struct MyTextureInfo
{
	MyTextureInfo()
	{
		mHandle = 0;
		mName[0] = 0;
	}

	bool	set( Pd3dTexture* tex, const char* name )
	{
		mHandle = 0;
		mName[0] = 0;
		mMem.unlock();

		if ( !mMem.lock( tex ) )
		{
			assert( false );
			return false;
		}

		mHandle = tex;
		strcpy( mName, name );
		return true;
	}

	Pd3dTexture*	mHandle;
	LockedTexture	mMem;
	char			mName[ 512 ];
};


class TexturePackerScanline
{
public:
	TexturePackerScanline( NxU32 maxWidth )
	{
		mMaxWidth = maxWidth;
		mWidth = 0;
		mScanline = (NxU32*)malloc( sizeof( NxU32 ) * maxWidth );
	}

	~TexturePackerScanline()
	{
		free( mScanline );
	}

	void	set( const NxU32* texel, NxU32 stride, NxU32 count, bool border )
	{
		mWidth = count;
		if ( border )
			mWidth += 2;
		assert( mWidth <= mMaxWidth );

		NxU32* dst = mScanline;
		const NxU32* src = texel;

		if ( border )
			*dst++ = texel[stride*(count - 1)];

		for ( NxU32 i = 0; i < count; ++i, src += stride )
		{
			*dst++ = *src;
		}

		if ( border )
			*dst++ = texel[0];
	}

	NxU32	getWidth() const { return mWidth; }
	NxU32*	getTexels() const { return mScanline; }

private:
	NxU32*	mScanline;
	NxU32	mWidth;
	NxU32	mMaxWidth;
};


class TestTexturePacker
{
public:
	TestTexturePacker( MeshSystemHelper* msh )
		: mScanline( 65536 )
	{
		mTexturePacker = 0;
		mMsh = msh;
		mPackedTexture = 0;
		mTextures = 0;
		mTexCount = 0;
	}

	~TestTexturePacker()
	{
		release();
	}

	void release()
	{
		if ( mTextures )
		{
			for ( NxU32 i = 0; i < mTexCount; ++i )
			{
				mTextures[i].mMem.unlock();
				gPd3d->releaseTexture( mTextures[i].mHandle );
			}

			delete [] mTextures;
			mTextures = 0;
		}

		mPacked.unlock();

		if ( mPackedTexture )
		{
			gPd3d->releaseTexture( mPackedTexture );
			mPackedTexture = 0;
		}

		if ( mTexturePacker )
		{
			releaseTexturePacker( mTexturePacker );
			mTexturePacker = 0;
		}
	}

	bool packTextures()
	{
		release();

		mTexturePacker = createTexturePacker();

		mTexCount = mMsh->getMaterialCount();

		mTexturePacker->setTextureCount( mTexCount );

		mTextures = new MyTextureInfo[ mTexCount ];

		for ( NxU32 i = 0; i < mTexCount; ++i )
		{
			NVSHARE::Pd3dMaterial* mat = mMsh->getMaterial( i );
			assert( mat && mat->mHandle );
			if ( mat && mat->mHandle )
			{

				LockedTexture tex;
				if ( !tex.lock( mat->mHandle ) )
				{
					assert( false );
					return false;
				}

				char name[512];
				strcpy( name, mat->mTexture );
				strcat( name, ".argb" );
				Pd3dTexture* uncompressedTex = gPd3d->createTexture( name, tex.mWidth, tex.mHeight, 4, true );

				tex.unlock();

				if ( !gPd3d->copyTexture( mat->mHandle, uncompressedTex ) )
				{
					assert( false );
					return false;
				}

				mTextures[i].set( uncompressedTex, mat->mTexture );

				mTexturePacker->addTexture( mTextures[i].mMem.mWidth, mTextures[i].mMem.mHeight );
			}
		}

		int wid = 0;
		int hit = 0;
		int area = mTexturePacker->packTextures( wid, hit, true, true );
		area = area;

		mPackedW = wid;
		mPackedH = hit;

		return createPackedTexture( (NxU32)wid, (NxU32)hit );
	}

	bool	saveTexture( const char* fname )
	{
		mPacked.unlock();
		return gPd3d->saveTextureDDS( mPackedTexture, fname );
	}

	bool	remapUVs( NVSHARE::MeshImportInterface* omii, Mesh* m, SubMesh* sm )
	{
		NxI32 texIdx = getTexIdx( sm->mMaterialName );
		assert( texIdx >= 0 && texIdx < (NxI32)mTexCount );
		if ( texIdx < 0 || texIdx >= (NxI32)mTexCount )
			return false;

		for ( NxU32 k = 0; k < sm->mTriCount; ++k )
		{
			NxU32 i1 = sm->mIndices[k*3+0];
			NxU32 i2 = sm->mIndices[k*3+1];
			NxU32 i3 = sm->mIndices[k*3+2];
			MeshVertex verts[] = {
				m->mVertices[i1],
				m->mVertices[i2],
				m->mVertices[i3]
			};

#if 0
			gRenderDebug->DebugTri(
				verts[0].mPos,
				verts[1].mPos,
				verts[2].mPos );
#endif

			remapVert( texIdx, verts[0] );
			remapVert( texIdx, verts[1] );
			remapVert( texIdx, verts[2] );

			omii->importTriangle( m->mName, "atlas.dds",
				NVSHARE::MIVF_POSITION | NVSHARE::MIVF_NORMAL | NVSHARE::MIVF_TEXEL1 | NVSHARE::MIVF_TEXEL2,
				verts[0], verts[1], verts[2] );
		}

		return true;
	}

	void	remapVert( NxI32 idx, MeshVertex& v )
	{
		int x, y, w, h;
		if ( mTexturePacker->getTextureLocation( idx, x, y, w, h ) )
		{
			assert( !"TODO: handle rotated" );
		}
		else
		{
			float scaleU = w / (NxF32)mPackedW;
			float scaleV = h / (NxF32)mPackedH;
			float offU = x / (NxF32)mPackedW;
			float offV = y / (NxF32)mPackedH;

			v.mTexel1[0] *= scaleU;
			v.mTexel1[0] += offU;

			v.mTexel1[1] *= scaleV;
			v.mTexel1[1] += offV;
		}
	}

	NxI32	getTexIdx( const char* name )
	{
		for ( NxU32 i = 0; i < mTexCount; ++i )
		{
			if ( strcmp( mTextures[i].mName, name ) == 0 )
				return i;
		}
		return -1;
	}

private:
	bool	createPackedTexture( NxU32 width, NxU32 height )
	{
		mPackedTexture = gPd3d->createTexture( "atlas", width, height, 4, true );

		bool ret = mPacked.lock( mPackedTexture );
		assert( ret );
		if ( !ret )
			return false;

		for ( NxU32 i = 0; i < mTexCount; ++i )
		{
			int x, y, wid, hit;
			if ( mTexturePacker->getTextureLocation( i, x, y, wid, hit ) )
				blitTexRotated( mPacked, mTextures[ i ].mMem, x, y, wid, hit );
			else
				blitTex( mPacked, mTextures[ i ].mMem, x, y, wid, hit );
		}

		return true;
	}

#pragma warning(disable:4100)
	void	blitTexRotated( LockedTexture& packed, LockedTexture& tex, int tx, int ty, int tw, int th )
	{
		assert( !"TODO: not implemented" );
	}

	void	blitTex( LockedTexture& packed, LockedTexture& tex, int tx, int ty, int tw, int th )
	{
		blitScanline( packed, tex,
			th-1,
			tx, ty-1, tw );

		for ( int y = 0; y < th; ++y )
		{
			blitScanline( packed, tex,
				y,
				tx, ty + y, tw );
		}

		blitScanline( packed, tex,
			0,
			tx, ty + th, tw );
	}

	void	blitScanline( LockedTexture& packed, LockedTexture& tex, int y, int tx, int ty, int tw )
	{
		const int bpp( sizeof( NxU32 ) );

		NxU32* dst = (NxU32*)mPacked.getScanline( ty );
		NxU32* src = (NxU32*)tex.getScanline( y );
		bool border = true;

		mScanline.set( src, 1, tw, border );

		dst += tx;
		if ( border )
			dst -= 1;
		memcpy( dst, mScanline.getTexels(), bpp * mScanline.getWidth() );
	}

	MeshSystemHelper*	mMsh;

	TexturePacker*		mTexturePacker;

	MyTextureInfo*		mTextures;

	LockedTexture		mPacked;
	Pd3dTexture*		mPackedTexture;
	int					mPackedW;
	int					mPackedH;
	TexturePackerScanline	mScanline;

	NxU32				mTexCount;
};

MeshSystemContainer*
testTexturePacker( MeshImport* mimport, MeshSystemHelper * msh )
{
	SEND_TEXT_MESSAGE(0,"Builds a texture atlas\r\n");

	assert( mimport );
	NVSHARE::MeshSystemContainer* omsc = mimport->createMeshSystemContainer(); // create an empty mesh system container.
	NVSHARE::MeshImportInterface* omii = mimport->getMeshImportInterface(omsc); // get an importer interface for this mesh system container.

	if ( msh )
	{
		TestTexturePacker ttp( msh );
		ttp.packTextures();
		ttp.saveTexture( "atlas.dds" );

		gRenderDebug->pushRenderState();
		gRenderDebug->reset( -1 );
		gRenderDebug->setCurrentColor( 0xFFFF00, 0xFFFFFF );
		gRenderDebug->setCurrentDisplayTime( 6000 );

		MeshSystem* ms = msh->getMeshSystem();
		if ( ms )
		{
			// specify the name of the asset.
			omii->importAssetName( ms->mAssetName, ms->mAssetInfo );

			{
				for ( NxU32 i = 0; i < ms->mMeshCount; ++i )
				{
					//const MeshMaterial& mat = ms->mMaterials[i];

					// specify the name of a material and any associated material meta data
					//omii->importMaterial( mat.mName, mat.mMetaData );
				}
			}

			for ( NxU32 i = 0; i < ms->mMeshCount; ++i )
			{
				Mesh* m = ms->mMeshes[i];
				// specify the name of the mesh, and the skeleton associated with that mesh.
				omii->importMesh( m->mName, m->mSkeletonName );

				for ( NxU32 j = 0; j < m->mSubMeshCount; ++j )
				{
					SubMesh* sm = m->mSubMeshes[j];
					ttp.remapUVs( omii, m, sm );
				}
			}
		}

		gRenderDebug->popRenderState();
	}

	// gather the contents of the mesh system container
	mimport->gather( omsc ); 

	return omsc;
}

