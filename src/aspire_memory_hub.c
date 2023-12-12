/*
MIT License

Copyright (c) 2023 MegrajChauhan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "../Components/Memory/aspire_memory_hub.h"

aBool _asp_init_pool(aSize_t pool_size)
{
    start_addr = sbrk(0);
    if(sbrk(pool_size) == (void*)-1) return aFalse; // currently we only assume that we are on a Unix-based system, we need a different method
    end_addr = sbrk(0);
    current_pos = start_addr;
    memory_in_use = 0;
    memory_size = pool_size;
    return aTrue;
}

aBool _asp_memory_request(aSize_t size)
{
    // we say, if the requested size is 0, we want all memory
    if (size == _ASP_MEMORY_WANT_ALL) size = memory_size;
    else if(memory_size - memory_in_use < size) return aFalse;
    current_pos += size;
    memory_in_use += size;
    return aTrue;
}

aBool _asp_pool_size_increase(aSize_t _inc_size)
{
    if(memory_size - memory_in_use >= _inc_size) return aTrue; // no need to increase if already available
    if(sbrk(_inc_size) == (void*)-1) return aFalse;
    end_addr = sbrk(0);
    memory_size += _inc_size;
    return aTrue;
}

aBool _asp_memory_free(aSize_t size)
{
    if(memory_size < size || memory_in_use < size) return aFalse;
    memory_in_use -= size;
    current_pos -= size;
    return aTrue;
}

aSize_t _asp_pool_size()
{
  return memory_size;
}

aSize_t _asp_pool_in_use()
{
    return memory_in_use;
}

void _asp_free_pool()
{
    start_addr = NULL;
    end_addr = NULL;
    current_pos = NULL;
    memory_in_use = 0;
    memory_size = 0;
}