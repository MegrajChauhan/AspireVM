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

#ifndef _ASP_POSIX_THREADS_
#define _ASP_POSIX_THREADS_

// This is for the systems that use posix threads or pthreads.h[systems like unix, linux and ios]

#include <pthread.h>
#include "../../Utils/aspire_typedefs.h"

// the mutex
struct _Asp_Mutex
{
    pthread_mutex_t mutex;
};

// the condition variables
struct _Asp_Cond
{
    pthread_cond_t cond;
};

// semaphores
struct _Asp_Semaphore
{
    aSize_t sempahore;
    struct _Asp_Mutex *mutex;
    struct _Asp_Cond *cond;
};

// the thread
struct _Asp_Thread
{
    pthread_t thread;
};

#endif