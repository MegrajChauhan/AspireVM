; C equivalent: int _asp_read_byte(char *buffer); size_t _asp_read_bytes(char * buffer);
; size_t _asp_read_until(char *buffer, char until); // reads until encountered until on input
; The input buffer is used as the parameter

section .data
_STDIN equ 0
_STDOUT equ 1
; _BUF_LIM dq 0
; _BUF_FILL_SIZE dq 0 ;; a pointer
; _BUFFER dq 0 ;; pointer to the buffer

section .text

global _asp_read_byte
global _asp_read_bytes
global _asp_read_until
global _asp_set_buf_limit

global _asp_write_byte
global _asp_Write_bytes

; global _asp_read_until_or_filled

; ;; void _asp_set_buf_limit(aSize_t buf_lim, aSize_t* buffer_filled_to, aBptr_t buffer);
; _asp_set_buf_limit:
;      ; this will set the buffer limit
;      ; rdi contains the buffer limit
;      mov [_BUF_LIM], rdi
;      mov [_BUF_FILL_SIZE], rsi ;; save the pointer
;      mov [_BUFFER], rdi ;; the buffer
;      ret ; void function

_asp_read_byte:
     ; we will use the read syscall
     ; for _asp_read_byte
     mov rax, 0      ; read syscall
     mov rsi, rdi    ; the pointer to the buffer
     mov rdi, _STDIN ; from stdin
     mov rdx, 1      ; just 1 byte
     syscall         ; syscall
     ret             ; the return value must be in rax

;; int _asp_read_bytes(aBptr_t buffer, aSize_t len);
_asp_read_bytes: ;; used to read strings of whose we know the size of
     ; for _asp_read_bytes
     mov rax, 0      ; read syscall
     mov rdx, rsi    ; as many bytes as requested 
     mov rsi, rdi    ; the pointer to the buffer
     mov rdi, _STDIN ; from stdin
     syscall         ; syscall
     ret             ; the return value must be in rax

; _asp_read_until:
;      ; rdi contains the pointer to the buffer
;      ; rsi contains the character to not read 
;      mov r8, rsi ; save the character
;      mov r9, 0   ; acts as character counter
;      mov r10, _BUF_LIM
; read_loop:
;      call _asp_read_byte
;      cmp rax, 1      ; was 1 byte read
;      jne read_error
;      mov rdi, rsi    ; restore the changed pointer
;      cmp [rdi], r8   ; check if the read character is the same as we don't want
;      je read_finished
;      inc r9          ; read another character
;      inc rdi         ; move to read next character
;      cmp r9, r10     ;; check if the buffer is full
;      je read_finished
;      jmp read_loop
; read_finished:
;      mov [rdi], 0
;      mov rax, r9       ; the number of bytes read is the return value
;      ret
; read_error:
;      ret
 
 _asp_write_byte:
     ; rdi contains the character to print
     mov rax, 1       ;; write syscall
     mov rsi, rdi     ;; the character to print
     mov rdi, _STDOUT ;; the standard output
     mov dx, 1        ;; just 1 character
     ret              ;; should return 1 on success

_asp_Write_bytes:
     ; rdi contains the characters to print