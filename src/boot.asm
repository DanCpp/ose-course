[BITS 16]

[ORG 0x7C00]

; now booting from floppy disk
; we would contain our kernel starting from 0x7E00

HIGH_LIMIT_SEGMENT equ (0x7E00 + KERNEL_SIZE) / 16
SECTORS_PER_TRACK equ 18
HEADS_PER_CYLINDER equ 2

.start:
  ; init stack
  cli
  xor ax, ax
  mov ss, ax
  mov sp, 0x7C00

  ; initialize CHS = (0, 0, 2) 
  xor cx, cx                      ; cylinder 0
  xor dh, dh                      ; head 0
  mov cl, 2                       ; sector 2

  ; initialize ES : BX = 0x7E00
  mov si, 0x07E0
  xor bx, bx
.load_kernel: 
  mov es, si

.try_read:
  mov ah, 0x02
  mov al, 1                       ; read 1 sector
  int 0x13
  jc .read_failure

.read_success:
; select next CHS coordinate
  inc cl                          ; sector + 1
  cmp cl, SECTORS_PER_TRACK + 1   ; cl should be less than 19 (Floppy disk)
  jl .done_indexing
  mov cl, 1                       ; reset sector to 1
  inc dh                          ; heads + 1
  cmp dh, HEADS_PER_CYLINDER + 1  ; dh should be less than 2 (Floppy disk)
  jl .done_indexing
  xor dh, dh                      ; reset head to 0
  inc ch                          ; cylinder + 1


.done_indexing:

  add si, 0x20                    ; increment segment will be +0x200
  cmp si, HIGH_LIMIT_SEGMENT      ; compare to end
  jl .load_kernel

.stuck:
  jmp .stuck


.read_failure:
  int 0x18                        ; get back to BIOS

times 510-($-$$) db 0
dw 0xAA55
