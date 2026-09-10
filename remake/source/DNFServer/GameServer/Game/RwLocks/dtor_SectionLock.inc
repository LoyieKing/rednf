SectionLock::~SectionLock() { pthread_mutex_destroy(&m_mutex); }
