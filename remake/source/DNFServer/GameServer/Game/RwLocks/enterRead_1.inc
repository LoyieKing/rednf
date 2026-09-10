void RwSectionLock::enterRead() { pthread_rwlock_rdlock(&m_rwlock); }
