void RwSectionLock::leaveRead() { pthread_rwlock_unlock(&m_rwlock); }
