#include "../../Manager/aspire_input.h"

aBool _asp_InBuf_init()
{
    // _In_buffer = (aBptr_t)malloc(sizeof(aBptr_t) * _ASP_IN_BUF_LEN); // get the buffer initialized
    // if (_In_buffer == NULL)
    //     return aFalse;
    _In_lock = _asp_mutex_init();
    if (_In_lock == NULL)
    {
        // free(_In_buffer);
        return aFalse;
    }
    // _In_buffer_first_byte = _In_buffer;
    // _asp_set_buf_limit(_ASP_IN_BUF_LEN, &_In_buffer_filled_to, _In_buffer);
    return aTrue;
}

aBool _asp_In_readStr(aBptr_t _store_in, aSize_t len)
{
    // we can check if the input buffer is empty
    register aBool ret = aTrue;
    _asp_mutex_lock(_In_lock);
    register aSize_t i = 0;
    for (i = 0; i < len - 1; i++)
    {
        if (_asp_read_byte(_store_in) != 1)
        {
            ret = aFalse; // we failed to read a byte
            break;
        }
        if (*_store_in == '\n')
        {
            break;
        }            // the read character, if '\n', exit successfully
        _store_in++; // to the next character
    }
    // we read the input, we don't care about the characters entered after the maximum size of the buffer
    *_store_in = 0; // terminate the string
    _asp_mutex_unlock(_In_lock);
    return ret;
}

aBool _asp_In_readChar(aBptr_t _store_in)
{
    if (_asp_read_byte(_store_in) != 1)
        return aFalse;
    return aTrue;
}

void _asp_InBuf_destroy()
{
    // free(_In_buffer);
    _asp_mutex_destroy(_In_lock);
    // _In_buffer_first_byte = NULL;
}