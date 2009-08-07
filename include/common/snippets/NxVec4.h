#ifndef NX_FOUNDATION_NXVEC4
#define NX_FOUNDATION_NXVEC4

#include "Nxf.h"
#include "NxMath.h"

class  NxMat44;

/**
\brief 4 Element vector class.

This is a vector class with public data members.
This is not nice but it has become such a standard that hiding the xyzw data members
makes it difficult to reuse external code that assumes that these are public in the library.
The vector class can be made to use float or double precision by appropriately defining NxReal.
This has been chosen as a cleaner alternative to a template class.
*/
class NxVec4
{
public:
  NX_INLINE NxReal& operator[](int index);

  NxReal x,y,z,w;
};

NxReal& NxVec4::operator[](int index)
{
	NX_ASSERT(index>=0 && index<=3);
	return (&x)[index];
}


#endif
