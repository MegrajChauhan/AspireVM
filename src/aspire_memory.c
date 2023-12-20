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

#include "../Components/Memory/aspire_memory.h"

// moving back to qwords again

// initialize the memory manager
_Asp_Memory *_asp_memory_init(aSize_t number_of_pages)
{
    // allocate the memory
    _Asp_Memory *memory = (_Asp_Memory *)malloc(sizeof(_Asp_Memory));

    if (memory == NULL)       // was allocation successfull?
        return NULL;          // it was not
    if (number_of_pages == 0) // if the number of pages is given 0, then we make it 1.
        (memory)->page_count = 1;
    else // else we make it so
        (memory)->page_count = number_of_pages;

    // allocate the array of pointers for the pages
    (memory)->pages = (_Asp_Mem_page **)malloc(sizeof(_Asp_Mem_page *) * (memory)->page_count);
    if ((memory)->pages == NULL) // was it successfull?
        goto failure;            // it was not :(

    // if the number of pages is 1 then instead of wasting the time on initializing the loops, we will do it seperately once
    if ((memory)->page_count == 1)
    {
        // allocate the page
        (memory)->pages[0] = (_Asp_Mem_page *)malloc(sizeof(_Asp_Mem_page));

        if ((memory)->pages[0] == NULL) // was the page allocated?
            goto failure;               // it was not :(

        // now allocate the bytes that the page will hold on to
        // we are allocating all of the bytes directly instead of using sbrk or similar functionality
        // to avoid cases when the OS might allocate the memory that should belong to the page to something else
        (memory)->pages[0]->addresses = (aQptr_t)malloc(_ASP_PAGE_SIZE);
        if ((memory)->pages[0]->addresses == NULL) // was allocation successfull?
            goto failure;                          // it was not :(

        // everything was successful
        return memory;
    }

    // we have a lot of pages to allocate
    for (aSize_t i = 0; i < number_of_pages; i++)
    {
        // same as for 1 page
        (memory)->pages[i] = (_Asp_Mem_page *)malloc(sizeof(_Asp_Mem_page));
        if ((memory)->pages[i] == NULL)
            goto failure;
        (memory)->pages[i]->addresses = (aQptr_t)malloc(_ASP_PAGE_SIZE);
        if ((memory)->pages[i]->addresses == NULL)
            goto failure;
    }
    return memory; // all good :)
failure:
    _asp_free_memory(memory);
    return NULL;
}

aBool _asp_memory_get_new_page(_Asp_Memory *memory)
{
    if (memory == NULL)
        return aFalse;

    // increase the page count
    memory->page_count++;
    // try increasing the size of the pages
    memory->pages = (_Asp_Mem_page **)realloc(memory->pages, sizeof(_Asp_Mem_page *) * memory->page_count);
    if (memory->pages == NULL)
        return aFalse; // we could not get another page

    // allocate the page
    register aSize_t ind = memory->page_count - 1;
    memory->pages[ind] = (_Asp_Mem_page *)malloc(sizeof(_Asp_Mem_page));

    if (memory->pages[ind] == NULL) // was the page allocated?
        return aFalse;              // it was not :(

    // now allocate the bytes that the page will hold on to
    memory->pages[ind]->addresses = (aQptr_t)malloc(_ASP_PAGE_SIZE);
    if (memory->pages[ind]->addresses == NULL) // was allocation successfull?
        return aFalse;                         // it was not :(

    // everything was successful
    return aTrue;
}

aBool _asp_memory_get_new_pages(_Asp_Memory *memory, aSize_t num_of_pages)
{
    if (memory == NULL)
        return aFalse;

    // increase the page count
    memory->page_count += num_of_pages;
    // try increasing the size of the pages
    memory->pages = (_Asp_Mem_page **)realloc(memory->pages, sizeof(_Asp_Mem_page *) * memory->page_count);
    if (memory->pages == NULL)
        return aFalse; // we could not get another page

    register aSize_t ind;
    // allocate the pages
    for (aSize_t i = num_of_pages; i > 0; i--)
    {
        ind = memory->page_count - i;
        memory->pages[ind] = (_Asp_Mem_page *)malloc(sizeof(_Asp_Mem_page));

        if (memory->pages[ind] == NULL) // was the page allocated?
            return aFalse;              // it was not :(

        // now allocate the bytes that the page will hold on to
        memory->pages[ind]->addresses = (aQptr_t)malloc(_ASP_PAGE_SIZE);
        if (memory->pages[ind]->addresses == NULL) // was allocation successfull?
            return aFalse;                         // it was not :(
    }
    // everything was successful
    return aTrue;
}

// write value to address in the memory
aBool _asp_memory_write(_Asp_Memory *memory, aAddr_t address, aQword value)
{
    // we are assuming that memory is not NULL
    // get the page number
    register aQword offset = address % _ASP_PAGE_SIZE;   // get the offset
    register aQword page_num = address / _ASP_PAGE_SIZE; // get the page number
    // is the page number being addressed even valid?
    if (page_num >= memory->page_count)
    {
        memory->err = _ASP_PAGE_FAULT; // oh.. Then it is a page fault
        return aFalse;
    }
    if (!_ASP_CHECK_BOUNDS(offset))
    {
        memory->err = _ASP_SEG_FAULT;
        return aTrue;
    }
    _asp_mutex_lock(memory->lock);
    memory->pages[page_num]->addresses[offset] = value;
    _asp_mutex_unlock(memory->lock);
    return aTrue;
}

// write all of the values pointed to by data one by one to address in the memory
aBool _asp_memory_write_chunk(_Asp_Memory *memory, aAddr_t address, aQptr_t data, aSize_t length)
{
    // we are assuming that memory is not NULL
    // get the page number
    register aQword offset = address % _ASP_PAGE_SIZE;   // get the offset
    register aQword page_num = address / _ASP_PAGE_SIZE; // get the page number
    // is the page number being addressed even valid?
    if (page_num >= memory->page_count)
    {
        memory->err = _ASP_PAGE_FAULT; // oh.. Then it is a page fault
        return aFalse;
    }
    register aBool _is_multi_page = !_ASP_CHECK_BOUNDS(offset + length) ? aTrue : aFalse;
    _asp_mutex_lock(memory->lock);
    register aQptr_t taddress = &(memory->pages[page_num]->addresses[offset]);
    // writing all of the data
    if (_is_multi_page == aFalse)
    {
        // the write is being requested in only one page
        for (aSize_t i = 0; i < length; i++)
        {
            *taddress = data[i];
            taddress++;
        }
    }
    else
    {
        // the write will write to multiple pages
        // first we need to confirm that there is a next page
        // we don't need to confirm if the address is in range
        // we need to keep track of how many pages are being written here
        // in future, for optimization, we might change how we do this
        register aSize_t _pgae_write_count = length / _ASP_PAGE_SIZE;    // this gives the number of page
        register aSize_t _remaining_addresses = length % _ASP_PAGE_SIZE; // the remaining addresses
        if (memory->page_count <= (page_num + _pgae_write_count + _remaining_addresses > 0 ? 1 : 0))
        {
            // this is a page fault
            memory->err = _ASP_PAGE_FAULT;
            _asp_mutex_unlock(memory->lock);
            return aFalse;
        }
        for (aSize_t i = 0; i < _pgae_write_count; i++)
        {
            for (aSize_t j = 0; j < _ASP_PAGE_SIZE; j++)
            {
                *taddress = *data;
                taddress++;
                data++;
            }
            taddress = &(memory->pages[(page_num++)]->addresses); // goto the next page
        }
        if (_remaining_addresses > 0)
        {
            // taddress is pointing to this new page
            for (aSize_t i = 0; i < _remaining_addresses; i++)
            {
                *taddress = *data;
                taddress++;
                data++;
            }
        }
    }
    _asp_mutex_unlock(memory->lock);
    return aTrue;
}

aBool _asp_memory_write_bytes(_Asp_Memory *memory, aAddr_t address, aBptr_t bytes, aSize_t length)
{
    // we will leave the writing to _asp_memory_write for writing, we just have to group bytes into Qwords
    register aSize_t _num_of_q = length / 8;
    register aSize_t _rem_q = length % 8;
    register aSize_t _len = _num_of_q + (_rem_q > 0) ? 1 : 0;
    register aQword _qs[_len]; // the grouped q's
    for (aSize_t i = 0; i < _num_of_q; i++)
    {
        _qs[i] = (_qs[i] << 56) & *bytes++;
        _qs[i] = (_qs[i] << 48) & *bytes++;
        _qs[i] = (_qs[i] << 40) & *bytes++;
        _qs[i] = (_qs[i] << 32) & *bytes++;
        _qs[i] = (_qs[i] << 24) & *bytes++;
        _qs[i] = (_qs[i] << 16) & *bytes++;
        _qs[i] = (_qs[i]) & *bytes++;
    }
    if (_rem_q > 0)
    {
        // there is this extra q
        for (aSize_t i = 0; i < _rem_q; i++)
        {
            
        }
    }
}

aBool _asp_memory_read(_Asp_Memory *memory, aAddr_t address, aQptr_t _store_in)
{
    // we are assuming that memory is not NULL
    // get the page number
    register aQword offset = address % _ASP_PAGE_SIZE;   // get the offset
    register aQword page_num = address / _ASP_PAGE_SIZE; // get the page number
    // is the page number being addressed even valid?
    if (page_num >= memory->page_count)
    {
        memory->err = _ASP_PAGE_FAULT; // oh.. Then it is a page fault
        return aFalse;
    }
    if (!_ASP_CHECK_BOUNDS(offset))
    {
        memory->err = _ASP_SEG_FAULT; // NO! A seg fault!
        return aFalse;
    }
    _asp_mutex_lock(memory->lock);
    *_store_in = memory->pages[page_num]->addresses[offset];
    _asp_mutex_unlock(memory->lock);
    return aTrue;
}

aBool _asp_memory_read_chunk(_Asp_Memory *memory, aAddr_t address, aSize_t length, aQptr_t _store_in)
{
    // we are assuming that memory is not NULL
    // get the page number
    register aQword offset = address % _ASP_PAGE_SIZE;   // get the offset
    register aQword page_num = address / _ASP_PAGE_SIZE; // get the page number
    // is the page number being addressed even valid?
    if (page_num >= memory->page_count)
    {
        memory->err = _ASP_PAGE_FAULT; // oh.. Then it is a page fault
        return aFalse;
    }
    register aBool _is_multi_page = !_ASP_CHECK_BOUNDS(offset + length);
    _asp_mutex_lock(memory->lock);
    register aQptr_t data = &(memory->pages[page_num]->addresses[offset]);
    if (_is_multi_page == aFalse)
    {
        // the read is not multipage
        for (aSize_t i = 0; i < length; i++)
        {
            _store_in[i] = data;
            data++;
        }
    }
    else
    {
        register aSize_t _pgae_write_count = length / _ASP_PAGE_SIZE;    // this gives the number of page
        register aSize_t _remaining_addresses = length % _ASP_PAGE_SIZE; // the remaining addresses
        if (memory->page_count <= (page_num + _pgae_write_count + _remaining_addresses > 0 ? 1 : 0))
        {
            // this is a page fault
            memory->err = _ASP_PAGE_FAULT;
            _asp_mutex_unlock(memory->lock);
            return aFalse;
        }
        for (aSize_t i = 0; i < _pgae_write_count; i++)
        {
            for (aSize_t j = 0; j < _ASP_PAGE_SIZE; j++)
            {
                *_store_in = *data;
                _store_in++;
                data++;
            }
            data = &(memory->pages[(page_num++)]->addresses); // goto the next page
        }
        if (_remaining_addresses > 0)
        {
            // data is pointing to this new page
            for (aSize_t i = 0; i < _remaining_addresses; i++)
            {
                *_store_in = *data;
                _store_in++;
                data++;
            }
        }
    }
    _asp_mutex_unlock(memory->lock);
    return aTrue;
}

// we now have to free all of the memory allocated
void _asp_free_memory(_Asp_Memory *memory)
{
    // first we free all of the pages that were allocated
    // we check if the page count is just 1 in which case, we avoid loop initialization
    if (memory == NULL)
        return;
    if (memory->pages != NULL)
    {
        for (aSize_t i = 0; i < memory->page_count; i++)
        {
            // free the bytes that was allocated
            if (memory->pages[i] != NULL)
            {
                if (memory->pages[i]->addresses != NULL)
                    free(memory->pages[i]->addresses);
                // point to nothing
                // memory->pages[i]->bottom_addr = NULL;
                // memory->pages[i]->top_addr = NULL;
                // free the page
                free(memory->pages[i]);
            }
            // free the pages pointer
        }
        if (memory->lock != NULL)
            _asp_mutex_destroy(memory->lock);
        free(memory->pages);
    }
    free(memory);
}

// aBool _asp_memory_set(_Asp_Memory *memory, aAddr_t address, aSize_t length, aByte value)
// {
//     // get the page number
//     register aQword offset = address % _ASP_PAGE_SIZE;   // get the offset
//     register aQword page_num = address / _ASP_PAGE_SIZE; // get the page number
//     // is the page number being addressed even valid?
//     if (page_num >= memory->page_count)
//     {
//         memory->err = _ASP_PAGE_FAULT; // oh.. Then it is a page fault
//         return aFalse;
//     }
//     // get the address
//     if (!_ASP_CHECK_BOUNDS(offset + length * 8)) // length is the number of addresses to set
//     {
//         memory->err = _ASP_SEG_FAULT; // NO! A seg fault!
//         return aFalse;
//     }
//     _asp_mutex_lock(memory->lock); // this would be absurd and an overhead if there were no other cores to access this memory but we still have to do it
//     if (length == 1)
//         memory->pages[page_num]->addresses[offset] = value;
//     else
//     {
//         register aQptr_t point = &(memory->pages[page_num]->addresses[offset]);
//         for (aSize_t i = 0; i < length; i++)
//         {
//             // *(point + i) = value;
//             *(point) = value;
//             point++;
//         }
//     }
//     _asp_mutex_unlock(memory->lock);
//     return aTrue;
// }

// aBool _asp_memory_copy(_Asp_Memory *memory, aAddr_t _src, aAddr_t _dest, aSize_t length)
// {
//     // we need details of the source and the destination addresses
//     register aQword src_page = _src / _ASP_PAGE_SIZE;   // get the page number
//     register aQword dest_page = _dest / _ASP_PAGE_SIZE; // get the page number
//     if ((src_page >= memory->page_count) || (dest_page >= memory->page_count))
//     {
//         memory->err = _ASP_PAGE_FAULT;
//         return aFalse;
//     }
//     register aQword src_addr = _src % _ASP_PAGE_SIZE;   // get the offset
//     register aQword dest_addr = _dest % _ASP_PAGE_SIZE; // get the offset
//     if (!_ASP_CHECK_BOUNDS(src_addr + length * 8) || !_ASP_CHECK_BOUNDS(dest_addr + length* 8))
//     {
//         memory->err = _ASP_SEG_FAULT;
//         return aFalse;
//     }
//     _asp_mutex_lock(memory->lock);
//     register aQptr_t dest_point = &(memory->pages[dest_page]->addresses[dest_addr]);
//     register aQptr_t src_point = &(memory->pages[src_page]->addresses[src_addr]);
//     for (aSize_t i = 0; i < length; i++)
//     {
//         *(dest_point) = *(src_point);
//         dest_point++;
//         src_point++;
//     }
//     _asp_mutex_unlock(memory->lock);
//     return aTrue;
// }

// aBool _asp_memory_move(_Asp_Memory *memory, aAddr_t _src, aAddr_t _dest, aSize_t length)
// {
//     // we need details of the source and the destination addresses
//     register aQword src_page = _src / _ASP_PAGE_SIZE;   // get the page number
//     register aQword dest_page = _dest / _ASP_PAGE_SIZE; // get the page number
//     if ((src_page >= memory->page_count) || (dest_page >= memory->page_count))
//     {
//         memory->err = _ASP_PAGE_FAULT;
//         return aFalse;
//     }
//     register aQword src_addr = _src % _ASP_PAGE_SIZE;   // get the offset
//     register aQword dest_addr = _dest % _ASP_PAGE_SIZE; // get the offset
//     if (!_ASP_CHECK_BOUNDS(src_addr + length * 8) || !_ASP_CHECK_BOUNDS(dest_addr + length * 8))
//     {
//         memory->err = _ASP_SEG_FAULT;
//         return aFalse;
//     }
//     // check if we are in different pages
//     _asp_mutex_lock(memory->lock);
//     if (src_page != dest_page)
//     {
//         // we are in different pages so now we copy normally
//         register aQptr_t dest_point = &(memory->pages[dest_page]->addresses[dest_addr]);
//         register aQptr_t src_point = &(memory->pages[src_page]->addresses[src_addr]);
//         for (aSize_t i = 0; i < length; i++)
//         {
//             *(dest_point) = *(src_point);
//             dest_point++;
//             src_point++;
//         }
//         _asp_mutex_unlock(memory->lock);
//     }
//     else
//     {
//         if (src_addr == dest_addr)
//         {
//             _asp_mutex_unlock(memory->lock);
//             return aTrue;
//         }
//         // we are in the same page
//         if (src_addr < dest_addr)
//         {
//             // the src comes before the destination
//             if ((src_addr + length) >= dest_addr)
//             {
//                 // in this case, we have to copy in opposite order
//                 register aQptr_t dtempp = &(memory->pages[dest_page]->addresses[dest_addr]);
//                 register aQptr_t stempp = &(memory->pages[src_page]->addresses[src_addr]);
//                 for (aSize_t i = length - 1; i != 0; i--)
//                 {
//                     (*dtempp) = *(stempp);
//                     dtempp++;
//                     stempp++;
//                 }
//                 (*dtempp) = *(stempp);
//                 _asp_mutex_unlock(memory->lock);
//             }
//         }
//         else
//         {
//             // we do not have to worry about them overlapping in this case and we can copy normally
//             register aQptr_t dest_point = &(memory->pages[dest_page]->addresses[dest_addr]);
//             register aQptr_t src_point = &(memory->pages[src_page]->addresses[src_addr]);
//             for (aSize_t i = 0; i < length; i++)
//             {
//                 (*dest_point) = *(src_point);
//                 dest_point++;
//                 src_point++;
//             }
//             _asp_mutex_unlock(memory->lock);
//         }
//     }
//     return aTrue;
// }

// aStr_t _asp_get_mem_err(_Asp_Mem_Err err)
// {
//     switch (err)
//     {
//     case _ASP_PAGE_FAULT:
//         return _asp_concat(_ASP_ERR_FATAL("Error"), ": The addressed page doesn't exist\033[37m\033[1m[PAGE FAULT]\033[0m.\n");
//     case _ASP_SEG_FAULT:
//         return _asp_concat(_ASP_ERR_FATAL("Error"), ": The requested address is invalid\033[37m\033[1m[SEGMENTATION FAULT]\033[0m.\n");
//     case _ASP_ACCESS_OUT_OF_BOUNDS:
//         return _asp_concat(_ASP_ERR_FATAL("Error"), ": The requested address doesn't exists\033[37m\033[1m[OUT OF BOUNDS]\033[0m.\n");
//     }
// }

// void _asp_memory_dump(_Asp_Memory *memory)
// {
//     FILE *file = fopen("aspmemcore", "w");
//     if (file == NULL)
//         return;
//     for (aSize_t i = 1; i <= memory->page_count; i++)
//     {
//         for (aAddr_t j = 1; j <= _ASP_PAGE_SIZE; j++)
//         {
//             fprintf(file, "%02X ", memory->pages[i - 1]->addresses[j - 1]);
//             if (j != 0 && j % 1024 == 0)
//                 fprintf(file, "\n");
//             if (j != 0 && j % 8 == 0)
//                 fprintf(file, "\t");
//         }
//     }
//     fclose(file);
// }
