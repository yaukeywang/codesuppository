#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TestUntileUV.h"
#include "MeshImport.h"
using namespace NVSHARE;
using namespace physx;
#include "UntileUV.h"
#include "shared/MeshSystem/MeshSystemHelper.h"
#include "RenderDebug.h"

static void
convertVertToUUV( UntileUVMeshVertex& out, const MeshVertex& in )
{
	out.setUV( in.mTexel1[0], in.mTexel1[1] );
	out.setAttribXYZ( 0, in.mPos );
	out.setAttribXYZ( 1, in.mNormal );
	out.setAttribXY( 2, in.mTexel2 );
}

static void
convertVertFromUUV( MeshVertex& out, const UntileUVMeshVertex& in )
{
	for ( int i = 0; i < 2; ++i )
		out.mTexel1[i] = in.mUV[i];

	for ( int i = 0; i < 3; ++i )
		out.mPos[i] = in.mAttributes[0][i];

	for ( int i = 0; i < 3; ++i )
		out.mNormal[i] = in.mAttributes[1][i];

	for ( int i = 0; i < 2; ++i )
		out.mTexel2[i] = in.mAttributes[2][i];
}

static void
processUntileUV( MeshImportInterface *mii, UntileUV* uuv, const char* meshName, const char* materialName )
{
	mii = mii;

	const UntileUVMeshVertex* verts		= uuv->getVerts();
	const NxU32* indices				= uuv->getIndices();

	NxU32 tcount = uuv->getTriCount();
	for ( NxU32 i = 0; i < tcount; ++i )
	{
		NxU32 i1 = indices[i*3+0];
		NxU32 i2 = indices[i*3+1];
		NxU32 i3 = indices[i*3+2];

		MeshVertex v[3];
		convertVertFromUUV( v[0], verts[i1] );
		convertVertFromUUV( v[1], verts[i2] );
		convertVertFromUUV( v[2], verts[i3] );

#if 0
		gRenderDebug->DebugTri(
			v[0].mPos,
			v[1].mPos,
			v[2].mPos );
#endif

		mii->importTriangle( meshName, materialName,
			physx::MIVF_POSITION | physx::MIVF_NORMAL | physx::MIVF_TEXEL1 | physx::MIVF_TEXEL2,
			v[0], v[1], v[2] );
	}
}

physx::MeshSystemContainer*
testUntileUV( physx::MeshImport* mimport, MeshSystemHelper * msh )
{
	SEND_TEXT_MESSAGE(0,"Demonstrates how to remove tiling UVs from a mesh\r\n");

	assert( mimport );
	physx::MeshSystemContainer* omsc = mimport->createMeshSystemContainer(); // create an empty mesh system container.
	physx::MeshImportInterface* omii = mimport->getMeshImportInterface(omsc); // get an importer interface for this mesh system container.

	if ( msh )
	{
		gRenderDebug->pushRenderState();
		gRenderDebug->reset( -1 );
		gRenderDebug->setCurrentColor( 0xFFFF00, 0xFFFFFF );
		gRenderDebug->setCurrentDisplayTime( 6000 );

		MeshSystem* ms = msh->getMeshSystem();
		if ( ms )
		{
			// specify the name of the asset.
			omii->importAssetName( ms->mAssetName, ms->mAssetInfo );

			for ( NxU32 i = 0; i < ms->mMeshCount; ++i )
			{
				//const MeshMaterial& mat = ms->mMaterials[i];

				// specify the name of a material and any associated material meta data
				//omii->importMaterial( mat.mName, mat.mMetaData );
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

#if 0
						gRenderDebug->DebugTri(
							verts[0]->mPos,
							verts[1]->mPos,
							verts[2]->mPos );
#else
						UntileUVMeshVertex v[3];
						convertVertToUUV( v[0], *verts[0] );
						convertVertToUUV( v[1], *verts[1] );
						convertVertToUUV( v[2], *verts[2] );

						UntileUV* uuv = createUntileUV();
						uuv->untile( v, 3 );

						processUntileUV( omii, uuv, m->mName, sm->mMaterialName );

						releaseUntileUV( uuv );
#endif
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

