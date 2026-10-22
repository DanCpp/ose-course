#include "kernel_panic.h"
#include "memory_utils.h"
#include "undead.h"
#include "types.h"
#include "assert.h"
#include "vga_driver.h"

#include <stdbool.h>


#define VECTORS_NUM 256
#define TRAMPOLINE_SIZE 0x8

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define GET_LOW_16_BITS(x) ((u16)((x) & 0xFFFF))


extern void collect_context();
extern void go_lidt(void* idt_descriptor);

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

#pragma pack(push, 1)
typedef struct {
  u16 offset_low;
  u16 segment_selector;
  u8 reserved;
  GateType gate_type : 3;
  u8 D0 : 2; // 0b01
  u8 DPL : 2; // Descriptor Privilege Level
  u8 present : 1;
  u16 offset_high;
} InterruptDescriptor;
#pragma pack(pop)

void universal_handler(context* ctx) {
  kernel_panic(
    "unhandled interrupt #%u at %x : %x\n\n"
    "Registers:\n"
    "EAX: %x, EBX: %x, ECX: %x, EDX: %x\n"
    "ESI: %x, EDI: %x, EBP: %x, ESP: %x\n"
    "Segment registers:\n"
    "DS: %x, ES: %x, FS: %x, GS: %x\n"
    "EFLAGS: %x\n"
    "Error code: %x\n"
    "", 
    GET_LOW_16_BITS(ctx->vector), ctx->cs, ctx->eip,
    ctx->eax, ctx->ebx, ctx->ecx, ctx->edx,
    ctx->esi, ctx->edi, ctx->ebp, ctx->esp,
    GET_LOW_16_BITS(ctx->ds), GET_LOW_16_BITS(ctx->es), GET_LOW_16_BITS(ctx->fs), GET_LOW_16_BITS(ctx->gs),
    ctx->eflags,
    ctx->error_code);
}


bool comes_with_error_code(u16 vector) {
  static const u16 vectors_with_error_code[] = {
    8, 10, 11, 12, 13, 14, 17, 21
  };

  for (size_t i = 0; i < ARRAY_SIZE(vectors_with_error_code); ++i) {
    if (vectors_with_error_code[i] == vector) {
      return true;
    }
  }

  return false;
}

InterruptDescriptor setup_interrupt_descriptor(const void* handler) {
  return (InterruptDescriptor) {
    .offset_low = (u32)handler & 0xFFFF,
    .segment_selector = 0x08, // Kernel code segment
    .reserved = 0,
    .gate_type = INTERRUPT_GATE,
    .D0 = 0b01,
    .DPL = 0b00,
    .present = 1,
    .offset_high = ((u32)handler >> 16) & 0xFFFF
  };
}

void* generate_trampolines() {
  u8* trampolines = malloc_undead(VECTORS_NUM * TRAMPOLINE_SIZE, 8);

  for (u16 vector = 0; vector < VECTORS_NUM; ++vector) {
    u8* trampoline = trampolines + vector * TRAMPOLINE_SIZE;
    size_t offset = 0;
    if (!comes_with_error_code(vector)) {
      trampoline[offset++] = 0x50; // push eax
    }

    trampoline[offset++] = 0x6A; // push
    trampoline[offset++] = vector; // vector number

    u32 offset_to_collect_context = (u32)collect_context - ((u32)trampoline + offset + 5);
    trampoline[offset++] = 0xE9; // jmp
    *(u32*)(trampoline + offset) = offset_to_collect_context;
    offset += 4;
    assert(offset <= TRAMPOLINE_SIZE, "Trampoline size exceeded");
  }

  return trampolines;
}


void* generate_idt(const u8* trampolines) {
  InterruptDescriptor* idt = malloc_undead(VECTORS_NUM * sizeof(InterruptDescriptor), 8);

  for (u16 vector = 0; vector < VECTORS_NUM; ++vector) {
    const u8* trampoline = trampolines + vector * TRAMPOLINE_SIZE;
    idt[vector] = setup_interrupt_descriptor(trampoline);
  }

  return idt;
}

void setup_interrupts() {
  assert(sizeof(InterruptDescriptor) == 8, "InterruptDescriptor size is not 8 bytes");
  assert(sizeof(context) == 17 * sizeof(u32), "Context size is not 17 * sizeof(u32)");

  void* trampolines = generate_trampolines();
  void* idt = generate_idt(trampolines);

  u16 idt_limit = VECTORS_NUM * sizeof(InterruptDescriptor) - 1;
  u64 idt_base = (u64)idt;
  u64 idt_pseudodescriptor = (idt_base << 16) | idt_limit;
  go_lidt(&idt_pseudodescriptor);
}