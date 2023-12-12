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

#ifndef _ASP_INTR_HDLR_
#define _ASP_INTR_HDLR_

/*
 This module is supposed to attach to the CPU cores and convey their messages to the Manager
*/

#include "../Utils/aspire_typedefs.h"
#include "../Utils/aspire_helpers.h"
#include "aspire_interrupts.h"

typedef struct _Asp_Intr_Hdlr _Asp_Intr_Hdlr;

#include "aspire_manager.h"

struct _Asp_Intr_Hdlr
{
    _Asp_Mutex *lock;      // the handler's lock
    _Asp_Manager *manager; // the manager
};

_Asp_Intr_Hdlr *_asp_init_intr_hdlr(_Asp_Manager *manager);

void _asp_handle_interrupt(_Asp_Intr_Hdlr *hdlr, aQword interrupt_number, aQword core_id);

void _asp_destroy_intr_hdlr(_Asp_Intr_Hdlr *hdlr);

#endif
