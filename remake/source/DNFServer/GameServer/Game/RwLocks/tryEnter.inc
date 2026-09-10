bool SectionLock::tryEnter() { return pthread_mutex_trylock(&m_mutex) != 0; }
