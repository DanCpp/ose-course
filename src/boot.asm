[BITS 16]

[ORG 0x7C00]

; Bootloader
; KERNEL_SIZE in bytes

SECTORS_TO_READ equ (KERNEL_SIZE + 511) / 512

SECTORS_MAX equ 18
HEADS_MAX equ 1

.init:
  ; Setup stack
  cli                            ; disable interrupts
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
.load_kernel
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


.stuck:
  jmp .stuck

.fatal:
  int 0x18                        ; get back to BIOS

times 510-($-$$) db 0
dw 0xAA55
