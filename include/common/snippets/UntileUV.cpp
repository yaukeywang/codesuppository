#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#pragma warning(disable:4702)
#pragma warning(disable:4127) //conditional expression is constant (because _HAS_EXCEPTIONS=0)
#include <vector>
#include <hash_map>
#include "UserMemAlloc.h"
#include "NvHashMap.h"
#include "UntileUV.h"
#include "FloatMath.h"

#pragma warning(disable:4189)

using namespace NVSHARE;

typedef NVSHARE::Array< NxU32 > NxU32Vector;
typedef NVSHARE::Array< UntileUVMeshVertex > VertexVector;

namespace UNTILE_UV
{
	class MyUntileUV : public UntileUV
	{
	public:
		MyUntileUV()
		{
		}

		~MyUntileUV()
		{
			reset();
		}

		void	reset()
		{
			_indices.clear();
			_vertices.clear();
		}

		NxU32	untile(
			const UntileUVMeshVertex* vA,
			const UntileUVMeshVertex* vB,
			const UntileUVMeshVertex* vC )
		{
			reset();

#if 0
			_vertices.pushBack( *vA );
			_vertices.pushBack( *vB );
			_vertices.pushBack( *vC );

			_indices.pushBack( 0 );
			_indices.pushBack( 1 );
			_indices.pushBack( 2 );
#else

			_vertices.pushBack( *vA );
			_vertices.pushBack( *vB );

			UntileUVMeshVertex v;
			v.Interpolate( *vB, *vC, 0.5f );
			_vertices.pushBack( v );

			_vertices.pushBack( *vC );

			_indices.pushBack( 0 );
			_indices.pushBack( 1 );
			_indices.pushBack( 2 );

			_indices.pushBack( 0 );
			_indices.pushBack( 2 );
			_indices.pushBack( 3 );
#endif

			return 2;
		}

		virtual const UntileUVMeshVertex*	getVerts()
		{
			return &_vertices[0];
		}

		virtual const NxU32*	getIndices()
		{
			return &_indices[0];
		}

		virtual NxU32	getTriCount()
		{
			assert( _indices.size() % 3 == 0 );
			return _indices.size() / 3;
		}

	private:
		NxU32Vector		_indices;
		VertexVector	_vertices;
	};

}; // end of namespace UNTILE_UV

using namespace UNTILE_UV;

UntileUV*
createUntileUV()
{
	MyUntileUV *m = new MyUntileUV;
	return static_cast< UntileUV *>(m);
}

void
releaseUntileUV( UntileUV *uuv )
{
	MyUntileUV *m = static_cast< MyUntileUV *>( uuv );
	delete m;
}

