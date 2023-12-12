#include "../../../lib/thread/aspire_threads.h"

// create a new _Asp_Mutex variable
_Asp_Mutex *_asp_mutex_init()
{
    _Asp_Mutex *mutex = (_Asp_Mutex *)malloc(sizeof(_Asp_Mutex));
    if (mutex == NULL)
        return NULL; // we failed
#if defined(_ASP_POSIX_THREADS)
    // for Unix based systems like Linux
    if (pthread_mutex_init(&mutex->mutex, NULL) != 0) // we use default attributes
    {
        free(mutex);
        return NULL;
    }
#endif
    // .. more for other systems
    return mutex; // return
}

// create a new _Asp_Cond variable
_Asp_Cond *_asp_cond_init()
{
    _Asp_Cond *cond = (_Asp_Cond *)malloc(sizeof(_Asp_Cond));
    if (cond == NULL)
        return NULL; //  we failed
#if defined(_ASP_POSIX_THREADS)
    // for unix based systems
    if (pthread_cond_init(&cond->cond, NULL) != 0)
    {
        free(cond);
        return NULL;
    }
#endif
    // .. for other systems
    return cond; // return
}

_Asp_Semaphore *_asp_semaphore_init(aSize_t sem_val)
{
    _Asp_Semaphore *sem = (_Asp_Semaphore *)malloc(sizeof(_Asp_Semaphore));
    if (sem == NULL)
        return NULL;
    sem->sempahore = sem_val > 0 ? sem_val : 1;
#if defined(_ASP_POSIX_THREADS)
    if ((sem->cond = _asp_cond_init()) == NULL)
    {
        free(sem);
        return NULL;
    }
#endif
#if defined(_ASP_POSIX_THREADS)
    if ((sem->mutex = _asp_mutex_init()) == NULL)
    {
        _asp_cond_destroy(sem->cond);
        free(sem);
        return NULL;
    }
#endif
    return sem;
}

void _asp_mutex_destroy(_Asp_Mutex *mutex)
{
    if (mutex == NULL)
        return;
#if defined(_ASP_POSIX_THREADS)
    pthread_mutex_destroy(&(mutex)->mutex);
#endif
    free(mutex);
}

void _asp_cond_destroy(_Asp_Cond *cond)
{
    if (cond == NULL)
        return;
#if defined(_ASP_POSIX_THREADS)
    pthread_cond_destroy(&(cond)->cond);
#endif
    free(cond);
}

void _asp_thread_destroy(_Asp_Thread *thread)
{
    if (thread == NULL)
        return;
    free(thread);
}

void _asp_semaphore_destroy(_Asp_Semaphore *sem)
{
    if (sem == NULL)
        return;
    _asp_mutex_destroy((sem)->mutex);
    _asp_cond_destroy((sem)->cond);
    free(sem);
}

aBool _asp_create_thread(_Asp_Thread *thread, aVoidFunc func, void *arg)
{
    // create a thread with default behaviour
    if (thread == NULL)
        return aFalse;
#if defined(_ASP_POSIX_THREADS)
    if (pthread_create(&(thread)->thread, NULL, func, arg) != 0)
        return aFalse;
#endif
    return aTrue;
}

void _asp_thread_join(_Asp_Thread *thread, void *retval)
{
#if defined(_ASP_POSIX_THREADS)
    pthread_join((thread)->thread, &retval);
#endif
}

// we may never need this function 'cause we don't need detached threads
aBool _asp_create_detached_thread(_Asp_Thread *thread, aVoidFunc func, void *arg)
{
    if (thread == NULL)
        return aFalse;
#if defined(_ASP_POSIX_THREADS)
    pthread_attr_t attr;
    if (pthread_attr_init(&attr) != 0)
        return aFalse;
    if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0)
    {
        pthread_attr_destroy(&attr);
        return aFalse;
    }
    if (pthread_create(&(thread)->thread, &attr, func, arg) != 0)
    {
        pthread_attr_destroy(&attr);
        return aFalse;
    }
    pthread_attr_destroy(&attr);
#endif
    return aTrue;
}

void _asp_mutex_lock(_Asp_Mutex *mutex)
{
#if defined(_ASP_POSIX_THREADS)
    pthread_mutex_lock(&(mutex)->mutex);
#endif
}

void _asp_mutex_unlock(_Asp_Mutex *mutex)
{
#if defined(_ASP_POSIX_THREADS)
    pthread_mutex_unlock(&(mutex)->mutex);
#endif
}

void _asp_cond_wait(_Asp_Cond *cond, _Asp_Mutex *mutex)
{
#if defined(_ASP_POSIX_THREADS)
    pthread_cond_wait(&(cond)->cond, &(mutex)->mutex);
#endif
}

void _asp_cond_signal(_Asp_Cond *cond)
{
#if defined(_ASP_POSIX_THREADS)
    pthread_cond_signal(&(cond)->cond);
#endif
}

void _asp_sem_wait(_Asp_Semaphore *sem)
{
    _asp_mutex_lock((sem)->mutex);
    while ((sem)->sempahore == 0)
    {
        _asp_cond_wait((sem)->cond, (sem)->mutex);
    }
    (sem)->sempahore--;
    _asp_mutex_unlock((sem)->mutex);
}

void _asp_sem_post(_Asp_Semaphore *sem)
{
    _asp_mutex_lock((sem)->mutex);
    (sem)->sempahore++;
    _asp_cond_signal((sem)->cond);
    _asp_mutex_unlock((sem)->mutex);
}

_Asp_Thread *_asp_thread_init()
{
    _Asp_Thread *thread = (_Asp_Thread *)malloc(sizeof(_Asp_Thread));
    if (thread == NULL)
        return NULL;
    // we are simply initializing the object and not starting a new thread
    return thread;
}
