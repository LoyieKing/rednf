#include <string.h>

#include "ThreadLock.h"

ThreadLock_linux::ThreadLock_linux()
{
    pthread_mutex_t h = {0};
    h.__data.__kind = 2;
    handle_ = h;
    pthread_mutex_init(&handle_, NULL);
}

ThreadLock_linux::~ThreadLock_linux()
{
    pthread_mutex_destroy(&handle_);
}

void ThreadLock_linux::_lock()
{
    int nErrorCode = pthread_mutex_lock(&handle_);
}

// --- Functions defined in this compilation unit ---

// ThreadLock_linux::_lock()
/* TODO: implement */

// ThreadLock_linux::ThreadLock_linux()
/* TODO: implement */

// ThreadLock_linux::ThreadLock_linux()
/* TODO: implement */

// ThreadLock_linux::~ThreadLock_linux()
/* TODO: implement */

// ThreadLock_linux::~ThreadLock_linux()
/* TODO: implement */
