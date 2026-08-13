#ifndef NSL_THREADLOCK_INL_
#define NSL_THREADLOCK_INL_

namespace nsl {

inline void ThreadLock_linux::lock()
{
    pthread_mutex_lock(&handle_);
}

inline void ThreadLock_linux::unlock()
{
    pthread_mutex_unlock(&handle_);
}

} // namespace nsl

#endif // NSL_THREADLOCK_INL_
