#include "vga_driver.h"
#include "assert.h"
#include "undead.h"
#include "types.h"

extern void endless_loop();

void kernel_entry() {
    assert(sizeof(u32) == 4, "u32 is not 4 bytes");
    assert(sizeof(u16) == 2, "u16 is not 2 bytes");
    assert(sizeof(uptr) == sizeof(void*), "uptr is not the same size as void*");
    assert(sizeof(size_t) == sizeof(void*), "size_t is not the same size as void*");
    assert(sizeof(u64) == 8, "u64 is not 8 bytes");
    assert(sizeof(i32) == 4, "i32 is not 4 bytes");
    assert(sizeof(i16) == 2, "i16 is not 2 bytes");
    assert(sizeof(i64) == 8, "i64 is not 8 bytes");
    assert(sizeof(u8) == 1, "u8 is not 1 byte");
    assert(sizeof(i8) == 1, "i8 is not 1 byte");

    init_printer();
    u32 size = 1;
    u32 alignment = 1;
    uptr prev_address = 0;
    while (1) {
        void* ptr = malloc_undead(size, alignment);
        uptr addr = (uptr)ptr;
        printf("Allocated %u bytes at address %p with alignment %u\n", size, addr, alignment);
        printf("Decimal addr: %u\n", addr);

        assert(addr % alignment == 0, "Address is not aligned properly");
        assert(addr > prev_address, "Address is not increasing");

        prev_address = addr;
        size *= 2;
        alignment += 1;

        volatile u32 sum = 0;
        for (volatile size_t i = 0; i < 100000; ++i) {
            // Busy wait to slow down the output
            sum += i;
        }
    }
    endless_loop();
}