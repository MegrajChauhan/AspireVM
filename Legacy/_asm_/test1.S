// C equivalent: int _asp_read_byte(char *buffer)// size_t _asp_read_bytes(char * buffer)//
// size_t _asp_read_until(char *buffer, char until)// // reads until encountered until on input
// The input buffer is used as the parameter

.intel_syntax noprefix

.data
_STDIN: .byte 0
_STDOUT: .byte 1
_BUF_LIM: .byte 0

.text

.global _asp_read_byte
.global _asp_read_bytes
.global _asp_read_until
.global _asp_set_buf_limit

_asp_set_buf_limit:
     // this will set the buffer limit
     // rdi contains the buffer limit
     mov _BUF_LIM, rdi
     ret // void function

_asp_read_byte:
     // we will use the read syscall
     // for _asp_read_byte
     mov rax, 0      // read syscall
     mov rsi, rdi    // the pointer to the buffer
     mov rdi, _STDIN // from stdin
     mov rdx, 1      // just 1 byte
     syscall         // syscall
     ret             // the return value must be in rax

_asp_read_bytes:
     // for _asp_read_bytes
     mov rax, 0      // read syscall
     mov rdx, rsi    // as many bytes as requested 
     mov rsi, rdi    // the pointer to the buffer
     mov rdi, _STDIN // from stdin
     syscall         // syscall
     ret             // the return value must be in rax

_asp_read_until:
     // rdi contains the pointer to the buffer
     // rsi contains the character to not read[This character is converted into string terminator] 
     mov r8, rsi // save the character
     mov r9, 0   // acts as character counter
read_loop:
     call _asp_read_byte
     cmp rax, 1      // was 1 byte read
     jne read_error
     cmp r9, _BUF_LIM
     je read_finished // just terminate
     mov rdi, rsi    // restore the changed pointer
     cmp [rdi], r8   // check if the read character is the same as we don't want
     je read_finished
     inc r9          // read another character
     inc rdi         // move to read next character
     jmp read_loop
read_finished:
     movq [rdi], 0 // null terminate this string and replace the unneeded character
     mov rax, r9       // the number of bytes read is the return value
     ret
read_error:
     ret
    