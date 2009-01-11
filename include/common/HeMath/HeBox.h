#ifndef HE_FOUNDATION_NXBOX
#define HE_FOUNDATION_NXBOX

#include "../snippets/He.h"
#include "HeVec3.h"
#include "HeMat44.h"

class HeCapsule;
class HePlane;
class HeBox;
class HeBounds3;


/**
\brief Represents an oriented bounding box. 

As a center point, extents(radii) and a rotation. i.e. the center of the box is at the center point, 
the box is rotated around this point with the rotation and it is 2*extents in width, height and depth.
*/
class HeBox
	{
	public:
	/**
	\brief Constructor
	*/
	HE_INLINE HeBox()
		{
		}

	/**
	\brief Constructor

	\param _center Center of the OBB
	\param _extents Extents/radii of the obb.
	\param _rot rotation to apply to the obb.
	*/
	HE_INLINE HeBox(const HeVec3& _center, const HeVec3& _extents, const HeMat44& _rot) : center(_center), extents(_extents), rot(_rot)
		{
		}

	/**
	\brief Destructor
	*/
	HE_INLINE ~HeBox()
		{
		}

	/**
	 \brief Setups an empty box.
	*/
	HE_INLINE void setEmpty()
		{
		center.zero();
		extents.set(HE_MIN_REAL, HE_MIN_REAL, HE_MIN_REAL);
		rot.id();
		}
#ifdef FOUNDATION_EXPORTS

	/**
	 \brief Tests if a point is contained within the box

	 See #HeBoxContainsPoint().

	 \param		p	[in] the world point to test
	 \return	true if inside the box
	*/
	HE_INLINE bool containsPoint(const HeVec3& p) const
		{
		return HeBoxContainsPoint(*this, p);
		}

	/**
	 \brief Builds a box from AABB and a world transform.

	 See #HeCreateBox().

	 \param		aabb	[in] the aabb
	 \param		mat		[in] the world transform
	*/
	HE_INLINE void create(const HeBounds3& aabb, const HeMat44& mat)
		{
		HeCreateBox(*this, aabb, mat);
		}
#endif
	/**
	 \brief Recomputes the box after an arbitrary transform by a 4x4 matrix.

	 \param		mtx		[in] the transform matrix
	 \param		obb		[out] the transformed OBB
	*/
	HE_INLINE void rotate(const HeMat44& mtx, HeBox& obb) const
		{		
		obb.extents = extents;// The extents remain constant
    HeVec3 t( &mtx.t.x );
		obb.center = mtx * center + t;
		obb.rot.multiply(mtx, rot);
		}

	/**
	 \brief Checks the box is valid.

	 \return	true if the box is valid
	*/
	HE_INLINE bool isValid() const
		{
		// Consistency condition for (Center, Extents) boxes: Extents >= 0.0f
		if(extents.x < 0.0f)	return false;
		if(extents.y < 0.0f)	return false;
		if(extents.z < 0.0f)	return false;
		return true;
		}
#ifdef FOUNDATION_EXPORTS

	/**
	 \brief Computes the obb planes.

	 See #HeComputeBoxPlanes().

	 \param		planes	[out] 6 box planes
	 \return	true if success
	*/
	HE_INLINE bool computePlanes(HePlane* planes) const
		{
		return HeComputeBoxPlanes(*this, planes);
		}

	/**
	 \brief Computes the obb points.

	 See #HeComputeBoxPoints().

	 \param		pts	[out] 8 box points
	 \return	true if success
	*/
	HE_INLINE bool computePoints(HeVec3* pts) const
		{
		return HeComputeBoxPoints(*this, pts);
		}

	/**
	 \brief Computes vertex normals.

	 See #HeComputeBoxVertexNormals().

	 \param		pts	[out] 8 box points
	 \return	true if success
	*/
	HE_INLINE bool computeVertexNormals(HeVec3* pts) const
		{
		return HeComputeBoxVertexNormals(*this, pts);
		}

	/**
	 \brief Returns edges.

	 See #HeGetBoxEdges().

	 \return	24 indices (12 edges) indexing the list returned by ComputePoints()
	*/
	HE_INLINE const HeU32* getEdges() const
		{
		return HeGetBoxEdges();
		}

		/**
		\brief Return edge axes indices.

		 See #HegetBoxEdgeAxes().

		 \return Array of edge axes indices.
		*/

	HE_INLINE const HeI32* getEdgesAxes() const
		{
		return HeGetBoxEdgesAxes();
		}

	/**
	 \brief Returns triangles.

	 See #HeGetBoxTriangles().


	 \return 36 indices (12 triangles) indexing the list returned by ComputePoints()
	*/
	HE_INLINE const HeU32* getTriangles() const
		{
		return HeGetBoxTriangles();
		}

	/**
	 \brief Returns local edge normals.

	 See #HeGetBoxLocalEdgeNormals().

	 \return edge normals in local space
	*/
	HE_INLINE const HeVec3* getLocalEdgeNormals() const
		{
		return HeGetBoxLocalEdgeNormals();
		}

	/**
	 \brief Returns world edge normal

	 See #HeComputeBoxWorldEdgeNormal().

	 \param		edge_index		[in] 0 <= edge index < 12
	 \param		world_normal	[out] edge normal in world space
	*/
	HE_INLINE void computeWorldEdgeNormal(HeU32 edge_index, HeVec3& world_normal) const
		{
		HeComputeBoxWorldEdgeNormal(*this, edge_index, world_normal);
		}

	/**
	 \brief Computes a capsule surrounding the box.

	 See #HeComputeCapsuleAroundBox().

	 \param		capsule	[out] the capsule
	*/
	HE_INLINE void computeCapsule(HeCapsule& capsule) const
		{
		HeComputeCapsuleAroundBox(*this, capsule);
		}

	/**
	 \brief Checks the box is inside another box

	 See #HeIsBoxAInsideBoxB().

	 \param		box		[in] the other box
	 \return	TRUE if we're inside the other box
	*/
	HE_INLINE bool isInside(const HeBox& box) const
		{
		return HeIsBoxAInsideBoxB(*this, box);
		}
#endif
	// Accessors

		/**
		\brief Return center of box.

		\return Center of box.
		*/
	HE_INLINE const HeVec3& GetCenter() const
		{
		return center;
		}

		/**
		\brief Return extents(radii) of box.

		\return Extents of box.
		*/

	HE_INLINE const HeVec3& GetExtents() const
		{
		return extents;
		}

		/**
		\brief return box rotation.

		\return Box Rotation.
		*/

	HE_INLINE const HeMat44& GetRot() const
		{
		return rot;
		}

	HeVec3	center;
	HeVec3	extents;
	HeMat44	rot;
	};

#endif
