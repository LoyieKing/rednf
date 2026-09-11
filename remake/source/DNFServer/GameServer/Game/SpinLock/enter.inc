void SpinLock::enter()
{
    while (__sync_lock_test_and_set(&m_lock, 1) == 1)
        YieldProcessor();
}