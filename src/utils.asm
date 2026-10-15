[BITS 32]

[GLOBAL go_cli]
go_cli:
  cli
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