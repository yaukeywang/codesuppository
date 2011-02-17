#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TestTexturePacker.h"
#include "MeshImport.h"
using namespace NVSHARE;
#include "TexturePacker.h"
#include "shared/MeshSystem/MeshSystemHelper.h"
#include "RenderDebug.h"

MeshSystemContainer*
testTexturePacker( MeshImport* mimport, MeshSystemHelper * msh )
{
	SEND_TEXT_MESSAGE(0,"Builds a texture atlas\r\n");

	assert( mimport );
	NVSHARE::MeshSystemContainer* omsc = mimport->createMeshSystemContainer(); // create an empty mesh system container.
	NVSHARE::MeshImportInterface* omii = mimport->getMeshImportInterface(omsc); // get an importer interface for this mesh system container.

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

