RwSectionLock::RwSectionLock() : Uncopyable()
{
    pthread_rwlockattr_t attr;
    pthread_rwlockattr_init(&attr);
    pthread_rwlock_init(&m_rwlock, &attr);
    pthread_rwlockattr_destroy(&attr);
