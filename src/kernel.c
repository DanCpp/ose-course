#include "vga_driver.h"

extern void endless_loop();

void kernel_entry() {
    init_printer();
    printf("Hello, \rWorld!\n");

    printf("This is a simple kernel written in %s.\n", "C99");
    endless_loop();
}