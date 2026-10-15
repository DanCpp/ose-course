#include "types.h"

void* memset(void* dst, int c, size_t n) {
  u8* d = (u8*)dst;
  for (u32 i = 0; i < n; i++) {
    d[i] = (u8)c;
  }
  return dst;
}

void* memzero(void* dst, size_t n) {
  return memset(dst, 0, n);
}