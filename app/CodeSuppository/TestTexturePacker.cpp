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
			mWidth = 0;
			mHeight = 0;
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


class TestTexturePacker
{
public:
	TestTexturePacker( MeshSystemHelper* msh )
	{
		mTexturePacker = 0;
		mMsh = msh;
		mPackedTexture = 0;
		mTextures = 0;
		mTexturesMem = 0;
		mTexCount = 0;
	}

	~TestTexturePacker()
	{
		release();
	}

	void release()
	{
		delete [] mTexturesMem;
		mTexturesMem = 0;

		if ( mTextures )
		{
			for ( NxU32 i = 0; i < mTexCount; ++i )
				gPd3d->releaseTexture( mTextures[i] );

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

		mTextures = new Pd3dTexture*[ mTexCount ];

		mTexturesMem = new LockedTexture[ mTexCount ];

		for ( NxU32 i = 0; i < mTexCount; ++i )
		{
			mTextures[i] = 0;

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
				mTextures[i] = gPd3d->createTexture( name, tex.mWidth, tex.mHeight, 4, true );

				tex.unlock();

				if ( !gPd3d->copyTexture( mat->mHandle, mTextures[i] ) )
				{
					assert( false );
					return false;
				}

				if ( !mTexturesMem[i].lock( mTextures[i] ) )
				{
					assert( false );
					return false;
				}

				mTexturePacker->addTexture( mTexturesMem[i].mWidth, mTexturesMem[i].mHeight );
			}
		}

		int wid = 0;
		int hit = 0;
		int area = mTexturePacker->packTextures( wid, hit, true, true );
		area = area;

		return createPackedTexture( (NxU32)wid, (NxU32)hit );
	}

	bool	saveTexture( const char* fname )
	{
		mPacked.unlock();
		return gPd3d->saveTextureDDS( mPackedTexture, fname );
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
				blitTexRotated( mPacked, mTexturesMem[ i ], x, y, wid, hit );
			else
				blitTex( mPacked, mTexturesMem[ i ], x, y, wid, hit );
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
		const int bpp( 4 );

		for ( int y = 0; y < th; ++y )
		{
			NxU8* dst = (NxU8*)mPacked.getScanline( y + ty );
			NxU8* src = (NxU8*)tex.getScanline( y );

			dst += bpp*tx;
			memcpy( dst, src, bpp*tw );
		}
	}

	MeshSystemHelper*	mMsh;

	TexturePacker*		mTexturePacker;

	Pd3dTexture**		mTextures;
	LockedTexture*		mTexturesMem;

	LockedTexture		mPacked;
	Pd3dTexture*		mPackedTexture;

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
				NVSHARE::MeshSystemContainer* msc = msh->getMeshSystemContainer();
				NVSHARE::MeshImportInterface* mii = mimport->getMeshImportInterface(msc);
				for ( NxU32 i = 0; i < ms->mMeshCount; ++i )
				{
					const MeshMaterial& mat = ms->mMaterials[i];

					// specify the name of a material and any associated material meta data
					mii->importMaterial( mat.mName, mat.mMetaData );
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
					for ( NxU32 k = 0; k < sm->mTriCount; ++k )
					{
						NxU32 i1 = sm->mIndices[k*3+0];
						NxU32 i2 = sm->mIndices[k*3+1];
						NxU32 i3 = sm->mIndices[k*3+2];
						const MeshVertex* verts[] = {
							&m->mVertices[i1],
							&m->mVertices[i2],
							&m->mVertices[i3]
						};

#if 1
						gRenderDebug->DebugTri(
							verts[0]->mPos,
							verts[1]->mPos,
							verts[2]->mPos );
#endif

						omii->importTriangle( m->mName, sm->mMaterialName,
							NVSHARE::MIVF_POSITION | NVSHARE::MIVF_NORMAL | NVSHARE::MIVF_TEXEL1 | NVSHARE::MIVF_TEXEL2,
							*verts[0], *verts[1], *verts[2] );
					}
				}
			}
		}

		gRenderDebug->popRenderState();
	}

	// gather the contents of the mesh system container
	mimport->gather( omsc ); 

	return omsc;
}

