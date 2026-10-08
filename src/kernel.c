#include "vga_driver.h"
#include "assert.h"
#include "undead.h"

extern void endless_loop();

void kernel_entry() {
    init_printer();
    printf("Kernel size is: %u bytes\n", KERNEL_SIZE);
    u32 size = 1;
    u32 alignment = 1;
    while (1) {
        void* ptr = malloc_undead(size, alignment);
        printf("Allocated %u bytes at address %p with alignment %u\n", size, ptr, alignment);
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