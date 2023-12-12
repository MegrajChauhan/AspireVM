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

#ifndef ASPIRE_MEMORY_HUB
#define AsPIRE_MEMORY_HUB

/*
 This is not really a "my version" of stdlib.h. This is simply an interface like component to communicate with the OS for memory.
 The hard work of managing memory blocks, their size, numbers etc is an overhead for the VM and the one from the standard library is the best one i think.
*/

#include "../../Utils/aspire_typedefs.h"
#include "../../Utils/aspire_config.h"

// we will have a dedicated separate module that does this for us
#if defined(_ASP_HOST_LINUX)
#include <unistd.h>
#endif

#define _ASP_MEMORY_WANT_ALL 0

// many of the following fields are useless as of now but they will be useful in the future

/**
 * @brief Pointer to the start of the memory pool.
 */
static aBptr_t start_addr = NULL;

/**
 * @brief Pointer to the end of the memory pool.
 */
static aBptr_t end_addr = NULL;

/**
 * @brief Current position in the memory pool.
 */
static aBptr_t current_pos = NULL;

/**
 * @brief Total size of the memory pool.
 */
static aSize_t memory_size;

/**
 * @brief Amount of memory in use (allocated) within the pool.
 */
static aSize_t memory_in_use;

/**
 * @brief Initializes the memory pool with the specified size.
 * @param pool_size The size of the memory pool in bytes.
 * @return aBool Returns `aTrue` if the initialization is successful, `aFalse` otherwise.
 */
aBool _asp_init_pool(aSize_t pool_size);

/**
 * @brief Allocates a specified amount of memory from the pool.
 * @param size The size of the memory block to allocate in bytes.
 * @return aBool Returns `aTrue` if the allocation is successful, `aFalse` otherwise.
 */
aBool _asp_memory_request(aSize_t size);

/**
 * @brief Increases the size of the memory pool by the specified amount.
 * @param _inc_size The size to increase the memory pool by in bytes.
 * @return aBool Returns `aTrue` if the pool size increase is successful, `aFalse` otherwise.
 */
aBool _asp_pool_size_increase(aSize_t _inc_size);

/**
 * @brief Frees a block of memory in the pool.
 * @param size The size of the memory block to free in bytes.
 * @return aBool Returns `aTrue` if the memory is freed successfully, `aFalse` otherwise.
 */
aBool _asp_memory_free(aSize_t size);

/**
 * @brief Gets the total size of the memory pool.
 * @return aSize_t The total size of the memory pool in bytes.
 */
aSize_t _asp_pool_size();

/**
 * @brief Gets the amount of memory currently in use (allocated) within the pool.
 * @return aSize_t The amount of memory in use (allocated) in bytes.
 */
aSize_t _asp_pool_in_use();

/**
 * @brief Frees the memory pool and all associated resources.
 */
void _asp_free_pool();


#endif