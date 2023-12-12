#include "../Components/Memory/aspire_instruction_memory.h"

_Asp_Inst_Mem *_asp_inst_mem_init(aSize_t numofpages)
{
    // we try to initialize the instruction memory
    _Asp_Inst_Mem *mem = (_Asp_Inst_Mem *)malloc(sizeof(_Asp_Inst_Mem));
    if (mem == NULL)
        return NULL;
    // now we need to initialize the pages
    mem->numofpages = numofpages;
    // we now need the pages
    mem->pages = (_Asp_Inst_Page **)malloc(sizeof(_Asp_Inst_Page *) * numofpages);
    if (mem->pages == NULL)
    {
        free(mem);
        return NULL;
    }
    if (numofpages == 1 || numofpages == 0)
    {
        // doing this is better than initializing a loop when it is just one
        // get the memory
        mem->pages[0] = (_Asp_Inst_Page *)malloc(sizeof(_Asp_Inst_Page));
        if (mem->pages[0] == NULL)
        {
            _asp_inst_mem_free(mem);
            return NULL;
        }
        // get the array
        mem->pages[0]->instmem = (aQptr_t)malloc(_ASP_INST_PAGE_SIZE);
        if (mem->pages[0]->instmem == NULL)
        {
            _asp_inst_mem_free(mem);
            return NULL;
        }
        goto ret;
    }
    for (aSize_t i = 0; i < numofpages; i++)
    {
        // we do the same as we did with one page but this time we do it for many pages
        mem->pages[i] = (_Asp_Inst_Page *)malloc(sizeof(_Asp_Inst_Page));
        if (mem->pages[i] == NULL)
        {
            _asp_inst_mem_free(mem);
            return NULL;
        }
        // get the array
        mem->pages[i]->instmem = (aQptr_t)malloc(_ASP_INST_PAGE_SIZE);
        if (mem->pages[i]->instmem == NULL)
        {
            _asp_inst_mem_free(mem);
            return NULL;
        }
    }
    goto ret;
ret:
    return mem; // everything was done successfully
}

aBool _asp_inst_mem_get_new_page(_Asp_Inst_Mem *mem)
{
    // we need to get a new page for the memory
    // we assume that the mem is not NULL
    mem->numofpages++;
    mem->pages = (_Asp_Inst_Page **)realloc(mem->pages, mem->numofpages);
    if (mem->pages == NULL)
        return aFalse;
    // now we just need to do the same as _asp_inst_mem_init
    register aSize_t p = mem->numofpages - 1;
    mem->pages[p] = (_Asp_Inst_Page *)malloc(sizeof(_Asp_Inst_Page));
    if (mem->pages[p] == NULL)
        return aFalse;
    // get the array
    mem->pages[p]->instmem = (aQptr_t)malloc(_ASP_INST_PAGE_SIZE);
    if (mem->pages[p]->instmem == NULL)
        return aFalse;
    return aTrue;
}

aBool _asp_inst_mem_get_pages(_Asp_Inst_Mem *mem, aSize_t num_of_pages)
{
    mem->numofpages += num_of_pages;
    mem->pages = (_Asp_Inst_Page **)realloc(mem->pages, mem->numofpages);
    if (mem->pages == NULL)
        return aFalse;
    register aSize_t p;
    for (aSize_t i = num_of_pages; i > 0; i--)
    {
        // now we just need to do the same as _asp_inst_mem_init
        p = mem->numofpages - i;
        mem->pages[p] = (_Asp_Inst_Page *)malloc(sizeof(_Asp_Inst_Page));
        if (mem->pages[p] == NULL)
            return aFalse;
        // get the array
        mem->pages[p]->instmem = (aQptr_t)malloc(_ASP_INST_PAGE_SIZE);
        if (mem->pages[p]->instmem == NULL)
            return aFalse;
    }
    return aTrue;
}

void _asp_inst_mem_free(_Asp_Inst_Mem *mem)
{
    // we will need to free the pages one by one
    if (mem == NULL)
        return;
    if (mem->pages != NULL)
    {
        if (mem->numofpages == 1)
        {
            if (mem->pages[0] != NULL)
            {
                if (mem->pages[0]->instmem != NULL)
                    free(mem->pages[0]->instmem);
                free(mem->pages[0]);
            }
            free(mem->pages);
        }
        else
        {
            for (aSize_t i = 0; i < mem->numofpages; i++)
            {
                if (mem->pages[i] != NULL)
                {
                    if (mem->pages[i]->instmem != NULL)
                        free(mem->pages[i]->instmem);
                    free(mem->pages[i]);
                }
            }
            free(mem->pages);
        }
    }
    free(mem);
}

// aBool _asp_inst_mem_load(_Asp_Inst_Mem *mem, aQptr_t inst, aSize_t size)
// {
//     register aSize_t pages = size / _ASP_INST_PAGE_SIZE; // size here means the number of instructions being loaded
//     register aSize_t tempsize = size;
//     if (mem->numofpages < pages)
//         return aFalse;
//     register aQptr_t temp;
//     for (aSize_t i = 0; i < pages; i++)
//     {
//         temp = mem->pages[i]->instmem;
//         for (aSize_t j = 0; j < _ASP_INST_PAGE_SIZE; j++)
//         {
//             if (j == size)
//                 break;
//             temp[j] = (*inst);
//             inst++;
//         }
//         size -= _ASP_INST_PAGE_SIZE;
//     }
//     return aTrue;
// }

aBool _asp_inst_mem_write(_Asp_Inst_Mem *mem, aQptr_t newinst, aSize_t size, aAddr_t addr)
{
    // we are sent the address that the CPU thinks it is on but we have to calculate the address ourselves
    register aQword offset = addr % _ASP_INST_PAGE_SIZE;   // get the offset
    register aQword page_num = addr / _ASP_INST_PAGE_SIZE; // get the page number
    // the offset won't be larger than needed but we have to check for page num
    if (mem->numofpages <= page_num)
    {
        mem->err = _ASP_IERR_PAGE_FAULT;
        return aFalse;
    }
    if ((addr + size) >= _ASP_INST_PAGE_SIZE)
    {
        mem->err = _ASP_IERR_SEG_FAULT;
        return aFalse;
    }
    register aQptr_t temp;
    temp = mem->pages[page_num]->instmem;
    for (aSize_t i = offset; i < size; i++)
    {
        temp[i] = (*newinst);
        newinst++;
    }
    return aTrue;
}

aBool _asp_inst_read(_Asp_Inst_Mem *mem, aAddr_t addr, aQptr_t _store_in)
{
    register aQword offset = addr % _ASP_INST_PAGE_SIZE;   // get the offset
    register aQword page_num = addr / _ASP_INST_PAGE_SIZE; // get the page number
    // the offset won't be larger than needed but we have to check for page num
    if (mem->numofpages <= page_num)
    {
        mem->err = _ASP_IERR_PAGE_FAULT;
        return aFalse;
    }
    *_store_in = mem->pages[page_num]->instmem[offset];
    return aTrue;
}

// aStr_t _asp_get_memory_err(_Asp_Imem_Err err)
// {
//     switch (err)
//     {
//     case _ASP_IERR_PAGE_FAULT:
//         return _asp_concat(_ASP_ERR_FATAL("Error"), ": The addressed page doesn't exist\033[37m\033[1m[PAGE FAULT]\033[0m.\n");
//     case _ASP_IERR_SEG_FAULT:
//         return _asp_concat(_ASP_ERR_FATAL("Error"), ": The requested memory address is invalid\033[37m\033[1m[SEGMEMTATION FAULT]\033[0m.\n");
//     }
// }
