; C equivalent: int _asp_read_byte(char *buffer); size_t _asp_read_bytes(char * buffer);
; size_t _asp_read_until(char *buffer, char until); // reads until encountered until on input
; The input buffer is used as the parameter

section .data
_STDIN equ 0
_STDOUT equ 1
_BUF_LIM dq 0

section .text

global _asp_read_byte
global _asp_read_bytes
global _asp_read_until
global _asp_set_buf_limit

_asp_set_buf_limit:
     ; this will set the buffer limit
     ; rdi contains the buffer limit
     mov [_BUF_LIM], rdi
     ret ; void function

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
     cmp r9, _BUF_LIM
     je read_finished ; just terminate
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

; ;; int _asp_read_string(aBptr_t buffer, aSize_t len);
; ; reads len number of bytes till a newline is entered which is replaced by '\0' after which the other characters are written to the _BUFFER
; _asp_read_until_or_filled_and:
;     mov r8, 10   ; the newline character
;     mov r9, 0    ; counter
;     mov r10, rsi ; the length of the buffer
; buffer_read_loop:
;     call _asp_read_byte
;     cmp rax, 1        ; was 1 byte read
;     jne read_error    ; reading error
;     mov rdi, rsi      ; restore the buffer pointer
;     cmp [rdi], r8     ; check if the read character is the same as we don't want
;     je  read_finished ; this implies we read and encountered the character 'until' before reaching the end of the buffer
;     inc r9            ; read another character
;     inc rdi           ; move to read next character
;     cmp r9, r10       ;; check if the buffer is full
;     je not_encountered  ; but this implies that the buffer is full and yet we haven't received the 'until' character
;     jmp read_loop
; not_encountered:
;     ; this does the same as the above but the read characters go into _BUFFER rather than buffer
;     ; we loop until we encounter a newline or hit the end of the buffer
;     ; the same code can be applied
;     mov r9, 0 ; restart the counter
;     ; r10 is preserved
;     push r10
;     mov r10, _BUF_LIM       ; move the new length
;     mov rdi, _BUF_FILL_SIZE ; move the buffer
;     jmp buffer_read_loop    ; go back