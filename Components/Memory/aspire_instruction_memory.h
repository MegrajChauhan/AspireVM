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

#ifndef ASPIRE_INSTRUCTION_MEMORY
#define ASPIRE_INSTRUCTION_MEMORY

/*
This memory is there to make instruction reading faster. Even though the main memory doesn't care about endianness it still reads and writes in big endian.
However, the endianness of this memory depends on the system it is running on.
*/

#include "../../Utils/aspire_typedefs.h"
#include "../../Utils/aspire_config.h"
#include "../../Utils/aspire_helpers.h"
#include <stdlib.h>

/*
  Despite the changes, instruction memory will continue using one address for 8 bytes
*/

typedef enum _Asp_Imem_Err
{
    _ASP_IERR_SEG_FAULT = 260,
    _ASP_IERR_PAGE_FAULT,
} _Asp_Imem_Err;

typedef struct _Asp_Inst_Page
{
    aQptr_t instmem;
    // we don't need size because the size is fixed
} _Asp_Inst_Page;

typedef struct _Asp_Inst_Mem
{
    _Asp_Inst_Page **pages; // pointer to the pointer of pages
    aSize_t numofpages;
    _Asp_Imem_Err err;
} _Asp_Inst_Mem;

// we do not need extra bound checking because the manager and the CPU will try their best to not try to access memory that is out of bounds

_Asp_Inst_Mem * _asp_inst_mem_init(aSize_t numofpages);

aBool _asp_inst_mem_get_new_page(_Asp_Inst_Mem *mem);

aBool _asp_inst_mem_get_pages(_Asp_Inst_Mem *mem, aSize_t num_of_pages);

void _asp_inst_mem_free(_Asp_Inst_Mem *mem);

// we need to be able to write to the instruction memory and read from the memory
// read and write cannot be done by the program at all. We do not write just a single instruction at all but in a large chunk

// aBool _asp_inst_mem_load(_Asp_Inst_Mem *mem, aQptr_t inst, aSize_t size);

// we might need to add additional instructions to somewhere in between the memory
aBool _asp_inst_mem_write(_Asp_Inst_Mem *mem, aQptr_t newinst, aSize_t size, aAddr_t addr);

// aBool _asp_inst_mem_write_pages(_Asp_Inst_Mem *mem, aQptr_t newinst, aSize_t size, aAddr_t page, aAddr_t addr);

// aBool _asp_inst_mem_write_pages_bytes(_Asp_Inst_Mem *mem, aBptr_t newinst, aSize_t size, aAddr_t page, aAddr_t addr);

aBool _asp_inst_read(_Asp_Inst_Mem *mem, aAddr_t addr, aQptr_t _store_in);

// aStr_t _asp_get_memory_err(_Asp_Imem_Err err);

#endif