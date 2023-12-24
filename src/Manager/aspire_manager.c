#include "../../Manager/aspire_manager.h"

_Asp_Manager *_asp_manager_init_manager()
{
    // this just initializes the Manager
    // it doesn't even start the cores
    _Asp_Manager *manager = (_Asp_Manager *)malloc(sizeof(_Asp_Manager));
    if (manager == NULL)
        return NULL;
    manager->core_count = 1;
    manager->stop_vm = aFalse;
    manager->cpu = (_Asp_CPU **)malloc(sizeof(_Asp_CPU *));
    if (manager->cpu == NULL)
    {
        free(manager);
        return NULL;
    }
    manager->hdlr = _asp_init_intr_hdlr(manager);
    if (manager->hdlr == NULL)
    {
        _asp_manager_destroy_manager(manager);
        return NULL;
    }
    manager->lock = _asp_mutex_init();
    if (manager->lock == NULL)
    {
        _asp_manager_destroy_manager(manager);
        return NULL;
    }
    manager->cond = _asp_cond_init();
    if (manager->cond == NULL)
    {
        _asp_manager_destroy_manager(manager);
        return NULL;
    }
    manager->core_threads = (_Asp_Thread **)malloc(sizeof(_Asp_Thread *));
    if (manager->core_threads == NULL)
    {
        _asp_manager_destroy_manager(manager);
        return NULL;
    }
    if (_asp_InBuf_init() == aFalse)
    {
        _asp_manager_destroy_manager(manager);
        return NULL;
    }
    return manager;
}

void _asp_manager_destroy_manager(_Asp_Manager *manager)
{
    if (manager == NULL)
        return;
    _asp_inst_mem_free(manager->inst_mem);
    _asp_free_memory(manager->memory);
    if (manager->cpu != NULL && manager->core_count > 0)
    {
        for (aSize_t i = 0; i < manager->core_count; i++)
        {
            _asp_stop_cpu(manager->cpu[i]);
        }
        free(manager->cpu);
    }
    _asp_mutex_destroy(manager->lock);
    _asp_cond_destroy(manager->cond);
    _asp_destroy_intr_hdlr(manager->hdlr);
    if (manager->core_threads != NULL && manager->core_count > 0)
    {
        for (aSize_t i = 0; i < manager->core_count; i++)
        {
            _asp_thread_destroy(manager->core_threads[i]);
        }
        free(manager->core_threads);
    }
    free(manager);
}

aBool _asp_manager_boot_core(_Asp_Manager *manager, aSize_t core_number, aQword start_executing_from)
{
    if (manager == NULL)
        return aFalse;
    _asp_mutex_lock(manager->lock);
    if (manager->core_count <= core_number)
        goto failed;
    manager->cpu[core_number] = _asp_start_cpu(manager->memory, manager->inst_mem, manager->hdlr);
    if (manager->cpu[core_number] == NULL)
        goto failed;
    manager->cpu[core_number]->pc = start_executing_from;
    manager->cpu[core_number]->running = aTrue;
    manager->cpu[core_number]->should_wait = aFalse;
    manager->cpu[core_number]->sp = 0;
    manager->cpu[core_number]->cid = manager->core_count - 1; // we need to ADD a core before we can boot it up
    manager->core_threads[core_number] = _asp_thread_init();
    if (manager->core_threads[core_number] == NULL)
    {
        free(manager->cpu[core_number]);
        manager->cpu[core_number] = NULL;
        goto failed;
    }
    _asp_mutex_unlock(manager->lock);
    return aTrue;
failed:
    _asp_mutex_unlock(manager->lock);
    return aFalse;
}

aBool _asp_manager_init_VM(_Asp_Manager *manager, acStr_t _file_path)
{
    if (manager == NULL)
        return aFalse;
    _Asp_Lexer *lexer = _asp_read_file(_file_path);
    if (lexer == NULL)
    {
        fprintf(stderr, "Reading failed.\n"); // additional error message
        return aFalse;
    }
    // we also have to figure out how many pages of memory we need
    manager->inst_mem = _asp_inst_mem_init(lexer->_inst_size / _ASP_PAGE_SIZE + ((lexer->_inst_size % _ASP_PAGE_SIZE) > 0 ? 1 : 0));
    if (manager->inst_mem == NULL)
        goto failure;
    if (_asp_inst_mem_write(manager->inst_mem, lexer->instrs, lexer->_inst_size, 0) == aFalse)
        goto failure;
    if (lexer->data != NULL)
    {
        manager->memory = _asp_memory_init((lexer->_data_size / _ASP_PAGE_SIZE) + ((lexer->_data_size % _ASP_PAGE_SIZE) > 0 ? 1 : 0));
        if (manager->memory == NULL)
            goto failure;
        if (_asp_memory_write_chunk(manager->memory, 0, lexer->data, lexer->_data_size) == aFalse)
            goto failure;
    }
    else
    {
        manager->memory = _asp_memory_init(1);
        if (manager->memory == NULL)
            goto failure;
    }
    if (_asp_manager_boot_core(manager, 0, 0) == aFalse)
        goto failure;
    if (lexer->data != NULL)
        free(lexer->data);
    free(lexer->instrs);
    free(lexer);
    return aTrue;
failure:
    if (lexer->data != NULL)
        free(lexer->data);
    free(lexer->instrs);
    free(lexer);
    return aFalse;
}

void _asp_manager_stop_core(_Asp_Manager *manager, aQword core_id)
{
    // the core itself puts in the id that it was assigned
    // i don't think that we need to check if the core_id is valid
    _asp_mutex_lock(manager->lock);          // access the manager
    manager->cpu[core_id]->running = aFalse; // tell it to stop execution
    manager->core_count--;
    manager->cpu[core_id] = NULL;
    _asp_thread_destroy(manager->core_threads[core_id]);
    manager->core_threads[core_id] = NULL;
    // if we have no more cores executing anything, we will just tell the manager to stop everything
    if (manager->core_count == 0)
    {
        manager->stop_vm = aTrue;
        _asp_cond_signal(manager->cond);
    }
    _asp_mutex_unlock(manager->lock);
}

void *_asp_manager_run_vm(void *manager)
{
    _Asp_Manager *boss = (_Asp_Manager *)manager;
    // the _asp_init_VM will initialize the manager and make it ready for execution while this will start doing everything
    if (_asp_create_detached_thread(boss->core_threads[0], _asp_start_execution, boss->cpu[0]) == aFalse)
        return (void*)-1;
    while (aTrue)
    {
        _asp_mutex_lock(boss->lock);
        if (boss->stop_vm == aFalse)
        {
            _asp_cond_wait(boss->cond, boss->lock);
        }
        else
        {
            // we can now stop the VM
            for (aSize_t i = 0; i < boss->core_count; i++)
            {
                _asp_mutex_lock(boss->cpu[i]->lock);
                boss->cpu[i]->running = aFalse; // tell the thread or the core to stop further execution
                _asp_mutex_unlock(boss->cpu[i]->lock);
            }
            _asp_mutex_unlock(boss->lock);
            break;
        }
        _asp_mutex_unlock(boss->lock);
    }
    // when stop_vm is set, the manager commands all the other cores(threads) to exit which terminates the VM
    // the manager thread also terminates and the control is transferred to the main thread which
    // the main thread can then clean things up
    return (void *)boss->return_val; // we need to return if anything went wrong or right
}

void _asp_manager_error(_Asp_Manager *manager, acStr_t err_msg)
{
    _asp_mutex_lock(manager->lock);
    // we simply display the error to the standard error and tell the manager to exit
    fprintf(stderr, "%s\n", err_msg);
    manager->stop_vm = aTrue;
    manager->return_val = -1;
    _asp_cond_signal(manager->cond); // wake up the manager
    _asp_mutex_unlock(manager->lock);
}

void _asp_manager_add_core(_Asp_Manager *manager, aQword core_id)
{
    // this will add a core, boot it up and make it start executing code
    // if the process fails at any step, the manager exits
    _asp_mutex_lock(manager->lock);
    _Asp_Thread **tempThreads = manager->core_threads;
    _Asp_CPU **tempCPU = manager->cpu;
    manager->core_threads = (_Asp_Thread **)realloc(manager->core_threads, sizeof(_Asp_Thread *) * (manager->core_count + 1)); // attempt to get one more core
    if (manager->core_threads == NULL)
        goto error;
    else
    {
        // we successfully added a core
        manager->cpu = (_Asp_CPU **)realloc(manager->cpu, sizeof(_Asp_CPU *) * (manager->core_count + 1));
        if (manager->cpu == NULL)
            goto error;
        else
        {
            // now we have to start that core
            // As per the definition, the Ax1 register of the requesting core should contain the starting address for this new core
            if (_asp_manager_boot_core(manager, manager->core_count, manager->cpu[core_id]->_asp_registers[Ax1]) == aFalse)
            {
                fprintf(stderr, "Internal Error: Failed to boot up core\n");
                manager->stop_vm = aTrue;
                manager->return_val = -1;
                _asp_cond_signal(manager->cond);
            }
            else
            {
                // we successfully did everything
                // we just need to start that core's thread
                if (_asp_create_detached_thread(manager->core_threads[manager->core_count], _asp_start_execution, manager->cpu[manager->core_count]) == aFalse)
                {
                    // if we fail that
                    fprintf(stderr, "Internal Error: Core created, couldn't start.\n");
                    manager->stop_vm = aTrue;
                    manager->return_val = -1;
                    _asp_cond_signal(manager->cond);
                }
                else
                {
                    manager->core_count++;
                }
            }
        }
    }
    manager->cpu[core_id]->_asp_registers[Aa] = 0; // indicates there was no error
    _asp_mutex_unlock(manager->lock);
error:
    fprintf(stderr, "Internal Error: Failed to boot up core\n");
    manager->stop_vm = aTrue;
    manager->return_val = -1;
    manager->core_threads = tempThreads; // we maynot want to completely stop execution and continue but how to do it is unknown to me as well
    manager->cpu = tempCPU;
    _asp_cond_signal(manager->cond);
    manager->cpu[core_id]->_asp_registers[Aa] = 1; // indicates there was an error
    _asp_mutex_unlock(manager->lock);
}

void _asp_manager_exit(_Asp_Manager *manager, aQword core_id)
{
    _asp_mutex_lock(manager->lock);
    manager->return_val = manager->cpu[core_id]->_asp_registers[Aa]; // return value in the Aa register
    manager->stop_vm = aTrue;
    _asp_cond_signal(manager->cond);
    _asp_mutex_unlock(manager->lock);
    _asp_InBuf_destroy();
}

void _asp_manager_IO_readChar(_Asp_Manager *manager, aQword core_id)
{
    // reads a character from the user
    _asp_mutex_lock(manager->lock);
    aByte _in;
    if (_asp_In_readChar(&_in) == aFalse)
    {
        manager->cpu[core_id]->running = aFalse; // stop for good purposes
        _asp_mutex_unlock(manager->lock);
        _asp_manager_error(manager, "IO error: Unable or failed to read a byte");
        return;
    }
    // the read was successful
    if (_asp_memory_write(manager->memory, manager->cpu[core_id]->_asp_registers[Aa], _in) == aFalse) // we are wasting 7 bytes here[WHY?!!!]
    {
        manager->cpu[core_id]->running = aFalse;
        _asp_mutex_unlock(manager->lock);
        _asp_manager_error(manager, "Internal Error: Failed to access memory"); // the error could have been anything and yet we are saying its the VM's fault
        return;
    }
    _asp_mutex_unlock(manager->lock); // everything worked
}

void _asp_manager_IO_readString(_Asp_Manager *manager, aQword core_id)
{
    _asp_mutex_lock(manager->lock);
    register aSize_t len = manager->cpu[core_id]->_asp_registers[Ab];
    aByte _in[len]; // the length of the string in Ab[length includes the space for the terminating NULL character as well]
    if (_asp_In_readStr(_in, len) == aFalse)
    {
        manager->cpu[core_id]->running = aFalse; // stop for good purposes
        _asp_mutex_unlock(manager->lock);
        _asp_manager_error(manager, "IO error: Unable or failed to read any byte");
        return;
    }
    if (_asp_memory_write_bytes(manager->memory, manager->cpu[core_id]->_asp_registers[Aa], _in, len) == aFalse)
    {
        // this is an error
        manager->cpu[core_id]->running = aFalse;
        _asp_mutex_unlock(manager->lock);
        _asp_manager_error(manager, "Internal Error: Failed to access memory"); // the error could have been anything and yet we are saying its the VM's fault
        return;
    }
    _asp_mutex_unlock(manager->lock); // everything worked
}

void _asp_manager_IO_writeChar(_Asp_Manager *manager, aQword core_id)
{
    _asp_mutex_lock(manager->lock);
    aByte val = manager->cpu[core_id]->_asp_registers[Aa] & 255;
    if (_asp_write_byte(&val) != 1)
    {
        // there has been an error
        fprintf(stderr, "IO error: Unable to print a byte.\n");
        manager->cpu[core_id]->running = aFalse;
        manager->stop_vm = aTrue;
        manager->return_val = -1;
        _asp_cond_signal(manager->cond);
    }
    _asp_mutex_unlock(manager->lock); // everything may have gone smoothly
}

void _asp_manager_IO_writeString(_Asp_Manager *manager, aQword core_id)
{
    _asp_mutex_lock(manager->lock);
    register aSize_t len = manager->cpu[core_id]->_asp_registers[Ab];
    aByte _bytes[len];
    if (_asp_memory_read_bytes(manager->memory, manager->cpu[core_id]->_asp_registers[Aa], _bytes, len) == aFalse)
    {
        // there has been an error
        manager->cpu[core_id]->running = aFalse;
        fprintf(stderr, "IO error: Unable to print bytes.\n");
        manager->stop_vm = aTrue;
        manager->return_val = -1;
        _asp_cond_signal(manager->cond);
    }
    else
    {
        // we now print it
        if (_asp_write_string(_bytes) != 0)
        {
            manager->cpu[core_id]->running = aFalse;
            fprintf(stderr, "IO error: Unable to print bytes.\n");
            manager->stop_vm = aTrue;
            manager->return_val = -1;
            _asp_cond_signal(manager->cond);
        }
    }
    _asp_mutex_unlock(manager->lock); // everything may have gone good
}