; C equivalent: int _asp_read_byte(char *buffer);
; The input buffer is used as the parameter

section .bss
_STDIN equ 0
_STDOUT equ 1

section .text

global _asp_read_byte

_asp_read_byte:
     ; we will use the read syscall
     ; for _asp_read_bytes
     mov rax, 0      ; read syscall
     mov rsi, rdi    ; the pointer to the buffer
     mov rdi, _STDIN ; from stdin
     mov rdx, 1      ; just 1 byte
     syscall         ; syscall
     ret             ; the return value must be in rax