#include "types.h"
#include "kernel_panic.h"
#include "memory_utils.h"

#define ARENA_START 0x100000
#define ARENA_END 0x400000

uptr current_address = ARENA_START;

void* malloc_undead(size_t size, size_t alignment) {
  if (size == 0) {
    size = 1;
  }
  uptr aligned_address = (current_address + (alignment - 1)) & ~(alignment - 1);

  if (aligned_address + size > ARENA_END) {
    kernel_panic("Out of undead memory: requested %u bytes with alignment %u\n", size, alignment);
  }

  current_address = aligned_address + size;
  return (void*)aligned_address;
}


void* calloc_undead(size_t size, size_t alignment) {
  void* ptr = malloc_undead(size, alignment);
  memzero(ptr, size);
  return ptr;
}

