[BITS 16]

[ORG 0x7C00]


start:
  mov ds, ax
  mov ss, ax
  mov sp, 0x7C00

  mov ah, 0x0E
  mov si, msg
print_char:
  lodsb
  int 0x10
  test al, al
  jnz print_char

loop:
  jmp loop

msg:
  db 'Hello, World!', 0x0A, 0x0D, 0

times 510-($-$$) db 0
dw 0xAA55
