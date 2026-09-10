RwSectionLock::~RwSectionLock() { pthread_rwlock_destroy(&m_rwlock); }
