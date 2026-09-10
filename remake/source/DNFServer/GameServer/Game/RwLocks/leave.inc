void SectionLock::leave() { pthread_mutex_unlock(&m_mutex); }
