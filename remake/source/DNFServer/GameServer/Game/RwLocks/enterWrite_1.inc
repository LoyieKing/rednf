void RwSectionLock::enterWrite() { pthread_rwlock_wrlock(&m_rwlock); }
