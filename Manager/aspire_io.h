#ifndef _ASP_INPUT_
#define _ASP_INPUT_

#include "../Utils/aspire_typedefs.h"
#include "../Utils/aspire_helpers.h"
// #include "../lib/io/IOread.h"             // for reading from the terminal
#include "../lib/thread/aspire_threads.h" // IO operations need to be thread safe
// #include "aspire_buffer.h"
#include <stdlib.h>
// #include <stdio.h> // for now we will be using the stdio provided by the C standard library for our IO

// #define _ASP_IN_BUF_LEN 512 // 512 bytes

// static aBptr_t _In_buffer = NULL;   // the input buffer
static _Asp_Mutex *_In_lock = NULL; // Input lock
// static aSize_t _In_buffer_filled_to = 0;
// static aBptr_t _In_buffer_first_byte = NULL;

extern int _asp_read_byte(aBptr_t buffer);
extern int _asp_read_bytes(aBptr_t buffer, aSize_t len);
extern int _asp_read_until(aBptr_t buffer, char until);
// extern void _asp_set_buf_limit(aSize_t buf_lim, aSize_t* buffer_filled_to, aBptr_t buffer);

extern int _asp_write_byte(aBptr_t byte);
extern int _asp_write_bytes(aBptr_t buffer, aSize_t len);
extern int _asp_write_string(aBptr_t buffer);

// static aBool _asp_InBuf_read_until_newline()
// {4142434445464748
//     // this overwrites anything previously in the buffer
//     // fill the buffer with a new input terminated by '\n'

//     // // to check if the return was an error, we simply check if 0 bytes were read
//     // if (_In_buffer_filled_to == 0)
//     //     return aFalse;
//     // _In_pointer = _In_buffer;
//     // return aTrue;
// }

aBool _asp_InBuf_init(); // initialize the Input buffer

aBool _asp_In_readChar(aBptr_t _store_in); // reads a byte and sends the byte back

/*
  For reading strings, the variables themselves act as the buffer
*/
aBool _asp_In_readStr(aBptr_t _store_in, aSize_t len);

void _asp_InBuf_destroy();

#endif