[BITS 16]

[ORG 0x7C00]

; now booting from floppy disk
; we would contain our kernel starting from 0x7E00

.start:
  ; init stack
  cli
  xor ax, ax
  mov ss, ax
  mov sp, 0x7C00

  ; initialize CHS = (0, 0, 2) 
  xor cx, cx
  xor dh, dh
  mov cl, 2 ; sector 2

  ; initialize ES : BX = 0x7E00
  mov si, 0x07E0
  xor bx, bx
.load_kernel: 
  mov es, si


  xor di, di ; problem counter
.try_read:
  mov ah, 0x02
  mov al, 1 ; read 1 sector
  int 0x13

  jnc .read_success
  inc di
  cmp di, 4
  jle .try_read
  
  int 0x18 ; get it back

.read_success:

  ; select next CHS coordinate
  inc cl
  cmp cl, 19 ; cl should be less than 19 (Floppy disk)
  jl .done_indexing
  mov cl, 1
  inc dh
  cmp dh, 2 ; dh should be less than 2 (Floppy disk)
  jl .done_indexing
  xor dh, dh
  inc ch


.done_indexing:

  add si, 0x20
  cmp si, (0x7E00 + KERNEL_SIZE) / 16
  jl .load_kernel

.stuck:
  jmp .stuck

times 510-($-$$) db 0
dw 0xAA55
