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

#ifndef ASPIRE_MEMORY
#define ASPIRE_MEMORY

#include <stdlib.h>
#include <stdio.h>
#include "../../Utils/aspire_typedefs.h"
#include "../../Utils/aspire_config.h"
#include "../../Utils/aspire_helpers.h"
#include "../../lib/thread/aspire_threads.h"

/**
 * @struct _Asp_Mem_page
 * Structure representing a memory page
 */
typedef struct _Asp_Mem_page
{
    aQptr_t addresses; // Pointer to the memory addresses in the page
} _Asp_Mem_page;

typedef enum _Asp_Mem_Err
{
    _ASP_ACCESS_OUT_OF_BOUNDS = 259,
    _ASP_SEG_FAULT,
    _ASP_PAGE_FAULT
} _Asp_Mem_Err;

/**
 * @struct _Asp_Memory
 * Structure representing the memory
 */
typedef struct _Asp_Memory
{
    _Asp_Mem_page **pages; // Array of pointers to memory pages
    aSize_t page_count;    // Number of pages in memory
    _Asp_Mem_Err err;
    _Asp_Mutex *lock; // this is the memory's lock
} _Asp_Memory;

/**
 * @struct _Asp_Memory_Flags
 * Structure representing memory flags for error handling
 */
typedef struct _Asp_Memory_Flags
{
    // when initializing memory or getting a new page, the only cause of error is the internal itself, we do not need flags for them
    aQword flags;
} _Asp_Memory_Flags;

// aBool _asp_memory_set(_Asp_Memory *memory, aAddr_t address, aSize_t length, aQptr_t value);

/*
 * @note For anything calling _asp_memory_copy or _asp_memory_move, the former doesn't care if the addresses overlap and hence unexpected result may be
 * obtained. The data might get corrupt and the program may crash so use it at your own risk, however, the latter handles such overlapping case as much as it can
 */

// aBool _asp_memory_copy(_Asp_Memory *memory, aAddr_t _src, aAddr_t _dest, aSize_t length);

// aBool _asp_memory_move(_Asp_Memory *memory, aAddr_t _src, aAddr_t _dest, aSize_t length);

// void _asp_memory_dump(_Asp_Memory *memory);

// ------

_Asp_Memory* _asp_memory_init(aSize_t number_of_pages);

aBool _asp_memory_get_new_page(_Asp_Memory *memory);

aBool _asp_memory_get_new_pages(_Asp_Memory *memory, aSize_t num_of_pages);

aBool _asp_memory_write(_Asp_Memory *memory, aAddr_t address, aQword value);

aBool _asp_memory_write_chunk(_Asp_Memory *memory, aAddr_t address, aQptr_t data, aSize_t length);

aBool _asp_memory_write_bytes(_Asp_Memory *memory, aAddr_t address, aBptr_t bytes, aSize_t length); // length means the number of bytes

aBool _asp_memory_read(_Asp_Memory *memory, aAddr_t address, aQptr_t _store_in);

aBool _asp_memory_read_chunk(_Asp_Memory *memory, aAddr_t address, aSize_t length, aQptr_t _store_in);

void _asp_free_memory(_Asp_Memory *memory);

// aStr_t _asp_get_mem_err(_Asp_Mem_Err err);

#endif