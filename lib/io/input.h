#ifndef _ASP_INPUT_
#define _ASP_INPUT_

#include "../../Utils/aspire_typedefs.h"
#include "../../Utils/aspire_helpers.h"

#define _ASP_INPUT_BUF_INIT_SIZE 24 // at initialization the buffer is 24 bytes long

static aBptr_t _asp_Input_buffer = NULL;     // the input buffer
static aSize_t _asp_Input_buffer_size = 0;   // the size of the input buffer[how many bytes can it store]
static aSize_t _asp_Input_buffer_filled = 0; // how many bytes have been occupied?

/*
 Based on the system, the required assembly file from _asm_ is used. These assembly files contain the abstraction required to communicate with the OS directly using syscalls.
 The buffer starts with a fixed size of 24 bytes which is expanded according to the needs. Everything is read as characters and thus needs to be converted into the needed
 format. 
 In case(I haven't checked it nor do i know during the time of writing this) other OS have different format for syscall(reading from keyboard) then we will have to implement for each system.
*/

aBool _asp_init_input_buffer();

#endif