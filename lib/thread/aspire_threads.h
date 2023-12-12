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

#ifndef __ASP_THREADS_
#define __ASP_THREADS_

// This module abstracts the multi-threading for _Asp_try
// For now _Asp_try only runs on AMD64 Ryzen 5 with Arch linux
// but in the future(if and only if) we may add the ability to run _Asp_try on other devices with other OSes
// In this setup, pthread.h is the OS provided API for multi-threading which may be different for other setups
// This module abstracts all of that

#include "../../Utils/aspire_config.h"
#include "../../Utils/aspire_typedefs.h"
#include "../../Utils/aspire_helpers.h"
#include <stdlib.h>

typedef struct _Asp_Mutex _Asp_Mutex;
typedef struct _Asp_Cond _Asp_Cond;
typedef struct _Asp_Thread _Asp_Thread;
typedef struct _Asp_Semaphore _Asp_Semaphore;

_ASP_DEFINE_FUNC_PTR(void *, aVoidFunc, void *);

#if defined(_ASP_HOST_LINUX)
#include "aspire_posix_threads.h" // we may add other headers only available to unix based systems
#define _ASP_POSIX_THREADS 1
#endif

_Asp_Thread *_asp_thread_init(); // create a new _Asp_Thread object[doesn't create a thread]

_Asp_Mutex *_asp_mutex_init();                        // initialize a mutex lock
_Asp_Cond *_asp_cond_init();                          // initialize a condition variable
_Asp_Semaphore *_asp_semaphore_init(aSize_t sem_val); // create a semaphore

// mutexes and condition variables are not supposed to be pointers but they can be used as pointers
void _asp_mutex_destroy(_Asp_Mutex *mutex); // destroy a mutex
void _asp_cond_destroy(_Asp_Cond *cond);    // destroy a condition variable

void _asp_thread_destroy(_Asp_Thread *thread);    // destroy a _Asp_Thread object
void _asp_semaphore_destroy(_Asp_Semaphore *sem); // destroy a semaphore

// initialize thread using _asp_thread_destroy but start a thread like this
aBool _asp_create_thread(_Asp_Thread *thread, aVoidFunc func, void *arg); // default thread
void _asp_thread_join(_Asp_Thread *thread, void *retval);                   // join the thread

// this creates a detached thread that should not use _asp_thread_join but the code that the thread executes should have
// a way for the thread to exit
aBool _asp_create_detached_thread(_Asp_Thread *thread, aVoidFunc func, void *arg); // create a thread that is detached

void _asp_mutex_lock(_Asp_Mutex *mutex);   // mutex lock
void _asp_mutex_unlock(_Asp_Mutex *mutex); // mutex unlock
void _asp_cond_wait(_Asp_Cond *cond, _Asp_Mutex *mutex);
void _asp_cond_signal(_Asp_Cond *cond);

void _asp_sem_wait(_Asp_Semaphore *sem); // wait
void _asp_sem_post(_Asp_Semaphore *sem); // signal

#endif