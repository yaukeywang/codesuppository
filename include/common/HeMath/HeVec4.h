#ifndef HE_FOUNDATION_NXVEC4
#define HE_FOUNDATION_NXVEC4

#include "Hef.h"
#include "HeMath.h"

class  HeMat44;

/**
\brief 4 Element vector class.

This is a vector class with public data members.
This is not nice but it has become such a standard that hiding the xyzw data members
makes it difficult to reuse external code that assumes that these are public in the library.
The vector class can be made to use float or double precision by appropriately defining HeReal.
This has been chosen as a cleaner alternative to a template class.
*/
class HeVec4
{
public:
  HE_INLINE HeReal& operator[](int index);

  HeReal x,y,z,w;
};

HeReal& HeVec4::operator[](int index)
{
	HE_ASSERT(index>=0 && index<=3);
	return (&x)[index];
}


#endif
