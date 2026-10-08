[BITS 32]

[GLOBAL go_cli]
go_cli:
  cli
  ret

[GLOBAL endless_loop]
endless_loop:
  jmp endless_loop