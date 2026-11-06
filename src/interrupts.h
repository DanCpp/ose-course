#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#define VECTORS_NUM 256
#define TRAMPOLINE_SIZE 0x8

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define GET_LOW_16_BITS(x) ((u16)((x) & 0xFFFF))

typedef enum {
  INTERRUPT_GATE = 0b110,
  TRAP_GATE = 0b111
} GateType;

typedef struct {
  u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
  u32 gs, fs, es, ds;
  u32 vector;
  u32 error_code;
  u32 eip;
  u32 cs;
  u32 eflags;
} context;

typedef void (*InterruptHandler)(context* ctx);

void setup_interrupts();

void override_interrupt_handler(u16 vector, InterruptHandler handler, GateType gate_type);

#endif // INTERRUPTS_H