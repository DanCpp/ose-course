[BITS 16]

; Bootloader
; KERNEL_SIZE in bytes

SECTORS_TO_READ equ (KERNEL_SIZE + 511) / 512

SECTORS_MAX equ 18
HEADS_MAX equ 1

.init:
  ; Setup stack
  cli                            ; disable interrupts
  cld                            ; clear direction flag
  xor ax, ax
  mov ss, ax
  mov sp, 0x7C00                 ; set stack pointer

  ; CHS (start: (0, 0, 2))
  xor cx, cx                     ; cylinder = 0
  xor dh, dh                     ; head = 0
  mov cl, 2                      ; sector = 2

  xor bx, bx
  mov si, 0x07E0

  mov di, SECTORS_TO_READ
.load_kernel:
  mov es, si
  mov ah, 0x02
  mov al, 1                      ; read 1 sector
  int 0x13
  jc .fatal                      ; jump if error

.configure_chs:
  inc cl                         ; next sector
  cmp cl, SECTORS_MAX + 1
  jl .done_configure_chs
  mov cl, 1                      ; reset sector
  inc dh                         ; next head
  cmp dh, HEADS_MAX + 1
  jl .done_configure_chs
  xor dh, dh                     ; reset head
  inc ch                         ; next cylinder

.done_configure_chs:
  add si, 0x20
  dec di
  jnz .load_kernel


.protected_mode_configuration:
  lgdt [gdt_pseudodescriptor]
  mov eax, cr0
  or eax, 1                      ; set PE bit
  mov cr0, eax

  jmp 0x08 : .trampoline         ; 0x08 - gdt offset of kernel code segment descriptor

[BITS 32]
.trampoline:
  mov eax, 0x10                  ; 0x10 - gdt offset of kernel data segment descriptor
  mov ds, eax
  mov ss, eax
  mov es, eax
  mov fs, eax
  mov gs, eax

[EXTERN kernel_entry]
  call kernel_entry

gdt_pseudodescriptor:
  .limit:                                   dw 0x17
  .base:                                    dd .gdt

align 8
.gdt:
  .null_descriptor:                         dq 0
  kernel_code_segment_descriptor:
    .limit_low:                             dw 0xFFFF
    .base_low:                              dw 0x0000
    .base_mid:                              db 0x00
    .P_DPL_S_TYPE:                          db 0b10011010
    .G_DorB_0_AVL_limit:                    db 0b11001111
    .base_high:                             db 0x00
  kernel_data_segment_descriptor:
    .limit_low:                             dw 0xFFFF
    .base_low:                              dw 0x0000
    .base_mid:                              db 0x00
    .P_DPL_S_TYPE:                          db 0b10010010
    .G_DorB_0_AVL_limit:                    db 0b11001111
    .base_high:                             db 0x00


%include "./src/utils.asm"


[BITS 16]
.fatal:
  int 0x18                        ; get back to BIOS

times 510-($-$$) db 0
dw 0xAA55
