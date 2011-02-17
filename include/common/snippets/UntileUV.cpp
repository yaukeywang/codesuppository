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

		NxF32	dot( const NxF32* xy ) const
		{
			return dot( xy[0], xy[1] );
		}

		NxF32	intersect( const NxF32* pA, const NxF32* pB ) const
		{
			NxF32 dpA = dot( pA );

			NxF32 dir[2];

			dir[0] = pB[0] - pA[0];
			dir[1] = pB[1] - pA[1];

			NxF32 dot1 = dir[0]*mA + dir[1]*mB;
			NxF32 dot2 = dpA - mC;

			NxF32    t = -(mC + dot2 ) / dot1;
			return t;
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
			mU = 0;
			mV = 0;
		}

		~Polygon()
		{
			reset();
		}

		void	reset()
		{
			mVertices.clear();
		}

		NxF32	getU() const { return mU; }
		NxF32	getV() const { return mV; }
		void	setU( NxF32 u ) { mU = u; }
		void	setV( NxF32 v ) { mV = v; }

		NxU32	getVertexCount() const { return (NxU32)mVertices.size(); }

		const UntileUVMeshVertex&	getVertex( NxI32 idx ) const
		{
			assert( idx >= -1 && idx <= (NxI32)mVertices.size() );

			// wraparound
			if ( idx == -1 )
				return mVertices[ mVertices.size() - 1 ];
			if ( idx == (NxI32)mVertices.size() )
				return mVertices[ 0 ];
			else
				return mVertices[ idx ];
		}

		void		addVertex( const UntileUVMeshVertex& v )
		{
			mVertices.push_back( v );
		}

		void		set( const UntileUVMeshVertex* verts, NxU32 count )
		{
			mVertices.clear();
			for ( NxU32 i = 0; i < count; ++i )
				mVertices.push_back( verts[i] );
		}

		NxF32		dot( NxU32 idx, const Line2D& line, NxF32 epsilon )
		{
			return line.dot( mVertices[idx].mUV ) + epsilon;
		}

		NxI32		classify( NxU32 idx, const Line2D& line, NxF32 epsilon )
		{
			if ( dot( idx, line, epsilon ) > 0 )
				return 1;
			else
				return -1;
		}

		int		split( const Line2D& line, Polygon& front, Polygon& back, NxF32 epsilon )
		{
			assert( mVertices.size() >= 3 );
			if ( mVertices.size() < 3 )
				return -2;

			front.reset();
			back.reset();

			NxI32 c;

#if 0
			c = classify( 0, line, epsilon );
			size_t i;
			for ( i = 1; i < mVertices.size(); ++i )
				if ( c != classify( i, line, epsilon ) )
					break;

			// all on one side?
			if ( i == mVertices.size() )
			{
				if ( c == 1 )
				{
					// all in front?
					front = *this;
				}
				else
				{
					// all behind?
					assert( c == -1 );
					back = *this;
				}

				return c;
			}
#else
			c = classify( 0, line, epsilon );
			if ( c == 1 )
			{
				size_t i;
				for ( i = 1; i < mVertices.size(); ++i )
					if ( c != classify( i, line, epsilon ) )
						break;

				// all in front?
				if ( i == mVertices.size() )
				{
					front = *this;
					return c;
				}
			}

			c = classify( 0, line, -epsilon );
			if ( c == -1 )
			{
				size_t i;
				for ( i = 1; i < mVertices.size(); ++i )
					if ( c != classify( i, line, -epsilon ) )
						break;

				// all behind?
				if ( i == mVertices.size() )
				{
					back = *this;
					return c;
				}
			}
#endif

			NxI32 count = (NxI32)getVertexCount();
			NxI32 out_c = 0, in_c = 0;

			const int MAXPTS( 256 );
			UntileUVMeshVertex ptA, ptB, outpts[MAXPTS],inpts[MAXPTS];

			NxF32 sideA, sideB;

			ptA = getVertex( count - 1 );
			sideA = line.dot( ptA.mUV );
			for ( NxI32 i = -1; ++i < count; )
			{
				ptB = getVertex( i );
				sideB = line.dot( ptB.mUV );
				if ( sideB > 0 )
				{
					if ( sideA < 0 )
					{
						UntileUVMeshVertex v;
						NxF32 t = line.intersect( ptB.mUV, ptA.mUV );
						v.interpolate( ptB, ptA, t );
						outpts[ out_c++ ] = inpts[ in_c++ ] = v;
					}
					outpts[ out_c++ ] = ptB;
				}
				else if ( sideB < 0 )
				{
					if ( sideA > 0 )
					{
						UntileUVMeshVertex v;
						NxF32 t = line.intersect( ptB.mUV, ptA.mUV );
						v.interpolate( ptB, ptA, t ); // hmm...
						outpts[ out_c++ ] = inpts[ in_c++ ] = v;
					}
					inpts[ in_c++ ] = ptB;
				}
				else
				{
					outpts[ out_c++ ] = inpts[ in_c++ ] = ptB;
				}
				ptA = ptB;
				sideA = sideB;
			}

			front.set( &outpts[0], out_c );
			back.set( &inpts[0], in_c );

			return 0;
		}

		void		translate( NxF32 u, NxF32 v )
		{
			for ( size_t i = 0; i < mVertices.size(); ++i )
			{
				mVertices[i].mUV[0] += u;
				mVertices[i].mUV[1] += v;
			}
		}

	private:

		VertexVector		mVertices;
		NxF32				mU;
		NxF32				mV;
	};
	//typedef NVSHARE::Array< Polygon > PolygonVector;
	typedef std::vector< Polygon > PolygonVector;


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
			const UntileUVMeshVertex* verts,
			NxU32 vcount,
			NxF32 epsilon )
		{
			reset();
			_epsilon = epsilon;

			Extents2D extents;
			for ( NxU32 i = 0; i < vcount; ++i )
				extents.add( verts[i].mUV );

			NxF32 fMinU = extents.getMin()[0];
			NxF32 fMaxU = extents.getMax()[0];
			NxF32 fMinV = extents.getMin()[1];
			NxF32 fMaxV = extents.getMax()[1];

			NxF32 iMinU = Floor( fMinU );
			NxF32 iMaxU = Ceil( fMaxU );
			NxF32 iMinV = Floor( fMinV );
			NxF32 iMaxV = Ceil( fMaxV );

			Polygon tri;
			for ( NxU32 i = 0; i < vcount; ++i )
				tri.addVertex( verts[i] );
			tri.setU( iMaxU );
			tri.setV( iMaxV );

			Polygon front;
			Polygon back;

			PolygonVector polys;
			for ( NxF32 u = iMinU; u <= iMaxU; u += 1.0f )
			{
				if ( tri.getVertexCount() == 0 )
					break;
				assert( tri.getVertexCount() >= 3 );

				Line2D line;
				line.set(
					u, 0.0f,
					u, 1.0f );

				tri.split( line, front, back, epsilon );
				front.setU( u - 1.0f );
				front.setV( tri.getV() );
				back.setU( u );
				back.setV( tri.getV() );
				if ( front.getVertexCount() > 0 )
				{
					assert( front.getVertexCount() >= 3 );
					polys.push_back( front );
				}
				tri = back;
			}
			if ( tri.getVertexCount() > 0 )
			{
				assert( tri.getVertexCount() >= 3 );
				//tri.setU( iMaxU );
				polys.push_back( tri );
			}

			for ( size_t i = 0; i < polys.size(); ++i )
			{
				tri = polys[i];

				for ( NxF32 v = iMinV; v <= iMaxV; v += 1.0f )
				{
					if ( tri.getVertexCount() == 0 )
						break;
					assert( tri.getVertexCount() >= 3 );

					Line2D line;
					line.set(
						1.0f, v,
						0.0f, v );

					tri.split( line, front, back, epsilon );
					front.setU( tri.getU() );
					front.setV( v - 1.0f );
					back.setU( tri.getU() );
					back.setV( v );
					if ( front.getVertexCount() > 0 )
					{
						assert( front.getVertexCount() >= 3 );
						front.translate( -front.getU(), -front.getV() );
						addPoly( front );
					}
					tri = back;
				}
				if ( tri.getVertexCount() > 0 )
				{
					assert( tri.getVertexCount() >= 3 );
					tri.translate( -tri.getU(), -tri.getV() );
					addPoly( tri );
				}
			}

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

			_vertices.push_back( checkRange( poly, 0 ) );
			_vertices.push_back( checkRange( poly, 1 ) );

			for ( NxU32 i = 2; i < vertCount; ++i )
			{
				_vertices.push_back( checkRange( poly, i ) );
				_indices.push_back( start );
				_indices.push_back( start + i - 1 );
				_indices.push_back( start + i );
			}
		}

		const UntileUVMeshVertex&	checkRange( const Polygon& poly, NxU32 idx )
		{
			const UntileUVMeshVertex& vert = poly.getVertex( idx );
			assert( vert.mUV[0] >= -_epsilon && vert.mUV[0] <= 1.0f+_epsilon );
			assert( vert.mUV[1] >= -_epsilon && vert.mUV[1] <= 1.0f+_epsilon );
			return vert;
		}

		NxF32			_epsilon;
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

