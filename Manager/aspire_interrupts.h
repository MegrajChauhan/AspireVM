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

#ifndef _ASP_INTERRUPTS_
#define _ASP_INTERRUPTS_

enum
{
    // some interrupts for the user program to use
    _INTR_EXIT, // shutdown the VM without any errors
    _INTR_ADD_NEW_CORE,
    _INTR_READ_NUM, // these IO interrupts are temporary and might change in the future
    _INTR_READ_CHAR,
    _INTR_READ_STR,

    _INTR_STOP_CPU = 256, // this interrupt is sent by the host core which has encountered the halt instruction[shutsdown the core]
    _INTR_DIVIDE_BY_ZERO,
    _INTR_RES,

    // Interrupts for memory related errors
    _INTR_MEMORY_ACCESS_OUT_OF_BOUNDS, // this is pretty much unneeded right now
    _INTR_MEMORY_SEG_FAULT,
    _INTR_MEMORY_PAGE_FAULT,

    // stack related errors
    _INTR_STACK_OVERFLOW,
    _INTR_STACK_UNDERFLOW,
    _INTR_STACK_INVALID_ACCESS,
    _INTR_STACK_INVALID_ADDRESSING,
};

#endif