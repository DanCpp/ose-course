#include "vga_driver.h"
#include "assert.h"
#include "undead.h"

extern void endless_loop();

void kernel_entry() {
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