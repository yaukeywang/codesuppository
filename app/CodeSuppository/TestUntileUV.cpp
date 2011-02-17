#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TestUntileUV.h"
#include "MeshImport.h"
using namespace NVSHARE;
#include "UntileUV.h"
#include "shared/MeshSystem/MeshSystemHelper.h"
#include "RenderDebug.h"

static void
renderUntileUV( UntileUV* uuv )
{
	const UntileUVMeshVertex* verts		= uuv->getVerts();
	const NxU32* indices				= uuv->getIndices();

	NxU32 tcount = uuv->getTriCount();
	for ( NxU32 i = 0; i < tcount; ++i )
	{
		NxU32 i1 = indices[i*3+0];
		NxU32 i2 = indices[i*3+1];
		NxU32 i3 = indices[i*3+2];
		gRenderDebug->DebugTri(
			verts[i1].mAttributes[0],
			verts[i2].mAttributes[0],
			verts[i3].mAttributes[0] );
	}
}

void
testUntileUV( MeshSystemHelper * msh )
{
	SEND_TEXT_MESSAGE(0,"Demonstrates how to remove tiling UVs from a mesh\r\n");

	if ( msh )
	{
		gRenderDebug->pushRenderState();
		gRenderDebug->reset( -1 );
		gRenderDebug->setCurrentColor( 0xFFFF00, 0xFFFFFF );
		gRenderDebug->setCurrentDisplayTime( 6000 );

		MeshSystem* ms = msh->getMeshSystem();
		if ( ms )
		{
			for ( NxU32 i = 0; i < ms->mMeshCount; ++i )
			{
				Mesh* m = ms->mMeshes[i];
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
						for ( NxU32 ii = 0; ii < 3; ++ii )
						{
							const MeshVertex* vert = verts[ii];
							v[ii].SetUV( vert->mTexel1[0], vert->mTexel1[1] );
							v[ii].SetAttribXYZ( 0, vert->mPos );
							v[ii].SetAttribXY( 1, vert->mTexel2 );
						}

						UntileUV* uuv = createUntileUV();
						uuv->untile( v+0, v+1, v+2 );
						renderUntileUV( uuv );
						releaseUntileUV( uuv );
#endif
					}
				}
			}
		}

		gRenderDebug->popRenderState();
	}
}

