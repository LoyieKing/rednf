void SpinLock::leave()
{
    __sync_lock_test_and_set(&m_lock, 0);
