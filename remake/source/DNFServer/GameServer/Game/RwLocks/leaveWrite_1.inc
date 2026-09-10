void RwSectionLock::leaveWrite() { pthread_rwlock_unlock(&m_rwlock); }
