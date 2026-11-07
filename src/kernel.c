#include "vga_driver.h"
#include "assert.h"
#include "undead.h"
#include "types.h"
#include "interrupts.h"
#include "pic.h"

#define TIMER_IRQ 0
#define KEYBOARD_IRQ 1


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

void c_delay_with_printf() {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 50000; j++) {
            write_to_port(0x80, 0);
        }
        printf("%d ", i);
    }
    printf("\n");
}

static int global = 0;
static const int N = 15;

void to_universal(context* ctx) {
    return;
}

void timer_handler_global_increment_printf(context* ctx) {
    printf("%d ", global++);
}

void timer_handler_global_zero(context* ctx) {
    global = 0;
}

void timer_handler_global_increment_printf_eoi(context* ctx) {
    printf("%d ", global++);
    eoi(TIMER_IRQ);
}

void timer_handler_global_increment_printf_eoi_sti_endless_loop(context* ctx) {
    printf("%d ", global++);
    eoi(TIMER_IRQ);
    go_sti();
    endless_loop();
}

void timer_handler_global_increment_printf_endless_loop_optional_sti_eoi(context* ctx) {
    printf("%d ", global++);
    if (global <= N) {
        eoi(TIMER_IRQ);
        go_sti();
    }
    endless_loop();
}

void timer_handler_global_increment_printf_endless_loop_optional_eoi(context* ctx) {
    printf("%d ", global++);
    if (global <= N) {
        eoi(TIMER_IRQ);
    }
    endless_loop();
}

void timer_handler_global_increment_printf_endless_loop_optional_sti(context* ctx) {
    printf("%d ", global++);
    if (global <= N) {
        go_sti();
    }
    endless_loop();
}

void timer_handler_global_printf_endless_loop(context* ctx) {
    printf("%d", global);
    endless_loop();
}

void timer_handler_final_boss(context* ctx) {
    pic8259_disable_device(TIMER_IRQ);
    c_delay_with_printf();
    go_sti();
    c_delay_with_printf();
}

void keyboard_handler_scan_and_printf(context* ctx) {
    u8 scancode = read_from_port(0x60);
    printf("%x ", scancode);
}

void keyboard_handler_scan_and_printf_endless_loop(context* ctx) {
    u8 scancode = read_from_port(0x60);
    printf("%x ", scancode);
    endless_loop();
}

void keyboard_handler_scan_and_printf_sti_endless_loop(context* ctx) {
    u8 scancode = read_from_port(0x60);
    printf("%x ", scancode);
    go_sti();
    endless_loop();
}

void keyboard_handler_scan_and_printf_eoi(context* ctx) {
    u8 scancode = read_from_port(0x60);
    printf("%x ", scancode);
    eoi(KEYBOARD_IRQ);
}

static InterruptHandler timer_handlers[21] = {
    NULL, // nothing
    NULL, // first
    to_universal, // second
    NULL, // third
    timer_handler_global_increment_printf, // fourth
    timer_handler_global_zero, // fifth
    timer_handler_global_increment_printf, // sixth
    timer_handler_global_increment_printf, // seventh
    timer_handler_global_increment_printf_eoi, // eighth
    timer_handler_global_increment_printf_eoi_sti_endless_loop, // ninth
    timer_handler_global_increment_printf_endless_loop_optional_sti_eoi, // tenth
    timer_handler_global_increment_printf_endless_loop_optional_eoi, // eleventh
    timer_handler_global_increment_printf_endless_loop_optional_sti, // twelfth
    NULL, // thirteenth
    timer_handler_global_increment_printf, // fourteenth
    timer_handler_global_printf_endless_loop, // fifteenth
    timer_handler_global_increment_printf, // sixteenth
    timer_handler_global_increment_printf, // seventeenth
    timer_handler_global_printf_endless_loop, // eighteenth
    timer_handler_global_increment_printf_eoi, // nineteenth
    timer_handler_final_boss, // twentieth
};

static InterruptHandler keyboard_handlers[21] = {
    NULL, // nothing
    NULL, // first
    NULL, // second
    to_universal, // third
    NULL, // fourth
    NULL, // fifth
    NULL, // sixth
    NULL, // seventh
    NULL, // eighth
    NULL, // ninth
    NULL, // tenth
    NULL, // eleventh
    NULL, // twelfth
    keyboard_handler_scan_and_printf, // thirteenth
    keyboard_handler_scan_and_printf, // fourteenth
    keyboard_handler_scan_and_printf, // fifteenth
    keyboard_handler_scan_and_printf_endless_loop, // sixteenth
    keyboard_handler_scan_and_printf_sti_endless_loop, // seventeenth
    keyboard_handler_scan_and_printf_eoi, // eighteenth
    keyboard_handler_scan_and_printf_endless_loop, // nineteenth
    keyboard_handler_scan_and_printf_endless_loop, // twentieth
};

static bool auto_eoi_enabled_on[21] = {
    false, // nothing
    false, // first
    false, // second
    false, // third
    true, // fourth
    true, // fifth
    true, // sixth
    false, // seventh
    false, // eighth
    false, // ninth
    false, // tenth
    false, // eleventh
    true, // twelfth
    true, // thirteenth
    true, // fourteenth
    true, // fifteenth
    true, // sixteenth
    true, // seventeenth
    false, // eighteenth
    false, // nineteenth
    true, // twentieth
};

static GateType gate_types_on[21] = {
    INTERRUPT_GATE, // nothing
    INTERRUPT_GATE, // first
    INTERRUPT_GATE, // second
    INTERRUPT_GATE, // third
    INTERRUPT_GATE, // fourth
    INTERRUPT_GATE, // fifth
    INTERRUPT_GATE, // sixth
    INTERRUPT_GATE, // seventh
    INTERRUPT_GATE, // eighth
    INTERRUPT_GATE, // ninth
    INTERRUPT_GATE, // tenth
    TRAP_GATE, // eleventh
    INTERRUPT_GATE, // twelfth
    INTERRUPT_GATE, // thirteenth
    INTERRUPT_GATE, // fourteenth
    INTERRUPT_GATE, // fifteenth
    INTERRUPT_GATE, // sixteenth
    INTERRUPT_GATE, // seventeenth
    TRAP_GATE, // eighteenth
    TRAP_GATE, // nineteenth
    INTERRUPT_GATE, // twentieth
};


int experiment_number = 20;

void experiment() {
    InterruptHandler timer_handler = timer_handlers[experiment_number];
    InterruptHandler keyboard_handler = keyboard_handlers[experiment_number];
    bool auto_eoi = auto_eoi_enabled_on[experiment_number];
    GateType gate_type = gate_types_on[experiment_number];


    setup_interrupts(gate_type);
    pic8259_setup(auto_eoi);
    if (timer_handler != NULL) {
        pic8259_enable_device(0); // Enable Timer interrupt (IRQ0)
        if (timer_handler != to_universal) {
            override_interrupt_handler(0x20, timer_handler, gate_type); // override handle for timer
        }
    }

    if (keyboard_handler != NULL) {
        pic8259_enable_device(1); // Enable Keyboard interrupt (IRQ1)
        if (keyboard_handler != to_universal) {
            override_interrupt_handler(0x21, keyboard_handler, gate_type); // override handle for keyboard
        }
    }
    go_sti(); // Enable interrupts
    // code below is for experiments 5 and 6
    if (experiment_number == 5 || experiment_number == 6) {
        while (true) {
            printf("%d ", global++);
            c_delay_without_printf(1000);
        }
    }
}


void kernel_entry() {
    init_printer();
    experiment();
    endless_loop();
}