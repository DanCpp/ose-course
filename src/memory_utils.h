#ifndef MEMORY_UTILS_H
#define MEMORY_UTILS_H

#include "types.h"
void* memset(void* dst, int c, size_t n);
void* memzero(void* dst, size_t n);
void* memmove(void* dst, const void* src, size_t n);

#endif // MEMORY_UTILS_H
