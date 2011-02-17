#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#pragma warning(disable:4702)
#pragma warning(disable:4127) //conditional expression is constant (because _HAS_EXCEPTIONS=0)
#include <vector>
#include <list>
#include <hash_map>
#include "UserMemAlloc.h"
#include "NvHashMap.h"
#include "UntileUV.h"
#include "FloatMath.h"
#include <cmath>

#if 1
#pragma warning(disable:4189)
#pragma warning(disable:4100)
#pragma warning(disable:4101)
#endif

using namespace NVSHARE;

//typedef NVSHARE::Array< NxU32 > NxU32Vector;
//typedef NVSHARE::Array< UntileUVMeshVertex > VertexVector;
typedef std::vector< NxU32 > NxU32Vector;
typedef std::vector< UntileUVMeshVertex > VertexVector;
typedef std::list< UntileUVMeshVertex > VertexList;

namespace UNTILE_UV
{
	template <class T>
	T Max(const T &a,const T &b) 
	{
		return (a>b)?a:b;
	}

	template <class T>
	T Min(const T &a,const T &b) 
	{
		return (a<b)?a:b;
	}

	NxF32 Abs( NxF32 f )
	{
		return (NxF32)fabs( f );
	}

	NxF32 Floor( NxF32 f )
	{
		return (NxF32)floor( f );
	}

	NxF32 Ceil( NxF32 f )
	{
		return (NxF32)ceil( f );
	}


	class Extents2D
	{
	public:
		Extents2D()
		{
			mNeedInit = true;
		}

		void	reset()
		{
			mNeedInit = true;
		}

		void	add( const NxF32* p )
		{
			if ( mNeedInit )
			{
				mNeedInit = false;
				mMin[0] = mMax[0] = p[0];
				mMin[1] = mMax[1] = p[1];
			}
			else
			{
				mMin[0] = Min( mMin[0], p[0] );
				mMin[1] = Min( mMin[1], p[1] );
				mMax[0] = Max( mMax[0], p[0] );
				mMax[1] = Max( mMax[1], p[1] );
			}
		}

		void	include( const Extents2D& extents )
		{
			add( extents.getMin() );
			add( extents.getMax() );
		}

		const NxF32*	getMin() const		{ assert( !mNeedInit ); return mMin; };
		const NxF32*	getMax() const		{ assert( !mNeedInit ); return mMax; };

	private:
		NxF32	mMin[2];
		NxF32	mMax[2];
		bool	mNeedInit;
	};


	// Ax + By + C = 0
	class Line2D
	{
	public:
		Line2D()
		{
			mA = mB = mC = 0.0f;
		}

		void	set( 
			NxF32 x1, NxF32 y1,
			NxF32 x2, NxF32 y2 )
		{
			NxF32 x12 = (x2 - x1);
			NxF32 y12 = (y2 - y1);

			mA = -y12;
			mB = x12;

			// C = -(Ax + By)
			mC = -(mA*x1 + mB*y1);
		}

		NxF32	dot( NxF32 x, NxF32 y ) const
		{
			return mA*x + mB*y + mC;
		}

		NxF32	mA;
		NxF32	mB;
		NxF32	mC;
	};


	class Edge
	{
	public:
		Edge(
			const UntileUVMeshVertex* vA,
			const UntileUVMeshVertex* vB )
		{
			mA = vA;
			mB = vB;
		}

		bool	intersect( NxF32& u, const Line2D& line )
		{
			NxF32 a = line.dot( mA->mUV[0], mA->mUV[1] );
			NxF32 b = line.dot( mB->mUV[0], mB->mUV[1] );

			if ( a >= 0.0f && b >= 0.0f )
				return false;

			if ( a <= 0.0f && b <= 0.0f )
				return false;

			a = Abs( a );
			b = Abs( b );
			u = a / (a + b);

			return true;
		}

		void	interpolate( UntileUVMeshVertex& outVert, NxF32 u )
		{
			assert( u >= 0.0f && u <= 1.0f );
			outVert.interpolate( *mA, *mB, u );
		}

		const UntileUVMeshVertex*	getA() const { return mA; }
		const UntileUVMeshVertex*	getB() const { return mB; }

	private:
		const UntileUVMeshVertex*	mA;
		const UntileUVMeshVertex*	mB;
		Line2D						mLine;
	};
	typedef std::list< Edge* > EdgeList;


	class Polygon
	{
	public:
		Polygon()
		{
		}

		Polygon( const Polygon& poly )
		{
			mVertices = poly.mVertices;
		}

		~Polygon()
		{
			reset();
		}

		void	reset()
		{
			mVertices.clear();
		}

		NxU32	getVertexCount() const { return (NxU32)mVertices.size(); }

		const UntileUVMeshVertex&	getVertex( NxU32 idx ) const
		{
			assert( idx <= mVertices.size() );

			// wraparound
			if ( idx == mVertices.size() )
				return mVertices[ 0 ];
			else
				return mVertices[ idx ];
		}

		void	addVertex( const UntileUVMeshVertex& v )
		{
			mVertices.push_back( v );
		}

		bool		split( const Line2D& line, Polygon& front, Polygon& back )
		{
			assert( mVertices.size() >= 3 );
			if ( mVertices.size() < 3 )
				return false;

			front.reset();
			back.reset();

			Polygon* polys[2];
			if ( line.dot( mVertices[0].mUV[0], mVertices[0].mUV[1] ) >= 0.0f )
			{
				polys[0] = &front;
				polys[1] = &back;
			}
			else
			{
				polys[0] = &back;
				polys[1] = &front;
			}
			NxU32 total = getVertexCount();

			NxU32 Si( (NxU32)-1 );

			NxU32 Ai;
			NxF32 At;
			if ( !findSplit( Ai, At, 0, getVertexCount(), line ) )
				return false;

			NxU32 Bi;
			NxF32 Bt;

			if ( !findSplit( Bi, Bt, Ai + 1, getVertexCount(), line ) )
			{
				for ( Si = 0; ; ++Si )
				{
					assert( Si <= getVertexCount() );
					if ( Si > getVertexCount() )
						return false;

					const UntileUVMeshVertex& p( getVertex( Si ) );
					if ( line.dot( p.mUV[0], p.mUV[1] ) == 0.0f )
						break;
				}

				if ( Si <= Ai )
				{
					Bi = Ai;
					Bt = At;
					Ai = Si;
					At = 0.0f;
				}
				else
				{
					Bi = Si;
					Bt = 0.0f;
				}
			}
			UntileUVMeshVertex v;
			NxU32 i = 0;

			for ( i; i <= Ai; ++i )
				polys[0]->addVertex( getVertex( i ) );

			if ( At == 0.0f )
				v = getVertex( Ai );
			else
				v.interpolate( getVertex( Ai ), getVertex( Ai + 1 ), At );
			polys[0]->addVertex( v );
			polys[1]->addVertex( v );

			for ( i; i <= Bi; ++i )
				polys[1]->addVertex( getVertex( i ) );

			if ( Bt == 0.0f )
				v = getVertex( Bi );
			else
				v.interpolate( getVertex( Bi ), getVertex( Bi + 1 ), Bt );
			polys[1]->addVertex( v );
			polys[0]->addVertex( v );

			for ( i; i < total; ++i )
				polys[0]->addVertex( getVertex( i ) );

			assert( polys[0]->getVertexCount() >= 3 );
			assert( polys[1]->getVertexCount() >= 3 );

			return true;
		}

	private:
		bool		findSplit( NxU32& outIdx, NxF32& outPercent,
			NxU32 startIdx, NxU32 endIdx,
			const Line2D& line )
		{
			for ( NxU32 i = startIdx; i < endIdx; ++i )
			{
				const UntileUVMeshVertex* vA = &getVertex( i );
				const UntileUVMeshVertex* vB = &getVertex( i + 1 );

				Edge edge( vA, vB );
				if ( edge.intersect( outPercent, line ) )
				{
					outIdx = i;
					return true;
				}
			}
			return false;
		}

		VertexVector		mVertices;
	};
	//typedef NVSHARE::Array< Polygon > PolygonVector;
	typedef std::vector< Polygon > PolygonVector;


	class SplitPolygon
	{
	public:
		SplitPolygon()
		{
			mFront = 0;
			mBack = 0;
			mPoly = 0;
			mLeaf = 0;
		}

		~SplitPolygon()
		{
			reset();
		}

		void	reset( const Polygon* init = 0 )
		{
			delete mLeaf; mLeaf = 0;
			delete mPoly; mPoly = 0;
			delete mFront; mFront = 0;
			delete mBack; mBack = 0;

			if ( init )
			{
				mLeaf = new Polygon( *init );
			}
		}

		bool	isLeaf() const	{ return mLeaf != 0; }

		Polygon*		getPoly() const	{ return mLeaf; }
		SplitPolygon*	getFront() const { return mFront; }
		SplitPolygon*	getBack() const { return mBack; }
		Line2D*			getLine() const { return mPoly; }

		void	split( const Line2D& line )
		{
			if ( isLeaf() )
			{
			}
		}

	private:
		SplitPolygon*	mFront;
		SplitPolygon*	mBack;
		Line2D*			mPoly;
		Polygon*		mLeaf;
	};


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

			Extents2D extents;
			extents.add( vA->mUV );
			extents.add( vB->mUV );
			extents.add( vC->mUV );

			NxF32 fMinU = extents.getMin()[0];
			NxF32 fMaxU = extents.getMax()[0];
			NxF32 fMinV = extents.getMin()[1];
			NxF32 fMaxV = extents.getMax()[1];

			NxF32 iMinU = Floor( fMinU );
			NxF32 iMaxU = Ceil( fMaxU );
			NxF32 iMinV = Floor( fMinV );
			NxF32 iMaxV = Ceil( fMaxV );

			Polygon tri;
			tri.addVertex( *vA );
			tri.addVertex( *vB );
			tri.addVertex( *vC );

#if 0
			addPoly( tri );
#elif 0
			Polygon front;
			Polygon back;

			Line2D line;
			line.set(
				fMinU, fMinV,
				fMinV, fMaxV );
			if ( tri.split( line, front, back ) )
			{
				addPoly( front );
				addPoly( back );
			}
			else
			{
				addPoly( tri );
			}
#elif 0
			Polygon front;
			Polygon back;

			PolygonVector polys;
			for ( NxF32 u = iMinU; u <= iMaxU; u += 1.0f )
			{
				Line2D line;
				line.set(
					u, iMinV,
					u, iMaxV );

				if ( tri.split( line, front, back ) )
				{
					addPoly( front );
					tri = back;
				}
			}
			addPoly( tri );
#else
			Polygon front;
			Polygon back;

			PolygonVector polys;
			for ( NxF32 u = iMinU; u <= iMaxU; u += 1.0f )
			{
				Line2D line;
				line.set(
					u, iMinV,
					u, iMaxV );

				if ( tri.split( line, front, back ) )
				{
					polys.push_back( front );
					tri = back;
				}
			}
			polys.push_back( tri );

			for ( size_t i = 0; i < polys.size(); ++i )
			{
				tri = polys[i];
				for ( NxF32 v = iMinV; v < iMaxV; v += 1.0f )
				{
					Line2D line;
					line.set(
						iMaxU, v,
						iMinU, v );

					if ( tri.split( line, front, back ) )
					{
						addPoly( front );
						tri = back;
					}
				}
				addPoly( tri );
			}
#endif

			return _indices.size() / 3;
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
		void	addPoly( const Polygon& poly )
		{
			NxU32 vertCount = poly.getVertexCount();
			assert( vertCount >= 3 );
			if ( vertCount < 3 )
				return;

			NxU32 start = _vertices.size();

			_vertices.push_back( poly.getVertex( 0 ) );
			_vertices.push_back( poly.getVertex( 1 ) );

			for ( NxU32 i = 2; i < vertCount; ++i )
			{
				_vertices.push_back( poly.getVertex( i ) );
				_indices.push_back( start );
				_indices.push_back( start + i - 1 );
				_indices.push_back( start + i );
			}
		}

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

