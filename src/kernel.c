#include "vga_driver.h"
#include "assert.h"
#include "undead.h"
#include "types.h"
#include "interrupts.h"

extern void endless_loop();
extern void setup_registers();

extern void first_challenge();
extern void second_challenge();
extern void third_challenge();
extern void byte_experiment();

void kernel_entry() {
    init_printer();
    setup_interrupts();

    setup_registers();
    // first_challenge();
    // second_challenge();
    // third_challenge();
    byte_experiment();

    printf("%s", "a\nb");
    endless_loop();
}