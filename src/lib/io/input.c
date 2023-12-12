#include "../../../lib/io/input.h"

aBool _asp_init_input_buffer()
{
    // initialize the input buffer
    _asp_Input_buffer = (aBptr_t)malloc(sizeof(aByte) * _ASP_INPUT_BUF_INIT_SIZE);
    if (_asp_Input_buffer == NULL)
        return aFalse; // the buffer couldn't be initialized
    _asp_Input_buffer_filled = 0; // no input yet
    _asp_Input_buffer_size = _ASP_INPUT_BUF_INIT_SIZE;
    return aTrue;
}