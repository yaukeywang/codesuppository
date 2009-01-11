#ifndef HE_FOUNDATION_NXVOLUMEINTEGRATION
#define HE_FOUNDATION_NXVOLUMEINTEGRATION


#include "../snippets/He.h"
#include "HeVec3.h"
#include "HeMat44.h"

class HeSimpleTriangleMesh;

/**
\brief Data structure used to store mass properties.
*/
struct HeIntegrals
	{
	HeVec3 COM;					//!< Center of mass
	HeF64 mass;						//!< Total mass
	HeF64 inertiaTensor[3][3];		//!< Inertia tensor (mass matrix) relative to the origin
	HeF64 COMInertiaTensor[3][3];	//!< Inertia tensor (mass matrix) relative to the COM

	/**
	\brief Retrieve the inertia tensor relative to the center of mass.

	\param inertia Inertia tensor.
	*/
	void getInertia(HeMat44& inertia)
	{
		for(HeU32 j=0;j<3;j++)
		{
			for(HeU32 i=0;i<3;i++)
			{
				inertia(i,j) = (HeF32)COMInertiaTensor[i][j];
			}
		}
	}

	/**
	\brief Retrieve the inertia tensor relative to the origin.

	\param inertia Inertia tensor.
	*/
	void getOriginInertia(HeMat44& inertia)
	{
		for(HeU32 j=0;j<3;j++)
		{
			for(HeU32 i=0;i<3;i++)
			{
				inertia(i,j) = (HeF32)inertiaTensor[i][j];
			}
		}
	}
	};

#endif
