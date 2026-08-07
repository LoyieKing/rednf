#include <string.h>

#include "ThreadLock.h"

namespace nsl {

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

} // namespace nsl
