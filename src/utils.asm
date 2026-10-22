[BITS 32]

[GLOBAL go_cli]
go_cli:
  cli
  ret

[GLOBAL go_lidt]
go_lidt:
  ; esp + 4 - pointer to the IDT descriptor
  mov eax, dword [esp + 4]
  lidt [eax]
  ret

[GLOBAL endless_loop]
endless_loop:
  jmp endless_loop

; my calling convention:
; edi, esi, ecx - first three arguments
; edi, esi, ecx - must be unchanged by the end of a function
; eax - return value

is_inside:
  ; edi - dst
  ; esi - src
  ; ecx - size

  ; src <= dst && dst < src + size
  cmp esi, edi
  ja .false

  lea eax, [esi + ecx]
  cmp edi, eax
  jb .true



.false:
  xor eax, eax
  jmp .end


.true:
  xor eax, eax
  inc eax

.end:
  ret


[GLOBAL memmove] ; void* memmove(void* dst, const void* src, size_t size)
memmove:
  push ebp
  mov ebp, esp
  push ebx
  push esi
  push edi

  ; ebp + 8 - dst
  ; ebp + 12 - src
  ; ebp + 16 - size

  mov edi, dword [ebp + 8] ; dst
  mov esi, dword [ebp + 12] ; src
  mov ecx, dword [ebp + 16] ; size

  call is_inside
  test eax, eax
  jnz .go_backwards

.go_forwards:
  cld
  rep movsb
  jmp .done

.go_backwards:
  std
  lea esi, [esi + ecx - 1]
  lea edi, [edi + ecx - 1]
  rep movsb

.done:
  cld
  mov eax, dword [ebp + 8] ; return dst

  pop edi
  pop esi
  pop ebx
  pop ebp
  ret

[EXTERN universal_handler]

[GLOBAL collect_context]
collect_context:
  push ds
  push es
  push fs
  push gs
  pusha
  mov ebx, esp
  and esp, -16
  cld
  mov eax, 0x10
  mov ds, eax
  mov es, eax
  mov fs, eax
  mov gs, eax
  push 0
  push 0
  push 0
  push ebx
  call universal_handler

[GLOBAL setup_registers]
setup_registers:
  mov eax, 0
  mov ecx, 1
  mov edx, 2
  mov ebx, 3
  mov ebp, 4
  mov esi, 5
  mov edi, 6
  ret

[GLOBAL first_challenge]
first_challenge:
  idiv eax
  ret

[GLOBAL second_challenge]
second_challenge:
  int 0x42
  ret

[GLOBAL third_challenge]
third_challenge:
  sti
  ret