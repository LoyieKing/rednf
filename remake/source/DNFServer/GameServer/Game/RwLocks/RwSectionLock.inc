// ---- RwSectionLock（ORIG T 0x08ad37f8..0x08ad389b；报告缺失，objdump 转写）----
RwSectionLock::RwSectionLock() : Uncopyable()
{
    pthread_rwlockattr_t attr;
    pthread_rwlockattr_init(&attr);
    pthread_rwlock_init(&m_rwlock, &attr);
    pthread_rwlockattr_destroy(&attr);
}