#ifndef HE_FOUNDATION_NXSIMPLETRIANGLEMESH
#define HE_FOUNDATION_NXSIMPLETRIANGLEMESH

#include "HeVec3.h"

/**
\brief Enum with flag values to be used in HeSimpleTriangleMesh::flags.
*/
enum HeMeshFlags
	{
	/**
	\brief Specifies if the SDK should flip normals.

	The He libraries assume that the face normal of a triangle with vertices [a,b,c] can be computed as:
	edge1 = b-a
	edge2 = c-a
	face_normal = edge1 x edge2.

	Note: This is the same as a counterclockwise winding in a right handed coordinate system or
	alternatively a clockwise winding order in a left handed coordinate system.

	If this does not match the winding order for your triangles, raise the below flag.
	*/
	HE_MF_FLIPNORMALS		=	(1<<0),
	HE_MF_16_BIT_INDICES	=	(1<<1),	//<! Denotes the use of 16-bit vertex indices
	HE_MF_HARDWARE_MESH		=	(1<<2),	//<! The mesh will be used in hardware scenes
	};

typedef HeVec3 HePoint;

/**
\brief A structure describing a triangle mesh.
*/
class HeSimpleTriangleMesh
	{
	public:
	HeU32 numVertices;			//!< Number of vertices.
	HeU32 numTriangles;			//!< Number of triangles.
	HeU32 pointStrideBytes;		//!< Offset between vertex points in bytes.
	HeU32 triangleStrideBytes;	//!< Offset between triangles in bytes.

	/**
	\brief Pointer to first vertex point.
	
	Caller may add pointStrideBytes bytes to the pointer to access the next point.
	*/
	const void* points;

	/**
	\brief Pointer to first triangle.
	
	Caller may add triangleStrideBytes bytes to the pointer to access the next triangle.
	
	These are triplets of 0 based indices:
	vert0 vert1 vert2
	vert0 vert1 vert2
	vert0 vert1 vert2
	...

	where vertex is either a 32 or 16 bit unsigned integer. There are numTriangles*3 indices.

	This is declared as a void pointer because it is actually either an HeU16 or a HeU32 pointer.
	*/
	const void* triangles;

	/**
	\brief Flags bits, combined from values of the enum ::HeMeshFlags
	*/
	HeU32 flags;

	/**
	\brief constructor sets to default.
	*/
	HE_INLINE HeSimpleTriangleMesh();	
	/**
	\brief (re)sets the structure to the default.	
	*/
	HE_INLINE void setToDefault();
	/**
	\brief returns true if the current settings are valid
	*/
	HE_INLINE bool isValid() const;
	};


HE_INLINE HeSimpleTriangleMesh::HeSimpleTriangleMesh()
	{
	setToDefault();
	}

HE_INLINE void HeSimpleTriangleMesh::setToDefault()
	{
	numVertices			= 0;
	numTriangles		= 0;
	pointStrideBytes	= 0;
	triangleStrideBytes	= 0;
	points				= NULL;
	triangles			= NULL;
	flags				= 0;
	}

HE_INLINE bool HeSimpleTriangleMesh::isValid() const
	{
	// Check geometry
	if(numVertices > 0xffff && flags & HE_MF_16_BIT_INDICES)
		return false;
	if(!points)
		return false;
	if(pointStrideBytes < sizeof(HePoint))	//should be at least one point's worth of data
		return false;

	// Check topology
	// The triangles pointer is not mandatory
	if(triangles)
		{
		// Indexed mesh
		if(flags & HE_MF_16_BIT_INDICES)
			{
			if((triangleStrideBytes < sizeof(HeU16)*3))
				return false;
			}
		else
			{
			if((triangleStrideBytes < sizeof(HeU32)*3))
				return false;
			}
		}
	return true;
	}

 /** @} */
#endif
