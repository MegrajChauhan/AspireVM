#ifndef _ASP_INPUT_
#define _ASP_INPUT_

#include "../../Utils/aspire_typedefs.h"
#include "../../Utils/aspire_helpers.h"
#include <stdlib.h>

#define _ASP_INPUT_BUF_INIT_SIZE 24 // at initialization the buffer is 24 bytes long

static aBptr_t _asp_Input_buffer = NULL;     // the input buffer
static aSize_t _asp_Input_buffer_size = 0;   // the size of the input buffer[how many bytes can it store]
static aSize_t _asp_Input_buffer_filled = 0; // how many bytes have been occupied?

// defined in assembly
extern int _asp_read_byte(aBptr_t buffer);
extern int _asp_read_bytes(aBptr_t buffer);
extern int _asp_read_until(aBptr_t *buffer, char until);

aBool _asp_init_input_buffer();

#endif