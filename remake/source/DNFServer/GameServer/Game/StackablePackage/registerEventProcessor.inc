void EventProcessManager::registerEventProcessor(EventProcessor* processor)
{
    m_impl->processors[processor->GetEventType()].insert(
        std::pair<int const, EventProcessor*>(processor->GetEventId(), processor));
