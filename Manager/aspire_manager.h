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

#ifndef _ASP_MANAGER_
#define _ASP_MANAGER_

/*
  The Manager's task is to:
  Manage the CPU
  Fulfill program requests such as IO, multi-threading, Networking etc
  Provide a clean module and API for the CPU to send requests to
*/

#include "../Utils/aspire_config.h"
#include "../Utils/aspire_typedefs.h"
#include "../Utils/aspire_helpers.h"
#include "../lib/thread/aspire_threads.h"
// #include "../Components/Memory/aspire_memory_hub.h" // we need to put up some abstraction for this
#include "../Components/Memory/aspire_memory.h"
#include "../Components/Memory/aspire_instruction_memory.h"

typedef struct _Asp_Manager _Asp_Manager;

#include "aspire_interrupt_handler.h"
#include "../Components/CPU/aspire_cpu.h"

#include "aspire_io.h"
#include "input/aspire_read_input.h"
#include <unistd.h>

struct _Asp_Manager
{
  _Asp_Thread **core_threads; // must be equal to the number of cores
  _Asp_CPU **cpu;             // a list of cores
  _Asp_Memory *memory;        // this memory needs protection from multiple simultaneous accesses
  _Asp_Inst_Mem *inst_mem;    // this doesn't neeed protection from anything
  aSize_t core_count;         // number of cores that are running
  _Asp_Intr_Hdlr *hdlr;       // the shared handler
  _Asp_Mutex *lock;           // the manager's lock
  _Asp_Cond *cond;
  aBool stop_vm;
  int return_val;    // 0 by default but can be changed
};

_Asp_Manager *_asp_manager_init_manager(); // By default, start with 1 core

/*
  The _asp_init_VM is used to initialize the VM before starting execution.
  The function will read from the given input file, read the instructions and the data and ready the VM for execution.
  The _asp_run_vm will be the actual entry point.
*/

aBool _asp_manager_init_VM(_Asp_Manager *manager, acStr_t _file_path);  // starts the VM[should be provided with the input file name but for now we will just take the direct instructions as input]
// aBool _asp_manager_init_VM(_Asp_Manager *manager, aQptr_t instructions, aSize_t ilen, aQptr_t data, aSize_t dlen);

aBool _asp_manager_boot_core(_Asp_Manager *manager, aSize_t core_number, aQword start_executing_from);

void _asp_manager_destroy_manager(_Asp_Manager *manager);

// the functions that the Manager provides for the interrupt handler to access
void _asp_manager_stop_core(_Asp_Manager *manager, aQword core_id); // stop the thread executing this core[The core becomes unavailable]

void *_asp_manager_run_vm(void *manager);

/*
  To be able to use multiple cores, the following steps are required:
  1) Add the core. if successful, go to step 2 else exit
  2) Boot up the core and start execution from a certain memory address.

  This process requires at least two interrupts: first to add a core and then the second to make it start working.

  This process can be, however, done in a single step. The interrupt handler will provide just one interrupt for this.
  The way communication works between the interrupt handler and the cores is predefined.
*/

/*
 Before booting up a core we need to add it. If it is added successfully then, the core can be booted up else the manager will exit with a message.
*/
void _asp_manager_add_core(_Asp_Manager *manager, aQword core_id);

// some error handling interfaces
/*
 Say core 0 encounters divide by zero error. It will generate an interrupt and request attention from the interrupt handler.
 The manager is then requested to handle everything. An error is showed like "Fatal: Divide By Zero" or simply "Divide By Zero" and the manager
 stops other cores as well and we exit
*/
void _asp_manager_error(_Asp_Manager *manager, acStr_t err_msg);

void _asp_manager_exit(_Asp_Manager *manager, aQword core_id);

void _asp_manager_IO_readChar(_Asp_Manager *manager, aQword core_id);

void _asp_manager_IO_readString(_Asp_Manager *manager, aQword core_id);

void _asp_manager_IO_writeChar(_Asp_Manager *manager, aQword core_id);

void _asp_manager_IO_writeString(_Asp_Manager *manager, aQword core_id);

#endif