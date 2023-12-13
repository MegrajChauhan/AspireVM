; C equivalent: int _asp_read_byte(char *buffer); size_t _asp_read_bytes(char * buffer);
; size_t _asp_read_until(char *buffer, char until); // reads until encountered until on input
; The input buffer is used as the parameter

section .data
_STDIN equ 0
_STDOUT equ 1

section .text

global _asp_read_byte
global _asp_read_bytes
global _asp_read_until

_asp_read_byte:
     ; we will use the read syscall
     ; for _asp_read_byte
     mov rax, 0      ; read syscall
     mov rsi, rdi    ; the pointer to the buffer
     mov rdi, _STDIN ; from stdin
     mov rdx, 1      ; just 1 byte
     syscall         ; syscall
     ret             ; the return value must be in rax

_asp_read_bytes:
     ; for _asp_read_bytes
     mov rax, 0      ; read syscall
     mov rdx, rsi    ; as many bytes as requested 
     mov rsi, rdi    ; the pointer to the buffer
     mov rdi, _STDIN ; from stdin
     syscall         ; syscall
     ret             ; the return value must be in rax

_asp_read_until:
     ; rdi contains the pointer to the buffer
     ; rsi contains the character to not read[This character is converted into string terminator] 
     mov r8, rsi ; save the character
     mov r9, 0   ; acts as character counter
read_loop:
     call _asp_read_byte
     cmp rax, 1      ; was 1 byte read
     jne read_error
     mov rdi, rsi    ; restore the changed pointer
     cmp [rdi], r8   ; check if the read character is the same as we don't want
     je read_finished
     inc r9          ; read another character
     inc rdi         ; move to read next character
     jmp read_loop
read_finished:
     mov BYTE [rdi], 0 ; null terminate this string and replace the unneeded character
     mov rax, r9       ; the number of bytes read is the return value
     ret
read_error:
     ret
    