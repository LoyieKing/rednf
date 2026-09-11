// ---- SectionLock（ORIG T 0x08ad3742..0x08ad37f7）----
SectionLock::SectionLock() : Uncopyable()
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m_mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}