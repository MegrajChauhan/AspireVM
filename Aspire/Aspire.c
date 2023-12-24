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

#include <stdio.h>
#include "../Manager/aspire_manager.h"
#include <syscall.h>

int main(int argc, char **argv)
{
    // halt the cpu
    // aQword program[PROGRAM_LEN] = {
    //     0x00, 0x00, 0x00, 0x00, 0x00,
    //     0x00, 0x00, 0x00, 0x00, 0b0000000100000000000000000000000000000000000000000000000000000000}; // the program

    // // test multi-threading
    // aQword program2[100] = {
    //     0x00, 0x00, 0x00, 0x00, 0x00,
    //     0b00000010,
    //     0x00, 0x00, 0x00, 0x00, 0b0000000100000000000000000000000000000000000000000000000000000000
    // };
    _Asp_Manager *manager = _asp_manager_init_manager();
    if (manager == NULL)
    {
        printf("Couldn't start the manager.\n");
        return 0;
    }
    // printf("Initialized\n");
    if (_asp_manager_init_VM(manager, "program.txt") == aFalse)
    {
        printf("Couldn't initialize VM\n");
        _asp_manager_destroy_manager(manager);
        return 0;
    }
    _asp_manager_destroy_manager(manager);
    return 0;
    // _Asp_Thread *manager_thread = _asp_thread_init();
    // if (manager_thread == NULL)
    // {
    //     printf("Couldn't start execution\n");
    //     _asp_manager_destroy_manager(manager);
    //     return 0;
    // }
    // if (_asp_create_thread(manager_thread, _asp_manager_run_vm, manager) == aFalse)
    // {
    //     printf("Couldn't start the VM\n");
    //     _asp_manager_destroy_manager(manager);
    //     _asp_thread_destroy(manager_thread);
    //     return 0;
    // }
    // aSize_t ret = 0;
    // _asp_thread_join(manager_thread, &ret);
    // _asp_thread_destroy(manager_thread);
    // _asp_manager_destroy_manager(manager);
    // return ret;
}