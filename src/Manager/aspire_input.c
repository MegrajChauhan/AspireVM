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
    register aBool encountered = aFalse;
    _asp_mutex_lock(_In_lock);
    register aSize_t i = 0;
    // len is the length of the string along with the terminating byte 0
    for (i = 0; i < len - 1; i++)
    {
        if (_asp_read_byte(_store_in) != 1)
        {
            ret = aFalse; // we failed to read a byte
            break;
        }
        if (*_store_in == '\n')
        {
            encountered = aTrue;
            break;
        }            // the read character, if '\n', exit successfully
        _store_in++; // to the next character
    }
    // we read the input, we don't care about the characters entered after the maximum size of the buffer
    *_store_in = 0; // terminate the string
    if (encountered == aFalse)
    {
        // discard all of the remaining characters
        register aByte temp = 0;
        while (temp != '\n')
        {
            if (_asp_read_byte(&temp) != 1)
            {
                ret = aFalse; // we successfully read the bytes for the string not for temp
                break;
            }
        }
    }
    _asp_mutex_unlock(_In_lock);
    return ret;
}

aBool _asp_In_readChar(aBptr_t _store_in)
{
    register aBool ret = aTrue;
    _asp_mutex_lock(_In_lock);
    if (_asp_read_byte(_store_in) != 1)
        ret = aFalse;
    // the character entered can be a newline which will be stored in the memory but until a new line is reached other inputs are not accepted
    if ( ret == aTrue && *_store_in != '\n')
    {
        register aByte temp = 0;
        while (temp != '\n')
        {
            if (_asp_read_byte(&temp) != 1)
            {
                ret = aFalse; // we successfully read the bytes for the character not for temp
                break;
            }
        }
    }
    _asp_mutex_unlock(_In_lock);
    return ret;
}

void _asp_InBuf_destroy()
{
    // free(_In_buffer);
    _asp_mutex_destroy(_In_lock);
    // _In_buffer_first_byte = NULL;
}