#ifndef UNDEAD_H
#define UNDEAD_H

#include "types.h"

void* malloc_undead(size_t size, size_t alignment);
void* calloc_undead(size_t size, size_t alignment);

#endif // UNDEAD_H
