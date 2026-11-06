#include "vga_driver.h"
#include "assert.h"
#include "undead.h"
#include "types.h"
#include "interrupts.h"
#include "pic.h"

extern void endless_loop();

extern void go_cli();
extern void go_sti();
extern void write_to_port(u16 port, u8 value);
extern u8 read_from_port(u16 port);

void c_delay_without_printf(int operation_count) {
    for (int i = 0; i < operation_count; i++) {
        write_to_port(0x80, 0);
    }
}

#define TIMER_IRQ 0
#define KEYBOARD_IRQ 1

static u32 global = 0;

void timer_handler_exp_3(context* ctx) {
    printf("%d ", global++);
}

void timer_handler_exp_4(context* ctx) {
    global = 0;
}

void timer_handler_exp_8(context* ctx) {
    printf("%d ", global++);
    eoi(TIMER_IRQ);
}

void timer_handler_exp_9(context* ctx) {
    printf("%d ", global++);
    eoi(TIMER_IRQ);
    go_sti();
    endless_loop();
}

static const u32 N = 15;
void timer_handler_exp_10(context* ctx) {
    printf("%d ", global++);
    if (global <= N) {
        eoi(TIMER_IRQ);
        go_sti();
    }
    endless_loop();
}

void timer_handler_exp_11(context* ctx) {
    printf("%d ", global++);
    if (global <= N) {
        eoi(TIMER_IRQ);
    }
    endless_loop();
}

void timer_handler_exp_12(context* ctx) {
    printf("%d ", global++);
    if (global <= N) {
        go_sti();
    }
    endless_loop();
}

void timer_handler_exp_14(context* ctx) {
    printf("%d ", global++);
}

void timer_handler_exp_15(context* ctx) {
    printf("%d ", global++);
    endless_loop();
}

void timer_handler_exp_16(context* ctx) {
    printf("%d ", global++);
}

void timer_handler_exp_18(context* ctx) {
    printf("%d ", global++);
    endless_loop();
}

void timer_handler_exp_19(context* ctx) {
    printf("%d ", global++);
    eoi(TIMER_IRQ);
}

void c_delay_with_printf() {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 50000; j++) {
            write_to_port(0x80, 0);
        }
        printf("%d ", i);
    }
    printf("\n");
}

void timer_handler_exp_20(context* ctx) {
    pic8259_disable_device(TIMER_IRQ);
    c_delay_with_printf();
    go_sti();
    endless_loop();
}

void keyboard_handler_exp_13(context* ctx) {
    u8 scancode = read_from_port(0x60);
    printf("%x ", scancode);
}

void keyboard_handler_exp_16(context* ctx) {
    u8 scancode = read_from_port(0x60);
    printf("%x ", scancode);
    endless_loop();
}

void keyboard_handler_exp_17(context* ctx) {
    u8 scancode = read_from_port(0x60);
    printf("%x ", scancode);
    go_sti();
    endless_loop();
}

void keyboard_handler_exp_18(context* ctx) {
    u8 scancode = read_from_port(0x60);
    printf("%x ", scancode);
    eoi(KEYBOARD_IRQ);
}

void keyboard_handler_exp_19(context* ctx) {
    u8 scancode = read_from_port(0x60);
    printf("%x ", scancode);
    endless_loop();
}

void keyboard_handler_exp_20(context* ctx) {
    u8 scancode = read_from_port(0x60);
    printf("%x ", scancode);
    endless_loop();
}

InterruptHandler timer_handler = timer_handler_exp_20;
InterruptHandler keyboard_handler = keyboard_handler_exp_20;

void experiment() {
    setup_interrupts(INTERRUPT_GATE);
    bool auto_eoi = true;
    pic8259_setup(auto_eoi);
    pic8259_enable_device(0); // Enable Timer interrupt (IRQ0)
    pic8259_enable_device(1); // Enable Keyboard interrupt (IRQ1)

    override_interrupt_handler(0x20, timer_handler, INTERRUPT_GATE); // override handle for timer
    override_interrupt_handler(0x21, keyboard_handler, INTERRUPT_GATE); // override handle for keyboard

    go_sti(); // Enable interrupts
    /*
    while (true) {
        printf("%d ", global++);
        c_delay_without_printf(1000);
    }
    */
}


void kernel_entry() {
    init_printer();
    experiment();
    endless_loop();
}