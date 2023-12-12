#include "../../Manager/aspire_interrupt_handler.h"

_Asp_Intr_Hdlr *_asp_init_intr_hdlr(_Asp_Manager *manager)
{
    if (manager == NULL)
        return NULL;
    _Asp_Intr_Hdlr *hdlr = (_Asp_Intr_Hdlr *)malloc(sizeof(_Asp_Intr_Hdlr));
    if (hdlr == NULL)
        return NULL;
    hdlr->manager = manager;
    hdlr->lock = _asp_mutex_init();
    if (hdlr->lock == NULL)
    {
        _asp_destroy_intr_hdlr(hdlr);
        return NULL;
    }
    // the interrupt handler was initialized
    return hdlr;
}

void _asp_handle_interrupt(_Asp_Intr_Hdlr *hdlr, aQword interrupt_number, aQword core_id)
{
    _asp_mutex_lock(hdlr->lock);
    switch (interrupt_number)
    {
    case _INTR_STOP_CPU:
        _asp_manager_stop_core(hdlr->manager, core_id);
        break; // that core stops execution
    case _INTR_DIVIDE_BY_ZERO:
        _asp_manager_error(hdlr->manager, "Divide by zero");
        break;
    case _INTR_ADD_NEW_CORE:
        _asp_manager_add_core(hdlr->manager, core_id);
        break; // may stop the VM or not
    case _INTR_MEMORY_ACCESS_OUT_OF_BOUNDS:
        _asp_manager_error(hdlr->manager, "Accessing memory that is out of bounds.");
        break;
    case _INTR_MEMORY_SEG_FAULT:
        _asp_manager_error(hdlr->manager, "Segmentation fault: Memory offset is out the page's range!");
        break;
    case _INTR_MEMORY_PAGE_FAULT:
        _asp_manager_error(hdlr->manager, "Page fault: Accessing page that doesn't exists!");
        break;
    case _INTR_STACK_OVERFLOW:
        _asp_manager_error(hdlr->manager, "Stack error: Stack overflow");
        break;
    case _INTR_STACK_UNDERFLOW:
        _asp_manager_error(hdlr->manager, "Stack error: Not enough data on stack to pop");
        break;
    case _INTR_STACK_INVALID_ACCESS:
        _asp_manager_error(hdlr->manager, "Stack error: Accessing stack memory of a different function.");
        break;
    case _INTR_STACK_INVALID_ADDRESSING:
        _asp_manager_error(hdlr->manager, "Stack error: Base pointer Error(SBA).");
        break;
    case _INTR_EXIT:
        _asp_manager_exit(hdlr->manager, core_id);
        break;
    }
    _asp_mutex_unlock(hdlr->lock);
}

void _asp_destroy_intr_hdlr(_Asp_Intr_Hdlr *hdlr)
{
    if (hdlr == NULL)
        return;
    if (hdlr->lock != NULL)
        _asp_mutex_destroy(hdlr->lock);
    // the manager frees itself
    hdlr->manager = NULL;
    free(hdlr);
}