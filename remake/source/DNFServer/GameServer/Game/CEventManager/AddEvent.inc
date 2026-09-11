int CEventManager::AddEvent(int type, CEventBase* event)
{
    m_events[type] = event;
    return 1;
}