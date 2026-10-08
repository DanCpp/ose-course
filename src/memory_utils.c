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

void* memmove (void* dst, const void* src, size_t n)
{
  u8* d = dst;
  const u8* s = src;

  if (d < s)
    {
      while (n--)
        *d++ = *s++;
    }
  else
    {
      u8* lasts = s + n;
      u8* lastd = d + n;
      while (n--)
        *--lastd = *--lasts;
    }
    
  return dst;
}
