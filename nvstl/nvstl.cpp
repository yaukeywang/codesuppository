#include "nvstl_stl_vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


namespace nvstl
{
void * nvstl_new(size_t size)
{
	return ::malloc(size);
}

void   nvstl_delete(void *p)
{
	::free(p);
}
};

typedef nvstl::vector< int > IntVector;

void main(int argc,const char **argv)
{
	IntVector v;

	v.push_back(1);
	v.push_back(2);
}
