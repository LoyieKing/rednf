EventProcessManager::EventProcessManager()
    : m_impl(new EventProcessManagerImpl)
{
    setupEventProcessors();
