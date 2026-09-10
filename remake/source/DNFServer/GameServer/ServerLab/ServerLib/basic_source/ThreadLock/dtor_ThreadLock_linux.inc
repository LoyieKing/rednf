ThreadLock_linux::~ThreadLock_linux()
{
    pthread_mutex_destroy(&handle_);
