bool SpinLock::tryEnter() { return __sync_lock_test_and_set(&m_lock, 1) == 0; }
