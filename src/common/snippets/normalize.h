#ifndef NORMALIZE_H

#define NORMALIZE_H


#include "common/snippets/UserMemAlloc.h"
#include <vector>

// puts the normalized fqn into the passed in vector, including the NULL character
void normalizeFQN(const char * fqn, USER_STL::vector< char > & normalizedFQN);

#endif
